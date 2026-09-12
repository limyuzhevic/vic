#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <future>

// NumPy support
#include <pybind11/numpy.h>

// Include project headers
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

// Custom exception types for different error scenarios
class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error("ConfigurationError: " + msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error("SimulationError: " + msg) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& msg) : std::runtime_error("MemoryError: " + msg) {}
};

class ProgressError : public std::runtime_error {
public:
    explicit ProgressError(const std::string& msg) : std::runtime_error("ProgressError: " + msg) {}
};

class SerializationError : public std::runtime_error {
public:
    explicit SerializationError(const std::string& msg) : std::runtime_error("SerializationError: " + msg) {}
};

class FactoryError : public std::runtime_error {
public:
    explicit FactoryError(const std::string& msg) : std::runtime_error("FactoryError: " + msg) {}
};

// Context manager base class for automatic cleanup
class ContextManagerBase {
protected:
    bool initialized_ = false;
    bool cleanup_on_destruction_ = true;

public:
    ContextManagerBase() = default;
    virtual ~ContextManagerBase() = default;

    bool initialized() const { return initialized_; }
    void setInitialized(bool val) { initialized_ = val; }
    bool cleanupOnDestruction() const { return cleanup_on_destruction_; }
    void setCleanupOnDestruction(bool val) { cleanup_on_destruction_ = val; }

    virtual void cleanup() = 0;
    virtual std::string getStatus() const = 0;
};

// Brain simulation context manager with property access
class BrainContextManager : public ContextManagerBase {
private:
    std::shared_ptr<Brain> brain_;
    std::function<void()> on_cleanup_;
    py::object cleanup_callback_;

public:
    explicit BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        setInitialized(true);
    }

    ~BrainContextManager() override {
        if (cleanupOnDestruction() && initialized_) {
            try {
                cleanup();
            } catch (const std::exception& e) {
                // Log but don't propagate cleanup errors
            }
        }
    }

    void cleanup() override {
        if (brain_ && cleanupOnDestruction()) {
            brain_->reset();
            if (cleanup_callback_.is(py::handle())) {
                try {
                    cleanup_callback_();
                } catch (...) {}
            }
            if (on_cleanup_) {
                on_cleanup_();
            }
        }
        setInitialized(false);
    }

    std::string getStatus() const override {
        if (!brain_) return "Cleaned up";
        return "Active - " + brain_->logStatus();
    }

    void setCleanupCallback(py::object callback) {
        cleanup_callback_ = callback;
    }

    void setOnCleanup(std::function<void()> callback) {
        on_cleanup_ = std::move(callback);
    }

    std::shared_ptr<Brain> brain() const { return brain_; }

    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }

    void step(SimulationStep currentStep, const Timestamp& currentTime = Timestamp()) {
        if (brain_) {
            brain_->step(currentStep, currentTime);
        }
    }

    // Python property access methods
    double getAverageFiringRate() const {
        return brain_ ? brain_->getAverageFiringRate() : 0.0;
    }

    size_t getTotalNeuronCount() const {
        return brain_ ? brain_->getTotalNeuronCount() : 0;
    }

    size_t getTotalSynapseCount() const {
        return brain_ ? brain_->getTotalSynapseCount() : 0;
    }

    size_t getActiveNeuronCount() const {
        return brain_ ? brain_->getActiveNeuronCount() : 0;
    }

    size_t getFiringNeuronCount() const {
        return brain_ ? brain_->getFiringNeuronCount() : 0;
    }

    double getExcitationInhibitionRatio() const {
        return brain_ ? brain_->getExcitationInhibitionRatio() : 0.0;
    }

    size_t getTotalSpikeCount() const {
        return brain_ ? brain_->getTotalSpikeCount() : 0;
    }

    std::vector<NeuronId> getRegionIds() const {
        return brain_ ? brain_->getRegionIds() : std::vector<NeuronId>{};
    }

    size_t getRegionCount() const {
        return brain_ ? brain_->getRegionCount() : 0;
    }

    py::object getRegion(RegionId id) {
        if (brain_) {
            auto region = brain_->getRegion(id);
            return py::cast(region);
        }
        return py::none();
    }

    // Additional property access
    DevelopmentStage getDevelopmentalStage() const {
        return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Initial;
    }

    float getAverageFiringRate() const {
        return brain_ ? brain_->getAverageFiringRate() : 0.0f;
    }

    size_t getTotalSpikeCount() const {
        return brain_ ? brain_->getTotalSpikeCount() : 0;
    }

    std::shared_ptr<const Config> getConfig() const {
        return brain_ ? brain_->getConfig() : nullptr;
    }

    // Working memory statistics
    size_t getWorkingMemoryCapacity() const {
        if (!brain_) return 0;
        auto* wm = brain_->getWorkingMemory();
        return wm ? wm->getCapacity() : 0;
    }

    size_t getWorkingMemoryUsage() const {
        if (!brain_) return 0;
        auto* wm = brain_->getWorkingMemory();
        return wm ? wm->getUsage() : 0;
    }

    // Neuromodulation levels
    float getNeuromodulationLevel() const {
        if (!brain_) return 0.0f;
        auto* dopamine = brain_->getDopamine();
        return dopamine ? dopamine->getLevel() : 0.0f;
    }

    float getCuriosityLevel() const {
        if (!brain_) return 0.0f;
        auto* curiosity = brain_->getCuriosity();
        return curiosity ? curiosity->getLevel() : 0.0f;
    }

    float getNoveltyLevel() const {
        if (!brain_) return 0.0f;
        auto* novelty = brain_->getNovelty();
        return novelty ? novelty->getLevel() : 0.0f;
    }

    float getPredictionError() const {
        if (!brain_) return 0.0f;
        auto* pred_error = brain_->getPredictionErrorSignal();
        return pred_error ? pred_error->getError() : 0.0f;
    }

    // Performance statistics
    double getMemoryUsage() const {
        if (!brain_) return 0.0;
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            return static_cast<double>(wm->getUsage()) / wm->getCapacity();
        }
        return 0.0;
    }

    size_t getEpisodicMemoryCount() const {
        if (!brain_) return 0;
        auto* em = brain_->getEpisodicMemory();
        return em ? em->getEpisodeCount() : 0;
    }
};

