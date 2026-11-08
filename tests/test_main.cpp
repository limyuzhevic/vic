// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <stdexcept>

// Enhanced test assertion helpers
void assertEqual(double expected, double actual, double tolerance, 
                 const std::string& message = "") {
    if (std::abs(expected - actual) > tolerance) {
        throw std::runtime_error("Assertion failed: " + message + 
                                " (expected: " + std::to_string(expected) + 
                                ", actual: " + std::to_string(actual) + ")");
    }
}

void assertWithinRange(double value, double min, double max, 
                       const std::string& message = "") {
    if (value < min || value > max) {
        throw std::runtime_error("Range assertion failed: " + message + 
                                " (value: " + std::to_string(value) + ", expected range: [" + std::to_string(min) + ", " + std::to_string(max) + "])");
    }
}

void assertGreaterThan(double actual, double threshold, 
                        const std::string& message = "") {
    if (actual <= threshold) {
        throw std::runtime_error("Greater than assertion failed: " + message + 
                                " (actual: " + std::to_string(actual) + ", expected > " + std::to_string(threshold) + ")");
    }
}

void assertLessThan(double actual, double threshold, 
                     const std::string& message = "") {
    if (actual >= threshold) {
        throw std::runtime_error("Less than assertion failed: " + message + 
                                " (actual: " + std::to_string(actual) + ", expected < " + std::to_string(threshold) + ")");
    }
}

void assertNotEqual(double actual, double value, 
                     const std::string& message = "") {
    if (std::abs(actual - value) < 1e-10) {
        throw std::runtime_error("Not equal assertion failed: " + message + 
                                " (actual: " + std::to_string(actual) + " == " + std::to_string(value) + ")");
    }
}

// Enhanced test macros for improved error messages
#define TEST_ASSERT_EQUAL(expected, actual, tolerance, message) \
    do { \
        if (std::abs((expected) - (actual)) > (tolerance)) { \
            throw std::runtime_error("Assertion failed: " + std::string(message) + " (expected: " + std::to_string(expected) + ", actual: " + std::to_string(actual) + ")"); \
        } \
    } while(0)

#define TEST_ASSERT_WITHIN_RANGE(value, min, max, message) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            throw std::runtime_error("Range assertion failed: " + std::string(message) + " (value: " + std::to_string(value) + ", expected range: [" + std::to_string(min) + ", " + std::to_string(max) + "])"); \
        } \
    } while(0)

#define TEST_ASSERT_GREATER_THAN(actual, threshold, message) \
    do { \
        if ((actual) <= (threshold)) { \
            throw std::runtime_error("Greater than assertion failed: " + std::string(message) + " (actual: " + std::to_string(actual) + ", expected > " + std::to_string(threshold) + ")"); \
        } \
    } while(0)

#define TEST_ASSERT_LESS_THAN(actual, threshold, message) \
    do { \
        if ((actual) >= (threshold)) { \
            throw std::runtime_error("Less than assertion failed: " + std::string(message) + " (actual: " + std::to_string(actual) + ", expected < " + std::to_string(threshold) + ")"); \
        } \
    } while(0)

// Helper functions for statistical analysis and verification
inline std::vector<double> generateTestData(size_t size, double min = 0.0, double max = 1.0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    
    std::vector<double> data;
    data.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        data.push_back(dis(gen));
    }
    return data;
}

inline double calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

inline double calculateStdDev(const std::vector<double>& data, double mean) {
    if (data.empty()) return 0.0;
    double sum = 0.0;
    for (double val : data) {
        sum += (val - mean) * (val - mean);
    }
    return std::sqrt(sum / data.size());
}

inline std::vector<double> calculateCumulativeStats(const std::vector<double>& data) {
    std::vector<double> result;
    if (data.empty()) return result;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    result.push_back(sorted[0]);           // min
    result.push_back(sorted.back());       // max  
    result.push_back(calculateMean(data)); // mean
    result.push_back(calculateStdDev(data, calculateMean(data))); // std dev
    
    size_t mid = sorted.size() / 2;
    if (sorted.size() % 2 == 0) {
        result.push_back((sorted[mid - 1] + sorted[mid]) / 2.0); // even median
    } else {
        result.push_back(sorted[mid]); // odd median
    }
    
    return result;
}

// Forward declarations of test functions
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Types tests..." << std::endl;
    try {
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Config tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Random tests..." << std::endl;
    try {
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Clock tests..." << std::endl;
    try {
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP tests..." << std::endl;
    try {
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}