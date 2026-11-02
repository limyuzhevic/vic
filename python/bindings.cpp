#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Config/EnhancedConfig.hpp"

namespace py = pybind11;
namespace nlm {

// Pythonic Context Manager for Brain Simulation
class BrainSimulationContext {
public:
    BrainSimulationContext(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent)
        : brain_(brain), world_(world), agent_(agent) {
        if (!brain_ || !world_ || !agent_) {
            throw std::runtime_error("All components must be initialized before creating simulation context");
        }
    }

    // Run simulation for specified number of steps
    BrainSimulationContext& runSteps(size_t num_steps, float timestep = 0.1f) {
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            if (world_) {
                world_->update(timestep);
            }
            
            // Process sensory input if agent exists
            if (agent_ && world_) {
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
            }
            
            // Run brain step
            if (brain_) {
                brain_->step(step);
            }
            
            // Decode and apply action if agent exists
            if (agent_ && world_) {
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
        }
        return *this;
    }

    // Run simulation until stop condition (predicate)
    BrainSimulationContext& runUntil(std::function<bool(size_t)> predicate, float timestep = 0.1f) {
        size_t step = 0;
        while (!predicate(step)) {
            // Update world
            if (world_) {
                world_->update(timestep);
            }
            
            // Process sensory input if agent exists
            if (agent_ && world_) {
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
            }
            
            // Run brain step
            if (brain_) {
                brain_->step(step);
            }
            
            // Decode and apply action if agent exists
            if (agent_ && world_) {
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
            
            step++;
        }
        return *this;
    }

    // Get simulation statistics
    std::unordered_map<std::string, double> getStatistics() const {
        std::unordered_map<std::string, double> stats;
        if (brain_) {
            stats["neurons"] = brain_->getTotalNeuronCount();
            stats["synapses"] = brain_->getTotalSynapseCount();
            stats["firing_neurons"] = brain_->getFiringNeuronCount();
            stats["total_spikes"] = brain_->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain_->getAverageFiringRate();
            stats["development_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
            
            if (agent_) {
                stats["curiosity"] = agent_->getCuriosityLevel();
                stats["novelty"] = agent_->getNoveltyLevel();
                stats["prediction_error"] = agent_->getPredictionError();
                stats["neuromodulation_level"] = agent_->getNeuromodulationLevel();
            }
        }
        if (world_) {
            stats["world_time"] = world_->getSimulationTime();
            stats["world_width"] = world_->getWidth();
            stats["world_height"] = world_->getHeight();
        }
        return stats;
    }

    // Export statistics as formatted string
    std::string formatStatistics(const std::string& format = "table") const {
        auto stats = getStatistics();
        std::stringstream ss;
        
        if (format == "table") {
            ss << "=== NLM Simulation Statistics ===" << std::endl;
            ss << std::left << std::setw(25) << "Metric" << "Value" << std::endl;
            ss << std::left << std::setw(25) << "-----" << "-----" << std::endl;
            
            const std::vector<std::string> metrics = {
                "neurons", "synapses", "firing_neurons", "total_spikes",
                "avg_firing_rate", "development_stage", "curiosity", "novelty",
                "prediction_error", "neuromodulation_level", "world_time", "world_width", "world_height"
            };
            
            for (const auto& metric : metrics) {
                auto it = stats.find(metric);
                if (it != stats.end()) {
                    ss << std::left << std::setw(25) << metric << it->second << std::endl;
                }
            }
        } else if (format == "json") {
            ss << "{" << std::endl;
            bool first = true;
            for (const auto& pair : stats) {
                if (!first) ss << "," << std::endl;
                ss << "  \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            ss << std::endl << "}";
        }
        
        return ss.str();
    }

    // Context manager protocol
    void __enter__() {}
    void __exit__(int, const char*, const char*, const char*) {}

    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }

    // Get the underlying world
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }

