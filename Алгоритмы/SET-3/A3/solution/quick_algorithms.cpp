#include "quick_algorithms.h"

#include <random>
#include <cmath>

static void insertion_sort_range(std::vector<long long> &a,
                                 const int left,
                                 const int right) {
    for (int i = left + 1; i <= right; ++i) {
        const long long key = a[i];
        int j = i - 1;
        while (j >= left && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

static void heapify(std::vector<long long> &a,
                    const int left,
                    const int heap_size,
                    int root) {
    while (2 * root + 1 < heap_size) {
        int largest = root;
        const int l = 2 * root + 1;
        const int r = 2 * root + 2;

        if (l < heap_size && a[left + l] > a[left + largest]) {
            largest = l;
        }
        if (r < heap_size && a[left + r] > a[left + largest]) {
            largest = r;
        }
        if (largest == root) {
            break;
        }
        std::swap(a[left + root], a[left + largest]);
        root = largest;
    }
}

static void heap_sort_range(std::vector<long long> &a,
                            const int left,
                            const int right) {
    const int heap_size0 = right - left + 1;
    if (heap_size0 <= 1) {
        return;
    }

    // Построение кучи
    for (int i = heap_size0 / 2 - 1; i >= 0; --i) {
        heapify(a, left, heap_size0, i);
    }

    // Выбор максимума и просеивание
    for (int end = heap_size0 - 1; end > 0; --end) {
        std::swap(a[left], a[left + end]);
        heapify(a, left, end, 0);
    }
}

static void quick_sort_impl(std::vector<long long> &a,
                            const int left,
                            const int right,
                            std::mt19937 &rng) {
    if (left >= right) {
        return;
    }

    int i = left;
    int j = right;

    std::uniform_int_distribution<int> dist(left, right);
    const long long pivot = a[dist(rng)];

    while (i <= j) {
        while (a[i] < pivot) {
            ++i;
        }
        while (a[j] > pivot) {
            --j;
        }
        if (i <= j) {
            std::swap(a[i], a[j]);
            ++i;
            --j;
        }
    }

    if (left < j) {
        quick_sort_impl(a, left, j, rng);
    }
    if (i < right) {
        quick_sort_impl(a, i, right, rng);
    }
}

static void intro_sort_impl(std::vector<long long> &a,
                            const int left,
                            const int right,
                            const int depth_limit,
                            std::mt19937 &rng) {
    if (left >= right) {
        return;
    }

    if (const int size = right - left + 1; size <= 16) {
        insertion_sort_range(a, left, right);
        return;
    }

    if (depth_limit == 0) {
        heap_sort_range(a, left, right);
        return;
    }

    int i = left;
    int j = right;

    std::uniform_int_distribution<int> dist(left, right);
    const long long pivot = a[dist(rng)];

    while (i <= j) {
        while (a[i] < pivot) {
            ++i;
        }
        while (a[j] > pivot) {
            --j;
        }
        if (i <= j) {
            std::swap(a[i], a[j]);
            ++i;
            --j;
        }
    }

    const int next_depth = depth_limit - 1;

    if (left < j) {
        intro_sort_impl(a, left, j, next_depth, rng);
    }
    if (i < right) {
        intro_sort_impl(a, i, right, next_depth, rng);
    }
}

namespace sorting {
    void quick_sort(std::vector<long long> &a) {
        const int n = static_cast<int>(a.size());
        if (n <= 1) {
            return;
        }

        static std::mt19937 rng(712367);
        quick_sort_impl(a, 0, n - 1, rng);
    }

    void intro_sort(std::vector<long long> &a) {
        const int n = static_cast<int>(a.size());
        if (n <= 1) {
            return;
        }

        const int depth_limit = 2 * static_cast<int>(std::log2(static_cast<double>(n)));

        static std::mt19937 rng(987654321);
        intro_sort_impl(a, 0, n - 1, depth_limit, rng);
    }
} // namespace sorting
