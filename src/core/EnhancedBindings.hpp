// Enhanced Python bindings for NLM with improved API and advanced features
#ifndef PYNLM_ENHANCED_BINDINGS_H
#define PYNLM_ENHANCED_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

namespace py = pybind11;
namespace nlm {

// Enhanced configuration class with validation and convenience methods
class EnhancedConfig {
public:
    EnhancedConfig() : config(std::make_shared<Config>()) {}
    
    // Create default configuration with optimal settings for different use cases
    static EnhancedConfig createBasicConfig() {
        EnhancedConfig cfg;
        cfg.set("brain.neuron_count", 1000);
        cfg.set("brain.synapse_density", 0.1f);
        cfg.set("brain.connection_probability", 0.05f);
        cfg.set("plasticity.stdp.enable", true);
        cfg.set("plasticity.hebbian.enable", true);
        cfg.set("neuromodulation.curiosity.enable", true);
        cfg.set("neuromodulation.novelty.enable", true);
        cfg.set("development.initial.plasticity", 1.0f);
        return cfg;
    }
    
    static EnhancedConfig createAdvancedConfig() {
        EnhancedConfig cfg;
        // Advanced settings for sophisticated simulations
        cfg.set("brain.neuron_count", 5000);
        cfg.set("brain.synapse_density", 0.2f);
        cfg.set("brain.connection_probability", 0.08f);
        cfg.set("plasticity.stdp.enable", true);
        cfg.set("plasticity.stdp.learning_rate", 0.001f);
        cfg.set("plasticity.hebbian.enable", true);
        cfg.set("plasticity.structural.enable", true);
        cfg.set("neuromodulation.curiosity.enable", true);
        cfg.set("neuromodulation.novelty.enable", true);
        cfg.set("neuromodulation.dopamine.scale", 1.0f);
        cfg.set("development.initial.plasticity", 1.0f);
        cfg.set("development.critical_period.plasticity", 0.8f);
        cfg.set("development.maturation.plasticity", 0.5f);
        cfg.set("prediction.enable", true);
        cfg.set("cognition.planner.enabled", true);
        cfg.set("cognition.concept_formation.enabled", true);
        cfg.set("memory.working.capacity", neuron_count / 10);
        cfg.set("memory.episodic.max_episodes", 10000);
        cfg.set("memory.associative.formation_rate", 0.01f);
        return cfg;
    }
    
    static EnhancedConfig createMinimalConfig() {
        EnhancedConfig cfg;
        cfg.set("brain.neuron_count", 100);
        cfg.set("brain.synapse_density", 0.05f);
        cfg.set("plasticity.stdp.enable", false);
        cfg.set("plasticity.hebbian.enable", false);
        cfg.set("neuromodulation.curiosity.enable", false);
        return cfg;
    }
    
    // Configuration methods with validation
    EnhancedConfig& set(const std::string& key, const std::string& value) {
        config->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, int64_t value) {
        config->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, double value) {
        config->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, float value) {
        config->set(key, value);
        return *this;
    }
    
    EnhancedConfig& set(const std::string& key, bool value) {
        config->set(key, value);
        return *this;
    }
    
    // Convenience methods for common configurations
    EnhancedConfig& enableAllSystems() {
        set("system.learning", true)
            .set("system.development", true)
            .set("system.neuromodulation", true)
            .set("system.prediction", true)
            .set("system.cognition", true);
        return *this;
    }
    
    EnhancedConfig& disableAllSystems() {
        set("system.learning", false)
            .set("system.development", false)
            .set("system.neuromodulation", false)
            .set("system.prediction", false)
            .set("system.cognition", false);
        return *this;
    }
    
    EnhancedConfig& enableLearning(bool enable = true) {
        set("system.learning", enable);
        return *this;
    }
    
    EnhancedConfig& enableDevelopment(bool enable = true) {
        set("system.development", enable);
        return *this;
    }
    
    EnhancedConfig& enableNeuromodulation(bool enable = true) {
        set("system.neuromodulation", enable);
        return *this;
    }
    
    EnhancedConfig& enablePrediction(bool enable = true) {
        set("system.prediction", enable);
        return *this;
    }
    
    EnhancedConfig& enableCognition(bool enable = true) {
        set("system.cognition", enable);
        return *this;
    }
    
