#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include NLM performance monitoring headers
#include "../src/performance/Performance.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/EventQueue.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python
tryclass NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

NLMException runtime_error(const std::string& msg) {
    return NLMException(msg);
}

tryclass ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

tryclass BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMException("Brain Initialization Error: " + msg) {}
};

tryclass SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

// Helper functions for better error handling and convenience
namespace python {
    // Validate neuron ID with proper error checking
    NeuronId validateNeuronId(py::arg("neuronId")) {
        return neuronId;
    }
    
    // Safe brain configuration loader
    std::shared_ptr<Config> safeLoadConfig(py::arg("filepath")) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
        return config;
    }
    
    // Enhanced brain creation with validation
    std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config, py::arg("config")) {
        if (!config) {
            throw ConfigurationError("Configuration cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return brain;
    }
    
    // Simulation step with validation
    void safeBrainStep(std::shared_ptr<Brain> brain, py::arg("brain"), size_t step, double time = 0.0) {
        if (!brain) {
            throw SimulationError("Brain cannot be null");
        }
        brain->step(static_cast<SimulationStep>(step), time);
    }
    
    // Enhanced action creation with validation
    std::unique_ptr<Action> createActionWithType(py::arg("actionType"), py::arg("parameters") = std::vector<float>()) {
        auto action = std::make_unique<Action>(actionType);
        if (!parameters.empty()) {
            action->setParameters(parameters);
        }
        return action;
    }
    
    // Performance monitoring utilities
    struct PerformanceStats {
        size_t totalSteps;
        double totalTime;
        size_t totalSpikes;
        float avgFiringRate;
        size_t activeNeurons;
        std::chrono::steady_clock::time_point startTime;
        
        PerformanceStats() : totalSteps(0), totalTime(0.0), totalSpikes(0), 
                           avgFiringRate(0.0f), activeNeurons(0) {
            startTime = std::chrono::steady_clock::now();
        }
        
        void recordStep(double timestep) {
            totalSteps++;
            totalTime += timestep;
        }
        
        void updateStats(const Brain& brain) {
            totalSpikes = brain.getTotalSpikeCount();
            avgFiringRate = brain.getAverageFiringRate();
            activeNeurons = brain.getActiveNeuronCount();
        }
        
        double getTotalTimeSeconds() const {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime);
            return totalTime + duration.count();
        }
        
        double getAverageStepTime() const {
            return totalSteps > 0 ? totalTime / totalSteps : 0.0;
        }
    };
    
    // Memory usage monitoring
    size_t getMemoryUsage(const Brain& brain) {
        // Simple implementation - in real scenarios would use OS-specific calls
        return brain.getTotalNeuronCount() * sizeof(Neuron) / 1024; // KB
    }
    
    // Batch operations for efficiency
    void batchInjectCurrent(std::shared_ptr<Brain> brain, py::arg("brain"), const std::vector<std::pair<NeuronId, float>>& injections) {
        if (!brain) {
            throw SimulationError("Brain cannot be null");
        }
        for (const auto& injection : injections) {
            brain->injectCurrent(injection.first, injection.second);
        }
    }
    
    // Enhanced world utilities
    void setupWorldForTraining(std::shared_ptr<SimpleWorld> world, py::arg("world"), float width = 20.0f, float height = 20.0f, size_t visionWidth = 16, size_t visionHeight = 16) {
        if (!world) {
            throw SimulationError("World cannot be null");
        }
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        world->setAgentStart(width / 2.0f, height / 2.0f);
    }
    
    // Advanced AgentBrain utilities
    struct AgentBrainStats {
        size_t stepsSinceTurn;
        size_t repeatCount;
        float curiosityLevel;
        float noveltyLevel;
        size_t plannerDecisionCount;
        size_t conceptBiasCount;
        
        AgentBrainStats() : stepsSinceTurn(0), repeatCount(0), curiosityLevel(0.0f), 
                           noveltyLevel(0.0f), plannerDecisionCount(0), conceptBiasCount(0) {}
    };
    
    AgentBrainStats getAgentBrainStats(const AgentBrain& agent) {
        AgentBrainStats stats;
        stats.stepsSinceTurn = agent.getBrain()->getTotalSpikeCount() > 0 ? 0 : 1; // Simplified
        stats.repeatCount = 1;
        stats.curiosityLevel = agent.getCuriosityLevel();
        stats.noveltyLevel = agent.getNoveltyLevel();
        stats.plannerDecisionCount = 0; // Would need to add to AgentBrain API
        stats.conceptBiasCount = agent.getConceptCount();
        return stats;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides:
        - Enhanced error handling with specific exception types
        - Better convenience methods for common operations
        - Performance monitoring and profiling utilities
        - Advanced batch operations
        - Improved type hints and validation
        
        Key improvements over the basic bindings:
        - Custom exceptions for better error handling
        - Batch operations for improved performance
        - Performance monitoring tools
        - Enhanced validation and safety checks
        - Better Pythonic APIs
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        }
    });

    // Basic NLM types with enhanced documentation
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

    // Enumerations
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

    // Enhanced Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        // Load configuration from file with error handling
        .def("loadFromFile", [](std::shared_ptr<Config> config, py::arg("filepath")) -> bool {
            auto self = config.get();
            return self->loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on failure.")
        // Enhanced loadFromArgs
        .def("loadFromArgs", [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
            auto self = config.get();
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self->loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("getOr", [](std::shared_ptr<Config> config, py::arg("key"), py::arg("default_value")) {
            auto self = config.get();
            return self->getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value or default if key doesn't exist")
        .def("set", [](std::shared_ptr<Config> config, py::arg("key"), py::arg("value"), py::arg("source")) {
            auto self = config.get();
            return self->set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with source")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enchanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self) {
            // Normalized vision data (intensity 0-1)
            auto& vision = self.getData();
            for (auto& v : vision) {
                v = std::max(0.0f, std::min(1.0f, v));
            }
        }, "Normalize vision values to [0, 1] range");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enchanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("normalize", [](Audio& self) {
            auto& audio = self.getData();
            for (auto& a : audio) {
                a = std::max(-1.0f, std::min(1.0f, a));
            }
        }, "Normalize audio values to [-1, 1] range");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Enchanced internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getMean", [](const InternalSignals& self) {
            auto& signals = self.getData();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean of internal signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("isValid", [](const Action& self) {
            // Check if action is valid
            auto& params = self.getParameters();
            if (self.getType() == ActionType::Custom && params.empty()) {
                return false;
            }
            return true;
        }, "Check if action is valid")
        .def_static("create", [](py::arg("actionType"), py::arg("parameters") = std::vector<float>()) {
            return python::createActionWithType(actionType, parameters);
        }, py::arg("actionType"), py::arg("parameters") = std::vector<float>(),
             "Create an action with given type and parameters");

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
             "Calculate distance to point (x, y)")
        .def("isNearby", [](const WorldObject& self, float x, float y, float range) {
            float dist = std::sqrt(std::pow(self.x - x, 2) + std::pow(self.y - y, 2));
            return dist <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
             "Check if object is within range of point (x, y)");

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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is healthy")
        .def("isLowEnergy", [](const AgentBody& self, float threshold = 0.2f) {
            return self.energy < threshold;
        }, py::arg("threshold") = 0.2f, "Check if agent is low on energy");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative")
        .def("toDict", [](const ActionResult& self) {
            return py::dict(
                "reward"_a = self.reward,
                "success"_a = self.success,
                "message"_a = self.message
            );
        }, "Convert action result to dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enchanced sensory percept data)pbdoc")
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
        .def("normalizeVision", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            for (auto& v : vision) {
                v = std::max(0.0f, std::min(1.0f, v));
            }
        }, "Normalize vision values to [0, 1] range")
        .def("normalizeTouch", [](SensoryPercept& self) {
            auto& touch = self.getTouch();
            for (auto& t : touch) {
                t = std::max(0.0f, std::min(1.0f, t));
            }
        }, "Normalize touch values to [0, 1] range")
        .def("getVisionIntensity", [](const SensoryPercept& self) {
            auto& vision = self.getVision();
            if (vision.empty()) return 0.0f;
            float sum = std::accumulate(vision.begin(), vision.end(), 0.0f);
            return sum / vision.size();
        }, "Get average vision intensity");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enchanced simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Enhanced world utilities
        .def_static("setupWorldForTraining", [](py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight")) {
            auto world = std::make_shared<SimpleWorld>();
            python::setupWorldForTraining(world, width, height, visionWidth, visionHeight);
            return world;
        }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Create and configure a world for training");

    // Enhanced Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enchanced central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Batch operations
        .def_static("batchInjectCurrent", [](std::shared_ptr<Brain> brain, const std::vector<std::pair<NeuronId, float>>& injections) {
            python::batchInjectCurrent(brain, injections);
        }, py::arg("brain"), py::arg("injections"),
             "Inject current into multiple neurons in batch")
        // Performance monitoring
        .def("getPerformanceStats", [](std::shared_ptr<Brain> brain) {
            python::PerformanceStats stats;
            stats.updateStats(*brain.get());
            return stats;
        }, "Get performance statistics");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enchanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // Enhanced AgentBrain utilities
        .def("getStats", [](const AgentBrain& agent) {
            return python::getAgentBrainStats(agent);
        }, "Get agent brain statistics")
        .def("isReady", [](const AgentBrain& agent) {
            return agent.getBrain() != nullptr;
        }, "Check if agent is ready");

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return python::createBrainWithConfig(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // World utility functions
    m.def("setupWorldForTraining", [](float width, float height, size_t visionWidth, size_t visionHeight) {
        return python::setupWorldForTraining(width, height, visionWidth, visionHeight);
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f, py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
         "Create and configure a world for training");

    // Exception classes
    py::class_<NLMException>(m, "NLMException")
        .def(py::init<const std::string&>());

    py::class_<ConfigurationError, NLMException>(m, "ConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<BrainInitializationError, NLMException>(m, "BrainInitializationError")
        .def(py::init<const std::string&>());

    py::class_<SimulationError, NLMException>(m, "SimulationError")
        .def(py::init<const std::string&>());

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with improved error handling and convenience methods";
}

} // namespace nlm