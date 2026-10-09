#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception hierarchy
class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public NLMBrainError {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMBrainError(msg) {}
};

class NLMMemoryError : public NLMBrainError {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMBrainError(msg) {}
};

class NLMPredictionError : public NLMBrainError {
public:
    explicit NLMPredictionError(const std::string& msg) : NLMBrainError(msg) {}
};

// Enhanced Config class with property access
class Config {
private:
    std::shared_ptr<nlm::Config> config_;

public:
    Config() : config_(std::make_shared<nlm::Config>()) {}
    explicit Config(const std::shared_ptr<nlm::Config>& cfg) : config_(cfg) {}

    // Property-based access
    void setProperty(const std::string& category, const std::string& key, const std::string& value) {
        std::string fullKey = category + "." + key;
        config_->set(fullKey, value, ConfigSource::Default);
    }

    std::string getProperty(const std::string& category, const std::string& key, const std::string& defaultValue = "") const {
        std::string fullKey = category + "." + key;
        auto valueOpt = config_->get<std::string>(fullKey);
        return valueOpt ? *valueOpt : defaultValue;
    }

    int getProperty(const std::string& category, const std::string& key, int defaultValue) const {
        std::string fullKey = category + "." + key;
        auto valueOpt = config_->get<int64_t>(fullKey);
        return valueOpt ? static_cast<int>(*valueOpt) : defaultValue;
    }

    float getProperty(const std::string& category, const std::string& key, float defaultValue) const {
        std::string fullKey = category + "." + key;
        auto valueOpt = config_->get<float>(fullKey);
        return valueOpt ? *valueOpt : defaultValue;
    }

    bool getProperty(const std::string& category, const std::string& key, bool defaultValue) const {
        std::string fullKey = category + "." + key;
        auto valueOpt = config_->get<bool>(fullKey);
        return valueOpt ? *valueOpt : defaultValue;
    }

    // Configuration presets
    void loadPreset(const std::string& preset) {
        if (preset == "beginner") {
            config_->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config_->set("brain.synapse_density", 0.1f, ConfigSource::Default);
            config_->set("neuromodulation.curiosity.enable", true, ConfigSource::Default);
            config_->set("neuromodulation.novelty.enable", true, ConfigSource::Default);
            config_->set("development.enable_development", true, ConfigSource::Default);
        } else if (preset == "explorer") {
            config_->set("brain.neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config_->set("brain.synapse_density", 0.05f, ConfigSource::Default);
            config_->set("neuromodulation.curiosity.enable", true, ConfigSource::Default);
            config_->set("neuromodulation.novelty.enable", true, ConfigSource::Default);
            config_->set("neuromodulation.dopamine.scale", 1.5f, ConfigSource::Default);
            config_->set("development.enable_development", true, ConfigSource::Default);
        } else if (preset == "researcher") {
            config_->set("brain.neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
            config_->set("brain.synapse_density", 0.01f, ConfigSource::Default);
            config_->set("neuromodulation.curiosity.enable", true, ConfigSource::Default);
            config_->set("neuromodulation.novelty.enable", true, ConfigSource::Default);
            config_->set("neuromodulation.dopamine.scale", 2.0f, ConfigSource::Default);
            config_->set("development.enable_development", true, ConfigSource::Default);
            config_->set("plasticity.structural.enable", true, ConfigSource::Default);
        }
    }

    bool validate() const {
        // Basic validation
        if (config_->has("brain.neuron_count")) {
            auto neuronCount = config_->get<int64_t>("brain.neuron_count");
            if (!neuronCount || *neuronCount < 100) {
                return false;
            }
        }
        return true;
    }

    // Delegate to underlying config
    bool has(const std::string& key) const { return config_->has(key); }
    std::vector<std::string> getKeys() const { return config_->getKeys(); }
    void clear() { config_->clear(); }
    std::string summary() const { return config_->summary(); }
    bool loadFromFile(const std::string& filepath) { return config_->loadFromFile(filepath); }
    bool saveToFile(const std::string& filepath) { return config_->saveToFile(filepath); }

    // Conversion to original config
    operator std::shared_ptr<nlm::Config>() const { return config_; }
};

// Enhanced Brain class with new functionality
class EnhancedBrain {
private:
    std::shared_ptr<Brain> brain_;

public:
    EnhancedBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}

    // Simple high-level API
    void initializeWithValidation() {
        if (!brain_->initialize()) {
            throw NLMBrainError("Brain initialization failed");
        }
        // Validate basic properties
        if (brain_->getTotalNeuronCount() == 0) {
            throw NLMBrainError("Brain initialized with 0 neurons");
        }
    }

    void runSteps(int steps) {
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
        }
    }

