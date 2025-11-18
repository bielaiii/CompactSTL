#ifndef FUNCTIONER_HEADER
#define FUNCTIONER_HEADER

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <functional>
#include <type_traits>
#include <utility>
#include<iostream>
using namespace std;
namespace CompactSTL {

static bool store_locally;
union aliasing_ {
    void *_[4];
};

enum op { clone_, init_, destory_, invoke_ };
union Stored_buf {
    [[nodiscard]] void *access() noexcept { return &buffer[0]; };
    [[nodiscard]] const void *access() const noexcept { return &buffer[0]; };

    template <typename T> [[nodiscard]] T &access_ref() noexcept {
        return *static_cast<T *>(access());
    };

    template <typename T> [[nodiscard]] const T &access_ref() const noexcept {
        return *static_cast<T *>(access());
    };

    aliasing_
        as__; // 保证内存对齐，alignas + 模板，要同时给union
              // 和buffer作用的方式得声明两次
              // 直接声明一个用作对齐的占位符更加方便，这样可以保证union和buffer此时一定都是8字节对齐
    char buffer[sizeof(aliasing_)];
};

template <typename Ret, typename Obj, typename... Args> struct hard_code_type {
    using simple_obj                    = std::decay_t<Obj>;
    constexpr static bool store_locally = sizeof(simple_obj) < sizeof(void *) * 4;

    template <typename func> static Ret operator()(func *ptr, Args &&...args) {
        if constexpr (std::is_same_v<Ret, void>) {
            std::invoke(ptr, std::forward<Args>(args)...);
        } else {
            return std::invoke(ptr, std::forward<Args>(args)...);
        }
    }

    static Ret operator()(Stored_buf &where, Args... args) {
        Obj *obj = where.access_ref<Obj *>();
        if constexpr (std::is_same_v<Ret, void>) {
            std::invoke(*obj, std::forward<Args>(args)...);
        } else {
            return std::invoke(*obj, std::forward<Args>(args)...);
        }
    }

    static void cleanup(Stored_buf &dst, Stored_buf &src) {
        using ty = std::decay_t<Obj>;
        if constexpr (!store_locally) {
            delete dst.access_ref<ty *>();
        } else if constexpr (!std::is_trivially_destructible_v<ty>) {
            dst.access_ref<ty>().~ty();
        }
    }

    static void copy_obj(Stored_buf &dst, Stored_buf &src) {
        // static assert guarantee that union copy is always trivially copy
        // constructible
        static_assert(std::is_trivially_copy_constructible_v<Stored_buf>,
                      "compiler stop");
        auto dst_ = static_cast<simple_obj *>(dst.access());
        auto src_ = static_cast<simple_obj *>(src.access());
        static_assert(std::is_pointer_v<decltype(dst_)>, "should not print");
        if constexpr (store_locally) {
            if constexpr (std::is_trivially_copy_constructible_v<Obj>) {
                std::memcpy(dst_, src_, sizeof(simple_obj));
            } else {
                ::new (dst_) Obj(*src_);
            }
        } else {
            dst.access_ref<simple_obj *>() = new simple_obj(*src_);
        }
    }

    static int init(Stored_buf &dst, Obj &&obj) {
        using ty = std::decay_t<Obj>;
        if constexpr (!store_locally) {
            dst.access_ref<ty *>() = new ty(std::forward<Obj>(obj));
        } else {
            ::new (dst.access()) ty(std::forward<Obj>(obj));
        }
        return 1;
    }

    static int manager_op(Stored_buf &dst, Stored_buf &src, op op_) {
        switch (op_) {
        case clone_: copy_obj(dst, src); break;
        case destory_: cleanup(dst, src); break;
        default: break;
        }
        return 0;
    }
    static Ret call_help(Stored_buf &bu, Args... args) {
        if constexpr (store_locally) {
            auto &obj = bu.access_ref<simple_obj>();
            if constexpr (std::is_same_v<Ret, void>) {
                std::invoke(obj, std::forward<Args>(args)...);
            } else {
                return std::invoke(obj, std::forward<Args>(args)...);
            }
        } else {
            auto obj = bu.access_ref<simple_obj *>();
            if constexpr (std::is_same_v<Ret, void>) {
                std::invoke(*obj, std::forward<Args>(args)...);
            } else {
                return std::invoke(*obj, std::forward<Args>(args)...);
            }
        }
    }
};

template <typename sig> class functional;

template <typename Ret, typename... Args> class functional<Ret(Args...)> {
private:
    using no_type_invoker = Ret (*)(Stored_buf &, Args...);
    using manager         = int (*)(Stored_buf &, Stored_buf &, op);
    no_type_invoker invoker_{};
    manager manager_{};
    Stored_buf stored_buf{};

public:
    functional() = default;

    template <typename Call_>
    functional(Call_ &&call_) noexcept
#if __cplusplus >= 202003L
        requires std::is_invocable_v<Call_, Args...>
#else
        
        typename = std::enable_if_t<std::is_invocable_v<Callable, Args...>>
#endif
    {
        using had = hard_code_type<Ret, Call_, Args...>;
        if (had::init(stored_buf, std::forward<Call_>(call_))) {
            invoker_ = &had::call_help;
            manager_ = &had::manager_op;
        }
    }

    functional(functional &other) noexcept {
        other.manager_(stored_buf, other.stored_buf, clone_);
        manager_ = other.manager_;
        invoker_ = other.invoker_;
    }

    functional(functional &&other) noexcept {
        other.manager_(stored_buf, other.stored_buf, clone_);
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

    operator bool() { return manager_ != nullptr; }

    void swap(functional &other) noexcept {
        std::swap(other.invoker_, this->invoker_);
        std::swap(other.manager_, this->manager_);
        std::swap(other.stored_buf, this->stored_buf);
    }

    Ret operator()(Args... args) {
        if (invoker_ != nullptr) {
            if constexpr (std::is_same_v<Ret, void>) {
                invoker_(stored_buf, std::forward<Args>(args)...);
            } else {
                return invoker_(stored_buf, std::forward<Args>(args)...);
            }
        } else {
            throw std::bad_function_call{};
        }
    }

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
