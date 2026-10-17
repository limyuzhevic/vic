#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Constants for agent brain behavior
constexpr float VISION_SENSITIVITY_SCALE_ = 5.0f;      // Vision signal scaling
constexpr float TOUCH_SENSITIVITY_SCALE_ = 8.0f;       // Touch collision signal scaling
constexpr float INTERNAL_SIGNAL_SCALE_ = 5.0f;         // Internal signal scaling
constexpr float PROPRIOCEPTION_SIGNAL_SCALE_ = 3.0f;   // Proprioception signal scaling
constexpr float INTERNAL_SIGNAL_CENTER_SCALE_ = 2.0f;  // Internal signal centering
constexpr float INTERNAL_SIGNAL_CENTER_OFFSET_ = 1.0f;  // Internal signal centering offset
constexpr float PROPRIOCEPTION_SIGNAL_CENTER_SCALE_ = 2.0f;  // Proprioception centering
constexpr float PROPRIOCEPTION_SIGNAL_CENTER_OFFSET_ = 1.0f;  // Proprioception centering offset
constexpr float NOVELTY_TO_CURIOUSITY_SCALE_ = 2.0f;     // Novelty contribution to curiosity
constexpr float PREDICTION_ERROR_TO_CURIOUSITY_SCALE_ = 0.5f;  // Prediction error contribution to curiosity
constexpr float DEFAULT_EXPLORATION_PROBABILITY_ = 0.3f;  // Base exploration probability
constexpr float DEFAULT_PREDICTION_ERROR_LEARNING_RATE_ = 0.05f;  // Learning rate for prediction error
constexpr float DEFAULT_PLASTICITY_MIN_ = 0.1f;          // Minimum plasticity factor
constexpr float DEFAULT_PLASTICITY_MAX_ = 2.0f;          // Maximum plasticity factor
constexpr float MOTOR_ACTIVITY_THRESHOLD_ = 0.5f;        // Minimum motor activity for action
constexpr float REWARD_PREDICTION_UPDATE_RATE_ = 0.95f; // Exponential decay for reward prediction
constexpr float ELIGIBILITY_THRESHOLD_ = 0.001f;         // Threshold for eligibility trace application
constexpr float SYNAPTOGENESIS_RATE_BASE_ = 0.0001f;     // Base synaptogenesis rate
constexpr float PRUNING_RATE_BASE_ = 0.00001f;          // Base pruning rate
constexpr double INITIAL_STAGE_DURATION_ = 60.0;        // Duration of initial developmental stage
constexpr double CRITICAL_PERIOD_DURATION_ = 300.0;     // Duration of critical period
constexpr double MATURATION_DURATION_ = 900.0;          // Duration of maturation stage

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , curiosityThreshold_(0.3f)
    , explorationProbability_(DEFAULT_EXPLORATION_PROBABILITY_)
    , predictionErrorLearningRate_(DEFAULT_PREDICTION_ERROR_LEARNING_RATE_)
    , plasticityFactorMin_(DEFAULT_PLASTICITY_MIN_)
    , plasticityFactorMax_(DEFAULT_PLASTICITY_MAX_)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::decodeMotorCommand called with null brain pointer");
        return;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < MOTOR_ACTIVITY_THRESHOLD_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationProbability_;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = REWARD_PREDICTION_UPDATE_RATE_ * expectedReward_ + 
                     predictionErrorLearningRate_ * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_THRESHOLD_) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, plasticityFactorMin_, plasticityFactorMax_);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_DEBUG("AgentBrain development update skipped - disabled or null brain");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < INITIAL_STAGE_DURATION_) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < CRITICAL_PERIOD_DURATION_) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < MATURATION_DURATION_) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_RATE_BASE_ * plasticityModifier_;
            float pruneRate = PRUNING_RATE_BASE_ * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

// Helper methods for error handling and validation

void AgentBrain::validateBrainState() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain state validation failed: brain pointer is null");
        throw std::runtime_error("AgentBrain: brain pointer is null");
    }
}

bool AgentBrain::validateNeuron(std::vector<Neuron*>& neurons, const std::string& operation) const {
    bool valid = true;
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (!neurons[i]) {
            NLM_LOG_WARNING("AgentBrain::" + operation + ": null neuron at index " + std::to_string(i));
            valid = false;
            neurons[i] = nullptr;  // Keep as nullptr for safety
        }
    }
    return valid;
}

