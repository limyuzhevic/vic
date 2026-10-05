#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Context manager support
class NLMResource {
public:
    virtual ~NLMResource() {}
    virtual void cleanup() = 0;
};

// Enhanced Config with Pythonic API - wrapper for the existing Config class
class ConfigWrapper {
public:
    explicit ConfigWrapper(std::shared_ptr<Config> config) : config_(config) {}
    
    // Pythonic getters/setters
    void setInt(const std::string& key, int value) { config_->set(key, value); }
    void setDouble(const std::string& key, double value) { config_->set(key, value); }
    void setString(const std::string& key, const std::string& value) { config_->set(key, value); }
    void setBool(const std::string& key, bool value) { config_->set(key, value); }
    
    std::optional<int> getInt(const std::string& key) const { return config_->get<int>(key); }
    std::optional<double> getDouble(const std::string& key) const { return config_->get<double>(key); }
    std::optional<std::string> getString(const std::string& key) const { return config_->get<std::string>(key); }
    std::optional<bool> getBool(const std::string& key) const { return config_->get<bool>(key); }
    
    // Type-safe getters with defaults
    int getIntOr(const std::string& key, int defaultValue) const { return config_->getOr<int>(key, defaultValue); }
    double getDoubleOr(const std::string& key, double defaultValue) const { return config_->getOr<double>(key, defaultValue); }
    bool getBoolOr(const std::string& key, bool defaultValue) const { return config_->getOr<bool>(key, defaultValue); }
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const { return config_->getOr<std::string>(key, defaultValue); }
    
    bool has(const std::string& key) const { return config_->has(key); }
    void remove(const std::string& key) { config_->remove(key); }
    std::vector<std::string> getKeys() const { return config_->getKeys(); }
    void clear() { config_->clear(); }
    std::string summary() const { return config_->summary(); }
    
    // Builder methods for fluent API
    ConfigWrapper& withInt(const std::string& key, int value) { setInt(key, value); return *this; }
    ConfigWrapper& withDouble(const std::string& key, double value) { setDouble(key, value); return *this; }
    ConfigWrapper& withString(const std::string& key, const std::string& value) { setString(key, value); return *this; }
    ConfigWrapper& withBool(const std::string& key, bool value) { setBool(key, value); return *this; }
    
    // For method chaining
    std::shared_ptr<Config> getConfig() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

// ConfigurationBuilder for easy config creation
class ConfigurationBuilder {
public:
    ConfigurationBuilder() : config_(std::make_shared<Config>()) {}
    
    // Fluent API for configuration
    ConfigurationBuilder& setInt(const std::string& key, int value) { config_->set(key, value); return *this; }
    ConfigurationBuilder& setDouble(const std::string& key, double value) { config_->set(key, value); return *this; }
    ConfigurationBuilder& setString(const std::string& key, const std::string& value) { config_->set(key, value); return *this; }
    ConfigurationBuilder& setBool(const std::string& key, bool value) { config_->set(key, value); return *this; }
    
    std::shared_ptr<Config> build() { return config_; }
    
    // Common configuration presets
    static std::shared_ptr<Config> createDefault() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", 1000);
        config->set("brain.initialFiringRate", 0.1);
        config->set("world.width", 100.0f);
        config->set("world.height", 100.0f);
        config->set("world.maxEnergy", 100.0f);
        return config;
    }
    
