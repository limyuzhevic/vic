#include "DemoRunner.hpp"
#include "experiments/Phase3Demo.cpp"  // Include the actual demo implementation
#include "experiments/Phase4Demo.cpp"
#include "experiments/Phase5Demo.cpp"
#include "experiments/Phase6Demo.cpp"
#include "experiments/Phase6TextDemo.cpp"
#include <iostream>
#include <algorithm>

namespace nlm {

DemoRunner::DemoRunner() 
    : currentDemoType(DemoType::UNKNOWN), demoRunning(false), verbose(false), currentDemo(nullptr) {
}

void DemoRunner::registerDemo(DemoType type, const std::string& name,
                             std::function<std::shared_ptr<DemoRunner>(std::shared_ptr<Config>)> factory) {
    demoFactories[type] = factory;
    if (verbose) {
        std::cout << "Registered demo: " << name << " (type: " << getStringFromDemoType(type) << ")" << std::endl;
    }
}

bool DemoRunner::createDemo(DemoType type, std::shared_ptr<Config> config) {
    if (demoRunning) {
        std::cerr << "Error: A demo is already running." << std::endl;
        return false;
    }
    
    // Check if demo is registered
    auto it = demoFactories.find(type);
    if (it == demoFactories.end()) {
        std::cerr << "Error: Demo type " << getStringFromDemoType(type) << " is not registered." << std::endl;
        return false;
    }
    
    // Create demo instance
    currentDemo = it->second(config);
    currentDemoType = type;
    currentDemoName = "";
    
    if (verbose) {
        std::cout << "Created demo instance for " << getStringFromDemoType(type) << std::endl;
    }
    
    return true;
}

bool DemoRunner::runDemo() {
    if (!currentDemo) {
        std::cerr << "Error: No demo created." << std::endl;
        return false;
    }
    
    if (demoRunning) {
        std::cerr << "Error: Demo is already running." << std::endl;
        return false;
    }
    
    demoRunning = true;
    
    if (verbose) {
        std::cout << "Starting demo: " << currentDemoName << std::endl;
    }
    
    // Run the demo
    bool success = currentDemo->runDemo();
    
    demoRunning = false;
    
    if (verbose) {
        std::cout << "Demo completed: " << (success ? "success" : "failure") << std::endl;
    }
    
    return success;
}

void DemoRunner::stopDemo() {
    if (demoRunning) {
        demoRunning = false;
        if (verbose) {
            std::cout << "Demo stopped by user." << std::endl;
        }
    }
}

std::string DemoRunner::getDemoStatus() const {
    if (!currentDemo) {
        return "No demo created";
    }
    
    if (demoRunning) {
        return "Running";
    } else {
        return "Stopped";
    }
}

void DemoRunner::addParameter(const std::string& key, const std::string& value) {
    demoParameters[key] = value;
    
    if (verbose) {
        std::cout << "Added parameter: " << key << " = " << value << std::endl;
    }
}

std::string DemoRunner::getParameter(const std::string& key, const std::string& defaultValue) const {
    auto it = demoParameters.find(key);
    if (it != demoParameters.end()) {
        return it->second;
    }
    return defaultValue;
}

std::vector<DemoType> DemoRunner::getAvailableDemoTypes() const {
    std::vector<DemoType> types;
    for (const auto& pair : demoFactories) {
        types.push_back(pair.first);
    }
    return types;
}

DemoType DemoRunner::getDemoTypeFromString(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "phase3" || lowerStr == "phase3_demo") {
        return DemoType::PHASE3_DEMO;
    } else if (lowerStr == "phase4" || lowerStr == "phase4_demo") {
        return DemoType::PHASE4_DEMO;
    } else if (lowerStr == "phase5" || lowerStr == "phase5_demo") {
        return DemoType::PHASE5_DEMO;
    } else if (lowerStr == "phase6" || lowerStr == "phase6_demo") {
        return DemoType::PHASE6_DEMO;
    } else if (lowerStr == "phase6_text" || lowerStr == "phase6_text_demo") {
        return DemoType::PHASE6_TEXT_DEMO;
    } else {
        return DemoType::UNKNOWN;
    }
}

std::string DemoRunner::getStringFromDemoType(DemoType type) {
    switch (type) {
        case DemoType::PHASE3_DEMO: return "phase3_demo";
        case DemoType::PHASE4_DEMO: return "phase4_demo";
        case DemoType::PHASE5_DEMO: return "phase5_demo";
        case DemoType::PHASE6_DEMO: return "phase6_demo";
        case DemoType::PHASE6_TEXT_DEMO: return "phase6_text_demo";
        case DemoType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

} // namespace nlm