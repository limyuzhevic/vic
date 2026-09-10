#include "EnhancedAgentBrain.hpp"
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <filesystem>

namespace nlm {

// Internal implementation details
class EnhancedAgentBrain::Impl {
public:
    Impl(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
        : brain_(brain)
        , config_(config)
        , agentBrain_(brain)
        , state_(AgentState::INITIALIZING)
        , debugOutput_(false)
        , socialLearningEnabled_(false)
        , emergentBehaviorsEnabled_(false)
        , energyBudget_(config.performance.energyBudget)
        , currentEnergy_(energyBudget_)
        , threadingEnabled_(false)
        , threadPoolSize_(std::thread::hardware_concurrency())
        , metricsCollector_(this)
        , signalHandlersRegistered_(false) {}
    
    ~Impl() {
        // Cleanup
    }
    
    // Configuration management
    void configure(const AgentBrainConfig& config) {
        std::lock_guard<std::mutex> lock(configMutex_);
        
        // Store previous config for comparison
        auto oldConfig = config_;
        
        // Update config
        config_ = config;
        
        // Apply configuration changes if agent is active
        if (state_ == AgentState::ACTIVE) {
            applyConfigurationChanges(oldConfig, config_);
        }
        
        // Log configuration change
        if (debugOutput_) {
            NLM_LOG_INFO("AgentBrain configuration updated");
        }
    }
    
    void applyConfigurationChanges(const AgentBrainConfig& oldConfig, const AgentBrainConfig& newConfig) {
        // Check for significant changes and apply them
        if (oldConfig.neuromodulation != newConfig.neumodulation) {
            // Update neuromodulation parameters
            agentBrain_.enableRewardModulation(newConfig.neumodulation.rewardLearningRate > 0.0f);
        }
        
        if (oldConfig.curiosity != newConfig.curiosity) {
            // Update curiosity parameters
        }
        
        if (oldConfig.development != newConfig.development) {
            // Update development parameters
        }
    }
    
    void setParameter(const std::string& name, float value) {
        std::lock_guard<std::mutex> lock(configMutex_);
        
        // Map parameter names to config values
        if (name == "reward_learning_rate") {
            config_.neuromodulation.rewardLearningRate = value;
        } else if (name == "reward_decay_factor") {
            config_.neuromodulation.rewardDecayFactor = value;
        } else if (name == "prediction_error_smoothing") {
            config_.neuromodulation.predictionErrorSmoothing = value;
        } else if (name == "eligibility_trace_decay") {
            config_.neuromodulation.eligibilityTraceDecay = value;
        } else if (name == "dopamine_gain") {
            config_.neuromodulation.dopamineGain = value;
        } else if (name == "reward_prediction_error_gain") {
            config_.neuromodulation.rewardPredictionErrorGain = value;
        } else if (name == "dopamine_uptake_rate") {
            config_.neuromodulation.dopamineUptakeRate = value;
        } else if (name == "neuromodulation_time_constant") {
            config_.neuromodulation.neuromodulationTimeConstant = value;
        } else if (name == "curiosity_threshold") {
            config_.curiosity.curiosityThreshold = value;
        } else if (name == "exploration_rate") {
            config_.curiosity.explorationRate = value;
        } else if (name == "novelty_decay") {
            config_.curiosity.noveltyDecay = value;
        } else if (name == "exploration_temperature") {
            config_.curiosity.explorationTemperature = value;
        } else if (name == "novelty_threshold_variance") {
            config_.curiosity.noveltyThresholdVariance = value;
        } else if (name == "intrinsic_motivation_base") {
            config_.curiosity.intrinsicMotivationBase = value;
        } else if (name == "stdp_ltp_weight") {
            config_.plasticity.stdpLTPWeight = value;
        } else if (name == "stdp_ltd_weight") {
            config_.plasticity.stdpLTDWeight = value;
        } else if (name == "learning_rate_ltp") {
            config_.plasticity.learningRateLTP = value;
        } else if (name == "learning_rate_ltd") {
            config_.plasticity.learningRateLTD = value;
        } else if (name == "eligibility_trace_time_constant") {
            config_.plasticity.eligibilityTraceTimeConstant = value;
        } else if (name == "synaptic_tagging_threshold") {
            config_.plasticity.synapticTaggingThreshold = value;
        } else if (name == "trace_replay_enabled") {
            config_.plasticity.traceReplayEnabled = (value > 0.5f);
        } else if (name == "development_stage_0") {
            config_.development.developmentalStages[0] = value;
        } else if (name == "development_stage_1") {
            config_.development.developmentalStages[1] = value;
        } else if (name == "development_stage_2") {
            config_.development.developmentalStages[2] = value;
        } else if (name == "development_stage_3") {
            config_.development.developmentalStages[3] = value;
        } else if (name == "energy_budget") {
            energyBudget_ = value;
            currentEnergy_ = energyBudget_;
        } else {
            NLM_LOG_WARNING("Unknown parameter name: " + name);
        }
    }
    
    float getParameter(const std::string& name) const {
        std::lock_guard<std::mutex> lock(configMutex_);
        
        // Map parameter names to config values
        if (name == "reward_learning_rate") {
            return config_.neuromodulation.rewardLearningRate;
        } else if (name == "reward_decay_factor") {
            return config_.neuromodulation.rewardDecayFactor;
        } else if (name == "prediction_error_smoothing") {
            return config_.neuromodulation.predictionErrorSmoothing;
        } else if (name == "eligibility_trace_decay") {
            return config_.neuromodulation.eligibilityTraceDecay;
        } else if (name == "dopamine_gain") {
            return config_.neuromodulation.dopamineGain;
        } else if (name == "reward_prediction_error_gain") {
            return config_.neuromodulation.rewardPredictionErrorGain;
        } else if (name == "dopamine_uptake_rate") {
            return config_.neuromodulation.dopamineUptakeRate;
        } else if (name == "neuromodulation_time_constant") {
            return config_.neuromodulation.neuromodulationTimeConstant;
        } else if (name == "curiosity_threshold") {
            return config_.curiosity.curiosityThreshold;
        } else if (name == "exploration_rate") {
            return config_.curiosity.explorationRate;
        } else if (name == "novelty_decay") {
            return config_.curiosity.noveltyDecay;
        } else if (name == "exploration_temperature") {
            return config_.curiosity.explorationTemperature;
        } else if (name == "novelty_threshold_variance") {
            return config_.curiosity.noveltyThresholdVariance;
        } else if (name == "intrinsic_motivation_base") {
            return config_.curiosity.intrinsicMotivationBase;
        } else if (name == "stdp_ltp_weight") {
            return config_.plasticity.stdpLTPWeight;
        } else if (name == "stdp_ltd_weight") {
            return config_.plasticity.stdpLTDWeight;
        } else if (name == "learning_rate_ltp") {
            return config_.plasticity.learningRateLTP;
        } else if (name == "learning_rate_ltd") {
            return config_.plasticity.learningRateLTD;
        } else if (name == "eligibility_trace_time_constant") {
            return config_.plasticity.eligibilityTraceTimeConstant;
        } else if (name == "synaptic_tagging_threshold") {
            return config_.plasticity.synapticTaggingThreshold;
        } else if (name == "trace_replay_enabled") {
            return config_.plasticity.traceReplayEnabled ? 1.0f : 0.0f;
        } else if (name == "development_stage_0") {
            return config_.development.developmentalStages[0];
        } else if (name == "development_stage_1") {
            return config_.development.developmentalStages[1];
        } else if (name == "development_stage_2") {
            return config_.development.developmentalStages[2];
        } else if (name == "development_stage_3") {
            return config_.development.developmentalStages[3];
        } else if (name == "energy_budget") {
            return energyBudget_;
        } else if (name == "current_energy") {
            return currentEnergy_;
        } else {
            NLM_LOG_WARNING("Unknown parameter name: " + name);
            return 0.0f;
        }
    }
    
    std::map<std::string, float> getAllParameters() const {
        std::lock_guard<std::mutex> lock(configMutex_);
        std::map<std::string, float> params;
        
        // Neuromodulation parameters
        params["reward_learning_rate"] = config_.neuromodulation.rewardLearningRate;
        params["reward_decay_factor"] = config_.neuromodulation.rewardDecayFactor;
        params["prediction_error_smoothing"] = config_.neuromodulation.predictionErrorSmoothing;
        params["eligibility_trace_decay"] = config_.neuromodulation.eligibilityTraceDecay;
        params["dopamine_gain"] = config_.neuromodulation.dopamineGain;
        params["reward_prediction_error_gain"] = config_.neuromodulation.rewardPredictionErrorGain;
        params["dopamine_uptake_rate"] = config_.neuromodulation.dopamineUptakeRate;
        params["neuromodulation_time_constant"] = config_.neuromodulation.neuromodulationTimeConstant;
        
        // Curiosity parameters
        params["curiosity_threshold"] = config_.curiosity.curiosityThreshold;
        params["exploration_rate"] = config_.curiosity.explorationRate;
        params["novelty_decay"] = config_.curiosity.noveltyDecay;
        params["exploration_temperature"] = config_.curiosity.explorationTemperature;
        params["novelty_threshold_variance"] = config_.curiosity.noveltyThresholdVariance;
        params["intrinsic_motivation_base"] = config_.curiosity.intrinsicMotivationBase;
        
        // Plasticity parameters
        params["base_plasticity"] = config_.plasticity.basePlasticity;
        params["structural_plasticity_rate"] = config_.plasticity.structuralPlasticityRate;
        params["hebbian_learning_rate"] = config_.plasticity.hebbianLearningRate;
        params["stdp_ltp_weight"] = config_.plasticity.stdpLTPWeight;
        params["stdp_ltd_weight"] = config_.plasticity.stdpLTDWeight;
        params["stdp_tau"] = config_.plasticity.stdpTau;
        params["learning_rate_ltp"] = config_.plasticity.learningRateLTP;
        params["learning_rate_ltd"] = config_.plasticity.learningRateLTD;
        params["eligibility_trace_time_constant"] = config_.plasticity.eligibilityTraceTimeConstant;
        params["synaptic_tagging_threshold"] = config_.plasticity.synapticTaggingThreshold;
        params["trace_replay_enabled"] = config_.plasticity.traceReplayEnabled ? 1.0f : 0.0f;
        
        // Development parameters
        params["development_stage_0"] = config_.development.developmentalStages[0];
        params["development_stage_1"] = config_.development.developmentalStages[1];
        params["development_stage_2"] = config_.development.developmentalStages[2];
        params["development_stage_3"] = config_.development.developmentalStages[3];
        params["synaptogenesis_rate"] = config_.development.synaptogenesisRate;
        params["pruning_rate"] = config_.development.pruningRate;
        params["critical_period_length"] = config_.development.criticalPeriodLength;
        
        // Sensory parameters
        params["sensory_feature_extraction"] = config_.sensory.featureExtraction ? 1.0f : 0.0f;
        params["sensory_noise_level"] = config_.sensory.noiseLevel;
        params["sensory_attention_radius"] = config_.sensory.attentionRadius;
        
        // Motor parameters
        params["motor_action_selection_temperature"] = config_.motor.actionSelectionTemperature;
        params["motor_noise"] = config_.motor.motorNoise;
        params["motor_smooth_motion"] = config_.motor.smoothMotion ? 1.0f : 0.0f;
        params["motor_interpolation_factor"] = config_.motor.interpolationFactor;
        params["motor_momentum_factor"] = config_.motor.momentumFactor;
        
        // Performance parameters
        params["performance_enable_metrics"] = config_.performance.enableMetrics ? 1.0f : 0.0f;
        params["performance_metrics_interval"] = config_.performance.metricsInterval;
        params["performance_max_processing_time"] = config_.performance.maxProcessingTime;
        params["performance_energy_budget"] = config_.performance.energyBudget;
        
        // Social parameters
        params["social_enable_social_learning"] = config_.social.enableSocialLearning ? 1.0f : 0.0f;
        params["social_imitation_rate"] = config_.social.imitationRate;
        params["social_observation_window"] = config_.social.observationWindow;
        params["social_trust_decay"] = config_.social.trustDecay;
        params["social_influence"] = config_.social.socialInfluence;
        
        // World integration
        params["world_width"] = world_.getWidth();
        params["world_height"] = world_.getHeight();
        params["world_vision_width"] = world_.getVisionWidth();
        params["world_vision_height"] = world_.getVisionHeight();
        
        // Energy management
        params["energy_budget"] = energyBudget_;
        params["current_energy"] = currentEnergy_;
        
        return params;
    }
    
    // Agent lifecycle management
    void create(const std::string& id, const AgentBrainConfig& config) {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        if (state_ != AgentState::INITIALIZING) {
            NLM_LOG_WARNING("Cannot create agent in current state: " + std::to_string(static_cast<int>(state_)));
            return;
        }
        
        // Store agent ID
        agentId_ = id;
        
        // Configure with provided settings
        configure(config);
        
        // Initialize neural systems
        if (brain_) {
            brain_->initialize();
        }
        
        // Set state to active
        state_ = AgentState::ACTIVE;
        
        // Reset metrics
        metricsCollector_.reset();
        
        // Log creation
        NLM_LOG_INFO("EnhancedAgentBrain created: ID=" + id + ", Neurons=" + 
                    std::to_string(brain_ ? brain_->getTotalNeuronCount() : 0));
    }
    
    void destroy() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        if (state_ == AgentState::DESTROYING || state_ == AgentState::INITIALIZING) {
            return;
        }
        
        // Set state to destroying
        state_ = AgentState::DESTROYING;
        
        // Clear all references
        world_.reset();
        for (auto* follower : followers_) {
            follower->impl_->setState(AgentState::DESTROYING);
        }
        followers_.clear();
        
        // Reset neural systems
        if (brain_) {
            brain_->reset();
        }
        
        // Reset state
        agentBrain_.reset();
        config_ = AgentBrainConfig();
        
        // Log destruction
        NLM_LOG_INFO("EnhancedAgentBrain destroyed: ID=" + agentId_);
        
        // Set final state
        state_ = AgentState::INITIALIZING;
        agentId_.clear();
    }
    