    // Query configuration
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        return config->get<std::string>(key, defaultValue);
    }
    
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const {
        return config->get<int64_t>(key, defaultValue);
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        return config->get<double>(key, defaultValue);
    }
    
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        return config->get<float>(key, defaultValue);
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        return config->get<bool>(key, defaultValue);
    }
    
    // File operations
    bool loadFromFile(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
    // Configuration summary
    std::string summary() const {
        return config->summary();
    }
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
private:
    std::shared_ptr<Config> config;
};

// Enhanced brain interface with more comprehensive methods
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain(createBrain(config)) {}
    
    // Simplified interface for common operations
    void initialize() {
        brain->initialize();
    }
    
    void step(size_t steps = 1) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
    }
    
    // Enhanced statistics
    struct BrainStats {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float totalSpikes;
        float curiosityLevel;
        float noveltyLevel;
        std::string developmentalStage;
        size_t memoryUsage;
    };
    
    BrainStats getStats() const {
        BrainStats stats;
        stats.neuronCount = brain->getTotalNeuronCount();
        stats.synapseCount = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        stats.totalSpikes = brain->getTotalSpikeCount();
        return stats;
    }
    
    // Learning and development controls
    void enableLearning(bool enable = true) {
        // Implementation depends on brain interface
    }
    
    void enableDevelopment(bool enable = true) {
        // Implementation depends on brain interface
    }
    
    void enableNeuromodulation(bool enable = true) {
        // Implementation depends on brain interface
    }
    
    void setCuriosity(float level) {
        curiosityLevel_ = std::clamp(level, 0.0f, 1.0f);
    }
    
    void setLearningRate(float rate) {
        learningRate_ = std::clamp(rate, 0.0f, 1.0f);
    }
    
    // Checkpoint and persistence
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Experiment control
    void runExperiment(const std::string& type, size_t steps) {
        if (type == "basic") {
            runBasicExperiment(steps);
        } else if (type == "learning") {
            runLearningExperiment(steps);
        } else if (type == "plasticity") {
            runPlasticityExperiment(steps);
        } else if (type == "development") {
            runDevelopmentExperiment(steps);
        }
    }
    
    // Get underlying brain (advanced access)
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
private:
    std::shared_ptr<Brain> brain;
    float curiosityLevel_ = 0.5f;
    float learningRate_ = 0.001f;
    
    void runBasicExperiment(size_t steps) {
        // Implementation
    }
    
    void runLearningExperiment(size_t steps) {
        // Implementation
    }
    
    void runPlasticityExperiment(size_t steps) {
        // Implementation
    }
    
    void runDevelopmentExperiment(size_t steps) {
        // Implementation
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
};

// Enhanced simple world with more control
class EnhancedSimpleWorld {
public:
    EnhancedSimpleWorld() : world(createWorld()) {}
    
    // Configure world with advanced options
    EnhancedSimpleWorld& configure(size_t width, size_t height, 
                                   size_t visionWidth, size_t visionHeight,
                                   size_t maxObjects = 100,
                                   float energyDecayRate = 0.01f,
                                   float maxEnergy = 100.0f) {
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
        return *this;
    }
    
    // Environment setup
    EnhancedSimpleWorld& reset() {
        world->reset();
        return *this;
    }
    
    EnhancedSimpleWorld& setAgentStart(float x, float y, float orientation = 0.0f) {
        world->setAgentStart(x, y);
        world->getAgentBody()->orientation = orientation;
        return *this;
    }
    
    EnhancedSimpleWorld& setRandomSeed(uint64_t seed) {
        world->setRandomSeed(seed);
        return *this;
    }
    
    // World update with time control
    EnhancedSimpleWorld& update(double timestep, bool applyPhysics = true) {
        if (applyPhysics) {
            world->update(timestep);
        }
        return *this;
    }
    
    // Agent control
    EnhancedSimpleWorld& applyMotorCommand(const Action& command, double time) {
        world->applyMotorCommand(command, time);
        return *this;
    }
    
    // Environment objects
    EnhancedSimpleWorld& addResource(float x, float y, float value = 1.0f) {
        world->addObject(std::make_unique<WorldObject>(x, y, WorldObjectType::Resource, value));
        return *this;
    }
    
