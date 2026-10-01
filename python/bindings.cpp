#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>

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

// Performance monitoring and statistics
struct PythonPerformanceMonitor {
    struct StepStats {
        SimulationStep step;
        Timestamp time;
        size_t spikeCount;
        size_t firingNeuronCount;
        float avgFiringRate;
        double cpuTime;
    };
    
    std::vector<StepStats> history;
    size_t totalSteps;
    double totalCPUTime;
    
    PythonPerformanceMonitor() : totalSteps(0), totalCPUTime(0.0) {}
    
    void recordStep(SimulationStep step, Timestamp time, size_t spikes, size_t firingNeurons, float avgFiringRate, double cpuTime) {
        StepStats stats{step, time, spikes, firingNeurons, avgFiringRate, cpuTime};
        history.push_back(stats);
        totalSteps++;
        totalCPUTime += cpuTime;
    }
    
    double getAverageCPUTimePerStep() const {
        return totalSteps > 0 ? totalCPUTime / totalSteps : 0.0;
    }
    
    size_t getMaxSpikesInStep() const {
        if (history.empty()) return 0;
        return std::max_element(history.begin(), history.end(),
                               [](const StepStats& a, const StepStats& b) {
                                   return a.spikeCount < b.spikeCount;
                               })->spikeCount;
    }
    
    double getAverageFiringRate() const {
        if (history.empty()) return 0.0;
        double total = std::accumulate(history.begin(), history.end(), 0.0,
                                      [](double sum, const StepStats& stats) {
                                          return sum + stats.avgFiringRate;
                                      });
        return total / history.size();
    }
};

// Advanced configuration utilities
class AdvancedConfig {
public:
    static std::shared_ptr<Config> loadConfigFromJSON(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }
    