    // Memory system access
    void enableWorkingMemory(bool enable) {
        if (enable) {
            auto* workingMem = brain_->getWorkingMemory();
            if (!workingMem) {
                // Create working memory
                brain_->enableWorkingMemory(true);
                workingMem = brain_->getWorkingMemory();
            }
        } else {
            brain_->enableWorkingMemory(false);
        }
    }

    size_t getWorkingMemoryTraceCount() const {
        auto* workingMem = brain_->getWorkingMemory();
        return workingMem ? workingMem->getActiveTraces() : 0;
    }

    void addWorkingMemoryTrace(const std::string& key, const std::vector<float>& pattern) {
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            workingMem->addTrace(key, pattern);
        } else {
            throw NLMMemoryError("Working memory not enabled");
        }
    }

    std::vector<float> getWorkingMemoryTrace(const std::string& key) const {
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            return workingMem->getTrace(key);
        }
        throw NLMMemoryError("Working memory trace not found");
    }

    // Episodic memory access
    void enableEpisodicMemory(bool enable) {
        if (enable) {
            auto* episodicMem = brain_->getEpisodicMemory();
            if (!episodicMem) {
                brain_->enableEpisodicMemory(true);
                episodicMem = brain_->getEpisodicMemory();
            }
        } else {
            brain_->enableEpisodicMemory(false);
        }
    }

    size_t getEpisodicMemoryCount() const {
        auto* episodicMem = brain_->getEpisodicMemory();
        return episodicMem ? episodicMem->getEpisodeCount() : 0;
    }

    void recordEpisode(float reward, const std::shared_ptr<Brain>& brain) {
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->recordEpisode(reward, brain);
        } else {
            throw NLMMemoryError("Episodic memory not enabled");
        }
    }

    // Prediction system access
    void enablePredictionSystem(bool enable) {
        if (enable) {
            auto* predictionSys = brain_->getPredictionSystem();
            if (!predictionSys) {
                brain_->enablePredictionSystem(true);
                predictionSys = brain_->getPredictionSystem();
            }
        } else {
            brain_->enablePredictionSystem(false);
        }
    }

    std::vector<float> predictNextAction(const std::vector<float>& state) {
        auto* predictionSys = brain_->getPredictionSystem();
        if (!predictionSys) {
            throw NLMPredictionError("Prediction system not enabled");
        }
        return predictionSys->predict(state);
    }

    float getPredictionError() const {
        auto* predictionSys = brain_->getPredictionSystem();
        if (!predictionSys) {
            return 0.0f;
        }
        return predictionSys->getError();
    }

    // Performance monitoring
    struct PerformanceMetrics {
        size_t totalSpikes;
        size_t firingNeurons;
        float averageFiringRate;
        size_t workingMemoryTraces;
        size_t episodicMemoryEpisodes;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        double wallClockTime;
    };

    PerformanceMetrics getPerformanceMetrics() const {
        PerformanceMetrics metrics;
        metrics.totalSpikes = brain_->getTotalSpikeCount();
        metrics.firingNeurons = brain_->getFiringNeuronCount();
        metrics.averageFiringRate = brain_->getAverageFiringRate();
        
        auto* workingMem = brain_->getWorkingMemory();
        metrics.workingMemoryTraces = workingMem ? workingMem->getActiveTraces() : 0;
        
        auto* episodicMem = brain_->getEpisodicMemory();
        metrics.episodicMemoryEpisodes = episodicMem ? episodicMem->getEpisodeCount() : 0;
        
        auto* curiosity = brain_->getCuriosity();
        metrics.curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
        
        auto* novelty = brain_->getNovelty();
        metrics.noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
        
        auto* predictionSys = brain_->getPredictionSystem();
        metrics.predictionError = predictionSys ? predictionSys->getError() : 0.0f;
        metrics.wallClockTime = 0.0; // Would need timing infrastructure
        
        return metrics;
    }

    // Enhanced agent interface
    void enableAllSystems(const std::map<std::string, bool>& systems) {
        if (systems.count("reward_modulation")) {
            brain_->getAgentBrain()->enableRewardModulation(systems.at("reward_modulation"));
        }
        if (systems.count("structural_plasticity")) {
            brain_->getAgentBrain()->enableStructuralPlasticity(systems.at("structural_plasticity"));
        }
        if (systems.count("development")) {
            brain_->getAgentBrain()->enableDevelopment(systems.at("development"));
        }
        if (systems.count("curiosity")) {
            brain_->getAgentBrain()->enableCuriosity(systems.at("curiosity"));
        }
        if (systems.count("novelty")) {
            brain_->getAgentBrain()->enableNovelty(systems.at("novelty"));
        }
    }

    // Convenience methods
    void setDevelopmentStage(const std::string& stage) {
        DevelopmentalStage devStage;
        if (stage == "Initial") devStage = DevelopmentalStage::Initial;
        else if (stage == "CriticalPeriod") devStage = DevelopmentalStage::CriticalPeriod;
        else if (stage == "Maturation") devStage = DevelopmentalStage::Maturation;
        else if (stage == "Adult") devStage = DevelopmentalStage::Adult;
        else if (stage == "Aging") devStage = DevelopmentalStage::Aging;
        else devStage = DevelopmentalStage::Initial;
        brain_->setDevelopmentalStage(devStage);
    }

    std::string getDevelopmentStage() const {
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Initial";
        }
    }

    // Batch processing
    static std::vector<std::shared_ptr<EnhancedBrain>> createBatch(const std::shared_ptr<Config>& config, size_t count) {
        std::vector<std::shared_ptr<EnhancedBrain>> brains;
        for (size_t i = 0; i < count; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(std::make_shared<EnhancedBrain>(brain));
        }
        return brains;
    }

    // Event-driven programming
    using SpikeCallback = std::function<void(const NeuronId&, Timestamp, float)>;
    using LearningCallback = std::function<void(const SynapseId&, float, float)>;

    void onSpike(const SpikeCallback& callback) {
        // Would need callback registration infrastructure
    }

    void onLearning(const LearningCallback& callback) {
        // Would need callback registration infrastructure
    }

    // Delegation to original brain for compatibility
    void initialize() { brain_->initialize(); }
    void step(SimulationStep step) { brain_->step(step); }
    void step(SimulationStep step, Timestamp time) { brain_->step(step, time); }
    void receiveSensoryInput(const SensoryPercept& input) { brain_->receiveSensoryInput(input); }
    void injectCurrent(const NeuronId& neuron, float current) { brain_->injectCurrent(neuron, current); }
    void injectCurrentToNeurons(NeuronType type, float current) { brain_->injectCurrentToNeurons(type, current); }
    Action produceAction() { return brain_->produceAction(); }
    void reset() { brain_->reset(); }
    bool save(const std::string& filepath) { return brain_->save(filepath); }
    bool load(const std::string& filepath) { return brain_->load(filepath); }
    size_t addRegion(const std::string& name) { return brain_->addRegion(name); }
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) { return brain_->getRegion(id); }
    size_t getRegionCount() const { return brain_->getRegionCount(); }
    std::vector<RegionId> getRegionIds() const { return brain_->getRegionIds(); }
    std::vector<std::shared_ptr<NeuralRegion>> getRegions() const { return brain_->getRegions(); }
    size_t getTotalNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getTotalSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t getTotalSpikeCount() const { return brain_->getTotalSpikeCount(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    void setDevelopmentalStage(DevelopmentalStage stage) { brain_->setDevelopmentalStage(stage); }
    std::shared_ptr<Config> getConfig() const { return brain_->getConfig(); }
    void logStatus() const { brain_->logStatus(); }

    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }
};