std::vector<Neuron*> AgentBrain::getValidNeurons(const std::vector<Neuron*>& neurons) const {
    std::vector<Neuron*> validNeurons;
    validNeurons.reserve(neurons.size());
    for (Neuron* n : neurons) {
        if (n) validNeurons.push_back(n);
    }
    return validNeurons;
}

float AgentBrain::safeClamp(float value, float min, float max) const {
    if (min > max) {
        NLM_LOG_WARNING("AgentBrain::safeClamp: min > max, swapping values");
        std::swap(min, max);
    }
    return std::clamp(value, min, max);
}

void AgentBrain::logSystemState() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== AgentBrain System State ===");
    NLM_LOG_INFO("Motor neuron groups: Forward=" + std::to_string(motorForward_.size()) +
                 " Backward=" + std::to_string(motorBackward_.size()) +
                 " Left=" + std::to_string(motorTurnLeft_.size()) +
                 " Right=" + std::to_string(motorTurnRight_.size()) +
                 " Interact=" + std::to_string(motorInteract_.size()) +
                 " Wait=" + std::to_string(motorWait_.size()));
    NLM_LOG_INFO("Sensory neuron groups: Vision=" + std::to_string(sensoryVision_.size()) +
                 " Touch=" + std::to_string(sensoryTouch_.size()) +
                 " Internal=" + std::to_string(sensoryInternal_.size()) +
                 " Proprioception=" + std::to_string(sensoryProprioception_.size()));
    NLM_LOG_INFO("Neuromodulation - Dopamine: " + std::to_string(dopamineLevel_) +
                 " Novelty: " + std::to_string(noveltyLevel_) +
                 " Curiosity: " + std::to_string(curiosityLevel_) +
                 " Prediction Error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("Development - Age: " + std::to_string(developmentalAge_) +
                 " Plasticity Modifier: " + std::to_string(plasticityModifier_) +
                 " Stage: " + developmentalStageToString(getDevelopmentalStage()));
    NLM_LOG_INFO("Configuration - Reward Mod: " + std::to_string(rewardModulationEnabled_) +
                 " Structural Plasticity: " + std::to_string(structuralPlasticityEnabled_) +
                 " Development: " + std::to_string(developmentEnabled_) +
                 " Curiosity: " + std::to_string(curiosityEnabled_));
    NLM_LOG_INFO("=====================================");
}

std::string AgentBrain::developmentalStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void AgentBrain::checkMemoryConsistency() const {
    // Check for memory leaks or inconsistencies
    size_t totalNeurons = motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                         motorTurnRight_.size() + motorInteract_.size() + motorWait_.size() +
                         sensoryVision_.size() + sensoryTouch_.size() +
                         sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalNeurons > 1000) {  // Threshold for warning
        NLM_LOG_WARNING("AgentBrain::checkMemoryConsistency: High neuron count (" + 
                       std::to_string(totalNeurons) + ") - possible memory leak");
    }
    
    // Check vector consistency
    if (previousVision_.size() != static_cast<size_t>(getSensoryInputSize())) {
        NLM_LOG_WARNING("AgentBrain::checkMemoryConsistency: previousVision_ size mismatch");
    }
}

void AgentBrain::resetMemoryTracking() {
    previousVision_.clear();
    previousVision_.shrink_to_fit();
    
    // Clear neuron vectors for safety
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    NLM_LOG_DEBUG("AgentBrain memory tracking reset completed");
}

} // namespace nlm

// Performance profiling and optimization implementation

namespace {
    class Timer {
    public:
        Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
            return duration.count() / 1000.0; // Convert to milliseconds
        }
        
        void reset() {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
    };
    
    class PerformanceProfiler {
    public:
        struct Metrics {
            double totalTime = 0.0;
            size_t callCount = 0;
            double avgTime = 0.0;
            double minTime = std::numeric_limits<double>::max();
            double maxTime = 0.0;
            size_t cacheHits = 0;
            size_t cacheMisses = 0;
        };
        
        void record(const std::string& operation, double time) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto& metrics = metrics_[operation];
            metrics.totalTime += time;
            metrics.callCount++;
            metrics.avgTime = metrics.totalTime / metrics.callCount;
            metrics.minTime = std::min(metrics.minTime, time);
            metrics.maxTime = std::max(metrics.maxTime, time);
        }
        
