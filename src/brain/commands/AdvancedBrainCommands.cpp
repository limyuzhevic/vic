#include "AdvancedBrainCommands.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "ExtensionSystem.hpp"
#include "ExtensionManager.hpp"
#include "factory/ComponentFactory.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

// Implementation of AdvancedBrainCommand derived classes

bool CreateNeuromodulatorCommand::execute(const std::vector<std::string>& args, 
                                         std::shared_ptr<Config> config,
                                         std::shared_ptr<Brain> brain) {
    try {
        // Parse arguments
        std::string name;
        std::string type;
        float level = 0.5f;
        float decayRate = 0.01f;
        
        if (args.size() >= 2) name = args[0];
        if (args.size() >= 3) type = args[1];
        if (args.size() >= 4) level = std::stof(args[2]);
        if (args.size() >= 5) decayRate = std::stof(args[3]);
        
        if (name.empty() || type.empty()) {
            NLM_LOG_ERROR("Create neuromodulator: name and type are required");
            return false;
        }
        
        // Create neuromodulator using component factory
        std::unordered_map<std::string, std::string> params;
        params["type"] = type;
        params["level"] = std::to_string(level);
        params["decay_rate"] = std::to_string(decayRate);
        
        auto neuromod = ComponentFactory<Neuromodulator>::create(type, config, brain, params);
        
        if (!neuromod) {
            NLM_LOG_ERROR("Create neuromodulator: failed to create neuromodulator of type: " + type);
            return false;
        }
        
        // Configure the neuromodulator
        neuromod->setLevel(level);
        neuromod->setDecayRate(decayRate);
        neuromod->initialize(brain.get());
        
        NLM_LOG_INFO("Created neuromodulator: " + name + " of type: " + type + " with level: " + std::to_string(level));
        
        // Store in extension system if available
        auto& extensionSystem = ExtensionSystem::getInstance();
        extensionSystem.emitEvent("neuromodulator_created", static_cast<void*>(neuromod.get()), name);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in create_neuromodulator: " + e.what()));
        return false;
    }
}

bool CreateMemorySystemCommand::execute(const std::vector<std::string>& args, 
                                       std::shared_ptr<Config> config,
                                       std::shared_ptr<Brain> brain) {
    try {
        // Parse arguments
        std::string name;
        std::string type;
        size_t capacity = 1000;
        size_t maxEpisodes = 1000;
        
        if (args.size() >= 2) name = args[0];
        if (args.size() >= 3) type = args[1];
        if (args.size() >= 4) capacity = std::stoul(args[2]);
        if (args.size() >= 5) maxEpisodes = std::stoul(args[3]);
        
        if (name.empty() || type.empty()) {
            NLM_LOG_ERROR("Create memory system: name and type are required");
            return false;
        }
        
        // Create memory system using component factory
        std::unordered_map<std::string, std::string> params;
        params["type"] = type;
        params["capacity"] = std::to_string(capacity);
        params["max_episodes"] = std::to_string(maxEpisodes);
        
        auto memory = ComponentFactory<MemorySystem>::create(type, config, brain, params);
        
        if (!memory) {
            NLM_LOG_ERROR("Create memory system: failed to create memory system of type: " + type);
            return false;
        }
        
        // Initialize the memory system
        memory->initialize(brain.get());
        
        NLM_LOG_INFO("Created memory system: " + name + " of type: " + type + 
                    " with capacity: " + std::to_string(capacity));
        
        // Store in extension system if available
        auto& extensionSystem = ExtensionSystem::getInstance();
        extensionSystem.emitEvent("memory_system_created", static_cast<void*>(memory.get()), name);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in create_memory_system: " + e.what()));
        return false;
    }
}

