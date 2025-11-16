#ifndef AADS_SORT_TESTER_H
#define AADS_SORT_TESTER_H
#pragma once

#include <vector>
#include <chrono>
#include <stdexcept>

#include "array_generator.h"

class SortTester {
public:
    using value_type = ArrayGenerator::value_type;

    explicit SortTester(const int repeats)
        : _repeats(repeats) {
        if (repeats <= 0) {
            throw std::invalid_argument("repeats must be > 0");
        }
    }

    template<typename SortFunc>
    [[nodiscard]] double measure_millis(SortFunc sort, const std::vector<value_type> &data) const {
        using clock = std::chrono::high_resolution_clock;
        using ms = std::chrono::duration<double, std::milli>;

        double total_ms = 0.0;

        for (int iter = 0; iter < _repeats; ++iter) {
            std::vector<value_type> copy{data.begin(), data.end()};

            const auto start = clock::now();
            sort(copy);
            const auto finish = clock::now();
            const auto elapsed = std::chrono::duration_cast<ms>(finish - start);

            total_ms += elapsed.count();
        }

        return total_ms / static_cast<double>(_repeats);
    }

    [[nodiscard]] int repeats() const noexcept {
        return _repeats;
    }

private:
    int _repeats;
};

#endif //AADS_SORT_TESTER_H
