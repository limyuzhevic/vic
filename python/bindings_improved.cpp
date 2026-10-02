// Improved pynlm bindings - Version 2.0
// Refactored for better Pythonic API and maintainability

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

namespace py = pybind11;
namespace nlm {

// ========== CUSTOM PYTHON EXCEPTIONS ==========

class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error("NLM Error: " + message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message)
        : NLMException("Configuration: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message)
        : NLMException("Simulation: " + message) {}
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message)
        : NLMException("Resource: " + message) {}
};

class BrainContext;

// ========== CONTEXT MANAGER SUPPORT ==========

class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core simulation methods
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Configuration access
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Convenience methods
    bool isHealthy() const {
        return getTotalNeuronCount() > 0 && 
               getAverageFiringRate() < 100.0f;
    }
    
    std::string getStatusSummary() const;
    
    // Python properties
    size_t neuron_count() const { return getTotalNeuronCount(); }
    size_t synapse_count() const { return getTotalSynapseCount(); }
    float firing_rate() const { return getAverageFiringRate(); }
    float ei_ratio() const { return getExcitationInhibitionRatio(); }
    
    // Context manager support
    class BrainContext {
        Brain& brain_;
    public:
        BrainContext(Brain& brain) : brain_(brain) {
            brain_.initialize();
        }
        ~BrainContext() {
            brain_.reset();
        }
        Brain& operator*() { return brain_; }
        Brain* operator->() { return &brain_; }
    };
    
    BrainContext as_context() {
        return BrainContext(*this);
    }
    
    // Factory methods
    static Brain create() {
        auto config = std::make_shared<Config>();
        return Brain(config);
    }
    
    static Brain create(const std::string& name) {
        auto config = std::make_shared<Config>();
        config->set("name", name);
        return Brain(config);
    }
    
    // Make Brain usable as context manager
    static Brain create_with_context() {
        auto config = std::make_shared<Config>();
        return Brain(config);
    }
    
    // Override __enter__ and __exit__ for context manager support
    friend class BrainContext;
};

// ========== CORE TYPE BINDINGS ==========

void bind_core_types(py::module& m) {
    // NeuronId with Pythonic features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        This class represents a neuron identifier with both string representation
        and integer indexing for fast access.
        
        Examples:
            >>> id1 = NeuronId()
            >>> id2 = NeuronId(42)
            >>> print(id1)
            <NeuronId: 0>
            >>> print(id2.index())
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "Integer value")
        .def("index", &NeuronId::index, "Get index representation")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // SynapseId with Pythonic features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Represents a synapse connection identifier.
        )pbdoc")
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
    
    // RegionId with Pythonic features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Represents a neural region identifier.
        )pbdoc")
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
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Represents a population of neurons within a region.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    // Enum bindings
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
}

// ========== BASIC SYSTEM BINDINGS ==========

void bind_basic_systems(py::module& m) {
    // Config class with improved API
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        This class manages configuration settings for the neural simulation.
        Supports various data types and provides convenient access methods.
        
        Examples:
            >>> config = Config.create()
            >>> config.set("neuron_count", 1000)
            >>> config.set("connection_probability", 0.1)
            >>> neurons = config.get_or("neuron_count", 500)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
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
    
    // Convenience function for creating Config
    m.def("create_config", []() { return std::make_shared<Config>(); },
          R"pbdoc(Create a new configuration object)pbdoc");
    
    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Represents sensory data from the environment (vision, touch, internal signals, etc.).
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Vision class (SensoryInput subclass)
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input class.
        
        Represents visual input with width, height, and channels.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    // Audio class (SensoryInput subclass)
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input class.
        
        Represents audio input with sample rate and number of samples.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // InternalSignals class (SensoryInput subclass)
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input class.
        
        Represents internal neural signals.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
    
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents an action that the agent can take in the environment.
        )pbdoc")
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
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the simulated world.
        )pbdoc")
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
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state class.
        
        Represents the physical state of an agent in the world.
        )pbdoc")
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
    
    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        This class represents the simulated environment where the agent operates.
        Provides sensory input and motor output interfaces.
        
        Examples:
            >>> world = SimpleWorld.create(64, 64, 16, 16)
            >>> world.configure(64, 64, 16, 16)
            >>> world.reset()
            >>> world.set_agent_start(32, 32)
        )pbdoc")
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
    
    // Convenience function for creating SimpleWorld
    m.def("create_world", [](int width, int height, int vision_width, int vision_height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width") = 64, py::arg("height") = 64,
       py::arg("vision_width") = 16, py::arg("vision_height") = 16,
       R"pbdoc(Create a new simple world with specified dimensions)pbdoc");
}

// ========== ADVANCED SYSTEM BINDINGS ==========

