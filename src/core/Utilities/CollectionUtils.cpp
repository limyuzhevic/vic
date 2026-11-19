#include "MathUtils.hpp"
#include <random>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// CollectionUtils implementation
template<typename T>
std::vector<T> CollectionUtils::toVector(const T* array, size_t size) {
    return std::vector<T>(array, array + size);
}

template<typename T>
T CollectionUtils::sum(const std::vector<T>& values) {
    T result = T();
    for (const auto& val : values) {
        result += val;
    }
    return result;
}

template<typename T>
T CollectionUtils::product(const std::vector<T>& values) {
    T result = T(1);
    for (const auto& val : values) {
        result *= val;
    }
    return result;
}

template<typename T>
T CollectionUtils::max(const std::vector<T>& values) {
    if (values.empty()) throw std::invalid_argument("Cannot find max of empty vector");
    return *std::max_element(values.begin(), values.end());
}

template<typename T>
T CollectionUtils::min(const std::vector<T>& values) {
    if (values.empty()) throw std::invalid_argument("Cannot find min of empty vector");
    return *std::min_element(values.begin(), values.end());
}

template<typename T>
double CollectionUtils::average(const std::vector<T>& values) {
    if (values.empty()) return 0.0;
    return static_cast<double>(sum(values)) / values.size();
}

template<typename T>
double CollectionUtils::variance(const std::vector<T>& values) {
    if (values.size() <= 1) return 0.0;
    double avg = average(values);
    double sum = 0.0;
    for (const auto& val : values) {
        sum += (val - avg) * (val - avg);
    }
    return sum / (values.size() - 1);
}

template<typename T>
void CollectionUtils::shuffle(std::vector<T>& vec, std::mt19937& rng) {
    std::shuffle(vec.begin(), vec.end(), rng);
}

template<typename T>
void CollectionUtils::sortDescending(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end(), std::greater<T>());
}

template<typename T>
void CollectionUtils::sortAscending(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
}

template<typename T>
std::vector<T> CollectionUtils::getUniqueElements(const std::vector<T>& vec) {
    std::vector<T> result = vec;
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

template<typename T>
std::vector<T> CollectionUtils::filter(const std::vector<T>& vec, std::function<bool(const T&)> predicate) {
    std::vector<T> result;
    for (const auto& val : vec) {
        if (predicate(val)) {
            result.push_back(val);
        }
    }
    return result;
}

template<typename T, typename U>
std::vector<U> CollectionUtils::transform(const std::vector<T>& vec, std::function<U(const T&)> func) {
    std::vector<U> result;
    for (const auto& val : vec) {
        result.push_back(func(val));
    }
    return result;
}

} // namespace nlm