    bool isActive() const {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        return state_ == AgentState::ACTIVE;
    }
    
    std::string getID() const {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        return agentId_;
    }
    
    AgentState getState() const {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        return state_;
    }
    
    void setState(AgentState state) {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        // Validate state transition
        if (!isValidStateTransition(state_, state)) {
            NLM_LOG_WARNING("Invalid state transition: " + 
                          std::to_string(static_cast<int>(state_)) + " -> " +
                          std::to_string(static_cast<int>(state)));
            return;
        }
        
        state_ = state;
        
        // Log state change
        NLM_LOG_INFO("AgentBrain state changed to: " + std::to_string(static_cast<int>(state_)));
    }
    
    bool isValidStateTransition(AgentState from, AgentState to) {
        // Simple state transition validation
        switch (from) {
            case AgentState::INITIALIZING:
                return to == AgentState::ACTIVE || to == AgentState::DESTROYING;
            case AgentState::ACTIVE:
                return to == AgentState::PAUSED || to == AgentState::EMERGENCY_STOPPED ||
                       to == AgentState::EVOLVING || to == AgentState::RESEtting ||
                       to == AgentState::DESTROYING;
            case AgentState::PAUSED:
                return to == AgentState::ACTIVE || to == AgentState::EMERGENCY_STOPPED ||
                       to == AgentState::DESTROYING;
            case AgentState::EMERGENCY_STOPPED:
                return to == AgentState::ACTIVE || to == AgentState::PAUSED ||
                       to == AgentState::DESTROYING;
            case AgentState::EVOLVING:
                return to == AgentState::ACTIVE || to == AgentState::PAUSED ||
                       to == AgentState::DESTROYING;
            case AgentState::RESEtting:
                return to == AgentState::ACTIVE || to == AgentState::PAUSED ||
                       to == AgentState::DESTROYING;
            case AgentState::DESTROYING:
                return false;
        }
        return false;
    }
    
