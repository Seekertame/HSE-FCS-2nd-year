#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

struct Circle {
    double cx;
    double cy;
    double r2;
};

struct Rectangle {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
};

bool inside_all(const double x, const double y, const std::array<Circle, 3>& cs) {
    for (const Circle& c : cs) {
        double dx = x - c.cx;
        double dy = y - c.cy;
        if (dx * dx + dy * dy > c.r2) {
            return false;
        }
    }
    return true;
}

double estimate_area(const Rectangle& rect,
                     const std::array<Circle, 3>& circles,
                     std::mt19937_64& rng,
                     const int n_points) {
    std::uniform_real_distribution<double> dist_x(rect.x_min, rect.x_max);
    std::uniform_real_distribution<double> dist_y(rect.y_min, rect.y_max);

    int inside_count = 0;
    for (int i = 0; i < n_points; ++i) {
        const double x = dist_x(rng);
        const double y = dist_y(rng);
        if (inside_all(x, y, circles)) {
            ++inside_count;
        }
    }

    const double s_rect = (rect.x_max - rect.x_min) * (rect.y_max - rect.y_min);
    return static_cast<double>(inside_count) / n_points * s_rect;
}

int main() {
    // Файл создается автоматически в cmake-build-debug
    std::ofstream out("data.csv");
    if (!out) {
        std::cerr << "cannot open data.csv\n";
        return 1;
    }

    constexpr double r1 = 1.0;
    const double r2 = std::sqrt(5.0) / 2.0;

    const std::array<Circle, 3> circles{{
        {1.0, 1.0, r1 * r1},
        {1.5, 2.0, r2 * r2},
        {2.0, 1.5, r2 * r2}
    }};

    const double PI = std::acos(-1.0);
    const double S_exact = 0.25 * PI + 1.25 * std::asin(0.8) - 1.0;

    const Rectangle wide{
        0.0,
        2.0 + r2,
        0.0,
        2.0 + r2
    };

    const Rectangle narrow{
        2.0 - r2, 2.0,
        2.0 - r2, 2.0
    };

    std::mt19937_64 rng(42);

    out << "N;"
        << "S_wide;"
        << "S_narrow;"
        << "rel_err_wide;"
        << "rel_err_narrow\n";

    for (int N = 100; N <= 100000; N += 500) {
        const double S_wide = estimate_area(wide, circles, rng, N);
        const double S_narrow = estimate_area(narrow, circles, rng, N);

        const double rel_err_wide = std::abs(S_wide - S_exact) / S_exact;
        const double rel_err_narrow = std::abs(S_narrow - S_exact) / S_exact;

        out << N << ';'
            << std::setprecision(12) << S_wide << ';'
            << std::setprecision(12) << S_narrow << ';'
            << std::setprecision(12) << rel_err_wide << ';'
            << std::setprecision(12) << rel_err_narrow << '\n';
    }

    return 0;
}
