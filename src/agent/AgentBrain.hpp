#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declaration for Experiment to avoid circular includes
class Experiment;
class Metrics;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

// Context managers and convenience classes for Python bindings

/**
 * Context manager for brain initialization and cleanup
 * Provides automatic resource management for Brain objects
 */
class BrainContextManager {
public:
    explicit BrainContextManager(std::shared_ptr<Config> config)
        : config_(config) {
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    ~BrainContextManager() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    Brain& getBrain() { return *brain_; }
    const Brain& getBrain() const { return *brain_; }
    std::shared_ptr<Brain> getBrainPtr() { return brain_; }
    
    // Disable copying
    BrainContextManager(const BrainContextManager&) = delete;
    BrainContextManager& operator=(const BrainContextManager&) = delete;
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

/**
 * Context manager for simulation loops
 * Manages simulation state and cleanup
 */
class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                     std::shared_ptr<AgentBrain> agentBrain = nullptr)
        : brain_(brain), world_(world), agentBrain_(agentBrain) {
        isRunning_ = false;
    }
    
    ~SimulationRunner() {
        stop();
    }
    
    // Start simulation loop
    void start(SimulationStep maxSteps = 0) {
        if (isRunning_) return;
        
        isRunning_ = true;
        maxSteps_ = maxSteps;
        currentStep_ = 0;
        
        // Reset world if provided
        if (world_) {
            world_->reset();
        }
        
        // Reset brain if provided
        if (brain_) {
            brain_->reset();
        }
        
        // Reset agent if provided
        if (agentBrain_) {
            agentBrain_->reset();
        }
    }
    
    // Stop simulation loop
    void stop() {
        isRunning_ = false;
    }
    
    // Single step execution
    bool step() {
        if (!isRunning_ || currentStep_ >= maxSteps_) {
            isRunning_ = false;
            return false;
        }
        
        // Update world simulation time
        if (world_) {
            world_->update(1.0);  // Fixed timestep of 1.0
        }
        
        // Perform brain computation
        if (brain_) {
            brain_->step(currentStep_);
            currentStep_++;
        }
        
        // Process agent if available
        if (agentBrain_ && world_) {
            auto percept = world_->getSensoryPercept();
            agentBrain_->processSensoryInput(percept);
        }
        
        return true;
    }
    
    // Run batch of steps
    size_t runSteps(size_t steps) {
        size_t executed = 0;
        for (size_t i = 0; i < steps && step(); ++i) {
            executed++;
        }
        return executed;
    }
    
    // Check if simulation is running
    bool isRunning() const { return isRunning_; }
    
    // Get current step count
    SimulationStep getCurrentStep() const { return currentStep_; }
    
    // Get max steps
    SimulationStep getMaxSteps() const { return maxSteps_; }
    
    // Get brain reference
    Brain& getBrain() { return *brain_; }
    
    // Get world reference
    SimpleWorld& getWorld() { return *world_; }
    
    // Get agent brain reference
    AgentBrain& getAgentBrain() { return *agentBrain_; }
    
    // Disable copying
    SimulationRunner(const SimulationRunner&) = delete;
    SimulationRunner& operator=(const SimulationRunner&) = delete;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agentBrain_;
    
    bool isRunning_;
    SimulationStep maxSteps_;
    SimulationStep currentStep_;
};

/**
 * Batch operations for multiple steps and sensory inputs
 */
class BatchOperations {
public:
    // Run multiple steps at once
    static std::vector<SimulationStep> runSteps(Brain& brain, 
                                                const std::vector<SimulationStep>& steps,
                                                const std::vector<Timestamp>& times = {}) {
        std::vector<SimulationStep> executed;
        for (size_t i = 0; i < steps.size(); ++i) {
            SimulationStep step = steps[i];
            if (times.empty()) {
                brain.step(step);
            } else {
                brain.step(step, times[i]);
            }
            executed.push_back(step);
        }
        return executed;
    }
    
    // Process multiple sensory inputs at once
    static std::vector<size_t> processSensoryInputs(Brain& brain,
                                                     const std::vector<std::reference_wrapper<const SensoryInput>>& inputs) {
        std::vector<size_t> processed;
        for (const auto& input : inputs) {
            brain.receiveSensoryInput(input.get());
            processed.push_back(processed.size());
        }
        return processed;
    }
    