// Configuration context manager
class ConfigContextManager : public ContextManagerBase {
private:
    std::shared_ptr<Config> config_;

public:
    explicit ConfigContextManager(std::shared_ptr<Config> config) : config_(config) {
        setInitialized(true);
    }

    ~ConfigContextManager() override {
        if (cleanupOnDestruction() && initialized_) {
            cleanup();
        }
    }

    void cleanup() override {
        if (config_) {
            config_->clear();
        }
        setInitialized(false);
    }

    std::string getStatus() const override {
        if (!config_) return "Cleaned up";
        return "Active - " + config_->summary();
    }

    std::shared_ptr<Config> config() const { return config_; }

    void loadFromFile(const std::string& filepath) {
        if (config_) {
            config_->loadFromFile(filepath);
        }
    }

    void saveToFile(const std::string& filepath) {
        if (config_) {
            config_->saveToFile(filepath);
        }
    }

    bool has(const std::string& key) const {
        return config_ ? config_->has(key) : false;
    }

    std::string getValue(const std::string& key) const {
        return config_ ? config_->get(key) : "";
    }

    void setValue(const std::string& key, const std::string& value) {
        if (config_) {
            config_->set(key, value);
        }
    }

    std::vector<std::string> getKeys() const {
        return config_ ? config_->getKeys() : std::vector<std::string>{};
    }

    void clear() {
        if (config_) {
            config_->clear();
        }
    }

    std::string summary() const {
        return config_ ? config_->summary() : "";
    }

    // Additional configuration properties
    std::string getString(const std::string& key) const {
        return getValue(key);
    }

    int getInt(const std::string& key) const {
        std::string val = getValue(key);
        try {
            return std::stoi(val);
        } catch (...) {
            return 0;
        }
    }

    double getDouble(const std::string& key) const {
        std::string val = getValue(key);
        try {
            return std::stod(val);
        } catch (...) {
            return 0.0;
        }
    }

    bool getBool(const std::string& key) const {
        std::string val = getValue(key);
        return val == "true" || val == "1" || val == "yes";
    }

