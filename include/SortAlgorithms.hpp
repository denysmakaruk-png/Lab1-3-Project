#ifndef SORT_ALGORITHMS_HPP
#define SORT_ALGORITHMS_HPP

#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <cmath>

template <typename T, typename Compare = std::less<T>>
struct DefaultCompare {
    Compare comp;
    DefaultCompare(Compare c = Compare()) : comp(c) {}
    bool operator()(const T& a, const T& b) const { return comp(a, b); }
};

template <typename Container, typename Compare = std::less<typename Container::value_type>>
class ISortStrategy {
public:
    virtual ~ISortStrategy() = default;
    virtual void sort(Container& container, Compare comp = Compare()) = 0;
};

template <typename Container, typename Compare = std::less<typename Container::value_type>>
class InsertionSort : public ISortStrategy<Container, Compare> {
public:
    void sort(Container& c, Compare comp = Compare()) override {
        size_t n = c.size();
        for (size_t i = 1; i < n; ++i) {
            auto key = c[i];
            int j = static_cast<int>(i) - 1;
            while (j >= 0 && comp(key, c[j])) {
                c[j + 1] = c[j];
                --j;
            }
            c[j + 1] = key;
        }
    }
};

template <typename Container, typename Compare = std::less<typename Container::value_type>>
class QuickSort : public ISortStrategy<Container, Compare> {
private:
    void quickSortImpl(Container& c, int low, int high, Compare comp) {
        if (low < high) {
            int pi = partition(c, low, high, comp);
            quickSortImpl(c, low, pi - 1, comp);
            quickSortImpl(c, pi + 1, high, comp);
        }
    }

    int partition(Container& c, int low, int high, Compare comp) {
        auto pivot = c[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (comp(c[j], pivot) || (!comp(pivot, c[j]) && !comp(c[j], pivot))) {
                ++i;
                std::swap(c[i], c[j]);
            }
        }
        std::swap(c[i + 1], c[high]);
        return i + 1;
    }

public:
    void sort(Container& c, Compare comp = Compare()) override {
        if (c.size() <= 1) return;
        quickSortImpl(c, 0, static_cast<int>(c.size()) - 1, comp);
    }
};

template <typename Container, typename Compare = std::less<typename Container::value_type>>
class MergeSort : public ISortStrategy<Container, Compare> {
private:
    void merge(Container& c, int left, int mid, int right, Compare comp) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        std::vector<typename Container::value_type> L, R;
        L.reserve(n1);
        R.reserve(n2);

        for (int i = 0; i < n1; ++i) L.push_back(c[left + i]);
        for (int j = 0; j < n2; ++j) R.push_back(c[mid + 1 + j]);

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (comp(L[i], R[j]) || (!comp(R[j], L[i]) && !comp(L[i], R[j]))) {
                c[k] = L[i];
                ++i;
            } else {
                c[k] = R[j];
                ++j;
            }
            ++k;
        }

        while (i < n1) c[k++] = L[i++];
        while (j < n2) c[k++] = R[j++];
    }

    void mergeSortImpl(Container& c, int left, int right, Compare comp) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortImpl(c, left, mid, comp);
            mergeSortImpl(c, mid + 1, right, comp);
            merge(c, left, mid, right, comp);
        }
    }

public:
    void sort(Container& c, Compare comp = Compare()) override {
        if (c.size() <= 1) return;
        mergeSortImpl(c, 0, static_cast<int>(c.size()) - 1, comp);
    }
};

template <typename Container, typename Compare = std::less<typename Container::value_type>>
class HeapSort : public ISortStrategy<Container, Compare> {
private:
    void heapify(Container& c, int n, int i, Compare comp) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && comp(c[largest], c[left])) largest = left;
        if (right < n && comp(c[largest], c[right])) largest = right;

        if (largest != i) {
            std::swap(c[i], c[largest]);
            heapify(c, n, largest, comp);
        }
    }

public:
    void sort(Container& c, Compare comp = Compare()) override {
        int n = static_cast<int>(c.size());
        for (int i = n / 2 - 1; i >= 0; --i) heapify(c, n, i, comp);
        for (int i = n - 1; i > 0; --i) {
            std::swap(c[0], c[i]);
            heapify(c, i, 0, comp);
        }
    }
};

template <typename Container>
class CountingSort {
public:
    static void sort(Container& c) {
        if (c.empty()) return;
        auto max_it = c[0];
        auto min_it = c[0];
        for (size_t i = 1; i < c.size(); ++i) {
            if (c[i] > max_it) max_it = c[i];
            if (c[i] < min_it) min_it = c[i];
        }

        int range = max_it - min_it + 1;
        std::vector<int> count(range, 0);
        std::vector<typename Container::value_type> output(c.size());

        for (size_t i = 0; i < c.size(); ++i) count[c[i] - min_it]++;
        for (int i = 1; i < range; ++i) count[i] += count[i - 1];
        for (int i = static_cast<int>(c.size()) - 1; i >= 0; --i) {
            output[count[c[i] - min_it] - 1] = c[i];
            count[c[i] - min_it]--;
        }
        for (size_t i = 0; i < c.size(); ++i) c[i] = output[i];
    }
};

template <typename Container>
class RadixSort {
private:
    static void countSortForRadix(Container& c, int exp) {
        size_t n = c.size();
        std::vector<typename Container::value_type> output(n);
        int count[10] = {0};

        for (size_t i = 0; i < n; ++i) count[(c[i] / exp) % 10]++;
        for (int i = 1; i < 10; ++i) count[i] += count[i - 1];
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            output[count[(c[i] / exp) % 10] - 1] = c[i];
            count[(c[i] / exp) % 10]--;
        }
        for (size_t i = 0; i < n; ++i) c[i] = output[i];
    }

public:
    static void sort(Container& c) {
        if (c.empty()) return;
        auto max_val = c[0];
        for (size_t i = 1; i < c.size(); ++i) if (c[i] > max_val) max_val = c[i];
        for (int exp = 1; max_val / exp > 0; exp *= 10) countSortForRadix(c, exp);
    }
};

namespace SortUtils {
    template <typename KeyExtractor, typename Order = std::less<>>
    auto makeKeyComparator(KeyExtractor extractor, Order order = Order()) {
        return [extractor, order](const auto& a, const auto& b) {
            return order(extractor(a), extractor(b));
        };
    }

    template <typename... Comparators>
    auto makeMultiCriteriaComparator(Comparators... comps) {
        return [comps...](const auto& a, const auto& b) {
            bool result = false;
            bool decided = false;
            auto check = [&](auto comp) {
                if (decided) return;
                if (comp(a, b)) {
                    result = true;
                    decided = true;
                } else if (comp(b, a)) {
                    result = false;
                    decided = true;
                }
            };
            (check(comps), ...);
            return result;
        };
    }
}

#endif
