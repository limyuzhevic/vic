// TestUtils.hpp - Simplified testing utilities for NLM test suite
#pragma once

#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Simple statistics utilities
class Statistics {
public:
    // Calculate mean of vector
    static double mean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        return sum / data.size();
    }
    
    // Calculate standard deviation
    static double stddev(const std::vector<double>& data) {
        if (data.size() < 2) return 0.0;
        double m = mean(data);
        double sum = 0.0;
        for (double val : data) {
            sum += (val - m) * (val - m);
        }
        return std::sqrt(sum / (data.size() - 1));
    }
    
    // Calculate min
    static double min(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        return *std::min_element(data.begin(), data.end());
    }
    
    // Calculate max
    static double max(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        return *std::max_element(data.begin(), data.end());
    }
    
    // Calculate median
    static double median(std::vector<double> data) {
        if (data.empty()) return 0.0;
        std::sort(data.begin(), data.end());
        size_t n = data.size();
        if (n % 2 == 0) {
            return (data[n/2 - 1] + data[n/2]) / 2.0;
        } else {
            return data[n/2];
        }
    }
};

// Simple random utilities
class RandomUtils {
private:
    static std::mt19937 gen;
    
public:
    // Seed the random generator
    static void seed(uint64_t seed) {
        gen.seed(seed);
    }
    
    // Generate random float in [0, 1)
    static float randomFloat() {
        return static_cast<float>(gen()) / static_cast<float>(gen.max());
    }
    
    // Generate random float in [min, max]
    static float randomFloat(float min, float max) {
        return min + randomFloat() * (max - min);
    }
    
    // Generate random integer in [min, max]
    static int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
    
    // Generate random boolean
    static bool randomBool() {
        return randomFloat() > 0.5;
    }
}; // namespace nlm

// Initialize static random generator
std::mt19937 nlm::RandomUtils::gen;

#endif // TESTUTILS_HPP
