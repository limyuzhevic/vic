// Test macros and utilities for NLM test suite
// Provides additional test assertions, helper functions, and utility macros

#pragma once

#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

// Enhanced assertion macros for floating-point comparisons
#define EXPECT_FLOAT_EQ_NEAR(actual, expected, tolerance) \
    do { \
        if (std::abs((actual) - (expected)) > (tolerance)) { \
            FAIL() << "Value mismatch: expected " << (expected) \
                  << ", got " << (actual) << \
                  " (tolerance: " << (tolerance) << ")"; \
        } \
    } while (0)

#define ASSERT_FLOAT_EQ_NEAR(actual, expected, tolerance) \
    do { \
        if (std::abs((actual) - (expected)) > (tolerance)) { \
            ADD_FAILURE() << "Value mismatch: expected " << (expected) \
                         << ", got " << (actual) << \
                         " (tolerance: " << (tolerance) << ")"; \
            throw ::testing::AssertionFailure(); \
        } \
    } while (0)

#define EXPECT_MEMB_POTENTIAL(actual, expected) \
    EXPECT_FLOAT_EQ_NEAR((actual).getMembranePotential(), (expected), 1e-6f)

#define EXPECT_SYNAPSE_WEIGHT(actual, expected) \
    EXPECT_FLOAT_EQ_NEAR((actual).getWeight(), (expected), 1e-6f)

#define EXPECT_EFFICACY(actual, expected) \
    EXPECT_FLOAT_EQ_NEAR((actual).getEfficacy(), (expected), 1e-6f)

// Memory comparison with tolerance for floating-point values
#define EXPECT_VECTOR_APPROX(actual, expected, tolerance) \
    do { \
        ASSERT_EQ((actual).size(), (expected).size()); \
        for (size_t i = 0; i < (actual).size(); ++i) { \
            EXPECT_FLOAT_EQ_NEAR((actual)[i], (expected)[i], (tolerance)); \
        } \
    } while (0)

// Type-specific assertion macros
#define EXPECT_NEURON_ID(actual, expected) \
    EXPECT_EQ((actual).getId(), (expected).getId()); \
    EXPECT_EQ(static_cast<int>((actual).getId()), static_cast<int>((expected).getId()))

#define EXPECT_SYNAPSE_ID(actual, expected) \
    EXPECT_EQ((actual).getId(), (expected).getId()); \
    EXPECT_EQ(static_cast<int>((actual).getId()), static_cast<int>((expected).getId()))

// Helper functions for test data manipulation
namespace TestHelpers {

// Generate random test data
inline std::vector<float> generateRandomFloats(int count, float min = 0.0f, float max = 1.0f) {
    std::vector<float> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        result.push_back(min + static_cast<float>(rand()) / RAND_MAX * (max - min));
    }
    return result;
}

// Generate spike trains
inline std::vector<float> generateSpikeTrain(int numSpikes, float duration = 100.0f) {
    std::vector<float> spikes;
    spikes.reserve(numSpikes);
    for (int i = 0; i < numSpikes; ++i) {
        spikes.push_back(static_cast<float>(i) * duration / numSpikes);
    }
    std::random_shuffle(spikes.begin(), spikes.end());
    return spikes;
}

// Validate spike train ordering
inline bool validateSpikeTrain(const std::vector<float>& spikes) {
    if (spikes.empty()) return true;
    
    for (size_t i = 1; i < spikes.size(); ++i) {
        if (spikes[i] <= spikes[i - 1]) {
            return false;
        }
    }
    return true;
}

// Calculate spike frequency
inline float calculateSpikeFrequency(const std::vector<float>& spikes, float duration) {
    if (duration <= 0.0f) return 0.0f;
    return static_cast<float>(spikes.size()) / duration;
}

// Memory test data generators
inline std::vector<std::pair<nlm::NeuronId, float>> generateMemoryData(int count) {
    std::vector<std::pair<nlm::NeuronId, float>> data;
    data.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        data.emplace_back(nlm::NeuronId(i), static_cast<float>(i) / static_cast<float>(count));
    }
    
    return data;
}

// Configuration test data generators
inline std::vector<std::pair<std::string, nlm::ConfigValue>> generateConfigData() {
    std::vector<std::pair<std::string, nlm::ConfigValue>> data;
    
    data.emplace_back("neuron_count", static_cast<int64_t>(100));
    data.emplace_back("region_count", static_cast<int64_t>(2));
    data.emplace_back("connection_probability", 0.1f);
    data.emplace_back("simulation_timestep", 1.0f);
    data.emplace_back("learning_rate", 0.01f);
    data.emplace_back("stdp_weight", 0.01f);
    data.emplace_back("hebbian_rate", 0.05f);
    
    return data;
}

// Validation helper functions
inline bool validateNeuronData(const nlm::Neuron& neuron) {
    return neuron.getId().isValid() && 
           neuron.getMembranePotential() >= -100.0f && 
           neuron.getMembranePotential() <= 100.0f &&
           neuron.getThreshold() >= -100.0f && neuron.getThreshold() <= 100.0f;
}

