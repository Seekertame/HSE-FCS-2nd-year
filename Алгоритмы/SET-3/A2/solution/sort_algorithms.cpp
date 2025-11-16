#include "sort_algorithms.h"

namespace sorting {
    // Вспомогательные функции

    static void insertion_sort(std::vector<long long> &a, const int left, const int right) {
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

    static void merge(std::vector<long long> &a,
                      std::vector<long long> &tmp,
                      const int left, const int mid, const int right) {
        int i = left;
        int j = mid + 1;
        int k = left;

        while (i <= mid && j <= right) {
            if (a[i] <= a[j]) {
                tmp[k++] = a[i++];
            } else {
                tmp[k++] = a[j++];
            }
        }
        while (i <= mid) {
            tmp[k++] = a[i++];
        }
        while (j <= right) {
            tmp[k++] = a[j++];
        }
        for (int p = left; p <= right; ++p) {
            a[p] = tmp[p];
        }
    }

    static void merge_sort_impl(std::vector<long long> &a,
                                std::vector<long long> &tmp,
                                const int left, const int right) {
        const int len = right - left + 1;
        if (len <= 1) {
            return;
        }

        const int mid = left + (right - left) / 2;
        merge_sort_impl(a, tmp, left, mid);
        merge_sort_impl(a, tmp, mid + 1, right);
        merge(a, tmp, left, mid, right);
    }

    static void merge_insertion_sort_impl(std::vector<long long> &a,
                                          std::vector<long long> &tmp,
                                          const int left, const int right,
                                          const int threshold) {
        const int len = right - left + 1;
        if (len <= 1) {
            return;
        }
        if (len <= threshold) {
            insertion_sort(a, left, right);
            return;
        }

        const int mid = left + (right - left) / 2;
        merge_insertion_sort_impl(a, tmp, left, mid, threshold);
        merge_insertion_sort_impl(a, tmp, mid + 1, right, threshold);
        merge(a, tmp, left, mid, right);
    }

    // Публичные функции

    void merge_sort(std::vector<long long> &a) {
        if (a.empty()) {
            return;
        }
        std::vector<long long> tmp(a.size());
        merge_sort_impl(a, tmp, 0, static_cast<int>(a.size()) - 1);
    }

    void merge_insertion_sort(std::vector<long long> &a, const int threshold) {
        if (a.empty()) {
            return;
        }
        std::vector<long long> tmp(a.size());
        merge_insertion_sort_impl(a, tmp, 0, static_cast<int>(a.size()) - 1, threshold);
    }
} // namespace sorting
