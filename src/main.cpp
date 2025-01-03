#include <iostream>
#include <coroutine>

template <typename T>
struct Generator {
    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always yield_value(T value) noexcept {
            current_value = value;
            return {};
        }

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type handle;

    explicit Generator(handle_type h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }

    T next() {
        handle.resume();
        return handle.promise().current_value;
    }

    bool done() { return handle.done(); }
};

Generator<int> generateNumbers() {
    for (int i = 1; i <= 5; ++i) {
        co_yield i;
    }
}

int main() {
    auto gen = generateNumbers();
    while (!gen.done()) {
        std::cout << "Generated: " << gen.next() << std::endl;
    }
    return 0;
}
