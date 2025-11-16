#include "array_generator.h"

#include <random>
#include <algorithm>
#include <stdexcept>

ArrayGenerator::ArrayGenerator(const std::size_t max_size,
                               const value_type min_value,
                               const value_type max_value,
                               const std::uint64_t seed)
    : _max_size(max_size)
      , _min_value(min_value)
      , _max_value(max_value)
      , _random_base(max_size)
      , _sorted_base(max_size) {
    if (max_size == 0) {
        throw std::invalid_argument("max_size must be > 0");
    }
    if (min_value > max_value) {
        throw std::invalid_argument("min_value must be <= max_value");
    }

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<value_type> dist(min_value, max_value);

    for (std::size_t i = 0; i < _max_size; ++i) {
        _random_base[i] = dist(rng);
    }

    _sorted_base = _random_base;
    std::ranges::sort(_sorted_base);
}

std::vector<ArrayGenerator::value_type>
ArrayGenerator::make_random(const std::size_t size) const {
    if (size > _max_size) {
        throw std::invalid_argument("size > max_size in make_random");
    }
    const auto first = _random_base.begin();
    return {first, first + static_cast<std::ptrdiff_t>(size)};
}

std::vector<ArrayGenerator::value_type>
ArrayGenerator::make_reversed(const std::size_t size) const {
    if (size > _max_size) {
        throw std::invalid_argument("size > max_size in make_reversed");
    }
    std::vector<value_type> result(_sorted_base.begin(),
                                   _sorted_base.begin() + static_cast<std::ptrdiff_t>(size));
    std::ranges::reverse(result);
    return result;
}

std::vector<ArrayGenerator::value_type>
ArrayGenerator::make_almost_sorted(const std::size_t size) const {
    if (size > _max_size) {
        throw std::invalid_argument("size > max_size in make_almost_sorted");
    }

    std::vector<value_type> result(_sorted_base.begin(),
                                   _sorted_base.begin() + static_cast<std::ptrdiff_t>(size));

    // "Небольшое количество" обменов.
    const std::size_t num_swaps = std::max<std::size_t>(1, size / 100);

    // Отдельный генератор, чтобы не зависеть от генератора в конструкторе.
    std::mt19937_64 rng(123456);
    std::uniform_int_distribution<std::size_t> dist_index(0, size - 1);

    for (std::size_t k = 0; k < num_swaps; ++k) {
        const std::size_t i = dist_index(rng);
        const std::size_t j = dist_index(rng);
        std::swap(result[i], result[j]);
    }

    return result;
}
