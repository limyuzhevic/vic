#include "NeuromodulatorFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

std::unordered_map<std::string, NeuromodulatorFactory::NeuromodulatorCreator> NeuromodulatorFactory::builtInCreators_;
std::vector<NeuromodulatorFactory::NeuromodulatorInfo> NeuromodulatorFactory::builtInTypes_;
std::mutex NeuromodulatorFactory::mutex_;
std::unordered_set<std::string> NeuromodulatorFactory::loadedPlugins_;

// Static initializer to load built-in neuromodulators
static void initializeNeuromodulatorFactory() {
    NeuromodulatorFactory::loadBuiltInNeuromodulators();
}

// Initialize on first use
void ensureInitialized() {
    static bool initialized = []() {
        initializeNeuromodulatorFactory();
        return true;
    }();
}

void NeuromodulatorFactory::loadBuiltInNeuromodulators() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!builtInCreators_.empty()) {
        return; // Already loaded
    }
    
    // Register all built-in neuromodulators
    registerBuiltInCreator("Dopamine", 
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto neuromodulator = std::make_unique<Dopamine>();
            return neuromodulator;
        },
        "Dopamine: Reward and reinforcement learning signal",
        false);
    
    registerBuiltInCreator("Curiosity",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto neuromodulator = std::make_unique<Curiosity>();
            return neuromodulator;
        },
        "Curiosity: Exploration motivation based on novelty and prediction error",
        false);
    
    registerBuiltInCreator("Novelty",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto neuromodulator = std::make_unique<Novelty>();
            return neuromodulator;
        },
        "Novelty: Detection of new or unexpected sensory input",
        false);
    
    registerBuiltInCreator("PredictionError",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto neuromodulator = std::make_unique<PredictionError>();
            return neuromodulator;
        },
        "PredictionError: Difference between predicted and actual values",
        false);
    
    // Register stub neuromodulators for future extension
    registerBuiltInCreator("Acetylcholine",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            return createStub("Acetylcholine", config);
        },
        "Acetylcholine: Attention and memory consolidation",
        true);
    
    registerBuiltInCreator("Norepinephrine",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            return createStub("Norepinephrine", config);
        },
        "Norepinephrine: Arousal and vigilance",
        true);
    
    registerBuiltInCreator("Serotonin",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            return createStub("Serotonin", config);
        },
        "Serotonin: Mood, impulsivity, and social behavior",
        true);
}

void NeuromodulatorFactory::registerBuiltInCreator(const std::string& typeName,
                                                   NeuromodulatorCreator creator,
                                                   const std::string& description,
                                                   bool isStub) {
    registerCreator(typeName, creator, description, isStub);
}

void NeuromodulatorFactory::registerCreator(const std::string& typeName,
                                            NeuromodulatorCreator creator,
                                            const std::string& description,
                                            bool isStub) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    builtInCreators_[typeName] = std::move(creator);
    
    NeuromodulatorInfo info;
    info.name = typeName;
    info.description = description;
    info.isBuiltIn = !isStub;
    info.isStub = isStub;
    
    // Add common config options
    info.configOptions["enabled"] = "Enable/disable this neuromodulator";
    info.configOptions["level"] = "Initial level (0.0-1.0)";
    
    builtInTypes_.push_back(std::move(info));
    
    NLM_LOG_INFO("Registered neuromodulator: " + typeName + " - " + description);
}

std::unique_ptr<Neuromodulator> NeuromodulatorFactory::create(const std::string& neuromodulatorType,
                                                             std::shared_ptr<Config> config,
                                                             TimestepDuration timestep) {
    ensureInitialized();
    
    std::string typeName = neuromodulatorType;
    // Convert to lowercase for case-insensitive comparison
    std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = builtInCreators_.find(typeName);
    if (it == builtInCreators_.end()) {
        throw std::runtime_error("Unknown neuromodulator type: " + neuromodulatorType);
    }
    
    try {
        return it->second(config, timestep);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create neuromodulator " + neuromodulatorType + ": " + std::string(e.what()));
    }
}

bool NeuromodulatorFactory::hasType(const std::string& neuromodulatorType) {
    ensureInitialized();
    
    std::string typeName = neuromodulatorType;
    std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
    
    std::lock_guard<std::mutex> lock(mutex_);
    return builtInCreators_.find(typeName) != builtInCreators_.end();
}

bool NeuromodulatorFactory::isStub(const std::string& neuromodulatorType) {
    ensureInitialized();
    
    for (const auto& info : builtInTypes_) {
        if (info.name == neuromodulatorType) {
            return info.isStub;
        }
    }
    return false;
}

void NeuromodulatorFactory::registerPlugin(const std::string& pluginName, 
                                           PluginRegistration registrationFunc) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (loadedPlugins_.find(pluginName) != loadedPlugins_.end()) {
        NLM_LOG_WARN("Plugin already loaded: " + pluginName);
        return;
    }
    
    try {
        registrationFunc(*this);
        loadedPlugins_.insert(pluginName);
        NLM_LOG_INFO("Loaded plugin: " + pluginName);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load plugin " + pluginName + ": " + std::string(e.what()));
    }
}

void NeuromodulatorFactory::initializePlugins() {
    ensureInitialized();
    // Plugins would be automatically initialized when registered
    NLM_LOG_INFO("Neuromodulator plugin system initialized");
}

std::unique_ptr<Neuromodulator> NeuromodulatorFactory::createWithFallback(const std::string& neuromodulatorType,
                                                                          std::shared_ptr<Config> config,
                                                                          TimestepDuration timestep) {
    try {
        return create(neuromodulatorType, config, timestep);
    } catch (const std::runtime_error& e) {
        NLM_LOG_WARN(std::string("Failed to create neuromodulator ") + neuromodulatorType + 
                   ". Using stub implementation: " + std::string(e.what()));
        
        // Try to create a stub if available
        std::string typeName = neuromodulatorType;
        std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = builtInCreators_.find(typeName);
        if (it != builtInCreators_.end()) {
            // Check if it's a stub creator
            for (const auto& info : builtInTypes_) {
                if (info.name == typeName && info.isStub) {
                    return it->second(config, timestep);
                }
            }
        }
        
        // If no stub found, create a generic placeholder
        return createStub(neuromodulatorType, config);
    }
}

std::unique_ptr<Neuromodulator> NeuromodulatorFactory::createStub(const std::string& typeName,
                                                                 std::shared_ptr<Config> config) {
    auto neuromodulator = std::make_unique<Neuromodulator>();
    neuromodulator->setName("Generic Stub " + typeName);
    neuromodulator->setLevel(0.0f);
    NLM_LOG_INFO("Created stub neuromodulator: " + typeName);
    return neuromodulator;
}

std::vector<NeuromodulatorFactory::NeuromodulatorInfo> NeuromodulatorFactory::getAvailableTypes() {
    ensureInitialized();
    
    std::lock_guard<std::mutex> lock(mutex_);
    return builtInTypes_;
}

} // namespace nlm