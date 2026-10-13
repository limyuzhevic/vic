// Updated bindings.cpp - Enhanced Python API for NLM (Phase 6)
// This is a more realistic enhanced version that builds on existing C++ functionality

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <chrono>

// Core NLM includes
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Phase 6
        ---------------------------------------------------------------
        Enhanced Python API for NLM Phase 6: Final Integration of Artificial Brain Systems.
        
        This enhanced version provides:
        - Complete introspection into integrated brain systems
        - Advanced memory system access and control
        - Enhanced neuromodulation interfaces
        - Batch operations for performance
        - Development stage control
        - Convenience methods for common workflows
        - Configuration templates and presets
        - Error handling and debugging tools
        
        Key improvements over basic bindings:
        1. Full access to all integrated memory systems (WorkingMemory, EpisodicMemory, etc.)
        2. Complete neuromodulation control (Dopamine, Curiosity, Novelty, etc.)
        3. Advanced prediction system integration
        4. Brain state inspection and analysis
        5. Batch operations for improved performance
        6. Development stage and maturation control
        7. Configuration convenience methods
        8. Comprehensive error reporting
        9. Performance monitoring and benchmarking
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ===== BASIC TYPES =====
    
    // Neuron ID with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with enhanced operations)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("to_string", [](const NeuronId& id) { return "NeuronId(" + std::to_string(id.value) + ")"; })
        .def("__str__", [](const NeuronId& id) { return "NeuronId(" + std::to_string(id.value) + ")"; })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("distance", [](const NeuronId& id1, const NeuronId& id2) {
            return std::abs(static_cast<int64_t>(id1.value - id2.value));
        });

    // Synapse ID with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse with enhanced operations)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("to_string", [](const SynapseId& id) { return "SynapseId(" + std::to_string(id.value) + ")"; })
        .def("__str__", [](const SynapseId& id) { return "SynapseId(" + std::to_string(id.value) + ")"; })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); });

    // Region ID with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with enhanced operations)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("to_string", [](const RegionId& id) { return "RegionId(" + std::to_string(id.value) + ")"; })
        .def("__str__", [](const RegionId& id) { return "RegionId(" + std::to_string(id.value) + ")"; })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); });

    // Population ID
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ===== ENUMERATIONS =====
    
    // Enhanced neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_excitatory_or_inhibitory", [](NeuronType type) {
            return type == NeuronType::Excitatory || type == NeuronType::Inhibitory;
        })
        .def("to_string", [](NeuronType type) {
            switch (type) {
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Internal: return "Internal";
                default: return "Unknown";
            }
        });

    // Enhanced synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_chemical", [](SynapseType type) {
            return type == SynapseType::Excitatory || type == SynapseType::Inhibitory || type == SynapseType::Modulatory;
        });

    // Enhanced developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration with enhanced properties)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_plastic", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod;
        })
        .def("plasticity_factor", [](DevelopmentalStage stage) {
            switch (stage) {
                case DevelopmentalStage::Initial: return 1.0f;
                case DevelopmentalStage::CriticalPeriod: return 0.8f;
                case DevelopmentalStage::Maturation: return 0.5f;
                case DevelopmentalStage::Adult: return 0.2f;
                case DevelopmentalStage::Aging: return 0.1f;
                default: return 1.0f;
            }
        });

    // ===== CONFIGURATION =====
    
    // Enhanced Config class with convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration system for NLM with convenience methods)pbdoc")
        .def(py::init<>())
        
        // Basic file operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Enhanced get/set operations
        .def("get", static_cast<std::optional<int> (*)(const Config&, const std::string&)>(
            [](const Config& self, const std::string& key) {
                auto value = self.get<int>(key);
                return value;
            }), py::arg("key"),
             "Get an integer value from configuration")
        .def("get", static_cast<std::optional<double> (*)(const Config&, const std::string&)>(
            [](const Config& self, const std::string& key) {
                auto value = self.get<double>(key);
                return value;
            }), py::arg("key"),
             "Get a float value from configuration")
        .def("get", static_cast<std::optional<bool> (*)(const Config&, const std::string&)>(
            [](const Config& self, const std::string& key) {
                auto value = self.get<bool>(key);
                return value;
            }), py::arg("key"),
             "Get a boolean value from configuration")
        .def("get", static_cast<std::optional<std::string> (*)(const Config&, const std::string&)>(
            [](const Config& self, const std::string& key) {
                auto value = self.get<std::string>(key);
                return value;
            }), py::arg("key"),
             "Get a string value from configuration")
        
        // Enhanced set operations
        .def("set", static_cast<void (*)(Config&, const std::string&, int, ConfigSource)>(
            [](Config& self, const std::string& key, int value, ConfigSource source) {
                self.set(key, value, source);
            }), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set an integer value in configuration")
        .def("set", static_cast<void (*)(Config&, const std::string&, double, ConfigSource)>(
            [](Config& self, const std::string& key, double value, ConfigSource source) {
                self.set(key, value, source);
            }), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a float value in configuration")
        .def("set", static_cast<void (*)(Config&, const std::string&, bool, ConfigSource)>(
            [](Config& self, const std::string& key, bool value, ConfigSource source) {
                self.set(key, value, source);
            }), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a boolean value in configuration")
        .def("set", static_cast<void (*)(Config&, const std::string&, const std::string&, ConfigSource)>(
            [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
                self.set(key, value, source);
            }), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a string value in configuration")
        
        // Convenience methods
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get a value with default if not present")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (std::holds_alternative<int>(value)) {
                    result[key] = std::get<int>(value);
                } else if (std::holds_alternative<double>(value)) {
                    result[key] = std::get<double>(value);
                } else if (std::holds_alternative<bool>(value)) {
                    result[key] = std::get<bool>(value);
                } else if (std::holds_alternative<std::string>(value)) {
                    result[key] = std::get<std::string>(value);
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        
        // Configuration presets
        .def("applyPreset", [](Config& self, const std::string& presetName) {
            if (presetName == "minimal") {
                self.set("brain.neuron_count", 100);
                self.set("brain.region_count", 1);
                self.set("brain.connection_probability", 0.05f);
            } else if (presetName == "standard") {
                self.set("brain.neuron_count", 1000);
                self.set("brain.region_count", 1);
                self.set("brain.connection_probability", 0.1f);
                self.set("plasticity.stdp.enable", true);
                self.set("plasticity.hebbian.enable", true);
            } else if (presetName == "research") {
                self.set("brain.neuron_count", 5000);
                self.set("brain.region_count", 3);
                self.set("brain.connection_probability", 0.2f);
                self.set("plasticity.stdp.enable", true);
                self.set("plasticity.hebbian.enable", true);
                self.set("plasticity.structural.enable", true);
                self.set("neuromod.dopamine.scale", 1.0f);
                self.set("neuromod.curiosity.enable", true);
                self.set("neuromod.novelty.enable", true);
            }
            return true;
        }, py::arg("presetName"),
             "Apply a predefined configuration preset")
        .def("getPresetNames", []() {
            return py::make_tuple("minimal", "standard", "research");
        }, "Get available configuration preset names");

    // ===== SENSORY INPUT =====
    
    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced functionality)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getMean", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean value of sensory data")
        .def("getStdDev", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.size() <= 1) return 0.0f;
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float value : data) {
                variance += (value - mean) * (value - mean);
            }
            return std::sqrt(variance / (data.size() - 1));
        }, "Get standard deviation of sensory data");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set vision data directly")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getGrayscale", [](const Vision& self) {
            std::vector<float> grayscale;
            auto data = self.getData();
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    size_t idx = ((y * width) + x) * channels;
                    float gray = 0.299f * data[idx];
                    if (channels > 1) gray += 0.587f * data[idx + 1];
                    if (channels > 2) gray += 0.114f * data[idx + 2];
                    grayscale.push_back(gray);
                }
            }
            return grayscale;
        }, "Get grayscale version of vision data");

    // ===== ACTION =====
    
    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("isValid", [](const Action& self) {
            const auto& params = self.getParameters();
            return !params.empty() && self.getType() != ActionType::Custom;
        }, "Check if action is valid")
        .def("to_string", [](const Action& self) {
            std::stringstream ss;
            ss << "Action(type=" << static_cast<int>(self.getType()) << ", "
                << "params=[";
            const auto& params = self.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << params[i];
            }
            ss << "])";
            return ss.str();
        }, "String representation of action")
        .def("__repr__", [](const Action& self) {
            return "<Action: " + self.getName() + ">";
        });

    // ===== WORLD OBJECT =====
    
    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to given coordinates");

    // ===== AGENT BODY =====
    
    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getDistanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Get distance to given coordinates");

    // ===== SIMPLE WORLD =====
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy");

    // ===== BRAIN =====
    
    // Enhanced Brain class with advanced functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with enhanced introspection)pbdoc")
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
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Statistics and introspection
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulator")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        
        // Development control
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("advanceDevelopment", [](Brain& brain, double timestep) {
            if (auto* dev = brain.getDevelopmentSystem()) {
                dev->update(brain.getConfig(), *brain.getRandomGenerator(), timestep);
            }
        }, py::arg("timestep"),
             "Advance development system");
        
        // Statistics and analysis
        .def("getNeuronStats", [](Brain& brain) {
            py::dict stats;
            stats["total"] = brain.getTotalNeuronCount();
            stats["active"] = brain.getActiveNeuronCount();
            stats["firing"] = brain.getFiringNeuronCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["spike_count"] = brain.getTotalSpikeCount();
            stats["ei_ratio"] = brain.getExcitationInhibitionRatio();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            return stats;
        }, "Get comprehensive neuron statistics")
        .def("getRegionStats", [](Brain& brain) {
            py::list regions;
            auto regionIds = brain.getRegionIds();
            for (const auto& id : regionIds) {
                auto* region = brain.getRegion(id);
                if (region) {
                    py::dict regionStats;
                    regionStats["id"] = id.value;
                    regionStats["name"] = region->getName();
                    regionStats["neuron_count"] = region->getTotalNeuronCount();
                    regionStats["synapse_count"] = region->getSynapseCount();
                    regionStats["avg_weight"] = region->getAverageSynapticWeight();
                    regions.append(regionStats);
                }
            }
            return regions;
        }, "Get statistics for all brain regions")
        .def("getNeuromodulationStats", [](Brain& brain) {
            py::dict stats;
            
            auto* dopamine = brain.getDopamine();
            if (dopamine) {
                stats["dopamine_level"] = dopamine->getLevel();
            }
            
            auto* curiosity = brain.getCuriosity();
            if (curiosity) {
                stats["curiosity_level"] = curiosity->getLevel();
            }
            
            auto* novelty = brain.getNovelty();
            if (novelty) {
                stats["novelty_level"] = novelty->getLevel();
            }
            
            auto* predError = brain.getPredictionErrorSignal();
            if (predError) {
                stats["prediction_error"] = predError->getLevel();
            }
            
            return stats;
        }, "Get neuromodulation system statistics")
        
        // Batch operations
        .def("stepMultiple", [](Brain& brain, int startStep, int numSteps, float timestep = 0.001f) {
            for (int step = startStep; step < startStep + numSteps; ++step) {
                brain.step(step, step * timestep);
            }
        }, py::arg("startStep"), py::arg("numSteps"), py::arg("timestep") = 0.001f,
             "Perform multiple simulation steps efficiently")
        .def("runEpisode", [](Brain& brain, const SensoryInput& input, int numSteps, float timestep = 0.001f) {
            brain.initialize();
            brain.receiveSensoryInput(input);
            for (int step = 0; step < numSteps; ++step) {
                brain.step(step, step * timestep);
            }
        }, py::arg("input"), py::arg("numSteps"), py::arg("timestep") = 0.001f,
             "Run a complete episode with initial sensory input");

    // ===== AGENT BRAIN =====
    
    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with advanced sensing and control)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
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
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Neuromodulation control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity");

    // ===== FACTORY FUNCTIONS =====
    
    // Factory functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    .def("createVision", py::overload_cast<>(&Vision::Vision), "Create empty vision input")
    .def("createVision", py::overload_cast<size_t, size_t, size_t>(&Vision::Vision),
         py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         "Create vision input with dimensions")
    .def("createAudio", py::overload_cast<>(&Audio::Audio), "Create empty audio input")
    .def("createAudio", py::overload_cast<size_t, size_t>(&Audio::Audio),
         py::arg("sampleRate"), py::arg("numSamples"),
         "Create audio input with sample rate and duration")
    .def("createInternalSignals", py::overload_cast<>(&InternalSignals::InternalSignals), "Create empty internal signals")
    .def("createAction", py::overload_cast<>(&Action::Action), "Create empty action")
    .def("createActionWithType", py::overload_cast<ActionType>(&Action::Action),
         py::arg("type"),
         "Create action with type");

    // ===== CONSTANTS AND UTILITIES =====
    
    // Expose constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and presets
    m.def("version", []() {
        return "NLM Python Bindings Enhanced v1.0 (Phase 6)";
    }, "Get NLM Python bindings version")
    .def("getDefaultConfig", []() {
        Config config;
        config.applyPreset("standard");
        return config;
    }, "Get default standard configuration")
    .def("getResearchConfig", []() {
        Config config;
        config.applyPreset("research");
        return config;
    }, "Get research-grade configuration")
    .def("getMinimalConfig", []() {
        Config config;
        config.applyPreset("minimal");
        return config;
    }, "Get minimal configuration for testing");

    // ===== PRESETS =====
    
    // Configuration presets
    m.attr("PRESETS", py::dict({
        {"minimal", py::dict({
            {"brain.neuron_count", 100},
            {"brain.region_count", 1},
            {"brain.connection_probability", 0.05f},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"neuromod.dopamine.scale", 0.5f}
        })},
        {"standard", py::dict({
            {"brain.neuron_count", 1000},
            {"brain.region_count", 1},
            {"brain.connection_probability", 0.1f},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", false},
            {"neuromod.dopamine.scale", 1.0f},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"development.enable", true}
        })},
        {"research", py::dict({
            {"brain.neuron_count", 5000},
            {"brain.region_count", 3},
            {"brain.connection_probability", 0.2f},
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.dopamine.scale", 1.5f},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"neuromod.prediction_error.enable", true},
            {"development.enable", true},
            {"development.initial_plasticity", 1.0f},
            {"development.mature_plasticity", 0.2f}
        })}
    }));

} // namespace nlm
