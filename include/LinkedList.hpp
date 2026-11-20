#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "IList.hpp"
#include <initializer_list>
#include <stdexcept>
#include <cstddef>
#include <utility>
#include <sstream>
#include <string>

template <typename T>
class LinkedList : public IList<T> {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& val, Node* n = nullptr) : value(val), next(n) {}
        Node(T&& val, Node* n = nullptr) : value(std::move(val)), next(n) {}
    };

    Node* head;
    Node* tail;
    size_t sz;

public:
    using value_type = T;

    class Iterator {
    private:
        Node* current;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        Iterator(Node* node = nullptr) : current(node) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
        Node* getNode() const { return current; }
    };

    LinkedList() : head(nullptr), tail(nullptr), sz(0) {}

    LinkedList(std::initializer_list<T> list) : LinkedList() {
        for (const auto& item : list) {
            push_back(item);
        }
    }

    LinkedList(const LinkedList& other) : LinkedList() {
        for (Node* curr = other.head; curr != nullptr; curr = curr->next) {
            push_back(curr->value);
        }
    }

    LinkedList(LinkedList&& other) noexcept : head(other.head), tail(other.tail), sz(other.sz) {
        other.head = nullptr;
        other.tail = nullptr;
        other.sz = 0;
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            for (Node* curr = other.head; curr != nullptr; curr = curr->next) {
                push_back(curr->value);
            }
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            sz = other.sz;
            other.head = nullptr;
            other.tail = nullptr;
            other.sz = 0;
        }
        return *this;
    }

    ~LinkedList() override { clear(); }

    void push_back(const T& val) override {
        Node* node = new Node(val);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++sz;
    }

    void push_back(T&& val) override {
        Node* node = new Node(std::move(val));
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++sz;
    }

    T& operator[](size_t index) override {
        if (index >= sz) throw std::out_of_range("Index out of range");
        Node* curr = head;
        for (size_t i = 0; i < index; ++i) curr = curr->next;
        return curr->value;
    }

    const T& operator[](size_t index) const override {
        if (index >= sz) throw std::out_of_range("Index out of range");
        Node* curr = head;
        for (size_t i = 0; i < index; ++i) curr = curr->next;
        return curr->value;
    }

    size_t size() const override { return sz; }
    bool empty() const override { return sz == 0; }

    void clear() override {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head = tail = nullptr;
        sz = 0;
    }

    int findByValue(const T& value) const override {
        size_t idx = 0;
        for (Node* curr = head; curr != nullptr; curr = curr->next, ++idx) {
            if (curr->value == value) return static_cast<int>(idx);
        }
        return -1;
    }

    const T* findFirstIf(const std::function<bool(const T&)>& predicate) const override {
        for (Node* curr = head; curr != nullptr; curr = curr->next) {
            if (predicate(curr->value)) return &curr->value;
        }
        return nullptr;
    }

    std::string toString() const override {
        std::ostringstream oss;
        oss << "[";
        Node* curr = head;
        while (curr) {
            oss << curr->value;
            if (curr->next) oss << " -> ";
            curr = curr->next;
        }
        oss << "]";
        return oss.str();
    }

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
};

#endif 
