/*
 * Enhanced Python API for NLM - Extended bindings and utilities
 * This header file provides additional Pythonic interfaces and utilities
 * for improved usability and performance.
 */

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/json.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <sstream>
#include <iomanip>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
    class SensoryInput;
    class Action;
    
    // Type definitions
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint64_t;
    using PopulationId = uint64_t;
    
    enum class NeuronType {
        Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
    };
    
    enum class DevelopmentalStage {
        Initial, CriticalPeriod, Maturation, Adult, Aging
    };
    
    enum class ActionType {
        MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight,
        Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom
    };
    
    enum class WorldObjectType {
        Empty, Resource, Hazard, Wall, Marker
    };
}

namespace nlm {
namespace python {

/**
 * @brief Enhanced configuration utilities for Python
 */
class PythonConfigUtils {
public:
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for Phase 6 integration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Memory settings
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development
        config->set("development.stage", "CriticalPeriod", ConfigSource::Default);
        
        return config;
    }
    
    static std::shared_ptr<Config> loadFromJSON(const std::string& jsonString) {
        try {
            auto config = std::make_shared<Config>();
            auto json = pybind11::json::parse(jsonString);
            
            // Helper function to convert JSON to config
            auto flattenJSON = [](const pybind11::json& j, const std::string& prefix) {
                std::unordered_map<std::string, std::string> flatMap;
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
                    if (it.value().is_object()) {
                        flattenJSON(it.value(), key);
                    } else if (it.value().is_number()) {
                        flatMap[key] = std::to_string(it.value().get<double>());
                    } else if (it.value().is_string()) {
                        flatMap[key] = it.value().get<std::string>();
                    } else if (it.value().is_boolean()) {
                        flatMap[key] = it.value().get<bool>() ? "true" : "false";
                    } else if (it.value().is_null()) {
                        flatMap[key] = "";
                    }
                }
                return flatMap;
            };
            
            auto flat = flattenJSON(json, "");
            for (const auto& pair : flat) {
                config->set(pair.first, pair.second, ConfigSource::User);
            }
            
            return config;
        } catch (const pybind11::json::parse_error& e) {
            throw std::runtime_error("Failed to parse JSON configuration: " + e.what() + " at position " + std::to_string(e.pos()));
        }
    }
    
    static std::string configToJSON(const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config pointer is null");
        }
        
        pybind11::json json;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            std::string currentKey = key;
            auto value = config->getOr<std::string>(key, "");
            
            // Navigate nested keys
            size_t dotPos;
            while ((dotPos = currentKey.find('.')) != std::string::npos) {
                std::string parentKey = currentKey.substr(0, dotPos);
                std::string childKey = currentKey.substr(dotPos + 1);
                
                if (!json.contains(parentKey)) {
                    json[parentKey] = pybind11::json::object();
                }
                json[parentKey][childKey] = value;
                currentKey = childKey;
            }
            
            // Handle leaf node
            if (!json.contains(currentKey)) {
                json[currentKey] = value;
            }
        }
        
        return json.dump(2);
    }
    
    static std::string getSystemInfo() {
        std::stringstream ss;
        ss << "NLM Python Bindings" << std::endl;
        ss << "====================" << std::endl;
        ss << "Version: 0.1.0" << std::endl;
        ss << "Build: Enhanced API" << std::endl;
        ss << "Features:" << std::endl;
        ss << "  - JSON configuration support" << std::endl;
        ss << "  - Enhanced error handling" << std::endl;
        ss << "  - Performance monitoring" << std::endl;
        ss << "  - Advanced simulation control" << std::endl;
        ss << "  - Debugging utilities" << std::endl;
        ss << "  - Context managers" << std::endl;
        return ss.str();
    }
};

/**
 * @brief Enhanced brain interface with Pythonic improvements
 */
