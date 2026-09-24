// Performance test base classes
// Abstract base classes for different types of performance tests

#pragma once

#include "PerformanceTestFramework.hpp"
#include "BenchmarkUtils.hpp"
#include <memory>
#include <string>
#include <vector>

// Abstract base class for performance tests
class PerformanceTest {
public:
    virtual ~PerformanceTest() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getComponent() const = 0;
    virtual void runSmallScale() = 0;
    virtual void runMediumScale() = 0;
    virtual void runLargeScale() = 0;
    virtual void setup() {}
    virtual void cleanup() {}
    virtual void configure(const std::string& configFile = "") {}
};

// Performance Test Factory
class PerformanceTestFactory {
public:
    static std::vector<std::unique_ptr<PerformanceTest>> createAllTests(
        PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::vector<std::unique_ptr<PerformanceTest>> tests;
        
        tests.push_back(std::make_unique<LIFNeuronPerformanceTest>(framework, collector));
        tests.push_back(std::make_unique<SynapticTransmissionPerformanceTest>(framework, collector));
        tests.push_back(std::make_unique<STDPPlasticityPerformanceTest>(framework, collector));
        
        return tests;
    }
};