    void pause() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        if (state_ == AgentState::ACTIVE) {
            setState(AgentState::PAUSED);
        }
    }
    
    void resume() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        if (state_ == AgentState::PAUSED) {
            setState(AgentState::ACTIVE);
        }
    }
    
    void emergencyStop() {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        
        if (state_ != AgentState::EMERGENCY_STOPPED) {
            setState(AgentState::EMERGENCY_STOPPED);
            
            // Stop all operations immediately
            pause();
        }
    }
    
    // World integration
    void connectToWorld(std::shared_ptr<SimpleWorld> world) {
        std::lock_guard<std::mutex> lock(worldMutex_);
        
        world_ = world;
        
        // Initialize with world if active
        if (state_ == AgentState::ACTIVE && world_) {
            agentBrain_.initialize(*world_);
        }
    }
    
    void disconnectFromWorld() {
        std::lock_guard<std::mutex> lock(worldMutex_);
        world_.reset();
    }
    
    bool isConnectedToWorld() const {
        std::lock_guard<std::mutex> lock(worldMutex_);
        return world_ != nullptr;
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const {
        std::lock_guard<std::mutex> lock(worldMutex_);
        return world_;
    }
    
    // Checkpoint and persistence
    bool save(const std::string& path) const {
        std::lock_guard<std::mutex> lock(persistenceMutex_);
        
        try {
            std::ofstream file(path, std::ios::binary);
            if (!file) {
                NLM_LOG_ERROR("Failed to open file for saving: " + path);
                return false;
            }
            
            // Save brain state
            if (brain_) {
                if (!brain_->save(path)) {
                    NLM_LOG_ERROR("Failed to save brain state");
                    return false;
                }
            }
            
            // Save agent configuration
            // (Implementation would continue here)
            
            NLM_LOG_INFO("AgentBrain saved successfully: " + path);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during save: " + std::string(e.what()));
            return false;
        }
    }
    
    bool load(const std::string& path) {
        std::lock_guard<std::mutex> lock(persistenceMutex_);
        
        try {
            std::ifstream file(path, std::ios::binary);
            if (!file) {
                NLM_LOG_ERROR("Failed to open file for loading: " + path);
                return false;
            }
            
            // Load brain state
            if (brain_) {
                if (!brain_->load(path)) {
                    NLM_LOG_ERROR("Failed to load brain state");
                    return false;
                }
            }
            
            // Load agent configuration
            // (Implementation would continue here)
            
            NLM_LOG_INFO("AgentBrain loaded successfully: " + path);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during load: " + std::string(e.what()));
            return false;
        }
    }
    
    bool createCheckpoint(const std::string& path) const {
        std::string checkpointPath = path + ".checkpoint";
        return save(checkpointPath);
    }
    
    bool restoreFromCheckpoint(const std::string& path) {
        std::string checkpointPath = path + ".checkpoint";
        return load(checkpointPath);
    }
    
    bool exportState(const std::string& path) const {
        // Export complete state including configuration and metrics
        return save(path);
    }
    
    bool importState(const std::string& path) {
        // Import complete state including configuration and metrics
        return load(path);
    }
    
    // Advanced neural operations
    void injectCurrentToNeuron(NeuronId neuron, MembranePotential current) {
        if (brain_) {
            brain_->injectCurrent(neuron, current);
        }
    }
    
    void injectCurrentToAllNeuronsOfType(NeuronType type, MembranePotential current) {
        if (brain_) {
            brain_->injectCurrentToNeurons(type, current);
        }
    }
    
    void stepBrain(SimulationStep step, Timestamp time) {
        if (brain_) {
            brain_->step(step, time);
        }
    }
    
    // Evolutionary capabilities
    void evolve(const std::vector<float>& geneticCode) {
        std::lock_guard<std::mutex> lock(evolutionMutex_);
        
        if (state_ == AgentState::ACTIVE) {
            setState(AgentState::EVOLVING);
            
            // Apply genetic code to modify brain parameters
            // This would involve modifying neural connections, plasticity rules, etc.
            
            // Reset state
            setState(AgentState::ACTIVE);
            
            NLM_LOG_INFO("AgentBrain evolved with genetic code of length " + 
                        std::to_string(geneticCode.size()));
        }
    }
    
    std::vector<float> getGeneticCode() const {
        std::lock_guard<std::mutex> lock(evolutionMutex_);
        
        // Generate genetic representation of current brain state
        std::vector<float> geneticCode;
        
        if (brain_) {
            // Extract neural connectivity patterns
            geneticCode.push_back(static_cast<float>(brain_->getTotalNeuronCount()));
            geneticCode.push_back(static_cast<float>(brain_->getTotalSynapseCount()));
            
            // Add plasticity parameters
            auto* plasticity = brain_->getStructuralPlasticity();
            if (plasticity) {
                geneticCode.push_back(plasticity->getSynaptogenesisRate());
                geneticCode.push_back(plasticity->getPruningRate());
            }
            
            // Add neuromodulation parameters
            auto* dopamine = brain_->getDopamine();
            if (dopamine) {
                // Extract dopamine system parameters
            }
        }
        
        return geneticCode;
    }
    
    void mutate(float mutationRate) {
        std::lock_guard<std::mutex> lock(evolutionMutex_);
        
        if (state_ == AgentState::ACTIVE) {
            // Apply random mutations to genetic code
            // This would involve random changes to brain parameters
            
            NLM_LOG_INFO("AgentBrain mutated with rate " + std::to_string(mutationRate));
        }
    }
    
    float calculateFitness() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        // Calculate fitness based on multiple factors
        float fitness = 0.0f;
        
        // Reward performance
        fitness += metrics_.totalReward * 0.3f;
        
        // Exploration efficiency
        if (metrics_.actionsTaken > 0) {
            fitness += (metrics_.explorationRate * metrics_.actionsTaken) * 0.2f;
        }
        
        // Neural stability
        float stability = 1.0f - std::abs(metrics_.excitationInhibitionRatio - 1.0f);
        fitness += stability * 0.2f;
        
        // Learning rate
        fitness += metrics_.averageFiringRate * 0.15f;
        
        // Memory usage efficiency
        if (metrics_.memoryFootprint > 0) {
            float efficiency = (static_cast<float>(metrics_.synapseCount) / metrics_.memoryFootprint) * 100.0f;
            fitness += std::min(efficiency, 100.0f) * 0.1f;
        }
        
        // Development progress
        float developmentProgress = static_cast<float>(metrics_.developmentalStage) / 4.0f;
        fitness += developmentProgress * 0.05f;
        
        return std::max(0.0f, std::min(fitness, 100.0f));
    }
    
    // Social learning
    void observe(const EnhancedAgentBrain& otherAgent) {
        if (socialLearningEnabled_) {
            // Learn from other agent's behavior
            // This would involve copying successful strategies
            
            NLM_LOG_INFO("AgentBrain observed another agent");
        }
    }
    
    void teach(const EnhancedAgentBrain& student) {
        if (socialLearningEnabled_) {
            // Teach the student agent
            // This would involve demonstrating successful behaviors
            
            NLM_LOG_INFO("AgentBrain taught another agent");
        }
    }
    
    void enableSocialLearning(bool enable) {
        std::lock_guard<std::mutex> lock(socialMutex_);
        socialLearningEnabled_ = enable;
    }
    
    bool isSocialLearningEnabled() const {
        std::lock_guard<std::mutex> lock(socialMutex_);
        return socialLearningEnabled_;
    }
    
    void setImitationRate(float rate) {
        std::lock_guard<std::mutex> lock(socialMutex_);
        config_.social.imitationRate = rate;
    }
    
    float getImitationRate() const {
        std::lock_guard<std::mutex> lock(socialMutex_);
        return config_.social.imitationRate;
    }
    
    // Performance monitoring
    void startPerformanceTimer() {
        metricsCollector_.startTimer();
    }
    
    void stopPerformanceTimer() {
        metricsCollector_.stopTimer();
    }
    
    void recordMetrics() {
        metricsCollector_.collectMetrics();
    }
    
    AgentBrainMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        
        // Combine internal metrics with current state
        AgentBrainMetrics metrics = metricsCollector_.getMetrics();
        
        // Add current state
        metrics.processingTime = currentProcessingTime_;
        metrics.stepTime = currentStepTime_;
        metrics.brainStepTime = brainStepTime_;
        metrics.decodeTime = decodeStepTime_;
        metrics.energyConsumption = energyBudget_ - currentEnergy_;
        
        return metrics;
    }
    
    void resetMetrics() {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsCollector_.reset();
    }
    
    std::string getMetricsSummary() const {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        std::stringstream ss;
        
        auto metrics = getMetrics();
        
        ss << "=== AgentBrain Performance Metrics ===\n";
        ss << "Processing Time: " << metrics.processingTime << " ms\n";
        ss << "Step Time: " << metrics.stepTime << " ms\n";
        ss << "Total Reward: " << metrics.totalReward << "\n";
        ss << "Actions Taken: " << metrics.actionsTaken << "\n";
        ss << "Exploration Rate: " << metrics.explorationRate << "\n";
        ss << "Average Firing Rate: " << metrics.averageFiringRate << "\n";
        ss << "Memory Footprint: " << metrics.memoryFootprint << " bytes\n";
        ss << "Fitness Score: " << calculateFitness() << "\n";
        
        return ss.str();
    }
    
    // Advanced sensory processing (stubs - would be implemented in separate files)
    void setSensoryProcessor(std::unique_ptr<class ISensoryProcessor> processor) {
        sensoryProcessor_ = std::move(processor);
    }
    
    void setMotorDecoder(std::unique_ptr<class IMotorDecoder> decoder) {
        motorDecoder_ = std::move(decoder);
    }
    
    void setNeuromodulator(std::unique_ptr<class INeuromodulator> modulator) {
        neuromodulator_ = std::move(modulator);
    }
    
    // Multi-agent coordination
    void addFollower(EnhancedAgentBrain* agent) {
        std::lock_guard<std::mutex> lock(multiAgentMutex_);
        followers_.push_back(agent);
    }
    
    void removeFollower(EnhancedAgentBrain* agent) {
        std::lock_guard<std::mutex> lock(multiAgentMutex_);
        followers_.erase(std::remove(followers_.begin(), followers_.end(), agent), followers_.end());
    }
    
    void coordinateWith(EnhancedAgentBrain* agent) {
        if (emergentBehaviorsEnabled_) {
            // Coordinate with other agent
            // This would involve shared decision making
        }
    }
    
    void enableEmergentBehaviors(bool enable) {
        std::lock_guard<std::mutex> lock(multiAgentMutex_);
        emergentBehaviorsEnabled_ = enable;
    }
    
    bool areEmergentBehaviorsEnabled() const {
        std::lock_guard<std::mutex> lock(multiAgentMutex_);
        return emergentBehaviorsEnabled_;
    }
    
    // Development control
    void accelerateDevelopment() {
        // Accelerate developmental stages
        config_.development.criticalPeriodLength *= 1.5f;
        if (config_.development.criticalPeriodLength > 600.0) {
            config_.development.criticalPeriodLength = 600.0;
        }
    }
    
    void decelerateDevelopment() {
        // Decelerate developmental stages
        config_.development.criticalPeriodLength *= 0.8f;
        if (config_.development.criticalPeriodLength < 60.0) {
            config_.development.criticalPeriodLength = 60.0;
        }
    }
    
    void setDevelopmentStage(DevelopmentalStage stage) {
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
    }
    
    void forceDevelopmentalStageTransition() {
        if (brain_) {
            // Force immediate developmental stage transition
            brain_->setDevelopmentalStage(static_cast<DevelopmentalStage>((
                static_cast<int>(brain_->getDevelopmentalStage()) + 1) % 5));
        }
    }
    
    void setCriticalPeriodActive(bool active) {
        // Enable/disable critical period plasticity
    }
    
    // Action planning and sequencing
    void setActionSequence(const std::vector<MotorCommand>& sequence) {
        actionSequence_ = sequence;
        currentActionIndex_ = 0;
    }
    
    void clearActionSequence() {
        actionSequence_.clear();
        currentActionIndex_ = 0;
    }
    
    bool hasActionSequence() const {
        return !actionSequence_.empty();
    }
    
    MotorCommand getNextAction() {
        if (hasActionSequence()) {
            return actionSequence_[currentActionIndex_];
        }
        return MotorCommand::Wait;
    }
    
    void skipCurrentAction() {
        if (currentActionIndex_ < actionSequence_.size()) {
            currentActionIndex_++;
        }
    }
    
    void repeatCurrentAction() {
        // Stay on current action
    }
    
    // Debug and visualization
    void enableDebugOutput(bool enable) {
        std::lock_guard<std::mutex> lock(debugMutex_);
        debugOutput_ = enable;
        
        if (enable) {
            NLM_LOG_INFO("Debug output enabled");
        } else {
            NLM_LOG_INFO("Debug output disabled");
        }
    }
    
    bool isDebugOutputEnabled() const {
        std::lock_guard<std::mutex> lock(debugMutex_);
        return debugOutput_;
    }
    
    void logState() const {
        if (debugOutput_) {
            NLM_LOG_INFO(getDebugInfo());
        }
    }
    
    std::string getDebugInfo() const {
        std::stringstream ss;
        
        ss << "=== EnhancedAgentBrain Debug Info ===\n";
        ss << "Agent ID: " << agentId_ << "\n";
        ss << "State: " << static_cast<int>(state_) << "\n";
        ss << "Active: " << (isActive() ? "Yes" : "No") << "\n";
        ss << "Connected to World: " << (isConnectedToWorld() ? "Yes" : "No") << "\n";
        ss << "Brain Neurons: " << (brain_ ? brain_->getTotalNeuronCount() : 0) << "\n";
        ss << "Brain Synapses: " << (brain_ ? brain_->getTotalSynapseCount() : 0) << "\n";
        ss << "Energy: " << currentEnergy_ << "/" << energyBudget_ << "\n";
        ss << "Social Learning: " << (socialLearningEnabled_ ? "Enabled" : "Disabled") << "\n";
        ss << "Emergent Behaviors: " << (emergentBehaviorsEnabled_ ? "Enabled" : "Disabled") << "\n";
        
        return ss.str();
    }
    
    // Memory management
    void enableMemoryManagement(bool enable) {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        memoryManagementEnabled_ = enable;
    }
    
    bool isMemoryManagementEnabled() const {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        return memoryManagementEnabled_;
    }
    
    void optimizeMemoryUsage() {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        
        if (memoryManagementEnabled_) {
            // Optimize memory usage
            // This would involve garbage collection and memory compaction
        }
    }
    
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        
        size_t memory = sizeof(*this);
        
        if (brain_) {
            memory += brain_->getTotalSynapseCount() * sizeof(Synapse);
        }
        
        return memory;
    }
    
    void garbageCollect() {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        
        if (memoryManagementEnabled_) {
            // Perform garbage collection
            // This would involve removing unused neurons/synapses
        }
    }
    
    // Resource management
    void setEnergyBudget(float budget) {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        energyBudget_ = budget;
        if (currentEnergy_ > energyBudget_) {
            currentEnergy_ = energyBudget_;
        }
    }
    
    float getEnergyBudget() const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        return energyBudget_;
    }
    
    float getCurrentEnergy() const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        return currentEnergy_;
    }
    
    bool hasEnergy(float amount) const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        return currentEnergy_ >= amount;
    }
    
    void consumeEnergy(float amount) {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        currentEnergy_ = std::max(0.0f, currentEnergy_ - amount);
    }
    
    void regenerateEnergy(float amount) {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        currentEnergy_ = std::min(energyBudget_, currentEnergy_ + amount);
    }
    
    // Threading and concurrency
    void enableThreading(bool enable) {
        threadingEnabled_ = enable;
    }
    
    bool isThreadingEnabled() const {
        return threadingEnabled_;
    }
    
    void setThreadPoolSize(size_t size) {
        threadPoolSize_ = size;
    }
    
    size_t getThreadPoolSize() const {
        return threadPoolSize_;
    }
    
    void processAsync() {
        if (threadingEnabled_) {
            // Process asynchronously
            // This would involve running brain simulation in separate threads
        }
    }
    
    // Signal handling
    void setSignalHandler(void (*handler)(EnhancedAgentBrain*, const std::string&)) {
        signalHandler_ = handler;
        signalHandlersRegistered_ = true;
    }
    
    void triggerSignal(const std::string& signal, const std::map<std::string, std::string>& data) {
        if (signalHandlersRegistered_ && signalHandler_) {
            signalHandler_(this, signal);
        }
    }
    
    // Metrics collector
    class MetricsCollector {
    public:
        MetricsCollector(EnhancedAgentBrain::Impl* parent) : parent_(parent) {}
        
        void startTimer() {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
        
        void stopTimer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
            
            std::lock_guard<std::mutex> lock(parent_->metricsMutex_);
            parent_->currentProcessingTime_ += duration;
        }
        
        void collectMetrics() {
            // Collect current metrics
            if (parent_->brain_) {
                std::lock_guard<std::mutex> lock(parent_->metricsMutex_);
                
                parent_->metrics_.sensorySpikes = parent_->brain_->getFiringNeuronCount();
                parent_->metrics_.motorSpikes = parent_->brain_->getActiveNeuronCount();
                parent_->metrics_.averageFiringRate = parent_->brain_->getAverageFiringRate();
                parent_->metrics_.excitationInhibitionRatio = parent_->brain_->getExcitationInhibitionRatio();
                parent_->metrics_.synapseCount = parent_->brain_->getTotalSynapseCount();
                parent_->metrics_.neuronCount = parent_->brain_->getTotalNeuronCount();
                parent_->metrics_.developmentalStage = parent_->brain_->getDevelopmentalStage();
            }
        }
        
        AgentBrainMetrics getMetrics() const {
            std::lock_guard<std::mutex> lock(parent_->metricsMutex_);
            return metrics_;
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(parent_->metricsMutex_);
            metrics_ = AgentBrainMetrics();
        }
        
    private:
        EnhancedAgentBrain::Impl* parent_;
        std::chrono::high_resolution_clock::time_point startTime_;
        mutable std::mutex metricsMutex_;
        AgentBrainMetrics metrics_;
    } metricsCollector_;
    
    // Data members
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    AgentBrain agentBrain_;  // Original AgentBrain for backward compatibility
    
    AgentState state_;
    std::string agentId_;
    bool debugOutput_;
    
    std::shared_ptr<SimpleWorld> world_;
    bool socialLearningEnabled_;
    bool emergentBehaviorsEnabled_;
    
    float energyBudget_;
    float currentEnergy_;
    
    bool threadingEnabled_;
    size_t threadPoolSize_;
    
    bool memoryManagementEnabled_;
    
    mutable std::mutex configMutex_;
    mutable std::mutex lifecycleMutex_;
    mutable std::mutex worldMutex_;
    mutable std::mutex persistenceMutex_;
    mutable std::mutex evolutionMutex_;
    mutable std::mutex socialMutex_;
    mutable std::mutex metricsMutex_;
    mutable std::mutex debugMutex_;
    mutable std::mutex multiAgentMutex_;
    mutable std::mutex memoryMutex_;
    mutable std::mutex resourceMutex_;
    
    std::vector<EnhancedAgentBrain*> followers_;
    
    std::unique_ptr<class ISensoryProcessor> sensoryProcessor_;
    std::unique_ptr<class IMotorDecoder> motorDecoder_;
    std::unique_ptr<class INeuromodulator> neuromodulator_;
    
    std::vector<MotorCommand> actionSequence_;
    size_t currentActionIndex_ = 0;
    
    double currentProcessingTime_ = 0.0;
    double currentStepTime_ = 0.0;
    double brainStepTime_ = 0.0;
    double decodeStepTime_ = 0.0;
    
    void (*signalHandler_)(EnhancedAgentBrain*, const std::string&) = nullptr;
    bool signalHandlersRegistered_ = false;
    
    friend class MetricsCollector;
}; // class EnhancedAgentBrain::Impl

