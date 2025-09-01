#ifndef FUNCTIONER_HEADER
#define FUNCTIONER_HEADER

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <functional>
#include <type_traits>
#include <utility>
namespace CompactSTL {

#define LOCAL_CALLABLE_OBJECT 16

struct destory_op {};
struct clone_op {};
struct init_op {};
struct invoke_op {};

template <typename Ret, typename... Args> struct callable_object {
public:
    enum op { clone_, init_, destory_, invoke_ };

    template <typename T> int manager(op op_) {
        switch (op_) {
        case init_:

        case clone_:

        case destory_:
            cleanup<T>();
            // case invoke_:
        }
        return 0;
    }
    void process(init_op) {}

    template <typename T> void clone_func() {}

    template <typename T> Ret invokeResult() {}
};

static bool store_locally;
union aliasing_ {
    void *_[4];
};

enum op { clone_, init_, destory_, invoke_ };
union buf {
    [[nodiscard]] void *access() noexcept { return &buffer[0]; };
    [[nodiscard]] const void *access() const noexcept { return &buffer[0]; };

    template <typename T> [[nodiscard]] T &access() noexcept {
        return *static_cast<T *>(access());
    };

    template <typename T> [[nodiscard]] const T &access() const noexcept {
        return *static_cast<T *>(access());
    };

    aliasing_
        as__; // 保证内存对齐，alignas + 模板，要同时给union
              // 和buffer作用的方式得声明两次
              // 直接声明一个用作对齐的占位符更加方便，这样可以保证union和buffer此时一定都是8字节对齐
    char buffer[LOCAL_CALLABLE_OBJECT];
} bu;

template <typename Ret, typename Obj, typename... Args> struct hard_code_type {
    constexpr static bool store_locally = sizeof(Obj) < sizeof(void *) * 4;
    using simple_obj                    = std::decay_t<Obj>;

    template <typename func> static Ret operator()(func *ptr, Args &&...args) {
        if constexpr (std::is_same_v<Ret, void>) {
            std::invoke(ptr, std::forward<Args>(args)...);
        } else {
            return std::invoke(ptr, std::forward<Args>(args)...);
        }
    }

    static Ret operator()(buf &where, Args... args) {
        Obj *obj = where.access<Obj *>();
        ;
        if constexpr (std::is_same_v<Ret, void>) {
            std::invoke(*obj, std::forward<Args>(args)...);
        } else {
            return std::invoke(*obj, std::forward<Args>(args)...);
        }
    }

    static void cleanup(buf &dst, buf &src) {
        using ty = std::decay_t<Obj>;
        if constexpr (!store_locally) {
            delete dst.access<ty *>();
        } else if constexpr (!std::is_trivially_destructible_v<ty>) {
            dst.access<ty>().~ty();
        }
    }

    static void copy_obj(buf &dst, buf &src) {
        auto dst_ = dst.access<simple_obj>();
        auto src_ = src.access<simple_obj>();
        if constexpr (std::is_trivially_copy_constructible_v<Obj>) {
            std::memcpy(&dst, &src, sizeof(Obj));
        } else {
            dst = new Obj(*src_);
        }
    }
    static void process(buf &dst, buf &src, init_op) {}

    static int init(buf &dst, Obj &&obj) {
        using ty = std::decay_t<Obj>;
        if constexpr (!store_locally) {
            dst.access<ty*>() = new ty(std::forward<Obj>(obj));
        } else {
            ::new (dst.access()) ty(std::forward<Obj>(obj));
        }
        return 1;
    }

    static int manager_op(buf &dst, buf &src, op op_) {
        switch (op_) {
        case clone_: copy_obj(dst, src); break;
        case destory_: cleanup(dst, src); break;
        // case invoke_: call_help(dst, src); break;
        default: break;
        }
        return 0;
    }
    static Ret call_help(buf &bu, Args... args) {
        if constexpr (store_locally) {
            auto &obj = bu.access<simple_obj>();
            if constexpr (std::is_same_v<Ret, void>) {
                std::invoke(obj, std::forward<Args>(args)...);
            } else {
                return std::invoke(obj, std::forward<Args>(args)...);
            }
        } else {
            auto obj = bu.access<simple_obj *>();
            if constexpr (std::is_same_v<Ret, void>) {
                std::invoke(*obj, std::forward<Args>(args)...);
            } else {
                return std::invoke(*obj, std::forward<Args>(args)...);
            }
        }
    }
};

template <typename sig> class functional;

template <typename Ret, typename... Args>
class functional<Ret(Args...)> : private callable_object<Ret, Args...> {
private:
    using no_type_invoker = Ret (*)(buf &, Args...);
    using manager         = int (*)(buf &, buf &, op);
    no_type_invoker invoker_{};
    manager manager_{};
    buf stored_buf{};

public:
    functional() = default;
    // functional(std::nullptr_t) = default;

