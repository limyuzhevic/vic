#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for common patterns
namespace detail {
    
    // Safe type checking helper
    template<typename T>
    T get_typed_attribute(py::object obj, const std::string& attr_name) {
        try {
            return obj.attr(attr_name.c_str()).cast<T>();
        } catch (const py::error_already_set& e) {
            throw py::value_error("Failed to get attribute " + attr_name);
        }
    }
    
    // Validation helper for numeric ranges
    template<typename T>
    void validate_range(T value, T min, T max, const std::string& param_name) {
        if (value < min || value > max) {
            throw py::value_error("Parameter " + param_name + " must be in range [" + 
                                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }
    
    // Safe vector size checking
    void validate_vector_size(const std::vector<float>& data, size_t expected_size, 
                             const std::string& param_name) {
        if (data.size() != expected_size) {
            throw py::value_error("Parameter " + param_name + " must have exactly " +
                                std::to_string(expected_size) + " elements, got " +
                                std::to_string(data.size()));
        }
    }
}

// Convenience factory functions for common configurations
namespace factories {
    
    // Create a standard experimental configuration
    std::shared_ptr<Config> createExperimentalConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
        return config;
    }
    
    // Create a minimal configuration
    std::shared_ptr<Config> createMinimalConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        return config;
    }
    
    // Create a high-performance configuration
    std::shared_ptr<Config> createHighPerformanceConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, nlm::ConfigSource::Default); // Disable plasticity for speed
        config->set("plasticity.hebbian.enable", false, nlm::ConfigSource::Default);
        config->set("plasticity.structural.enable", false, nlm::ConfigSource::Default);
        return config;
    }
}

// Monitoring and statistics helpers
namespace monitoring {
    
