// Test utilities and fixtures for Phase 6
// Common test infrastructure used across all Phase 6 tests

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>

namespace test_phase6::utils {

    // Floating-point comparison for tests
    inline bool compareFloat(float a, float b, float epsilon = 1e-5f) {
        return std::abs(a - b) <= epsilon;
    }
    
    // Vector comparison
    template<typename T>
    bool vectorsEqual(const std::vector<T>& a, const std::vector<T>& b, float epsilon = 1e-5f) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if constexpr (std::is_floating_point_v<T>) {
                if (!compareFloat(a[i], b[i], epsilon)) return false;
            } else {
                if (a[i] != b[i]) return false;
            }
        }
        return true;
    }
    
    // Test random number generator
    class TestRandom {
    public:
        TestRandom(unsigned int seed = 42) : generator(seed) {}
        float uniformFloat(float min = 0.0f, float max = 1.0f) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(generator);
        }
        size_t uniformSizeT(size_t min = 0, size_t max = 1000) {
            std::uniform_int_distribution<size_t> dist(min, max);
            return dist(generator);
        }
        
    private:
        std::mt19937 generator;
    };
    
    // Test constants
    namespace constants {
        constexpr float DEFAULT_SIMULATION_TIME = 1.0f;
        constexpr size_t DEFAULT_TEST_ITERATIONS = 100;
        constexpr float DEFAULT_TOLERANCE = 1e-3f;
        constexpr float DEFAULT_ACCURACY = 0.95f;
    }

} // namespace test_phase6::utils