    std::vector<std::string> getStringVector(const std::string& key) const {
        std::string val = getValue(key);
        // Simple parsing - would need proper JSON handling in real implementation
        return {val};
    }
};

// World simulation context manager
class WorldContextManager : public ContextManagerBase {
private:
    std::shared_ptr<SimpleWorld> world_;
    std::function<void()> on_cleanup_;

public:
    explicit WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {
        setInitialized(true);
    }

    ~WorldContextManager() override {
        if (cleanupOnDestruction() && initialized_) {
            try {
                cleanup();
            } catch (const std::exception& e) {}
        }
    }

    void cleanup() override {
        if (world_ && cleanupOnDestruction()) {
            world_->reset();
            if (on_cleanup_) {
                on_cleanup_();
            }
        }
        setInitialized(false);
    }

    std::string getStatus() const override {
        if (!world_) return "Cleaned up";
        return "Active - World simulation running";
    }

    void setOnCleanup(std::function<void()> callback) {
        on_cleanup_ = std::move(callback);
    }

    std::shared_ptr<SimpleWorld> world() const { return world_; }

    void update(float timestep) {
        if (world_) {
            world_->update(timestep);
        }
    }

    void applyMotorCommand(const Action& action, float currentTime) {
        if (world_) {
            world_->applyMotorCommand(action, currentTime);
        }
    }

    float getSimulationTime() const {
        return world_ ? world_->getSimulationTime() : 0.0f;
    }

    size_t getObjectCount() const {
        return world_ ? 0 : 0;  // Simplified - would need actual implementation
    }

    // Additional world properties
    size_t getMaxEnergy() const {
        return world_ ? world_->getMaxEnergy() : 0.0f;
    }

    float getEnergyDecayRate() const {
        return world_ ? world_->getEnergyDecayRate() : 0.0f;
    }

    size_t getWidth() const {
        return world_ ? world_->getWidth() : 0;
    }

    size_t getHeight() const {
        return world_ ? world_->getHeight() : 0;
    }

    int getRandomSeed() const {
        return world_ ? world_->getRandomSeed() : 0;
    }
};

// Progress monitor for simulation steps
class ProgressMonitor {
private:
    std::function<void(double)> on_progress_;
    std::function<void(const std::string&)> on_message_;
    std::function<void()> on_complete_;
    std::function<void()> on_error_;
    std::atomic<double> progress_ = 0.0;
    std::atomic<bool> cancelled_ = false;

public:
    ProgressMonitor() = default;

    void setOnProgress(std::function<void(double)> callback) {
        on_progress_ = std::move(callback);
    }

    void setOnMessage(std::function<void(const std::string&)> callback) {
        on_message_ = std::move(callback);
    }

    void setOnComplete(std::function<void()> callback) {
        on_complete_ = std::move(callback);
    }

    void setOnError(std::function<void()> callback) {
        on_error_ = std::move(callback);
    }

    void setCancelled(bool cancelled) {
        cancelled_ = cancelled;
    }

    bool cancelled() const {
        return cancelled_;
    }

    void updateProgress(double progress) {
        progress_ = progress;
        if (on_progress_) {
            try {
                on_progress_(progress);
            } catch (...) {}
        }
    }

    void logMessage(const std::string& message) {
        if (on_message_) {
            try {
                on_message_(message);
            } catch (...) {}
        }
    }

    void complete() {
        if (on_complete_) {
            try {
                on_complete_();
            } catch (...) {}
        }
    }

    void error() {
        if (on_error_) {
            try {
                on_error_();
            } catch (...) {}
        }
    }

    double progress() const {
        return progress_;
    }

    // Additional progress methods
    void setProgress(double progress) {
        updateProgress(progress);
    }

    bool isCompleted() const {
        return progress_ >= 1.0;
    }

    bool isRunning() const {
        return progress_ > 0.0 && progress_ < 1.0;
    }

    double getProgressPercentage() const {
        return progress_ * 100.0;
    }
};