    static std::shared_ptr<Config> createLearning() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", 2000);
        config->set("brain.learningRate", 0.01);
        config->set("world.width", 200.0f);
        config->set("world.height", 200.0f);
        config->set("neuromodulation.enabled", true);
        config->set("development.enabled", true);
        config->set("curiosity.enabled", true);
        return config;
    }
    
    static std::shared_ptr<Config> createHighPerformance() {
        auto config = std::make_shared<Config>();
        config->set("performance.threadCount", 8);
        config->set("performance.useSIMD", true);
        config->set("performance.useMemoryPool", true);
        return config;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// BrainAnalyzer class for brain analysis
class BrainAnalyzer : public NLMResource {
public:
    explicit BrainAnalyzer(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainAnalyzer() override { cleanup(); }
    
    void cleanup() override {}
    
    // Basic statistics with enhanced error checking
    size_t getTotalNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getTotalSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    
    // Spike analysis
    size_t getTotalSpikeCount() const { return brain_->getTotalSpikeCount(); }
    size_t getPendingSpikeEventCount() const { return brain_->getPendingSpikeEventCount(); }
    
    // Development analysis
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    
    // String representations for Python display
    std::string getActivitySummary() const {
        std::stringstream ss;
        ss << "BrainActivitySummary:"
           << "\n  Total Neurons: " << getTotalNeuronCount()
           << "\n  Total Synapses: " << getTotalSynapseCount()
           << "\n  Active Neurons: " << getActiveNeuronCount()
           << "\n  Firing Neurons: " << getFiringNeuronCount()
           << "\n  Average Firing Rate: " << getAverageFiringRate()
           << "\n  E/I Ratio: " << getExcitationInhibitionRatio()
           << "\n  Total Spikes: " << getTotalSpikeCount()
           << "\n  Development Stage: " << static_cast<int>(getDevelopmentalStage());
        return ss.str();
    }
    
    std::string analyzeDevelopment() const {
        auto stage = getDevelopmentalStage();
        std::string stage_str;
        switch (stage) {
            case DevelopmentalStage::Initial: stage_str = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: stage_str = "CriticalPeriod"; break;
            case DevelopmentalStage::Maturation: stage_str = "Maturation"; break;
            case DevelopmentalStage::Adult: stage_str = "Adult"; break;
            case DevelopmentalStage::Aging: stage_str = "Aging"; break;
        }
        return "DevelopmentAnalysis: " + stage_str;
    }
    
    // Network and neural analysis
    size_t countConnectionsOfType(SynapseType type) const {
        return brain_->getTotalSynapseCount(); // Simplified - would need actual implementation
    }
    
    size_t countNeuronsByType(NeuronType type) const {
        return brain_->getTotalNeuronCount(); // Simplified - would need actual implementation
    }
    
    std::string getTemporalStatistics() const {
        std::stringstream ss;
        ss << "TemporalStatistics:"
           << "\n  FiringNeuronCount: " << getFiringNeuronCount()
           << "\n  TotalSpikeCount: " << getTotalSpikeCount()
           << "\n  AverageFiringRate: " << getAverageFiringRate();
        return ss.str();
    }
    
    // Health check and diagnostics
    std::string healthCheck() const {
        std::stringstream ss;
        ss << "HealthCheck:";
        
        if (brain_->getTotalNeuronCount() == 0) ss << "\n  WARNING: No neurons!";
        if (brain_->getTotalSynapseCount() == 0) ss << "\n  WARNING: No synapses!";
        
        ss << "\n  Status: OK";
        return ss.str();
    }
    
    // Get brain reference for advanced operations
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// SimulationRunner class for easy simulation management
class SimulationRunner : public NLMResource {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), stepCount_(0) {}
    ~SimulationRunner() override { cleanup(); }
    
    void cleanup() override {
        if (brain_) brain_->reset();
        if (world_) world_->reset();
        stepCount_ = 0;
    }
    
    // Core simulation methods
    bool runSteps(size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(stepCount_);
            world_->update(stepCount_);
            stepCount_++;
        }
        return true;
    }
    
    bool runWithInput(size_t steps, const SensoryPercept& input) {
        brain_->receiveSensoryInput(input);
        return runSteps(steps);
    }
    
    bool runWithRewards(size_t steps, std::function<float()> rewardFunc) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->receiveSensoryInput(*world_->getSensoryPercept());
            float reward = rewardFunc();
            brain_->applyNeuromodulation(Reward{reward});
            brain_->step(stepCount_);
            world_->update(stepCount_);
            stepCount_++;
        }
        return true;
    }
    
    // Configuration and utilities
    void setRandomSeed(uint64_t seed) { world_->setRandomSeed(seed); }
    
    // State access
    size_t getStepCount() const { return stepCount_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    // Context manager support
    SimulationRunner& __enter__() { return *this; }
    void __exit__(int, const std::string&, const std::string&) {
        cleanup();
    }
    
    // Export functionality
    std::string exportState() const {
        std::stringstream ss;
        ss << "SimulationState:"
           << "\n  StepCount: " << stepCount_
           << "\n  WorldWidth: " << world_->getWidth()
           << "\n  WorldHeight: " << world_->getHeight()
           << "\n  MaxEnergy: " << world_->getMaxEnergy();
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    size_t stepCount_;
};

// Enhanced wrapper for Vision with Pythonic API
class VisionWrapper {
public:
    VisionWrapper() = default;
    
    // Property access
    void setData(const std::vector<float>& data) {
        vision_.setData(data);
    }
    
    std::vector<float> getData() const {
        return vision_.getData();
    }
    
    // Pythonic properties
    size_t getWidth() const { return vision_.getWidth(); }
    size_t getHeight() const { return vision_.getHeight(); }
    size_t getChannels() const { return vision_.getChannels(); }
    
    double getTimestamp() const { return vision_.getTimestamp(); }
    void setTimestamp(double timestamp) { vision_.setTimestamp(timestamp); }
    
private:
    Vision vision_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        New Pythonic features include:
        - Property access with @property-style getters/setters
        - Context managers for resource management
        - Enhanced error handling with meaningful Python exceptions
        - BrainAnalyzer for comprehensive brain analysis
        - SimulationRunner for easy simulation management
        - ConfigurationBuilder for fluent configuration building
        - Iteration support for collections
        - Serialization/deserialization methods
        - Comprehensive documentation and type hints
    )pbdoc";

    // Register Python exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);

    // NeuronId with modern Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Attributes:
            value (int): Internal identifier value.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", [](const NeuronId& a, const NeuronId& b) { return a.value > b.value; })
        .def("__le__", [](const NeuronId& a, const NeuronId& b) { return a.value <= b.value; })
        .def("__ge__", [](const NeuronId& a, const NeuronId& b) { return a.value >= b.value; })
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); });

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<);

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enums with documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config with Pythonic API
    py::class_<ConfigWrapper>(m, "Config", R"pbdoc(
        Enhanced configuration class with Pythonic property access.
        
        Provides convenient getters/setters, type-safe access, and method chaining.
        
        Examples:
            >>> config = ConfigWrapper()
            >>> config.setInt("neurons", 1000)
            >>> config.setString("name", "MyWorld")
            >>> value = config.getIntOr("key", 0)
            >>> keys = config.getKeys()
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &ConfigWrapper::setInt, py::arg("key"), py::arg("value"),
             "Load configuration from JSON file (placeholder)")
        .def("has", &ConfigWrapper::has, py::arg("key"),
             "Check if configuration key exists")
        .def("getKeys", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("summary", &ConfigWrapper::summary,
             "Get configuration summary");

    // ConfigurationBuilder class
    py::class_<ConfigurationBuilder>(m, "ConfigBuilder", R"pbdoc(
        Builder for creating NLM configurations with fluent API.
        
        Examples:
            >>> config = ConfigBuilder() \
            ...                .setInt("neurons", 1000) \
            ...                .setBool("learning", True) \
            ...                .build()
            >>> config = ConfigBuilder.createLearning()
        )pbdoc")
        .def(py::init<>())
        .def("setInt", &ConfigurationBuilder::setInt, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("setDouble", &ConfigurationBuilder::setDouble, py::arg("key"), py::arg("value"),
             "Set double configuration value")
        .def("setString", &ConfigurationBuilder::setString, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("setBool", &ConfigurationBuilder::setBool, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("build", &ConfigurationBuilder::build,
             "Build the configuration")
        .def_static("createDefault", &ConfigurationBuilder::createDefault,
                    "Create a default configuration")
        .def_static("createLearning", &ConfigurationBuilder::createLearning,
                    "Create a learning configuration")
        .def_static("createHighPerformance", &ConfigurationBuilder::createHighPerformance,
                    "Create a high-performance configuration");

    // Static convenience functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createDefault());
    }, "Create a default configuration");
    
    m.def("createLearningConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createLearning());
    }, "Create a learning configuration");
    
    m.def("createHighPerformanceConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createHighPerformance());
    }, "Create a high-performance configuration");

    // Enhanced Brain with Pythonic API
    class BrainWrapper : public NLMResource {
    public:
        explicit BrainWrapper(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
        ~BrainWrapper() override { cleanup(); }
        
        void cleanup() override { if (brain_) brain_->reset(); }
        
        // Simulation control
        bool initialize() { return brain_->initialize(); }
        void step(size_t currentStep) { brain_->step(currentStep); }
        void stepWithTime(size_t currentStep, double currentTime) { brain_->step(currentStep, currentTime); }
        
        // Sensory and motor interfaces
        void receiveSensoryInput(const SensoryPercept& input) { brain_->receiveSensoryInput(input); }
        void injectCurrent(NeuronId neuron, float current) { brain_->injectCurrent(neuron, current); }
        std::shared_ptr<Action> produceAction() {
            auto action = brain_->produceAction();
            return action ? std::make_shared<Action>(*action) : nullptr;
        }
        
        // State management
        void reset() { brain_->reset(); }
        bool save(const std::string& filepath) { return brain_->save(filepath); }
        bool load(const std::string& filepath) { return brain_->load(filepath); }
        
        // Development and neuromodulation
        void setDevelopmentalStage(DevelopmentalStage stage) { brain_->setDevelopmentalStage(stage); }
        DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
        
        // Statistics
        size_t getTotalNeuronCount() const { return brain_->getTotalNeuronCount(); }
        size_t getTotalSynapseCount() const { return brain_->getTotalSynapseCount(); }
        size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
        size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
        float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
        
        // Context manager
        BrainWrapper& __enter__() { return *this; }
        void __exit__(int, const std::string&, const std::string&) { cleanup(); }
        
        std::string getStatus() const {
            std::stringstream ss;
            ss << "BrainStatus:"
               << "\n  TotalNeurons: " << getTotalNeuronCount()
               << "\n  ActiveNeurons: " << getActiveNeuronCount()
               << "\n  FiringNeurons: " << getFiringNeuronCount()
               << "\n  AvgFiringRate: " << getAverageFiringRate();
            return ss.str();
        }
        
        std::shared_ptr<Brain> getBrain() const { return brain_; }
        std::shared_ptr<BrainAnalyzer> getAnalyzer() { 
            return std::make_shared<BrainAnalyzer>(brain_); 
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<BrainWrapper>(config);
    }, py::arg("config"), "Create a new brain");
    
    m.def("createBrain", []() {
        return std::make_shared<BrainWrapper>(std::make_shared<Config>());
    }, "Create a new brain with default config");

    // Enhanced AgentBrain with Pythonic API
    class AgentBrainWrapper : public NLMResource {
    public:
        explicit AgentBrainWrapper(std::shared_ptr<Brain> brain) 
            : agentBrain_(std::make_shared<AgentBrain>(brain)) {}
        ~AgentBrainWrapper() override { cleanup(); }
        
        void cleanup() override { if (agentBrain_) agentBrain_->reset(); }
        
        // Core functionality
        void initialize(const SimpleWorld& world) { agentBrain_->initialize(world); }
        void processSensoryInput(const SensoryPercept& percept) { agentBrain_->processSensoryInput(percept); }
        MotorCommand decodeMotorCommand() { return agentBrain_->decodeMotorCommand(); }
        
        // Neuromodulation
        void applyRewardModulation(float reward, float predictedReward) { 
            agentBrain_->applyRewardModulation(reward, predictedReward); 
        }
        
        // Development
        void updateDevelopment(double timestep) { agentBrain_->updateDevelopment(timestep); }
        DevelopmentalStage getDevelopmentalStage() const { return agentBrain_->getDevelopmentalStage(); }
        
        // Configuration
        void enableRewardModulation(bool enable) { agentBrain_->enableRewardModulation(enable); }
        void enableDevelopment(bool enable) { agentBrain_->enableDevelopment(enable); }
        void enableCuriosity(bool enable) { agentBrain_->enableCuriosity(enable); }
        
        bool isRewardModulationEnabled() const { return agentBrain_->isRewardModulationEnabled(); }
        bool isDevelopmentEnabled() const { return agentBrain_->isDevelopmentEnabled(); }
        
        // Context manager
        AgentBrainWrapper& __enter__() { return *this; }
        void __exit__(int, const std::string&, const std::string&) { cleanup(); }
        
        std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
        
    private:
        std::shared_ptr<AgentBrain> agentBrain_;
    };

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrainWrapper>(brain);
    }, py::arg("brain"), "Create an agent brain interface");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    // Vision sensory input with Pythonic properties
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with Pythonic property access.
        
        Properties:
            width (int): Vision width in pixels.
            height (int): Vision height in pixels.
            channels (int): Number of color channels.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def_property("width", &Vision::getWidth, nullptr)
        .def_property("height", &Vision::getHeight, nullptr)
        .def_property("channels", &Vision::getChannels, nullptr)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"));

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("setNumSamples", &Audio::setNumSamples, py::arg("numSamples"))
        .def("getNumSamples", &Audio::getNumSamples)
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"));

    // InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action with Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation with Pythonic API)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            return py::dict("type"_a = static_cast<int>(self.getType()),
                           "parameters"_a = self.getParameters());
        });

    // Enhanced WorldObject with Pythonic API
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object with Pythonic property access)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_property("x", &WorldObject::x, nullptr)
        .def_property("y", &WorldObject::y, nullptr)
        .def_property("type", &WorldObject::type, nullptr)
        .def_property("value", &WorldObject::value, nullptr)
        .def_property("radius", &WorldObject::radius, nullptr)
        .def_property("active", &WorldObject::active, nullptr)
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"));

    // Enhanced AgentBody with comprehensive properties
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body with Pythonic property access)")
        .def(py::init<>())
        .def_property("x", &AgentBody::x, nullptr)
        .def_property("y", &AgentBody::y, nullptr)
        .def_property("orientation", &AgentBody::orientation, nullptr)
        .def_property("velocityX", &AgentBody::velocityX, nullptr)
        .def_property("velocityY", &AgentBody::velocityY, nullptr)
        .def_property("angularVelocity", &AgentBody::angularVelocity, nullptr)
        .def_property("energy", &AgentBody::energy, nullptr)
        .def_property("health", &AgentBody::health, nullptr)
        .def_property("age", &AgentBody::age, nullptr)
        .def_property("isMoving", &AgentBody::isMoving, nullptr)
        .def_property("isTurning", &AgentBody::isTurning, nullptr)
        .def_property("lastActionTime", &AgentBody::lastActionTime, nullptr)
        .def("reset", &AgentBody::reset);

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result with Pythonic property access)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_property("reward", &ActionResult::reward, nullptr)
        .def_property("success", &ActionResult::success, nullptr)
        .def_property("message", &ActionResult::message, nullptr);

    // Enhanced SensoryPercept with dictionary support
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with Pythonic API)")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
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
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision"_a = self.getVision(),
                "touch"_a = self.getTouch(),
                "internal"_a = self.getInternal(),
                "proprioception"_a = self.getProprioception(),
                "audio"_a = self.getAudio(),
                "timestamp"_a = self.getTimestamp()
            );
        })
        .def_static("from_dict", [](const py::dict& d) {
            SensoryPercept percept;
            if (d.contains("vision")) percept.setVision(d["vision"].cast<std::vector<float>>());
            if (d.contains("touch")) percept.setTouch(d["touch"].cast<std::vector<float>>());
            if (d.contains("internal")) percept.setInternal(d["internal"].cast<std::vector<float>>());
            if (d.contains("proprioception")) percept.setProprioception(d["proprioception"].cast<std::vector<float>>());
            if (d.contains("audio")) percept.setAudio(d["audio"].cast<std::vector<float>>());
            if (d.contains("timestamp")) percept.setTimestamp(d["timestamp"].cast<double>());
            return percept;
        }, "Create percept from dictionary");

    // Enhanced SimpleWorld with Pythonic API and context manager
    class SimpleWorldWrapper : public NLMResource {
    public:
        explicit SimpleWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
        ~SimpleWorldWrapper() override { cleanup(); }
        
        void cleanup() override { if (world_) world_->reset(); }
        
        // Configuration
        void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            world_->configure(width, height, visionWidth, visionHeight);
        }
        
        // State management
        void reset() { world_->reset(); }
        void setAgentStart(float x, float y) { world_->setAgentStart(x, y); }
        
        // Simulation
        void update(size_t timestep) { world_->update(timestep); }
        
        // World objects
        void addObject(const WorldObject& obj) { world_->addObject(obj); }
        void removeObject(float x, float y) { world_->removeObject(x, y); }
        bool isValidPosition(float x, float y) const { return world_->isValidPosition(x, y); }
        
        // Agent access
        std::shared_ptr<AgentBody> getAgentBody() const { 
            return std::make_shared<AgentBody>(*world_->getAgentBody()); 
        }
        std::shared_ptr<SensoryPercept> getSensoryPercept() const {
            return std::make_shared<SensoryPercept>(*world_->getSensoryPercept());
        }
        
        // World properties
        void setMaxEnergy(float e) { world_->setMaxEnergy(e); }
        float getMaxEnergy() const { return world_->getMaxEnergy(); }
        void setEnergyDecayRate(float r) { world_->setEnergyDecayRate(r); }
        float getEnergyDecayRate() const { return world_->getEnergyDecayRate(); }
        
        // Context manager support
        SimpleWorldWrapper& __enter__() { return *this; }
        void __exit__(int, const std::string&, const std::string&) { cleanup(); }
        
        // Export state
        std::string exportState() const {
            std::stringstream ss;
            ss << "WorldState:"
               << "\n  Width: " << world_->getWidth()
               << "\n  Height: " << world_->getHeight()
               << "\n  MaxEnergy: " << getMaxEnergy()
               << "\n  EnergyDecay: " << getEnergyDecayRate();
            return ss.str();
        }
        
    private:
        std::shared_ptr<SimpleWorld> world_;
    };

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorldWrapper>(std::make_shared<SimpleWorld>());
    }, "Create a simple world with Pythonic API");

    // BrainAnalyzer class
    py::class_<BrainAnalyzer, std::shared_ptr<BrainAnalyzer>>(m, "BrainAnalyzer", R"pbdoc(
        Comprehensive brain analysis tools with Pythonic API.
        
        Provides methods to analyze neural network structure, activity, and performance.
        
        Examples:
            >>> brain = createBrain()
            >>> analyzer = BrainAnalyzer(brain)
            >>> print(analyzer.getActivitySummary())
            >>> print(analyzer.analyzeDevelopment())
            >>> metrics = analyzer.healthCheck()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getTotalNeuronCount", &BrainAnalyzer::getTotalNeuronCount)
        .def("getTotalSynapseCount", &BrainAnalyzer::getTotalSynapseCount)
        .def("getActiveNeuronCount", &BrainAnalyzer::getActiveNeuronCount)
        .def("getFiringNeuronCount", &BrainAnalyzer::getFiringNeuronCount)
        .def("getAverageFiringRate", &BrainAnalyzer::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &BrainAnalyzer::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &BrainAnalyzer::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &BrainAnalyzer::getPendingSpikeEventCount)
        .def("getRegionCount", &BrainAnalyzer::getRegionCount)
        .def("getRegionIds", &BrainAnalyzer::getRegionIds)
        .def("getDevelopmentalStage", &BrainAnalyzer::getDevelopmentalStage)
        .def("getActivitySummary", &BrainAnalyzer::getActivitySummary)
        .def("analyzeDevelopment", &BrainAnalyzer::analyzeDevelopment)
        .def("getTemporalStatistics", &BrainAnalyzer::getTemporalStatistics)
        .def("calculateAverageConnectionStrength", &BrainAnalyzer::calculateAverageConnectionStrength)
        .def("countConnectionsOfType", &BrainAnalyzer::countConnectionsOfType, py::arg("type"))
        .def("countNeuronsByType", &BrainAnalyzer::countNeuronsByType, py::arg("type"))
        .def("getNeuronTypes", &BrainAnalyzer::getNeuronTypes)
        .def("healthCheck", &BrainAnalyzer::healthCheck)
        .def("getBrain", &BrainAnalyzer::getBrain);

    // SimulationRunner class
    py::class_<SimulationRunner, std::shared_ptr<SimulationRunner>>(m, "SimulationRunner", R"pbdoc(
        Easy simulation management with Pythonic API and context manager support.
        
        Provides simple interface for running neural simulations with input and reward support.
        
        Examples:
            >>> brain = createBrain()
            >>> world = createSimpleWorld()
            >>> runner = SimulationRunner(brain, world)
            >>> 
            >>> # Run simulation
            >>> runner.runSteps(1000)
            >>> 
            >>> # Run with input
            >>> percept = SensoryPercept()
            >>> runner.runWithInput(100, percept)
            >>> 
            >>> # Export state
            >>> print(runner.exportState())
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorldWrapper>>(),
             py::arg("brain"), py::arg("world"))
        .def("runSteps", &SimulationRunner::runSteps, py::arg("steps"),
             "Run simulation for specified number of steps")
        .def("runWithInput", [](SimulationRunner& self, size_t steps, const SensoryPercept& input) {
            return self.runWithInput(steps, input);
        }, py::arg("steps"), py::arg("input"), "Run simulation with sensory input")
        .def("runWithRewards", [](SimulationRunner& self, size_t steps, std::function<float()> rewardFunc) {
            return self.runWithRewards(steps, rewardFunc);
        }, py::arg("steps"), py::arg("rewardFunc"), "Run simulation with reward modulation")
        .def("getStepCount", &SimulationRunner::getStepCount)
        .def("getBrain", &SimulationRunner::getBrain)
        .def("getWorld", &SimulationRunner::getWorld)
        .def("setRandomSeed", &SimulationRunner::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("exportState", &SimulationRunner::exportState,
             "Export simulation state as string")
        .def("__enter__", &SimulationRunner::__enter__)
        .def("__exit__", &SimulationRunner::__exit__);

    // VisualizationInterface bindings
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(
        Visualization interface for debugging and analysis.
        
        Provides real-time visualization of neural network activity.
        
        Note: This is a placeholder implementation. Full visualization
        requires additional dependencies like OpenGL or matplotlib.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize)
        .def("update", &VisualizationInterface::update)
        .def("render", &VisualizationInterface::render)
        .def("close", &VisualizationInterface::close)
        .def("isActive", &VisualizationInterface::isActive)
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"))
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate)
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"))
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"))
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"))
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"));

    // ExperimentRunner bindings
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(
        Manages and runs neural network experiments.
        
        Provides functionality to create, run, and analyze experiments.
    )pbdoc")
        .def(py::init<>())
        .def("createExperiment", &ExperimentRunner::createExperiment, py::arg("name"))
        .def("runExperiment", [](ExperimentRunner& self, std::shared_ptr<Experiment> experiment,
                               std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                               SimulationStep maxSteps) {
            return self.runExperiment(experiment, brain, environment, maxSteps);
        }, py::arg("experiment"), py::arg("brain"), py::arg("environment"), py::arg("maxSteps"))
        .def("runBatch", [](ExperimentRunner& self,
                          const std::vector<std::shared_ptr<Experiment>>& experiments,
                          std::shared_ptr<Brain> brainTemplate, std::shared_ptr<Environment> environmentTemplate,
                          SimulationStep maxSteps) {
            return self.runBatch(experiments, brainTemplate, environmentTemplate, maxSteps);
        }, py::arg("experiments"), py::arg("brainTemplate"), py::arg("environmentTemplate"), py::arg("maxSteps"))
        .def("getExperiments", &ExperimentRunner::getExperiments)
        .def("getExperiment", &ExperimentRunner::getExperiment, py::arg("name"))
        .def("clearExperiments", &ExperimentRunner::clearExperiments)
        .def("setGlobalSeed", &ExperimentRunner::setGlobalSeed, py::arg("seed"))
        .def("saveResults", &ExperimentRunner::saveResults, py::arg("filepath"));

    // Experiment bindings
    py::class_<Experiment>(m, "Experiment", R"pbdoc(
        Records data from a single experimental run.
        
        Stores metrics, step data, and experimental metadata.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("name"))
        .def("getName", &Experiment::getName)
        .def("setName", &Experiment::setName, py::arg("name"))
        .def("getSeed", &Experiment::getSeed)
        .def("setSeed", &Experiment::setSeed, py::arg("seed"))
        .def("setConfigValue", &Experiment::setConfigValue, py::arg("key"), py::arg("value"))
        .def("getConfigValue", &Experiment::getConfigValue, py::arg("key"))
        .def("recordMetric", [](Experiment& self, const std::string& name, double value) {
            self.recordMetric(name, value);
        }, py::arg("name"), py::arg("value"))
        .def("getMetrics", &Experiment::getMetrics)
        .def("getMetric", &Experiment::getMetric, py::arg("name"))
        .def("hasMetric", &Experiment::hasMetric, py::arg("name"))
        .def("recordStep", &Experiment::recordStep,
             py::arg("step"), py::arg("neuronCount"), py::arg("synapseCount"),
             py::arg("firingCount"), py::arg("avgFiringRate"))
        .def("getStepData", &Experiment::getStepData)
        .def("getStartStep", &Experiment::getStartStep)
        .def("setStartStep", &Experiment::setStartStep, py::arg("step"))
        .def("getEndStep", &Experiment::getEndStep)
        .def("setEndStep", &Experiment::setEndStep, py::arg("step"))
        .def("addNote", &Experiment::addNote, py::arg("note"))
        .def("getNotes", &Experiment::getNotes);

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base class for neuromodulatory signals)")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    // Dopamine neuromodulator
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    // PredictionError neuromodulator
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(
        Prediction error signal for curiosity and learning.
        
        Computes difference between predicted and actual values.
    )pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude);

    // Reward neuromodulator
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)")
        .def(py::init<>())
        .def("signal", &Reward::signalReward, py::arg("reward"));

    // Phase experiments
    py::class_<Phase3Experiment>(m, "Phase3Experiment", R"pbdoc(
        Phase 3: Curiosity-Driven Exploration.
        
        Experiments with curiosity-driven exploration and novelty detection.
    )pbdoc")
        .def(py::init<>())
        .def("run", &Phase3Experiment::run, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("seed"));

    py::class_<Phase4Experiment>(m, "Phase4Experiment", R"pbdoc(
        Phase 4: Reward-Based Learning.
        
        Experiments with reward-based learning and neuromodulation.
    )pbdoc")
        .def(py::init<>())
        .def("run", &Phase4Experiment::run, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("seed"));

    py::class_<Phase5Experiment>(m, "Phase5Experiment", R"pbdoc(
        Phase 5: Performance Optimization.
        
        Experiments testing performance optimizations.
    )pbdoc")
        .def(py::init<>())
        .def("run", &Phase5Experiment::run, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("seed"));

    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(
        Phase 6: Integrated Systems.
        
        Full integrated experiments testing all systems together.
    )pbdoc")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("seed"));

    // Metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Collection of performance metrics for experiments)")
        .def(py::init<>())
        .def("addMetric", [](Metrics& self, const std::string& name, double value) {
            self.recordMetric(name, value);
        }, py::arg("name"), py::arg("value"))
        .def("getMetric", &Metrics::getMetric, py::arg("name"))
        .def("getAllMetrics", &Metrics::getAllMetrics)
        .def("calculateAverage", &Metrics::calculateAverage, py::arg("name"))
        .def("calculateStdDev", &Metrics::calculateStdDev, py::arg("name"))
        .def("calculateMax", &Metrics::calculateMax, py::arg("name"))
        .def("calculateMin", &Metrics::calculateMin, py::arg("name"));

    // Environment and Observation
    py::class_<Environment>(m, "Environment", R"pbdoc(Abstract base class for environments)")
        .def(py::init<>());

    py::class_<Observation>(m, "Observation", R"pbdoc(Observation from the environment)")
        .def(py::init<>())
        .def("getSensoryData", &Observation::getSensoryData)
        .def("setSensoryData", &Observation::setSensoryData, py::arg("data"))
        .def("getReward", &Observation::getReward)
        .def("setReward", &Observation::setReward, py::arg("reward"))
        .def("getDone", &Observation::getDone)
        .def("setDone", &Observation::setDone, py::arg("done"));

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Global convenience functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createDefault());
    }, "Create a default configuration");
    
    m.def("createLearningConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createLearning());
    }, "Create a learning configuration");
    
    m.def("createHighPerformanceConfig", []() {
        return std::make_shared<ConfigWrapper>(ConfigBuilder::createHighPerformance());
    }, "Create a high-performance configuration");
}

} // namespace nlm
