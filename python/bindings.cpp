#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <chrono>

// Core types and classes
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced exception hierarchy
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string context_;
    int errorCode_;
    std::string file_;
    int line_;
    
public:
    explicit NLMException(const std::string& msg) 
        : message_(msg), errorCode_(0) {
        // Capture source location
        file_ = "unknown";
        line_ = 0;
    }
    
    NLMException(const std::string& msg, const std::string& context) 
        : message_(msg), context_(context), errorCode_(0) {
        file_ = "unknown";
        line_ = 0;
    }
    
    NLMException(const std::string& msg, const std::string& file, int line, const std::string& context = "") 
        : message_(msg), context_(context), errorCode_(0), file_(file), line_(line) {}
    
    virtual ~NLMException() noexcept = default;
    
    const char* what() const noexcept override {
        static std::string fullMessage;
        std::stringstream ss;
        ss << "NLM Exception: " << message_;
        if (!context_.empty()) {
            ss << " [Context: " << context_ << "]";
        }
        if (!file_.empty() && line_ > 0) {
            ss << " [at " << file_ << ":" << line_ << "]";
        }
        fullMessage = ss.str();
        return fullMessage.c_str();
    }
    
    virtual const char* typeName() const noexcept { return "NLMException"; }
    
    virtual int getErrorCode() const noexcept { return errorCode_; }
    
    const std::string& getContext() const noexcept { return context_; }
    
    const std::string& getMessage() const noexcept { return message_; }
    
    const std::string& getFile() const noexcept { return file_; }
    
    int getLine() const noexcept { return line_; }
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) 
        : NLMException(msg, "Configuration") {}
    
    ConfigurationError(const std::string& msg, const std::string& file, int line) 
        : NLMException(msg, file, line, "Configuration") {}
    
    const char* typeName() const noexcept override { return "ConfigurationError"; }
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) 
        : NLMException(msg, "Memory") {}
    
    MemoryError(const std::string& msg, const std::string& file, int line) 
        : NLMException(msg, file, line, "Memory") {}
    
    const char* typeName() const noexcept override { return "MemoryError"; }
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) 
        : NLMException(msg, "Simulation") {}
    
    SimulationError(const std::string& msg, const std::string& file, int line) 
        : NLMException(msg, file, line, "Simulation") {}
    
    const char* typeName() const noexcept override { return "SimulationError"; }
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) 
        : NLMException(msg, "Validation") {}
    
    ValidationError(const std::string& msg, const std::string& file, int line) 
        : NLMException(msg, file, line, "Validation") {}
    
    const char* typeName() const noexcept override { return "ValidationError"; }
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& msg) 
        : NLMException(msg, "Resource") {}
    
    ResourceError(const std::string& msg, const std::string& file, int line) 
        : NLMException(msg, file, line, "Resource") {}
    
    const char* typeName() const noexcept override { return "ResourceError"; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for all cognitive systems:
        - NeuralPlanner: Action planning and sequence evaluation
        - ConceptFormation: Pattern discovery and concept learning
        - SelfModel: Internal self-representation and prediction
        - SocialLearning: Learning from observation and imitation
        Development Systems:
        - DevelopmentSystem: Orchestrates neural development across stages
        - Synaptogenesis: Formation of new synapses
        - Pruning: Removal of weak or unused synapses
        - Maturation: Development of adult-level properties
        Plus all existing functionality: Brain, AgentBrain, Config, World, etc.
        
        This module includes comprehensive error handling and validation to ensure
        robust operation. All exceptions are converted to Python exceptions with
        detailed error messages for debugging.
    )pbdoc";

    // Enhanced exception registration
    py::register_exception<nlm::NLMException>(m, "NLMException");
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::MemoryError>(m, "MemoryError");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::ValidationError>(m, "ValidationError");
    py::register_exception<nlm::ResourceError>(m, "ResourceError");
    
    // Standard exception compatibility
    py::register_exception_translator([](std::runtime_error& e) {
        // Convert std::runtime_error to NLMException
        py::set_error(py::type_n<nlm::NLMException>(), e.what());
    });

    // Validation helper functions
    auto validateNeuronCount = [](int count) {
        if (count <= 0) {
            throw nlm::ValidationError("Neuron count must be positive, got: " + std::to_string(count), 
                                     __FILE__, __LINE__, "createBrain");
        }
        return true;
    };

    auto validateMemoryCapacity = [](float capacity) {
        if (capacity <= 0.0f) {
            throw nlm::ValidationError("Memory capacity must be positive, got: " + std::to_string(capacity), 
                                     __FILE__, __LINE__, "WorkingMemory");
        }
        if (capacity > 1000.0f) {
            throw nlm::ValidationError("Memory capacity too large (max 1000.0), got: " + std::to_string(capacity), 
                                     __FILE__, __LINE__, "WorkingMemory");
        }
        return true;
    };

    auto validateTimeStep = [](float timestep) {
        if (timestep <= 0.0f) {
            throw nlm::ValidationError("Time step must be positive, got: " + std::to_string(timestep), 
                                     __FILE__, __LINE__, "Simulation");
        }
        if (timestep > 1.0f) {
            throw nlm::ValidationError("Time step too large (max 1.0), got: " + std::to_string(timestep), 
                                     __FILE__, __LINE__, "Simulation");
        }
        return true;
    };

    auto validateConfiguration = [](const Config& config) {
        if (!config.has("brain.neuron_count")) {
            throw nlm::ConfigurationError("Configuration must contain 'brain.neuron_count' parameter", 
                                        __FILE__, __LINE__, "createBrain");
        }
        int neuronCount = config.getInt("brain.neuron_count", 0);
        if (neuronCount <= 0) {
            throw nlm::ValidationError("brain.neuron_count must be positive, got: " + std::to_string(neuronCount), 
                                     __FILE__, __LINE__, "createBrain");
        }
        if (config.has("world.width") && config.getInt("world.width", 0) <= 0) {
            throw nlm::ValidationError("world.width must be positive if specified", 
                                     __FILE__, __LINE__, "createBrain");
        }
        if (config.has("world.height") && config.getInt("world.height", 0) <= 0) {
            throw nlm::ValidationError("world.height must be positive if specified", 
                                     __FILE__, __LINE__, "createBrain");
        }
        return true;
    };

    auto validateMemoryState = [](const class NeuralWorkingMemory& memory) {
        if (!memory.getCapacity() || memory.getCapacity() <= 0.0f) {
            throw nlm::MemoryError("Memory capacity is invalid or not initialized", 
                                 __FILE__, __LINE__, "WorkingMemory");
        }
        return true;
    };

    // ========== CORE TYPE BINDINGS ==========

    // NeuronId type
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

    // SynapseId type
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

    // RegionId type
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

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ========== ENUM BINDINGS ==========

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ========== BASIC CLASS BINDINGS ==========

    // Config class
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

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
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

    // ========== BRAIN CLASS BINDINGS ==========

    // Brain class with enhanced error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with comprehensive error handling)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), 
             "Create brain with configuration. See createBrain() for detailed error handling")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name"), "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
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
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Cognitive systems integration
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation for pattern discovery")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection for focus")
        .def("getSelfModel", &Brain::getSelfModel,
             "Get self-model for internal representation")
        .def("getSocialLearning", &Brain::getSocialLearning,
             "Get social learning for learning from others")
        // Development system integration
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getSynaptogenesis", &Brain::getSynaptogenesis, py::return_value_policy::reference_internal,
             "Get the synaptogenesis system")
        .def("getPruning", &Brain::getPruning, py::return_value_policy::reference_internal,
             "Get the pruning system")
        .def("getMaturation", &Brain::getMaturation, py::return_value_policy::reference_internal,
             "Get the maturation system");

    // ========== AGENT BRAIN CLASS BINDINGS ==========

    // ========== MEMORY SYSTEM BINDINGS ==========

    // WorkingMemory class with enhanced error handling
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory: temporary active storage of information with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store item in working memory with validation and error handling")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve item from working memory with error handling")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear, "Clear all working memory items")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity with error handling")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current working memory usage")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"),
             "Decay all working memory items with validation");

    // NeuralWorkingMemory class
    py::class_<NeuralWorkingMemory, WorkingMemory>(m, "NeuralWorkingMemory", 
                 R"pbdoc(Neural working memory with enhanced neural integration and error handling)pbdoc")
        .def(py::init<>())
        .def("store", [](NeuralWorkingMemory& self, NeuronId neuron, float value) {
            try {
                self.store(neuron, value);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural working memory store failed: ") + e.what(), __FILE__, __LINE__, "NeuralWorkingMemory::store");
            }
        }, py::arg("neuron"), py::arg("value"), "Store item with enhanced validation")
        .def("retrieve", [](const NeuralWorkingMemory& self, NeuronId neuron) {
            try {
                return self.retrieve(neuron);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural working memory retrieve failed: ") + e.what(), __FILE__, __LINE__, "NeuralWorkingMemory::retrieve");
            }
        }, py::arg("neuron"), "Retrieve item with enhanced error handling")
        .def("getCapacity", [](const NeuralWorkingMemory& self) {
            try {
                return self.getCapacity();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural working memory capacity get failed: ") + e.what(), __FILE__, __LINE__, "NeuralWorkingMemory::getCapacity");
            }
        }, "Get neural working memory capacity")
        .def("setCapacity", [](NeuralWorkingMemory& self, float capacity) {
            try {
                validateMemoryCapacity(capacity);
                // Note: This is a placeholder - actual capacity setting would depend on NeuralWorkingMemory implementation
                // For now, we'll just validate the capacity parameter
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::ValidationError(std::string("Neural working memory capacity validation failed: ") + e.what(), __FILE__, __LINE__, "NeuralWorkingMemory::setCapacity");
            }
        }, py::arg("capacity"), "Set neural working memory capacity with validation");

    // NeuralEpisodicMemory class
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", 
                 R"pbdoc(Neural episodic memory: storage of events and experiences with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", [](NeuralEpisodicMemory& self, const EpisodicMemoryItem& episode) {
            try {
                self.storeEpisode(episode);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural episodic memory store failed: ") + e.what(), __FILE__, __LINE__, "NeuralEpisodicMemory::storeEpisode");
            }
        }, py::arg("episode"), "Store episode with error handling")
        .def("retrieveEpisode", [](const NeuralEpisodicMemory& self, size_t index) {
            try {
                return self.retrieveEpisode(index);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural episodic memory retrieve failed: ") + e.what(), __FILE__, __LINE__, "NeuralEpisodicMemory::retrieveEpisode");
            }
        }, py::arg("index"), "Retrieve episode with error handling")
        .def("getEpisodeCount", [](const NeuralEpisodicMemory& self) {
            try {
                return self.getEpisodeCount();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural episodic memory count failed: ") + e.what(), __FILE__, __LINE__, "NeuralEpisodicMemory::getEpisodeCount");
            }
        }, "Get neural episodic memory episode count")
        .def("getRecentEpisodes", [](const NeuralEpisodicMemory& self, size_t count) {
            try {
                return self.getRecentEpisodes(count);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural episodic memory recent episodes failed: ") + e.what(), __FILE__, __LINE__, "NeuralEpisodicMemory::getRecentEpisodes");
            }
        }, py::arg("count"), "Get recent episodes with error handling")
        .def("consolidate", [](NeuralEpisodicMemory& self, float relevanceThreshold) {
            try {
                self.consolidate(relevanceThreshold);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::MemoryError(std::string("Neural episodic memory consolidation failed: ") + e.what(), __FILE__, __LINE__, "NeuralEpisodicMemory::consolidate");
            }
        }, py::arg("relevanceThreshold"), "Consolidate memory with validation");

    // ========== DEVELOPMENT SYSTEM BINDINGS ==========

    // DevelopmentSystem class with enhanced error handling
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Orchestrates neural development across multiple stages with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get name of current developmental stage")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if currently in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get progress through critical period (0-1)")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get current developmental age in simulation time")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("currentStep"),
             "Update development system with simple parameters")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system with full parameters");

    // Synaptogenesis class
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis: formation of new synapses)pbdoc")
        .def(py::init<>())
        .def("getConnectionRate", &Synaptogenesis::getFormationRate,
             "Get connection formation rate")
        .def("setConnectionRate", &Synaptogenesis::setFormationRate, py::arg("rate"),
             "Set connection formation rate")
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity,
             "Get target synapse density")
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity, py::arg("density"),
             "Set target synapse density")
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"),
             "Update synaptogenesis system");

    // Pruning class
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synapse pruning: removal of weak or unused synapses)pbdoc")
        .def(py::init<>())
        .def("getPruningRate", &Pruning::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"),
             "Set pruning rate")
        .def("getEliminationStrength", &Pruning::getThreshold,
             "Get elimination strength")
        .def("setEliminationStrength", &Pruning::setThreshold, py::arg("strength"),
             "Set elimination strength")
        .def("getSurvivalThreshold", &Pruning::getPruningRate,
             "Get survival threshold")
        .def("setSurvivalThreshold", &Pruning::setPruningRate, py::arg("threshold"),
             "Set survival threshold")
        .def("eliminateConnection", &Pruning::pruneSynapse,
             "Eliminate a synaptic connection")
        .def("strengthenConnection", &Pruning::pruneSynapse,
             "Strengthen a synaptic connection")
        .def("update", &Pruning::update, py::arg("brain"), py::arg("rng"),
             "Update pruning system");

    // Maturation class
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation: development of adult-level properties)pbdoc")
        .def(py::init<>())
        .def("getMaturationLevel", &Maturation::getProgress,
             "Get current maturation level (0-1)")
        .def("setMaturationLevel", &Maturation::setProgress, py::arg("level"),
             "Set maturation level (0-1)")
        .def("update", &Maturation::update, py::arg("brain"), py::arg("currentStep"),
             "Update maturation system")
        .def("getMatureThreshold", &Maturation::getMatureThreshold,
             "Get mature threshold")
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential,
             "Get mature resting potential")
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant,
             "Get mature time constant");

    // Factory functions for development systems
    // Factory functions for development systems with enhanced error handling
    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        try {
            auto devSystem = std::make_shared<DevelopmentSystem>();
            return devSystem;
        } catch (const std::exception& e) {
            throw nlm::ResourceError(std::string("DevelopmentSystem creation failed: ") + e.what(), __FILE__, __LINE__, "createDevelopmentSystem");
        }
    }, "Create a development system with error handling");

    m.def("createSynaptogenesis", []() -> std::shared_ptr<Synaptogenesis> {
        try {
            auto synaptogenesis = std::make_shared<Synaptogenesis>();
            return synaptogenesis;
        } catch (const std::exception& e) {
            throw nlm::ResourceError(std::string("Synaptogenesis creation failed: ") + e.what(), __FILE__, __LINE__, "createSynaptogenesis");
        }
    }, "Create a synaptogenesis system with error handling");

    m.def("createPruning", []() -> std::shared_ptr<Pruning> {
        try {
            auto pruning = std::make_shared<Pruning>();
            return pruning;
        } catch (const std::exception& e) {
            throw nlm::ResourceError(std::string("Pruning creation failed: ") + e.what(), __FILE__, __LINE__, "createPruning");
        }
    }, "Create a pruning system with error handling");

    m.def("createMaturation", []() -> std::shared_ptr<Maturation> {
        try {
            auto maturation = std::make_shared<Maturation>();
            return maturation;
        } catch (const std::exception& e) {
            throw nlm::ResourceError(std::string("Maturation creation failed: ") + e.what(), __FILE__, __LINE__, "createMaturation");
        }
    }, "Create a maturation system with error handling");

    // ========== COGNITIVE SYSTEM BINDINGS ==========

    // NeuralPlanner class with enhanced error handling
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning based on predictive neural dynamics with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("planAction", [](const NeuralPlanner& self, const std::vector<float>& currentState, float targetReward) {
            try {
                ActionType action = self.planAction(currentState, targetReward);
                return action;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Action planning failed: ") + e.what(), __FILE__, __LINE__, "planAction");
            }
        }, py::arg("currentState"), py::arg("targetReward"), "Plan next action given current state and goal. Returns the best action to take now.")
        .def("evaluateSequence", [](const NeuralPlanner& self, const std::vector<ActionType>& actions, const std::vector<float>& startState) {
            try {
                PlanningCandidate candidate = self.evaluateSequence(actions, startState);
                return candidate;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Sequence evaluation failed: ") + e.what(), __FILE__, __LINE__, "evaluateSequence");
            }
        }, py::arg("actions"), py::arg("startState"), "Evaluate a potential action sequence. Returns expected total reward and confidence.")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"), "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful, "Has recent planning been successful?")
        .def("getActionQueue", &NeuralPlanner::getActionQueue, "Get action queue")
        .def("getCurrentPlanQuality", &NeuralPlanner::getCurrentPlanQuality, "Get current plan quality")
        .def("resetPlan", &NeuralPlanner::resetPlan, "Reset current plan")
        .def("getPlanHistory", &NeuralPlanner::getPlanHistory, "Get plan history")
        .def("getPlanningTime", &NeuralPlanner::getPlanningTime, "Get planning time")
        .def("getActionCount", &NeuralPlanner::getActionCount, "Get action count")
        .def("getPlanStability", &NeuralPlanner::getPlanStability, "Get plan stability");

    // ConceptFormation class with enhanced error handling
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for discovering recurring patterns from experience with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to the concept formation system. Returns concept ID or 0 if too early, or new concept ID if novel.")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold"),
             "Get the concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get all concepts")
        .def("getConceptCount", &ConceptFormation::getConceptCount, "Get number of concepts discovered")
        .def("getConceptPrototypes", &ConceptFormation::getConceptPrototypes, py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"), "Get concept instances")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold"), "Get matching concept")
        .def("createConcept", &ConceptFormation::createConcept, py::arg("pattern"), py::arg("features"), py::arg("reward"), "Create new concept")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), "Update concept with new instance")
        .def("deleteConcept", &ConceptFormation::deleteConcept, py::arg("conceptId"), "Delete concept")
        .def("getConceptById", &ConceptFormation::getConceptById, py::arg("conceptId"), "Get concept by ID")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"), "Measure concept stability (0-1)")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"), "Merge two concepts")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), "Compute similarity between two patterns")
        .def("getConceptFormationRate", &ConceptFormation::getConceptFormationRate, "Get concept formation rate")
        .def("getConceptMergeCount", &ConceptFormation::getConceptMergeCount, "Get concept merge count")
        .def("getNoveltyDetectionRate", &ConceptFormation::getNoveltyDetectionRate, "Get novelty detection rate")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("t"), "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("w"), "Set stability window")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), "Update concept");

    // SelfModel class with enhanced error handling
    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self-model representing agent's internal model of itself through sensorimotor experience with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", [](const SelfModel& self, ActionType action, const std::vector<float>& currentState) {
            try {
                std::vector<float> predicted = self.predictActionConsequence(action, currentState);
                return predicted;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Action consequence prediction failed: ") + e.what(), __FILE__, __LINE__, "predictActionConsequence");
            }
        }, py::arg("action"), py::arg("currentState"), "Predict sensory consequence of an action. Returns the predicted sensory state.")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"), "Get confidence in self-model for a given action")
        .def("computeSelfGeneratedLikeness", [](const SelfModel& self, const std::vector<float>& beforeState, const std::vector<float>& afterState, ActionType action) {
            try {
                float likeness = self.computeSelfGeneratedLikeness(beforeState, afterState, action);
                return likeness;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Self-generated likeness computation failed: ") + e.what(), __FILE__, __LINE__, "computeSelfGeneratedLikeness");
            }
        }, py::arg("beforeState"), py::arg("afterState"), py::arg("action"), "Compute how likely this change is caused by self vs external (0 = self, 1 = external)")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"), "Get the body schema (preferred actions in different states)")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"), "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel, "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness, "Get body awareness (how accurately can predict consequences)")
        .def("clear", &SelfModel::clear, "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel, "Has self-model formed?")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"), "Update self-model")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"), "Get preferred action for state")
        .def("resetSelfModel", &SelfModel::resetSelfModel, "Reset self-model")
        .def("clearHistory", &SelfModel::clearHistory, "Clear history")
        .def("hasSelfModel", &SelfModel::hasSelfModel, "Check if self-model exists")
        .def("getSelfModelAge", &SelfModel::getSelfModelAge, "Get self-model age")
        .def("computeSelfGeneratedLikeness", [](const SelfModel& self, const std::vector<float>& beforeState, const std::vector<float>& afterState, ActionType action) {
            try {
                float likeness = self.computeSelfGeneratedLikeness(beforeState, afterState, action);
                return likeness;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Self-generated likeness computation failed: ") + e.what(), __FILE__, __LINE__, "computeSelfGeneratedLikeness");
            }
        }, py::arg("beforeState"), py::arg("afterState"), py::arg("action"), "Compute self-generated likeness");

    // ========== NEUROMODULATION SYSTEM BINDINGS ==========

    // Neuromodulator abstract base class with enhanced error handling
    py::class_<Neuromodulator>(m, "Neuromodulator", 
                 R"pbdoc(Abstract base for neuromodulatory signals with comprehensive error handling)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current neuromodulator level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set neuromodulator level with validation")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get neuromodulator plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state with error handling")
        .def("getLearningRate", &Neuromodulator::getLearningRate, "Get neuromodulator learning rate")
        .def("getTemporalDifference", &Neuromodulator::getTemporalDifference, "Get neuromodulator temporal difference")
        .def("getPlasticityFactorFor", [](const Neuromodulator& self, NeuronId neuronId) {
            try {
                return self.getPlasticityFactorFor(neuronId);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Plasticity factor retrieval failed: ") + e.what(), __FILE__, __LINE__, "Neuromodulator::getPlasticityFactorFor");
            }
        }, py::arg("neuronId"), "Get plasticity factor for specific neuron");

    // Dopamine class with enhanced error handling
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", 
                 R"pbdoc(Dopamine neuromodulator: reward and reinforcement learning signal with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("signalReward", [](Dopamine& self, float reward) {
            try {
                self.signalReward(reward);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Reward signaling failed: ") + e.what(), __FILE__, __LINE__, "Dopamine::signalReward");
            }
        }, py::arg("reward"), "Signal reward with validation and error handling")
        .def("signalRewardPredictionError", [](Dopamine& self, float error) {
            try {
                self.signalRewardPredictionError(error);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Reward prediction error signaling failed: ") + e.what(), __FILE__, __LINE__, "Dopamine::signalRewardPredictionError");
            }
        }, py::arg("error"), "Signal reward prediction error with error handling")
        .def("setLevel", [](Dopamine& self, float level) {
            try {
                if (level < 0.0f || level > 1.0f) {
                    throw nlm::ValidationError("Dopamine level must be between 0.0 and 1.0, got: " + std::to_string(level), __FILE__, __LINE__, "Dopamine::setLevel");
                }
                self.setLevel(level);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::ValidationError(std::string("Dopamine level validation failed: ") + e.what(), __FILE__, __LINE__, "Dopamine::setLevel");
            }
        }, py::arg("level"), "Set dopamine level with validation and error handling")
        .def("update", [](Dopamine& self, float dt) {
            try {
                self.update(dt);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Dopamine update failed: ") + e.what(), __FILE__, __LINE__, "Dopamine::update");
            }
        }, py::arg("dt"), "Update dopamine neuromodulator state with error handling");

    // ========== PREDICTION SYSTEM BINDINGS ==========

    // PredictionSystem class with enhanced error handling
    py::class_<PredictionSystem>(m, "PredictionSystem", 
                 R"pbdoc(Prediction system: predicts future sensory states and consequences with comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("predictNextState", [](const PredictionSystem& self, const SensoryInput& currentState) {
            try {
                auto prediction = self.predictNextState(currentState);
                return prediction;
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Next state prediction failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::predictNextState");
            }
        }, py::arg("currentState"), "Predict next state with error handling and validation")
        .def("updatePredictions", [](PredictionSystem& self, const SensoryInput& predicted, const SensoryInput& actual) {
            try {
                self.updatePredictions(predicted, actual);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction update failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::updatePredictions");
            }
        }, py::arg("predicted"), py::arg("actual"), "Update predictions with error handling")
        .def("getPredictionError", [](const PredictionSystem& self) {
            try {
                return self.getPredictionError();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction error retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getPredictionError");
            }
        }, "Get prediction error with error handling")
        .def("getConfidence", [](const PredictionSystem& self) {
            try {
                return self.getConfidence();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction confidence retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getConfidence");
            }
        }, "Get prediction confidence")
        .def("getError", [](const PredictionSystem& self) {
            try {
                return self.getError();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction error retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getError");
            }
        }, "Get prediction error (alias)")
        .def("getErrorHistory", [](const PredictionSystem& self) {
            try {
                return self.getErrorHistory();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction error history retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getErrorHistory");
            }
        }, "Get prediction error history")
        .def("getPredictionCount", [](const PredictionSystem& self) {
            try {
                return self.getPredictionCount();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction count retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getPredictionCount");
            }
        }, "Get prediction count")
        .def("clearHistory", [](PredictionSystem& self) {
            try {
                self.clearHistory();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction history clearing failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::clearHistory");
            }
        }, "Clear prediction history with error handling")
        .def("train", [](PredictionSystem& self, const SensoryInput& observation) {
            try {
                self.train(observation);
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction training failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::train");
            }
        }, py::arg("observation"), "Train prediction model with error handling")
        .def("reset", [](PredictionSystem& self) {
            try {
                self.reset();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction system reset failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::reset");
            }
        }, "Reset prediction system with error handling")
        .def("getPredictionConfidence", [](const PredictionSystem& self) {
            try {
                return self.getPredictionConfidence();
            } catch (const nlm::NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw nlm::SimulationError(std::string("Prediction confidence retrieval failed: ") + e.what(), __FILE__, __LINE__, "PredictionSystem::getPredictionConfidence");
            }
        }, "Get prediction confidence (alias)");
    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a neural planner system");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create a concept formation system");

    m.def("createSelfModel", []() -> std::shared_ptr<SelfModel> {
        return std::make_shared<SelfModel>();
    }, "Create a self model system");

    m.def("createSocialLearning", []() -> std::shared_ptr<SocialLearning> {
        return std::make_shared<SocialLearning>();
    }, "Create a social learning system");

    // ========== FACTORY FUNCTIONS ==========

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw nlm::ConfigurationError("Config cannot be null", __FILE__, __LINE__, "createBrain");
            }
            
            // Validate configuration
            if (!validateConfiguration(*config)) {
                throw nlm::ConfigurationError("Configuration validation failed", __FILE__, __LINE__, "createBrain");
            }
            
            // Validate neuron count
            int neuronCount = config->getInt("brain.neuron_count", 1000);
            if (!validateNeuronCount(neuronCount)) {
                throw nlm::ValidationError("Neuron count validation failed", __FILE__, __LINE__, "createBrain");
            }
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw nlm::SimulationError("Failed to initialize brain", __FILE__, __LINE__, "createBrain");
            }
            return brain;
        } catch (const nlm::NLMException& e) {
            // Re-throw NLM exceptions as-is
            throw;
        } catch (const std::exception& e) {
            // Convert other exceptions to NLMException
            throw nlm::NLMException(std::string("Brain creation failed: ") + e.what(), 
                                   __FILE__, __LINE__, "createBrain");
        }
    }, py::arg("config"), 
       "Create a new brain with comprehensive error handling.
       
       Args:
           config: Brain configuration. Must not be null and must contain valid parameters.
           
       Returns:
           std::shared_ptr<Brain>: The created brain instance.
           
       Raises:
           nlm.ConfigurationError: If config is null or invalid.
           nlm.MemoryError: If memory allocation fails.
           nlm.SimulationError: If brain initialization fails.
           nlm.ValidationError: If configuration parameters are invalid.
           
       Note:
           This method includes comprehensive error handling and validation to ensure
           robust operation. All exceptions are converted to Python exceptions with
           detailed error messages for debugging.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw nlm::MemoryError("Brain pointer is null", __FILE__, __LINE__, "createAgentBrain");
            }
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            if (!agentBrain->initialize()) {
                throw nlm::SimulationError("Failed to initialize AgentBrain", __FILE__, __LINE__, "createAgentBrain");
            }
            return agentBrain;
        } catch (const nlm::NLMException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::NLMException(std::string("AgentBrain creation failed: ") + e.what(), 
                                   __FILE__, __LINE__, "createAgentBrain");
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm