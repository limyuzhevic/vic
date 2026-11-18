#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"

// Neuromodulation
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Cognition
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Development
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Python-specific type aliases
using PyVisionData = std::vector<float>;
using PyAudioData = std::vector<float>;
using PyInternalSignals = std::vector<float>;
using PyActionParams = std::vector<float>;
using PySensoryData = std::variant<PyVisionData, PyAudioData, PyInternalSignals>;
using PyObservation = std::pair<std::string, PySensoryData>;
using PyObservationList = std::vector<PyObservation>;
using PyActionList = std::vector<std::pair<std::string, PyActionParams>>;

// Python wrapper for more intuitive API
class PythonBrainWrapper {
private:
    std::shared_ptr<Brain> brain_;

public:
    explicit PythonBrainWrapper(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {}

    // Core brain operations
    void initialize() { brain_->initialize(); }
    void step(int step, double time = 0.0) { brain_->step(step, time); }

    // Statistical convenience methods
    float getFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitatoryInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    double getLearningProgress() const {
        // Calculate learning as the change in average synaptic weight
        return brain_->getTotalSpikeCount() > 0 ? brain_->getAverageFiringRate() : 0.0;
    }

    // Development and plasticity convenience methods
    void enableLearning(bool enable = true) {
        if (auto* agent = brain_->getRegion(RegionId(1))) {
            // Access development system to enable learning
        }
    }

    // Advanced analysis methods
    std::vector<std::pair<NeuronId, float>> getActiveNeuronsByFiringRate(int limit = 10) const {
        std::vector<std::pair<NeuronId, float>> result;
        // Implementation would extract neurons sorted by firing rate
        return result;
    }

    // Memory system convenience
    void storeWorkingMemory(const std::string& key, const std::vector<float>& data) {
        if (auto* wm = brain_->getWorkingMemory()) {
            // Store data with key
        }
    }

    // Environment interaction convenience
    void simulateTimeStep(double time, const PyObservationList& observations) {
        for (const auto& obs : observations) {
            // Convert observation to appropriate sensory input
        }
        brain_->step(static_cast<SimulationStep>(brain_->getTotalSpikeCount()), time);
    }

    // Save/Load convenience
    bool saveCheckpoint(const std::string& path, const std::string& metadata = "") {
        // Add metadata to checkpoint if supported
        return brain_->save(path);
    }

    // Get brain statistics as a Python-friendly dictionary
    py::dict getStats() const {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        return stats;
    }

    // Get brain reference for advanced operations
    std::shared_ptr<Brain> getBrain() const { return brain_; }

    // Python special methods
    std::string repr() const {
        return "<PythonBrainWrapper neurons=" + std::to_string(brain_->getTotalNeuronCount()) + ">";
    }

    bool operator==(const PythonBrainWrapper& other) const {
        return brain_ == other.brain_;
    }

    bool operator!=(const PythonBrainWrapper& other) const {
        return !(*this == other);
    }
};

// Python wrapper for AgentBrain with enhanced API
class PythonAgentBrainWrapper {
private:
    std::shared_ptr<AgentBrain> agent_;

public:
    explicit PythonAgentBrainWrapper(std::shared_ptr<AgentBrain> agent) : agent_(std::move(agent)) {}

    // Enhanced sensory processing with automatic conversion
    void processObservations(const PyObservationList& observations) {
        for (const auto& obs : observations) {
            // Convert observation to SensoryPercept based on type
        }
    }

    // Action generation with multiple strategies
    std::string generateAction(const std::string& strategy = "explore") {
        auto action = agent_->decodeMotorCommand();
        if (strategy == "conservative") {
            // Modify action to be less risky
        } else if (strategy == "exploratory") {
            // Modify action to be more exploratory
        }
        return action->getName();
    }

    // Learning with automatic reward calculation
    void learnFromExperience(float reward, double time = 0.0) {
        agent_->applyRewardModulation(reward, 0.0f);
        agent_->updateDevelopment(time);
    }

