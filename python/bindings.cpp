#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
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

// Forward declarations for context managers and advanced features
class PyBrainWrapper;
class PyConfigHelper;

// Configuration presets helper
class PyConfigHelper {
public:
    PyConfigHelper() {}
    
    static std::shared_ptr<Config> createMinimalBrainConfig(size_t neuronCount = 1000) {
        auto config = std::make_shared<Config>();
        config->set<size_t>("neuron_count", neuronCount);
        config->set<size_t>("region_count", 1);
        config->set<float>("connection_probability", 0.1f);
        config->set<double>("simulation_timestep", 0.001);
        return config;
    }
    
    static std::shared_ptr<Config> createStandardBrainConfig(size_t neuronCount = 1000) {
        auto config = createMinimalBrainConfig(neuronCount);
        config->set<float>("stdp_ltp_weight", 0.01f);
        config->set<float>("stdp_ltd_weight", 0.012f);
        config->set<float>("stdp_tau", 20.0f);
        config->set<float>("synaptogenesis_rate", 0.0001f);
        config->set<float>("pruning_rate", 0.00001f);
        config->set<size_t>("replay_interval", 100);
        config->set<size_t>("consolidation_interval", 1000);
        config->set<std::string>("checkpoint_dir", "./checkpoints");
        config->set<uint64_t>("random_seed", 42);
        return config;
    }
    
    static std::shared_ptr<Config> createResearchBrainConfig(size_t neuronCount = 5000) {
        auto config = createStandardBrainConfig(neuronCount);
        config->set<size_t>("neuron_count", neuronCount);
        config->set<size_t>("region_count", 4);
        config->set<float>("connection_probability", 0.05f);
        config->set<double>("simulation_timestep", 0.0001);
        return config;
    }
    
    static std::shared_ptr<Config> createDevelopmentBrainConfig() {
        auto config = createStandardBrainConfig(2000);
        config->set<size_t>("region_count", 2);
        config->set<float>("synaptogenesis_rate", 0.001f);
        config->set<float>("pruning_rate", 0.0001f);
        return config;
    }
};

// Advanced Brain wrapper with Pythonic API
class PyBrainWrapper {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    bool initialized = false;
    
public:
    PyBrainWrapper(size_t neuronCount = 1000) {
        auto config = PyConfigHelper::createStandardBrainConfig(neuronCount);
        brain = std::make_shared<Brain>(config);
        world = std::make_shared<SimpleWorld>();
    }
    
    PyBrainWrapper(std::shared_ptr<Config> config) {
        brain = std::make_shared<Brain>(config);
        world = std::make_shared<SimpleWorld>();
    }
    
    bool initialize() {
        if (brain->initialize()) {
            initialized = true;
            return true;
        }
        return false;
    }
    
    // Pythonic property access
    size_t getNeuronCount() const {
        return brain->getTotalNeuronCount();
    }
    
    size_t getRegionCount() const {
        return brain->getRegionCount();
    }
    
