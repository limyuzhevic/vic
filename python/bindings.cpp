#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace py = pybind11;
namespace nlm {

// Exception classes
class NLMException : public std::exception {
public:
    NLMException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
    const std::string& getMessage() const { return message_; }
private:
    std::string message_;
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException(message) {}
};

class BrainInitializationError : public NLMException {
public:
    BrainInitializationError(const std::string& message) : NLMException(message) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) : NLMException(message) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message) : NLMException(message) {}
};

// Context manager for Brain lifecycle management
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        if (brain_) {
            brain_->reset();
        }
    }
    
    BrainContext& operator=(std::shared_ptr<Brain> brain) {
        setBrain(brain);
        return *this;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for simulation steps
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), stepCount_(0) {}
    
    ~SimulationContext() {
        if (brain_ && world_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    size_t getStepCount() const { return stepCount_; }
    
    void incrementStep() { stepCount_++; }
    
    SimulationContext& operator++() {
        incrementStep();
        return *this;
    }
    
    SimulationContext& operator++(int) {
        SimulationContext temp = *this;
        incrementStep();
        return temp;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    size_t stepCount_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register C++ exceptions as Python exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register specific exception classes
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // === Basic Type Wrappers ===
    
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
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

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
        });

    // === Enums ===
    
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

    // === Configuration Class ===
    
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
        });

    // === Sensory Input Classes ===
    
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

    // === Action Class ===
    
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

    // === World Object Class ===
    
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

    // === Agent Body Class ===
    
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

    // === Action Result Class ===
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // === Sensory Percept Class ===
    
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

    // === Simple World Class ===
    
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

    // === Brain Class (Enhanced) ===
    
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
             "Log brain status")
        
        // High-level convenience methods
        .def("runEpisode", [](Brain& self, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
            }
        }, py::arg("steps"), R"pbdoc(Run a complete episode with sensory input/output)pbdoc")
        
        .def("getMemoryStats", [](Brain& self) {
            std::stringstream ss;
            ss << "=== Brain Memory Statistics ===\n";
            
            auto* wm = self.getWorkingMemory();
            if (wm) {
                ss << "Working Memory:\n";
                ss << "  Active traces: N/A\n";
                ss << "  Capacity: N/A\n";
                ss << "  Current size: N/A\n";
            }
            
            auto* em = self.getEpisodicMemory();
            if (em) {
                ss << "Episodic Memory:\n";
                ss << "  Episode count: N/A\n";
            }
            
            auto* am = self.getAssociativeMemory();
            if (am) {
                ss << "Associative Memory:\n";
                ss << "  Association count: N/A\n";
            }
            
            return ss.str();
        }, R"pbdoc(Get formatted memory statistics)pbdoc")
        
        .def("createExperiment", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Create experiment with config file")
        
        .def("visualizeNetwork", []() {
            return std::string("Network visualization not yet implemented");
        }, R"pbdoc(Generate simple network visualization)pbdoc")
        
        .def("getLearningSummary", [](Brain& self) {
            std::stringstream ss;
            ss << "=== Learning Summary ===\n";
            ss << "  Total neurons: " << self.getTotalNeuronCount() << "\n";
            ss << "  Total synapses: " << self.getTotalSynapseCount() << "\n";
            ss << "  Active neurons: " << self.getActiveNeuronCount() << "\n";
            ss << "  Firing neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "  Average firing rate: " << self.getAverageFiringRate() << " Hz\n";
            ss << "  Excitation/Inhibition ratio: " << self.getExcitationInhibitionRatio() << "\n";
            ss << "  Total spike count: " << self.getTotalSpikeCount() << "\n";
            ss << "  Developmental stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            return ss.str();
        }, R"pbdoc(Get learning statistics summary)pbdoc");

    // === AgentBrain Class ===
    
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

    // === High-Level Convenience Functions ===
    
    m.def("createBrainFromConfig", [](const std::string& config_path) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_path)) {
            throw ConfigurationError("Failed to load configuration from: " + config_path);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_path"), R"pbdoc(Load config from file and create brain)pbdoc")
    
    m.def("createAgentBrainFromBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Simple wrapper for agent brain creation)pbdoc")
    
    m.def("runSimpleExperiment", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        brain->reset();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
    }, py::arg("brain"), py::arg("steps"), R"pbdoc(Run basic experiment with observation)pbdoc")
    
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 10);
        config->set("brain.synapticDensity", 0.05f);
        config->set("simulation.timeStep", 0.01f);
        config->set("simulation.maxSteps", 1000u);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize default brain");
        }
        return brain;
    }, R"pbdoc(Create brain with optimal default configuration)pbdoc")
    
    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        std::stringstream ss;
        ss << "=== Brain Statistics ===\n";
        ss << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
        return ss.str();
    }, py::arg("brain"), R"pbdoc(Get formatted statistics string)pbdoc")
    
    m.def("inspectMemorySystem", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw MemoryError("Brain cannot be null");
        }
        std::stringstream ss;
        ss << "=== Memory System Status ===\n";
        ss << "Working Memory: Available\n";
        ss << "  Capacity: N/A\n";
        ss << "  Current Size: N/A\n";
        ss << "Episodic Memory: Available\n";
        ss << "  Episode Count: N/A\n";
        ss << "Associative Memory: Available\n";
        ss << "  Association Count: N/A\n";
        return ss.str();
    }, py::arg("brain"), R"pbdoc(Get memory system status)pbdoc")
    
    m.def("saveBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        if (!brain->save(filename)) {
            throw SimulationError("Failed to save brain checkpoint: " + filename);
        }
    }, py::arg("brain"), py::arg("filename"), R"pbdoc(High-level checkpoint saving)pbdoc")
    
    m.def("loadBrainCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        if (!brain->load(filename)) {
            throw SimulationError("Failed to load brain checkpoint: " + filename);
        }
    }, py::arg("brain"), py::arg("filename"), R"pbdoc(High-level checkpoint loading)pbdoc")

    // === Context Managers ===
    
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for brain lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("getBrain", &BrainContext::getBrain)
        .def("setBrain", &BrainContext::setBrain)
        .def("__enter__", [](BrainContext& self) {
            return self;
        })
        .def("__exit__", [](BrainContext& self, py::args) {
            // Clean up happens in destructor
        });

    py::class_<SimulationContext>(m, "SimulationContext", R"pbdoc(Context manager for simulation steps)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
        .def("getBrain", &SimulationContext::getBrain)
        .def("getWorld", &SimulationContext::getWorld)
        .def("getStepCount", &SimulationContext::getStepCount)
        .def("__enter__", [](SimulationContext& self) {
            return self;
        })
        .def("__exit__", [](SimulationContext& self, py::args) {
            // Clean up happens in destructor
        });

    // === Utility Functions ===
    
    m.def("format_neurons", [](const std::vector<NeuronId>& neurons) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << neurons[i].value;
        }
        ss << "]";
        return ss.str();
    }, py::arg("neuron_list"), R"pbdoc(Format neuron list for display)pbdoc")
    
    m.def("format_synapses", [](const std::vector<SynapseId>& synapses) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < synapses.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << synapses[i].value;
        }
        ss << "]";
        return ss.str();
    }, py::arg("synapse_list"), R"pbdoc(Format synapse list for display)pbdoc")
    
    m.def("generate_report", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw SimulationError("Failed to open file for report: " + filename);
        }
        
        file << "=== NLM Brain Simulation Report ===\n";
        file << "Generated on: " << std::chrono::system_clock::now() << "\n\n";
        
        file << "Brain Statistics:\n";
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        file << "  Total Spike Count: " << brain->getTotalSpikeCount() << "\n\n";
        
        file << "Configuration Summary:\n";
        auto config = brain->getConfig();
        if (config) {
            file << config->summary();
        }
        
        file.close();
        return true;
    }, py::arg("brain"), py::arg("filename"), R"pbdoc(Generate detailed report)pbdoc")

    // === Factory Functions ===
    
