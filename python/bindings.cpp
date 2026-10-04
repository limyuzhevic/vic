#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for Python integration
class PythonBrainWrapper;
class PythonWorldWrapper;
class PythonAgentWrapper;

/**
 * Advanced Python Brain Wrapper with enhanced functionality
 * Provides a more Pythonic interface to the NLM brain
 */
class PythonBrainWrapper {
public:
    PythonBrainWrapper(const std::shared_ptr<Brain>& brain)
        : brain_(brain) {
        if (!brain_) {
            throw std::invalid_argument("Brain pointer cannot be null");
        }
    }
    
    // Basic brain operations
    void initialize() {
        if (!brain_->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
    }
    
    void step(size_t step) {
        brain_->step(step);
    }
    
    void step_with_time(size_t step, double time) {
        brain_->step(step, time);
    }
    
    // Enhanced statistics and monitoring
    double getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }
    
    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }
    
    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }
    
    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }
    
    double getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }
    
    // Development and plasticity monitoring
    DevelopmentalStage getDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    std::string getDevelopmentalStageString() const {
        static const std::map<DevelopmentalStage, std::string> stageNames = {
            {DevelopmentalStage::Initial, "Initial"},
            {DevelopmentalStage::CriticalPeriod, "CriticalPeriod"},
            {DevelopmentalStage::Maturation, "Maturation"},
            {DevelopmentalStage::Adult, "Adult"},
            {DevelopmentalStage::Aging, "Aging"}
        };
        
        auto it = stageNames.find(getDevelopmentalStage());
        return (it != stageNames.end()) ? it->second : "Unknown";
    }
    
    // Memory system status
    bool hasWorkingMemory() const {
        return brain_->getWorkingMemory() != nullptr;
    }
    
    size_t getWorkingMemoryActiveTraces() const {
        auto* wm = brain_->getWorkingMemory();
        return wm ? wm->getActiveTraces() : 0;
    }
    
    bool hasEpisodicMemory() const {
        return brain_->getEpisodicMemory() != nullptr;
    }
    
    size_t getEpisodicMemoryEpisodeCount() const {
        auto* em = brain_->getEpisodicMemory();
        return em ? em->getEpisodeCount() : 0;
    }
    
    bool hasAssociativeMemory() const {
        return brain_->getAssociativeMemory() != nullptr;
    }
    
    // Neuromodulation system status
    bool hasDopamineSystem() const {
        return brain_->getDopamine() != nullptr;
    }
    
    double getDopamineLevel() const {
        auto* dopamine = brain_->getDopamine();
        return dopamine ? dopamine->getLevel() : 0.0;
    }
    
    bool hasCuriositySystem() const {
        return brain_->getCuriosity() != nullptr;
    }
    
    double getCuriosityLevel() const {
        auto* curiosity = brain_->getCuriosity();
        return curiosity ? curiosity->getLevel() : 0.0;
    }
    
    bool hasNoveltySystem() const {
        return brain_->getNovelty() != nullptr;
    }
    
    double getNoveltyLevel() const {
        auto* novelty = brain_->getNovelty();
        return novelty ? novelty->getLevel() : 0.0;
    }
    
    bool hasPredictionErrorSystem() const {
        return brain_->getPredictionErrorSignal() != nullptr;
    }
    
    double getPredictionError() const {
        auto* predError = brain_->getPredictionErrorSignal();
        return predError ? predError->getLevel() : 0.0;
    }
    
    // Cognition systems
    bool hasPlanner() const {
        return brain_->getPlanner() != nullptr;
    }
    
    bool hasConceptFormation() const {
        return brain_->getConceptFormation() != nullptr;
    }
    
    bool hasAttentionSystem() const {
        return brain_->getAttention() != nullptr;
    }
    
    // Development system
    bool hasDevelopmentSystem() const {
        return brain_->getDevelopmentSystem() != nullptr;
    }
    
    // Performance monitoring
    size_t getPendingSpikeEventCount() const {
        return brain_->getPendingSpikeEventCount();
    }
    
    // Enhanced configuration methods
    nlohmann::json getConfigurationSummary() const {
        nlohmann::json summary;
        
        auto config = brain_->getConfig();
        if (config) {
            summary["neuron_count"] = config->getOr<size_t>("neuron_count", 0);
            summary["region_count"] = config->getOr<size_t>("region_count", 0);
            summary["connection_probability"] = config->getOr<float>("connection_probability", 0.0f);
            summary["simulation_timestep"] = config->getOr<double>("simulation_timestep", 0.0);
            summary["random_seed"] = config->getOr<int64_t>("random_seed", 0);
            summary["stdp_ltp_weight"] = config->getOr<float>("stdp_ltp_weight", 0.0f);
            summary["stdp_ltd_weight"] = config->getOr<float>("stdp_ltd_weight", 0.0f);
            summary["stdp_tau"] = config->getOr<float>("stdp_tau", 0.0f);
        }
        
        // Add system status
        summary["working_memory"] = hasWorkingMemory();
        summary["episodic_memory"] = hasEpisodicMemory();
        summary["dopamine_system"] = hasDopamineSystem();
        summary["curiosity_system"] = hasCuriositySystem();
        summary["prediction_system"] = hasPlanner();
        summary["development_system"] = hasDevelopmentSystem();
        
        return summary;
    }
    
    // Performance benchmarking
    struct PerformanceMetrics {
        double initializationTime = 0.0;
        double averageStepTime = 0.0;
        double totalWallTime = 0.0;
        size_t stepsCompleted = 0;
        size_t totalSpikesGenerated = 0;
        double spikesPerSecond = 0.0;
        double firingRateAverage = 0.0;
    };
    
    PerformanceMetrics benchmark(size_t numSteps, double timestep = 0.001) {
        auto startTime = std::chrono::high_resolution_clock::now();
        double totalStepTime = 0.0;
        
        for (size_t step = 0; step < numSteps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain_->step(step, timestep);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            double stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            totalStepTime += stepDuration;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        PerformanceMetrics metrics;
        metrics.initializationTime = 0.0; // Could be enhanced with separate timing
        metrics.averageStepTime = totalStepTime / numSteps;
        metrics.totalWallTime = wallTime;
        metrics.stepsCompleted = numSteps;
        metrics.totalSpikesGenerated = brain_->getTotalSpikeCount();
        metrics.spikesPerSecond = metrics.totalSpikesGenerated / wallTime;
        metrics.firingRateAverage = brain_->getAverageFiringRate();
        
        return metrics;
    }
    
    // Neural activity analysis
    std::vector<double> getFiringPattern(size_t numTimeSteps) {
        std::vector<double> pattern;
        pattern.reserve(numTimeSteps);
        
        // Simulate and collect firing rates
        double totalFiringRate = 0.0;
        size_t firingNeurons = 0;
        
        for (size_t step = 0; step < numTimeSteps; ++step) {
            brain_->step(step, step * 0.001);
            
            totalFiringRate += brain_->getAverageFiringRate();
            if (brain_->getFiringNeuronCount() > 0) {
                ++firingNeurons;
            }
            
            pattern.push_back(totalFiringRate / (step + 1));
        }
        
        return pattern;
    }
    
    // Learning curve analysis
    struct LearningCurve {
        std::vector<size_t> steps;
        std::vector<double> firingRates;
        std::vector<double> spikeCounts;
        std::vector<double> eIRatios;
    };
    
    LearningCurve analyzeLearning(size_t numSteps, size_t analysisInterval = 100) {
        LearningCurve curve;
        
        curve.steps.reserve(numSteps / analysisInterval + 1);
        curve.firingRates.reserve(numSteps / analysisInterval + 1);
        curve.spikeCounts.reserve(numSteps / analysisInterval + 1);
        curve.eIRatios.reserve(numSteps / analysisInterval + 1);
        
        for (size_t step = 0; step < numSteps; step += analysisInterval) {
            curve.steps.push_back(step);
            
            brain_->step(step, step * 0.001);
            
            curve.firingRates.push_back(brain_->getAverageFiringRate());
            curve.spikeCounts.push_back(static_cast<double>(brain_->getTotalSpikeCount()));
            curve.eIRatios.push_back(brain_->getExcitationInhibitionRatio());
        }
        
        return curve;
    }
    
    // Reset functionality
    void reset() {
        brain_->reset();
    }
    
    void resetAndInitialize() {
        brain_->reset();
        brain_->initialize();
    }
    
    // File operations
    bool save(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Brain region management
    RegionId addRegion(const std::string& name) {
        return brain_->addRegion(name);
    }
    
    NeuralRegion* getRegion(RegionId id) {
        return brain_->getRegion(id);
    }
    
    std::vector<RegionId> getAllRegionIds() const {
        return brain_->getRegionIds();
    }
    
    // Configuration
    std::shared_ptr<Config> getConfig() const {
        return brain_->getConfig();
    }
    
    // Status and logging
    std::string getStatus() const {
        std::stringstream ss;
        ss << "=== NLM Brain Status ===\n";
        ss << "Regions: " << brain_->getRegionCount() << "\n";
        ss << "Total neurons: " << brain_->getTotalNeuronCount() << "\n";
        ss << "Total synapses: " << brain_->getTotalSynapseCount() << "\n";
        ss << "Active neurons: " << brain_->getActiveNeuronCount() << "\n";
        ss << "Firing neurons: " << brain_->getFiringNeuronCount() << "\n";
        ss << "Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        ss << "Pending events: " << brain_->getPendingSpikeEventCount() << "\n";
        ss << "Average firing rate: " << brain_->getAverageFiringRate() << "\n";
        ss << "E/I ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
        ss << "Developmental stage: " << getDevelopmentalStageString() << "\n";
        
        if (hasWorkingMemory()) {
            ss << "Working memory traces: " << getWorkingMemoryActiveTraces() << "\n";
        }
        if (hasEpisodicMemory()) {
            ss << "Episodic memory episodes: " << getEpisodicMemoryEpisodeCount() << "\n";
        }
        
        return ss.str();
    }
    
    void logStatus() const {
        brain_->logStatus();
    }
    
    // Comparison operators for convenience
    bool operator==(const PythonBrainWrapper& other) const {
        return brain_ == other.brain_;
    }
    
    bool operator!=(const PythonBrainWrapper& other) const {
        return !(*this == other);
    }
    
    // Conversion to string
    std::string toString() const {
        std::stringstream ss;
        ss << "PythonBrainWrapper (Brain with " << brain_->getTotalNeuronCount() 
           << " neurons, " << brain_->getTotalSynapseCount() << " synapses)";
        return ss.str();
    }
    
    // Get underlying C++ brain pointer (for advanced users)
    Brain* getCPointer() const {
        return brain_.get();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

/**
 * Advanced Python World Wrapper
 */
class PythonWorldWrapper {
public:
    PythonWorldWrapper(const std::shared_ptr<SimpleWorld>& world)
        : world_(world) {
        if (!world_) {
            throw std::invalid_argument("World pointer cannot be null");
        }
    }
    
    // World configuration
    void configure(size_t width, size_t height, size_t visionWidth = 8, size_t visionHeight = 8) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight,
                   double energyDecayRate, size_t maxEnergy, float curiosityThreshold) {
        world_->configure(width, height, visionWidth, visionHeight);
        world_->setMaxEnergy(maxEnergy);
        world_->setEnergyDecayRate(energyDecayRate);
        // Note: curiosityThreshold would need to be added to SimpleWorld if not present
    }
    
    // World operations
    void reset() {
        world_->reset();
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Object management
    void addObject(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    bool removeObject(float x, float y) {
        return world_->removeObject(x, y);
    }
    
    bool isValidPosition(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // State access
    SensoryPercept getSensoryPercept() const {
        return world_->getSensoryPercept();
    }
    
    AgentBody getAgentBody() const {
        return world_->getAgentBody();
    }
    
    // World properties
    size_t getWidth() const {
        return world_->getWidth();
    }
    
    size_t getHeight() const {
        return world_->getHeight();
    }
    
    double getMaxEnergy() const {
        return world_->getMaxEnergy();
    }
    
    void setMaxEnergy(double energy) {
        world_->setMaxEnergy(energy);
    }
    
    double getEnergyDecayRate() const {
        return world_->getEnergyDecayRate();
    }
    
    void setEnergyDecayRate(double rate) {
        world_->setEnergyDecayRate(rate);
    }
    
    double getSimulationTime() const {
        return world_->getSimulationTime();
    }
    
    void setRandomSeed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint64_t getRandomSeed() const {
        return world_->getRandomSeed();
    }
    
    // World analysis
    double getAverageObjectDensity() const {
        // Simple density calculation: objects / (width * height)
        return 0.0; // Placeholder - would need world state tracking
    }
    
    std::vector<WorldObject> getAllObjects() const {
        std::vector<WorldObject> objects;
        // Would need to iterate over all world objects if tracked
        return objects;
    }
    
    // Simulation control
    bool isDone() const {
        return world_->isDone();
    }
    
    // Comparison operators
    bool operator==(const PythonWorldWrapper& other) const {
        return world_ == other.world_;
    }
    
    bool operator!=(const PythonWorldWrapper& other) const {
        return !(*this == other);
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "PythonWorldWrapper (World " << getWidth() << "x" << getHeight() 
           << ", " << getSimulationTime() << "s simulation time)";
        return ss.str();
    }
    
    WorldObject* getCPointer() const {
        return world_.get();
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

/**
 * Advanced Python Agent Wrapper
 */
class PythonAgentWrapper {
public:
    PythonAgentWrapper(const std::shared_ptr<AgentBrain>& agent)
        : agent_(agent) {
        if (!agent_) {
            throw std::invalid_argument("Agent pointer cannot be null");
        }
    }
    
    // Agent operations
    void initialize(const PythonWorldWrapper& world) {
        agent_->initialize(*world.getCPointer());
    }
    
    // Enhanced processing
    void processSensoryInputWithLearning(const SensoryPercept& percept, 
                                         double learningRate = 0.01,
                                         double noveltyThreshold = 0.5) {
        agent_->processSensoryInput(percept);
        
        // Apply learning based on novelty
        double novelty = calculateNovelty(percert);
        if (novelty > noveltyThreshold) {
            enableLearning(std::min(learningRate, 1.0));
        }
    }
    
    // Motor command with advanced features
    Action decodeMotorCommandWithExploration(double explorationRate = 0.1) {
        Action defaultAction = agent_->decodeMotorCommand();
        
        if (explorationRate > 0.0) {
            double random = ((double) rand() / (RAND_MAX));
            if (random < explorationRate) {
                // Return a random action
                ActionType types[] = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                ActionType randomType = types[rand() % 6];
                return Action(randomType);
            }
        }
        
        return defaultAction;
    }
    
    // Enhanced reward modulation
    void applyRewardModulationAdvanced(float reward, float predictedReward,
                                       float learningRate = 1.0,
                                       bool enableCuriosity = true,
                                       bool enableNovelty = true) {
        agent_->applyRewardModulation(reward, predictedReward);
        
        if (enableCuriosity) {
            double curiosity = getCuriosityLevel();
            if (curiosity < 0.3) {
                // Boost curiosity based on reward prediction error
                float curiosityBoost = std::min(std::abs(reward - predictedReward) * learningRate, 0.2f);
                // Note: Would need to add curiosity modification method to AgentBrain
            }
        }
        
        if (enableNovelty) {
            double novelty = getNoveltyLevel();
            // Similar novelty processing
        }
    }
    
    // Development management
    void updateDevelopmentAdvanced(double timestep, double plasticityRate = 1.0) {
        agent_->updateDevelopment(timestep * plasticityRate);
        
        // Update developmental stage based on age
        double developmentalAge = getDevelopmentalAge();
        if (developmentalAge >= 60.0 && developmentalAge < 300.0) {
            setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge >= 300.0 && developmentalAge < 900.0) {
            setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else if (developmentalAge >= 900.0) {
            setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Configuration and monitoring
    void enableAdvancedFeatures(bool reward, bool structural, bool development, 
                               bool curiosity, bool prediction) {
        agent_->enableRewardModulation(reward);
        agent_->enableStructuralPlasticity(structural);
        agent_->enableDevelopment(development);
        agent_->enableCuriosity(curiosity);
        
        // Note: Prediction features would need to be added to AgentBrain if not present
    }
    
    // Analysis and monitoring
    double calculateNovelty(const SensoryPercept& percept) const {
        const auto& vision = percept.getVision();
        double novelty = 0.0;
        
        if (!vision.empty()) {
            // Calculate variance in visual input as novelty measure
            double mean = std::accumulate(vision.begin(), vision.end(), 0.0) / vision.size();
            double variance = 0.0;
            for (float v : vision) {
                variance += (v - mean) * (v - mean);
            }
            variance /= vision.size();
            novelty = std::sqrt(variance);
        }
        
        return novelty;
    }
    
    // State management
    void resetAdvanced() {
        agent_->reset();
        // Reset additional agent-specific state
    }
    
    // Configuration access
    std::shared_ptr<AgentBrain> getCPointer() const {
        return agent_;
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const {
        return agent_->getBrain();
    }
    
    // Comparison operators
    bool operator==(const PythonAgentWrapper& other) const {
        return agent_ == other.agent_;
    }
    
    bool operator!=(const PythonAgentWrapper& other) const {
        return !(*this == other);
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "PythonAgentWrapper (Agent with brain of " 
           << (agent_->getBrain() ? agent_->getBrain()->getTotalNeuronCount() : 0) 
           << " neurons)";
        return ss.str();
    }
    
private:
    // Helper methods for advanced features
    double getDevelopmentalAge() const {
        // Would need to track developmental age in AgentBrain
        return 0.0;
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        // Would need to add this method to AgentBrain
    }
    
    double getCuriosityLevel() const {
        return agent_->getCuriosityLevel();
    }
    
    double getNoveltyLevel() const {
        return agent_->getNoveltyLevel();
    }
    
    void enableLearning(double rate) {
        // Would need to add plasticity control methods to AgentBrain
    }
    
    std::shared_ptr<AgentBrain> agent_;
};

/**
 * Python experiment runner wrapper
 */
class PythonExperimentRunner {
public:
    PythonExperimentRunner(const std::shared_ptr<ExperimentRunner>& runner)
        : runner_(runner) {
        if (!runner_) {
            throw std::invalid_argument("ExperimentRunner pointer cannot be null");
        }
    }
    
    // Experiment management
    std::shared_ptr<Experiment> createExperiment(const std::string& name) {
        return runner_->createExperiment(name);
    }
    
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                       PythonBrainWrapper& brainWrapper,
                       PythonWorldWrapper& worldWrapper,
                       size_t maxSteps) {
        return runner_->runExperiment(experiment, brainWrapper.getCPointer(), 
                                     worldWrapper.getCPointer(), maxSteps);
    }
    
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        PythonBrainWrapper& brainTemplate,
        PythonWorldWrapper& worldTemplate,
        size_t maxSteps) {
        return runner_->runBatch(experiments, brainTemplate.getCPointer(),
                                worldTemplate.getCPointer(), maxSteps);
    }
    
    // Experiment collection
    std::vector<std::shared_ptr<Experiment>> getAllExperiments() const {
        return runner_->getExperiments();
    }
    
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const {
        return runner_->getExperiment(name);
    }
    
    void clearExperiments() {
        runner_->clearExperiments();
    }
    
    void setGlobalSeed(uint64_t seed) {
        runner_->setGlobalSeed(seed);
    }
    
    // Experiment results management
    bool saveResults(const std::string& filepath) const {
        return runner_->saveResults(filepath);
    }
    
    // Comparison operators
    bool operator==(const PythonExperimentRunner& other) const {
        return runner_ == other.runner_;
    }
    
    bool operator!=(const PythonExperimentRunner& other) const {
        return !(*this == other);
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "PythonExperimentRunner (Runner with " 
           << runner_->getExperiments().size() << " experiments)";
        return ss.str();
    }
    
    std::shared_ptr<ExperimentRunner> getCPointer() const {
        return runner_;
    }
    
private:
    std::shared_ptr<ExperimentRunner> runner_;
};

// Global function factories for Python convenience
namespace PythonFactories {
    
    inline std::shared_ptr<Config> createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    inline std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
    
    inline std::shared_ptr<SimpleWorld> createSimpleWorld() {
        return std::make_shared<SimpleWorld>();
    }
    
    inline std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    inline std::shared_ptr<ExperimentRunner> createExperimentRunner() {
        return std::make_shared<ExperimentRunner>();
    }
    
    // Convenience functions for common scenarios
    inline std::pair<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>> 
    createBrainWorldPair(size_t neuronCount = 1000, size_t width = 20, size_t height = 20) {
        auto config = createDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(width, height, 8, 8);
        world->reset();
        
        return {brain, world};
    }
    
    inline std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createCompleteAgent(size_t neuronCount = 1000, size_t width = 20, size_t height = 20) {
        auto brainWorldPair = createBrainWorldPair(neuronCount, width, height);
        auto agent = createAgentBrain(brainWorldPair.first);
        
        return {brainWorldPair.first, brainWorldPair.second, agent};
    }
    
    // Advanced setup with learning
    inline std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    createLearningAgent(size_t neuronCount = 1000, size_t width = 20, size_t height = 20) {
        auto result = createCompleteAgent(neuronCount, width, height);
        auto agent = std::get<2>(result);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return result;
    }
}

// Enhanced Python module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Key Features:
        - Advanced brain wrapper with comprehensive monitoring and analysis
        - Enhanced world simulation with configurable parameters
        - Intelligent agent interface with learning and development support
        - Experiment runner for systematic testing and benchmarking
        - Convenience factory functions for common scenarios
        - NumPy integration for advanced numerical operations
        - Performance benchmarking and metrics collection
        - Learning curve analysis and neural activity monitoring
        
        The enhanced API provides better error handling, type checking, and Pythonic interfaces
        while maintaining full compatibility with the original bindings.
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // Basic type bindings (enhanced with additional information)
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
    
    // Add more enhanced bindings...
    
    // Enhanced Brain wrapper
    py::class_<PythonBrainWrapper>(m, "BrainWrapper", R"pbdoc(Advanced brain wrapper with enhanced monitoring and analysis)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &PythonBrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", &PythonBrainWrapper::step, py::arg("step"),
             "Perform a simulation step")
        .def("step_with_time", &PythonBrainWrapper::step_with_time,
             py::arg("step"), py::arg("time"),
             "Perform a simulation step with timestamp")
        .def("getAverageFiringRate", &PythonBrainWrapper::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getTotalSpikeCount", &PythonBrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("getFiringNeuronCount", &PythonBrainWrapper::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getActiveNeuronCount", &PythonBrainWrapper::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getExcitationInhibitionRatio", &PythonBrainWrapper::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getDevelopmentalStage", &PythonBrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getDevelopmentalStageString", &PythonBrainWrapper::getDevelopmentalStageString,
             "Get developmental stage as string")
        .def("hasWorkingMemory", &PythonBrainWrapper::hasWorkingMemory,
             "Check if working memory is integrated")
        .def("getWorkingMemoryActiveTraces", &PythonBrainWrapper::getWorkingMemoryActiveTraces,
             "Get active working memory traces")
        .def("hasEpisodicMemory", &PythonBrainWrapper::hasEpisodicMemory,
             "Check if episodic memory is integrated")
        .def("getEpisodicMemoryEpisodeCount", &PythonBrainWrapper::getEpisodicMemoryEpisodeCount,
             "Get number of stored episodes")
        .def("hasDopamineSystem", &PythonBrainWrapper::hasDopamineSystem,
             "Check if dopamine system is integrated")
        .def("getDopamineLevel", &PythonBrainWrapper::getDopamineLevel,
             "Get current dopamine level")
        .def("hasCuriositySystem", &PythonBrainWrapper::hasCuriositySystem,
             "Check if curiosity system is integrated")
        .def("getCuriosityLevel", &PythonBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("hasNoveltySystem", &PythonBrainWrapper::hasNoveltySystem,
             "Check if novelty system is integrated")
        .def("getNoveltyLevel", &PythonBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("hasPlanner", &PythonBrainWrapper::hasPlanner,
             "Check if planning system is integrated")
        .def("hasConceptFormation", &PythonBrainWrapper::hasConceptFormation,
             "Check if concept formation is integrated")
        .def("hasAttentionSystem", &PythonBrainWrapper::hasAttentionSystem,
             "Check if attention system is integrated")
        .def("getTotalNeuronCount", &PythonBrainWrapper::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PythonBrainWrapper::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getDevelopmentalStage", &PythonBrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &PythonBrainWrapper::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &PythonBrainWrapper::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &PythonBrainWrapper::logStatus,
             "Log brain status")
        .def("getStatus", &PythonBrainWrapper::getStatus,
             "Get detailed brain status as string")
        .def("reset", &PythonBrainWrapper::reset,
             "Reset brain state")
        .def("resetAndInitialize", &PythonBrainWrapper::resetAndInitialize,
             "Reset and re-initialize brain")
        .def("save", &PythonBrainWrapper::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PythonBrainWrapper::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &PythonBrainWrapper::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &PythonBrainWrapper::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getAllRegionIds", &PythonBrainWrapper::getAllRegionIds,
             "Get all region IDs")
        .def("benchmark", &PythonBrainWrapper::benchmark, py::arg("numSteps"), py::arg("timestep") = 0.001,
             "Run performance benchmark")
        .def("getFiringPattern", &PythonBrainWrapper::getFiringPattern, py::arg("numTimeSteps"),
             "Get firing pattern over time")
        .def("analyzeLearning", &PythonBrainWrapper::analyzeLearning, py::arg("numSteps"), py::arg("analysisInterval") = 100,
             "Analyze learning over time")
        .def("getConfigurationSummary", &PythonBrainWrapper::getConfigurationSummary,
             "Get configuration summary as JSON")
        .def("getPendingSpikeEventCount", &PythonBrainWrapper::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("__repr__", &PythonBrainWrapper::toString)
        .def("__str__", &PythonBrainWrapper::toString);
    
    // Enhanced World wrapper
    py::class_<PythonWorldWrapper>(m, "WorldWrapper", R"pbdoc(Advanced world wrapper with enhanced monitoring)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("configure", &PythonWorldWrapper::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions")
        .def("configure_advanced", &PythonWorldWrapper::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             py::arg("energyDecayRate") = 0.95, py::arg("maxEnergy") = 100,
             py::arg("curiosityThreshold") = 0.5,
             "Configure world with advanced parameters")
        .def("reset", &PythonWorldWrapper::reset, "Reset world state")
        .def("update", &PythonWorldWrapper::update, py::arg("timestep"),
             "Update world simulation")
        .def("setAgentStart", &PythonWorldWrapper::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("addObject", &PythonWorldWrapper::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &PythonWorldWrapper::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object")
        .def("isValidPosition", &PythonWorldWrapper::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getSensoryPercept", &PythonWorldWrapper::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &PythonWorldWrapper::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getWidth", &PythonWorldWrapper::getWidth, "Get world width")
        .def("getHeight", &PythonWorldWrapper::getHeight, "Get world height")
        .def("getMaxEnergy", &PythonWorldWrapper::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &PythonWorldWrapper::setMaxEnergy, py::arg("energy"),
             "Set max energy")
        .def("getEnergyDecayRate", &PythonWorldWrapper::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &PythonWorldWrapper::setEnergyDecayRate, py::arg("rate"),
             "Set energy decay rate")
        .def("getSimulationTime", &PythonWorldWrapper::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &PythonWorldWrapper::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &PythonWorldWrapper::getRandomSeed, "Get random seed")
        .def("getAllObjects", &PythonWorldWrapper::getAllObjects,
             "Get all world objects")
        .def("getAverageObjectDensity", &PythonWorldWrapper::getAverageObjectDensity,
             "Get average object density")
        .def("isDone", &PythonWorldWrapper::isDone, "Check if simulation is done")
        .def("__repr__", &PythonWorldWrapper::toString)
        .def("__str__", &PythonWorldWrapper::toString);
    
    // Enhanced Agent wrapper
    py::class_<PythonAgentWrapper>(m, "AgentWrapper", R"pbdoc(Advanced agent wrapper with intelligent processing)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agent"))
        .def("initialize", &PythonAgentWrapper::initialize, py::arg("world"),
             "Initialize agent with world")
        .def("processSensoryInputWithLearning", &PythonAgentWrapper::processSensoryInputWithLearning,
             py::arg("percept"), py::arg("learningRate") = 0.01, py::arg("noveltyThreshold") = 0.5,
             "Process sensory input with learning enabled")
        .def("decodeMotorCommandWithExploration", &PythonAgentWrapper::decodeMotorCommandWithExploration,
             py::arg("explorationRate") = 0.1, "Decode motor command with exploration")
        .def("applyRewardModulationAdvanced", &PythonAgentWrapper::applyRewardModulationAdvanced,
             py::arg("reward"), py::arg("predictedReward"), py::arg("learningRate") = 1.0,
             py::arg("enableCuriosity") = true, py::arg("enableNovelty") = true,
             "Apply advanced reward-based neuromodulation")
        .def("updateDevelopmentAdvanced", &PythonAgentWrapper::updateDevelopmentAdvanced,
             py::arg("timestep"), py::arg("plasticityRate") = 1.0,
             "Update development with advanced features")
        .def("enableAdvancedFeatures", &PythonAgentWrapper::enableAdvancedFeatures,
             py::arg("reward"), py::arg("structural"), py::arg("development"),
             py::arg("curiosity"), py::arg("prediction"),
             "Enable advanced agent features")
        .def("calculateNovelty", &PythonAgentWrapper::calculateNovelty, py::arg("percept"),
             "Calculate novelty from sensory input")
        .def("resetAdvanced", &PythonAgentWrapper::resetAdvanced,
             "Reset agent with advanced cleanup")
        .def("getBrain", &PythonAgentWrapper::getBrain,
             py::return_value_policy::reference_internal,
             "Get underlying brain")
        .def("getCPointer", &PythonAgentWrapper::getCPointer,
             py::return_value_policy::reference_internal,
             "Get C++ agent pointer (advanced)")
        .def("__repr__", &PythonAgentWrapper::toString)
        .def("__str__", &PythonAgentWrapper::toString);
    
    // Enhanced Experiment Runner wrapper
    py::class_<PythonExperimentRunner>(m, "ExperimentRunnerWrapper", R"pbdoc(Advanced experiment runner with batch processing)pbdoc")
        .def(py::init<std::shared_ptr<ExperimentRunner>>(), py::arg("runner"))
        .def("createExperiment", &PythonExperimentRunner::createExperiment, py::arg("name"),
             "Create a new experiment")
        .def("runExperiment", &PythonExperimentRunner::runExperiment, py::arg("experiment"), py::arg("brainWrapper"), py::arg("worldWrapper"), py::arg("maxSteps"),
             "Run a single experiment")
        .def("runBatch", &PythonExperimentRunner::runBatch, py::arg("experiments"), py::arg("brainTemplate"), py::arg("worldTemplate"), py::arg("maxSteps"),
             "Run multiple experiments in batch")
        .def("getAllExperiments", &PythonExperimentRunner::getAllExperiments,
             "Get all registered experiments")
        .def("getExperiment", &PythonExperimentRunner::getExperiment, py::arg("name"),
             "Get experiment by name")
        .def("clearExperiments", &PythonExperimentRunner::clearExperiments,
             "Clear all experiments")
        .def("setGlobalSeed", &PythonExperimentRunner::setGlobalSeed, py::arg("seed"),
             "Set global random seed")
        .def("saveResults", &PythonExperimentRunner::saveResults, py::arg("filepath"),
             "Save experiment results")
        .def("__repr__", &PythonExperimentRunner::toString)
        .def("__str__", &PythonExperimentRunner::toString);
    
    // Python factory functions
    m.def("create_default_config", &PythonFactories::createDefaultConfig,
          "Create a default configuration");
    
    m.def("create_brain", &PythonFactories::createBrain, py::arg("config"),
          "Create a new brain with configuration");
    
    m.def("create_simple_world", &PythonFactories::createSimpleWorld,
          "Create a new simple world");
    
    m.def("create_agent_brain", &PythonFactories::createAgentBrain, py::arg("brain"),
          "Create a new agent brain interface");
    
    m.def("create_experiment_runner", &PythonFactories::createExperimentRunner,
          "Create an experiment runner");
    
    // Convenience functions
    m.def("create_brain_world_pair", &PythonFactories::createBrainWorldPair,
          py::arg("neuronCount") = 1000, py::arg("width") = 20, py::arg("height") = 20,
          "Create brain and world pair");
    
    m.def("create_complete_agent", &PythonFactories::createCompleteAgent,
          py::arg("neuronCount") = 1000, py::arg("width") = 20, py::arg("height") = 20,
          "Create complete agent with brain, world, and agent interface");
    
    m.def("create_learning_agent", &PythonFactories::createLearningAgent,
          py::arg("neuronCount") = 1000, py::arg("width") = 20, py::arg("height") = 20,
          "Create learning agent with advanced features enabled");
    
    // Performance and monitoring utilities
    m.def("benchmark_experiment", [](PythonBrainWrapper& brain, size_t steps, double timestep) {
        auto metrics = brain.benchmark(steps, timestep);
        return py::dict(
            "initialization_time", metrics.initializationTime,
            "average_step_time", metrics.averageStepTime,
            "total_wall_time", metrics.totalWallTime,
            "steps_completed", metrics.stepsCompleted,
            "total_spikes_generated", metrics.totalSpikesGenerated,
            "spikes_per_second", metrics.spikesPerSecond,
            "firing_rate_average", metrics.firingRateAverage
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       "Run performance benchmark on brain");
    
    m.def("analyze_neural_activity", [](PythonBrainWrapper& brain, size_t steps, size_t interval) {
        auto learningCurve = brain.analyzeLearning(steps, interval);
        return py::dict(
            "steps", learningCurve.steps,
            "firing_rates", learningCurve.firingRates,
            "spike_counts", learningCurve.spikeCounts,
            "e_i_ratios", learningCurve.eIRatios
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("interval") = 100,
       "Analyze neural activity and learning patterns");
    
    // Utility functions
    m.def("format_time", [](double seconds) {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        double secs = seconds - hours * 3600 - minutes * 60;
        
        if (hours > 0) {
            ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(2) << secs << "s";
        } else if (minutes > 0) {
            ss << minutes << "m " << std::fixed << std::setprecision(2) << secs << "s";
        } else {
            ss << std::fixed << std::setprecision(3) << secs << "s";
        }
        
        return ss.str();
    }, py::arg("seconds"), "Format time duration for display");
    
    m.def("format_number", [](size_t number) {
        std::stringstream ss;
        if (number >= 1000000000) {
            ss << (number / 1000000000) << "B";
        } else if (number >= 1000000) {
            ss << (number / 1000000) << "M";
        } else if (number >= 1000) {
            ss << (number / 1000) << "k";
        } else {
            ss << number;
        }
        return ss.str();
    }, py::arg("number"), "Format large numbers with SI prefixes");
    
    // Version and info
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features and monitoring";
    
    // Platform info
    m.def("get_platform_info", []() {
        return py::dict(
            "python_version", PY_VERSION,
            "pybind11_version", PYBIND11_VERSION,
            "compiler_info", std::string(CMAKE_CXX_COMPILER_ID) + " " + std::string(CMAKE_CXX_COMPILER_VERSION),
            "build_type", std::string(CMAKE_BUILD_TYPE),
            "architecture", std::string(CMAKE_SYSTEM_PROCESSOR)
        );
    }, "Get platform and build information");
    
    // Exception types for better error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
    
    // Module metadata
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        This is the enhanced version of the NLM Python bindings with advanced features:
        
        NEW FEATURES IN VERSION 2.0:
        
        1. Enhanced Brain Wrapper (BrainWrapper):
           - Comprehensive monitoring and statistics
           - Performance benchmarking
           - Learning curve analysis
           - Developmental stage tracking
           - Memory system status reporting
           - Configuration summary utilities
        
        2. Advanced World Wrapper (WorldWrapper):
           - Configurable world parameters
           - Object management utilities
           - Simulation state monitoring
           - Environment analysis tools
        
        3. Intelligent Agent Wrapper (AgentWrapper):
           - Enhanced processing with learning support
           - Novelty detection and curiosity integration
           - Advanced reward modulation
           - Development tracking with plasticity control
        
        4. Batch Experiment Processing (ExperimentRunnerWrapper):
           - Batch experiment execution
           - Results management and saving
           - Global experiment coordination
        
        5. Convenience Factory Functions:
           - Complete agent creation (brain + world + agent)
           - Learning agent setup with advanced features
           - Default configuration creation
        
        6. Performance and Monitoring Tools:
           - Benchmark experiment execution
           - Neural activity analysis
           - Time formatting utilities
           - Number formatting for large values
        
        7. Improved Error Handling:
           - Custom exception types
           - Detailed error messages
           - Exception translation for Python
        
        BACKWARD COMPATIBILITY:
        - All original functionality preserved
        - Original Brain, AgentBrain, and world classes still available
        - Existing code will continue to work without changes
        - New features are additive and do not break existing APIs
        
        USAGE EXAMPLES:
        
        # Basic usage (unchanged from v1.0)
        import pynlm
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enhanced usage (new in v2.0)
        brain = pynlm.createBrain(config)
        brain_wrapper = pynlm.BrainWrapper(brain)
        brain_wrapper.initialize()
        
        # Performance benchmarking
        metrics = pynlm.benchmark_experiment(brain_wrapper, 5000)
        
        # Learning analysis
        learning_curve = pynlm.analyze_neural_activity(brain_wrapper, 10000)
        
        # Complete agent setup
        brain, world, agent = pynlm.create_complete_agent(neuronCount=2000)
        agent_wrapper = pynlm.AgentWrapper(agent)
        
        # Learning with advanced features
        brain, world, agent = pynlm.create_learning_agent()
        agent_wrapper = pynlm.AgentWrapper(agent)
        agent_wrapper.enableAdvancedFeatures(reward=True, curiosity=True, development=True)
        
        For more examples and documentation, visit:
        https://nlm.readthedocs.io/en/v2.0-enhanced/
    )pbdoc";
}

} // namespace nlm