// Test Framework Base Class
// This provides the foundation for all NLM brain simulation tests

#pragma once

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace nlm {
namespace tests {

class TestFramework {
public:
    struct TestResult {
        std::string name;
        bool passed = false;
        double duration_ms = 0.0;
        std::string message;
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        float performanceScore = 0.0f;
    };

    TestFramework() {
        // Initialize logger for tests
        logger = std::make_shared<Logger>();
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Test Framework initialized");
    }
    
    virtual ~TestFramework() {
        NLM_LOG_INFO("Test Framework shutdown");
    }

    // Test base class
    class Test {
    public:
        virtual ~Test() = default;
        virtual std::string getName() const = 0;
        virtual bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) = 0;
        virtual void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {}
        virtual void teardown(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {}
        
    protected:
        template<typename T>
        T getConfigValue(const std::string& key, const T& defaultValue) {
            return config->getOr<T>(key, defaultValue);
        }
        
        void logInfo(const std::string& message) {
            NLM_LOG_INFO("  [" + getName() + "] " + message);
        }
        
        void logSuccess(const std::string& message) {
            NLM_LOG_INFO("  [" + getName() + "] ✓ " + message);
        }
        
        void logFailure(const std::string& message) {
            NLM_LOG_INFO("  [" + getName() + "] ✗ " + message);
        }
        
        void logWarning(const std::string& message) {
            NLM_LOG_INFO("  [" + getName() + "] ! " + message);
        }
        
        void setConfig(std::shared_ptr<Config> cfg) {
            config = cfg;
        }
        
    protected:
        std::shared_ptr<Config> config;
    };

    // Run a single test
    TestResult runTest(Test* test, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        TestResult result;
        result.name = test->getName();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup
            test->setup(brain, config);
            
            // Record initial state
            if (brain) {
                result.neuronCount = brain->getTotalNeuronCount();
                result.synapseCount = brain->getTotalSynapseCount();
            }
            
            // Run test
            result.passed = test->run(brain, config);
            
            // Teardown
            test->teardown(brain, config);
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = "Exception: " + std::string(e.what());
            logFailure(result.message);
        } catch (...) {
            result.passed = false;
            result.message = "Unknown exception";
            logFailure(result.message);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.duration_ms = static_cast<double>(duration.count());
        
        // Calculate performance score
        if (result.neuronCount > 0 && result.synapseCount > 0) {
            // Simple performance metric: operations per millisecond
            result.performanceScore = (result.neuronCount + result.synapseCount * 10) / 
                                     (static_cast<float>(result.duration_ms) + 0.001f);
        }
        
        // Log result
        if (result.passed) {
            logSuccess("Test passed in " + std::to_string(result.duration_ms) + "ms");
            logSuccess("Performance: " + std::to_string(result.performanceScore) + " ops/ms");
        } else {
            logFailure("Test failed after " + std::to_string(result.duration_ms) + "ms");
            if (!result.message.empty()) {
                logFailure("Details: " + result.message);
            }
        }
        
        return result;
    }
    
    // Helper for parameterized tests
    template<typename T, typename... Args>
    void runParameterizedTest(const std::string& testName, 
                             std::function<bool(T, Args...)> testFunc,
                             const std::vector<T>& params,
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<Config> config) {
        
        for (size_t i = 0; i < params.size(); ++i) {
            NLM_LOG_INFO("Running parameter test: " + testName + " (param " + 
                        std::to_string(i) + ")");
            
            TestResult result;
            result.name = testName + "_param_" + std::to_string(i);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                result.passed = testFunc(params[i], std::get<0>(std::make_tuple(std::declval<Args>(), ...))...);
                if (result.passed) {
                    logSuccess("Param test passed");
                } else {
                    logFailure("Param test failed");
                }
            } catch (const std::exception& e) {
                result.passed = false;
                result.message = "Exception: " + std::string(e.what());
                logFailure(result.message);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            result.duration_ms = static_cast<double>(duration.count());
            
            // Store or report result...
        }
    }

private:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<ConsoleLogger> consoleLogger;
};

} // namespace tests
} // namespace nlm