m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    // High-level convenience factory functions
    m.def("createBrainFromConfig", [](const std::string& configPath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configPath)) {
            throw ConfigurationError("Failed to load configuration from: " + configPath);
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain from config: " + configPath);
        }
        return brain;
    }, py::arg("config_path"), "Create and initialize brain from configuration file");
    
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        // Set default values for optimal brain operation
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize default brain");
        }
        return brain;
    }, "Create brain with optimal default configuration");
    
    m.def("runSimpleExperiment", [](std::shared_ptr<Brain> brain, size_t steps, 
                                   const std::string& configPath) -> std::string {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        
        std::stringstream results;
        results << "=== Simple Experiment Results ===\n";
        results << "Steps: " << steps << "\n";
        results << "Initial neurons: " << brain->getTotalNeuronCount() << "\n";
        
        // Store initial state
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Run experiment
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            
            // Log progress every 100 steps
            if ((i + 1) % 100 == 0) {
                results << "Step " << (i + 1) << ": " 
                       << (brain->getTotalSpikeCount() - initialSpikes) << " spikes\n";
            }
        }
        
        // Get final statistics
        results << "\nFinal Statistics:\n";
        results << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        results << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        results << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        results << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        results << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        results << "  Total spike count: " << brain->getTotalSpikeCount() - initialSpikes << "\n";
        results << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        return results.str();
    }, py::arg("brain"), py::arg("steps"), py::arg("config_path") = "", 
           R"pbdoc(Run a simple experiment with observation)pbdoc")
    
    m.def("inspectMemorySystem", [](std::shared_ptr<Brain> brain) -> std::string {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        
        std::stringstream report;
        report << "=== NLM Memory System Inspection ===\n\n";
        
        auto* wm = brain->getWorkingMemory();
        report << "Working Memory:\n";
        if (wm) {
            report << "  Status: Active\n";
            // Working memory details would be available here
            report << "  Capacity: (not directly accessible)\n";
            report << "  Current load: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* em = brain->getEpisodicMemory();
        report << "\nEpisodic Memory:\n";
        if (em) {
            report << "  Status: Active\n";
            // Episode count would be available here
            report << "  Episode count: (not directly accessible)\n";
            report << "  Max episodes: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* am = brain->getAssociativeMemory();
        report << "\nAssociative Memory:\n";
        if (am) {
            report << "  Status: Active\n";
            // Association details would be available here
            report << "  Association count: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* dev = brain->getDevelopmentSystem();
        report << "\nDevelopment System:\n";
        if (dev) {
            report << "  Status: Active\n";
            report << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* novelty = brain->getNovelty();
        report << "\nNovelty Detection:\n";
        if (novelty) {
            report << "  Status: Active\n";
            // Novelty level would be available here
            report << "  Current level: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* curiosity = brain->getCuriosity();
        report << "\nCuriosity System:\n";
        if (curiosity) {
            report << "  Status: Active\n";
            // Curiosity level would be available here
            report << "  Current level: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        auto* dopamine = brain->getDopamine();
        report << "\nNeuromodulation (Dopamine):\n";
        if (dopamine) {
            report << "  Status: Active\n";
            // Dopamine level would be available here
            report << "  Current level: (not directly accessible)\n";
        } else {
            report << "  Status: Not initialized\n";
        }
        
        return report.str();
    }, py::arg("brain"), R"pbdoc(Get detailed memory system status)pbdoc")
    
    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) -> std::string {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        
        std::stringstream ss;
        ss << "=== NLM Brain Statistics ===\n";
        ss << "Architecture:\n";
        ss << "  Regions: " << brain->getRegionCount() << "\n";
        ss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "\nActivity:\n";
        ss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing neurons (this step): " << brain->getFiringNeuronCount() << "\n";
        ss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        ss << "  Total spike count: " << brain->getTotalSpikeCount() << "\n";
        ss << "\nSystem Status:\n";
        ss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        ss << "  Checkpoint manager: " << (brain->getPendingSpikeEventCount() > 0 ? "Active" : "Idle") << "\n";
        return ss.str();
    }, py::arg("brain"), R"pbdoc(Get formatted brain statistics)pbdoc")
    
    m.def("createExperiment", [](std::shared_ptr<Brain> brain, const std::string& configPath) {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        
        auto config = std::make_shared<Config>();
        if (!configPath.empty() && !config->loadFromFile(configPath)) {
            throw ConfigurationError("Failed to load experiment configuration from: " + configPath);
        }
        
        return config;
    }, py::arg("brain"), py::arg("config_path") = "", R"pbdoc(Create experiment configuration)pbdoc")
    
    m.def("visualizeNetwork", [](std::shared_ptr<Brain> brain) -> std::string {
        if (!brain) {
            throw BrainInitializationError("Brain cannot be null");
        }
        
        std::stringstream ss;
        ss << "=== Network Visualization ===\n";
        ss << "NLM Brain Network Diagram (text-based)\n\n";
        ss << "Brain Structure:\n";
        ss << "├── Regions (" << brain->getRegionCount() << ")\n";
        
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            auto* region = brain->getRegion(RegionId(i + 1));
            if (region) {
                ss << "│   ├── Region " << region->getId().index() << " (" << region->getName() << ")\n";
                ss << "│   │   ├── Populations: " << region->getPopulationCount() << "\n";
                ss << "│   │   ├── Neurons: " << region->getTotalNeuronCount() << "\n";
                ss << "│   │   └── Synapses: " << region->getSynapseCount() << "\n";
            }
        }
        
        ss << "└── Network Properties:\n";
        ss << "    ├── Connection density: " << 
            (brain->getTotalSynapseCount() / (brain->getTotalNeuronCount() * 0.01f)) << "%\n";
        ss << "    ├── Average synaptic weight: N/A\n";
        ss << "    └── Reciprocal connections: N/A\n";
        
        ss << "\nActivity Profile:\n";
        ss << "├── Firing Statistics:\n";
        ss << "│   ├── Total spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "│   ├── Current firing: " << brain->getFiringNeuronCount() << "\n";
        ss << "│   └── Average rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "└── Network State:\n";
        ss << "    ├── Development: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        ss << "    └── Plasticity: Enabled\n";
        
        ss << "\nNote: This is a simplified text-based visualization.\n";
        ss << "For detailed network graphs, use external visualization tools.\n";
        
        return ss.str();
    }, py::arg("brain"), R"pbdoc(Generate simple network visualization)pbdoc")

} // namespace nlm
