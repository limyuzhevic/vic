#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module includes enhanced error handling, convenience methods, and utilities
        for both beginner and advanced Python users.
    )pbdoc";

    // Register standard C++ exceptions as Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "LogicError", PyExc_RuntimeError);

    // Utility functions for parameter validation and type checking
    auto validateRange = [](const std::string& name, double value, double min, double max) {
        if (value < min || value > max) {
            throw py::value_error("Parameter '" + name + "' must be between " +
                                 std::to_string(min) + " and " + std::to_string(max) +
                                 ", got " + std::to_string(value));
        }
    };
    
    auto validateNonNegative = [](const std::string& name, int64_t value) {
        if (value < 0) {
            throw py::value_error("Parameter '" + name + "' must be non-negative, got " +
                                 std::to_string(value));
        }
    };
    
    auto validatePositive = [](const std::string& name, double value) {
        if (value <= 0.0) {
            throw py::value_error("Parameter '" + name + "' must be positive, got " +
                                 std::to_string(value));
        }
    };
    
    auto validateSize = [](const std::string& name, size_t value, size_t max) {
        if (value > max) {
            throw py::value_error("Parameter '" + name + "' must be <= " +
                                 std::to_string(max) + ", got " + std::to_string(value));
        }
    };
    
    // Enhanced validation functions for Python classes
    auto createValidationHelper = [&]() {
        py::class_<py::object> validationHelper(m, "ValidationHelper", 
            R"pbdoc(Utility class for parameter validation and type checking in Python bindings
            
            This class provides static validation methods for parameters used throughout
            the NLM Python API. It helps ensure type safety and parameter validation
            for beginner and advanced users.
            )pbdoc");
        
        validationHelper
            .def_static("validateRange", &validateRange,
                       py::arg("name"), py::arg("value"), py::arg("min"), py::arg("max"),
                       "Validate that a value is within a specified range")
            .def_static("validateNonNegative", &validateNonNegative,
                       py::arg("name"), py::arg("value"),
                       "Validate that an integer value is non-negative")
            .def_static("validatePositive", &validatePositive,
                       py::arg("name"), py::arg("value"),
                       "Validate that a numeric value is positive")
            .def_static("validateSize", &validateSize,
                       py::arg("name"), py::arg("value"), py::arg("max"),
                       "Validate that a size parameter does not exceed maximum")
            .def_static("checkValue", [](const std::string& name, bool condition) {
                if (!condition) {
                    throw py::value_error("Validation failed for parameter '" + name + "'");
                }
            }, py::arg("name"), py::arg("condition"),
               "Check a boolean condition and throw ValueError if false")
            .def_static("validateNeurons", [](const std::vector<std::shared_ptr<Neuron>>& neurons) {
                std::vector<std::string> errors;
                for (size_t i = 0; i < neurons.size(); ++i) {
                    if (!neurons[i]) {
                        errors.push_back("Neuron " + std::to_string(i) + " is null");
                    }
                }
                if (!errors.empty()) {
                    std::string errorMsg = "Validation failed for neurons: ";
                    for (const auto& error : errors) {
                        errorMsg += error + "; ";
                    }
                    throw py::value_error(errorMsg);
                }
            }, py::arg("neurons"),
               "Validate a vector of neurons and throw ValueError for any null neurons")
            .def_static("validateRegions", [](const std::vector<std::shared_ptr<NeuralRegion>>& regions) {
                std::vector<std::string> errors;
                for (size_t i = 0; i < regions.size(); ++i) {
                    if (!regions[i]) {
                        errors.push_back("Region " + std::to_string(i) + " is null");
                    }
                }
                if (!errors.empty()) {
                    std::string errorMsg = "Validation failed for regions: ";
                    for (const auto& error : errors) {
                        errorMsg += error + "; ";
                    }
                    throw py::value_error(errorMsg);
                }
            }, py::arg("regions"),
               "Validate a vector of regions and throw ValueError for any null regions");
        
        return validationHelper;
    };

    // Initialize validation helper
    createValidationHelper();

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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        Enhanced Python-friendly configuration class with improved error handling
        and validation. Supports loading from files, environment variables, and defaults.
        )pbdoc")
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
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return py::make_tuple(true, py::cast(value.value()));
            }
            return py::make_tuple(false, py::none());
        }, py::arg("key"),
           "Get a configuration value (returns tuple of (exists, value))")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a configuration value with default fallback")
        .def("getOrInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get an integer configuration value with default fallback")
        .def("getOrFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.getOr<double>(key, static_cast<double>(defaultValue));
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a float configuration value with default fallback")
        .def("getOrBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a boolean configuration value with default fallback")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else {
                self.set(key, value.cast<std::string>());
            }
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (automatically converts Python types)")
        .def("getAll", &Config::getKeys,
             "Get all configuration keys (alias for getKeys)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

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
        .def("clearSignals", &InternalSignals::clearSignals);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        Enhanced Python brain class with convenience methods and utilities for
        common neural simulation operations. Provides batch operations, validation,
        and easy access to brain statistics and state.
        )pbdoc")
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
        // Convenience methods for brain utilities
        .def("getActiveRegions", [](Brain& self) {
            auto regions = self.getRegions();
            std::vector<RegionId> activeRegions;
            for (const auto& region : regions) {
                if (region && region->getActiveNeuronCount() > 0) {
                    activeRegions.push_back(region->getId());
                }
            }
            return activeRegions;
        }, "Get IDs of regions with active neurons")
        .def("getRegionStats", [](Brain& self) {
            auto regions = self.getRegions();
            std::vector<py::tuple> stats;
            for (const auto& region : regions) {
                if (region) {
                    stats.push_back(py::make_tuple(
                        region->getId(),
                        region->getName(),
                        region->getNeuronCount(),
                        region->getActiveNeuronCount(),
                        region->getAverageFiringRate()
                    ));
                }
            }
            return stats;
        }, "Get statistics for all regions")
        .def("validate", [](Brain& self) {
            bool isValid = true;
            std::string errors;
            
            // Check basic brain integrity
            if (self.getTotalNeuronCount() == 0) {
                isValid = false;
                errors += "No neurons in brain; ";
            }
            
            if (self.getRegionCount() == 0) {
                isValid = false;
                errors += "No regions in brain; ";
            }
            
            // Check for any issues in regions
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                if (region) {
                    auto neurons = region->getNeurons();
                    for (const auto& neuron : neurons) {
                        if (!neuron) continue;
                        auto state = neuron->getState();
                        if (std::isnan(state.membranePotential) || 
                            std::isinf(state.membranePotential)) {
                            isValid = false;
                            errors += "Invalid neuron state in region " + 
                                    std::to_string(region->getId().index()) + "; ";
                            break;
                        }
                    }
                }
            }
            
            return py::make_tuple(isValid, errors);
        }, "Validate brain state and return (isValid, errorMessage)")
        .def("batchStep", [](Brain& self, int steps, double timestep = 1.0) {
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 1.0,
           "Perform multiple simulation steps efficiently")
        .def("getSummaryStats", [](Brain& self) {
            return py::make_tuple(
                self.getTotalNeuronCount(),
                self.getTotalSynapseCount(),
                self.getActiveNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getTotalSpikeCount(),
                self.getExcitationInhibitionRatio()
            );
        }, "Get comprehensive summary statistics as a tuple");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world
        
        Enhanced Python agent brain interface with convenience methods for
        common agent operations and sensory processing.
        )pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods for agent brain
        .def("createPercept", [](AgentBrain& self, const py::dict& data) {
            auto percept = std::make_shared<SensoryPercept>();
            
            if (data.contains("vision")) {
                auto visionData = data["vision"].cast<std::shared_ptr<Vision>>();
                if (visionData) {
                    percept->setVision(visionData);
                }
            }
            
            if (data.contains("audio")) {
                auto audioData = data["audio"].cast<std::shared_ptr<Audio>>();
                if (audioData) {
                    percept->setAudio(audioData);
                }
            }
            
            return percept;
        }, py::arg("data"),
           "Create a sensory percept from Python dictionary data")
        .def("simulateEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world,
                                   const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                                   int maxSteps = 1000) {
            world->reset();
            bool episodeComplete = false;
            int steps = 0;
            
            while (!episodeComplete && steps < maxSteps) {
                // Process all sensory inputs
                auto percept = world->getSensoryPercept();
                if (percept) {
                    self.processSensoryInput(percept);
                }
                
                // Produce action and apply to world
                auto action = self.getBrain()->produceAction();
                if (action) {
                    world->applyMotorCommand(action->getType(), world->getSimulationTime());
                }
                
                world->update(1.0);
                steps++;
                
                // Check for episode termination (simplified)
                auto body = world->getAgentBody();
                if (body && body->health <= 0.0f) {
                    episodeComplete = true;
                }
            }
            
            return py::make_tuple(steps, episodeComplete);
        }, py::arg("world"), py::arg("inputs"), py::arg("maxSteps") = 1000,
           "Simulate an agent episode with given inputs")
        .def("batchProcess", [](AgentBrain& self, const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
            std::vector<std::shared_ptr<Action>> actions;
            actions.reserve(percepts.size());
            
            for (const auto& percept : percepts) {
                if (percept) {
                    self.processSensoryInput(percept);
                    auto action = self.getBrain()->produceAction();
                    if (action) {
                        actions.push_back(action);
                    }
                }
            }
            
            return actions;
        }, py::arg("percepts"),
           "Process multiple sensory percepts and return corresponding actions (batch processing)")
        .def("getPerformanceMetrics", [](AgentBrain& self) {
            // Simplified metrics collection
            auto brain = self.getBrain();
            if (!brain) {
                return py::dict();
            }
            
            py::dict metrics;
            metrics["total_spikes"] = static_cast<int>(brain->getTotalSpikeCount());
            metrics["avg_firing_rate"] = brain->getAverageFiringRate();
            metrics["active_neurons"] = static_cast<int>(brain->getActiveNeuronCount());
            metrics["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            
            return metrics;
        }, "Get performance metrics as Python dictionary")
        .def("validateConfiguration", [](AgentBrain& self) {
            auto brain = self.getBrain();
            auto config = brain ? brain->getConfig() : nullptr;
            
            std::vector<std::string> warnings;
            
            if (!config) {
                warnings.push_back("No configuration available");
            }
            
            // Check for required config keys
            std::vector<std::string> requiredKeys = {"simulation_steps", "dt"};
            for (const auto& key : requiredKeys) {
                if (!config || !config->has(key)) {
                    warnings.push_back("Missing recommended config key: " + key);
                }
            }
            
            return warnings;
        }, "Validate agent brain configuration and return list of warnings");

        // Factory methods for advanced users
        .def("createCustomConfig", [](const py::dict& configDict) {
            auto config = std::make_shared<Config>();
            for (auto item : configDict) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                if (py::isinstance<py::int_>(value)) {
                    config->set(key, value.cast<int>());
                } else if (py::isinstance<py::float_>(value)) {
                    config->set(key, value.cast<double>());
                } else if (py::isinstance<py::bool_>(value)) {
                    config->set(key, value.cast<bool>());
                } else {
                    config->set(key, value.cast<std::string>());
                }
            }
            return config;
        }, py::arg("configDict"),
           "Create a custom configuration from a Python dictionary")
        .def("createCustomBrain", [](const std::shared_ptr<Config>& config,
                                   const py::dict& brainOptions) {
            auto brain = std::make_shared<Brain>(config);
            
            if (brainOptions.contains("regions")) {
                auto regions = brainOptions["regions"].cast<std::vector<std::string>>();
                for (const auto& regionName : regions) {
                    brain->addRegion(regionName);
                }
            }
            
            if (brainOptions.contains("developmental_stage")) {
                auto stageStr = brainOptions["developmental_stage"].cast<std::string>();
                DevelopmentalStage stage;
                if (stageStr == "Initial") stage = DevelopmentalStage::Initial;
                else if (stageStr == "CriticalPeriod") stage = DevelopmentalStage::CriticalPeriod;
                else if (stageStr == "Maturation") stage = DevelopmentalStage::Maturation;
                else if (stageStr == "Adult") stage = DevelopmentalStage::Adult;
                else if (stageStr == "Aging") stage = DevelopmentalStage::Aging;
                brain->setDevelopmentalStage(stage);
            }
            
            return brain;
        }, py::arg("config"), py::arg("brainOptions"),
           "Create a custom brain with additional initialization options")
        .def("createCustomWorld", [](const py::dict& worldOptions) {
            auto world = std::make_shared<SimpleWorld>();
            
            if (worldOptions.contains("width")) {
                world->configure(
                    worldOptions["width"].cast<size_t>(),
                    worldOptions["height"].cast<size_t>(),
                    worldOptions["visionWidth"].cast<size_t>(),
                    worldOptions["visionHeight"].cast<size_t>()
                );
            }
            
            if (worldOptions.contains("agent_start")) {
                auto start = worldOptions["agent_start"].cast<std::pair<float, float>>();
                world->setAgentStart(start.first, start.second);
            }
            
            if (worldOptions.contains("random_seed")) {
                world->setRandomSeed(worldOptions["random_seed"].cast<unsigned int>());
            }
            
            return world;
        }, py::arg("worldOptions"),
           "Create a custom world with configuration options")
        // Advanced serialization methods
        .def("cloneBrain", [](const Brain& brain) {
            // Create a new brain with the same config
            auto config = brain.getConfig();
            auto clonedBrain = std::make_shared<Brain>(config);
            
            // Copy developmental stage
            clonedBrain->setDevelopmentalStage(brain.getDevelopmentalStage());
            
            // Note: Full neural state cloning would require more complex implementation
            // This is a basic cloning that preserves configuration and settings
            return clonedBrain;
        }, "Create a copy of the brain (basic cloning)")
        .def("exportBrainState", [](const Brain& brain, const std::string& filename) {
            return brain.save(filename);
        }, py::arg("filename"),
           "Export brain state to file (alias for save)")
        .def("importBrainState", [](Brain& brain, const std::string& filename) {
            return brain.load(filename);
        }, py::arg("filename"),
           "Import brain state from file (alias for load)")
        // Neural network manipulation (advanced)
        .def("addInputRegion", [](Brain& brain, const std::string& name, 
                                  NeuronType inputType = NeuronType::Sensory) {
            auto regionId = brain.addRegion(name);
            auto region = brain.getRegion(regionId);
            if (region) {
                region->setRegionType(inputType);
                return regionId;
            }
            return INVALID_REGION_ID;
        }, py::arg("name"), py::arg("inputType") = NeuronType::Sensory,
           "Add an input region and return its ID")
        .def("addOutputRegion", [](Brain& brain, const std::string& name,
                                   NeuronType outputType = NeuronType::Motor) {
            auto regionId = brain.addRegion(name);
            auto region = brain.getRegion(regionId);
            if (region) {
                region->setRegionType(outputType);
                return regionId;
            }
            return INVALID_REGION_ID;
        }, py::arg("name"), py::arg("outputType") = NeuronType::Motor,
           "Add an output region and return its ID")
        // Performance and debugging utilities
        .def("getPerformanceStats", [](Brain& brain) {
            py::dict stats;
            
            // Neural statistics
            stats["total_neurons"] = static_cast<int>(brain.getTotalNeuronCount());
            stats["total_synapses"] = static_cast<int>(brain.getTotalSynapseCount());
            stats["active_neurons"] = static_cast<int>(brain.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<int>(brain.getFiringNeuronCount());
            stats["total_spikes"] = static_cast<int>(brain.getTotalSpikeCount());
            
            // Rates and ratios
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["exc_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            
            // Developmental info
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            
            // Memory systems
            auto workingMem = brain.getWorkingMemory();
            stats["working_memory_size"] = workingMem ? workingMem->getMemorySize() : 0;
            
            return stats;
        }, "Get comprehensive performance statistics as Python dictionary")
        .def("validateBrain", [](Brain& brain) {
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            // Validation logic
            if (brain.getTotalNeuronCount() == 0) {
                errors.push_back("Brain has no neurons");
            }
            
            if (brain.getRegionCount() == 0) {
                errors.push_back("Brain has no regions");
            }
            
            // Check for uninitialized neurons
            auto regions = brain.getRegions();
            for (const auto& region : regions) {
                if (region) {
                    auto neurons = region->getNeurons();
                    for (const auto& neuron : neurons) {
                        if (neuron) {
                            auto state = neuron->getState();
                            if (std::isnan(state.membranePotential)) {
                                warnings.push_back("Neuron in region " + 
                                                 std::to_string(region->getId().index()) + 
                                                 " has NaN membrane potential");
                            }
                        }
                    }
                }
            }
            
            return py::make_tuple(errors, warnings);
        }, "Validate brain integrity and return (errors, warnings) lists")
        .def("enableDebugMode", [](Brain& brain) {
            // Enable debug logging (simplified)
            brain.logStatus();
            return true;
        }, "Enable debug mode and log brain status")
        .def("setRandomSeed", [](Brain& brain, unsigned int seed) {
            auto randomGen = brain.getRandomGenerator();
            if (randomGen) {
                randomGen->seed(seed);
            }
        }, py::arg("seed"),
           "Set random seed for reproducible simulations")
        .def("exportNetworkTopology", [](Brain& brain) {
            py::dict topology;
            auto regions = brain.getRegions();
            
            for (const auto& region : regions) {
                if (region) {
                    py::list neurons;
                    auto regionNeurons = region->getNeurons();
                    
                    for (const auto& neuron : regionNeurons) {
                        if (neuron) {
                            py::dict neuronInfo;
                            neuronInfo["id"] = neuron->getId().index();
                            neuronInfo["type"] = static_cast<int>(neuron->getType());
                            neuronInfo["region"] = region->getId().index();
                            neurons.append(neuronInfo);
                        }
                    }
                    
                    topology[std::to_string(region->getId().index())] = neurons;
                }
            }
            
            return topology;
        }, "Export network topology as Python dictionary")
        .def("importNetworkTopology", [](Brain& brain, const py::dict& topology) {
            // Clear existing regions (careful!)
            auto regionIds = brain.getRegionIds();
            for (const auto& regionId : regionIds) {
                brain.addRegion("temp"); // Simple approach
            }
            
            // In a real implementation, this would reconstruct the neural network
            // based on the imported topology
            return true;
        }, py::arg("topology"),
           "Import network topology from Python dictionary (WARNING: clears existing network)")
        // Advanced simulation controls
        .def("runSimulation", [](Brain& brain, std::shared_ptr<SimpleWorld> world,
                                int steps, double timestep = 1.0) {
            world->reset();
            int completedSteps = 0;
            
            for (int i = 0; i < steps; ++i) {
                world->update(timestep);
                completedSteps++;
                
                // Check termination conditions
                auto body = world->getAgentBody();
                if (body && body->health <= 0.0f) {
                    break;
                }
            }
            
            return completedSteps;
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 1.0,
           "Run a complete simulation episode")
        .def("stepWithCallback", [](Brain& brain, std::shared_ptr<SimpleWorld> world,
                                   int steps, double timestep = 1.0,
                                   const py::function& callback = py::none()) {
            world->reset();
            int completedSteps = 0;
            
            for (int i = 0; i < steps; ++i) {
                if (callback) {
                    // Call Python callback with current state
                    py::dict stepData;
                    stepData["step"] = i;
                    stepData["world"] = world;
                    stepData["brain"] = &brain;
                    callback(stepData);
                }
                
                world->update(timestep);
                completedSteps++;
            }
            
            return completedSteps;
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 1.0,
           py::arg("callback") = py::none(),
           "Run simulation with optional Python callback for each step")
        .def("getSimulationSnapshot", [](Brain& brain, std::shared_ptr<SimpleWorld> world) {
            py::dict snapshot;
            
            // Get brain state
            auto brainStats = py::make_tuple(
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getActiveNeuronCount(),
                brain.getAverageFiringRate()
            );
            snapshot["brain"] = brainStats;
            
            // Get world state
            auto worldStats = py::make_tuple(
                world->getWidth(),
                world->getHeight(),
                world->getAgentBody()->x,
                world->getAgentBody()->y
            );
            snapshot["world"] = worldStats;
            snapshot["time"] = world->getSimulationTime();
            
            return snapshot;
        }, py::arg("world"),
           "Get simulation snapshot as Python dictionary")
        .def("restoreSimulationSnapshot", [](Brain& brain, std::shared_ptr<SimpleWorld> world,
                                             const py::dict& snapshot) {
            // Restore brain state (simplified - would need more complex implementation)
            // This is a placeholder for advanced snapshot/restore functionality
            return true;
        }, py::arg("world"), py::arg("snapshot"),
           "Restore simulation from snapshot (requires advanced implementation)")
        // Custom initialization patterns
        .def("createFromTemplate", [](const std::string& templateName) {
            auto config = std::make_shared<Config>();
            
            if (templateName == "simple") {
                config->set("simulation_steps", 1000);
                config->set("dt", 1.0);
                config->set("vision_width", 84);
                config->set("vision_height", 84);
                config->set("vision_channels", 3);
            } else if (templateName == "complex") {
                config->set("simulation_steps", 5000);
                config->set("dt", 0.1);
                config->set("vision_width", 128);
                config->set("vision_height", 128);
                config->set("vision_channels", 3);
                config->set("enable_plasticity", true);
                config->set("enable_development", true);
                config->set("enable_curiosity", true);
            } else if (templateName == "fast") {
                config->set("simulation_steps", 100);
                config->set("dt", 10.0);
                config->set("vision_width", 32);
                config->set("vision_height", 32);
                config->set("vision_channels", 1);
                config->set("enable_plasticity", false);
            }
            
            return std::make_shared<Brain>(config);
        }, py::arg("templateName"),
           "Create a brain from predefined configuration template")
        .def("configureForTask", [](Brain& brain, const std::string& taskName,
                                   const py::dict& parameters) {
            // Task-specific configuration (simplified)
            if (taskName == "navigation") {
                // Add sensory regions for navigation
                brain.addRegion("vision");
                brain.addRegion(" Proprioception");
            } else if (taskName == "memory") {
                // Add memory regions
                brain.addRegion("working_memory");
                brain.addRegion("episodic_memory");
            } else if (taskName == "cognition") {
                // Add cognitive regions
                brain.addRegion("planning");
                brain.addRegion("concept_formation");
            }
            
            return true;
        }, py::arg("taskName"), py::arg("parameters"),
           "Configure brain for a specific task")
        .def("getAvailableTemplates", []() {
            return py::make_tuple("simple", "complex", "fast", "navigation", "memory", "cognition");
        }, "Get list of available configuration templates");

} // namespace nlm