    template <typename Call_>
    functional(Call_ &&call_) noexcept
#if __cplusplus >= 202003L
        requires std::is_invocable_v<Call_, Args...>
#else
        ,
        typename = std::enable_if_t<std::is_invocable_v<Callable, Args...>>
#endif
    {
        using had = hard_code_type<Ret, Call_, Args...>;
        if (had::init(stored_buf, std::forward<Call_>(call_))) {
            invoker_ = &had::call_help;
            manager_ = &had::manager_op;
        }
    }

    /*     template<typename Call_>
        functional(Call_ &&call_) noexcept {
            using had = hard_code_type<Ret, Call_, Args...>;
            if (had::process(forward<Call_>(call_))) {
                invoker_ = &had::call_help;
                manager_ = &had::manager_op;
            }
        } */

    functional(functional &other) noexcept {
        stored_buf = other.stored_buf;
        manager_   = other.manager_;
        invoker_   = other.invoker_;
    }

    functional(functional &&other) noexcept {

        stored_buf       = other.stored_buf;
        manager_         = other.manager_;
        invoker_         = other.invoker_;
        other.stored_buf = {};
        other.manager_   = nullptr;
        other.invoker_   = nullptr;
    }
    functional &operator()(functional f) noexcept {
        swap(f, this);
        return *this;
    }

    void swap(functional &other) noexcept {
        std::swap(other.invoker_, this->invoker_);
        std::swap(other.manager_, this->manager_);
        std::swap(other.stored_buf, this->stored_buf);
    }

    Ret operator()(Args... args) {
        if (invoker_) {
            if constexpr (std::is_same_v<Ret, void>) {
                invoker_(stored_buf, std::forward<Args>(args)...);
            } else {
                return invoker_(stored_buf, std::forward<Args>(args)...);
            }
        }
    }

    // Ret operator()(Args &&...args) {}
    ~functional() {
        if (manager_) {
            manager_(stored_buf, stored_buf, destory_);
        }
    };
};

/* for compiler to deduce signature for any callable object or pointer */

template <typename> struct func_signature_deducing_helper;

template <typename Ret, typename... Args>
struct func_signature_deducing_helper<Ret (*)(Args...)> {
    using type = Ret(Args...);
};

template <typename Ret, typename struct_, bool has_except_, typename... Args>
struct func_signature_deducing_helper<Ret (struct_::*)(Args...) noexcept(
    has_except_)> {
    using type = Ret(Args...);
};

template <typename Ret, typename struct_, bool has_except_, typename... Args>
struct func_signature_deducing_helper<Ret (struct_::*)(Args...)
                                          const noexcept(has_except_)> {
    using type = Ret(Args...);
};

template <typename Ret, typename struct_, bool has_except_, typename... Args>
struct func_signature_deducing_helper<Ret (struct_::*)(Args...) const &
                                      noexcept(has_except_)> {
    using type = Ret(Args...);
};

template <typename Ret, typename struct_, bool has_except_, typename... Args>
struct func_signature_deducing_helper<Ret (struct_::*)(
    struct_ *, Args...) noexcept(has_except_)> {
    using type = Ret(Args...);
};

template <typename sig>
using func_signature_deducing_helper_t =
    typename func_signature_deducing_helper<sig>::type;

template <typename Ret, typename... Args> struct simple_ptr {
    using type = Ret (*)(Args...);
};

template <typename Ret, typename... Args>
using simple_ptr_t = simple_ptr<Ret, Args...>;

template <typename T, bool = std::is_pointer_v<std::decay_t<T>>>
struct template_select;

template <typename T> struct template_select<T, false> : std::false_type {};

template <typename T> struct template_select<T, true> : std::true_type {};

template <typename Func, typename ops, typename = void>
using function_deducing_template_t =
    std::conditional_t<template_select<std::decay_t<Func>>::value,
                       simple_ptr_t<ops>,
                       func_signature_deducing_helper_t<ops>>;

template <typename T, typename sig = function_deducing_template_t<
                          T, decltype(&T ::operator())>>
functional(T) -> functional<sig>;
} // namespace CompactSTL
#endif