    float getExcitementInhibitionRatio() const {
        return brain->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikeCount() const {
        return brain->getTotalSpikeCount();
    }
    
    double getAverageFiringRate() const {
        return brain->getAverageFiringRate();
    }
    
    // Pythonic action methods
    PyBrainWrapper& step(size_t currentStep) {
        brain->step(currentStep);
        return *this;
    }
    
    PyBrainWrapper& stepWithTime(size_t currentStep, double currentTime) {
        brain->step(currentStep, currentTime);
        return *this;
    }
    
    PyBrainWrapper& injectSensory(std::vector<float> data) {
        auto sensoryInput = std::make_unique<Vision>(data.size() / 3, data.size() / 3, 3);
        sensoryInput->setData(data);
        brain->receiveSensoryInput(*sensoryInput);
        return *this;
    }
    
    PyBrainWrapper& injectCurrent(NeuronId neuronId, float current) {
        brain->injectCurrent(neuronId, current);
        return *this;
    }
    
    PyBrainWrapper& injectToNeurons(NeuronType type, float current) {
        brain->injectCurrentToNeurons(type, current);
        return *this;
    }
    
    // Context manager support for easy brain lifecycle management
    struct ContextManager {
        PyBrainWrapper& wrapper;
        ContextManager(PyBrainWrapper& w) : wrapper(w) {}
        void enter() {
            if (!wrapper.initialized) {
                wrapper.initialize();
            }
        }
        void exit(int, int, const std::string&) {
            wrapper.reset();
        }
    };
    
    ContextManager getContext() {
        return ContextManager(*this);
    }
    
    // Fluent interface for chaining operations
    class FluentBrain {
    private:
        PyBrainWrapper& wrapper;
        
    public:
        FluentBrain(PyBrainWrapper& w) : wrapper(w) {}
        
        FluentBrain& step(size_t step) {
            wrapper.step(step);
            return *this;
        }
        
        FluentBrain& injectSensory(std::vector<float> data) {
            wrapper.injectSensory(data);
            return *this;
        }
        
        FluentBrain& advance(size_t steps = 1) {
            for (size_t i = 0; i < steps; ++i) {
                wrapper.step(i);
            }
            return *this;
        }
        
        FluentBrain& withCurrent(float current) {
            // Store current for later injection
            currentToInject = current;
            return *this;
        }
        
        std::vector<float> getObservations() {
            return wrapper.getObservations();
        }
        
        float getLearningRate() {
            return wrapper.getLearningRate();
        }
        
    private:
        float currentToInject;
    };
    
    FluentBrain fluent() {
        return FluentBrain(*this);
    }
    
    // Advanced sensory and motor operations
    std::vector<float> getObservations() const {
        std::vector<float> observations;
        if (world) {
            auto percept = world->getSensoryPercept();
            // Convert percept to observation vector
            auto vision = percept.getVision();
            if (vision) {
                // Flatten vision data
                auto data = vision->getData();
                observations.insert(observations.end(), data.begin(), data.end());
            }
        }
        return observations;
    }
    
    float getLearningRate() const {
        if (brain->getDopamine()) {
            return brain->getDopamine()->getLevel();
        }
        return 0.0f;
    }
    
    // Checkpoint management
    bool save(const std::string& filepath) {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Development system access
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return brain->getDevelopmentalStage();
    }
    
    // Neuromodulation access
    float getCuriosityLevel() const {
        if (brain->getCuriosity()) {
            return brain->getCuriosity()->getLevel();
        }
        return 0.0f;
    }
    
    float getNoveltyLevel() const {
        if (brain->getNovelty()) {
            return brain->getNovelty()->getLevel();
        }
        return 0.0f;
    }
    
    float getPredictionError() const {
        if (brain->getPredictionErrorSignal()) {
            return brain->getPredictionErrorSignal()->getError();
        }
        return 0.0f;
    }
    
    // Simulation control
    void enableLearning(bool enable) {
        if (enable) {
            brain->getCuriosity()->enable();
        } else {
            brain->getCuriosity()->disable();
        }
    }
    
    bool isLearningEnabled() const {
        return brain->getCuriosity() && brain->getCuriosity()->isEnabled();
    }
    
    // State management
    void reset() {
        brain->reset();
        initialized = false;
    }
    
    bool isInitialized() const {
        return initialized;
    }
    
    // Brain access for advanced users
    std::shared_ptr<Brain> getUnderlyingBrain() const {
        return brain;
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world;
    }
    
    void setWorld(std::shared_ptr<SimpleWorld> w) {
        world = w;
    }
    
    // String representation
    std::string to_string() const {
        std::stringstream ss;
        ss << "PyBrainWrapper(neuron_count=" << getNeuronCount()
           << ", region_count=" << getRegionCount()
           << ", initialized=" << (initialized ? "true" : "false")
           << ")";
        return ss.str();
    }
    
    friend void setupBindings(py::module& m);
};

// Main bindings setup function
void setupBindings(py::module& m) {
    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register basic types
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
    
    // Config with Pythonic helpers
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
        // Pythonic property access
        .def_property("neuron_count", &Config::get<size_t>, &Config::set<size_t, size_t>)
        .def_property("region_count", &Config::get<size_t>, &Config::set<size_t, size_t>)
        .def_property("simulation_timestep", &Config::get<double>, &Config::set<double, double>)
        .def_property("connection_probability", &Config::get<float>, &Config::set<float, float>);
    
    // Pythonic configuration helpers
    m.def("create_config", [](size_t neuronCount) {
        return PyConfigHelper::createStandardBrainConfig(neuronCount);
    }, py::arg("neuron_count") = 1000, "Create a standard brain configuration");
    
    m.def("create_research_config", [](size_t neuronCount) {
        return PyConfigHelper::createResearchBrainConfig(neuronCount);
    }, py::arg("neuron_count") = 5000, "Create a research-grade brain configuration");
    
    m.def("create_development_config", []() {
        return PyConfigHelper::createDevelopmentBrainConfig();
    }, "Create a development-stage brain configuration");
    
    // Improved SensoryInput with Pythonic API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("setData", [](SensoryInput& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the sensory data");
    
    // Pythonic Vision with convenience constructors
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
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
             "Create a new vision tensor with given dimensions");
    
