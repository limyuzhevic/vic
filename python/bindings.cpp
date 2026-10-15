#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom exception classes for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

// Helper function for converting exceptions
void registerCustomExceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
}

// Configuration presets
void addConfigPresets(py::class_<Config>& configClass) {
    configClass.def_static("getDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        config->set("brain.region_count", 2, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("world.width", 20, nlm::ConfigSource::Default);
        config->set("world.height", 20, nlm::ConfigSource::Default);
        config->set("world.vision_width", 8, nlm::ConfigSource::Default);
        config->set("world.vision_height", 8, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration with sensible default values");
    
    configClass.def_static("getPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.05, nlm::ConfigSource::Default);
        config->set("brain.region_count", 1, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", false, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for performance");
    
    configClass.def_static("getExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.15, nlm::ConfigSource::Default);
        config->set("brain.region_count", 4, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for exploration and learning");
    
    configClass.def("validate", &Config::validate, "Validate configuration values and return True if valid");
    
    configClass.def("getConfigSummary", &Config::summary, "Get a human-readable configuration summary");
    
    configClass.def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
        auto value = self.get<float>(key);
        if (value.has_value()) {
            return value.value();
        }
        return defaultValue;
    }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get a float value with default fallback");
    
    configClass.def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
        auto value = self.get<int64_t>(key);
        if (value.has_value()) {
            return value.value();
        }
        return defaultValue;
    }, py::arg("key"), py::arg("defaultValue") = 0, "Get an integer value with default fallback");
    
    configClass.def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
        auto value = self.get<bool>(key);
        if (value.has_value()) {
            return value.value();
        }
        return defaultValue;
    }, py::arg("key"), py::arg("defaultValue") = false, "Get a boolean value with default fallback");
    
    configClass.def("setFloat", [](Config& self, const std::string& key, float value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        self.set(key, value, source);
    }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime, "Set a float configuration value");
    
    configClass.def("setInt", [](Config& self, const std::string& key, int64_t value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        self.set(key, value, source);
    }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime, "Set an integer configuration value");
    
    configClass.def("setBool", [](Config& self, const std::string& key, bool value, nlm::ConfigSource source = nlm::ConfigSource::Runtime) {
        self.set(key, value, source);
    }, py::arg("key"), py::arg("value"), py::arg("source") = nlm::ConfigSource::Runtime, "Set a boolean configuration value");
}

// Enhanced Brain class with Pythonic APIs
void addBrainEnhancements(py::class_<Brain>& brainClass) {
    brainClass.def("initializeWithValidation", [](Brain& self) {
        bool success = self.initialize();
        if (!success) {
            throw BrainError("Failed to initialize brain");
        }
        return success;
    }, "Initialize brain with validation and error handling");
    
    brainClass.def("stepWithTime", [](Brain& self, uint64_t step, double time) {
        self.step(step, time);
    }, py::arg("step"), py::arg("time"), "Perform a simulation step with timestamp");
    
    brainClass.def("getBrainStatistics", [](const Brain& self) {
        std::stringstream ss;
        ss << "Neuron Count: " << self.getTotalNeuronCount() << "\n";
        ss << "Synapse Count: " << self.getTotalSynapseCount() << "\n";
        ss << "Firing Neurons: " << self.getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << self.getAverageFiringRate() << "\n";
        ss << "E/I Ratio: " << self.getExcitationInhibitionRatio() << "\n";
        ss << "Total Spikes: " << self.getTotalSpikeCount();
        return ss.str();
    }, "Get formatted brain statistics");
    
    brainClass.def("createSnapshot", [](Brain& self, const std::string& filename) {
        self.save(filename);
    }, py::arg("filename"), "Save brain state to file");
    
    brainClass.def("loadSnapshot", [](Brain& self, const std::string& filename) {
        self.load(filename);
    }, py::arg("filename"), "Load brain state from file");
    
    brainClass.def("getRegionByName", [](Brain& self, const std::string& name) -> nlm::RegionId {
        for (const auto& rid : self.getRegionIds()) {
            auto* region = self.getRegion(rid);
            if (region && region->name == name) {
                return rid;
            }
        }
        return nlm::INVALID_REGION_ID;
    }, py::arg("name"), "Get region ID by name");
    
    brainClass.def("getRegionNames", [](const Brain& self) {
        std::vector<std::string> names;
        for (const auto& rid : self.getRegionIds()) {
            auto* region = self.getRegion(rid);
            if (region) {
                names.push_back(region->name);
            }
        }
        return names;
    }, "Get all region names");
}

