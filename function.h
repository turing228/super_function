//
// Created by jesus on 19.01.19.
//

#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <cassert>
#include <functional>
#include <iostream>
#include <new>
#include <utility>
#include <array>
#include <variant>
#include <any>


typedef std::aligned_storage<sizeof(10), alignof(10)>::type Buffer;
Buffer buffer;

template<typename Signature>
struct function;

template<typename T, typename ...Args>
struct function<T(Args...)> {

    function() noexcept : pointer() {}

    function(std::nullptr_t) noexcept : pointer() {}

    function(const function &other) : pointer(other.pointer) {
        if (other != Pointer()) {
            other.getImplementation().copy(static_cast<void *>(pointer.data()));
        }
    }

    function(function &&other) noexcept : pointer(other.pointer) {
        other.pointer = Pointer();
    }

    template<typename F>
    function(const F &f) {
        new(pointer.data()) Implementation<F>(f);
    }

    function &operator=(const function &other) {
        function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) noexcept {
        function(std::move(other)).swap(*this);
        return *this;
    }

    ~function() {
        if (pointer != Pointer()) {
            getImplementation().~Base();
        }
    }

    T operator()(Args &&...args) const {
        if (pointer == Pointer()) {
            throw std::bad_function_call();
        }
        return getImplementation().runFunction(std::forward<Args>(args)...);
    }

    void swap(function &other) noexcept {
        std::swap(pointer, other.pointer);
    }

private:

    typedef std::array<long int, 4> Pointer;
    Pointer pointer;

    struct Base {
        virtual ~Base() {}

        virtual T runFunction(Args &&...args) const = 0;

        virtual void copy(void *where) const = 0;
    };

/*
    struct small_buffer{
        std::aligned_storage<sizeof(char[10]),alignof(char[10])> storage;
    };*/
    struct small_buffer {
        typename std::aligned_storage<10, 2>::type *storage;
        //char storage[10];
    };

    template<typename F>
    struct Implementation : Base {

        std::any f;
        int type;

//        Implementation(){
//            t = std::move(F());
//        }

        Implementation(const F &func) : f() {
            if (sizeof(func) * alignof(func) > 10) {
                f = func;
                type = 0;
            } else {
                small_buffer small;
                new(small.storage) F(func);
                f = small;
                type = 1;
            }
        }

        T runFunction(Args &&...args) const override final {
            if (type == 0) {
                return std::any_cast<F>(f)(std::forward<Args>(args)...);
            }
            //std::cout<< "TYPE:" << f.type().name() << "<<<";
//            return reinterpret_cast<F> (std::any_cast<small_buffer>(f).storage) (std::forward<Args>(args)...);
//            return (reinterpret_cast<F>(std::any_cast<small_buffer>(f).storage))->invoke(args...);
            return (reinterpret_cast<F &> (*std::any_cast<small_buffer>(f).storage))(std::forward<Args>(args)...);
//            if (declval(f) == small_buffer::type) {
//                return reinterpret_cast<T>(std::any_cast<T>(f)) (std::forward<Args>(args)...);
//            }
//            return std::any_cast<T>
//            if (std::holds_alternative<small_buffer>(f)){
//                return reinterpret_cast<T>(std::get<small_buffer>(f)) (std::forward<Args>(args)...);
//            }
//            return std::get<F>(f) (std::forward<Args>(args)...);
        }

        void copy(void *where) const override final { new(where) Implementation<F>(*this); }
    };

    const Base &getImplementation() const {
        assert(pointer != Pointer());
        return *static_cast<const Base *>(static_cast<const void *>(pointer.data()));
    }

    Base &getImplementation() {
        assert(pointer != Pointer());
        return *static_cast<Base *>(static_cast<void *>(pointer.data()));
    }
};

#endif //FUNCTION_FUNCTION_H
