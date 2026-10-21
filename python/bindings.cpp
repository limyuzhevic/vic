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

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidArgumentError : public std::runtime_error {
public:
    explicit InvalidArgumentError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& message) : std::runtime_error(message) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& message) : std::runtime_error(message) {}
};

// Helper function to convert C++ exceptions to Python
void register_nlm_exceptions(py::module_& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const InvalidArgumentError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

PYBIND11_MODULE(pynlm, m) {
    register_nlm_exceptions(m);
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Complete brain simulation with spiking neurons
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and structural plasticity
        - Prediction and cognition systems
        - Checkpoint/save-load functionality
        - Comprehensive error handling and validation
        
        Quick Start:
        ```python
        import pynlm
        
        # Create a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
            if step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        ```
    )pbdoc";

    // Set version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "An experimental artificial developmental brain for neural simulation";

    // Register custom exceptions for better error handling
    py::class_<NLMException, std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<InvalidArgumentError, std::runtime_error>(m, "InvalidArgumentError")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, std::runtime_error>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError, std::runtime_error>(m, "SimulationError")
        .def(py::init<const std::string&>());

    // Register basic std::runtime_error for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);

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
        // Enhanced Config methods
        .def("copy", [](const Config& self) {
            auto copy = std::make_shared<Config>(self);
            return copy;
        }, "Create a copy of this configuration")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("compare", [](const Config& self, const Config& other) {
            std::vector<std::string> differences;
            
            auto self_keys = self.getKeys();
            auto other_keys = other.getKeys();
            
            for (const auto& key : self_keys) {
                if (!other.has(key)) {
                    differences.push_back("Missing in other: " + key);
                } else {
                    auto self_val = self.getAsString(key);
                    auto other_val = other.getAsString(key);
                    if (self_val != other_val) {
                        differences.push_back("Different: " + key + " (self: " + 
                                            (self_val ? *self_val : "null") + 
                                            ", other: " + 
                                            (other_val ? *other_val : "null") + ")");
                    }
                }
            }
            
            for (const auto& key : other_keys) {
                if (!self.has(key)) {
                    differences.push_back("Missing in self: " + key);
                }
            }
            
            return differences;
        }, "Compare this configuration with another and return differences")
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            
            // Basic validation
            if (!self.has("neuron_count")) {
                errors.push_back("Missing required key: neuron_count");
            } else {
                auto neuron_count = self.get<int64_t>("neuron_count");
                if (neuron_count && *neuron_count < 10) {
                    errors.push_back("neuron_count should be at least 10");
                }
            }
            
            if (!self.has("simulation_timestep")) {
                errors.push_back("Missing required key: simulation_timestep");
            } else {
                auto timestep = self.get<double>("simulation_timestep");
                if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
                    errors.push_back("simulation_timestep must be between 0.0 and 1.0");
                }
            }
            
            if (errors.empty()) {
                return true;
            } else {
                std::string error_msg = "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  - " + error + "\n";
                }
                throw ConfigurationError(error_msg);
            }
        }, "Validate configuration and throw ConfigurationError on issues")
        .def("__eq__", [](const Config& self, const Config& other) {
            return self.getKeys() == other.getKeys();
        }, "Check if two configurations are equal")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__copy__", [](Config& self) {
            return Config(self);
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // Add convenience functions for easier Python usage
    m.def("quick_brain", [](size_t neuron_count, const std::string& config_file) {
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count"), py::arg("config_file") = "",
         "Create a brain with specified neuron count and optional config file");

    m.def("run_agent_simulation", [](std::shared_ptr<AgentBrain> agent, 
                                     std::shared_ptr<SimpleWorld> world, 
                                     size_t num_steps,
                                     bool enable_learning) {
        
        std::vector<std::unordered_map<std::string, double>> stats;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get percept
            auto percept = world->getSensoryPercept();
            
            // Process input
            agent->processSensoryInput(percept);
            
            // Get brain's step number
            auto brain = agent->getBrain();
            brain->step(static_cast<SimulationStep>(step));
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply learning if enabled
            if (enable_learning) {
                double reward = 1.0; // Simple reward for movement
                double predicted = 0.5;
                agent->applyRewardModulation(reward, predicted);
            }
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Collect statistics
            std::unordered_map<std::string, double> step_stats;
            step_stats["step"] = step;
            step_stats["neurons_firing"] = brain->getFiringNeuronCount();
            step_stats["total_spikes"] = brain->getTotalSpikeCount();
            step_stats["average_firing_rate"] = brain->getAverageFiringRate();
            step_stats["action_type"] = static_cast<int>(action->getType());
            stats.push_back(step_stats);
        }
        
        // Return final statistics
        std::unordered_map<std::string, double> final_stats;
        auto final_brain = agent->getBrain();
        final_stats["final_spike_count"] = final_brain->getTotalSpikeCount();
        final_stats["final_firing_neurons"] = final_brain->getFiringNeuronCount();
        final_stats["final_firing_rate"] = final_brain->getAverageFiringRate();
        final_stats["total_steps"] = num_steps;
        
        return std::make_pair(stats, final_stats);
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), py::arg("enable_learning") = false,
         "Run a complete agent simulation with statistics collection");

    m.def("create_brain_with_preset", [](const std::string& preset_name, size_t neuron_count) {
        auto config = std::make_shared<Config>();
        
        if (preset_name == "simple") {
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
        } else if (preset_name == "complex") {
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
        } else if (preset_name == "optimized") {
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.02f, ConfigSource::Default);
        } else {
            throw InvalidArgumentError("Unknown preset: " + preset_name);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("preset_name"), py::arg("neuron_count"),
         "Create a brain with predefined configuration presets");

    m.def("create_agent_with_preset", [](const std::string& preset_name, std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (preset_name == "explorer") {
            agent->enableCuriosity(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
        } else if (preset_name == "learner") {
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
        } else if (preset_name == "methodical") {
            agent->enableDevelopment(true);
            agent->enableCuriosity(false);
        } else {
            throw InvalidArgumentError("Unknown agent preset: " + preset_name);
        }
        
        return agent;
    }, py::arg("preset_name"), py::arg("brain"),
         "Create an agent brain with predefined behavior preset");

    m.def("save_config_to_file", [](std::shared_ptr<Config> config, const std::string& filepath) {
        bool success = config->saveToFile(filepath);
        if (!success) {
            throw ConfigurationError("Failed to save configuration to: " + filepath);
        }
        return true;
    }, py::arg("config"), py::arg("filepath"),
         "Save configuration to file with error handling");

    m.def("load_config_from_file", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
        return config;
    }, py::arg("filepath"),
         "Load configuration from file with error handling");

    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Check for required keys
        if (!config->has("neuron_count")) {
            errors.push_back("Missing required key: neuron_count");
        } else {
            auto neuron_count = config->get<int64_t>("neuron_count");
            if (neuron_count && *neuron_count < 10) {
                errors.push_back("neuron_count should be at least 10");
            }
        }
        
        if (!config->has("simulation_timestep")) {
            errors.push_back("Missing required key: simulation_timestep");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw ConfigurationError(error_msg);
        }
        
        return true;
    }, py::arg("config"),
         "Validate configuration and throw ConfigurationError on issues");

    m.def("quick_experiment", [](const std::string& experiment_type, size_t steps) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::unordered_map<std::string, double> results;
        
        if (experiment_type == "memory") {
            // Run memory experiment
            for (size_t step = 0; step < steps; ++step) {
                brain->step(static_cast<SimulationStep>(step));
                if (step % 10 == 0) {
                    results["step_" + std::to_string(step)] = brain->getTotalSpikeCount();
                }
            }
            results["experiment_type"] = "memory";
        } else if (experiment_type == "learning") {
            // Run learning experiment with injected input
            for (size_t step = 0; step < steps; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                brain->step(static_cast<SimulationStep>(step));
                if (step % 10 == 0) {
                    results["step_" + std::to_string(step)] = brain->getTotalSpikeCount();
                }
            }
            results["experiment_type"] = "learning";
        } else if (experiment_type == "plasticity") {
            // Run plasticity experiment
            for (size_t step = 0; step < steps; ++step) {
                brain->step(static_cast<SimulationStep>(step));
                if (step % 10 == 0) {
                    results["step_" + std::to_string(step)] = brain->getTotalSpikeCount();
                }
            }
            results["experiment_type"] = "plasticity";
        } else {
            throw InvalidArgumentError("Unknown experiment type: " + experiment_type);
        }
        
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["total_steps"] = steps;
        
        return results;
    }, py::arg("experiment_type"), py::arg("steps"),
         "Run quick predefined experiments with automatic statistics collection");

} // namespace nlm