// Enhanced AgentBrain class
void addAgentBrainEnhancements(py::class_<AgentBrain>& agentBrainClass) {
    agentBrainClass.def("enableAllSubsystems", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
    }, "Enable all agent subsystems");
    
    agentBrainClass.def("disableAllSubsystems", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
    }, "Disable all agent subsystems");
    
    agentBrainClass.def("getSubsystemStatus", [](const AgentBrain& self) {
        return std::make_tuple(
            self.isRewardModulationEnabled(),
            self.isStructuralPlasticityEnabled(),
            self.isDevelopmentEnabled(),
            self.isCuriosityEnabled()
        );
    }, "Get status of all subsystems as a tuple");
    
    agentBrainClass.def("updateAllSystems", [](AgentBrain& self, double timestep) {
        self.updateDevelopment(timestep);
    }, py::arg("timestep"), "Update all enabled systems");
    
    agentBrainClass.def("getAgentInfo", [](const AgentBrain& self) {
        std::stringstream ss;
        ss << "Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
        ss << "Neuromodulation Level: " << self.getNeuromodulationLevel() << "\n";
        ss << "Curiosity Level: " << self.getCuriosityLevel() << "\n";
        ss << "Novelty Level: " << self.getNoveltyLevel() << "\n";
        ss << "Prediction Error: " << self.getPredictionError();
        return ss.str();
    }, "Get formatted agent information");
}

