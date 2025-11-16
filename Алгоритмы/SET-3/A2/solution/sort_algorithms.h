#ifndef AADS_SORT_ALGORITHMS_H
#define AADS_SORT_ALGORITHMS_H
#pragma once

#include <vector>

namespace sorting {
    void merge_sort(std::vector<long long> &a);

    // threshold — порог переключения на INSERTION SORT
    void merge_insertion_sort(std::vector<long long> &a, int threshold);
} // namespace sorting

#endif //AADS_SORT_ALGORITHMS_H