    // Get the underlying agent
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
};

// Pythonic Context Manager for World Simulation
class WorldSimulationContext {
public:
    WorldSimulationContext(std::shared_ptr<SimpleWorld> world)
        : world_(world) {
        if (!world_) {
            throw std::runtime_error("World must be provided");
        }
    }

    // Update world for specified number of steps
    WorldSimulationContext& runSteps(size_t num_steps, float timestep = 0.1f) {
        for (size_t step = 0; step < num_steps; ++step) {
            world_->update(timestep);
        }
        return *this;
    }

    // Update world until stop condition (predicate)
    WorldSimulationContext& runUntil(std::function<bool(float)> predicate, float timestep = 0.1f) {
        while (!predicate(world_->getSimulationTime())) {
            world_->update(timestep);
        }
        return *this;
    }

    // Context manager protocol
    void __enter__() {}
    void __exit__(int, const char*, const char*, const char*) {}

    // Get current simulation time
    double getTime() const { return world_->getSimulationTime(); }

    // Get world dimensions
    std::pair<size_t, size_t> getDimensions() const {
        return {world_->getWidth(), world_->getHeight()};
    }

    // Get world properties
    std::unordered_map<std::string, double> getProperties() const {
        std::unordered_map<std::string, double> props;
        props["time"] = world_->getSimulationTime();
        props["width"] = world_->getWidth();
        props["height"] = world_->getHeight();
        props["max_energy"] = world_->getMaxEnergy();
        props["energy_decay_rate"] = world_->getEnergyDecayRate();
        return props;
    }

    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }

private:
    std::shared_ptr<SimpleWorld> world_;
};

// Enhanced Simulation Utilities
namespace SimulationUtils {
    // Create a complete agent simulation setup
    static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createAgentSetup(const std::string& preset = "basic") {
        auto config = nlm::EnhancedConfig::ConfigHelper::createFromPreset(preset);
        auto brain = std::make_shared<Brain>(config->getConfig());
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(world);
        
        // Enable common subsystems based on preset
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }

    // Run a complete agent simulation episode
    static std::unordered_map<std::string, double> 
    runAgentEpisode(size_t max_steps = 1000, const std::string& preset = "basic") {
        auto [brain, world, agent] = createAgentSetup(preset);
        brain->initialize();
        
        double total_reward = 0.0;
        size_t steps = 0;
        
        for (; steps < max_steps; ++steps) {
            // Update world
            world->update(0.1);
            
            // Get percept and process
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain
            brain->step(steps);
            
            // Get action and apply
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward
            agent->applyRewardModulation(result.reward, 0.0);
            
            total_reward += result.reward;
            
            // Check for termination conditions
            if (agent->getCuriosityLevel() > 0.95 || agent->getNoveltyLevel() > 0.95) {
                break; // Too much exploration/exploration
            }
        }
        
        std::unordered_map<std::string, double> results;
        results["total_reward"] = total_reward;
        results["steps_completed"] = static_cast<double>(steps);
        results["avg_reward_per_step"] = steps > 0 ? total_reward / steps : 0.0;
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["final_novelty"] = agent->getNoveltyLevel();
        results["final_prediction_error"] = agent->getPredictionError();
        
        return results;
    }

    // Batch run multiple episodes for statistical analysis
    static std::vector<std::unordered_map<std::string, double>> 
    batchRunEpisodes(size_t num_episodes, size_t steps_per_episode = 100, const std::string& preset = "basic") {
        std::vector<std::unordered_map<std::string, double>> all_results;
        
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            // Set different random seed for each episode
            auto config = nlm::EnhancedConfig::ConfigHelper::createFromPreset(preset);
            config->set("random_seed", static_cast<int64_t>(42 + episode * 1000));
            
            auto [brain, world, agent] = createAgentSetup(preset);
            brain->initialize();
            
            double total_reward = 0.0;
            for (size_t step = 0; step < steps_per_episode; ++step) {
                world->update(0.1);
                agent->processSensoryInput(world->getSensoryPercept());
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                agent->applyRewardModulation(result.reward, 0.0);
                total_reward += result.reward;
            }
            
            std::unordered_map<std::string, double> episode_result;
            episode_result["episode"] = static_cast<double>(episode);
            episode_result["total_reward"] = total_reward;
            episode_result["avg_reward_per_step"] = total_reward / steps_per_episode;
            episode_result["final_curiosity"] = agent->getCuriosityLevel();
            episode_result["final_novelty"] = agent->getNoveltyLevel();
            all_results.push_back(episode_result);
        }
        
