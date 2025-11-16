#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>

#include "array_generator.h"
#include "sort_tester.h"
#include "quick_algorithms.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ofstream out("data_A3.csv");
    if (!out) {
        std::cerr << "Failed to open data_A3.csv for writing\n";
        return 1;
    }

    const ArrayGenerator generator(100'000, 0, 10'000);
    const SortTester tester(3); // число повторов на одну точку

    out << "size,array_type,algorithm,repeats,time_ms\n";

    for (std::size_t n = 500; n <= 100'000; n += 100) {
        const auto random_data = generator.make_random(n);
        const auto reversed_data = generator.make_reversed(n);
        const auto almost_sorted_data = generator.make_almost_sorted(n);

        auto measure_and_write = [&](const std::vector<ArrayGenerator::value_type> &data,
                                     const char *array_type) {
            // quick_sort
            {
                const double t_ms = tester.measure_millis(
                    [](std::vector<long long> &v) {
                        sorting::quick_sort(v);
                    },
                    data
                );

                out << n << ','
                        << array_type << ','
                        << "quick" << ','
                        << tester.repeats() << ','
                        << t_ms << '\n';
            }

            // intro_sort
            {
                const double t_ms = tester.measure_millis(
                    [](std::vector<long long> &v) {
                        sorting::intro_sort(v);
                    },
                    data
                );

                out << n << ','
                        << array_type << ','
                        << "introsort" << ','
                        << tester.repeats() << ','
                        << t_ms << '\n';
            }
        };

        measure_and_write(random_data, "random");
        measure_and_write(reversed_data, "reversed");
        measure_and_write(almost_sorted_data, "almost_sorted");
    }

    std::cout << "Results written to data_A3.csv\n";
    return 0;
}
