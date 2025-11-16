#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cstddef>

#include "array_generator.h"
#include "sort_algorithms.h"
#include "sort_tester.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ofstream out("data_A2.csv");
    if (!out) {
        std::cerr << "Failed to open results.csv for writing\n";
        return 1;
    }

    ArrayGenerator generator(100'000, 0, 10'000);

    SortTester tester(5);

    // Заголовок CSV
    out << "size,array_type,algorithm,threshold,repeats,time_ms\n";

    for (std::size_t n = 500; n <= 100'000; n += 100) {
        constexpr std::array<int, 6> THRESHOLDS{5, 10, 15, 20, 30, 50};
        const auto random_data = generator.make_random(n);
        const auto reversed_data = generator.make_reversed(n);
        const auto almost_sorted_data = generator.make_almost_sorted(n);

        // random
        {
            const double time_ms = tester.measure_millis(
                [](std::vector<long long> &v) {
                    sorting::merge_sort(v);
                },
                random_data
            );

            out << n << ','
                    << "random" << ','
                    << "merge" << ','
                    << 0 << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }

        for (const int threshold: THRESHOLDS) {
            const double time_ms = tester.measure_millis(
                [threshold](std::vector<long long> &v) {
                    sorting::merge_insertion_sort(v, threshold);
                },
                random_data
            );

            out << n << ','
                    << "random" << ','
                    << "merge_insertion" << ','
                    << threshold << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }

        // reversed

        {
            const double time_ms = tester.measure_millis(
                [](std::vector<long long> &v) {
                    sorting::merge_sort(v);
                },
                reversed_data
            );

            out << n << ','
                    << "reversed" << ','
                    << "merge" << ','
                    << 0 << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }

        for (const int threshold: THRESHOLDS) {
            const double time_ms = tester.measure_millis(
                [threshold](std::vector<long long> &v) {
                    sorting::merge_insertion_sort(v, threshold);
                },
                reversed_data
            );

            out << n << ','
                    << "reversed" << ','
                    << "merge_insertion" << ','
                    << threshold << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }

        // almost_sorted

        {
            const double time_ms = tester.measure_millis(
                [](std::vector<long long> &v) {
                    sorting::merge_sort(v);
                },
                almost_sorted_data
            );

            out << n << ','
                    << "almost_sorted" << ','
                    << "merge" << ','
                    << 0 << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }

        for (const int threshold: THRESHOLDS) {
            const double time_ms = tester.measure_millis(
                [threshold](std::vector<long long> &v) {
                    sorting::merge_insertion_sort(v, threshold);
                },
                almost_sorted_data
            );

            out << n << ','
                    << "almost_sorted" << ','
                    << "merge_insertion" << ','
                    << threshold << ','
                    << tester.repeats() << ','
                    << time_ms << '\n';
        }
    }

    std::cout << "Done. Results written to data_A2.csv\n";
    return 0;
}