// EnhancedAgentBrain implementation
EnhancedAgentBrain::EnhancedAgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : pImpl(std::make_unique<Impl>(brain, config)) {
    // Create agent with provided brain
    pImpl->create("agent_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()), config);
}

EnhancedAgentBrain::~EnhancedAgentBrain() = default;

EnhancedAgentBrain::EnhancedAgentBrain(EnhancedAgentBrain&&) noexcept = default;
EnhancedAgentBrain& EnhancedAgentBrain::operator=(EnhancedAgentBrain&&) noexcept = default;

void EnhancedAgentBrain::configure(const AgentBrainConfig& config) {
    pImpl->configure(config);
}

const AgentBrainConfig& EnhancedAgentBrain::getConfig() const {
    return pImpl->config_;
}

void EnhancedAgentBrain::setParameter(const std::string& name, float value) {
    pImpl->setParameter(name, value);
}

float EnhancedAgentBrain::getParameter(const std::string& name) const {
    return pImpl->getParameter(name);
}

std::map<std::string, float> EnhancedAgentBrain::getAllParameters() const {
    return pImpl->getAllParameters();
}

void EnhancedAgentBrain::create(const std::string& id, const AgentBrainConfig& config) {
    pImpl->create(id, config);
}

void EnhancedAgentBrain::destroy() {
    pImpl->destroy();
}

bool EnhancedAgentBrain::isActive() const {
    return pImpl->isActive();
}

std::string EnhancedAgentBrain::getID() const {
    return pImpl->getID();
}

AgentState EnhancedAgentBrain::getState() const {
    return pImpl->getState();
}

void EnhancedAgentBrain::pause() {
    pImpl->pause();
}

void EnhancedAgentBrain::resume() {
    pImpl->resume();
}

void EnhancedAgentBrain::emergencyStop() {
    pImpl->emergencyStop();
}

void EnhancedAgentBrain::connectToWorld(std::shared_ptr<SimpleWorld> world) {
    pImpl->connectToWorld(world);
}

void EnhancedAgentBrain::disconnectFromWorld() {
    pImpl->disconnectFromWorld();
}

bool EnhancedAgentBrain::isConnectedToWorld() const {
    return pImpl->isConnectedToWorld();
}

std::shared_ptr<SimpleWorld> EnhancedAgentBrain::getWorld() const {
    return pImpl->getWorld();
}

bool EnhancedAgentBrain::save(const std::string& path) const {
    return pImpl->save(path);
}

bool EnhancedAgentBrain::load(const std::string& path) {
    return pImpl->load(path);
}

bool EnhancedAgentBrain::createCheckpoint(const std::string& path) const {
    return pImpl->createCheckpoint(path);
}

bool EnhancedAgentBrain::restoreFromCheckpoint(const std::string& path) {
    return pImpl->restoreFromCheckpoint(path);
}

bool EnhancedAgentBrain::exportState(const std::string& path) const {
    return pImpl->exportState(path);
}

bool EnhancedAgentBrain::importState(const std::string& path) {
    return pImpl->importState(path);
}

void EnhancedAgentBrain::injectCurrentToNeuron(NeuronId neuron, MembranePotential current) {
    pImpl->injectCurrentToNeuron(neuron, current);
}

void EnhancedAgentBrain::injectCurrentToAllNeuronsOfType(NeuronType type, MembranePotential current) {
    pImpl->injectCurrentToAllNeuronsOfType(type, current);
}

void EnhancedAgentBrain::stepBrain(SimulationStep step, Timestamp time) {
    pImpl->stepBrain(step, time);
}

void EnhancedAgentBrain::evolve(const std::vector<float>& geneticCode) {
    pImpl->evolve(geneticCode);
}

std::vector<float> EnhancedAgentBrain::getGeneticCode() const {
    return pImpl->getGeneticCode();
}

void EnhancedAgentBrain::mutate(float mutationRate) {
    pImpl->mutate(mutationRate);
}

float EnhancedAgentBrain::calculateFitness() const {
    return pImpl->calculateFitness();
}

void EnhancedAgentBrain::observe(const EnhancedAgentBrain& otherAgent) {
    pImpl->observe(otherAgent);
}

void EnhancedAgentBrain::teach(const EnhancedAgentBrain& student) {
    pImpl->teach(student);
}

void EnhancedAgentBrain::enableSocialLearning(bool enable) {
    pImpl->enableSocialLearning(enable);
}

bool EnhancedAgentBrain::isSocialLearningEnabled() const {
    return pImpl->isSocialLearningEnabled();
}

void EnhancedAgentBrain::setImitationRate(float rate) {
    pImpl->setImitationRate(rate);
}

float EnhancedAgentBrain::getImitationRate() const {
    return pImpl->getImitationRate();
}

void EnhancedAgentBrain::startPerformanceTimer() {
    pImpl->startPerformanceTimer();
}

void EnhancedAgentBrain::stopPerformanceTimer() {
    pImpl->stopPerformanceTimer();
}

void EnhancedAgentBrain::recordMetrics() {
    pImpl->recordMetrics();
}

AgentBrainMetrics EnhancedAgentBrain::getMetrics() const {
    return pImpl->getMetrics();
}

void EnhancedAgentBrain::resetMetrics() {
    pImpl->resetMetrics();
}

std::string EnhancedAgentBrain::getMetricsSummary() const {
    return pImpl->getMetricsSummary();
}

void EnhancedAgentBrain::setSensoryProcessor(std::unique_ptr<ISensoryProcessor> processor) {
    pImpl->setSensoryProcessor(std::move(processor));
}

void EnhancedAgentBrain::setMotorDecoder(std::unique_ptr<IMotorDecoder> decoder) {
    pImpl->setMotorDecoder(std::move(decoder));
}

void EnhancedAgentBrain::setNeuromodulator(std::unique_ptr<INeuromodulator> modulator) {
    pImpl->setNeuromodulator(std::move(modulator));
}

void EnhancedAgentBrain::addFollower(EnhancedAgentBrain* agent) {
    pImpl->addFollower(agent);
}

void EnhancedAgentBrain::removeFollower(EnhancedAgentBrain* agent) {
    pImpl->removeFollower(agent);
}

void EnhancedAgentBrain::coordinateWith(EnhancedAgentBrain* agent) {
    pImpl->coordinateWith(agent);
}

void EnhancedAgentBrain::enableEmergentBehaviors(bool enable) {
    pImpl->enableEmergentBehaviors(enable);
}

bool EnhancedAgentBrain::areEmergentBehaviorsEnabled() const {
    return pImpl->areEmergentBehaviorsEnabled();
}

void EnhancedAgentBrain::accelerateDevelopment() {
    pImpl->accelerateDevelopment();
}

void EnhancedAgentBrain::decelerateDevelopment() {
    pImpl->decelerateDevelopment();
}

void EnhancedAgentBrain::setDevelopmentStage(DevelopmentalStage stage) {
    pImpl->setDevelopmentStage(stage);
}

void EnhancedAgentBrain::forceDevelopmentalStageTransition() {
    pImpl->forceDevelopmentalStageTransition();
}

void EnhancedAgentBrain::setCriticalPeriodActive(bool active) {
    pImpl->setCriticalPeriodActive(active);
}

void EnhancedAgentBrain::setActionSequence(const std::vector<MotorCommand>& sequence) {
    pImpl->setActionSequence(sequence);
}

void EnhancedAgentBrain::clearActionSequence() {
    pImpl->clearActionSequence();
}

bool EnhancedAgentBrain::hasActionSequence() const {
    return pImpl->hasActionSequence();
}

MotorCommand EnhancedAgentBrain::getNextAction() {
    return pImpl->getNextAction();
}

void EnhancedAgentBrain::skipCurrentAction() {
    pImpl->skipCurrentAction();
}

void EnhancedAgentBrain::repeatCurrentAction() {
    pImpl->repeatCurrentAction();
}

void EnhancedAgentBrain::enableDebugOutput(bool enable) {
    pImpl->enableDebugOutput(enable);
}

bool EnhancedAgentBrain::isDebugOutputEnabled() const {
    return pImpl->isDebugOutputEnabled();
}

void EnhancedAgentBrain::logState() const {
    pImpl->logState();
}

std::string EnhancedAgentBrain::getDebugInfo() const {
    return pImpl->getDebugInfo();
}

void EnhancedAgentBrain::enableMemoryManagement(bool enable) {
    pImpl->enableMemoryManagement(enable);
}

bool EnhancedAgentBrain::isMemoryManagementEnabled() const {
    return pImpl->isMemoryManagementEnabled();
}

void EnhancedAgentBrain::optimizeMemoryUsage() {
    pImpl->optimizeMemoryUsage();
}

void EnhancedAgentBrain::garbageCollect() {
    pImpl->garbageCollect();
}

size_t EnhancedAgentBrain::getMemoryUsage() const {
    return pImpl->getMemoryUsage();
}

void EnhancedAgentBrain::setEnergyBudget(float budget) {
    pImpl->setEnergyBudget(budget);
}

float EnhancedAgentBrain::getEnergyBudget() const {
    return pImpl->getEnergyBudget();
}

float EnhancedAgentBrain::getCurrentEnergy() const {
    return pImpl->getCurrentEnergy();
}

bool EnhancedAgentBrain::hasEnergy(float amount) const {
    return pImpl->hasEnergy(amount);
}

void EnhancedAgentBrain::consumeEnergy(float amount) {
    pImpl->consumeEnergy(amount);
}

void EnhancedAgentBrain::regenerateEnergy(float amount) {
    pImpl->regenerateEnergy(amount);
}

void EnhancedAgentBrain::enableThreading(bool enable) {
    pImpl->enableThreading(enable);
}

bool EnhancedAgentBrain::isThreadingEnabled() const {
    return pImpl->isThreadingEnabled();
}

void EnhancedAgentBrain::setThreadPoolSize(size_t size) {
    pImpl->setThreadPoolSize(size);
}

void EnhancedAgentBrain::processAsync() {
    pImpl->processAsync();
}

void EnhancedAgentBrain::setSignalHandler(void (*handler)(EnhancedAgentBrain*, const std::string&)) {
    pImpl->setSignalHandler(handler);
}

void EnhancedAgentBrain::triggerSignal(const std::string& signal, const std::map<std::string, std::string>& data) {
    pImpl->triggerSignal(signal, data);
}

// Backward compatibility implementations
void EnhancedAgentBrain::initialize(const SimpleWorld& world) {
    pImpl->agentBrain_.initialize(world);
}

void EnhancedAgentBrain::processSensoryInput(const SensoryPercept& percept) {
    pImpl->agentBrain_.processSensoryInput(percept);
}

MotorCommand EnhancedAgentBrain::decodeMotorCommand() {
    return pImpl->agentBrain_.decodeMotorCommand();
}

void EnhancedAgentBrain::applyRewardModulation(float reward, float predictedReward) {
    pImpl->agentBrain_.applyRewardModulation(reward, predictedReward);
}

void EnhancedAgentBrain::updateDevelopment(double timestep) {
    pImpl->agentBrain_.updateDevelopment(timestep);
}

void EnhancedAgentBrain::reset() {
    pImpl->agentBrain_.reset();
}

} // namespace nlm