// Enhanced SimpleWorld class
void addWorldEnhancements(py::class_<SimpleWorld>& worldClass) {
    worldClass.def("configureSimple", [](SimpleWorld& self, size_t width, size_t height) {
        self.configure(width, height, width / 3, height / 3);
    }, py::arg("width"), py::arg("height"), "Configure world with square vision");
    
    worldClass.def("addResource", [](SimpleWorld& self, float x, float y, float value) {
        nlm::WorldObject obj(x, y, nlm::WorldObjectType::Resource, value, 0.5f);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource object to the world");
    
    worldClass.def("addHazard", [](SimpleWorld& self, float x, float y, float damage) {
        nlm::WorldObject obj(x, y, nlm::WorldObjectType::Hazard, damage, 0.5f);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("damage"), "Add a hazard object to the world");
    
    worldClass.def("getObjectCount", [](const SimpleWorld& self, nlm::WorldObjectType type) {
        int count = 0;
        // Note: This would need to be implemented in the C++ world class
        // For now, return 0 as a placeholder
        return count;
    }, py::arg("type"), "Get count of objects of a specific type");
    
    worldClass.def("resetAndSeed", [](SimpleWorld& self, uint32_t seed) {
        self.setRandomSeed(seed);
        self.reset();
    }, py::arg("seed"), "Reset world and set random seed");
    
    worldClass.def("getWorldBounds", [](const SimpleWorld& self) {
        return std::make_tuple(self.getWidth(), self.getHeight());
    }, "Get world dimensions as (width, height)");
    
    worldClass.def("isAgentNear", [](const SimpleWorld& self, float x, float y, float distance) {
        auto body = self.getAgentBody();
        float dx = body.x - x;
        float dy = body.y - y;
        return (dx * dx + dy * dy) <= (distance * distance);
    }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if agent is within distance of position");
}

// Context manager for simulation episodes
class EpisodeManager {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    bool initialized_ = false;
    
public:
    EpisodeManager(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                   std::shared_ptr<SimpleWorld> world) 
        : brain_(brain), agent_(agent), world_(world) {}
    
    bool enter() {
        world_->reset();
        agent_->reset();
        if (brain_) {
            brain_->reset();
        }
        initialized_ = true;
        return initialized_;
    }
    
    void exit() {
        if (initialized_) {
            // Save final state
            try {
                brain_->save("final_state.bin");
            } catch (...) {
                // Ignore save errors on exit
            }
            initialized_ = false;
        }
    }
    
    bool isInitialized() const { return initialized_; }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "Episode initialized: " << (initialized_ ? "Yes" : "No") << "\n";
        if (world_) {
            ss << "World time: " << world_->getSimulationTime() << "\n";
            ss << "Agent position: (" << world_->getAgentBody().x << ", " 
                << world_->getAgentBody().y << ")\n";
        }
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        - Enhanced Brain class with validation and statistics
        - AgentBrain interface for world interaction
        - SimpleWorld for 2D simulation environments
        - Configuration presets and validation
        - Episode context manager for simulation loops
        - Comprehensive error handling with custom Python exceptions
        
        Key Features:
        * Pythonic APIs with type hints and validation
        * Custom exceptions for better error handling
        * Configuration presets (default, performance, exploration)
        * Episode context manager for simulation management
        * Enhanced agent subsystems control
        * World object management utilities
        * Brain statistics and debugging tools
        * Developmental stage tracking
        
        Example Usage:
            import pynlm
            
            # Create brain with enhanced configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Create agent and world
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            agent.initialize(world)
            
            # Use episode context manager
            with pynlm.episode_context(brain, agent, world) as episode:
                for step in range(1000):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    )pbdoc";
    
    // Register custom exceptions
    registerCustomExceptions(m);
    
    // Neuron ID class with enhanced functionality
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // Synapse ID class
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
    
    // Region ID class
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
    
    // Population ID class
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
        .def("isExcitatory", [](SynapseType type) { return type == SynapseType::Excitatory; }, "Check if synapse is excitatory")
        .def("isInhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; }, "Check if synapse is inhibitory");
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("isAdult", [](DevelopmentalStage stage) { return stage >= DevelopmentalStage::Adult; }, "Check if stage is adult or later")
        .def("isCritical", [](DevelopmentalStage stage) { return stage == DevelopmentalStage::CriticalPeriod; }, "Check if stage is critical period");
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // Config enhancements
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with validation and presets)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
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
        })
        .def("__len__", &Config::getKeys().size,
             "Get the number of configuration entries")
        .def("__contains__", &Config::has,
             "Check if a key exists in the configuration (supports 'in' operator)")
        .def("__getitem__", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                auto intValue = self.get<int64_t>(key);
                if (intValue.has_value()) {
                    return py::cast(intValue.value());
                }
                auto floatValue = self.get<double>(key);
                if (floatValue.has_value()) {
                    return py::cast(floatValue.value());
                }
                auto boolValue = self.get<bool>(key);
                if (boolValue.has_value()) {
                    return py::cast(boolValue.value());
                }
                throw ConfigError("Key not found: " + key);
            }
            return py::cast(value.value());
        }, py::arg("key"), "Get a configuration value (supports [] operator)")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else {
                throw ConfigError("Unsupported value type for configuration: " + key);
            }
        }, py::arg("key"), py::arg("value"), "Set a configuration value (supports [] operator)")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            if (py::isinstance<py::int_>(defaultValue)) {
                return self.getOr(key, defaultValue.cast<int64_t>());
            } else if (py::isinstance<py::float_>(defaultValue)) {
                return self.getOr(key, defaultValue.cast<double>());
            } else if (py::isinstance<py::bool_>(defaultValue)) {
                return self.getOr(key, defaultValue.cast<bool>());
            } else if (py::isinstance<py::str>(defaultValue)) {
                return self.getOr(key, defaultValue.cast<std::string>());
            } else {
                throw ConfigError("Unsupported default value type: " + key);
            }
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default (supports type flexibility)")
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto value = other.get<std::string>(key);
                if (value.has_value()) {
                    self.set(key, value.value());
                }
            }
        }, py::arg("other"), "Merge another configuration into this one");
    
    addConfigPresets(m.attr("Config"));
    
    // SensoryInput enhancements
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            self.setData(std::vector<float>(width * height * channels, 0.0f));
        }, py::arg("width"), py::arg("height"), py::arg("channels"), "Reshape vision data");
    
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds");
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get the number of signals");
    
    // Action enhancements
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        })
        .def("isEmpty", &Action::isEmpty, "Check if action has no parameters")
        .def("getParameterCount", [](const Action& action) {
            return action.getParameters().size();
        }, "Get the number of parameters");
    
    // WorldObject enhancements
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) + " pos=(" + 
                   std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        })
        .def("getDistance", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to a point");
    
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
        .def("reset", &AgentBody::reset)
        .def("getDistance", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to a point");
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) + " success=" + 
                   std::to_string(result.success) + ">";
        })
        .def("isSuccessful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful");
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        })
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprioception = self.getProprioception();
            auto audio = self.getAudio();
            return vision.getDimensions() + touch.getDimensions() + 
                   internal.getDimensions() + proprioception.getDimensions() +
                   audio.getDimensions();
        }, "Get total number of signals across all modalities");
    
    // SimpleWorld enhancements
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions and vision")
        .def("configureSimple", [](SimpleWorld& self, size_t width, size_t height) {
            self.configure(width, height, width / 3, height / 3);
        }, py::arg("width"), py::arg("height"), "Configure world with square vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation with given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid for agent")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            nlm::WorldObject obj(x, y, nlm::WorldObjectType::Resource, value, 0.5f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource object to the world");
    
    addWorldEnhancements(m.attr("SimpleWorld"));
    
    // Brain enhancements
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initializeWithValidation", [](Brain& self) {
            bool success = self.initialize();
            if (!success) {
                throw BrainError("Failed to initialize brain");
            }
            return success;
        }, "Initialize brain with validation and error handling")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", [](Brain& self, uint64_t step, double time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"), "Perform a simulation step with timestamp")
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
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
             "Get count of pending spike events")
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
        .def("getBrainStatistics", [](const Brain& self) {
            std::stringstream ss;
            ss << "Neuron Count: " << self.getTotalNeuronCount() << "\n";
            ss << "Synapse Count: " << self.getTotalSynapseCount() << "\n";
            ss << "Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "Average Firing Rate: " << self.getAverageFiringRate() << "\n";
            ss << "E/I Ratio: " << self.getExcitationInhibitionRatio() << "\n";
            ss << "Total Spikes: " << self.getTotalSpikeCount();
            return ss.str();
        }, "Get formatted brain statistics")
        .def("getRegionByName", [](Brain& self, const std::string& name) -> nlm::RegionId {
            for (const auto& rid : self.getRegionIds()) {
                auto* region = self.getRegion(rid);
                if (region && region->name == name) {
                    return rid;
                }
            }
            return nlm::INVALID_REGION_ID;
        }, py::arg("name"), "Get region ID by name")
        .def("getRegionNames", [](const Brain& self) {
            std::vector<std::string> names;
            for (const auto& rid : self.getRegionIds()) {
                auto* region = self.getRegion(rid);
                if (region) {
                    names.push_back(region->name);
                }
            }
            return names;
        }, "Get all region names")
        .def("createSnapshot", [](Brain& self, const std::string& filename) {
            self.save(filename);
        }, py::arg("filename"), "Save brain state to file")
        .def("loadSnapshot", [](Brain& self, const std::string& filename) {
            self.load(filename);
        }, py::arg("filename"), "Load brain state from file")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) + 
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        });
    
    addBrainEnhancements(m.attr("Brain"));
    
    // AgentBrain enhancements
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with brain")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("enableAllSubsystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all agent subsystems")
        .def("disableAllSubsystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all agent subsystems")
        .def("getSubsystemStatus", [](const AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get status of all subsystems as a tuple")
        .def("getAgentInfo", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "Neuromodulation Level: " << self.getNeuromodulationLevel() << "\n";
            ss << "Curiosity Level: " << self.getCuriosityLevel() << "\n";
            ss << "Novelty Level: " << self.getNoveltyLevel() << "\n";
            ss << "Prediction Error: " << self.getPredictionError();
            return ss.str();
        }, "Get formatted agent information")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain>";
        });
    
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Configuration presets
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        config->set("brain.region_count", 2, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("world.width", 20, nlm::ConfigSource::Default);
        config->set("world.height", 20, nlm::ConfigSource::Default);
        config->set("world.vision_width", 8, nlm::ConfigSource::Default);
        config->set("world.vision_height", 8, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration with sensible default values");
    
    m.def("createPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.05, nlm::ConfigSource::Default);
        config->set("brain.region_count", 1, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", false, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for performance");
    
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000, nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.15, nlm::ConfigSource::Default);
        config->set("brain.region_count", 4, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for exploration and learning");
    
    // Episode context manager for simulation loops
    py::class_<EpisodeManager>(m, "EpisodeManager", R"pbdoc(Context manager for simulation episodes)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("agent"), py::arg("world"),
             "Create episode manager with brain, agent, and world")
        .def("__enter__", &EpisodeManager::enter,
             "Enter episode context (reset and initialize)")
        .def("__exit__", [](EpisodeManager& self, py::args) {
            self.exit();
        }, "Exit episode context (save state)")
        .def("isInitialized", &EpisodeManager::isInitialized,
             "Check if episode is initialized")
        .def("getStatus", &EpisodeManager::getStatus,
             "Get episode status information");
    
    m.def("episode_context", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world) {
        return EpisodeManager(brain, agent, world);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
       "Create an episode context manager for simulation loops");
    
    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
    
    // Utility functions
    m.def("getVersion", []() {
        return std::string("NLM 0.1.0 - Neural Learning Machine");
    }, "Get NLM version information");
    
    m.def("getPlatformInfo", []() {
        return std::string("C++20 with Python bindings");
    }, "Get platform information");
    
    m.def("validateConfig", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration values and return True if valid");
    
    m.def("createDebugConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100, nlm::ConfigSource::Default);
        config->set("brain.region_count", 1, nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.5, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration suitable for debugging (smaller, denser connections)");
}

} // namespace nlm
