// demo_framework.hpp - Base framework for all NLM phase demos
//
// This provides a common base class and utility functions for all phase demos,
// reducing code duplication and improving maintainability.

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <functional>

using namespace nlm;

// Demo framework base class
class BaseDemo {
public:
    virtual ~BaseDemo() = default;
    
    // Run the demo
    bool run(const Config& config) {
        onInitialize(config);
        return onRun(config);
    }
    
    // Get demo name
    virtual std::string getName() const = 0;
    
    // Get demo description
    virtual std::string getDescription() const = 0;
    
    // Get demo version
    virtual std::string getVersion() const { return "1.0"; }
    
    // Check if demo supports the given configuration
    virtual bool supportsConfig(const Config& config) const { return true; }
    
protected:
    // Override these methods in derived classes
    virtual void onInitialize(const Config& config) {
        // Optional initialization logic
    }
    
    virtual bool onRun(const Config& config) = 0;
    
    // Utility functions for derived classes
    void logInfo(const std::string& message) const {
        NLM_LOG_INFO(message);
    }
    
    void logError(const std::string& message) const {
        NLM_LOG_ERROR(message);
    }
    
    void logWarning(const std::string& message) const {
        NLM_LOG_WARNING(message);
    }
    
    // Get configuration value with type safety
    template<typename T>
    T getConfig(const std::string& key, const T& defaultValue = T{}) const {
        auto value = config_.get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    // Validate configuration
    bool validateConfig(const std::string& key, 
                       std::function<bool(const ConfigValue&)> validator) const {
        if (!config_.has(key)) return false;
        
        // Get the config entry
        auto it = std::find_if(config_.pImpl->entries.begin(), config_.pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == config_.pImpl->entries.end()) return false;
        
        return validator(it->value);
    }
    
    // Set the config reference (called by framework)
    void setConfig(const Config& config) {
        config_ = config;
    }
    
    // Measure execution time
    template<typename Func>
    double measureTime(Func&& func, const std::string& operationName = "operation") {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(end - start).count();
        logInfo("" + operationName + " completed in " + std::to_string(elapsed) + " seconds");
        return elapsed;
    }
    
    // Safe sleep for demo timing
    void sleep(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }
    
    // Print configuration summary
    void printConfigSummary() const {
        logInfo("Configuration summary:");
        for (const auto& key : config_.getKeys()) {
            auto value = config_.get<std::string>(key);
            if (value.has_value()) {
                logInfo("  " + key + ": " + value.value());
            }
        }
    }
    
private:
    Config config_;
};

// Macro for easy demo class definition
#define DEFINE_DEMO_BASE(ClassName, DemoName, DemoDescription) \
class ClassName : public BaseDemo { \
public: \
    std::string getName() const override { return DemoName; } \
    std::string getDescription() const override { return DemoDescription; } \
};

// Demo factory for easy registration
class DemoFactory {
public:
    using DemoCreator = std::function<std::unique_ptr<BaseDemo>()>;
    
    static DemoFactory& getInstance() {
        static DemoFactory instance;
        return instance;
    }
    
    // Register a demo
    void registerDemo(const std::string& name, DemoCreator creator) {
        demoCreators_[name] = creator;
    }
    
    // Get a demo creator
    DemoCreator getDemoCreator(const std::string& name) const {
        auto it = demoCreators_.find(name);
        if (it != demoCreators_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Get all registered demo names
    std::vector<std::string> getRegisteredDemoNames() const {
        std::vector<std::string> names;
        for (const auto& pair : demoCreators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    DemoFactory() = default;
    std::map<std::string, DemoCreator> demoCreators_;
};

// Convenience macro for registering demos
#define REGISTER_DEMO(DemoClass, DemoName) \
    namespace { \
        struct DemoRegistration_##DemoName { \
            DemoRegistration_##DemoName() { \
                DemoFactory::getInstance().registerDemo( \
                    #DemoName, \
                    []() { return std::make_unique<DemoClass>(); } \
                ); \
            } \
        }; \
        DemoRegistration_##DemoName demoRegistrationInstance_##DemoName; \
    }