// Enhanced AgentBrain interface
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agentBrain_;

public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agentBrain_(std::make_shared<AgentBrain>(brain)) {}

    // Enhanced initialization
    void initializeWithWorld(const std::shared_ptr<SimpleWorld>& world) {
        agentBrain_->initialize(*world);
    }

    // Advanced neuromodulation control
    void setNeuromodulationLevels(float dopamine, float curiosity, float novelty) {
        // Access neuromodulation levels would require internal access
        // For now, we'll use the existing interface
    }

    // Advanced action selection
    Action decodeMotorCommandWithPlanning() {
        Action cmd = agentBrain_->decodeMotorCommand();
        
        // Apply planning logic
        if (agentBrain_->getCuriosityLevel() > 0.5f) {
            // Exploration behavior
            float exploreChance = agentBrain_->getCuriosityLevel() * 0.3f;
            auto* brain = agentBrain_->getBrain();
            if (brain) {
                float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
                if (r < exploreChance) {
                    // Return random action
                    int choice = brain->getRandomGenerator()->uniformInt(0, 6);
                    switch (choice) {
                        case 0: return Action(ActionType::MoveForward);
                        case 1: return Action(ActionType::MoveBackward);
                        case 2: return Action(ActionType::TurnLeft);
                        case 3: return Action(ActionType::TurnRight);
                        case 4: return Action(ActionType::Look);
                        case 5: return Action(ActionType::Interact);
                        default: return Action(ActionType::Wait);
                    }
                }
            }
        }
        
        return cmd;
    }

    // Event-driven programming
    using SensoryInputCallback = std::function<void(const SensoryPercept&)>;
    using ActionCallback = std::function<void(const Action&)>;

    void onSensoryInput(const SensoryInputCallback& callback) {
        // Would need callback registration
    }

    void onAction(const ActionCallback& callback) {
        // Would need callback registration
    }

    // Delegation to original agent brain
    void getSensoryInputSize() const { return agentBrain_->getSensoryInputSize(); }
    void getMotorOutputSize() const { return agentBrain_->getMotorOutputSize(); }
    void processSensoryInput(const SensoryPercept& percept) { agentBrain_->processSensoryInput(percept); }
    Action decodeMotorCommand() { return agentBrain_->decodeMotorCommand(); }
    void applyRewardModulation(float reward, float predictedReward) { agentBrain_->applyRewardModulation(reward, predictedReward); }
    void updateDevelopment(double timestep) { agentBrain_->updateDevelopment(timestep); }
    DevelopmentalStage getDevelopmentalStage() const { return agentBrain_->getDevelopmentalStage(); }
    float getNeuromodulationLevel() const { return agentBrain_->getNeuromodulationLevel(); }
    float getCuriosityLevel() const { return agentBrain_->getCuriosityLevel(); }
    float getNoveltyLevel() const { return agentBrain_->getNoveltyLevel(); }
    float getPredictionError() const { return agentBrain_->getPredictionError(); }
    void reset() { agentBrain_->reset(); }

    // Enhanced system control
    void enableRewardModulation(bool enable) { agentBrain_->enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) { agentBrain_->enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) { agentBrain_->enableDevelopment(enable); }
    void enableCuriosity(bool enable) { agentBrain_->enableCuriosity(enable); }
    void enableNovelty(bool enable) { agentBrain_->enableNovelty(enable); }

    bool isRewardModulationEnabled() const { return agentBrain_->isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return agentBrain_->isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return agentBrain_->isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return agentBrain_->isCuriosityEnabled(); }
    bool isNoveltyEnabled() const { return agentBrain_->isNoveltyEnabled(); }

    std::shared_ptr<Brain> getBrain() const { return agentBrain_->getBrain(); }
};