// Factory methods for preset configurations
namespace Factory {
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", "1000");
        config->set("brain.synaptic_density", "0.1");
        config->set("brain.initial_firing_rate", "0.01");
        config->set("brain.development.enabled", "true");
        config->set("brain.reward_modulation.enabled", "true");
        config->set("brain.structural_plasticity.enabled", "false");
        config->set("brain.curiosity.enabled", "true");
        config->set("brain.novelty.enabled", "true");
        config->set("brain.prediction.enabled", "true");
        config->set("brain.memory.enabled", "true");
        config->set("world.width", "100.0");
        config->set("world.height", "100.0");
        config->set("world.vision_width", "20");
        config->set("world.vision_height", "20");
        config->set("world.max_energy", "100.0");
        config->set("world.energy_decay_rate", "0.01");
        config->set("environment.random_seed", "42");
        return config;
    }

    static std::shared_ptr<Config> createAdvancedConfig() {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", "5000");
        config->set("brain.synaptic_density", "0.15");
        config->set("brain.initial_firing_rate", "0.02");
        config->set("brain.development.enabled", "true");
        config->set("brain.development.critical_period.duration", "10.0");
        config->set("brain.reward_modulation.enabled", "true");
        config->set("brain.reward_modulation.gamma", "0.9");
        config->set("brain.structural_plasticity.enabled", "true");
        config->set("brain.structural_plasticity.rate", "0.001");
        config->set("brain.curiosity.enabled", "true");
        config->set("brain.curiosity.exploration_rate", "0.3");
        config->set("brain.novelty.enabled", "true");
        config->set("brain.novelty.threshold", "0.7");
        config->set("brain.prediction.enabled", "true");
        config->set("brain.prediction.precision", "0.01");
        config->set("brain.memory.enabled", "true");
        config->set("brain.memory.capacity", "10000");
        config->set("world.width", "200.0");
        config->set("world.height", "200.0");
        config->set("world.vision_width", "30");
        config->set("world.vision_height", "30");
        config->set("world.max_energy", "200.0");
        config->set("world.energy_decay_rate", "0.005");
        config->set("environment.random_seed", std::to_string(std::random_device()()));
        return config;
    }

    static std::shared_ptr<Config> createResearchConfig() {
        auto config = createAdvancedConfig();
        config->set("brain.neuron_count", "10000");
        config->set("brain.synaptic_density", "0.2");
        config->set("brain.initial_firing_rate", "0.05");
        config->set("brain.development.enabled", "true");
        config->set("brain.development.maturation.rate", "0.01");
        config->set("brain.development.aging.enabled", "true");
        config->set("brain.development.aging.start_age", "50.0");
        config->set("brain.reward_modulation.enabled", "true");
        config->set("brain.reward_modulation.gamma", "0.8");
        config->set("brain.structural_plasticity.enabled", "true");
        config->set("brain.structural_plasticity.rate", "0.005");
        config->set("brain.curiosity.enabled", "true");
        config->set("brain.curiosity.exploration_rate", "0.5");
        config->set("brain.novelty.enabled", "true");
        config->set("brain.novelty.threshold", "0.8");
        config->set("brain.prediction.enabled", "true");
        config->set("brain.prediction.precision", "0.001");
        config->set("brain.memory.enabled", "true");
        config->set("brain.memory.capacity", "50000");
        config->set("brain.memory.episodic.enabled", "true");
        config->set("world.width", "400.0");
        config->set("world.height", "400.0");
        config->set("world.vision_width", "50");
        config->set("world.vision_height", "50");
        config->set("world.max_energy", "400.0");
        config->set("world.energy_decay_rate", "0.001");
        config->set("environment.random_seed", std::to_string(std::random_device()()));
        config->set("performance.profiling.enabled", "true");
        config->set("performance.batching.enabled", "true");
        config->set("performance.batching.size", "100");
        return config;
    }

    static std::shared_ptr<Config> createMinimalConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", "100");
        config->set("brain.synaptic_density", "0.05");
        config->set("brain.initial_firing_rate", "0.01");
        config->set("brain.development.enabled", "false");
        config->set("brain.reward_modulation.enabled", "false");
        config->set("brain.structural_plasticity.enabled", "false");
        config->set("brain.curiosity.enabled", "false");
        config->set("brain.novelty.enabled", "false");
        config->set("brain.prediction.enabled", "false");
        config->set("brain.memory.enabled", "false");
        config->set("world.width", "50.0");
        config->set("world.height", "50.0");
        config->set("world.vision_width", "10");
        config->set("world.vision_height", "10");
        config->set("world.max_energy", "50.0");
        config->set("world.energy_decay_rate", "0.02");
        config->set("environment.random_seed", "42");
        return config;
    }

    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw FactoryError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }

    static std::shared_ptr<SimpleWorld> createWorld() {
        return std::make_shared<SimpleWorld>();
    }

    static std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw FactoryError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }

    static py::dict createBenchmarkConfig() {
        py::dict config;
        config["neuron_count"] = 1000;
        config["duration_seconds"] = 60.0;
        config["timestep"] = 0.01;
        config["output_file"] = "benchmark_results.json";
        config["memory_tracking"] = true;
        config["profiling"] = true;
        config["parallel"] = false;
        config["verbose"] = true;
        return config;
    }

    // Additional factory methods
    static py::dict createSimpleScenarioConfig() {
        py::dict config;
        config["name"] = "simple_scenario";
        config["description"] = "Simple simulation scenario for testing";
        config["duration_seconds"] = 30.0;
        config["timestep"] = 0.01;
        config["goal"] = "observe_behavior";
        config["reward_function"] = "sparse";
        return config;
    }

    static py::dict createExplorationConfig() {
        py::dict config;
        config["name"] = "exploration_scenario";
        config["description"] = "Exploration-focused simulation";
        config["duration_seconds"] = 120.0;
        config["timestep"] = 0.005;
        config["goal"] = "maximize_novelty";
        config["reward_function"] = "continuous";
        config["exploration_rate"] = 0.5;
        return config;
    }

    static py::dict createBenchmarkScenarioConfig() {
        py::dict config;
        config["name"] = "benchmark_scenario";
        config["description"] = "Performance benchmarking scenario";
        config["duration_seconds"] = 300.0;
        config["timestep"] = 0.001;
        config["goal"] = "maximize_speed";
        config["memory_limit_mb"] = 1024;
        config["parallel_workers"] = 4;
        config["profiling"] = true;
        config["output_format"] = "json";
        return config;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Custom exception types (ConfigurationError, SimulationError, MemoryError, ProgressError, SerializationError, FactoryError)
        - Context managers for Brain simulation, Configuration, and World control
        - Python property access for commonly used getters
        - NumPy array integration for vision and motor data
        - Advanced simulation features (Progress monitoring, callbacks, etc.)
        - Improved serialization with checkpoint management
        - Factory methods for preset configurations
        - Ready-made simulation scenarios
        - Benchmarking utilities
    )pbdoc";

    // Register custom exception types
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<ProgressError>(m, "ProgressError");
    py::register_exception<SerializationError>(m, "SerializationError");
    py::register_exception<FactoryError>(m, "FactoryError");

    // Register existing std::runtime_error as RuntimeError
    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
        .def("__ne__", &NeuronId::operator!=)
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

    // Config class with context manager support
    py::class_<Config, ConfigContextManager>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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

    // Context manager for Config
    py::class_<ConfigContextManager>(m, "ConfigManager", R"pbdoc(Context manager for configuration)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](ConfigContextManager& self) -> ConfigContextManager& {
            return self;
        })
        .def("__exit__", [](ConfigContextManager& self, py::args) {
            self.cleanup();
        })
        .def("cleanup", &ConfigContextManager::cleanup)
        .def("getStatus", &ConfigContextManager::getStatus)
        .def("brain", &ConfigContextManager::config)
        .def("load_from_file", &ConfigContextManager::loadFromFile, py::arg("filepath"))
        .def("save_to_file", &ConfigContextManager::saveToFile, py::arg("filepath"))
        .def("has", &ConfigContextManager::has, py::arg("key"))
        .def("get", &ConfigContextManager::getValue, py::arg("key"))
        .def("set", &ConfigContextManager::setValue, py::arg("key"), py::arg("value"))
        .def("get_keys", &ConfigContextManager::getKeys)
        .def("clear", &ConfigContextManager::clear)
        .def("summary", &ConfigContextManager::summary);

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with NumPy support
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataNumpy", [](Vision& self, py::array data) {
            auto buf = data.request();
            if (buf.ndim != 2 && buf.ndim != 3) {
                throw SimulationError("Vision data must be 2D or 3D numpy array");
            }
            size_t size = 1;
            for (size_t i = 0; i < buf.ndim; ++i) {
                size *= buf.shape[i];
            }
            std::vector<float> vec(size);
            float* ptr = static_cast<float*>(buf.ptr);
            std::copy(ptr, ptr + size, vec.begin());
            self.setData(vec);
        }, py::arg("data"),
             "Set vision data from numpy array")
        .def("getDataNumpy", [](Vision& self) {
            auto data = self.getData();
            npy_intp shape[] = {static_cast<npy_intp>(data.size())};
            return py::array_t<float>(shape, data.data());
        }, "Get vision data as numpy array")
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
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData)
        .def("getNumSignals", &InternalSignals::getDimensions)
        .def("getType", &InternalSignals::getType)
        .def("getData", &InternalSignals::getData)
        .def("getTimestamp", &InternalSignals::getTimestamp)
        .def("setTimestamp", &InternalSignals::setTimestamp, py::arg("timestamp"));

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

    // AgentBody class with property access
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

    // SimpleWorld class with context manager support
    py::class_<SimpleWorld, WorldContextManager>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // Context manager for World
    py::class_<WorldContextManager>(m, "WorldManager", R"pbdoc(Context manager for world simulation)")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("__enter__", [](WorldContextManager& self) -> WorldContextManager& {
            return self;
        })
        .def("__exit__", [](WorldContextManager& self, py::args) {
            self.cleanup();
        })
        .def("cleanup", &WorldContextManager::cleanup)
        .def("getStatus", &WorldContextManager::getStatus)
        .def("update", &WorldContextManager::update, py::arg("timestep"))
        .def("apply_motor_command", &WorldContextManager::applyMotorCommand,
             py::arg("action"), py::arg("currentTime"));

    // Brain class with context manager support
    py::class_<Brain, BrainContextManager>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             "Get all regions");

    // Context manager for Brain
    py::class_<BrainContextManager>(m, "BrainManager", R"pbdoc(Context manager for brain simulation)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](BrainContextManager& self) -> BrainContextManager& {
            return self;
        })
        .def("__exit__", [](BrainContextManager& self, py::args) {
            self.cleanup();
        })
        .def("cleanup", &BrainContextManager::cleanup)
        .def("getStatus", &BrainContextManager::getStatus)
        .def("reset", &BrainContextManager::reset)
        .def("step", &BrainContextManager::step, py::arg("currentStep"), py::arg("currentTime"))
        .def("get_average_firing_rate", &BrainContextManager::getAverageFiringRate)
        .def("get_total_neuron_count", &BrainContextManager::getTotalNeuronCount)
        .def("get_total_synapse_count", &BrainContextManager::getTotalSynapseCount)
        .def("get_active_neuron_count", &BrainContextManager::getActiveNeuronCount)
        .def("get_firing_neuron_count", &BrainContextManager::getFiringNeuronCount)
        .def("get_excitation_inhibition_ratio", &BrainContextManager::getExcitationInhibitionRatio)
        .def("get_total_spike_count", &BrainContextManager::getTotalSpikeCount)
        .def("get_region_ids", &BrainContextManager::getRegionIds)
        .def("get_region_count", &BrainContextManager::getRegionCount)
        .def("get_region", &BrainContextManager::getRegion, py::arg("id"));

    // AgentBrain class
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

    // Progress monitor class
    py::class_<ProgressMonitor>(m, "ProgressMonitor", R"pbdoc(Progress monitoring for simulation steps)")
        .def(py::init<>())
        .def("set_on_progress", &ProgressMonitor::setOnProgress, py::arg("callback"),
             "Set callback for progress updates")
        .def("set_on_message", &ProgressMonitor::setOnMessage, py::arg("callback"),
             "Set callback for log messages")
        .def("set_on_complete", &ProgressMonitor::setOnComplete, py::arg("callback"),
             "Set callback for completion")
        .def("set_on_error", &ProgressMonitor::setOnError, py::arg("callback"),
             "Set callback for error")
        .def("set_cancelled", &ProgressMonitor::setCancelled, py::arg("cancelled"),
             "Set cancelled flag")
        .def("cancelled", &ProgressMonitor::cancelled, "Check if cancelled")
        .def("update_progress", &ProgressMonitor::updateProgress, py::arg("progress"),
             "Update progress value")
        .def("log_message", &ProgressMonitor::logMessage, py::arg("message"),
             "Log a message")
        .def("complete", &ProgressMonitor::complete, "Mark as complete")
        .def("error", &ProgressMonitor::error, "Mark as error")
        .def("progress", &ProgressMonitor::progress, "Get current progress");

    // Factory methods
    m.def("createDefaultConfig", &Factory::createDefaultConfig,
          "Create a default configuration")
        .def("createAdvancedConfig", &Factory::createAdvancedConfig,
             "Create an advanced configuration")
        .def("createResearchConfig", &Factory::createResearchConfig,
             "Create a research configuration")
        .def("createMinimalConfig", &Factory::createMinimalConfig,
             "Create a minimal configuration")
        .def("createBrain", &Factory::createBrain, py::arg("config"),
             "Create a new brain with configuration")
        .def("createSimpleWorld", &Factory::createWorld,
             "Create a new simple world")
        .def("createAgentBrain", &Factory::createAgentBrain, py::arg("brain"),
             "Create a new agent brain interface")
        .def("createBenchmarkConfig", &Factory::createBenchmarkConfig,
             "Create a benchmark configuration dictionary")
        .def("createSimpleScenarioConfig", &Factory::createSimpleScenarioConfig,
             "Create a simple scenario configuration")
        .def("createExplorationConfig", &Factory::createExplorationConfig,
             "Create an exploration scenario configuration")
        .def("createBenchmarkScenarioConfig", &Factory::createBenchmarkScenarioConfig,
             "Create a benchmark scenario configuration");

    // Simulation with progress monitoring helper
    m.def("simulateWithProgress", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   const py::dict& config,
                                   std::shared_ptr<ProgressMonitor> monitor) {
        // Implementation of simulation with progress monitoring
        double totalSteps = py::cast<double>(config["total_steps"]);
        double timestep = py::cast<double>(config["timestep"]);
        for (double step = 0; step < totalSteps && !monitor->cancelled(); step += 1.0) {
            brain->step(static_cast<SimulationStep>(step), Timestamp(step * timestep));
            monitor->updateProgress(step / totalSteps);
            monitor->logMessage("Step " + std::to_string(step) + " completed");
        }
        monitor->complete();
    }, py::arg("brain"), py::arg("world"), py::arg("config"), py::arg("monitor"),
       "Simulate with progress monitoring");

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // NumPy array integration helper functions
    m.def("array_to_vector", [](py::array array) {
        auto buf = array.request();
        if (buf.ndim != 1) {
            throw SimulationError("Array must be 1D");
        }
        size_t size = buf.shape[0];
        std::vector<float> result(size);
        float* ptr = static_cast<float*>(buf.ptr);
        std::copy(ptr, ptr + size, result.begin());
        return result;
    }, py::arg("array"),
          "Convert numpy array to std::vector<float>");

    m.def("vector_to_array", [](const std::vector<float>& vec) {
        npy_intp shape[] = {static_cast<npy_intp>(vec.size())};
        return py::array_t<float>(shape, vec.data());
    }, py::arg("vector"),
          "Convert std::vector<float> to numpy array");

    m.def("batch_process", [](const std::vector<py::array>& arrays) {
        std::vector<std::vector<float>> batch;
        for (const auto& array : arrays) {
            auto buf = array.request();
            if (buf.ndim != 2) {
                throw SimulationError("Each array must be 2D");
            }
            std::vector<float> row(buf.shape[1]);
            float* ptr = static_cast<float*>(buf.ptr);
            std::copy(ptr, ptr + buf.shape[1], row.begin());
            batch.push_back(row);
        }
        return batch;
    }, py::arg("arrays"),
          "Process batch of numpy arrays");
}

} // namespace nlm