bool CreateCognitiveFunctionCommand::execute(const std::vector<std::string>& args, 
                                            std::shared_ptr<Config> config,
                                            std::shared_ptr<Brain> brain) {
    try {
        // Parse arguments
        std::string name;
        std::string type;
        int planningDepth = 5;
        float inhibitionStrength = 0.5f;
        float excitationStrength = 1.5f;
        
        if (args.size() >= 2) name = args[0];
        if (args.size() >= 3) type = args[1];
        if (args.size() >= 4) planningDepth = std::stoi(args[2]);
        if (args.size() >= 5) inhibitionStrength = std::stof(args[3]);
        if (args.size() >= 6) excitationStrength = std::stof(args[4]);
        
        if (name.empty() || type.empty()) {
            NLM_LOG_ERROR("Create cognitive function: name and type are required");
            return false;
        }
        
        // Create cognitive function using component factory
        std::unordered_map<std::string, std::string> params;
        params["type"] = type;
        params["planning_depth"] = std::to_string(planningDepth);
        params["inhibition_strength"] = std::to_string(inhibitionStrength);
        params["excitation_strength"] = std::to_string(excitationStrength);
        
        auto cognition = ComponentFactory<CognitiveFunction>::create(type, config, brain, params);
        
        if (!cognition) {
            NLM_LOG_ERROR("Create cognitive function: failed to create cognitive function of type: " + type);
            return false;
        }
        
        // Initialize the cognitive function
        cognition->initialize(brain.get());
        
        NLM_LOG_INFO("Created cognitive function: " + name + " of type: " + type);
        
        // Store in extension system if available
        auto& extensionSystem = ExtensionSystem::getInstance();
        extensionSystem.emitEvent("cognitive_function_created", static_cast<void*>(cognition.get()), name);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in create_cognitive_function: " + e.what()));
        return false;
    }
}