// Helper functions for complete agent setup
std::pair<std::shared_ptr<EnhancedBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<EnhancedAgentBrain>> 
createCompleteAgent(const std::string& preset = "explorer") {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->loadPreset(preset);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    auto enhancedBrain = std::make_shared<EnhancedBrain>(brain);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Create agent
    auto agent = std::make_shared<EnhancedAgentBrain>(brain);
    agent->initializeWithWorld(world);
    
    return std::make_pair(enhancedBrain, world, agent);
}

void runEpisode(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<SimpleWorld> world, 
                std::shared_ptr<EnhancedAgentBrain> agent, int steps) {
    brain->initializeWithValidation();
    
    for (int step = 0; step < steps; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommandWithPlanning();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation based on world state
        auto worldPercept = world->getSensoryPercept();
        float reward = 0.0f;
        if (worldPercept.getInternal().size() > 0) {
            reward = worldPercept.getInternal()[0];
        }
        agent->applyRewardModulation(reward, 0.0f);
    }
}

// High-level API for common use cases
std::shared_ptr<EnhancedBrain> createSimpleBrain() {
    auto config = std::make_shared<Config>();
    config->loadPreset("beginner");
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return std::make_shared<EnhancedBrain>(brain);
}

std::shared_ptr<Config> createConfig(const std::map<std::string, std::map<std::string, std::string>>& settings) {
    auto config = std::make_shared<Config>();
    for (const auto& category : settings) {
        for (const auto& setting : category.second) {
            config->setProperty(category.first, setting.first, setting.second);
        }
    }
    return config;
}

