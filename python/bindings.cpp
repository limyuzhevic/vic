#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

namespace py = pybind11;
namespace nlm {

// Define custom exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory Error: " + msg) {}
};

// Brain Builder class
class BrainBuilder {
public:
    BrainBuilder& neuron_count(size_t n) {
        config_->set("neuron_count", static_cast<int64_t>(n));
        return *this;
    }
    
    BrainBuilder& region_count(size_t n) {
        config_->set("region_count", static_cast<int64_t>(n));
        return *this;
    }
    
    BrainBuilder& connection_probability(float p) {
        config_->set("connection_probability", p);
        return *this;
    }
    
    BrainBuilder& enable_plasticity(bool enable) {
        config_->set("enable_plasticity", enable);
        return *this;
    }
    
    BrainBuilder& enable_reward_modulation(bool enable) {
        config_->set("enable_reward_modulation", enable);
        return *this;
    }
    
    BrainBuilder& enable_development(bool enable) {
        config_->set("enable_development", enable);
        return *this;
    }
    
    std::shared_ptr<Brain> create() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            throw ConfigurationError("Failed to initialize brain with specified parameters");
        }
        return brain;
    }
    
    std::shared_ptr<Brain> createWithInit() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            throw ConfigurationError("Failed to initialize brain with specified parameters");
        }
        return brain;
    }
    
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_ = std::make_shared<Config>();
    friend class BrainBuilderFactory;
};

class BrainBuilderFactory {
public:
    static BrainBuilder create() {
        return BrainBuilder();
    }
};

// Simulation helper functions
inline std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw ConfigurationError("Failed to initialize brain");
    }
    return brain;
}

inline std::shared_ptr<Brain> createBrainQuick(size_t neuron_count, size_t region_count) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuron_count));
    config->set("region_count", static_cast<int64_t>(region_count));
    config->set("connection_probability", 0.1f);
    return createBrainWithConfig(config);
}

