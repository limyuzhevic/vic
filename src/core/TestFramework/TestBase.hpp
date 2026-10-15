#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <memory>

namespace nlm {

// Test metadata structure
struct TestMetadata {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    std::string phase; // Phase1, Phase2, Phase3, etc.
    std::map<std::string, std::string> parameters;
    bool requiresBrainReset;
    bool runsSequentially;
    std::vector<std::string> dependencies;
    std::chrono::milliseconds maxDuration;
    std::vector<std::string> outputFiles;
    bool enabledByDefault;
};

// Abstract base class for all tests
class TestBase {
public:
    virtual ~TestBase() = default;
    
    // Get test metadata
    virtual const TestMetadata& getMetadata() const = 0;
    
    // Test lifecycle management
    virtual bool setup(std::shared_ptr<Brain> brain, const Config& config) = 0;
    virtual bool execute(std::shared_ptr<Brain> brain, const Config& config) = 0;
    virtual bool teardown(std::shared_ptr<Brain> brain, const Config& config) = 0;
    
    // Get test results
    virtual std::string getResultsSummary() const = 0;
    virtual std::map<std::string, std::string> getMetrics() const = 0;
    
    // Test execution control
    virtual bool shouldRun(const Config& config) const { return true; }
    virtual std::chrono::milliseconds getEstimatedDuration() const { return std::chrono::milliseconds(100); }
    
    // Test identification
    std::string getName() const { return getMetadata().name; }
    std::string getCategory() const { return getMetadata().category; }
    std::string getDescription() const { return getMetadata().description; }
    
protected:
    // Helper method for test results
    template<typename T>
    std::string formatMetric(const std::string& name, T value) const {
        std::stringstream ss;
        ss << name << ": " << value;
        return ss.str();
    }
};

} // namespace nlm