class PythonBrainInterface {
public:
    static void registerEnhancedBrainInterface(pybind11::module_& m) {
        // Add enhanced brain methods
        m.def("runEpisode", &PythonBrainInterface::runEpisode, py::arg("brain"), py::arg("steps"), py::arg("world") = nullptr,
              "Run a complete episode with optional world interaction");
        
        m.def("benchmarkPerformance", &PythonBrainInterface::benchmarkPerformance, py::arg("brain"), py::arg("steps") = 1000,
              "Run performance benchmark on brain");
        
        m.def("monitorActivity", &PythonBrainInterface::monitorActivity, py::arg("brain"), py::arg("interval") = 10,
              "Monitor brain activity over time");
        
        m.def("exportBrainState", &PythonBrainInterface::exportBrainState, py::arg("brain"), py::arg("filename"),
              "Export brain state to file");
        
        m.def("importBrainState", &PythonBrainInterface::importBrainState, py::arg("brain"), py::arg("filename"),
              "Import brain state from file");
        
        m.def("getBrainStatistics", &PythonBrainInterface::getBrainStatistics, py::arg("brain"),
              "Get comprehensive brain statistics as a dictionary");
        
        m.def("compareBrains", &PythonBrainInterface::compareBrains, py::arg("brain1"), py::arg("brain2"),
              "Compare two brains and return differences");
        
        m.def("validateBrainConfiguration", &PythonBrainInterface::validateBrainConfiguration, py::arg("brain"),
              "Validate brain configuration and return warnings/errors");
    }
    
private:
    static pybind11::dict runEpisode(std::shared_ptr<Brain> brain, size_t steps, std::shared_ptr<SimpleWorld> world) {
        pybind11::dict result;
        result["success"] = false;
        
        try {
            if (!brain || !brain->initialize()) {
                throw std::runtime_error("Brain initialization failed");
            }
            
            result["steps"] = steps;
            result["success"] = true;
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["firing_rate"] = brain->getAverageFiringRate();
            result["neurons"] = brain->getTotalNeuronCount();
            
            for (size_t step = 0; step < steps; ++step) {
                if (world) {
                    auto percept = world->getSensoryPercept();
                    // Would need agent interface to process this
                }
                brain->step(step);
            }
            
        } catch (const std::exception& e) {
            result["error"] = e.what();
        }
        
        return result;
    }
    
    static pybind11::dict benchmarkPerformance(std::shared_ptr<Brain> brain, size_t steps) {
        pybind11::dict result;
        
        if (!brain || !brain->initialize()) {
            throw std::runtime_error("Brain initialization failed for benchmark");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        result["steps"] = steps;
        result["time_seconds"] = elapsed;
        result["steps_per_second"] = steps / elapsed;
        result["spikes_per_step"] = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result["neurons"] = brain->getTotalNeuronCount();
        result["firing_rate_hz"] = brain->getAverageFiringRate() * 1000.0;
        
        return result;
    }
    
    static pybind11::dict monitorActivity(std::shared_ptr<Brain> brain, size_t interval) {
        pybind11::dict result;
        
        if (!brain) {
            throw std::runtime_error("Invalid brain pointer");
        }
        
        std::vector<pybind11::dict> history;
        
        for (size_t step = 0; step < interval; ++step) {
            brain->step(step);
            
            pybind11::dict snapshot;
            snapshot["step"] = step;
            snapshot["total_spikes"] = brain->getTotalSpikeCount();
            snapshot["firing_neurons"] = brain->getFiringNeuronCount();
            snapshot["average_firing_rate"] = brain->getAverageFiringRate();
            snapshot["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            snapshot["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            history.push_back(snapshot);
        }
        
        result["history"] = history;
        result["duration_steps"] = interval;
        result["final_total_spikes"] = brain->getTotalSpikeCount();
        result["final_firing_neurons"] = brain->getFiringNeuronCount();
        
        return result;
    }
    
    static bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        return brain->save(filename);
    }
    
    static bool importBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        return brain->load(filename);
    }
    