    EnhancedSimpleWorld& addHazard(float x, float y, float severity = 1.0f) {
        world->addObject(std::make_unique<WorldObject>(x, y, WorldObjectType::Hazard, severity));
        return *this;
    }
    
    // World query
    std::vector<float> getVision() const {
        auto percept = world->getSensoryPercept();
        return percept.getVision();
    }
    
    std::vector<float> getTouch() const {
        auto percept = world->getSensoryPercept();
        return percept.getTouch();
    }
    
    const AgentBody& getAgentBody() const {
        return *world->getAgentBody();
    }
    
    AgentBody& getAgentBody() {
        return *world->getAgentBody();
    }
    
    double getSimulationTime() const {
        return world->getSimulationTime();
    }
    
    // Enhanced agent brain interface
    class EnhancedAgentBrain {
    public:
        EnhancedAgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) 
            : agentBrain(createAgentBrain(brain, world)) {
        }
        
        EnhancedAgentBrain& initialize() {
            agentBrain->initialize(*world);
            return *this;
        }
        
        EnhancedAgentBrain& enableAllSystems() {
            agentBrain->enableRewardModulation(true)
                .enableStructuralPlasticity(true)
                .enableDevelopment(true)
                .enableCuriosity(true);
            return *this;
        }
        
        EnhancedAgentBrain& disableAllSystems() {
            agentBrain->enableRewardModulation(false)
                .enableStructuralPlasticity(false)
                .enableDevelopment(false)
                .enableCuriosity(false);
            return *this;
        }
        
        EnhancedAgentBrain& setCuriosity(float level) {
            curiosityLevel_ = std::clamp(level, 0.0f, 1.0f);
            return *this;
        }
        
        EnhancedAgentBrain& setLearningRate(float rate) {
            learningRate_ = std::clamp(rate, 0.0f, 0.1f);
            return *this;
        }
        
        // Control methods
        EnhancedAgentBrain& step(size_t steps = 1) {
            for (size_t i = 0; i < steps; ++i) {
                agentBrain->updateDevelopment(0.1);
            }
            return *this;
        }
        
        // Processing methods
        EnhancedAgentBrain& process(double timestep, bool autoAction = true) {
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            brain->step(static_cast<SimulationStep>(world->getSimulationTime()));
            
            if (autoAction) {
                Action action = agentBrain->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            return *this;
        }
        
        // Statistics and monitoring
        struct AgentStats {
            float curiosityLevel;
            float noveltyLevel;
            float predictionError;
            float dopamineLevel;
            std::string developmentalStage;
            size_t stepsExecuted;
        };
        
        AgentStats getStats() const {
            AgentStats stats;
            stats.curiosityLevel = agentBrain->getCuriosityLevel();
            stats.noveltyLevel = agentBrain->getNoveltyLevel();
            stats.predictionError = agentBrain->getPredictionError();
            stats.dopamineLevel = agentBrain->getNeuromodulationLevel();
            stats.developmentalStage = "TODO"; // Would need to implement
            stats.stepsExecuted = 0; // Would need to track
            return stats;
        }
        
        // Get underlying agent brain (advanced access)
        std::shared_ptr<AgentBrain> getAgentBrain() const {
            return agentBrain;
        }
        
    private:
        std::shared_ptr<AgentBrain> agentBrain;
        std::shared_ptr<Brain> brain;
        std::shared_ptr<SimpleWorld> world;
        float curiosityLevel_ = 0.5f;
        float learningRate_ = 0.001f;
        
        std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w) {
            auto agent = std::make_shared<AgentBrain>(b);
            agent->initialize(*w);
            return agent;
        }
    };
    
    EnhancedAgentBrain createAgentBrain() {
        return EnhancedAgentBrain(brain, world);
    }
    
private:
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<Brain> brain;
    
    std::shared_ptr<SimpleWorld> createWorld() {
        return std::make_shared<SimpleWorld>();
    }
};

// Convenience function to create enhanced configuration
EnhancedConfig createEnhancedConfig(const std::string& type = "basic") {
    if (type == "basic") return EnhancedConfig::createBasicConfig();
    else if (type == "advanced") return EnhancedConfig::createAdvancedConfig();
    else if (type == "minimal") return EnhancedConfig::createMinimalConfig();
    else return EnhancedConfig::createBasicConfig();
}

} // namespace nlm

#endif // PYNLM_ENHANCED_BINDINGS_H
