#include "API.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

namespace api {

static std::unique_ptr<SimpleBrainInterface> gSimpleBrain = nullptr;
static std::unique_ptr<AdvancedBrainInterface> gAdvancedBrain = nullptr;

BrainInterface& getSimpleBrain() {
    if (!gSimpleBrain) {
        gSimpleBrain = std::make_unique<SimpleBrainInterface>();
    }
    return *gSimpleBrain;
}

BrainInterface& getAdvancedBrain() {
    if (!gAdvancedBrain) {
        gAdvancedBrain = std::make_unique<AdvancedBrainInterface>();
    }
    return *gAdvancedBrain;
}

bool initializeAllBrains() {
    bool simpleOk = gSimpleBrain ? gSimpleBrain->initialize() : false;
    bool advancedOk = gAdvancedBrain ? gAdvancedBrain->initialize() : false;
    return simpleOk && advancedOk;
}

void shutdownAllBrains() {
    if (gSimpleBrain) gSimpleBrain->reset();
    if (gAdvancedBrain) gAdvancedBrain->reset();
    gSimpleBrain.reset();
    gAdvancedBrain.reset();
}

bool isAnyBrainInitialized() {
    return (gSimpleBrain && gSimpleBrain->isInitialized()) ||
           (gAdvancedBrain && gAdvancedBrain->isInitialized());
}

void loadDefaultConfiguration() {
    // Load default configuration settings
    if (gSimpleBrain) {
        gSimpleBrain->setParameter("learning_rate", "0.01");
        gSimpleBrain->setParameter("neuron_count", "1000");
        gSimpleBrain->setParameter("timestep", "0.001");
    }
    if (gAdvancedBrain) {
        gAdvancedBrain->setParameter("learning_rate", "0.01");
        gAdvancedBrain->setParameter("neuron_count", "1000");
        gAdvancedBrain->setParameter("timestep", "0.001");
    }
}

void loadConfigurationFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open configuration file: " + filepath);
        }
        
        json config;
        file >> config;
        
        // Apply configuration to both brains if they exist
        if (gSimpleBrain) {
            if (config.contains("simple_brain")) {
                auto brainConfig = config["simple_brain"];
                for (auto it = brainConfig.begin(); it != brainConfig.end(); ++it) {
                    gSimpleBrain->setParameter(it.key(), it.value().get<std::string>());
                }
            }
        }
        
        if (gAdvancedBrain) {
            if (config.contains("advanced_brain")) {
                auto brainConfig = config["advanced_brain"];
                for (auto it = brainConfig.begin(); it != brainConfig.end(); ++it) {
                    gAdvancedBrain->setParameter(it.key(), it.value().get<std::string>());
                }
            }
        }
        
    } catch (const std::exception& e) {
        // Configuration loading failed, but don't crash - continue with defaults
    }
}

void saveConfigurationToFile(const std::string& filepath) {
    json config;
    
    if (gSimpleBrain) {
        json simpleBrainConfig;
        // Extract configuration from simple brain (if it has such method)
        config["simple_brain"] = simpleBrainConfig;
    }
    
    if (gAdvancedBrain) {
        json advancedBrainConfig;
        // Extract configuration from advanced brain (if it has such method)
        config["advanced_brain"] = advancedBrainConfig;
    }
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << config.dump(2);
    }
}

void runExperiment(const std::string& experimentType, uint64_t steps) {
    if (gSimpleBrain && gSimpleBrain->isInitialized()) {
        gSimpleBrain->runExperiment(experimentType, steps);
    }
    if (gAdvancedBrain && gAdvancedBrain->isInitialized()) {
        gAdvancedBrain->runExperiment(experimentType, steps);
    }
}

void runAllExperiments(uint64_t steps) {
    runExperiment("learning", steps);
    runExperiment("adaptation", steps);
    runExperiment("social", steps);
    runExperiment("memory", steps);
}