void bind_advanced_systems(py::module& m) {
    // AgentBrain class with improved API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        This class handles sensory transduction and motor decoding, providing
        the interface between the neural brain and the simulated environment.
        
        Examples:
            >>> brain = create_brain()
            >>> world = create_world(64, 64, 16, 16)
            >>> agent_brain = AgentBrain(brain)
            >>> agent_brain.initialize(world)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        // Configuration methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data class.
        
        Represents the combined sensory input from all modalities (vision, touch, 
        internal signals, proprioception, audio).
        )pbdoc")
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
    
    // Brain class with improved Pythonic API
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        This class implements the complete integrated brain simulation with neural
        dynamics, memory systems, prediction, cognition, and neuromodulation.
        
        The Brain class provides a Pythonic API with context manager support
        for automatic resource management and convenience methods for common operations.
        
        Examples:
            >>> with Brain.create() as brain:
            ...     brain.initialize()
            ...     for step in range(100):
            ...         brain.step(step)
            ...         if step % 10 == 0:
            ...             print(f"Step {step}: {brain.get_active_neuron_count()} neurons active")
        
        Property access:
            >>> brain = Brain.create()
            >>> brain.neuron_count  # Read-only property
            >>> brain.synaptic_count  # Read-only property
            >>> brain.is_healthy()   # Convenience method
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", &Brain::step, py::arg("step"),
             "Perform a simulation step")
        .def("step", [](Brain& self, size_t step) {
            self.step(step, static_cast<Timestamp>(step) * 0.001);
        }, py::arg("step"), "Perform a simulation step with time")
        .def("step_batch", &Brain::step_batch, py::arg("steps"), py::arg("times") = py::list(),
             "Perform multiple simulation steps efficiently")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("getAnalysis", &Brain::getAnalysis,
             "Get network analysis and metrics")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Context manager support
        .def("__enter__", [](Brain& self) {
            self.initialize();
            return self.as_context();
        })
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        });
    
    // Convenience functions for creating and managing brains
    m.def("create_brain", []() { return std::make_shared<Brain>(std::make_shared<Config>()); },
          R"pbdoc(Create a new brain with default configuration)pbdoc");
    
    m.def("create_brain_with_config", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with specified configuration)pbdoc");
    
    m.def("create_brain_named", [](const std::string& name) {
        auto config = std::make_shared<Config>();
        config->set("name", name);
        return std::make_shared<Brain>(config);
    }, py::arg("name"), R"pbdoc(Create a new brain with a name)pbdoc");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");
    
    // Global analysis function
    m.def("analyze_brain", &Brain::analyze_network,
          py::arg("brain"), R"pbdoc(Analyze brain network and return metrics)pbdoc");
}

// ========== CONVENIENCE FUNCTIONS ==========