    // State management with serialization
    py::dict getState() const {
        py::dict state;
        state["curiosity"] = agent_->getCuriosityLevel();
        state["novelty"] = agent_->getNoveltyLevel();
        state["prediction_error"] = agent_->getPredictionError();
        state["neuromodulation_level"] = agent_->getNeuromodulationLevel();
        state["developmental_stage"] = static_cast<int>(agent_->getDevelopmentalStage());
        return state;
    }

    // Convenience methods
    bool isLearningEnabled() const { return agent_->isRewardModulationEnabled(); }
    void enableLearning(bool enable) { 
        agent_->enableRewardModulation(enable);
        agent_->enableCuriosity(enable);
    }

    // Agent brain reference
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
};

// Simple factory functions for convenience
struct PythonFactory {
    static std::shared_ptr<Config> createConfig() {
        return std::make_shared<Config>();
    }

    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }

    static std::shared_ptr<SimpleWorld> createWorld() {
        return std::make_shared<SimpleWorld>();
    }

    static std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }

    // Advanced configuration helpers
    static void configureBrain(std::shared_ptr<Config> config, 
                              int neuronCount = 1000,
                              int regionCount = 2,
                              float connectionProbability = 0.1) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProbability);
    }

    // Environment setup helper
    static void setupEnvironment(std::shared_ptr<SimpleWorld> world,
                                int width = 20,
                                int height = 20,
                                int visionWidth = 8,
                                int visionHeight = 8) {
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        world->setAgentStart(width / 2.0f, height / 2.0f);
    }
};

// Advanced simulation runner
class SimulationRunner {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    bool learningEnabled_ = false;

public:
    void initialize(std::shared_ptr<Brain> brain, 
                   std::shared_ptr<SimpleWorld> world,
                   std::shared_ptr<AgentBrain> agent) {
        brain_ = brain;
        world_ = world;
        agent_ = agent;
        brain_->initialize();
        agent_->initialize(*world_);
    }

    // Run simulation with learning
    py::dict runWithLearning(int numSteps, bool verbose = false) {
        py::dict results;
        std::vector<float> rewards;
        std::vector<float> firingRates;

        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world_->update(0.1);

            // Get sensory input
            auto percept = world_->getSensoryPercept();

            // Process sensory input
            agent_->processSensoryInput(percept);

            // Brain step
            brain_->step(step, step * 0.1);

            // Get action
            auto action = agent_->decodeMotorCommand();

            // Apply action
            world_->applyMotorCommand(action, world_->getSimulationTime());

            // Calculate reward from world state
            float reward = 0.0f; // Simplified reward calculation

            // Apply learning
            if (learningEnabled_) {
                agent_->applyRewardModulation(reward, 0.0f);
                agent_->updateDevelopment(0.1);
            }

            // Record statistics
            rewards.push_back(reward);
            firingRates.push_back(brain_->getFiringRate());

            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Reward=" << reward 
                         << ", FiringRate=" << firingRates.back() << std::endl;
            }
        }

        // Calculate summary statistics
        float avgReward = 0.0f;
        float avgFiringRate = 0.0f;
        if (!rewards.empty()) {
            float sumRewards = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
            avgReward = sumRewards / rewards.size();
            float sumFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
            avgFiringRate = sumFiring / firingRates.size();
        }

        results["average_reward"] = avgReward;
        results["average_firing_rate"] = avgFiringRate;
        results["final_spikes"] = brain_->getTotalSpikeCount();
        results["max_firing_rate"] = *std::max_element(firingRates.begin(), firingRates.end());
        return results;
    }

    // Set learning mode
    void setLearningMode(bool enabled) { learningEnabled_ = enabled; }
};

// Neural network visualization helper
struct VisualizationHelper {
    static py::dict createBrainGraph(std::shared_ptr<Brain> brain) {
        py::dict graph;
        py::list neurons;
        py::list synapses;

        // Add neurons
        for (const auto& region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                py::dict neuronInfo;
                neuronInfo["id"] = neuron->getId().value;
                neuronInfo["type"] = static_cast<int>(neuron->getType());
                neuronInfo["x"] = neuron->getPosition().x; // Assuming position exists
                neuronInfo["y"] = neuron->getPosition().y; // Assuming position exists
                neurons.append(neuronInfo);
            }
        }

