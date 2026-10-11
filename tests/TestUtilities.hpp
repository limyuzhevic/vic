// Common test utilities for NLM tests
// Provides reusable test infrastructure and helper functions

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include "TestInfrastructure.hpp"

namespace nlm_test {

// Common test configuration
struct TestConfig {
    static constexpr double DEFAULT_TOLERANCE = 1e-6;
    static constexpr float FLOAT_TOLERANCE = 1e-6f;
    static constexpr size_t DEFAULT_SEED = 42;
    static constexpr nlm::SimulationStep DEFAULT_STEPS = 1000;
    static constexpr nlm::Timestamp DEFAULT_TIME = 1.0;
    static constexpr float DEFAULT_WEIGHT = 1.0f;
    static constexpr nlm::MembranePotential DEFAULT_CURRENT = 10.0f;
};

// Test data generators
class TestDataGenerator {
public:
    static std::vector<nlm::NeuronId> generateNeuronIds(size_t count, uint64_t start = 0) {
        std::vector<nlm::NeuronId> ids;
        for (size_t i = 0; i < count; ++i) {
            ids.push_back(nlm::NeuronId(start + i));
        }
        return ids;
    }
    
    static std::vector<nlm::SynapseId> generateSynapseIds(size_t count, uint64_t start = 1000) {
        std::vector<nlm::SynapseId> ids;
        for (size_t i = 0; i < count; ++i) {
            ids.push_back(nlm::SynapseId(start + i));
        }
        return ids;
    }
    
    static std::vector<nlm::RegionId> generateRegionIds(size_t count, uint64_t start = 2000) {
        std::vector<nlm::RegionId> ids;
        for (size_t i = 0; i < count; ++i) {
            ids.push_back(nlm::RegionId(start + i));
        }
        return ids;
    }
    
    static std::vector<float> generateRandomValues(size_t count, float min = 0.0f, float max = 1.0f) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        
        std::vector<float> values;
        for (size_t i = 0; i < count; ++i) {
            values.push_back(dis(gen));
        }
        return values;
    }
    
    static nlm::NeuronType randomNeuronType() {
        int type = rand() % 6;
        return static_cast<nlm::NeuronType>(type);
    }
    
    static nlm::SynapseType randomSynapseType() {
        int type = rand() % 5;
        return static_cast<nlm::NeuronType>(type);
    }
    
    static nlm::PlasticityFlags randomPlasticityFlags() {
        nlm::PlasticityFlags flags;
        flags.hebbian = rand() % 2 == 0;
        flags.stdp = rand() % 2 == 0;
        flags.reward_modulated = rand() % 2 == 0;
        flags.structural = rand() % 2 == 0;
        flags.eligible = rand() % 2 == 0;
        return flags;
    }
};

// Test result formatter
class TestFormatter {
public:
    static std::string formatFloat(float value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    static std::string formatDouble(double value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    static std::string formatTestResult(const TestResult& result) {
        std::stringstream ss;
        ss << (result.passed ? "PASS" : "FAIL") << ": " << result.name;
        if (!result.passed && !result.errorMessage.empty()) {
            ss << " (" << result.errorMessage << ")";
        }
        ss << " [" << std::fixed << std::setprecision(2) << result.duration_ms << "ms]";
        return ss.str();
    }
};

// Memory checking utilities
class MemoryChecker {
public:
    static void checkMemoryLeaks() {
        // Placeholder for memory leak detection
        // Could use valgrind, address sanitizer, or custom tracking
        if constexpr (__has_builtin(__builtin_check_stack)) {
            // Stack overflow check
        }
    }
    
    static void trackAllocationSize() {
        // Track allocation sizes for performance testing
    }
};

// Benchmark utilities
class Benchmark {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string name;
    
public:
    Benchmark(const std::string& benchmarkName) : name(benchmarkName) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    ~Benchmark() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "    BENCHMARK: " << name << " took " 
                  << duration.count() << " microseconds" << std::endl;
    }
    
    double getElapsedTimeMs() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count();
    }
};

// Test helper classes
class TestConfigBuilder {
public:
    static std::shared_ptr<nlm::Config> createDefaultConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("weight_initial_min", -1.0, nlm::ConfigSource::Default);
        config->set("weight_initial_max", 1.0, nlm::ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<nlm::Config> createSmallConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<nlm::Config> createLargeConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
        return config;
    }
};

// Test assertion helpers
namespace assertions {
    inline void assertFloatEqual(float a, float b, float tolerance = TestConfig::FLOAT_TOLERANCE, 
                                const std::string& message = "Float values not equal") {
        if (!nlm_test::utils::approxEqual(a, b, tolerance)) {
            throw std::runtime_error(message + ": " + 
                                   TestFormatter::formatFloat(a) + " != " + 
                                   TestFormatter::formatFloat(b));
        }
    }
    
    inline void assertDoubleEqual(double a, double b, double tolerance = TestConfig::DEFAULT_TOLERANCE,
                                 const std::string& message = "Double values not equal") {
        if (!nlm_test::utils::approxEqual(a, b, tolerance)) {
            throw std::runtime_error(message + ": " + 
                                   TestFormatter::formatDouble(a) + " != " + 
                                   TestFormatter::formatDouble(b));
        }
    }
    
    inline void assertVectorEqual(const std::vector<float>& a, const std::vector<float>& b, 
                                 float tolerance = TestConfig::FLOAT_TOLERANCE,
                                 const std::string& message = "Vector values not equal") {
        if (a.size() != b.size()) {
            throw std::runtime_error(message + ": Vectors have different sizes: " +
                                   std::to_string(a.size()) + " != " + std::to_string(b.size()));
        }
        
        for (size_t i = 0; i < a.size(); ++i) {
            if (!nlm_test::utils::approxEqual(a[i], b[i], tolerance)) {
                throw std::runtime_error(message + ": Elements at index " + std::to_string(i) +
                                       " differ: " + TestFormatter::formatFloat(a[i]) + " != " +
                                       TestFormatter::formatFloat(b[i]));
            }
        }
    }
}

} // namespace nlm_test
