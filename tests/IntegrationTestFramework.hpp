// Integration Test Framework for NLM Brain Systems
// Phase 6: Comprehensive Integration Tests for All Connected Systems

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>

namespace nlm {

class IntegrationTestFramework {
public:
    IntegrationTestFramework() : config_(std::make_shared<Config>()) {
        setupDefaultConfig();
    }
    
    virtual ~IntegrationTestFramework() = default;
    
    void setupDefaultConfig() {
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config_->set("connection_probability", 0.1, ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("random_seed", uint64_t(42), ConfigSource::Default);
    }
    
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    void logTestStart(const std::string& testName) {
        std::cout << "=== Testing " << testName << " ===" << std::endl;
    }
    
    void logTestResult(const std::string& testName, bool passed, const std::string& details = "") {
        if (passed) {
            std::cout << "PASS: " << testName << std::endl;
            if (!details.empty()) std::cout << "  Details: " << details << std::endl;
        } else {
            std::cout << "FAIL: " << testName << std::endl;
            if (!details.empty()) std::cout << "  Details: " << details << std::endl;
            throw std::runtime_error("Test failed: " + testName);
        }
    }
    
    bool checkFloatEquality(float a, float b, float epsilon = 1e-5f) const {
        return std::abs(a - b) < epsilon;
    }
    
    bool checkApprox(float a, float b, float relativeError = 1e-3f) const {
        if (b == 0.0f) return a == 0.0f;
        return std::abs(a - b) / std::abs(b) < relativeError;
    }
    
protected:
    std::shared_ptr<Config> config_;
};

} // namespace nlm
