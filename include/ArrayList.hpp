#ifndef ARRAY_LIST_HPP
#define ARRAY_LIST_HPP

#include "IList.hpp"
#include <initializer_list>
#include <stdexcept>
#include <cstddef>
#include <utility>
#include <sstream>
#include <string>
#include <type_traits>

template <typename T>
class ArrayList : public IList<T> {
private:
    T* data;
    size_t capacity;
    size_t sz;

    void resize(size_t new_cap) {
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < sz; ++i) {
            new_data[i] = std::move(data[i]);
        }
        delete[] data;
        data = new_data;
        capacity = new_cap;
    }

public:
    using value_type = T;

    ArrayList(size_t initial_capacity = 8) : capacity(initial_capacity > 0 ? initial_capacity : 8), sz(0) {
        data = new T[capacity];
    }

    ArrayList(std::initializer_list<T> list) : ArrayList(list.size() > 8 ? list.size() : 8) {
        for (const auto& item : list) {
            push_back(item);
        }
    }

    ArrayList(const ArrayList& other) : capacity(other.capacity), sz(other.sz) {
        data = new T[capacity];
        for (size_t i = 0; i < sz; ++i) {
            data[i] = other.data[i];
        }
    }

    ArrayList(ArrayList&& other) noexcept : data(other.data), capacity(other.capacity), sz(other.sz) {
        other.data = nullptr;
        other.capacity = 0;
        other.sz = 0;
    }

    ArrayList& operator=(const ArrayList& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            sz = other.sz;
            data = new T[capacity];
            for (size_t i = 0; i < sz; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ArrayList& operator=(ArrayList&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            capacity = other.capacity;
            sz = other.sz;
            other.data = nullptr;
            other.capacity = 0;
            other.sz = 0;
        }
        return *this;
    }

    ~ArrayList() override { delete[] data; }

    void push_back(const T& val) override {
        if (sz == capacity) resize(capacity * 2);
        data[sz++] = val;
    }

    void push_back(T&& val) override {
        if (sz == capacity) resize(capacity * 2);
        data[sz++] = std::move(val);
    }

    T& operator[](size_t index) override {
        if (index >= sz) throw std::out_of_range("Index out of range");
        return data[index];
    }

    const T& operator[](size_t index) const override {
        if (index >= sz) throw std::out_of_range("Index out of range");
        return data[index];
    }

    size_t size() const override { return sz; }
    bool empty() const override { return sz == 0; }

    void clear() override {
        sz = 0;
    }

    int findByValue(const T& value) const override {
        for (size_t i = 0; i < sz; ++i) {
            if (data[i] == value) return static_cast<int>(i);
        }
        return -1;
    }

    const T* findFirstIf(const std::function<bool(const T&)>& predicate) const override {
        for (size_t i = 0; i < sz; ++i) {
            if (predicate(data[i])) return &data[i];
        }
        return nullptr;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < sz; ++i) {
            oss << data[i];
            if (i + 1 < sz) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    T* begin() { return data; }
    T* end() { return data + sz; }
    const T* begin() const { return data; }
    const T* end() const { return data + sz; }
};

#endif 