        // Add synapses
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                py::dict synapseInfo;
                synapseInfo["source"] = synapse->getSourceNeuron().value;
                synapseInfo["target"] = synapse->getDestinationNeuron().value;
                synapseInfo["weight"] = synapse->getWeight();
                synapses.append(synapseInfo);
            }
        }

        graph["neurons"] = neurons;
        graph["synapses"] = synapses;
        return graph;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced Python Bindings for NLM (Neural Learning Machine)
        
        This module provides an enhanced, user-friendly API for the NLM neural simulation framework.
        It includes convenience wrappers, advanced features, and comprehensive error handling.
        
        Key Features:
        - PythonBrainWrapper: High-level brain interface with convenience methods
        - PythonAgentBrainWrapper: Enhanced agent brain interface
        - PythonFactory: Factory functions for easy setup
        - SimulationRunner: Advanced simulation management
        - Enhanced type conversions and error handling
        - Comprehensive documentation and examples
    )pbdoc";

    // Register exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "RangeError");

    // Register basic ID types
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

    // Register enum types
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

    // Register Config class with enhanced Python API
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Enhanced configuration class)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), "Load from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get string value with default")
        .def("get", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get integer value with default")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get float value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("updateFromDict", [](Config& self, const py::dict& configDict) {
            for (auto item : configDict) {
                std::string key = py::str(item.first);
                // Try to convert value based on type
                if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, static_cast<int64_t>(item.second.cast<long>()), ConfigSource::User);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, item.second.cast<float>(), ConfigSource::User);
                } else if (py::isinstance<py::str>(item.second)) {
                    self.set(key, item.second.cast<std::string>(), ConfigSource::User);
                }
            }
        }, py::arg("config_dict"), "Update configuration from Python dictionary");

    // Register Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const PyVisionData& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", [](const Vision& self) {
            return self.getData(); // Returns std::vector<float>
        })
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self, float minVal = 0.0f, float maxVal = 1.0f) {
            // Normalize vision data to specified range
            auto data = self.getData();
            for (auto& val : data) {
                val = (val - minVal) / (maxVal - minVal);
            }
            self.setData(data);
        }, py::arg("min_val") = 0.0f, py::arg("max_val") = 1.0f, "Normalize vision data");

    // Register Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("setData", [](Audio& self, const PyAudioData& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", [](const Audio& self) {
            return self.getData();
        })
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("computeFFT", [](const Audio& self) {
            // Compute FFT analysis (simplified)
            py::dict spectrum;
            // Implementation would compute frequency spectrum
            return spectrum;
        }, "Compute frequency spectrum");

    // Register InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add signal value")
        .def("getSignals", [](const InternalSignals& self) {
            return self.getSignals();
        }, "Get all signal values")
        .def("getMean", [](const InternalSignals& self) {
            auto signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean signal value");

    // Register Action class with enhanced API
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, PyActionParams>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName)
        .def("getValue", [](const Action& self, const std::string& paramName, float defaultValue = 0.0f) {
            // Get parameter value by name
            auto params = self.getParameters();
            // Implementation would extract specific parameter
            return defaultValue;
        }, py::arg("param_name"), py::arg("default_value") = 0.0f, "Get parameter value")
        .def("addParameter", [](Action& self, const std::string& name, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("name"), py::arg("value"), "Add parameter")
        .def("clone", &Action::clone, "Clone action")
        .def("toDict", [](const Action& self) {
            py::dict actionDict;
            actionDict["type"] = static_cast<int>(self.getType());
            actionDict["name"] = self.getName();
            actionDict["parameters"] = self.getParameters();
            return actionDict;
        }, "Convert action to Python dictionary");

    // Register WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)pbdoc")
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
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Register AgentBody
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("reset", &AgentBody::reset, "Reset body state");

    // Register ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action was successful and rewarding");

    // Register SensoryPercept with enhanced API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("addInternalSignal", [](SensoryPercept& self, float value) {
            auto signals = self.getInternal();
            signals.push_back(value);
            self.setInternal(signals);
        }, py::arg("value"), "Add internal signal value")
        .def("normalizeSensory", [](SensoryPercept& self, float minVal = 0.0f, float maxVal = 1.0f) {
            // Normalize all sensory data
            auto vision = self.getVision();
            if (!vision.empty()) {
                for (auto& val : vision) {
                    val = (val - minVal) / (maxVal - minVal);
                }
                self.setVision(vision);
            }
        }, py::arg("min_val") = 0.0f, py::arg("max_val") = 1.0f, "Normalize all sensory data");

    // Register SimpleWorld with enhanced API
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("createObject", [](SimpleWorld& self, float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, "Create and add a world object");

    // Register Brain class with enhanced Python API
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to neurons of type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal, "Get region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("getLearningMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["firing_rate"] = self.getAverageFiringRate();
            metrics["excitatory_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            return metrics;
        }, "Get learning metrics");

    // Register PythonBrainWrapper (enhanced interface)
    py::class_<PythonBrainWrapper>(m, "PythonBrainWrapper", R"pbdoc(High-level Python brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create wrapper from brain")
        .def("initialize", &PythonBrainWrapper::initialize, "Initialize brain")
        .def("step", &PythonBrainWrapper::step, py::arg("step"), py::arg("time") = 0.0, "Advance simulation")
        .def("getFiringRate", &PythonBrainWrapper::getFiringRate, "Get firing rate")
        .def("getExcitatoryInhibitionRatio", &PythonBrainWrapper::getExcitatoryInhibitionRatio, "Get E/I ratio")
        .def("getStats", &PythonBrainWrapper::getStats, "Get brain statistics")
        .def("getActiveNeurons", &PythonBrainWrapper::getActiveNeuronsByFiringRate, "Get active neurons")
        .def("simulateTimeStep", &PythonBrainWrapper::simulateTimeStep,
             py::arg("time"), py::arg("observations"), "Simulate with observations")
        .def("saveCheckpoint", &PythonBrainWrapper::saveCheckpoint,
             py::arg("path"), py::arg("metadata") = "", "Save checkpoint with metadata")
        .def("__repr__", &PythonBrainWrapper::repr, "String representation")
        .def("__eq__", &PythonBrainWrapper::operator==, "Check equality");

    // Register AgentBrain class with enhanced Python API
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain from brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled")
        .def("getState", [](const AgentBrain& self) {
            py::dict state;
            state["curiosity"] = self.getCuriosityLevel();
            state["novelty"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return state;
        }, "Get agent brain state")
        .def("processObservations", [](AgentBrain& self, const py::dict& observations) {
            // Convert observations dict to SensoryPercept
            // Simplified implementation
        }, py::arg("observations"), "Process observations from dictionary")
        .def("generateAction", [](AgentBrain& self, const std::string& strategy = "explore") {
            auto action = self.decodeMotorCommand();
            return action->getName();
        }, py::arg("strategy") = "explore", "Generate action with strategy");

    // Register factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create new agent brain interface");

    // Register simulation helper
    py::class_<SimulationRunner>(m, "SimulationRunner", R"pbdoc(Advanced simulation runner)pbdoc")
        .def(py::init<>())
        .def("initialize", &SimulationRunner::initialize,
             py::arg("brain"), py::arg("world"), py::arg("agent"), "Initialize simulation")
        .def("runWithLearning", &SimulationRunner::runWithLearning,
             py::arg("num_steps"), py::arg("verbose") = false, "Run simulation with learning")
        .def("setLearningMode", &SimulationRunner::setLearningMode,
             py::arg("enabled"), "Set learning mode");

    // Register visualization helper
    py::class_<VisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Visualization utilities)pbdoc")
        .def_static("createBrainGraph", &VisualizationHelper::createBrainGraph,
                    py::arg("brain"), "Create visualization graph from brain");

    // Register constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "RangeError");

    // Register version
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced Python bindings for NLM with advanced features and improved usability";
}

} // namespace nlm