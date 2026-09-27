// Enhanced Test Fixture for NLM Brain Simulation
// Modern C++17/20 test fixture with setup/teardown support

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <atomic>

namespace nlm {
namespace test {

// Enhanced Test fixture base class with resource management
class TestFixture {
protected:
    std::string fixtureName;
    bool setupCalled = false;
    bool teardownCalled = false;
    std::vector<std::function<void()>> resources;
    mutable std::mutex resourcesMutex;
    
public:
    TestFixture(const std::string& name) : fixtureName(name) {}
    virtual ~TestFixture() {
        // Cleanup resources
        for (auto& cleanup : resources) {
            try { cleanup(); } catch (...) {}
        }
        resources.clear();
    }
    
    const std::string& getFixtureName() const { return fixtureName; }
    
    virtual void setup() {
        std::lock_guard<std::mutex> lock(resourcesMutex);
        setupCalled = true;
    }
    
    virtual void teardown() {
        std::lock_guard<std::mutex> lock(resourcesMutex);
        teardownCalled = true;
    }
    
    void registerResourceCleanup(std::function<void()> cleanup) {
        std::lock_guard<std::mutex> lock(resourcesMutex);
        resources.push_back(cleanup);
    }
    
    bool isSetupCalled() const { return setupCalled; }
    bool isTeardownCalled() const { return teardownCalled; }
    
    // Utility methods for test fixtures
    template<typename T>
    T& getFixtureData(const std::string& key, T defaultValue) {
        static std::map<std::string, T> fixtureData;
        auto it = fixtureData.find(key);
        if (it != fixtureData.end()) {
            return it->second;
        }
        return fixtureData[key] = defaultValue;
    }
    
    void setFixtureData(const std::string& key, const std::string& value) {
        static std::map<std::string, std::string> fixtureData;
        fixtureData[key] = value;
    }
    
    std::string getFixtureData(const std::string& key, const std::string& defaultValue = "") {
        static std::map<std::string, std::string> fixtureData;
        auto it = fixtureData.find(key);
        return it != fixtureData.end() ? it->second : defaultValue;
    }
};

// Test parameter management for parameterized tests
class TestParameters {
public:
    TestParameters() = default;
    
    void addParameter(const std::string& name, const std::string& value) {
        parameters[name] = value;
    }
    
    std::string getParameter(const std::string& name, const std::string& defaultValue = "") const {
        auto it = parameters.find(name);
        return it != parameters.end() ? it->second : defaultValue;
    }
    
    int getParameterInt(const std::string& name, int defaultValue = 0) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {}
        }
        return defaultValue;
    }
    
    float getParameterFloat(const std::string& name, float defaultValue = 0.0f) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            try {
                return std::stof(it->second);
            } catch (...) {}
        }
        return defaultValue;
    }
    
    bool getParameterBool(const std::string& name, bool defaultValue = false) const {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            std::string value = it->second;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return value == "true" || value == "1" || value == "yes";
        }
        return defaultValue;
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters.find(name) != parameters.end();
    }
    
    const std::map<std::string, std::string>& getAllParameters() const {
        return parameters;
    }
    
    void clear() {
        parameters.clear();
    }
    
private:
    std::map<std::string, std::string> parameters;
};

// Parameterized test support
class ParameterizedTestBase : public TestFixture {
protected:
    template<typename T>
    class ParameterSet {
    public:
        std::string name;
        T value;
        
        ParameterSet(const std::string& name, const T& value) : name(name), value(value) {}
    };
    
public:
    ParameterizedTestBase(const std::string& name) : TestFixture(name) {}
    
    virtual void runTestWithParameter(const std::string& paramName, const std::string& paramValue) = 0;
};

// Test dependency management
class TestDependency {
public:
    TestDependency(const std::string& name, std::function<bool()> checkFunction)
        : name(name), checkFunction(checkFunction) {}
    
    const std::string& getName() const { return name; }
    
    bool check() const {
        return checkFunction();
    }
    
private:
    std::string name;
    std::function<bool()> checkFunction;
};

class TestDependencies {
public:
    void addDependency(const std::string& name, std::function<bool()> checkFunction) {
        dependencies.emplace_back(name, checkFunction);
    }
    
    bool allSatisfied() const {
        for (const auto& dependency : dependencies) {
            if (!dependency.check()) {
                return false;
            }
        }
        return true;
    }
    
    std::vector<std::string> getUnsatisfiedDependencies() const {
        std::vector<std::string> unsatisfied;
        for (const auto& dependency : dependencies) {
            if (!dependency.check()) {
                unsatisfied.push_back(dependency.getName());
            }
        }
        return unsatisfied;
    }
    
private:
    std::vector<TestDependency> dependencies;
};

} // namespace test
} // namespace nlm