inline void runSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps,
    float timestep
) {
    for (size_t i = 0; i < steps; ++i) {
        world->update(timestep);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(i);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
}

// Context manager implementation
class BrainContext {
public:
    explicit BrainContext(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {
        if (!brain_->initialize()) {
            throw ConfigurationError("Failed to initialize brain in context");
        }
    }
    
    BrainContext(std::shared_ptr<Config> config, size_t neuron_count, size_t region_count) {
        config->set("neuron_count", static_cast<int64_t>(neuron_count));
        config->set("region_count", static_cast<int64_t>(region_count));
        brain_ = std::make_shared<Brain>(config);
        if (!brain_->initialize()) {
            throw ConfigurationError("Failed to initialize brain in context with specified parameters");
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    BrainContext(const BrainContext&) = delete;
    BrainContext& operator=(const BrainContext&) = delete;
    
    BrainContext(BrainContext&& other) noexcept : brain_(std::move(other.brain_)) {}
    BrainContext& operator=(BrainContext&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    void __enter__() {
        // Initialize brain on enter if not already initialized
        if (brain_ && !brain_->getConfig()->has("initialized")) {
            brain_->initialize();
        }
    }
    
    void __exit__(py::object, py::object, py::object) {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
    }
    
    std::shared_ptr<Brain> operator->() {
        if (!brain_) {
            throw MemoryError("Brain context has been closed");
        }
        return brain_;
    }
    
    std::shared_ptr<const Brain> operator->() const {
        if (!brain_) {
            throw MemoryError("Brain context has been closed");
        }
        return brain_;
    }
    
    explicit operator bool() const {
        return static_cast<bool>(brain_);
    }
    
    bool is_open() const {
        return static_cast<bool>(brain_);
    }
    
    void close() {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Helper for batch operations
class BatchOperations {
public:
    static std::vector<std::unique_ptr<Action>> produceActions(
        std::shared_ptr<Brain> brain, 
        size_t count
    ) {
        std::vector<std::unique_ptr<Action>> actions;
        actions.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            actions.push_back(brain->produceAction());
        }
        
        return actions;
    }
    
    static std::vector<NeuronId> getSensoryNeurons(
        std::shared_ptr<Brain> brain,
        NeuronType type
    ) {
        // This would need to be implemented in Brain class
        // For now, return empty vector
        return std::vector<NeuronId>{};
    }
    
    static float getBrainStats(
        std::shared_ptr<Brain> brain,
        const std::string& stat_name
    ) {
        if (stat_name == "firing_rate") {
            return brain->getAverageFiringRate();
        } else if (stat_name == "excitation_inhibition_ratio") {
            return brain->getExcitationInhibitionRatio();
        } else if (stat_name == "total_spikes") {
            return static_cast<float>(brain->getTotalSpikeCount());
        } else if (stat_name == "active_neurons") {
            return static_cast<float>(brain->getActiveNeuronCount());
        }
        throw ConfigurationError("Unknown statistic: " + stat_name);
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action.
        
        Key Features:
        - Pythonic API with properties and context managers
        - Builder pattern for brain creation
        - Comprehensive error handling
        - NumPy integration
        - Simulation helpers
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");

    // Basic types
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

    // Config class with enhanced API
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
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<int64_t>(key);
        }, py::arg("key"), "Get an integer value from configuration")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get a double value from configuration")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<bool>(key);
        }, py::arg("key"), "Get a boolean value from configuration")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get a string value from configuration")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            return self.validate();
        }, "Validate configuration parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
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
        .def("setData", [](Vision& self, py::array arr) {
            py::buffer_info buf = arr.request();
            if (buf.ndim != 3) {
                throw std::invalid_argument("Vision data must be 3D array (H,W,C)");
            }
            size_t width = buf.shape[0];
            size_t height = buf.shape[1];
            size_t channels = buf.shape[2];
            
            float* ptr = static_cast<float*>(buf.ptr);
            std::vector<float> data(ptr, ptr + buf.size);
            self.setData(data);
        }, py::arg("data"), "Set vision data from NumPy array")
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
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

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

    // Brain class with enhanced API
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with the given configuration. The brain will "
             "automatically initialize neural networks and prepare for simulation.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with the configured parameters. This sets up "
             "neurons, synapses, and prepares the neural networks for operation.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Advance the brain by one simulation step. Each step processes "
             "neuron activity, synaptic connections, and plasticity rules.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Advance the brain by one simulation step with explicit time.")
        .def("stepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Convenience method for stepping with time.")
        .def("stepMultiple", [](Brain& self, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(static_cast<SimulationStep>(i));
            }
        }, py::arg("steps"), "Run multiple simulation steps sequentially.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. The brain will process "
             "the input and activate appropriate neurons.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor/action output based on neural activity. The brain "
             "analyzes motor neuron states to generate appropriate actions.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing).")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics.")
        // Pythonic properties
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
                              "The total number of neurons in the brain (int)")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
                              "The total number of synapses in the brain (int)")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                              "The number of currently active neurons (int)")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                              "The number of neurons currently firing (int)")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "The average firing rate across all neurons (float)")
        .def_property_readonly("excitaton_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
                              "The excitation/inhibition balance ratio (float)")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
                              "The total number of spikes recorded (int)")
        .def_property_readonly("region_count", &Brain::getRegionCount,
                              "The number of neural regions (int)")
        .def_property_readonly("regions", &Brain::getRegions,
                              "All neural regions (list of Region objects)")
        .def_property_readonly("region_ids", &Brain::getRegionIds,
                              "All region IDs (list of RegionId objects)")
        // Context manager support
        .def("__enter__", [](py::object self) {
            self.attr("initialize")();
            return self;
        })
        .def("__exit__", [](py::object self, py::object, py::object, py::object) {
            self.attr("reset")();
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the given brain.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.");

    // Brain Builder - expose through module
    py::class_<BrainBuilder>(m, "BrainBuilder", R"pbdoc(Builder for creating brains with specific parameters)pbdoc")
        .def(py::init<>())
        .def("neuron_count", &BrainBuilder::neuron_count, py::arg("count"),
             "Set the number of neurons for the brain.")
        .def("region_count", &BrainBuilder::region_count, py::arg("count"),
             "Set the number of neural regions for the brain.")
        .def("connection_probability", &BrainBuilder::connection_probability, py::arg("probability"),
             "Set the connection probability between neurons.")
        .def("enable_plasticity", &BrainBuilder::enable_plasticity, py::arg("enable"),
             "Enable or disable plasticity rules.")
        .def("enable_reward_modulation", &BrainBuilder::enable_reward_modulation, py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enable_development", &BrainBuilder::enable_development, py::arg("enable"),
             "Enable or disable development.")
        .def("create", &BrainBuilder::create,
             "Create and initialize the brain with the specified parameters.")
        .def("get_config", &BrainBuilder::getConfig,
             "Get the configuration object for further customization.");

    // Context manager class
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for brain lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def(py::init<std::shared_ptr<Config>, size_t, size_t>(),
             py::arg("config"), py::arg("neuron_count"), py::arg("region_count"),
             "Create a brain context with specified parameters.")
        .def("__enter__", &BrainContext::__enter__)
        .def("__exit__", &BrainContext::__exit__)
        .def("is_open", &BrainContext::is_open,
             "Check if the context is open.")
        .def("close", &BrainContext::close,
             "Close the context and clean up resources.")
        .def("brain", &BrainContext::getBrain,
             py::return_value_policy::reference_internal,
             "Get the brain object from this context.");

    // Batch operations
    py::class_<BatchOperations>(m, "BatchOperations", R"pbdoc(Helper class for batch operations)pbdoc")
        .def_static("produce_actions", &BatchOperations::produceActions,
                   py::arg("brain"), py::arg("count"),
                   "Produce multiple actions in a batch.")
        .def_static("get_stats", &BatchOperations::getBrainStats,
                   py::arg("brain"), py::arg("stat_name"),
                   "Get brain statistics by name.");

    // Factory functions
    m.def("createBrain", createBrainWithConfig, py::arg("config"),
          "Create and initialize a brain with the given configuration.");

    m.def("createBrainQuick", createBrainQuick, py::arg("neuron_count"), py::arg("region_count"),
          "Quick brain creation with default settings.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface.");

    m.def("runSimulation", runSimulation,
          "Run a complete simulation with brain, world, and agent.");

    m.def("BrainBuilder", &BrainBuilderFactory::create,
          "Create a BrainBuilder for configurable brain creation.");

    // Special values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error types (for users to catch)
    py::class_<NLMError>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>());

    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>());

    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>());

    py::class_<MemoryError>(m, "MemoryError", py::module_local())
        .def(py::init<const std::string&>());
}

} // namespace nlm