        return all_results;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        This module provides an enhanced Python interface for the NLM
        C++ neural simulation framework with Pythonic APIs, context managers,
        and advanced simulation utilities.
        
        Key features:
        - Pythonic context managers for resource management
        - Enhanced simulation helpers and utilities
        - Advanced configuration presets with JSON schema validation
        - Batch running for statistical analysis
        - Comprehensive error handling and validation
        
        Examples:
            
        ```python
        # Basic usage with enhanced context managers
        import pynlm
        
        with pynlm.agent_simulation(preset="research") as sim:
            sim.runSteps(100)
            stats = sim.getStatistics()
        
        # Using enhanced configuration system
        config = pynlm.EnhancedConfig.createFromPreset("advanced")
        
        # Batch running for analysis
        results = pynlm.SimulationUtils.batchRunEpisodes(50, 200)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // EnhancedConfig class
    py::class_<nlm::EnhancedConfig, std::shared_ptr<nlm::EnhancedConfig>>(m, "EnhancedConfig",
         R"pbdoc(Enhanced configuration class with JSON schema validation)pbdoc")
        .def(py::init<>())
        .def_static("createFromPreset", &nlm::EnhancedConfig::createFromPreset,
                   py::arg("preset_name"),
                   "Create configuration from preset")
        .def_static("createFromJSON", &nlm::EnhancedConfig::createFromJSON,
                   py::arg("json_str"),
                   "Create configuration from JSON string")
        .def_static("createFromFile", &nlm::EnhancedConfig::createFromFile,
                   py::arg("filepath"),
                   "Create configuration from file")
        .def("validate", &nlm::EnhancedConfig::validate,
             "Validate configuration and return errors")
        .def("isValid", &nlm::EnhancedConfig::isValid,
             "Check if configuration is valid")
        .def("getValidationErrors", &nlm::EnhancedConfig::getValidationErrors,
             "Get validation errors")
        .def("getValidationStats", &nlm::EnhancedConfig::getValidationStats,
             "Get validation statistics")
        .def("saveToFile", &nlm::EnhancedConfig::saveToFile,
             py::arg("filepath"), "Save configuration to file")
        .def("toJSON", &nlm::EnhancedConfig::toJSON,
             "Get configuration as JSON")
        .def("get", [](const nlm::EnhancedConfig& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get configuration value")
        .def("set", [](nlm::EnhancedConfig& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set", [](nlm::EnhancedConfig& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set int configuration value")
        .def("set", [](nlm::EnhancedConfig& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("set", [](nlm::EnhancedConfig& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set bool configuration value")
        .def("has", &nlm::EnhancedConfig::has, py::arg("key"),
             "Check if configuration has key")
        .def("getKeys", &nlm::EnhancedConfig::getKeys,
             "Get all configuration keys")
        .def("getValidKeys", &nlm::EnhancedConfig::getValidKeys,
             "Get valid configuration keys from schema")
        .def("getKeyInfo", &nlm::EnhancedConfig::getKeyInfo, py::arg("key"),
             "Get information about configuration key")
        .def("merge", &nlm::EnhancedConfig::merge, py::arg("other"),
             "Merge with another configuration")
        .def("mergeFromJSON", &nlm::EnhancedConfig::mergeFromJSON, py::arg("other"),
             "Merge from JSON configuration")
        .def("diff", &nlm::EnhancedConfig::diff, py::arg("other"),
             "Get configuration difference")
        .def("equals", &nlm::EnhancedConfig::equals, py::arg("other"),
             "Check if configurations are equal")
        .def("summary", &nlm::EnhancedConfig::summary,
             "Get configuration summary")
        .def("getSchemaDescription", &nlm::EnhancedConfig::getSchemaDescription,
             "Get JSON schema description")
        .def_static("ConfigHelper.createMinimal", &nlm::EnhancedConfig::ConfigHelper::createMinimal,
                   "Create minimal configuration")
        .def_static("ConfigHelper.createResearch", &nlm::EnhancedConfig::ConfigHelper::createResearch,
                   "Create research configuration")
        .def_static("ConfigHelper.createRealTime", &nlm::EnhancedConfig::ConfigHelper::createRealTime,
                   "Create real-time configuration")
        .def_static("ConfigHelper.createFromPreset", &nlm::EnhancedConfig::ConfigHelper::createFromPreset,
                   py::arg("preset_name"), "Create configuration from preset")
        .def_static("ConfigHelper.getAllPresetNames", &nlm::EnhancedConfig::ConfigHelper::getAllPresetNames,
                   "Get all available preset names");

    // ConfigValidator class
    py::class_<nlm::ConfigValidator, std::shared_ptr<nlm::ConfigValidator>>(m, "ConfigValidator",
         R"pbdoc(Configuration validator with JSON schema support)pbdoc")
        .def(py::init<>())
        .def("loadSchemaFromFile", &nlm::ConfigValidator::loadSchemaFromFile, py::arg("filepath"),
             "Load schema from JSON file")
        .def("loadSchemaFromString", &nlm::ConfigValidator::loadSchemaFromString, py::arg("json_str"),
             "Load schema from JSON string")
        .def("findPreset", &nlm::ConfigValidator::findPreset, py::arg("name"),
             "Find preset by name")
        .def("validate", &nlm::ConfigValidator::validate, py::arg("config"),
             "Validate configuration and return errors")
        .def("isValid", &nlm::ConfigValidator::isValid, py::arg("config"),
             "Check if configuration is valid")
        .def("getValidationStats", &nlm::ConfigValidator::getValidationStats, py::arg("config"),
             "Get validation statistics")
        .def("createDefaultConfig", &nlm::ConfigValidator::createDefaultConfig,
             "Create default configuration from schema")
        .def("getAllKeys", &nlm::ConfigValidator::getAllKeys,
             "Get all configuration keys from schema")
        .def("hasKey", &nlm::ConfigValidator::hasKey, py::arg("key"),
             "Check if key exists in schema")
        .def("getKeyInfo", &nlm::ConfigValidator::getKeyInfo, py::arg("key"),
             "Get key information from schema")
        .def_static("mergeConfigs", &nlm::ConfigValidator::mergeConfigs,
                   py::arg("configs"), "Merge multiple configurations")
        .def_static("diffConfig", &nlm::ConfigValidator::diffConfig,
                   py::arg("config1"), py::arg("config2"), "Diff two configurations")
        .def_static("diffConfigHuman", &nlm::ConfigValidator::diffConfigHuman,
                   py::arg("config1"), py::arg("config2"), "Get human-readable diff");

    // SensoryInput
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get total pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP system")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
             "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference, py::arg("preference"),
             "Set learned preference trait")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("enableRewardModulation", &Brain::applyNeuromodulation, py::arg("signal"),
             "Enable reward modulation");

    // AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Simulation helper classes
    py::class_<BrainSimulationContext>(m, "BrainSimulationContext",
         R"pbdoc(Context manager for brain simulations with advanced helpers)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>())
        .def("runSteps", &BrainSimulationContext::runSteps,
             py::arg("num_steps"), py::arg("timestep") = 0.1f,
             "Run simulation for specified number of steps")
        .def("runUntil", &BrainSimulationContext::runUntil,
             py::arg("predicate"), py::arg("timestep") = 0.1f,
             "Run simulation until predicate returns true")
        .def("getStatistics", &BrainSimulationContext::getStatistics,
             "Get simulation statistics")
        .def("formatStatistics", &BrainSimulationContext::formatStatistics,
             py::arg("format") = "table",
             "Format statistics as string")
        .def("getBrain", &BrainSimulationContext::getBrain,
             "Get the underlying brain")
        .def("getWorld", &BrainSimulationContext::getWorld,
             "Get the underlying world")
        .def("getAgent", &BrainSimulationContext::getAgent,
             "Get the underlying agent");

    py::class_<WorldSimulationContext>(m, "WorldSimulationContext",
         R"pbdoc(Context manager for world simulations)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("runSteps", &WorldSimulationContext::runSteps,
             py::arg("num_steps"), py::arg("timestep") = 0.1f,
             "Run world simulation for specified number of steps")
        .def("runUntil", &WorldSimulationContext::runUntil,
             py::arg("predicate"), py::arg("timestep") = 0.1f,
             "Run world simulation until predicate returns true")
        .def("getTime", &WorldSimulationContext::getTime,
             "Get current simulation time")
        .def("getDimensions", &WorldSimulationContext::getDimensions,
             "Get world dimensions")
        .def("getProperties", &WorldSimulationContext::getProperties,
             "Get world properties")
        .def("getWorld", &WorldSimulationContext::getWorld,
             "Get the underlying world");

    // Simulation utility functions
    m.def("agentSimulation", [](const std::string& preset, std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<BrainSimulationContext>(brain, world, agent);
    }, py::arg("preset"), py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create an enhanced agent simulation context");

    m.def("agentSimulation", [](const std::string& preset) {
        auto [brain, world, agent] = SimulationUtils::createAgentSetup(preset);
        return std::make_shared<BrainSimulationContext>(brain, world, agent);
    }, py::arg("preset") = "basic",
       "Create an enhanced agent simulation context with preset configuration");

    m.def("worldSimulation", [](std::shared_ptr<SimpleWorld> world) {
        return std::make_shared<WorldSimulationContext>(world);
    }, py::arg("world"),
          "Create a world simulation context");

    // Advanced simulation utilities
    m.def("runAgentEpisode", &SimulationUtils::runAgentEpisode,
         py::arg("max_steps") = 1000, py::arg("preset") = "basic",
         "Run a complete agent simulation episode");

    m.def("batchRunEpisodes", &SimulationUtils::batchRunEpisodes,
         py::arg("num_episodes"), py::arg("steps_per_episode") = 100, py::arg("preset") = "basic",
         "Run multiple episodes for statistical analysis");

    m.def("createAgentSetup", &SimulationUtils::createAgentSetup,
         py::arg("preset") = "basic",
         "Create a complete agent simulation setup");

    // Validation utilities
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        auto errors = nlm::EnhancedConfig::ConfigHelper::createValidator()->validate(config);
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation failed: " + errors[0]);
        }
    }, py::arg("config"),
       "Validate configuration (raises exception if invalid)");

    m.def("getConfigValidationErrors", [](const std::shared_ptr<Config>& config) {
        return nlm::EnhancedConfig::ConfigHelper::createValidator()->validate(config);
    }, py::arg("config"),
       "Get configuration validation errors");

    // Configuration helpers
    m.def("validateConfig", [](const nlm::EnhancedConfig& config) {
        auto errors = config.validate();
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation failed: " + errors[0]);
        }
    }, py::arg("config"),
       "Validate enhanced configuration (raises exception if invalid)");

    m.def("getConfigValidationErrors", [](const nlm::EnhancedConfig& config) {
        return config.validate();
    }, py::arg("config"),
       "Get enhanced configuration validation errors");

    // SimpleWorld factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM (Neural Learning Machine) Python Bindings with Pythonic APIs and context managers";
}

} // namespace nlm