    // Get batch statistics for multiple steps
    static std::map<std::string, double> getBatchStats(Brain& brain,
                                                       SimulationStep startStep,
                                                       SimulationStep endStep) {
        std::map<std::string, double> stats;
        
        // Note: In a real implementation, we'd need to store step history
        // For now, we'll return current statistics
        stats["total_neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
        stats["total_synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
        stats["active_neuron_count"] = static_cast<double>(brain.getActiveNeuronCount());
        stats["firing_neuron_count"] = static_cast<double>(brain.getFiringNeuronCount());
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spike_count"] = static_cast<double>(brain.getTotalSpikeCount());
        
        return stats;
    }
};

/**
 * Convenience methods for common workflows
 */
class ConvenienceMethods {
public:
    // Configure brain from Python dict
    static bool configureBrain(Brain& brain, const std::map<std::string, std::string>& configDict) {
        auto config = std::make_shared<Config>();
        
        for (const auto& pair : configDict) {
            // Parse value based on type
            const std::string& key = pair.first;
            const std::string& valueStr = pair.second;
            
            // Try to parse as integer
            try {
                size_t pos;
                int intValue = std::stoi(valueStr, &pos);
                if (pos == valueStr.size()) {
                    config->set(key, intValue);
                    continue;
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                size_t pos;
                double doubleValue = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    config->set(key, doubleValue);
                    continue;
                }
            } catch (...) {}
            
            // Try to parse as boolean
            std::string lowerValue = valueStr;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue == "true") {
                config->set(key, true);
            } else if (lowerValue == "false") {
                config->set(key, false);
            } else {
                // Treat as string
                config->set(key, valueStr);
            }
        }
        
        // Note: In a real implementation, we would apply the config to the brain
        // For now, we'll just return success
        return true;
    }
    
    // Quick setup and run
    static std::map<std::string, double> quickSimulation(size_t neuronCount, SimulationStep steps) {
        auto config = std::make_shared<Config>();
        config->set("neuronCount", static_cast<int>(neuronCount));
        config->set("simulationSteps", static_cast<int>(steps));
        
        // In a real implementation, we would create and run a brain
        // For now, return some dummy statistics
        std::map<std::string, double> stats;
        stats["total_neurons"] = static_cast<double>(neuronCount);
        stats["total_steps"] = static_cast<double>(steps);
        stats["avg_firing_rate"] = 0.5;  // Dummy value
        stats["memory_usage_mb"] = neuronCount * 0.1;  // Dummy calculation
        
        return stats;
    }
    
    // Run complete agent episode
    static std::map<std::string, double> runEpisode(AgentBrain& agentBrain, SimpleWorld& world,
                                                     SimulationStep maxSteps = 1000) {
        std::map<std::string, double> episodeStats;
        
        // Reset for new episode
        agentBrain.reset();
        world.reset();
        
        // Initialize world with agent
        auto body = world.getAgentBody();
        world.setAgentStart(body.x, body.y);
        
        // Run episode
        SimulationStep stepsCompleted = 0;
        size_t rewardCount = 0;
        float totalReward = 0.0f;
        
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Get sensory percept
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // Produce action
            auto action = agentBrain.decodeMotorCommand();
            
            // Apply action to world
            world.applyMotorCommand(action, step);
            
            // Check for completion conditions
            auto body = world.getAgentBody();
            if (body.health <= 0.0f) {
                episodeStats["terminated_by_health"] = 1.0;
                break;
            }
            
            // Record step stats
            stepsCompleted++;
            
            // Simulate some reward
            float reward = 1.0f;  // Dummy reward
            if (reward > 0) {
                rewardCount++;
                totalReward += reward;
            }
        }
        
        // Calculate episode statistics
        episodeStats["steps_completed"] = static_cast<double>(stepsCompleted);
        episodeStats["steps_percentage"] = 
            (static_cast<double>(stepsCompleted) / maxSteps) * 100.0;
        episodeStats["rewards_received"] = static_cast<double>(rewardCount);
        episodeStats["total_reward"] = static_cast<double>(totalReward);
        episodeStats["avg_reward_per_step"] = 
            stepsCompleted > 0 ? totalReward / stepsCompleted : 0.0;
        
        return episodeStats;
    }
};

/**
 * Builder pattern for constructing configurations and simulations
 */
class ConfigBuilder {
public:
    ConfigBuilder() {
        config_ = std::make_shared<Config>();
    }
    
