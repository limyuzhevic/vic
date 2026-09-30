#pragma once

#include <cmath>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <limits>

namespace nlm {
namespace Core {
namespace Utilities {

/**
 * Mathematical utility functions for neural network simulations
 */
class MathUtils {
public:
    // Mathematical constants
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double E = 2.71828182845904523536;
    static constexpr double GOLDEN_RATIO = 1.61803398874989484820;
    static constexpr double SQRT_2 = 1.41421356237309504880;
    static constexpr double SQRT_3 = 1.73205080756887729353;

    // Mathematical functions
    template<typename T>
    static T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }

    template<typename T>
    static T lerp(T a, T b, double t) {
        return a + static_cast<T>((b - a) * t);
    }

    template<typename T>
    static T normalize(T value, T min, T max) {
        if constexpr (std::is_floating_point_v<T>) {
            return (value - min) / (max - min);
        } else {
            return static_cast<T>((value - min) * 1.0 / (max - min));
        }
    }

    template<typename T>
    static T denormalize(T normalized, T min, T max) {
        return min + normalized * (max - min);
    }

    static double gaussian(double x, double mean = 0.0, double stddev = 1.0) {
        return std::exp(-0.5 * std::pow((x - mean) / stddev, 2.0)) / (stddev * SQRT_2 * PI);
    }

    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    static double tanh(double x) {
        return std::tanh(x);
    }

    static double relu(double x) {
        return x > 0.0 ? x : 0.0;
    }

    static double softmax(double x, double temperature = 1.0) {
        double expX = std::exp(x / temperature);
        return expX / (1.0 + expX);
    }

    template<typename T>
    static T mean(const std::vector<T>& values) {
        if (values.empty()) return T{};
        T sum = std::accumulate(values.begin(), values.end(), T{});
        return sum / static_cast<T>(values.size());
    }

    template<typename T>
    static T variance(const std::vector<T>& values) {
        if (values.size() < 2) return T{0};
        T m = mean(values);
        T sum = 0;
        for (const T& val : values) {
            sum += (val - m) * (val - m);
        }
        return sum / static_cast<T>(values.size() - 1);
    }

    template<typename T>
    static T stddev(const std::vector<T>& values) {
        return std::sqrt(variance(values));
    }

    template<typename T>
    static bool isPowerOfTwo(T value) {
        return value > 0 && (value & (value - 1)) == 0;
    }

    static uint32_t nextPowerOfTwo(uint32_t value) {
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        return value + 1;
    }

    static double radToDeg(double radians) {
        return radians * 180.0 / PI;
    }

    static double degToRad(double degrees) {
        return degrees * PI / 180.0;
    }

    static double wrapAngle(double angle) {
        while (angle > PI) angle -= 2 * PI;
        while (angle < -PI) angle += 2 * PI;
        return angle;
    }

    template<typename T>
    static bool isClose(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) {
        return std::abs(a - b) <= epsilon;
    }

    template<typename T>
    static T safeDiv(T numerator, T denominator, T defaultValue = T{0}) {
        return denominator != 0 ? numerator / denominator : defaultValue;
    }

    static double interpolate(double x, const std::vector<double>& xValues, 
                              const std::vector<double>& yValues) {
        if (xValues.size() != yValues.size() || xValues.empty()) {
            return 0.0;
        }

        // Find the interval
        auto it = std::upper_bound(xValues.begin(), xValues.end(), x);
        if (it == xValues.begin()) return yValues.front();
        if (it == xValues.end()) return yValues.back();

        auto prevIt = std::prev(it);
        double x1 = *prevIt;
        double x2 = *it;
        double y1 = yValues[prevIt - xValues.begin()];
        double y2 = yValues[it - xValues.begin()];

        return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }

    // Numerical integration
    template<typename T, typename Func>
    static T integrate(Func f, T a, T b, size_t steps = 1000) {
        T h = (b - a) / static_cast<T>(steps);
        T sum = 0.5 * (f(a) + f(b));
        for (size_t i = 1; i < steps; ++i) {
            sum += f(a + i * h);
        }
        return sum * h;
    }

    // Distance and similarity metrics
    static double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }

    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    static double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b) {
        double dot = dotProduct(a, b);
        double normA = 0.0, normB = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        normA = std::sqrt(normA);
        normB = std::sqrt(normB);
        return normA * normB != 0.0 ? dot / (normA * normB) : 0.0;
    }
};

} // namespace Utils
} // namespace Core
} // namespace nlm