    // Get brain health summary
    py::dict getBrainHealthSummary(const std::shared_ptr<Brain>& brain) {
        py::dict summary;
        summary["total_neurons"] = brain->getTotalNeuronCount();
        summary["total_synapses"] = brain->getTotalSynapseCount();
        summary["firing_neurons"] = brain->getFiringNeuronCount();
        summary["active_neurons"] = brain->getActiveNeuronCount();
        summary["total_spikes"] = brain->getTotalSpikeCount();
        summary["avg_firing_rate"] = brain->getAverageFiringRate();
        summary["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        summary["firing_neuron_count"] = brain->getFiringNeuronCount();
        summary["region_count"] = brain->getRegionCount();
        return summary;
    }
    
    // Check if brain is in a stable state
    bool isBrainStable(const std::shared_ptr<Brain>& brain, float max_firing_rate = 0.8) {
        float firing_rate = brain->getAverageFiringRate();
        return firing_rate < max_firing_rate;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Authors
        License: MIT
    )pbdoc";
    
    // Module version
    m.attr("__version__") = "0.1.0";
    
    // Register standard exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    //=============================================================
    // Section 1: ID Classes (NeuronId, SynapseId, etc.)
    //=============================================================
    
    // NeuronId class
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
    
    // SynapseId class
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });
    
    // RegionId class
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });
    
    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });
    
    //=============================================================
    // Section 2: Enumerations
    //=============================================================
    
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // ActionType enumeration
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
    
    // MotorCommand enumeration
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
    
    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    //=============================================================
    // Section 3: Config Class
    //=============================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Pythonic convenience methods
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"),
             "Get a configuration value by key (returns empty string if not found)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration key and value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration key to an integer value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration key to a float value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration key to a boolean value")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get(key);
            try {
                return std::stoll(value);
            } catch (...) {
                throw py::value_error("Cannot convert value to integer: " + value);
            }
        }, py::arg("key"),
             "Get a configuration value as integer")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get(key);
            try {
                return std::stod(value);
            } catch (...) {
                throw py::value_error("Cannot convert value to float: " + value);
            }
        }, py::arg("key"),
             "Get a configuration value as float")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get(key);
            // Simple boolean conversion
            std::string lower_value = value;
            std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(),
                          [](unsigned char c) { return std::tolower(c); });
            return lower_value == "true" || lower_value == "1" || lower_value == "yes";
        }, py::arg("key"),
             "Get a configuration value as boolean");
    
    //=============================================================
    // Section 4: SensoryInput Classes
    //=============================================================
    
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
        .def("getChannels", &Vision::getChannels);
    
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getSignals);
    
    //=============================================================
    // Section 5: Action and WorldObject Classes
    //=============================================================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        });
    
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " x=" + std::to_string(obj.x) + " y=" + std::to_string(obj.y) + ">";
        });
    
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to a position");
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", &ActionResult::success)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + ">";
        });
    
    //=============================================================
    // Section 6: SensoryPercept Class
    //=============================================================
    
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("addVisionSignal", [](SensoryPercept& self, float x, float y, float intensity) {
            auto vision = self.getVision();
            // Add a simple point to vision (simplified implementation)
            vision.push_back(intensity);
            self.setVision(vision);
        }, py::arg("x"), py::arg("y"), py::arg("intensity"),
             "Add a vision signal (simplified)")
        .def("addInternalSignal", &SensoryPercept::addInternalSignal, py::arg("value"),
             "Add an internal signal");
    
    //=============================================================
    // Section 7: SimpleWorld Class
    //=============================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision settings")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
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
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        // Pythonic convenience methods
        .def("getDistance", [](const SimpleWorld& self, float x1, float y1, float x2, float y2) {
            float dx = x1 - x2;
            float dy = y1 - y2;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
             "Calculate distance between two positions")
        .def("getObjectsWithinRadius", [](const SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> objects;
            // This would require additional world implementation
            // For now, return empty vector
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get objects within radius (placeholder)");
    
    //=============================================================
    // Section 8: Brain Class
    //=============================================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        // Pythonic convenience methods and statistics
        .def("getHealthSummary", [](const std::shared_ptr<Brain>& brain) {
            return monitoring::getBrainHealthSummary(brain);
        }, "Get brain health summary as dictionary")
        .def("isStable", [](const std::shared_ptr<Brain>& brain, float max_firing_rate = 0.8) {
            return monitoring::isBrainStable(brain, max_firing_rate);
        }, py::arg("max_firing_rate") = 0.8,
             "Check if brain is in a stable firing state")
        .def("runSteps", [](std::shared_ptr<Brain>& brain, size_t num_steps) {
            for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
                brain->step(step);
            }
        }, py::arg("num_steps"),
             "Run multiple simulation steps")
        .def("runTimedSteps", [](std::shared_ptr<Brain>& brain, size_t num_steps, float timestep) {
            for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
                brain->step(step, step * timestep);
            }
        }, py::arg("num_steps"), py::arg("timestep"),
             "Run multiple simulation steps with timestamps")
        .def("getMetrics", [](const std::shared_ptr<Brain>& brain) {
            py::dict metrics;
            metrics["neuron_count"] = brain->getTotalNeuronCount();
            metrics["synapse_count"] = brain->getTotalSynapseCount();
            metrics["firing_neuron_count"] = brain->getFiringNeuronCount();
            metrics["active_neuron_count"] = brain->getActiveNeuronCount();
            metrics["total_spikes"] = brain->getTotalSpikeCount();
            metrics["firing_rate"] = brain->getAverageFiringRate();
            metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            metrics["region_count"] = brain->getRegionCount();
            return metrics;
        }, "Get brain metrics as dictionary");
    
    //=============================================================
    // Section 9: AgentBrain Class
    //=============================================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
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
        // Pythonic convenience methods
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning and adaptation systems")
        .def("disableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning and adaptation systems")
        .def("getSystemStatus", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get system status as dictionary");
    
    //=============================================================
    // Section 10: Factory Functions and Global Constants
    //=============================================================
    
    // Factory functions for common configurations
    m.def("createDefaultConfig", &factories::createExperimentalConfig,
          "Create a standard experimental configuration");
    
    m.def("createMinimalConfig", &factories::createMinimalConfig,
          "Create a minimal configuration for testing");
    
    m.def("createHighPerformanceConfig", &factories::createHighPerformanceConfig,
          "Create a high-performance configuration (plasticity disabled)");
    
    // Core factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain");
    
    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = factories::createExperimentalConfig();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize a brain with experimental configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        return world;
    }, "Create and configure a simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface");
    
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Python-style constants
    m.attr("VERSION") = "0.1.0";
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_VISION_WIDTH") = 8;
    m.attr("DEFAULT_VISION_HEIGHT") = 8;
    
    // Common neuron type shortcuts
    m.attr("EXCITATORY_NEURON") = NeuronType::Excitatory;
    m.attr("INHIBITORY_NEURON") = NeuronType::Inhibitory;
    m.attr("SENSORY_NEURON") = NeuronType::Sensory;
    m.attr("MOTOR_NEURON") = NeuronType::Motor;
    
    //=============================================================
    // Section 11: Integration and Utilities
    //=============================================================
    
    // Complete agent setup utility
    m.def("createCompleteAgent", [](std::shared_ptr<Config> config,
                                   size_t world_width = 20,
                                   size_t world_height = 20,
                                   size_t vision_width = 8,
                                   size_t vision_height = 8) {
        // Create brain and initialize
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world and configure
        auto world = std::make_shared<SimpleWorld>();
        world->configure(world_width, world_height, vision_width, vision_height);
        world->reset();
        
        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all systems for full learning capability
        agent->enableAllSystems();
        
        // Return tuple of (brain, world, agent)
        return py::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("world_width") = 20, py::arg("world_height") = 20,
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a complete agent setup (brain, world, agent)");
    
    // Convenience simulation runner
    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain,
                              std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent,
                              size_t num_steps,
                              float timestep = 0.1) {
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation (simplified)
            float reward = 0.0f;
            if (percept.getInternal() && !percept.getInternal()->empty()) {
                reward = percept.getInternal()->back();
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(timestep);
        }
        
        return py::dict(
            "final_spike_count", brain->getTotalSpikeCount(),
            "final_firing_neurons", brain->getFiringNeuronCount(),
            "health_summary", monitoring::getBrainHealthSummary(brain)
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("timestep") = 0.1,
       "Run a complete agent episode and return results");
    
    // Error handling utilities
    m.def("safe_brain_step", [](std::shared_ptr<Brain>& brain, nlm::SimulationStep step) {
        try {
            brain->step(step);
            return true;
        } catch (const std::exception& e) {
            throw py::runtime_error(std::string("Brain step failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"),
       "Safely execute a brain step with error handling");
    
    m.def("validate_brain_config", [](const std::shared_ptr<Config>& config) {
        py::dict errors;
        
        // Validate required keys
        if (!config->has("brain.neuron_count")) {
            errors["missing_neuron_count"] = "brain.neuron_count is required";
        }
        
        if (!config->has("brain.region_count")) {
            errors["missing_region_count"] = "brain.region_count is required";
        }
        
        // Validate values
        if (config->has("brain.neuron_count")) {
            try {
                int64_t neuron_count = std::stoll(config->get("brain.neuron_count"));
                if (neuron_count < 10) {
                    errors["invalid_neuron_count"] = "brain.neuron_count must be at least 10";
                }
            } catch (...) {
                errors["invalid_neuron_count"] = "brain.neuron_count must be an integer";
            }
        }
        
        return errors;
    }, py::arg("config"),
       "Validate brain configuration and return any errors");
}

} // namespace nlm