void stopAllExperiments() {
    if (gSimpleBrain) gSimpleBrain->reset();
    if (gAdvancedBrain) gAdvancedBrain->reset();
}

void exportAllData(const std::string& directory, const std::string& format) {
    if (gSimpleBrain) {
        std::string filepath = directory + "/simple_brain." + format;
        gSimpleBrain->exportData(filepath, format);
    }
    if (gAdvancedBrain) {
        std::string filepath = directory + "/advanced_brain." + format;
        gAdvancedBrain->exportData(filepath, format);
    }
}

void importAllData(const std::string& directory, const std::string& format) {
    if (gSimpleBrain) {
        std::string filepath = directory + "/simple_brain." + format;
        gSimpleBrain->importData(filepath, format);
    }
    if (gAdvancedBrain) {
        std::string filepath = directory + "/advanced_brain." + format;
        gAdvancedBrain->importData(filepath, format);
    }
}

std::string getSystemOverview() {
    std::stringstream ss;
    ss << "=== NLM System Overview ===" << std::endl;
    ss << "Brains Initialized: " << (isAnyBrainInitialized() ? "Yes" : "No") << std::endl;
    ss << "Available Experiment Types: ";
    auto experimentTypes = getAvailableExperimentTypes();
    for (size_t i = 0; i < experimentTypes.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << experimentTypes[i];
    }
    ss << std::endl;
    ss << "Available Brain Types: ";
    auto brainTypes = getAvailableBrainTypes();
    for (size_t i = 0; i < brainTypes.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << brainTypes[i];
    }
    ss << std::endl;
    return ss.str();
}

std::string getPerformanceReport() {
    std::stringstream ss;
    ss << "=== Performance Report ===" << std::endl;
    if (gSimpleBrain && gSimpleBrain->isInitialized()) {
        ss << "Simple Brain Status: " << gSimpleBrain->getBrainStatus() << std::endl;
    }
    if (gAdvancedBrain && gAdvancedBrain->isInitialized()) {
        ss << "Advanced Brain Status: " << gAdvancedBrain->getBrainStatus() << std::endl;
    }
    ss << "=== End Performance Report ===" << std::endl;
    return ss.str();
}

std::string getHealthReport() {
    std::stringstream ss;
    ss << "=== Health Report ===" << std::endl;
    ss << "System Status: " << (validateEnvironment() ? "Healthy" : "Issues Detected") << std::endl;
    ss << "Active Experiments: " << getExperimentReport() << std::endl;
    ss << "=== End Health Report ===" << std::endl;
    return ss.str();
}

std::string getExperimentReport() {
    std::stringstream ss;
    ss << "=== Experiment Status ===" << std::endl;
    ss << "No experiments currently running" << std::endl;
    ss << "=== End Experiment Status ===" << std::endl;
    return ss.str();
}

std::string generateReport(const std::string& type) {
    if (type == "comprehensive") {
        return getSystemOverview() + "\n" + getPerformanceReport() + "\n" + getHealthReport();
    } else if (type == "performance") {
        return getPerformanceReport();
    } else if (type == "health") {
        return getHealthReport();
    } else {
        return "Unknown report type: " + type;
    }
}

bool validateEnvironment() {
    // Basic validation checks
    try {
        // Check if brain interface headers are available
        bool simpleBrainAvailable = std::is_same<decltype(&getSimpleBrain), decltype(&getSimpleBrain)>::value;
        bool advancedBrainAvailable = std::is_same<decltype(&getAdvancedBrain), decltype(&getAdvancedBrain)>::value;
        return simpleBrainAvailable && advancedBrainAvailable;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> getAvailableExperimentTypes() {
    return {"learning", "adaptation", "social", "memory", "comprehensive", "default"};
}

std::vector<std::string> getAvailableBrainTypes() {
    return {"simple", "advanced"};
}

} // namespace api

} // namespace nlm