        const Metrics& getMetrics(const std::string& operation) const {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = metrics_.find(operation);
            if (it != metrics_.end()) {
                return it->second;
            }
            static const Metrics emptyMetrics;
            return emptyMetrics;
        }
        
        void printReport() const {
            NLM_LOG_INFO("=== Performance Profiling Report ===");
            for (const auto& pair : metrics_) {
                const auto& metrics = pair.second;
                if (metrics.callCount > 0) {
                    NLM_LOG_INFO(pair.first + ": calls=" + std::to_string(metrics.callCount) +
                                " avg=" + std::to_string(metrics.avgTime) + "ms " +
                                "min=" + std::to_string(metrics.minTime) + "ms " +
                                "max=" + std::to_string(metrics.maxTime) + "ms");
                }
            }
            NLM_LOG_INFO("====================================");
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            metrics_.clear();
        }
        
    private:
        mutable std::mutex mutex_;
        std::unordered_map<std::string, Metrics> metrics_;
    };
    
    PerformanceProfiler globalProfiler;
    
    class SimpleCache {
    public:
        template<typename Key, typename Value>
        Value getOrCompute(const Key& key, std::function<Value()> computeFunc) {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            auto it = cache_.find(key);
            if (it != cache_.end()) {
                cacheHits_++;
                return it->second;
            }
            cacheMisses_++;
            Value value = computeFunc();
            cache_[key] = value;
            return value;
        }
        
        void clear() {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            cache_.clear();
            cacheHits_ = 0;
            cacheMisses_ = 0;
        }
        
        size_t size() const {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            return cache_.size();
        }
        
        double getHitRate() const {
            size_t total = cacheHits_ + cacheMisses_;
            return total > 0 ? static_cast<double>(cacheHits_) / total : 0.0;
        }
        
    private:
        mutable std::mutex cacheMutex_;
        std::unordered_map<size_t, std::any> cache_;
        size_t cacheHits_ = 0;
        size_t cacheMisses_ = 0;
    };
}

void AgentBrain::addPerformanceProfiling() {
    // Enable performance profiling for all major operations
    NLM_LOG_INFO("AgentBrain performance profiling enabled");
    
    // Reset global profiler
    globalProfiler.reset();
    
    // Initialize caching
    performanceCache_.clear();
    
    // Log initial performance state
    NLM_LOG_INFO("Initial cache size: " + std::to_string(performanceCache_.size()));
}

void AgentBrain::printPerformanceReport() const {
    globalProfiler.printReport();
    
    NLM_LOG_INFO("Cache statistics:");
    NLM_LOG_INFO("  Cache size: " + std::to_string(performanceCache_.size()));
    NLM_LOG_INFO("  Hit rate: " + std::to_string(performanceCache_.getHitRate() * 100.0) + "%");
    NLM_LOG_INFO("  Active neuron groups:");
    NLM_LOG_INFO("    Motor: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                     motorTurnLeft_.size() + motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()));
    NLM_LOG_INFO("    Sensory: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                     sensoryInternal_.size() + sensoryProprioception_.size()));
    NLM_LOG_INFO("  Memory state:");
    NLM_LOG_INFO("    previousVision_ size: " + std::to_string(previousVision_.size()));
    NLM_LOG_INFO("    development age: " + std::to_string(developmentalAge_));
}

void AgentBrain::enablePerformanceLogging(bool enable) {
    performanceLoggingEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("AgentBrain performance logging enabled");
    } else {
        NLM_LOG_INFO("AgentBrain performance logging disabled");
    }
}

void AgentBrain::setCacheSize(size_t maxSize) {
    cacheSizeLimit_ = maxSize;
    NLM_LOG_INFO("AgentBrain cache size limit set to: " + std::to_string(maxSize));
}

void AgentBrain::clearCache() {
    performanceCache_.clear();
    NLM_LOG_INFO("AgentBrain cache cleared");
}

void AgentBrain::enableMemoryOptimization(bool enable) {
    memoryOptimizationEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("AgentBrain memory optimization enabled");
    } else {
        NLM_LOG_INFO("AgentBrain memory optimization disabled");
    }
}