bool LoadExtensionCommand::execute(const std::vector<std::string>& args, 
                                  std::shared_ptr<Config> config,
                                  std::shared_ptr<Brain> brain) {
    try {
        // Parse arguments
        std::string name;
        std::string modulePath;
        bool autoInit = true;
        std::string paramString;
        
        if (args.size() >= 2) name = args[0];
        if (args.size() >= 3) modulePath = args[1];
        if (args.size() >= 4) autoInit = (args[2] == "true" || args[2] == "1");
        if (args.size() >= 5) paramString = args[3];
        
        if (name.empty()) {
            NLM_LOG_ERROR("Load extension: name is required");
            return false;
        }
        
        // Parse parameters
        std::unordered_map<std::string, std::any> parameters;
        if (!paramString.empty()) {
            // Simple parameter parsing: "key=value,key2=value2"
            size_t pos = 0;
            while (pos < paramString.length()) {
                size_t comma = paramString.find(',', pos);
                if (comma == std::string::npos) {
                    comma = paramString.length();
                }
                
                std::string param = paramString.substr(pos, comma - pos);
                size_t equals = param.find('=');
                if (equals != std::string::npos) {
                    std::string key = param.substr(0, equals);
                    std::string value = param.substr(equals + 1);
                    parameters[key] = value;
                }
                
                pos = comma + 1;
                if (paramString[pos - 1] == ',') {
                    pos++;
                }
            }
        }
        
        // Load extension using extension manager
        ExtensionManager::LoadConfig loadConfig;
        loadConfig.autoInitialize = autoInit;
        loadConfig.parameters = parameters;
        
        if (modulePath.empty()) {
            // Try to load from extension system
            auto& extensionSystem = ExtensionSystem::getInstance();
            return extensionSystem.loadExtension(name);
        } else {
            // Try to load from file/module
            auto& extensionManager = ExtensionManager::getInstance();
            return extensionManager.loadExtension(name, modulePath, loadConfig);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in load_extension: " + e.what()));
        return false;
    }
}

bool UnloadExtensionCommand::execute(const std::vector<std::string>& args, 
                                    std::shared_ptr<Config> config,
                                    std::shared_ptr<Brain> brain) {
    try {
        if (args.size() < 2) {
            NLM_LOG_ERROR("Unload extension: name is required");
            return false;
        }
        
        std::string name = args[0];
        
        // Unload extension using extension manager
        auto& extensionManager = ExtensionManager::getInstance();
        return extensionManager.unloadExtension(name);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in unload_extension: " + e.what()));
        return false;
    }
}

bool ListExtensionsCommand::execute(const std::vector<std::string>& args, 
                                   std::shared_ptr<Config> config,
                                   std::shared_ptr<Brain> brain) {
    try {
        std::string category;
        std::string state;
        
        if (args.size() >= 2) category = args[0];
        if (args.size() >= 3) state = args[1];
        
        // Get extensions using extension manager
        auto& extensionManager = ExtensionManager::getInstance();
        std::vector<ExtensionManager::ExtensionInfo*> extensions;
        
        if (!category.empty()) {
            extensions = extensionManager.getExtensionsByCategory(category);
        } else {
            extensions = extensionManager.getAllExtensions();
        }
        
        // Filter by state if specified
        if (!state.empty()) {
            ExtensionManager::LifecycleState filterState;
            if (state == "unloaded") filterState = ExtensionManager::LifecycleState::Unloaded;
            else if (state == "loading") filterState = ExtensionManager::LifecycleState::Loading;
            else if (state == "loaded") filterState = ExtensionManager::LifecycleState::Loaded;
            else if (state == "initializing") filterState = ExtensionManager::LifecycleState::Initializing;
            else if (state == "initialized") filterState = ExtensionManager::LifecycleState::Initialized;
            else if (state == "shutting_down") filterState = ExtensionManager::LifecycleState::ShuttingDown;
            else if (state == "failed") filterState = ExtensionManager::LifecycleState::Failed;
            else {
                NLM_LOG_ERROR("List extensions: invalid state: " + state);
                return false;
            }
            
            std::vector<ExtensionManager::ExtensionInfo*> filtered;
            for (auto* info : extensions) {
                if (info->state == filterState) {
                    filtered.push_back(info);
                }
            }
            extensions = filtered;
        }
        
        // Print extension list
        NLM_LOG_INFO("Loaded Extensions:");
        for (const auto* info : extensions) {
            NLM_LOG_INFO("  - " + info->name + " (v" + info->version + ") - " + 
                        info->category + " - State: " + std::to_string(static_cast<int>(info->state)));
            NLM_LOG_INFO("    Description: " + info->description);
        }
        
        if (extensions.empty()) {
            NLM_LOG_INFO("  (no extensions found)");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in list_extensions: " + e.what()));
        return false;
    }
}

bool SystemStatusCommand::execute(const std::vector<std::string>& args, 
                                 std::shared_ptr<Config> config,
                                 std::shared_ptr<Brain> brain) {
    try {
        bool detailed = false;
        if (args.size() >= 2) detailed = (args[0] == "true" || args[0] == "1");
        
        std::stringstream status;
        
        // Basic status
        status << "System Status:\n";
        status << "  Components:\n";
        
        if (brain) {
            status << "    Brain:\n";
            status << "      - Neurons: " << brain->getTotalNeuronCount() << "\n";
            status << "      - Synapses: " << brain->getTotalSynapseCount() << "\n";
            status << "      - Regions: " << brain->getRegionCount() << "\n";
            status << "      - Spike Count: " << brain->getTotalSpikeCount() << "\n";
            status << "      - Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        }
        
        // Extension system status
        auto& extensionSystem = ExtensionSystem::getInstance();
        status << "  Extension System:\n";
        status << "    " << extensionSystem.getStatus();
        
        // Configuration status
        status << "  Configuration:\n";
        if (config) {
            auto keys = config->getKeys();
            status << "    Configuration entries: " << keys.size() << "\n";
            if (detailed) {
                for (const auto& key : keys) {
                    status << "      - " << key << "\n";
                }
            }
        }
        
        // Print status
        NLM_LOG_INFO(status.str());
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in system_status: " + e.what()));
        return false;
    }
}

bool ExtensionPointStatusCommand::execute(const std::vector<std::string>& args, 
                                         std::shared_ptr<Config> config,
                                         std::shared_ptr<Brain> brain) {
    try {
        std::string pointName;
        
        if (args.size() < 2) {
            NLM_LOG_ERROR("Extension point status: point_name is required");
            return false;
        }
        
        pointName = args[0];
        
        // Get extension system
        auto& extensionSystem = ExtensionSystem::getInstance();
        
        if (!extensionSystem.hasExtensionPoint(pointName)) {
            NLM_LOG_ERROR("Extension point not found: " + pointName);
            return false;
        }
        
        // Get handlers
        std::vector<std::string> handlers = extensionSystem.getExtensionPointHandlers(pointName);
        
        NLM_LOG_INFO("Extension Point Status: " + pointName);
        NLM_LOG_INFO("  Description: " + extensionSystem.getExtensionPointDescription(pointName));
        NLM_LOG_INFO("  Handlers: " + std::to_string(handlers.size()));
        
        for (const auto& handler : handlers) {
            NLM_LOG_INFO("    - " + handler);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in extension_point_status: " + e.what()));
        return false;
    }
}

bool EventSystemStatusCommand::execute(const std::vector<std::string>& args, 
                                       std::shared_ptr<Config> config,
                                       std::shared_ptr<Brain> brain) {
    try {
        // Get extension system
        auto& extensionSystem = ExtensionSystem::getInstance();
        
        // Get listeners
        std::vector<std::string> listeners = extensionSystem.getEventListenerNames();
        
        NLM_LOG_INFO("Event System Status:");
        NLM_LOG_INFO("  Listeners: " + std::to_string(listeners.size()));
        
        for (const auto& listener : listeners) {
            NLM_LOG_INFO("    - " + listener);
        }
        
        // Get events
        std::vector<std::string> events = extensionSystem.getEvents();
        NLM_LOG_INFO("  Registered Events: " + std::to_string(events.size()));
        
        for (const auto& event : events) {
            NLM_LOG_INFO("    - " + event + ": " + extensionSystem.getEventDescription(event));
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in event_system_status: " + e.what()));
        return false;
    }
}

bool ConfigurationCommand::execute(const std::vector<std::string>& args, 
                                  std::shared_ptr<Config> config,
                                  std::shared_ptr<Brain> brain) {
    try {
        if (args.size() < 2) {
            NLM_LOG_ERROR("Configuration: operation and name are required");
            return false;
        }
        
        std::string operation = args[0];
        std::string name = args[1];
        std::string value;
        if (args.size() >= 3) value = args[2];
        
        if (operation == "set") {
            config->set(name, value);
            NLM_LOG_INFO("Set configuration: " + name + " = " + value);
        } else if (operation == "get") {
            if (config->has(name)) {
                NLM_LOG_INFO("Get configuration: " + name + " = " + config->get<std::string>(name).value_or(""));
            } else {
                NLM_LOG_ERROR("Configuration key not found: " + name);
                return false;
            }
        } else if (operation == "delete") {
            config->remove(name);
            NLM_LOG_INFO("Deleted configuration: " + name);
        } else if (operation == "list") {
            auto keys = config->getKeys();
            NLM_LOG_INFO("Configuration keys: " + std::to_string(keys.size()));
            for (const auto& key : keys) {
                NLM_LOG_INFO("  - " + key);
            }
        } else {
            NLM_LOG_ERROR("Configuration: unknown operation: " + operation);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in configuration: " + e.what()));
        return false;
    }
}

bool PluginManagementCommand::execute(const std::vector<std::string>& args, 
                                     std::shared_ptr<Config> config,
                                     std::shared_ptr<Brain> brain) {
    try {
        if (args.size() < 2) {
            NLM_LOG_ERROR("Plugin management: operation and plugin_name are required");
            return false;
        }
        
        std::string operation = args[0];
        std::string pluginName = args[1];
        std::string paramString;
        if (args.size() >= 3) paramString = args[2];
        
        // Parse parameters
        std::unordered_map<std::string, std::any> parameters;
        if (!paramString.empty()) {
            // Simple parameter parsing
            size_t pos = 0;
            while (pos < paramString.length()) {
                size_t comma = paramString.find(',', pos);
                if (comma == std::string::npos) {
                    comma = paramString.length();
                }
                
                std::string param = paramString.substr(pos, comma - pos);
                size_t equals = param.find('=');
                if (equals != std::string::npos) {
                    std::string key = param.substr(0, equals);
                    std::string value = param.substr(equals + 1);
                    parameters[key] = value;
                }
                
                pos = comma + 1;
                if (paramString[pos - 1] == ',') {
                    pos++;
                }
            }
        }
        
        if (operation == "start") {
            auto& extensionSystem = ExtensionSystem::getInstance();
            if (extensionSystem.loadExtension(pluginName)) {
                NLM_LOG_INFO("Started plugin: " + pluginName);
                return true;
            } else {
                NLM_LOG_ERROR("Failed to start plugin: " + pluginName);
                return false;
            }
        } else if (operation == "stop") {
            auto& extensionSystem = ExtensionSystem::getInstance();
            if (extensionSystem.unloadExtension(pluginName)) {
                NLM_LOG_INFO("Stopped plugin: " + pluginName);
                return true;
            } else {
                NLM_LOG_ERROR("Failed to stop plugin: " + pluginName);
                return false;
            }
        } else if (operation == "status") {
            auto& extensionManager = ExtensionManager::getInstance();
            auto* info = extensionManager.getExtension(pluginName);
            if (info) {
                NLM_LOG_INFO("Plugin Status: " + pluginName);
                NLM_LOG_INFO("  Version: " + info->version);
                NLM_LOG_INFO("  Category: " + info->category);
                NLM_LOG_INFO("  Description: " + info->description);
                NLM_LOG_INFO("  State: " + std::to_string(static_cast<int>(info->state)));
                return true;
            } else {
                NLM_LOG_ERROR("Plugin not found: " + pluginName);
                return false;
            }
        } else {
            NLM_LOG_ERROR("Plugin management: unknown operation: " + operation);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in plugin_management: " + e.what()));
        return false;
    }
}

} // namespace nlm
