#ifndef STD_LIST_ADAPTER_HPP
#define STD_LIST_ADAPTER_HPP

#include "IList.hpp"
#include <vector>
#include <sstream>
#include <stdexcept>
#include <initializer_list>

template <typename T>
class StdListAdapter : public IList<T> {
private:
    std::vector<T> vec;

public:
    using value_type = T;

    StdListAdapter() = default;
    StdListAdapter(std::initializer_list<T> list) : vec(list) {}

    void push_back(const T& val) override { vec.push_back(val); }
    void push_back(T&& val) override { vec.push_back(std::move(val)); }

    T& operator[](size_t index) override {
        if (index >= vec.size()) throw std::out_of_range("Index out of range");
        return vec[index];
    }

    const T& operator[](size_t index) const override {
        if (index >= vec.size()) throw std::out_of_range("Index out of range");
        return vec[index];
    }

    size_t size() const override { return vec.size(); }
    bool empty() const override { return vec.empty(); }
    void clear() override { vec.clear(); }

    int findByValue(const T& value) const override {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] == value) return static_cast<int>(i);
        }
        return -1;
    }

    const T* findFirstIf(const std::function<bool(const T&)>& predicate) const override {
        for (const auto& item : vec) {
            if (predicate(item)) return &item;
        }
        return nullptr;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << "std::vector[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i + 1 < vec.size()) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    auto begin() { return vec.begin(); }
    auto end() { return vec.end(); }
    auto begin() const { return vec.begin(); }
    auto end() const { return vec.end(); }
};

#endif 