inline bool validateSynapseData(const nlm::Synapse& synapse) {
    return synapse.getId().isValid() && 
           synapse.getSourceNeuron().isValid() && 
           synapse.getDestinationNeuron().isValid() &&
           synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f &&
           synapse.getDelay() >= 0 && synapse.getDelay() <= 1000;
}

inline bool validateConfigData(const nlm::Config& config) {
    // Basic validation - check for essential keys
    std::vector<std::string> essentialKeys = {
        "neuron_count", "region_count", "simulation_timestep"
    };
    
    for (const auto& key : essentialKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    
    return true;
}

// Test result formatter
class TestResultFormatter {
public:
    static std::string formatTestName(const std::string& testName, const std::string& testCase) {
        return testName + "." + testCase;
    }
    
    static std::string formatPerformanceResult(double timeMs, size_t iterations) {
        double opsPerMs = iterations / timeMs;
        std::stringstream ss;
        ss << "Performance: " << timeMs << " ms for " << iterations 
           << " iterations (" << opsPerMs << " ops/ms)";
        return ss.str();
    }
    
    static std::string formatMemoryResult(size_t memoryUsage) {
        std::stringstream ss;
        if (memoryUsage < 1024) {
            ss << memoryUsage << " bytes";
        } else if (memoryUsage < 1024 * 1024) {
            ss << memoryUsage / 1024 << " KB";
        } else {
            ss << memoryUsage / (1024 * 1024) << " MB";
        }
        return ss.str();
    }
};

// Test progress tracker
class TestProgressTracker {
public:
    TestProgressTracker() : totalTests(0), passedTests(0), failedTests(0) {}
    
    void incrementTotal() { totalTests++; }
    void incrementPassed() { passedTests++; }
    void incrementFailed() { failedTests++; }
    
    double getPassRate() const {
        if (totalTests == 0) return 0.0;
        return static_cast<double>(passedTests) / totalTests * 100.0;
    }
    
    std::string getProgressReport() const {
        std::stringstream ss;
        ss << "Progress: " << passedTests << "/" << totalTests << " tests passed (" 
           << getPassRate() << "%)";
        if (failedTests > 0) {
            ss << " - " << failedTests << " tests failed";
        }
        return ss.str();
    }
    
private:
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
};

// Test data validator
class TestDataValidator {
public:
    static bool validateTestData(const std::string& testName, const std::vector<std::string>& testData) {
        if (testData.empty()) {
            return false;
        }
        
        for (const auto& data : testData) {
            if (data.empty() || data.length() > 10000) {
                return false;
            }
        }
        
        return true;
    }
    
    static std::vector<std::string> filterTestData(const std::vector<std::string>& testData, 
                                                   const std::function<bool(const std::string&)>& filter) {
        std::vector<std::string> result;
        for (const auto& data : testData) {
            if (filter(data)) {
                result.push_back(data);
            }
        }
        return result;
    }
};

} // namespace TestHelpers

// Global test utilities
class TestGlobals {
public:
    static TestGlobals& getInstance() {
        static TestGlobals instance;
        return instance;
    }
    
    void setRandomSeed(int seed) {
        srand(seed);
        randomSeed = seed;
    }
    
    int getRandomSeed() const {
        return randomSeed;
    }
    
    void reset() {
        randomSeed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
        srand(randomSeed);
    }
    
private:
    TestGlobals() : randomSeed(0) {}
    int randomSeed;
};

// Test fixture base with enhanced functionality
class EnhancedTestBase : public ::testing::Test {
protected:
    void SetUp() override {
        TestGlobals::getInstance().setRandomSeed(42); // Deterministic for tests
        setupTime = std::chrono::high_resolution_clock::now();
    }
    
    void TearDown() override {
        cleanupTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(cleanupTime - setupTime);
        
        if (showTiming) {
            std::cout << "[TEST] Setup/teardown completed in " << duration.count() << " ms" << std::endl;
        }
    }
    
    // Enhanced assertion helpers
    void expectApproximatelyEqual(float actual, float expected, float tolerance = 1e-6f) {
        if (std::abs(actual - expected) > tolerance) {
            FAIL() << "Expected approximately " << expected << ", got " << actual 
                  << " (tolerance: " << tolerance << ")";
        }
    }
    
    void expectVectorContains(const std::vector<int>& vec, int value) {
        if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
            FAIL() << "Vector does not contain value: " << value;
        }
    }
    
    // Utility methods
    std::vector<nlm::NeuronId> generateNeuronIds(int count) {
        std::vector<nlm::NeuronId> ids;
        for (int i = 0; i < count; ++i) {
            ids.emplace_back(i);
        }
        return ids;
    }
    
    std::vector<nlm::SynapseId> generateSynapseIds(int count) {
        std::vector<nlm::SynapseId> ids;
        for (int i = 0; i < count; ++i) {
            ids.emplace_back(i);
        }
        return ids;
    }
    
    bool measureTime(std::function<void()> func, double& timeMs) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        timeMs = static_cast<double>(duration.count());
        return true;
    }
    
protected:
    std::chrono::high_resolution_clock::time_point setupTime;
    std::chrono::high_resolution_clock::time_point cleanupTime;
    bool showTiming = false;
};

} // namespace test
} // namespace nlm
