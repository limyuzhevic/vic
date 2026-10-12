#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "src/core/Config/ConfigBuilder.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/memory/Memory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/plasticity/PlasticityRule.hpp"

/**
 * Common test utilities and helpers for NLM testing
 */

namespace TestHelpers {
    
    // Random number generation for deterministic tests
    class RandomGenerator {
    private:
        std::mt19937 rng_;
        
    public:
        RandomGenerator(unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count())
            : rng_(seed) {}
        
        double uniform(double min = 0.0, double max = 1.0) {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(rng_);
        }
        
        int integer(int min = 0, int max = 100) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng_);
        }
        
        double gaussian(double mean = 0.0, double stddev = 1.0) {
            std::normal_distribution<double> dist(mean, stddev);
            return dist(rng_);
        }
    };
    
    // Test data generators
    struct TestData {
        std::vector<double> values;
        std::vector<std::vector<double>> matrix;
        std::vector<std::string> strings;
        
        static TestData generateRandomVector(size_t size, double min = 0.0, double max = 1.0) {
            TestData data;
            RandomGenerator rng;
            data.values.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                data.values.push_back(rng.uniform(min, max));
            }
            return data;
        }
        
        static TestData generateRandomMatrix(size_t rows, size_t cols, double min = 0.0, double max = 1.0) {
            TestData data;
            RandomGenerator rng;
            data.matrix.reserve(rows);
            for (size_t i = 0; i < rows; ++i) {
                std::vector<double> row;
                row.reserve(cols);
                for (size_t j = 0; j < cols; ++j) {
                    row.push_back(rng.uniform(min, max));
                }
                data.matrix.push_back(row);
            }
            return data;
        }
    };
    
    // Performance timing utilities
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
        std::chrono::high_resolution_clock::time_point end_time_;
        bool running_ = false;
        
    public:
        void start() {
            start_time_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }
        
        void stop() {
            end_time_ = std::chrono::high_resolution_clock::now();
            running_ = false;
        }
        
        double elapsedMs() const {
            auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
            return std::chrono::duration<double, std::milli>(end - start_time_).count();
        }
        
        double elapsedSeconds() const {
            return elapsedMs() / 1000.0;
        }
    };
    
    // Numerical comparison utilities
    namespace MathUtils {
        inline bool areEqual(double a, double b, double epsilon = 1e-9) {
            return std::abs(a - b) < epsilon;
        }
        
        inline bool areClose(double a, double b, double epsilon = 1e-9) {
            return std::abs(a - b) < epsilon;
        }
        
        inline double clamp(double value, double min, double max) {
            return std::max(min, std::min(value, max));
        }
        
        inline double normalize(double value, double min, double max) {
            return (value - min) / (max - min);
        }
        
        inline double denormalize(double normalized, double min, double max) {
            return min + normalized * (max - min);
        }
        
        inline double sigmoid(double x) {
            return 1.0 / (1.0 + std::exp(-x));
        }
        
        inline double relu(double x) {
            return x > 0 ? x : 0.0;
        }
        
        inline double sqrt(double x) {
            return std::sqrt(x);
        }
        
        inline double power(double base, double exponent) {
            return std::pow(base, exponent);
        }
    }
    
    // Test assertions helpers
    class Assertions {
    public:
        template<typename T>
        static void equal(const T& expected, const T& actual, const std::string& message = "Values are not equal") {
            if (!(expected == actual)) {
                std::cerr << "ASSERT_EQUAL FAILED: " << message << std::endl;
                std::cerr << "  Expected: " << expected << std::endl;
                std::cerr << "  Actual:   " << actual << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename T, typename U>
        static void equal(const T& expected, const U& actual, const std::string& message = "Values are not equal") {
            if (!(expected == actual)) {
                std::cerr << "ASSERT_EQUAL FAILED: " << message << std::endl;
                std::cerr << "  Expected: " << expected << std::endl;
                std::cerr << "  Actual:   " << actual << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename T>
        static void notEqual(const T& val1, const T& val2, const std::string& message = "Values should not be equal") {
            if (val1 == val2) {
                std::cerr << "ASSERT_NOT_EQUAL FAILED: " << message << std::endl;
                std::cerr << "  Both values: " << val1 << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        static void assertTrue(bool condition, const std::string& message = "Condition should be true") {
            if (!condition) {
                std::cerr << "ASSERT_TRUE FAILED: " << message << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        static void assertFalse(bool condition, const std::string& message = "Condition should be false") {
            if (condition) {
                std::cerr << "ASSERT_FALSE FAILED: " << message << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        static void assertNullptr(const void* ptr, const std::string& message = "Pointer should be null") {
            if (ptr != nullptr) {
                std::cerr << "ASSERT_NULLPTR FAILED: " << message << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        static void assertNotNullptr(const void* ptr, const std::string& message = "Pointer should not be null") {
            if (ptr == nullptr) {
                std::cerr << "ASSERT_NOT_NULLPTR FAILED: " << message << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename T>
        static void isLessThan(const T& a, const T& b, const std::string& message = "Value should be less than") {
            if (!(a < b)) {
                std::cerr << "ASSERT_LESS_THAN FAILED: " << message << std::endl;
                std::cerr << "  A: " << a << ", B: " << b << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename T>
        static void isGreaterThan(const T& a, const T& b, const std::string& message = "Value should be greater than") {
            if (!(a > b)) {
                std::cerr << "ASSERT_GREATER_THAN FAILED: " << message << std::endl;
                std::cerr << "  A: " << a << ", B: " << b << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        static void isClose(double a, double b, double epsilon = 1e-9, const std::string& message = "Values should be close") {
            if (std::abs(a - b) > epsilon) {
                std::cerr << "ASSERT_CLOSE FAILED: " << message << std::endl;
                std::cerr << "  A: " << a << ", B: " << b << " (epsilon: " << epsilon << ")" << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename Container>
        static void containersEqual(const Container& expected, const Container& actual, const std::string& message = "Containers should be equal") {
            if (!(expected == actual)) {
                std::cerr << "ASSERT_CONTAINERS_EQUAL FAILED: " << message << std::endl;
                std::cerr << "  Expected size: " << expected.size() << ", Actual size: " << actual.size() << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
        
        template<typename Iterator1, typename Iterator2>
        static void iteratorsEqual(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2, const std::string& message = "Iterators should be equal") {
            auto it1 = begin1, it2 = begin2;
            while (it1 != end1 && it2 != end2) {
                if (!(*it1 == *it2)) {
                    std::cerr << "ASSERT_ITERATORS_EQUAL FAILED: " << message << std::endl;
                    throw std::runtime_error("Test assertion failed");
                }
                ++it1;
                ++it2;
            }
            if (it1 != end1 || it2 != end2) {
                std::cerr << "ASSERT_ITERATORS_EQUAL FAILED: " << message << std::endl;
                throw std::runtime_error("Test assertion failed");
            }
        }
    };
    
    // Test data providers
    class DataProviders {
    public:
        static ConfigBuilder::Config buildTestConfig() {
            ConfigBuilder::Config config;
            config.learning_rate = 0.01;
            config.plasticity_rate = 0.1;
            config.memory_capacity = 1000.0;
            config.max_neurons = 10000;
            config.max_synapses = 50000;
            config.max_populations = 100;
            config.max_regions = 10;
            config.dt = 0.001;
            config.max_steps = 1000;
            config.checkpoint_interval = 100;
            config.num_threads = 4;
            config.simulation_seed = 12345;
            return config;
        }
        
        static std::vector<double> generateTestSpikeTrain(size_t num_spikes, double duration_ms, double min ISI_ms = 1.0, double max ISI_ms = 10.0) {
            std::vector<double> spikes;
            RandomGenerator rng;
            double current_time = 0.0;
            
            for (size_t i = 0; i < num_spikes; ++i) {
                double isi = rng.uniform(min ISI_ms, max ISI_ms);
                current_time += isi;
                if (current_time > duration_ms) break;
                spikes.push_back(current_time);
            }
            
            std::sort(spikes.begin(), spikes.end());
            return spikes;
        }
        
        static std::vector<std::vector<double>> generateTestConnectivityMatrix(size_t rows, size_t cols, double sparsity = 0.1, double weight_range = 1.0) {
            std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols, 0.0));
            RandomGenerator rng;
            
            for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    if (rng.uniform(0.0, 1.0) < sparsity) {
                        matrix[i][j] = (rng.uniform(0.0, 1.0) < 0.5 ? 1.0 : -1.0) * rng.uniform(0.0, weight_range);
                    }
                }
            }
            
            return matrix;
        }
        
        static std::vector<double> generateTestWeights(size_t size, double min = -1.0, double max = 1.0) {
            std::vector<double> weights(size);
            RandomGenerator rng;
            for (size_t i = 0; i < size; ++i) {
                weights[i] = rng.uniform(min, max);
            }
            return weights;
        }
        
        static std::vector<double> generateTestBiases(size_t size, double mean = 0.0, double stddev = 1.0) {
            std::vector<double> biases(size);
            RandomGenerator rng;
            for (size_t i = 0; i < size; ++i) {
                biases[i] = rng.gaussian(mean, stddev);
            }
            return biases;
        }
    };
}

// For backward compatibility - alias to TestHelpers::Assertions
namespace TestAssertions = TestHelpers::Assertions;
namespace TestDataProviders = TestHelpers::DataProviders;
namespace TestMath = TestHelpers::MathUtils;

#endif // TEST_HELPERS_HPP