    // Audio with convenience API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("normalize", &Audio::setData, py::arg("data"),
             "Normalize audio data to [-1, 1] range");
    
    // InternalSignals with Pythonic API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignals", &InternalSignals::getData,
             "Get all signal values as a vector")
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float signal : signals) {
                self.addSignal(signal);
            }
        }, py::arg("signals"), "Set all signals at once");
    
    // Improved Action with fluent interface
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
        // Pythonic convenience methods
        .def("addParameter", [](Action& self, float param) {
            auto params = self.getParameters();
            params.push_back(param);
            self.setParameters(params);
        }, py::arg("param"), "Add a parameter to the action")
        .def("getParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            return (index < params.size()) ? params[index] : 0.0f;
        }, py::arg("index"), "Get a specific parameter")
        .def("setAllParameters", [](Action& self, std::vector<float> params) {
            self.setParameters(params);
        }, py::arg("params"), "Set all parameters at once");
    
    // Improved WorldObject with Pythonic API
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
        // Pythonic convenience methods
        .def("move", [](WorldObject& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move the object by (dx, dy)")
        .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("isWithinRange", [](const WorldObject& self, const WorldObject& other, float range) {
            float distance = self.distanceTo(other);
            return distance <= range;
        }, py::arg("other"), py::arg("range"), "Check if object is within range of another");
    
    // Improved AgentBody with Pythonic API
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
        // Pythonic convenience methods
        .def("moveForward", [](AgentBody& self, float distance) {
            float rad = self.orientation * M_PI / 180.0f;
            self.x += distance * std::cos(rad);
            self.y += distance * std::sin(rad);
            self.isMoving = true;
        }, py::arg("distance"), "Move forward by given distance")
        .def("turn", [](AgentBody& self, float angle) {
            self.orientation += angle;
            self.isTurning = true;
        }, py::arg("angle"), "Turn by given angle (degrees)")
        .def("updatePosition", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = true;
        }, py::arg("dx"), py::arg("dy"), "Update position by (dx, dy)")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position to (x, y)");
    
    // Improved ActionResult with Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Pythonic convenience methods
        .def("isSuccessful", &ActionResult::success,
             "Check if action was successful")
        .def("getReward", &ActionResult::reward,
             "Get the reward value")
        .def("getMessage", &ActionResult::message,
             "Get the result message")
        .def("setSuccess", &ActionResult::success,
             "Set success flag")
        .def("setReward", &ActionResult::reward,
             "Set reward value")
        .def("setMessage", &ActionResult::message,
             "Set result message");
    
    // Improved SensoryPercept with Pythonic API
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
        // Pythonic convenience methods
        .def("updateVision", [](SensoryPercept& self, size_t width, size_t height) {
            auto vision = std::make_unique<Vision>(width, height, 3);
            self.setVision(*vision);
        }, py::arg("width"), py::arg("height"), "Update vision with new dimensions")
        .def("getNormalizedVision", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            if (vision) {
                auto data = vision->getData();
                // Normalize to [0, 1]
                float maxVal = *std::max_element(data.begin(), data.end());
                if (maxVal > 0.0f) {
                    for (auto& val : data) {
                        val /= maxVal;
                    }
                }
                return data;
            }
            return std::vector<float>();
        }, "Get normalized vision data (values in [0, 1])")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if percept has vision data");
    
    // Improved SimpleWorld with Pythonic API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        // Pythonic convenience methods
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body->x, body->y);
        }, "Get agent position as (x, y) tuple")
        .def("moveAgent", [](SimpleWorld& self, float dx, float dy) {
            auto body = self.getAgentBody();
            body->x += dx;
            body->y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move agent by (dx, dy)")
        .def("turnAgent", [](SimpleWorld& self, float angle) {
            auto body = self.getAgentBody();
            body->orientation += angle;
        }, py::arg("angle"), "Turn agent by angle (degrees)")
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add a resource object")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float radius) {
            WorldObject obj(x, y, WorldObjectType::Hazard, 0.0f, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Add a hazard object");
    
    // Improved Brain with Pythonic API
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
    
    // Improved AgentBrain with Pythonic API
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
    
    // PyBrainWrapper with Pythonic API
    py::class_<PyBrainWrapper>(m, "Brain", R"pbdoc(Improved Pythonic brain interface with fluent API)pbdoc")
        .def(py::init<size_t>(), py::arg("neuron_count") = 1000,
             "Create a brain wrapper with standard configuration")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain wrapper with custom configuration")
        .def("initialize", &PyBrainWrapper::initialize,
             "Initialize the brain wrapper")
        // Pythonic property access
        .def("getNeuronCount", &PyBrainWrapper::getNeuronCount,
             "Get total neuron count")
        .def("getRegionCount", &PyBrainWrapper::getRegionCount,
             "Get total region count")
        .def("getExcitementInhibitionRatio", &PyBrainWrapper::getExcitementInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &PyBrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("getAverageFiringRate", &PyBrainWrapper::getAverageFiringRate,
             "Get average firing rate")
        // Fluent interface methods
        .def("step", &PyBrainWrapper::step, py::arg("currentStep"),
             "Perform a simulation step (fluent interface)")
        .def("stepWithTime", &PyBrainWrapper::stepWithTime,
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with time (fluent interface)")
        .def("injectSensory", &PyBrainWrapper::injectSensory, py::arg("data"),
             "Inject sensory input (fluent interface)")
        .def("injectCurrent", &PyBrainWrapper::injectCurrent,
             py::arg("neuronId"), py::arg("current"),
             "Inject current into neuron (fluent interface)")
        .def("injectToNeurons", &PyBrainWrapper::injectToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of type (fluent interface)")
        // Context manager support
        .def(py::init_alias<PyBrainWrapper::ContextManager>(), "getContext",
             "Get context manager for brain lifecycle management")
        // FluentBrain interface
        .def("fluent", &PyBrainWrapper::fluent,
             "Get fluent interface for chaining operations")
        // Advanced operations
        .def("getObservations", &PyBrainWrapper::getObservations,
             "Get sensory observations from world")
        .def("getLearningRate", &PyBrainWrapper::getLearningRate,
             "Get current learning rate (neuromodulation level)")
        .def("save", &PyBrainWrapper::save, py::arg("filepath"),
             "Save brain state")
        .def("load", &PyBrainWrapper::load, py::arg("filepath"),
             "Load brain state")
        .def("setDevelopmentalStage", &PyBrainWrapper::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getDevelopmentalStage", &PyBrainWrapper::getDevelopmentalStage,
             "Get developmental stage")
        .def("getCuriosityLevel", &PyBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &PyBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &PyBrainWrapper::getPredictionError,
             "Get prediction error")
        .def("enableLearning", &PyBrainWrapper::enableLearning,
             py::arg("enable"), "Enable/disable learning")
        .def("isLearningEnabled", &PyBrainWrapper::isLearningEnabled,
             "Check if learning is enabled")
        .def("reset", &PyBrainWrapper::reset,
             "Reset brain wrapper")
        .def("isInitialized", &PyBrainWrapper::isInitialized,
             "Check if brain is initialized")
        // String representation
        .def("__repr__", &PyBrainWrapper::to_string,
             "Get string representation")
        .def("__str__", &PyBrainWrapper::to_string,
             "Get string representation");
    
    // SimpleWorld with Pythonic API (alternative interface)
    py::class_<SimpleWorld>(m, "World", R"pbdoc(Alternative Pythonic world interface)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world (alternative interface)")
        .def("reset", &SimpleWorld::reset, "Reset world (alternative interface)")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position (alternative)")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body->x, body->y);
        }, "Get agent position (alternative)")
        .def("moveAgent", [](SimpleWorld& self, float dx, float dy) {
            auto body = self.getAgentBody();
            body->x += dx;
            body->y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move agent (alternative)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept (alternative)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world (alternative)");
    
    // AgentBrain with Pythonic API (alternative interface)
    py::class_<AgentBrain>(m, "Agent", R"pbdoc(Alternative Pythonic agent interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent with brain (alternative interface)")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent (alternative interface)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory input (alternative interface)")
        .def("produceAction", &AgentBrain::decodeMotorCommand,
             "Produce action (alternative interface)")
        .def("reset", &AgentBrain::reset,
             "Reset agent (alternative interface)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get underlying brain (alternative interface)");
    
    // Create default config function
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Create brain function (Pythonic)
    m.def("createBrain", [](size_t neuronCount) {
        return PyBrainWrapper(neuronCount);
    }, py::arg("neuron_count") = 1000, "Create a brain with standard configuration (Pythonic)");
    
    m.def("createBrainWithConfig", [](std::shared_ptr<Config> config) {
        return PyBrainWrapper(config);
    }, py::arg("config"), "Create a brain with custom configuration (Pythonic)");
    
    // Create world function
    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new world");
    
    // Create agent function
    m.def("createAgent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent with brain");
    
    // Constants for easy access
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Example usage helper
    m.def("example_usage", []() {
        py::print("Example usage of improved NLM Python bindings:");
        py::print("""
        # Basic usage
        from pynlm import Brain, World, Agent
        
        # Create brain with default configuration
        brain = Brain(neuron_count=1000)
        
        # Use context manager for safe initialization
        with brain.getContext():
            # Simulate some steps
            for i in range(10):
                brain.step(i)
        
        # Or use fluent interface for chaining
        brain = Brain()
        brain.step(0).injectSensory([0.5, 0.8, 0.3]).step(1).step(2)
        
        # Create world and agent
        world = World()
        agent = Agent(brain)
        
        # Create research configuration
        config = create_config(neuron_count=5000)
        brain = Brain(config)
        """);
    }, "Show example usage of improved bindings");
}

} // namespace nlm