void runScript(const std::string& script, std::shared_ptr<EnhancedBrain> brain) {
    // Simple script interpreter
    // In real implementation, would use proper parsing
    if (script.find("initialize()") != std::string::npos) {
        brain->initializeWithValidation();
    }
    if (script.find("step") != std::string::npos) {
        // Extract step number from script
        // For now, just run 10 steps
        brain->runSteps(10);
    }
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        Advanced Python bindings for NLM with enhanced API, memory system access, 
        prediction systems, and performance monitoring.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMBrainError>(m, "NLMBrainError", PyExc_RuntimeError);
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError", PyExc_ValueError);
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError", PyExc_RuntimeError);
    py::register_exception<NLMPredictionError>(m, "NLMPredictionError", PyExc_RuntimeError);

    // Enhanced Config class
    py::class_<Config>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration class with property access)pbdoc")
        .def(py::init<>())
        .def("setProperty", &Config::setProperty, py::arg("category"), py::arg("key"), py::arg("value"),
             "Set configuration property using category.key notation")
        .def("getProperty", [](const Config& self, const std::string& category, const std::string& key, const std::string& defaultValue) {
            return self.getProperty(category, key, defaultValue);
        }, py::arg("category"), py::arg("key"), py::arg("defaultValue") = "",
             "Get configuration property with category.key notation")
        .def("getInt", [](const Config& self, const std::string& category, const std::string& key, int defaultValue) {
            return self.getProperty(category, key, defaultValue);
        }, py::arg("category"), py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer property")
        .def("getFloat", [](const Config& self, const std::string& category, const std::string& key, float defaultValue) {
            return self.getProperty(category, key, defaultValue);
        }, py::arg("category"), py::arg("key"), py::arg("defaultValue") = 0.0f,
             "Get float property")
        .def("getBool", [](const Config& self, const std::string& category, const std::string& key, bool defaultValue) {
            return self.getProperty(category, key, defaultValue);
        }, py::arg("category"), py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean property")
        .def("loadPreset", &Config::loadPreset, py::arg("preset"),
             "Load configuration preset ('beginner', 'explorer', 'researcher')")
        .def("validate", &Config::validate,
             "Validate configuration parameters")
        .def("createConfig", [](const std::map<std::string, std::map<std::string, std::string>>& settings) {
            return createConfig(settings);
        }, py::arg("settings"),
             "Create config from settings map");

    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain", R"pbdoc(Enhanced brain interface with memory and prediction systems)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initializeWithValidation", &EnhancedBrain::initializeWithValidation,
             "Initialize brain with validation")
        .def("runSteps", &EnhancedBrain::runSteps, py::arg("steps"),
             "Run specified number of simulation steps")
        .def("enableWorkingMemory", &EnhancedBrain::enableWorkingMemory, py::arg("enable"),
             "Enable or disable working memory system")
        .def("getWorkingMemoryTraceCount", &EnhancedBrain::getWorkingMemoryTraceCount,
             "Get number of working memory traces")
        .def("addWorkingMemoryTrace", &EnhancedBrain::addWorkingMemoryTrace,
             py::arg("key"), py::arg("pattern"),
             "Add a trace to working memory")
        .def("getWorkingMemoryTrace", &EnhancedBrain::getWorkingMemoryTrace,
             py::arg("key"),
             "Get a working memory trace")
        .def("enableEpisodicMemory", &EnhancedBrain::enableEpisodicMemory, py::arg("enable"),
             "Enable or disable episodic memory system")
        .def("getEpisodicMemoryCount", &EnhancedBrain::getEpisodicMemoryCount,
             "Get number of episodic memory entries")
        .def("recordEpisode", &EnhancedBrain::recordEpisode,
             py::arg("reward"), py::arg("brain"),
             "Record an episode in episodic memory")
        .def("enablePredictionSystem", &EnhancedBrain::enablePredictionSystem, py::arg("enable"),
             "Enable or disable prediction system")
        .def("predictNextAction", &EnhancedBrain::predictNextAction,
             py::arg("state"),
             "Predict next action from current state")
        .def("getPredictionError", &EnhancedBrain::getPredictionError,
             "Get current prediction error")
        .def("getPerformanceMetrics", &EnhancedBrain::getPerformanceMetrics,
             "Get performance metrics as dictionary")
        .def("enableAllSystems", &EnhancedBrain::enableAllSystems, py::arg("systems"),
             "Enable multiple systems at once")
        .def("setDevelopmentStage", &EnhancedBrain::setDevelopmentStage, py::arg("stage"),
             "Set developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)")
        .def("getDevelopmentStage", &EnhancedBrain::getDevelopmentStage,
             "Get current developmental stage")
        .def("createBatch", [](const std::shared_ptr<Config>& config, size_t count) {
            return EnhancedBrain::createBatch(config, count);
        }, py::arg("config"), py::arg("count"),
             "Create multiple brains (batch processing)")
        .def("onSpike", &EnhancedBrain::onSpike,
             "Register spike callback")
        .def("onLearning", &EnhancedBrain::onLearning,
             "Register learning callback")
        // Delegate to original brain methods
        .def("initialize", &EnhancedBrain::initialize)
        .def("step", [](EnhancedBrain& self, SimulationStep step) { self.step(step); }, py::arg("step"))
        .def("step", [](EnhancedBrain& self, SimulationStep step, Timestamp time) { self.step(step, time); },
             py::arg("step"), py::arg("time"))
        .def("receiveSensoryInput", &EnhancedBrain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &EnhancedBrain::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &EnhancedBrain::injectCurrentToNeurons, py::arg("type"), py::arg("current"))
        .def("produceAction", &EnhancedBrain::produceAction)
        .def("reset", &EnhancedBrain::reset)
        .def("save", &EnhancedBrain::save, py::arg("filepath"))
        .def("load", &EnhancedBrain::load, py::arg("filepath"))
        .def("addRegion", &EnhancedBrain::addRegion, py::arg("name") = "")
        .def("getRegion", &EnhancedBrain::getRegion, py::arg("id"))
        .def("getRegionCount", &EnhancedBrain::getRegionCount)
        .def("getRegionIds", &EnhancedBrain::getRegionIds)
        .def("getRegions", &EnhancedBrain::getRegions)
        .def("getTotalNeuronCount", &EnhancedBrain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &EnhancedBrain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &EnhancedBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &EnhancedBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &EnhancedBrain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &EnhancedBrain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &EnhancedBrain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &EnhancedBrain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &EnhancedBrain::setDevelopmentalStage, py::arg("stage"))
        .def("getConfig", &EnhancedBrain::getConfig)
        .def("logStatus", &EnhancedBrain::logStatus)
        .def("createSimpleBrain", &createSimpleBrain,
             "Create a simple brain with beginner preset");

    // Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain", R"pbdoc(Enhanced agent brain interface with advanced control)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initializeWithWorld", &EnhancedAgentBrain::initializeWithWorld, py::arg("world"),
             "Initialize agent with world using enhanced API")
        .def("decodeMotorCommandWithPlanning", &EnhancedAgentBrain::decodeMotorCommandWithPlanning,
             "Decode motor command with planning and exploration")
        .def("onSensoryInput", &EnhancedAgentBrain::onSensoryInput,
             "Register sensory input callback")
        .def("onAction", &EnhancedAgentBrain::onAction,
             "Register action callback")
        // Delegate to original agent brain methods
        .def("getSensoryInputSize", &EnhancedAgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &EnhancedAgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &EnhancedAgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &EnhancedAgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &EnhancedAgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &EnhancedAgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &EnhancedAgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &EnhancedAgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &EnhancedAgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &EnhancedAgentBrain::getNoveltyLevel)
        .def("getPredictionError", &EnhancedAgentBrain::getPredictionError)
        .def("reset", &EnhancedAgentBrain::reset)
        .def("getBrain", &EnhancedAgentBrain::getBrain)
        .def("enableAllSystems", &EnhancedAgentBrain::enableAllSystems, py::arg("systems"),
             "Enable multiple agent systems at once")
        .def("enableRewardModulation", &EnhancedAgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &EnhancedAgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &EnhancedAgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &EnhancedAgentBrain::enableCuriosity, py::arg("enable"))
        .def("enableNovelty", &EnhancedAgentBrain::enableNovelty, py::arg("enable"))
        .def("isRewardModulationEnabled", &EnhancedAgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &EnhancedAgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &EnhancedAgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &EnhancedAgentBrain::isCuriosityEnabled)
        .def("isNoveltyEnabled", &EnhancedAgentBrain::isNoveltyEnabled);

    // Complete agent setup helper
    m.def("createCompleteAgent", &createCompleteAgent,
          py::arg("preset") = "explorer",
          "Create a complete agent setup (brain, world, agent)");

    m.def("runEpisode", &runEpisode,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          "Run a complete episode with agent and world");

    m.def("createSimpleBrain", &createSimpleBrain,
          "Create a simple brain with beginner preset");

    m.def("runScript", &runScript,
          py::arg("script"), py::arg("brain"),
          "Run a simple script on brain");

    // Original compatibility functions (delegated to original bindings)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto cfg = std::make_shared<Config>();
        cfg->loadPreset("beginner");
        return cfg;
    }, "Create a default configuration with beginner preset");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
}

} // namespace nlm
