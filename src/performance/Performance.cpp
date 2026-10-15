#include "Performance.hpp"
#include <iostream>

namespace nlm {

// Performance benchmark logging
bool PerformanceBenchmark::logResults_ = true;

// SIMD utilities implementation
namespace simd {
    bool isAvailable() {
        // Check for AVX support
#ifdef __AVX__
        return true;
#else
        return false;
#endif
    }
    
    template<typename T>
    std::vector<T> add(const std::vector<T>& a, const std::vector<T>& b) {
        std::vector<T> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] + b[i];
        }
        return result;
    }
    
    template<typename T>
    std::vector<T> multiply(const std::vector<T>& a, T scalar) {
        std::vector<T> result(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] * scalar;
        }
        return result;
    }
    
    // Explicit instantiations
    template std::vector<float> simd::add(const std::vector<float>& a, const std::vector<float>& b);
    template std::vector<double> simd::add(const std::vector<double>& a, const std::vector<double>& b);
    template std::vector<int> simd::add(const std::vector<int>& a, const std::vector<int>& b);
    
    template std::vector<float> simd::multiply(const std::vector<float>& a, float scalar);
    template std::vector<double> simd::multiply(const std::vector<double>& a, double scalar);
    template std::vector<int> simd::multiply(const std::vector<int>& a, int scalar);
    
} // namespace simd

} // namespace nlm