    static std::shared_ptr<Config> loadConfigFromDict(const std::map<std::string, std::string>& dict) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : dict) {
            config->set(pair.first, pair.second);
        }
        return config;
    }
    
    static std::map<std::string, std::string> configToDict(const Config& config) {
        std::map<std::string, std::string> dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (config.has(key)) {
                dict[key] = config.get<std::string>(key);
            }
        }
        return dict;
    }
    
    static std::string configToJSON(const Config& config) {
        std::stringstream ss;
        ss << "{\n";
        auto keys = config.getKeys();n
        for (size_t i = 0; i < keys.size(); ++i) {
            ss << "  \"" << keys[i] << "\": \"" << config.get<std::string>(keys[i]) << "\"";
            if (i < keys.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "}";
        return ss.str();
    }
    
    static std::shared_ptr<Config> getOptimizedConfig(const std::string& useCase) {
        auto config = std::make_shared<Config>();
        
        if (useCase == "fast") {
            config->set("brain.neuron_count", "100");
            config->set("brain.synapse_density", "0.05");
            config->set("plasticity.stdp.enable", "true");
            config->set("plasticity.stdp.learning_rate", "0.001");
            config->set("neuromod.dopamine.scale", "1.0");
        } else if (useCase == "detailed") {
            config->set("brain.neuron_count", "10000");
            config->set("brain.synapse_density", "0.15");
            config->set("plasticity.stdp.enable", "true");
            config->set("plasticity.hebbian.enable", "true");
            config->set("plasticity.structural.enable", "true");
            config->set("neuromod.curiosity.enable", "true");
            config->set("neuromod.novelty.enable", "true");
        } else if (useCase == "balanced") {
            config->set("brain.neuron_count", "1000");
            config->set("brain.synapse_density", "0.1");
            config->set("plasticity.stdp.enable", "true");
            config->set("plasticity.hebbian.enable", "true");
            config->set("neuromod.curiosity.enable", "true");
        }
        
        return config;
    }
};

// Batch processing utilities
class BatchProcessor {
public:
    struct BatchResult {
        size_t stepsCompleted;
        size_t totalSteps;
        size_t totalSpikes;
        double avgCPUTimePerStep;
        float finalFiringRate;
        std::vector<float> firingHistory;
    };
    
    static BatchResult runBatchSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t numSteps,
        bool enableNeuromodulation = true,
        bool enableDevelopment = true,
        bool enableCuriosity = true
    ) {
        BatchResult result;
        result.stepsCompleted = 0;
        result.totalSteps = numSteps;
        result.totalSpikes = 0;
        result.firingHistory.reserve(numSteps);
        
        // Enable requested subsystems
        if (agent) {
            if (enableNeuromodulation) agent->enableRewardModulation(true);
            if (enableDevelopment) agent->enableDevelopment(true);
            if (enableCuriosity) agent->enableCuriosity(true);
        }
        
        // Record initial state
        result.firingHistory.push_back(brain->getAverageFiringRate());
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                    brain->step(step);
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Apply neuromodulation if enabled
                    if (enableNeuromodulation) {
                        float reward = 0.0f;
                        if (world->getSensoryPercept().getInternal()) {
                            reward = world->getSensoryPercept().getInternal()[0];
                        }
                        agent->applyRewardModulation(reward, 0.0f);
                    }
                    
                    // Update development
                    if (enableDevelopment) {
                        agent->updateDevelopment(0.1);
                    }
                } else {
                    brain->step(step);
                }
            } else {
                brain->step(step);
            }
            
            // Collect statistics
            result.totalSpikes += brain->getTotalSpikeCount();
            result.firingHistory.push_back(brain->getAverageFiringRate());
            result.stepsCompleted++;
        }
        
        result.avgCPUTimePerStep = 0.01; // Placeholder - would use actual timing
        result.finalFiringRate = brain->getAverageFiringRate();
        
        return result;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides advanced functionality for:
        - Batch processing and simulation utilities
        - Performance monitoring and statistics
        - Advanced configuration management
        - Enhanced error handling and debugging tools
        - Statistical analysis and data export
        
        Features:
        * Real-time performance monitoring
        * Batch simulation execution
        * Advanced configuration loading/saving
        * Statistical analysis tools
        * Enhanced error reporting
        * Data export utilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Add standard neuron type definitions
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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        // Enhanced type-safe getters
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.get<int>(key).value_or(defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0,
            "Get integer value with default fallback")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return self.get<float>(key).value_or(defaultValue);
        }, py::arg("key"), py::arg("default_value") = 0.0f,
            "Get float value with default fallback")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.get<std::string>(key).value_or(defaultValue);
        }, py::arg("key"), py::arg("default_value") = "",
            "Get string value with default fallback")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.get<bool>(key).value_or(defaultValue);
        }, py::arg("key"), py::arg("default_value") = false,
            "Get boolean value with default fallback")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value ? "true" : "false");
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (bool)");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& self) {
            return py::dict(
                "type"_a = self.getType(),
                "data"_a = self.getData(),
                "dimensions"_a = self.getDimensions(),
                "timestamp"_a = self.getTimestamp()
            );
        }, "Convert sensory input to dictionary");

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
        .def("to_array", [](Vision& self) {
            return self.getData();
        }, "Convert vision data to numpy array");

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
        .def("getAllSignals", &InternalSignals::getAllSignals,
             "Get all signal values as vector");

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
        .def("to_dict", [](const Action& self) {
            return py::dict(
                "type"_a = self.getType(),
                "parameters"_a = self.getParameters(),
                "name"_a = self.getName()
            );
        }, "Convert action to dictionary");

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
        .def("to_dict", [](const WorldObject& self) {
            return py::dict(
                "x"_a = self.x,
                "y"_a = self.y,
                "radius"_a = self.radius,
                "type"_a = self.type,
                "value"_a = self.value,
                "active"_a = self.active
            );
        }, "Convert world object to dictionary");

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
        .def("setTouch", &SensoryPercept::getTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::getProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::getAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced simple 2D world for NLM simulation)pbdoc")
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
        .def("to_dict", [](const SimpleWorld& self) {
            return py::dict(
                "width"_a = self.getWidth(),
                "height"_a = self.getHeight(),
                "simulationTime"_a = self.getSimulationTime(),
                "maxEnergy"_a = self.getMaxEnergy(),
                "energyDecayRate"_a = self.getEnergyDecayRate()
            );
        }, "Convert world state to dictionary");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
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
        // Enhanced statistics and analysis
        .def("getStatistics", [](std::shared_ptr<Brain> brain) {
            return py::dict(
                "totalNeurons"_a = brain->getTotalNeuronCount(),
                "totalSynapses"_a = brain->getTotalSynapseCount(),
                "activeNeurons"_a = brain->getActiveNeuronCount(),
                "firingNeurons"_a = brain->getFiringNeuronCount(),
                "avgFiringRate"_a = brain->getAverageFiringRate(),
                "eIRatio"_a = brain->getExcitationInhibitionRatio(),
                "totalSpikes"_a = brain->getTotalSpikeCount(),
                "devStage"_a = static_cast<int>(brain->getDevelopmentalStage())
            );
        }, "Get comprehensive brain statistics as dictionary")
        .def("getRegionStatistics", [](std::shared_ptr<Brain> brain, RegionId regionId) {
            auto region = brain->getRegion(regionId);
            if (!region) {
                throw std::runtime_error("Region not found");
            }
            return py::dict(
                "id"_a = regionId.value,
                "neuronCount"_a = region->getTotalNeuronCount(),
                "activeNeurons"_a = region->getActiveNeuronCount(),
                "firingNeurons"_a = region->getFiringNeuronCount(),
                "avgFiringRate"_a = region->getAverageFiringRate(),
                "totalSynapses"_a = region->getSynapseCount(),
                "synapticDensity"_a = region->getSynapticDensity()
            );
        }, py::arg("regionId"),
            "Get statistics for a specific region")
        .def("analyzeNetworkHealth", [](std::shared_ptr<Brain> brain) {
            auto stats = brain->getStatistics();
            
            float healthScore = 100.0f;
            
            // Check various health metrics
            float firingRate = stats["avgFiringRate"].cast<float>();
            if (firingRate < 0.1f || firingRate > 10.0f) {
                healthScore -= 20.0f; // Abnormal firing rate
            }
            
            float eir = stats["eIRatio"].cast<float>();
            if (eir < 0.5f || eir > 2.0f) {
                healthScore -= 15.0f; // Abnormal E/I balance
            }
            
            size_t firingNeurons = stats["firingNeurons"].cast<size_t>();
            size_t totalNeurons = stats["totalNeurons"].cast<size_t>();
            if (totalNeurons > 0) {
                float firingPercentage = static_cast<float>(firingNeurons) / totalNeurons;
                if (firingPercentage < 0.01f || firingPercentage > 0.5f) {
                    healthScore -= 10.0f; // Abnormal firing percentage
                }
            }
            
            return py::dict(
                "healthScore"_a = std::max(0.0f, healthScore),
                "overallStatus"_a = (healthScore > 80.0f ? "Good" :
                                    healthScore > 60.0f ? "Fair" : "Poor"),
                "recommendations"_a = std::vector<std::string>{
                    (healthScore < 80.0f ? "Consider adjusting learning rates" : "None"),
                    (eir < 0.5f || eir > 2.0f ? "Balance E/I ratio" : "None"),
                    (firingRate < 0.1f || firingRate > 10.0f ? "Adjust neuron excitability" : "None")
                }
            );
        }, "Analyze network health and provide recommendations");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // Enhanced AgentBrain methods
        .def("getAgentStats", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
            auto percept = world->getSensoryPercept();
            return py::dict(
                "sensorySize"_a = agent->getSensoryInputSize(),
                "motorSize"_a = agent->getMotorOutputSize(),
                "devStage"_a = static_cast<int>(agent->getDevelopmentalStage()),
                "neuromodLevel"_a = agent->getNeuromodulationLevel(),
                "curiosityLevel"_a = agent->getCuriosityLevel(),
                "noveltyLevel"_a = agent->getNoveltyLevel(),
                "predictionError"_a = agent->getPredictionError(),
                "enabledModules"_a = std::vector<std::string>{
                    (agent->isRewardModulationEnabled() ? "Reward" : ""),
                    (agent->isStructuralPlasticityEnabled() ? "StructuralPlasticity" : ""),
                    (agent->isDevelopmentEnabled() ? "Development" : ""),
                    (agent->isCuriosityEnabled() ? "Curiosity" : "")
                }
            );
        }, py::arg("world"),
            "Get comprehensive agent statistics")
        .def("compareWithWorld", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
            auto body = world->getAgentBody();
            return py::dict(
                "agentPos"_a = py::dict("x"_a = body.x, "y"_a = body.y, "orientation"_a = body.orientation),
                "agentState"_a = py::dict(
                    "energy"_a = body.energy,
                    "health"_a = body.health,
                    "age"_a = body.age,
                    "isMoving"_a = body.isMoving,
                    "isTurning"_a = body.isTurning
                ),
                "actionReady"_a = agent->decodeMotorCommand().getType() != ActionType::Wait
            );
        }, py::arg("world"),
            "Compare agent state with world state");

    // Factory functions with enhanced options
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Enhanced factory functions with optional parameters
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Create a simple world with specified dimensions");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create and initialize an agent brain with world");

    // Performance monitoring functions
    m.def("createPerformanceMonitor", []() {
        return std::make_shared<PythonPerformanceMonitor>();
    }, "Create a performance monitor for tracking simulation statistics");

    m.def("runSimulationWithMonitoring", [](std::shared_ptr<Brain> brain, std::shared_ptr<PythonPerformanceMonitor> monitor, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            auto startTime = std::chrono::high_resolution_clock::now();
            brain->step(step);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            monitor->recordStep(step, static_cast<Timestamp>(step) * 0.001,
                               brain->getTotalSpikeCount(), brain->getFiringNeuronCount(),
                               brain->getAverageFiringRate(), duration);
        }
    }, py::arg("brain"), py::arg("monitor"), py::arg("numSteps"),
        "Run simulation with performance monitoring");

    // Batch processing functions
    m.def("runBatchSimulation", &BatchProcessor::runBatchSimulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
          py::arg("enableNeuromodulation") = true,
          py::arg("enableDevelopment") = true,
          py::arg("enableCuriosity") = true,
          "Run a complete batch simulation with agent");

    // Advanced configuration utilities
    m.def("loadConfigFromJSON", &AdvancedConfig::loadConfigFromJSON,
          py::arg("filepath"), "Load configuration from JSON file");

    m.def("createOptimizedConfig", &AdvancedConfig::getOptimizedConfig,
          py::arg("useCase"), "Create optimized configuration for specific use case");

    // Statistical analysis and export functions
    m.def("exportBrainData", [](std::shared_ptr<Brain> brain, const std::string& filepath, const std::string& format = "csv") {
        // Simple data export functionality
        auto stats = brain->getStatistics();
        
        if (format == "json") {
            // JSON export
            std::ofstream file(filepath);
            file << "{\n";
            file << "  \"brainStats\": {\n";
            // Write stats as JSON (simplified)
            file << "    \"totalNeurons\": " << stats["totalNeurons"].cast<size_t>() << "," << std::endl;
            file << "    \"totalSynapses\": " << stats["totalSynapses"].cast<size_t>() << "," << std::endl;
            file << "    \"firingNeurons\": " << stats["firingNeurons"].cast<size_t>() << "," << std::endl;
            file << "    \"avgFiringRate\": " << stats["avgFiringRate"].cast<float>() << std::endl;
            file << "  }\n";
            file << "}\n";
            file.close();
        } else {
            // CSV export
            std::ofstream file(filepath);
            file << "metric,value\n";
            file << "total_neurons," << stats["totalNeurons"].cast<size_t>() << std::endl;
            file << "total_synapses," << stats["totalSynapses"].cast<size_t>() << std::endl;
            file << "active_neurons," << stats["activeNeurons"].cast<size_t>() << std::endl;
            file << "firing_neurons," << stats["firingNeurons"].cast<size_t>() << std::endl;
            file << "avg_firing_rate," << stats["avgFiringRate"].cast<float>() << std::endl;
            file << "e_i_ratio," << stats["eIRatio"].cast<float>() << std::endl;
            file << "total_spikes," << stats["totalSpikes"].cast<size_t>() << std::endl;
            file.close();
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("format") = "csv",
        "Export brain statistics to file");

    m.def("analyzeSimulationData", [](const std::vector<float>& firingHistory) {
        if (firingHistory.empty()) {
            return py::dict("error"_a = "No data provided");
        }
        
        float minFiring = *std::min_element(firingHistory.begin(), firingHistory.end());
        float maxFiring = *std::max_element(firingHistory.begin(), firingHistory.end());
        float avgFiring = std::accumulate(firingHistory.begin(), firingHistory.end(), 0.0f) / firingHistory.size();
        
        // Calculate standard deviation
        float variance = 0.0f;
        for (float rate : firingHistory) {
            variance += (rate - avgFiring) * (rate - avgFiring);
        }
        variance /= firingHistory.size();
        float stdDev = std::sqrt(variance);
        
        return py::dict(
            "minFiringRate"_a = minFiring,
            "maxFiringRate"_a = maxFiring,
            "avgFiringRate"_a = avgFiring,
            "stdDevFiringRate"_a = stdDev,
            "stabilityScore"_a = (stdDev < avgFiring * 0.1f ? "High" : "Low")
        );
    }, py::arg("firingHistory"),
        "Analyze simulation firing rate data and return statistics");

    // Error handling and validation utilities
    m.def("validateSimulationParameters", [](size_t neuronCount, float synapseDensity) {
        std::vector<std::string> warnings;
        
        if (neuronCount < 10) {
            warnings.push_back("Low neuron count may result in insufficient network complexity");
        } else if (neuronCount > 100000) {
            warnings.push_back("High neuron count may impact performance");
        }
        
        if (synapseDensity < 0.01f) {
            warnings.push_back("Very low synapse density may limit network connectivity");
        } else if (synapseDensity > 0.5f) {
            warnings.push_back("High synapse density may impact simulation stability");
        }
        
        return py::dict(
            "valid"_a = (warnings.empty()),
            "warnings"_a = warnings,
            "recommendations"_a = std::vector<std::string>{
                (neuronCount < 10 ? "Increase neuron count for better results" : "None"),
                (synapseDensity < 0.01f ? "Increase synapse density for better connectivity" : "None")
            }
        );
    }, py::arg("neuronCount"), py::arg("synapseDensity"),
        "Validate simulation parameters and return warnings/recommendations");

    // Debug and development utilities
    m.def("debugSensoryInput", [](std::shared_ptr<SensoryInput> input) {
        return py::dict(
            "type"_a = input->getType(),
            "dimensions"_a = input->getDimensions(),
            "timestamp"_a = input->getTimestamp(),
            "dataSize"_a = input->getData().size(),
            "isValid"_a = !input->getData().empty()
        );
    }, py::arg("input"),
        "Debug sensory input and return analysis");

    m.def("compareActions", [](const Action& action1, const Action& action2) {
        return py::dict(
            "typesMatch"_a = action1.getType() == action2.getType(),
            "parametersMatch"_a = action1.getParameters() == action2.getParameters(),
            "namesMatch"_a = action1.getName() == action2.getName(),
            "identical"_a = (action1.getType() == action2.getType() && 
                           action1.getParameters() == action2.getParameters() &&
                           action1.getName() == action2.getName())
        );
    }, py::arg("action1"), py::arg("action2"),
        "Compare two actions and return comparison results");

    // Constants for easy access
    m.attr("USE_CASE_FAST") = "fast";
    m.attr("USE_CASE_BALANCED") = "balanced";
    m.attr("USE_CASE_DETAILED") = "detailed";
    
    m.attr("FORMAT_JSON") = "json";
    m.attr("FORMAT_CSV") = "csv";
    
    m.attr("STABILITY_HIGH") = "High";
    m.attr("STABILITY_LOW") = "Low";
    
    m.attr("STATUS_GOOD") = "Good";
    m.attr("STATUS_FAIR") = "Fair";
    m.attr("STATUS_POOR") = "Poor";

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}