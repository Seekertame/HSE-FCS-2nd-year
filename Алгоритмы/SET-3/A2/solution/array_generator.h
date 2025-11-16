#ifndef AADS_ARRAY_GENERATOR_H
#define AADS_ARRAY_GENERATOR_H
#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

class ArrayGenerator {
public:
    using value_type = long long;

    ArrayGenerator(std::size_t max_size,
                   value_type min_value,
                   value_type max_value,
                   std::uint64_t seed = 42);

    [[nodiscard]] std::vector<value_type> make_random(std::size_t size) const;

    [[nodiscard]] std::vector<value_type> make_reversed(std::size_t size) const;

    [[nodiscard]] std::vector<value_type> make_almost_sorted(std::size_t size) const;

    [[nodiscard]] std::size_t max_size() const noexcept {
        return _max_size;
    }

private:
    std::size_t _max_size;
    value_type _min_value;
    value_type _max_value;

    std::vector<value_type> _random_base;
    std::vector<value_type> _sorted_base;
};

#endif //AADS_ARRAY_GENERATOR_H