    static pybind11::dict getBrainStatistics(std::shared_ptr<Brain> brain) {
        pybind11::dict stats;
        
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        auto config = brain->getConfig();
        if (config) {
            auto keys = config->getKeys();
            pybind11::dict configDict;
            for (const auto& key : keys) {
                configDict[key] = config->getOr<std::string>(key, "");
            }
            stats["configuration"] = configDict;
        }
        
        return stats;
    }
    
    static pybind11::dict compareBrains(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        pybind11::dict comparison;
        
        if (!brain1 || !brain2) {
            throw std::invalid_argument("Invalid brain pointers");
        }
        
        comparison["neuron_count_diff"] = static_cast<int>(brain1->getTotalNeuronCount() - brain2->getTotalNeuronCount());
        comparison["synapse_count_diff"] = static_cast<int>(brain1->getTotalSynapseCount() - brain2->getTotalSynapseCount());
        comparison["spike_count_diff"] = static_cast<int>(brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount());
        comparison["firing_rate_diff"] = brain1->getAverageFiringRate() - brain2->getAverageFiringRate();
        comparison["e_i_ratio_diff"] = brain1->getExcitationInhibitionRatio() - brain2->getExcitationInhibitionRatio();
        
        return comparison;
    }
    
    static pybind11::dict validateBrainConfiguration(std::shared_ptr<Brain> brain) {
        pybind11::dict validation;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        if (!brain) {
            errors.push_back("Brain pointer is null");
        }
        
        auto config = brain->getConfig();
        if (!config) {
            warnings.push_back("No configuration loaded");
        } else {
            // Validate configuration parameters
            auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errors.push_back("Invalid neuron_count: must be positive");
            }
            
            auto connectionProb = config->getOr<float>("connection_probability", 0.0f);
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                warnings.push_back("connection_probability should be between 0.0 and 1.0");
            }
            
            auto steps = config->getOr<size_t>("max_simulation_steps", 0);
            if (steps == 0) {
                warnings.push_back("max_simulation_steps not set, using default");
            }
        }
        
        validation["valid"] = errors.empty();
        validation["warnings"] = warnings;
        validation["errors"] = errors;
        
        return validation;
    }
};

/**
 * @brief Context manager for brain simulation
 */
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain), wasInitialized_(false) {
        if (brain_) {
            wasInitialized_ = brain_->initialize();
        }
    }
    
    ~BrainContextManager() {
        // Cleanup handled by brain destructor
    }
    
    bool runSteps(size_t steps) {
        if (!brain_) return false;
        
        try {
            for (size_t step = 0; step < steps; ++step) {
                brain_->step(step);
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
    bool wasInitialized_;
};

/**
 * @brief Utility functions for enhanced Python API
 */
namespace utils {
    
    static std::string formatNeuronId(const NeuronId& id) {
        std::stringstream ss;
        ss << "Neuron(" << id.index() << ")";
        return ss.str();
    }
    
    static std::string formatSynapseId(const SynapseId& id) {
        std::stringstream ss;
        ss << "Synapse(" << id.index() << ")";
        return ss.str();
    }
    
    static std::string formatRegionId(const RegionId& id) {
        std::stringstream ss;
        ss << "Region(" << id.index() << ")";
        return ss.str();
    }
    
    static std::string formatDevelopmentStage(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    static std::string formatNeuronType(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    static std::string formatActionType(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    static std::string formatWorldObjectType(WorldObjectType type) {
        switch (type) {
            case WorldObjectType::Empty: return "Empty";
            case WorldObjectType::Resource: return "Resource";
            case WorldObjectType::Hazard: return "Hazard";
            case WorldObjectType::Wall: return "Wall";
            case WorldObjectType::Marker: return "Marker";
            default: return "Unknown";
        }
    }
    
    static std::string generateRandomSeed() {
        std::random_device rd;
        return std::to_string(rd());
    }
    
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        std::stringstream ss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) ss << delimiter;
            ss << strings[i];
        }
        return ss.str();
    }
};

/**
 * @brief Main function to extend the Python module
 */
void extendPythonModule(pybind11::module_& m);

} // namespace python
} // namespace nlm

