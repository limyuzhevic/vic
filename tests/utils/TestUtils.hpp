// NLM Test Utilities and Fixtures
// Comprehensive testing infrastructure for NLM project
// Supports unit, integration, performance, and stress testing

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <unordered_map>
#include <optional>
#include <cassert>
#include <cmath>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "experiment/ExperimentManager.hpp"

namespace nlm::tests {

// Test assertion framework
class TestAssert {
public:
    static void equal(double expected, double actual, double tolerance = 1e-9,
                      const std::string& message = "Values should be equal") {
        if (std::abs(expected - actual) > tolerance) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (expected: " + std::to_string(expected) +
                                    ", actual: " + std::to_string(actual) + ")");
        }
    }
    
    static void notEqual(double a, double b, double tolerance = 1e-9,
                         const std::string& message = "Values should not be equal") {
        if (std::abs(a - b) <= tolerance) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (both are approximately: " + std::to_string(a) + ")");
        }
    }
    
    static void greater(double actual, double expected,
                        const std::string& message = "Value should be greater") {
        if (actual <= expected) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (expected > " + std::to_string(expected) +
                                    ", actual: " + std::to_string(actual) + ")");
        }
    }
    
    static void less(double actual, double expected,
                     const std::string& message = "Value should be less") {
        if (actual >= expected) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (expected < " + std::to_string(expected) +
                                    ", actual: " + std::to_string(actual) + ")");
        }
    }
    
    static void true(bool condition, const std::string& message = "Condition should be true") {
        if (!condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
    
    static void false(bool condition, const std::string& message = "Condition should be false") {
        if (condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
    
    static void contains(const std::string& container, const std::string& value,
                         const std::string& message = "Container should contain value") {
        if (container.find(value) == std::string::npos) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (value not found in container)");
        }
    }
    
    static void notContains(const std::string& container, const std::string& value,
                            const std::string& message = "Container should not contain value") {
        if (container.find(value) != std::string::npos) {
            throw std::runtime_error("Assertion failed: " + message +
                                    " (value found in container)");
        }
    }
};

// Test configuration
struct TestConfig {
    std::string name;
    std::string description;
    std::map<std::string, std::string> parameters;
    std::chrono::milliseconds timeout{30000};
    bool enabled = true;
    bool printDetails = false;
    
    TestConfig(const std::string& n = "Unnamed Test",
               const std::string& desc = "",
               const std::chrono::milliseconds& timeout = std::chrono::milliseconds{30000})
        : name(n), description(desc), timeout(timeout) {}
};

// Test utilities
class TestUtils {
public:
    // Random number generators
    static std::mt19937& getRandomGenerator() {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        return rng;
    }
    
    static double randomDouble(double min = 0.0, double max = 1.0) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(getRandomGenerator());
    }
    
    static int randomInt(int min = 0, int max = 100) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(getRandomGenerator());
    }
    
    static double randomNormal(double mean = 0.0, double stddev = 1.0) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(getRandomGenerator());
    }
    
    // File operations
    static std::string generateTestFilePath(const std::string& filename = "") {
        static std::string testDir = "/tmp/nlm_test_" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        
        if (!filename.empty()) {
            return testDir + "/" + filename;
        }
        return testDir;
    }
    
    static bool writeTestFile(const std::string& content, const std::string& filename = "") {
        try {
            std::string filepath = filename.empty() ? generateTestFilePath() : generateTestFilePath(filename);
            std::ofstream file(filepath, std::ios::out | std::ios::trunc);
            if (!file.is_open()) return false;
            file << content;
            file.close();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static std::string readTestFile(const std::string& filename = "") {
        try {
            std::string filepath = filename.empty() ? generateTestFilePath() : generateTestFilePath(filename);
            std::ifstream file(filepath, std::ios::in);
            if (!file.is_open()) return "";
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            return buffer.str();
        } catch (...) {
            return "";
        }
    }
    
    // Time utilities
    static std::chrono::steady_clock::time_point now() {
        return std::chrono::steady_clock::now();
    }
    
    static double elapsedMs(const std::chrono::steady_clock::time_point& start) {
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    static std::string formatTime(double ms) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << ms << "ms";
        return ss.str();
    }
    
    // String utilities
    static std::string toString(double value, int precision = 6) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    static std::string generateUniqueName(const std::string& prefix = "test") {
        return prefix + "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    
    // Configuration utilities
    static std::shared_ptr<nlm::Config> createTestConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("test_param_1", 42, nlm::ConfigSource::Runtime);
        config->set("test_param_2", "test_value", nlm::ConfigSource::Runtime);
        config->set("test_param_3", 3.14, nlm::ConfigSource::Runtime);
        config->set("test_param_4", true, nlm::ConfigSource::Runtime);
        config->set("test_param_5", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
        return config;
    }
    
    // Performance utilities
    static double calculateThroughput(size_t operations, double timeMs) {
        if (timeMs <= 0.0) return 0.0;
        return operations / (timeMs / 1000.0); // operations per second
    }
    
    static double calculateMemoryUsageMB(size_t bytes) {
        return static_cast<double>(bytes) / (1024.0 * 1024.0);
    }
    
    // Data utilities
    static std::vector<double> generateNormalData(size_t size, double mean = 0.0, double stddev = 1.0) {
        std::vector<double> data(size);
        std::normal_distribution<double> dist(mean, stddev);
        for (auto& value : data) {
            value = dist(getRandomGenerator());
        }
        return data;
    }
    
    static std::vector<double> generateRandomData(size_t size, double min = 0.0, double max = 1.0) {
        std::vector<double> data(size);
        std::uniform_real_distribution<double> dist(min, max);
        for (auto& value : data) {
            value = dist(getRandomGenerator());
        }
        return data;
    }
    
    static double calculateMean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        return sum / data.size();
    }
    
    static double calculateStdDev(const std::vector<double>& data, double mean = 0.0) {
        if (data.empty()) return 0.0;
        
        double sum = 0.0;
        for (double value : data) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / data.size());
    }
    
    // Environment setup/teardown
    class TestEnvironment {
    public:
        TestEnvironment() {
            Logger::getInstance().setLevel(LogLevel::WARNING); // Reduce noise in tests
        }
        
        ~TestEnvironment() {
            // Cleanup will happen here
        }
    };
    
    // Test session tracking
    class TestSession {
    public:
        TestSession(const std::string& name) : name_(name), startTime_(now()) {}
        
        ~TestSession() {
            double elapsed = elapsedMs(startTime_);
            if (detailsCallback_) {
                detailsCallback_(name_, elapsed, success_);
            }
        }
        
        void setSuccess(bool success) { success_ = success; }
        void setDetailsCallback(std::function<void(const std::string&, double, bool)> callback) {
            detailsCallback_ = callback;
        }
        
        double getElapsedMs() const { return elapsedMs(startTime_); }
        
    private:
        std::string name_;
        std::chrono::steady_clock::time_point startTime_;
        bool success_ = false;
        std::function<void(const std::string&, double, bool)> detailsCallback_;
    };
    
    // Comparison utilities
    static bool doubleEqual(double a, double b, double tolerance = 1e-9) {
        return std::abs(a - b) <= tolerance;
    }
    
    static bool vectorsEqual(const std::vector<double>& a, const std::vector<double>& b,
                             double tolerance = 1e-9) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (!doubleEqual(a[i], b[i], tolerance)) return false;
        }
        return true;
    }
}; // namespace nlm::tests