// NLM (熙然) - Neural Learning Machine
// DemoRunner Class
//
// This class provides a unified interface for managing and running
// different NLM demos with improved modularity and configurability.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

// Demo types supported by NLM
enum class DemoType {
    UNKNOWN,
    PHASE3_DEMO,
    PHASE4_DEMO,
    PHASE5_DEMO,
    PHASE6_DEMO,
    PHASE6_TEXT_DEMO
};

// Demo configuration structure
struct DemoConfig {
    DemoType type;
    std::string name;
    std::map<std::string, std::string> parameters;
    std::shared_ptr<Config> config;
    bool enabled;
    
    DemoConfig() : type(DemoType::UNKNOWN), enabled(false) {}
    DemoConfig(DemoType t, const std::string& n) : type(t), name(n), enabled(true) {}
};

// Demo runner class
class DemoRunner {
public:
    DemoRunner();
    ~DemoRunner() = default;
    
    // Register a demo factory function
    void registerDemo(DemoType type, const std::string& name,
                     std::function<std::shared_ptr<DemoRunner>(std::shared_ptr<Config>)> factory);
    
    // Create and configure a demo
    bool createDemo(DemoType type, std::shared_ptr<Config> config);
    
    // Run the current demo
    bool runDemo();
    
    // Stop the demo
    void stopDemo();
    
    // Get demo type
    DemoType getDemoType() const { return currentDemoType; }
    
    // Get demo name
    const std::string& getDemoName() const { return currentDemoName; }
    
    // Check if demo is running
    bool isDemoRunning() const { return demoRunning; }
    
    // Get demo status
    std::string getDemoStatus() const;
    
    // Add parameter to current demo
    void addParameter(const std::string& key, const std::string& value);
    
    // Get parameter value
    std::string getParameter(const std::string& key, const std::string& defaultValue = "") const;
    
    // List available demos
    std::vector<DemoType> getAvailableDemoTypes() const;
    
    // Get demo type from string
    static DemoType getDemoTypeFromString(const std::string& str);
    
    // Get string from demo type
    static std::string getStringFromDemoType(DemoType type);
    
    // Set verbose logging
    void setVerbose(bool verbose) { this->verbose = verbose; }
    bool isVerbose() const { return verbose; }
    
private:
    // Current demo state
    DemoType currentDemoType;
    std::string currentDemoName;
    bool demoRunning;
    bool verbose;
    
    // Registered demo factories
    std::map<DemoType, std::function<std::shared_ptr<DemoRunner>(std::shared_ptr<Config>)>> demoFactories;
    
    // Current demo instance
    std::shared_ptr<DemoRunner> currentDemo;
    
    // Demo parameters
    std::map<std::string, std::string> demoParameters;
    
    // Internal demo implementations
    class Phase3DemoWrapper;
    class Phase4DemoWrapper;
    class Phase5DemoWrapper;
    class Phase6DemoWrapper;
    class Phase6TextDemoWrapper;
};

} // namespace nlm