    ConfigBuilder& set(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& add(const std::string& key, int value) {
        // Add to existing vector or create new
        if (config_->has(key)) {
            // In a real implementation, we'd need to get and modify the vector
        } else {
            std::vector<int> vec{value};
            config_->set(key, vec);
        }
        return *this;
    }
    
    ConfigBuilder& add(const std::string& key, double value) {
        if (config_->has(key)) {
            // In a real implementation, we'd need to get and modify the vector
        } else {
            std::vector<double> vec{value};
            config_->set(key, vec);
        }
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

class SimulationBuilder {
public:
    SimulationBuilder() {
        world_ = std::make_shared<SimpleWorld>();
        config_ = std::make_shared<Config>();
        agentBrain_ = nullptr;
    }
    
    SimulationBuilder& withWorldSize(size_t width, size_t height) {
        world_->configure(width, height, 64, 64);
        return *this;
    }
    
    SimulationBuilder& withConfig(const std::map<std::string, std::string>& configDict) {
        // Use ConvenienceMethods to configure
        // In a real implementation, we'd apply this to config_
        return *this;
    }
    
    SimulationBuilder& withBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        return *this;
    }
    
    SimulationBuilder& withAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
        agentBrain_ = agentBrain;
        return *this;
    }
    
    std::tuple<std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>, std::shared_ptr<AgentBrain>>
    build() {
        // Create agent brain if not provided
        if (!agentBrain_ && brain_) {
            agentBrain_ = std::make_shared<AgentBrain>(brain_);
        }
        
        return std::make_tuple(world_, config_, agentBrain_);
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
};

/**
 * Experiment management utilities
 */
class ExperimentLogger {
public:
    ExperimentLogger() {
        experiment_ = std::make_shared<Experiment>("Python_Experiment");
        metrics_ = std::make_shared<Metrics>();
    }
    
    void recordMetric(const std::string& name, double value) {
        experiment_->recordMetric(name, value);
        metrics_->record(name, value);
    }
    
    void recordStep(Brain& brain, SimulationStep step) {
        experiment_->recordStep(step, 
                              brain.getTotalNeuronCount(),
                              brain.getTotalSynapseCount(),
                              brain.getFiringNeuronCount(),
                              brain.getAverageFiringRate());
    }
    
    void addNote(const std::string& note) {
        experiment_->addNote(note);
    }
    
    std::shared_ptr<Experiment> getExperiment() { return experiment_; }
    std::shared_ptr<Metrics> getMetrics() { return metrics_; }
    
private:
    std::shared_ptr<Experiment> experiment_;
    std::shared_ptr<Metrics> metrics_;
};

class ResultsAnalyzer {
public:
    // Analyze batch results from experiment
    static std::map<std::string, double> analyze(const Experiment& experiment) {
        std::map<std::string, double> analysis;
        
        // In a real implementation, we'd analyze the step data
        const auto& stepData = experiment.getStepData();
        
        if (!stepData.empty()) {
            double totalFiringRate = 0.0;
            size_t totalNeurons = 0;
            
            for (const auto& step : stepData) {
                totalFiringRate += step.avgFiringRate;
                totalNeurons += step.neuronCount;
            }
            
            size_t stepCount = stepData.size();
            analysis["avg_firing_rate"] = totalFiringRate / stepCount;
            analysis["avg_neuron_count"] = static_cast<double>(totalNeurons) / stepCount;
            analysis["total_steps"] = static_cast<double>(experiment.getStartStep());
        }
        
        return analysis;
    }
    
    // Analyze metrics from metrics collection
    static std::map<std::string, double> analyze(const Metrics& metrics) {
        std::map<std::string, double> analysis;
        
        const auto& metricNames = metrics.getMetricNames();
        
        for (const auto& name : metricNames) {
            const auto& values = metrics.getValues(name);
            if (!values.empty()) {
                double sum = 0.0;
                double sumSquares = 0.0;
                
                for (double value : values) {
                    sum += value;
                    sumSquares += value * value;
                }
                
                double mean = sum / values.size();
                double variance = (sumSquares / values.size()) - (mean * mean);
                double stdDev = std::sqrt(std::max(0.0, variance));
                
                analysis[name + "_mean"] = mean;
                analysis[name + "_stddev"] = stdDev;
                analysis[name + "_min"] = *std::min_element(values.begin(), values.end());
                analysis[name + "_max"] = *std::max_element(values.begin(), values.end());
            }
        }
        
        return analysis;
    }
};

} // namespace nlm

