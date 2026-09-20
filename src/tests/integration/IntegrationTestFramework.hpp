// Integration Test Framework
// Comprehensive integration testing framework for NLM
// Tests multiple systems simultaneously for system integration validation

#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cassert>
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

class IntegrationTestResult {
public:
    struct TestCase {
        std::string name;
        std::string category;
        std::chrono::microseconds duration;
        bool passed;
        std::string details;
        double performanceScore;
    };
    
    struct PerformanceMetrics {
        std::chrono::microseconds totalDuration;
        double cpuUsage;
        double memoryUsage;
        size_t operationsPerSecond;
        std::map<std::string, double> subsystemMetrics;
    };
    
    IntegrationTestResult();
    
    void recordTest(const std::string& name, const std::string& category,
                   bool passed, const std::string& details = "",
                   double performanceScore = 0.0);
    
    void recordPerformance(const PerformanceMetrics& metrics);
    
    void saveResults(const std::string& filepath) const;
    
    void printResults() const;
    
    size_t getTestCount() const;
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    double getAveragePerformanceScore() const;
    
    const std::vector<TestCase>& getTestCases() const { return testCases; }
    const PerformanceMetrics& getPerformanceMetrics() const { return metrics; }
    
    static std::string getTimestamp();
    
private:
    std::vector<TestCase> testCases;
    PerformanceMetrics metrics;
    std::map<std::string, std::string> metadata;
};

class IntegrationTestBase {
protected:
    std::shared_ptr<Config> config;
    std::unique_ptr<Brain> brain;
    IntegrationTestResult& results;
    
    explicit IntegrationTestBase(std::shared_ptr<Config> cfg, IntegrationTestResult& testResults)
        : config(cfg), results(testResults) {}
    
    void setupSystem() {
        brain = std::make_unique<Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
    }
    
    void teardownSystem() {
        brain.reset();
    }
    
    template<typename TestFunc>
    void runTestCase(const std::string& name, const std::string& category,
                    TestFunc&& testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        bool passed = false;
        std::string details;
        
        try {
            testFunc();
            passed = true;
            details = "Test passed";
        } catch (const std::exception& e) {
            details = std::string("Exception: ") + e.what();
        } catch (...) {
            details = "Unknown error occurred";
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        double performanceScore = 1.0 / (1.0 + duration.count() / 1000000.0);
        
        results.recordTest(name, category, passed, details, performanceScore);
    }
};

} // namespace nlm