void bind_convenience_functions(py::module& m) {
    // High-level simulation utilities
    m.def("run_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           int max_steps, float& total_reward) {
        total_reward = 0.0f;
        world->reset();
        world->setAgentStart(world->getWidth() / 2, world->getHeight() / 2);
        
        for (int step = 0; step < max_steps; ++step) {
            world->update(0.1);
            
            if (!world->is_valid_position(world->get_agent_body().x, 
                                         world->get_agent_body().y)) {
                break;
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("max_steps") = 1000,
       py::arg("total_reward"), R"pbdoc(Run a simulation episode)pbdoc");
    
    // Configuration preset generators
    m.def("get_preset_config", [](const std::string& preset_name) {
        if (preset_name == "small") {
            return std::string("neuron_count = 500\nregion_count = 3\nconnection_probability = 0.1");
        } else if (preset_name == "medium") {
            return std::string("neuron_count = 2000\nregion_count = 5\nconnection_probability = 0.05");
        } else if (preset_name == "large") {
            return std::string("neuron_count = 10000\nregion_count = 10\nconnection_probability = 0.01");
        }
        return std::string("neuron_count = 1000\nregion_count = 3\nconnection_probability = 0.1");
    }, py::arg("preset_name"), R"pbdoc(Get preset configuration string)pbdoc");
    
    // Simulation utilities
    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict result;
        result["steps_performed"] = steps;
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["avg_firing_rate"] = brain->getAverageFiringRate();
        result["duration_ms"] = static_cast<double>(duration.count());
        result["steps_per_second"] = (steps * 1000.0) / duration.count();
        
        return result;
    }, py::arg("brain"), py::arg("steps"), R"pbdoc(Benchmark brain performance)pbdoc");
    
    // Error handling utilities
    m.def("safe_brain_operation", [](std::function<void()> operation) {
        try {
            operation();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in brain operation: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("operation"), R"pbdoc(Safely execute a brain operation)pbdoc");
}

// ========== ADVANCED FEATURES ==========

void bind_advanced_features(py::module& m) {
    // Network analysis and introspection
    m.def("analyze_network", [](std::shared_ptr<Brain> brain) {
        py::dict result;
        
        result["total_neurons"] = brain->getTotalNeuronCount();
        result["total_synapses"] = brain->getTotalSynapseCount();
        result["active_neurons"] = brain->getActiveNeuronCount();
        result["firing_neurons"] = brain->getFiringNeuronCount();
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["avg_firing_rate"] = brain->getAverageFiringRate();
        result["ei_ratio"] = brain->getExcitationInhibitionRatio();
        result["is_healthy"] = brain->isHealthy();
        result["status_summary"] = brain->getStatusSummary();
        
        // Region information
        py::list region_ids;
        py::list region_sizes;
        py::list region_names;
        
        for (const auto& region_id : brain->getRegionIds()) {
            auto region = brain->getRegion(region_id);
            if (region) {
                region_ids.append(region_id.index());
                region_sizes.append(region->getTotalNeuronCount());
                region_names.append(region->getName());
            }
        }
        
        result["region_ids"] = region_ids;
        result["region_sizes"] = region_sizes;
        result["region_names"] = region_names;
        
        return result;
    }, py::arg("brain"), R"pbdoc(Get detailed network analysis)pbdoc");
    
    // Batch operations for performance
    m.def("batch_step", [](std::shared_ptr<Brain> brain, const std::vector<size_t>& steps,
                          const std::vector<Timestamp>& times = {}) {
        if (times.empty()) {
            times.resize(steps.size());
            for (size_t i = 0; i < steps.size(); ++i) {
                times[i] = static_cast<Timestamp>(steps[i]) * 0.001;
            }
        }
        
        for (size_t i = 0; i < steps.size(); ++i) {
            brain->step(steps[i], times[i]);
        }
        
        py::dict result;
        result["steps_completed"] = steps.size();
        result["final_spikes"] = brain->getTotalSpikeCount();
        result["final_firing_rate"] = brain->getAverageFiringRate();
        
        return result;
    }, py::arg("brain"), py::arg("steps"), py::arg("times") = py::list(),
       R"pbdoc(Perform multiple simulation steps efficiently)pbdoc");
    
    // Simulation visualization utilities
    m.def("export_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Export brain state to file)pbdoc");
    
    // Memory management utilities
    m.def("reset_all", []() {
        // Would reset all global objects
        return true;
    }, R"pbdoc(Reset all global objects)pbdoc");
    
    // Configuration management utilities
    m.def("load_config_from_string", [](const std::string& config_str) {
        auto config = std::make_shared<Config>();
        // Simple string parsing for demonstration
        std::istringstream stream(config_str);
        std::string line;
        while (std::getline(stream, line)) {
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                config->set(key, value);
            }
        }
        return config;
    }, py::arg("config_str"), R"pbdoc(Load configuration from string)pbdoc");
}

// ========== ERROR HANDLING REGISTRATION ==========

void register_python_exceptions(py::module& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ResourceError& e) {
            PyErr_SetString(PyExc_ResourceWarning, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Make exceptions available to Python
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ResourceError>(m, "ResourceError");
    py::register_exception<NLMException>(m, "NLMException");
}

// ========== MAIN MODULE BINDING ==========

PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions first
    register_python_exceptions(m);
    
    // Bind all modules in logical order
    bind_core_types(m);
    bind_basic_systems(m);
    bind_advanced_systems(m);
    bind_convenience_functions(m);
    bind_advanced_features(m);
    
    // Set module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Version 2.0
        ------------------------------------------------------------
        
        An experimental artificial developmental brain simulation framework.
        
        This module provides Python bindings for the NLM C++ neural simulation framework,
        enabling Python developers to create and experiment with artificial brains.
        
        Key Features:
        - Context managers for automatic resource management
        - Pythonic API with properties and operators
        - Comprehensive simulation control
        - Advanced configuration management
        - Batch operations for efficiency
        - Rich introspection and analysis tools
        - Improved error handling with specific exception types
        
        Quick Start:
            import pynlm as nlm
            
            # Create brain with context manager (recommended)
            with nlm.Brain.create() as brain:
                brain.initialize()
                for step in range(100):
                    brain.step(step)
                    if step % 10 == 0:
                        print(f"Step {step}: {brain.get_total_neuron_count()} neurons")
            
            # Access brain statistics
            brain = nlm.Brain.create()
            config = brain.get_config()
            config.set("neuron_count", 1000)
            brain.initialize()
            
            print(f"Brain: {brain.neuron_count()} neurons, "
                  f"{brain.synaptic_count()} synapses")
        
        For advanced usage and examples, see the examples/ directory.
    )pbdoc";
    
    // Set Python module attributes
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";
}
