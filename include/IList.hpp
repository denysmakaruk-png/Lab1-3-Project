#ifndef ILIST_HPP
#define ILIST_HPP

#include <cstddef>
#include <string>
#include <functional>


template <typename T>
class IList {
public:
    using value_type = T;

    virtual ~IList() = default;

    virtual void push_back(const T& val) = 0;
    virtual void push_back(T&& val) = 0;
    virtual T& operator[](size_t index) = 0;
    virtual const T& operator[](size_t index) const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void clear() = 0;

    virtual int findByValue(const T& value) const = 0;
    virtual const T* findFirstIf(const std::function<bool(const T&)>& predicate) const = 0;

    virtual std::string toString() const = 0;
};

#endif 
