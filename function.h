//
// Created by jesus on 19.01.19.
//

#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <cassert>
#include <functional>
#include <iostream>
#include <new>
#include <memory>
#include <utility>
#include <array>
#include <variant>
#include <any>

template<typename Signature>
struct function;

template<typename T, typename ...Args>
struct function<T(Args...)> {

    function() noexcept : type(-1), big(nullptr) {}

    function(nullptr_t) noexcept : type(-1), big(nullptr) {}

    function(function const &other) : type(other.type) {
        if (type == 0) {
            other.getImplementation()->copy(small);
        } else {
            big = other.big->copy();
        }
    }

    function(function &&other) : type(-1), big(nullptr) {
        swap(other);
        other.~function();
    }

    template<typename F>
    function(F function) {
        if (sizeof(F) <= smallSize) {
            type = 0;
            new(small)Implementation<F>(std::move(function));
        } else {
            type = 1;
            big = std::make_unique<Implementation < F>>
            (std::move(function));
        }
    }

    function &operator=(function const &other) {
        function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) noexcept {
        function(std::move(other)).swap(*this);
        return *this;
    }

    explicit operator bool() const noexcept {
        if (big == nullptr) {
            return true;
        }
        return false;
    }

    ~function() {
        if (type == 0) {
            getImplementation()->~Base();
        } else {
            big.reset();
        }
    }

    T operator()(Args &&...args) const {
        if (type == -1) {
            throw std::bad_function_call();
        }
        if (type == 0) {
            return reinterpret_cast<Base *>(const_cast<char *>(small))->runFunction(std::forward<Args>(args)...);
//            return getImplementation()->runFunction(std::forward<Args>(args)...);
        } else {
            return big->runFunction(std::forward<Args>(args)...);
        }
    }

    void swap(function &other) noexcept {
        std::swap(type, other.type);
        std::swap(small, other.small);
    }

private:
    struct Base;

    template<typename F>
    struct Implementation;


    char type;
    static const int smallSize = 96;

    union {
        std::unique_ptr<Base> big;
        char small[smallSize];
    };


    struct Base {
    public:
        Base() = default;

        virtual ~Base() = default;

        virtual T runFunction(Args &&...args) = 0;

        virtual void copy(void *where) const = 0;

        virtual std::unique_ptr<Base> copy() const = 0;
    };


    template<typename F>
    struct Implementation : Base {

        explicit Implementation(F const &function) : f(function) {}

        virtual T runFunction(Args &&...args) override final {
            return f(std::forward<Args>(args)...);
        }

        void copy(void *where) const override final { new(where) Implementation<F>(*this); }

        std::unique_ptr<Base> copy() const override final {
            return std::make_unique<Implementation<F>>(f);
        }

        F f;
    };

    const Base *getImplementation() const {
        return reinterpret_cast<const Base *>(static_cast<const void *>(small));
    }

    Base *getImplementation() {
        return reinterpret_cast<Base *>(static_cast<void *>(small));
    }
};

#endif //FUNCTION_FUNCTION_H