// Improved AgentBrain.cpp with enhanced modularity and extensibility
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>

namespace nlm {

// ========== AGENT BRAIN MODULARITY ENHANCEMENTS ==========

// Plugin system for extensibility
class AgentPlugin {
public:
    virtual ~AgentPlugin() = default;
    virtual std::string getName() const = 0;
    virtual void initialize(std::shared_ptr<AgentBrain> agent) = 0;
    virtual void update(double timestep) = 0;
    virtual void onSensorInput(const SensoryPercept& percept) = 0;
    virtual void onActionProduced(const Action& action) = 0;
    virtual py::dict getStatus() const = 0;
};

// Enhanced sensory processor with plugin architecture
class EnhancedSensoryProcessor {
public:
    struct ProcessingStep {
        std::string name;
        std::function<void(SensoryPercept&)> processor;
        double priority; // Higher priority processes first
    };
    
    void addProcessingStep(const std::string& name, std::function<void(SensoryPercept&)>, double priority = 1.0) {
        steps_.emplace_back(ProcessingStep{name, processor, priority});
        // Sort by priority (highest first)
        std::sort(steps_.begin(), steps_.end(),
                 [](const ProcessingStep& a, const ProcessingStep& b) {
                     return a.priority > b.priority;
                 });
    }
    
    void process(SensoryPercept& percept) {
        for (const auto& step : steps_) {
            step.processor(percept);
        }
    }
    
    std::vector<std::string> getProcessingSteps() const {
        std::vector<std::string> names;
        for (const auto& step : steps_) {
            names.push_back(step.name);
        }
        return names;
    }
    
private:
    std::vector<ProcessingStep> steps_;
};

// Enhanced action selector with strategy pattern
class ActionSelectionStrategy {
public:
    virtual ~ActionSelectionStrategy() = default;
    virtual MotorCommand selectAction(const std::vector<Neuron*>& motorNeurons) = 0;
    virtual std::string getName() const = 0;
};

// Random strategy for exploration
class RandomStrategy : public ActionSelectionStrategy {
public:
    RandomStrategy(std::shared_ptr<RandomGenerator> rng, float explorationRate)
        : rng_(rng), explorationRate_(explorationRate) {}
    
    MotorCommand selectAction(const std::vector<Neuron*>& motorNeurons) override {
        if (rng_->uniformReal(0.0f, 1.0f) < explorationRate_) {
            // Random action selection
            int choice = rng_->uniformInt(0, 7);
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
        
        // Default to wait if not exploring
        return MotorCommand::Wait;
    }
    
    std::string getName() const override {
        return "RandomStrategy";
    }
    
private:
    std::shared_ptr<RandomGenerator> rng_;
    float explorationRate_;
};

// Preference-based strategy
class PreferenceStrategy : public ActionSelectionStrategy {
public:
    PreferenceStrategy(std::shared_ptr<RandomGenerator> rng, 
                      const std::vector<MotorCommand>& preferredActions)
        : rng_(rng), preferredActions_(preferredActions) {}
    
    MotorCommand selectAction(const std::vector<Neuron*>& motorNeurons) override {
        if (preferredActions_.empty()) {
            return MotorCommand::Wait;
        }
        
        // Calculate action preferences based on neuron activity
        std::vector<std::pair<MotorCommand, float>> actionScores;
        
        for (size_t i = 0; i < preferredActions_.size(); ++i) {
            MotorCommand cmd = preferredActions_[i];
            float score = calculateActionScore(cmd, motorNeurons);
            actionScores.emplace_back(cmd, score);
        }
        
        // Select action with highest score
        return selectBestAction(actionScores);
    }
    
    std::string getName() const override {
        return "PreferenceStrategy";
    }
    
private:
    float calculateActionScore(MotorCommand cmd, const std::vector<Neuron*>& motorNeurons) {
        // Calculate score based on neural activity
        float totalScore = 0.0f;
        
        // Find motor neurons corresponding to this action
        std::vector<Neuron*> actionNeurons;
        for (Neuron* neuron : motorNeurons) {
            // In a real implementation, this would map neurons to actions
            // For now, use a simple heuristic
            totalScore += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        
        return totalScore / std::max(1.0f, static_cast<float>(motorNeurons.size()));
    }
    
    MotorCommand selectBestAction(std::vector<std::pair<MotorCommand, float>>& actionScores) {
        if (actionScores.empty()) {
            return MotorCommand::Wait;
        }
        
        // Find maximum score
        auto maxIt = std::max_element(actionScores.begin(), actionScores.end(),
                                    [](const auto& a, const auto& b) {
                                        return a.second < b.second;
                                    });
        
        return maxIt->first;
    }
    
    std::shared_ptr<RandomGenerator> rng_;
    std::vector<MotorCommand> preferredActions_;
};

// Neuromodulation coordinator with plugin system
class NeuromodulationCoordinator {
public:
    struct NeuromodulatorConfig {
        std::string name;
        float baseline;
        float range;
        std::function<float(const std::vector<float>&)> modifier;
        bool enabled = true;
    };
    
    NeuromodulationCoordinator() {
        // Register default neuromodulators
        registerNeuromodulator("dopamine", 0.1f, 0.5f,
                               [this](const std::vector<float>& signals) { return calculateDopamineLevel(signals); });
        registerNeuromodulator("curiosity", 0.0f, 0.3f,
                               [this](const std::vector<float>& signals) { return calculateCuriosityLevel(signals); });
        registerNeuromodulator("novelty", 0.0f, 0.2f,
                               [this](const std::vector<float>& signals) { return calculateNoveltyLevel(signals); });
        registerNeuromodulator("prediction_error", 0.0f, 0.5f,
                               [this](const std::vector<float>& signals) { return calculatePredictionError(signals); });
    }
    
    void registerNeuromodulator(const std::string& name, float baseline, float range,
                                std::function<float(const std::vector<float>&)> modifier,
                                bool enabled = true) {
        neuromodulators_[name] = NeuromodulatorConfig{name, baseline, range, modifier, enabled};
    }
    
    void updateNeuromodulators(const std::vector<float>& signals) {
        for (auto& [name, config] : neuromodulators_) {
            if (config.enabled) {
                float level = config.modifier(signals);
                neuromodulatorLevels_[name] = level;
                
                // Apply to brain if available
                if (brain_) {
                    applyNeuromodulatorToBrain(name, level);
                }
            }
        }
    }
    
    float getNeuromodulatorLevel(const std::string& name) const {
        auto it = neuromodulatorLevels_.find(name);
        return it != neuromodulatorLevels_.end() ? it->second : 0.0f;
    }
    
    std::vector<std::string> getNeuromodulatorNames() const {
        std::vector<std::string> names;
        for (const auto& [name, config] : neuromodulators_) {
            names.push_back(name);
        }
        return names;
    }
    
    void enableNeuromodulator(const std::string& name, bool enabled) {
        auto it = neuromodulators_.find(name);
        if (it != neuromodulators_.end()) {
            it->second.enabled = enabled;
        }
    }
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    py::dict getStatus() const {
        py::dict status;
        for (const auto& [name, level] : neuromodulatorLevels_) {
            status[name.c_str()] = level;
        }
        return status;
    }
    
private:
    std::unordered_map<std::string, NeuromodulatorConfig> neuromodulators_;
    std::unordered_map<std::string, float> neuromodulatorLevels_;
    std::shared_ptr<Brain> brain_;
    
    float calculateDopamineLevel(const std::vector<float>& signals) {
        // Calculate dopamine level based on reward prediction error
        if (signals.empty()) return 0.0f;
        
        float total = 0.0f;
        for (float signal : signals) {
            total += signal;
        }
        return std::clamp(total / signals.size(), -1.0f, 1.0f);
    }
    
    float calculateCuriosityLevel(const std::vector<float>& signals) {
        // Calculate curiosity based on novelty and uncertainty
        if (signals.empty()) return 0.0f;
        
        float novelty = *std::max_element(signals.begin(), signals.end(),
                                          [](float a, float b) { return std::abs(a) < std::abs(b); });
        float uncertainty = calculateEntropy(signals);
        return novelty * uncertainty * 0.5f;
    }
    
    float calculateNoveltyLevel(const std::vector<float>& signals) {
        // Calculate novelty based on deviation from expected values
        if (signals.empty()) return 0.0f;
        
        float mean = std::accumulate(signals.begin(), signals.end(), 0.0f) / signals.size();
        float variance = 0.0f;
        for (float signal : signals) {
            variance += (signal - mean) * (signal - mean);
        }
        variance /= signals.size();
        
        return std::sqrt(variance) * 0.3f;
    }
    
    float calculatePredictionError(const std::vector<float>& signals) {
        // Calculate prediction error
        if (signals.empty()) return 0.0f;
        
        float total = 0.0f;
        for (float signal : signals) {
            total += std::abs(signal);
        }
        return total / signals.size();
    }
    
    float calculateEntropy(const std::vector<float>& values) {
        // Calculate entropy of the distribution
        if (values.empty()) return 0.0f;
        
        std::unordered_map<float, int> frequency;
        for (float value : values) {
            frequency[value]++;
        }
        
        float entropy = 0.0f;
        for (const auto& [value, count] : frequency) {
            float probability = count / values.size();
            entropy -= probability * std::log(probability + 1e-10f);
        }
        
        return entropy;
    }
    
    void applyNeuromodulatorToBrain(const std::string& name, float level) {
        if (!brain_) return;
        
        // Apply neuromodulator effects based on type
        if (name == "dopamine") {
            applyDopamineEffects(level);
        } else if (name == "curiosity") {
            applyCuriosityEffects(level);
        } else if (name == "novelty") {
            applyNoveltyEffects(level);
        } else if (name == "prediction_error") {
            applyPredictionErrorEffects(level);
        }
    }
    
    void applyDopamineEffects(float level) {
        // Dopamine modulates learning and exploration
        float plasticityMod = 0.5f + 0.5f * level;
        plasticityMod = std::clamp(plasticityMod, 0.1f, 2.0f);
        
        // Apply to brain systems
        if (auto* stdp = brain_->getSTDP()) {
            stdp->setLTPWeight(0.01f * plasticityMod);
            stdp->setLTDWeight(0.012f * plasticityMod);
        }
    }
    
    void applyCuriosityEffects(float level) {
        // Curiosity drives exploration
        // In a real implementation, this would affect action selection
    }
    
    void applyNoveltyEffects(float level) {
        // Novelty affects attention and memory
        // In a real implementation, this would affect cognitive processes
    }
    
    void applyPredictionErrorEffects(float level) {
        // Prediction error affects learning
        // In a real implementation, this would affect plasticity
    }
};

// Performance monitor with statistics
class PerformanceMonitor {
public:
    struct PerformanceStats {
        size_t stepsProcessed = 0;
        size_t spikesDetected = 0;
        double averageStepTime = 0.0;
        double totalSimulationTime = 0.0;
        size_t memoryUsage = 0;
        size_t cpuUsage = 0;
        
        void reset() {
            stepsProcessed = 0;
            spikesDetected = 0;
            averageStepTime = 0.0;
            totalSimulationTime = 0.0;
            memoryUsage = 0;
            cpuUsage = 0;
        }
    };
    
    void recordStep(double stepTime) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        stats_.stepsProcessed++;
        stats_.averageStepTime = (stats_.averageStepTime * (stats_.stepsProcessed - 1) + stepTime) / stats_.stepsProcessed;
        stats_.totalSimulationTime += stepTime;
        
        // Update memory usage (approximate)
        stats_.memoryUsage = getMemoryUsage();
        
        // Record spike count
        stats_.spikesDetected = getSpikeCount();
    }
    
    PerformanceStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    std::string getPerformanceReport() const {
        std::ostringstream oss;
        oss << "=== Agent Brain Performance Monitor ===" << std::endl;
        oss << "Steps processed: " << stats_.stepsProcessed << std::endl;
        oss << "Average step time: " << stats_.averageStepTime * 1000 << " ms" << std::endl;
        oss << "Total simulation time: " << stats_.totalSimulationTime * 1000 << " ms" << std::endl;
        oss << "Spikes detected: " << stats_.spikesDetected << std::endl;
        oss << "Memory usage: " << stats_.memoryUsage / 1024 / 1024 << " MB" << std::endl;
        oss << "Performance: " << (stats_.stepsProcessed / std::max(1.0, stats_.totalSimulationTime)) << " steps/second" << std::endl;
        
        return oss.str();
    }
    
private:
    mutable std::mutex mutex_;
    PerformanceStats stats_;
    
    size_t getMemoryUsage() {
        // Approximate memory usage
        return 1024 * 1024; // 1MB (placeholder)
    }
    
    size_t getSpikeCount() {
        // In a real implementation, this would get the spike count from the brain
        return 0; // placeholder
    }
};

// ========== ENHANCED AGENT BRAIN IMPLEMENTATION ==========

struct AgentBrain::Impl {
    std::shared_ptr<Brain> brain_;
    
    // Enhanced sensory processor
    EnhancedSensoryProcessor sensoryProcessor;
    
    // Action selection strategies
    std::vector<std::unique_ptr<ActionSelectionStrategy>> strategies_;
    std::string activeStrategyName;
    
    // Neuromodulation coordinator
    NeuromodulationCoordinator neuromodulationCoordinator;
    
    // Performance monitor
    PerformanceMonitor performanceMonitor;
    
    // Plugin system
    std::vector<std::unique_ptr<AgentPlugin>> plugins_;
    
    // Advanced memory
    std::unordered_map<std::string, py::object> customMemory_;
    
    // Configuration
    bool curiosityEnabled_;
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    
    // Internal state
    std::vector<float> lastSensorySignals_;
    std::vector<MotorCommand> actionHistory_;
    size_t episodeSteps_;
    float episodeReward_;
    
    Impl(std::shared_ptr<Brain> brain) 
        : brain_(brain)
        , curiosityEnabled_(true)
        , rewardModulationEnabled_(true)
        , structuralPlasticityEnabled_(true)
        , developmentEnabled_(true)
        , episodeSteps_(0)
        , episodeReward_(0.0f) {
        
        // Set up enhanced components
        neuromodulationCoordinator.setBrain(brain_);
        
        // Add default processing steps
        setupDefaultSensoryProcessing();
        
        // Add default action strategies
        setupDefaultActionStrategies();
        
        // Set active strategy
        activeStrategyName = "RandomStrategy";
        
        // Initialize plugins
        initializePlugins();
    }
    
private:
    void setupDefaultSensoryProcessing() {
        // Vision processing
        sensoryProcessor.addProcessingStep("vision_normalization",
            [](SensoryPercept& percept) {
                auto vision = percept.getVision();
                for (size_t i = 0; i < vision.size(); ++i) {
                    vision[i] = std::clamp(vision[i], 0.0f, 1.0f);
                }
            }, 2.0f);
        
        // Touch processing
        sensoryProcessor.addProcessingStep("touch_filtering",
            [](SensoryPercept& percept) {
                auto touch = percept.getTouch();
                for (size_t i = 0; i < touch.size(); ++i) {
                    // Apply smoothing filter
                    if (i > 0) {
                        touch[i] = (touch[i] + touch[i-1]) * 0.5f;
                    }
                }
            }, 1.5f);
        
        // Internal signals processing
        sensoryProcessor.addProcessingStep("internal_processing",
            [](SensoryPercept& percept) {
                auto internal = percept.getInternal();
                for (size_t i = 0; i < internal.size(); ++i) {
                    // Center and scale internal signals
                    internal[i] = (internal[i] * 2.0f - 1.0f) * 0.5f;
                }
            }, 1.8f);
    }
    
    void setupDefaultActionStrategies() {
        // Create random strategy with exploration
        if (brain_) {
            auto rng = brain_->getRandomGenerator();
            if (rng) {
                auto randomStrategy = std::make_unique<RandomStrategy>(rng, 0.1f);
                strategies_.push_back(std::move(randomStrategy));
                
                auto preferenceStrategy = std::make_unique<PreferenceStrategy>(*rng, 
                    std::vector<MotorCommand>{MotorCommand::MoveForward, MotorCommand::TurnLeft, MotorCommand::Interact});
                strategies_.push_back(std::move(preferenceStrategy));
            }
        }
    }
    
    void initializePlugins() {
        // In a real implementation, this would load plugins dynamically
        // For now, just add a basic performance monitoring plugin
        plugins_.push_back(std::make_unique<PerformanceMonitoringPlugin>(this));
    }
    
    void updateNeuromodulators() {
        // Gather current sensory signals
        std::vector<float> signals;
        
        // Collect signals from different sensory modalities
        if (!lastSensorySignals_.empty()) {
            signals = lastSensorySignals_;
        }
        
        // Update neuromodulators
        neuromodulationCoordinator.updateNeuromodulators(signals);
    }
    
    MotorCommand selectActionWithStrategy(const std::vector<Neuron*>& motorNeurons) {
        // Find active strategy
        for (const auto& strategy : strategies_) {
            if (strategy->getName() == activeStrategyName) {
                return strategy->selectAction(motorNeurons);
            }
        }
        
        // Fallback to random strategy
        if (!strategies_.empty()) {
            return strategies_[0]->selectAction(motorNeurons);
        }
        
        return MotorCommand::Wait;
    }
    
    void updateActionHistory(MotorCommand action) {
        actionHistory_.push_back(action);
        
        // Keep only recent history
        if (actionHistory_.size() > 100) {
            actionHistory_.erase(actionHistory_.begin());
        }
    }
    
    float calculateEpisodeStatistics() {
        if (episodeSteps_ == 0) return 0.0f;
        
        // Calculate average reward per step
        return episodeReward_ / episodeSteps_;
    }
    
    void resetEpisodeState() {
        episodeSteps_ = 0;
        episodeReward_ = 0.0f;
        actionHistory_.clear();
        lastSensorySignals_.clear();
    }
};

// Performance monitoring plugin
class PerformanceMonitoringPlugin : public AgentPlugin {
public:
    PerformanceMonitoringPlugin(AgentBrain::Impl* impl) : impl_(impl) {}
    
    std::string getName() const override {
        return "PerformanceMonitoringPlugin";
    }
    
    void initialize(std::shared_ptr<AgentBrain> agent) override {
        // Initialize performance monitoring
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void update(double timestep) override {
        // Update performance metrics
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        
        if (impl_) {
            impl_->performanceMonitor.recordStep(duration.count() / 1000.0); // Convert to ms
        }
    }
    
    void onSensorInput(const SensoryPercept& percept) override {
        // Update sensory signal statistics
        if (impl_) {
            impl_->lastSensorySignals_.clear();
            
            auto vision = percept.getVision();
            impl_->lastSensorySignals_.insert(impl_->lastSensorySignals_.end(),
                                            vision.begin(), vision.end());
            
            auto touch = percept.getTouch();
            impl_->lastSensorySignals_.insert(impl_->lastSensorySignals_.end(),
                                            touch.begin(), touch.end());
        }
    }
    
    void onActionProduced(const Action& action) override {
        // Update action history
        if (impl_) {
            impl_->updateActionHistory(action.getType());
        }
    }
    
    py::dict getStatus() const override {
        py::dict status;
        status["plugin_name"] = getName();
        status["steps_processed"] = impl_->episodeSteps_;
        status["current_reward"] = impl_->episodeReward_;
        status["average_reward"] = calculateEpisodeStatistics();
        
        if (impl_) {
            auto perfStats = impl_->performanceMonitor.getStats();
            status["performance"] = perfStats.averageStepTime;
            status["spikes"] = perfStats.spikesDetected;
        }
        
        return status;
    }
    
private:
    AgentBrain::Impl* impl_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    float calculateEpisodeStatistics() const {
        if (!impl_ || impl_->episodeSteps_ == 0) return 0.0f;
        return impl_->episodeReward_ / impl_->episodeSteps_;
    }
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(brain)) {}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    pImpl->sensoryVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    pImpl->episodeSteps_ = 0;
    pImpl->episodeReward_ = 0.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(pImpl->sensoryVision_.size()) + " vision sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!pImpl->brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < pImpl->sensoryVision_.size() && i < vision.size(); ++i) {
        if (pImpl->sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            pImpl->sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < pImpl->sensoryTouch_.size() && i < touch.size(); ++i) {
        if (pImpl->sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            pImpl->sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < pImpl->sensoryInternal_.size() && i < intern.size(); ++i) {
        if (pImpl->sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            pImpl->sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < pImpl->sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (pImpl->sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            pImpl->sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Store sensory signals for neuromodulation
    pImpl->lastSensorySignals_.clear();
    pImpl->lastSensorySignals_.insert(pImpl->lastSensorySignals_.end(), vision.begin(), vision.end());
    pImpl->lastSensorySignals_.insert(pImpl->lastSensorySignals_.end(), touch.begin(), touch.end());
    pImpl->lastSensorySignals_.insert(pImpl->lastSensorySignals_.end(), intern.begin(), intern.end());
    pImpl->lastSensorySignals_.insert(pImpl->lastSensorySignals_.end(), proprio.begin(), proprio.end());
    
    // Update neuromodulators
    pImpl->updateNeuromodulators();
    
    // Call plugins
    for (auto& plugin : pImpl->plugins_) {
        plugin->onSensorInput(percept);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!pImpl->brain_) return MotorCommand::Wait;
    
    // Get motor neurons for action selection
    std::vector<Neuron*> motorNeurons;
    for (const auto& region : pImpl->brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    motorNeurons.push_back(n);
                }
            }
        }
    }
    
    // Select action using current strategy
    MotorCommand action = pImpl->selectActionWithStrategy(motorNeurons);
    
    // Update action history
    pImpl->updateActionHistory(action);
    
    // Update episode statistics
    pImpl->episodeSteps_++;
    
    return action;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!pImpl->brain_ || !pImpl->rewardModulationEnabled_) return;
    
    // Compute prediction error
    float predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward_ = 0.95f * pImpl->expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    pImpl->dopamineLevel_ = predictionError;
    
    // Clamp to reasonable range
    pImpl->dopamineLevel_ = std::clamp(pImpl->dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : pImpl->brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * pImpl->dopamineLevel_ * pImpl->plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Update episode reward
    pImpl->episodeReward_ += reward;
    
    // Apply neuromodulator effects
    pImpl->updateNeuromodulators();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!pImpl->brain_ || !pImpl->developmentEnabled_) return;
    
    pImpl->developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (pImpl->developmentalAge_ < 60.0) {  // ~1 minute
        pImpl->plasticityModifier_ = 1.0f;  // High plasticity
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (pImpl->developmentalAge_ < 300.0) {  // ~5 minutes
        pImpl->plasticityModifier_ = 0.8f;
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (pImpl->developmentalAge_ < 900.0) {  // ~15 minutes
        pImpl->plasticityModifier_ = 0.5f;
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        pImpl->plasticityModifier_ = 0.2f;  // Adult - more stable
        pImpl->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (pImpl->structuralPlasticityEnabled_) {
        auto* sp = pImpl->brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * pImpl->plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - pImpl->plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!pImpl->brain_) return DevelopmentalStage::Initial;
    return pImpl->brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return pImpl->dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return pImpl->curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return pImpl->noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return pImpl->predictionError_;
}

void AgentBrain::reset() {
    pImpl->dopamineLevel_ = 0.0f;
    pImpl->noveltyLevel_ = 0.0f;
    pImpl->curiosityLevel_ = 0.0f;
    pImpl->predictionError_ = 0.0f;
    pImpl->expectedReward_ = 0.0f;
    pImpl->developmentalAge_ = 0.0;
    pImpl->plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(pImpl->previousVision_.begin(), pImpl->previousVision_.end(), 0.0f);
    
    // Reset episode state
    pImpl->resetEpisodeState();
}

void AgentBrain::enableRewardModulation(bool enable) {
    pImpl->rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    pImpl->structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    pImpl->developmentEnabled_ = enable;
}

void AgentBrain::enableCuriosity(bool enable) {
    pImpl->curiosityEnabled_ = enable;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return pImpl->rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return pImpl->structuralPlasticityEnabled_;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return pImpl->developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return pImpl->curiosityEnabled_;
}

std::shared_ptr<Brain> AgentBrain::getBrain() const {
    return pImpl->brain_;
}

// Helper function for enhanced Python bindings
namespace {
    // Register enhanced Python bindings
    void registerEnhancedAgentBrainBindings(py::module& m) {
        // Register enhanced AgentBrain with additional methods
        py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "EnhancedAgentBrain", 
            py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
            
            // Enhanced sensory processing
            .def("addProcessingStep", [](AgentBrain& self, const std::string& name, 
                                      std::function<void(SensoryPercept&)> processor, double priority) {
                self.pImpl->sensoryProcessor.addProcessingStep(name, processor, priority);
            }, py::arg("name"), py::arg("processor"), py::arg("priority") = 1.0,
               "Add a new sensory processing step")
            
            .def("getProcessingSteps", &AgentBrain::pImpl->sensoryProcessor.getProcessingSteps,
                 "Get list of registered processing steps")
            
            // Action strategy management
            .def("setActionStrategy", [](AgentBrain& self, const std::string& strategyName) {
                self.pImpl->activeStrategyName = strategyName;
            }, py::arg("strategy_name"), "Set active action selection strategy")
            
            .def("getAvailableStrategies", []() {
                std::vector<std::string> strategies = {"RandomStrategy", "PreferenceStrategy"};
                return strategies;
            }, "Get available action selection strategies")
            
            // Neuromodulation control
            .def("registerNeuromodulator", [](AgentBrain& self, const std::string& name, float baseline,
                                            float range, std::function<float(const std::vector<float>&)> modifier) {
                self.pImpl->neuromodulationCoordinator.registerNeuromodulator(name, baseline, range, modifier);
            }, py::arg("name"), py::arg("baseline"), py::arg("range"), py::arg("modifier"),
               "Register a new neuromodulator")
            
            .def("enableNeuromodulator", &AgentBrain::pImpl->neuromodulationCoordinator.enableNeuromodulator,
                 py::arg("name"), py::arg("enabled"), "Enable or disable a neuromodulator")
            
            .def("getNeuromodulatorNames", &AgentBrain::pImpl->neuromodulationCoordinator.getNeuromodulatorNames,
                 "Get names of registered neuromodulators")
            
            // Performance monitoring
            .def("getPerformanceReport", [](const AgentBrain& self) {
                return self.pImpl->performanceMonitor.getPerformanceReport();
            }, "Get performance monitoring report")
            
            .def("getNeuromodulationStatus", &AgentBrain::pImpl->neuromodulationCoordinator.getStatus,
                 "Get current neuromodulation levels")
            
            // Plugin management
            .def("getPluginNames", []() {
                std::vector<std::string> plugins = {"PerformanceMonitoringPlugin"};
                return plugins;
            }, "Get available plugins")
            
            .def("getCustomMemory", [](AgentBrain& self, const std::string& key) {
                auto it = self.pImpl->customMemory_.find(key);
                if (it != self.pImpl->customMemory_.end()) {
                    return it->second;
                }
                return py::none();
            }, py::arg("key"), "Get custom memory data")
            
            .def("setCustomMemory", [](AgentBrain& self, const std::string& key, py::object value) {
                self.pImpl->customMemory_[key] = value;
            }, py::arg("key"), py::arg("value"), "Set custom memory data")
            
            // Advanced configuration
            .def("enableCuriosity", &AgentBrain::enableCuriosity,
                 py::arg("enable"), "Enable or disable curiosity-driven exploration")
            
            .def("enableDevelopment", &AgentBrain::enableDevelopment,
                 py::arg("enable"), "Enable or disable developmental processes")
            
            .def("getEpisodeStatistics", [](const AgentBrain& self) {
                float averageReward = self.pImpl->episodeSteps_ > 0 ? 
                    self.pImpl->episodeReward_ / self.pImpl->episodeSteps_ : 0.0f;
                py::dict stats;
                stats["steps"] = self.pImpl->episodeSteps_;
                stats["total_reward"] = self.pImpl->episodeReward_;
                stats["average_reward"] = averageReward;
                return stats;
            }, "Get episode statistics");
    }
}

// Register enhanced bindings
void registerEnhancedBindings(py::module& m) {
    registerEnhancedAgentBrainBindings(m);
}

} // namespace nlm

// C++ to Python conversion functions for enhanced bindings
namespace nlm {
    // Enhanced helper functions
    namespace {
        // Convert Enhanced AgentBrain to Python
        py::object convertEnhancedAgentBrainToPython(std::shared_ptr<AgentBrain> agentBrain) {
            // In a real implementation, this would create a Python wrapper
            // For now, return None to avoid compilation errors
            return py::none();
        }
        
        // Enhanced configuration loading
        std::shared_ptr<Config> loadConfigFromPythonDict(const py::dict& configDict) {
            auto config = std::make_shared<Config>();
            
            // Convert Python dict to config
            for (auto it : configDict) {
                std::string key = py::cast<std::string>(it.first);
                py::object value = it.second;
                
                if (value.is(py::bool_())) {
                    config->set(key, value.cast<bool>());
                } else if (value.is(py::int_())) {
                    config->set(key, value.cast<int64_t>());
                } else if (value.is(py::float_())) {
                    config->set(key, value.cast<double>());
                } else if (value.is(py::str())) {
                    config->set(key, value.cast<std::string>());
                }
            }
            
            return config;
        }
        
        // Enhanced brain creation with Python interface
        std::shared_ptr<PythonBrainWrapper> createPythonBrain(const py::dict& configDict) {
            auto config = loadConfigFromPythonDict(configDict);
            return std::make_shared<PythonBrainWrapper>(config);
        }
    }
}

// Register enhanced factory functions
void registerEnhancedFactoryFunctions(py::module& m) {
    m.def("createEnhancedAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an enhanced agent brain with plugin support");
    
    m.def("createPythonBrain", &nlm::createPythonBrain,
          py::arg("config_dict"), "Create a Python-wrapped brain with enhanced interface");
    
    m.def("loadConfigFromPythonDict", &nlm::loadConfigFromPythonDict,
          py::arg("config_dict"), "Load configuration from Python dictionary");
    
    m.def("createActionStrategy", [](const std::string& strategyName, 
                                    std::shared_ptr<RandomGenerator> rng,
                                    const std::vector<MotorCommand>& preferredActions) {
        if (strategyName == "Random") {
            return std::make_shared<RandomStrategy>(rng, 0.1f);
        } else if (strategyName == "Preference") {
            return std::make_shared<PreferenceStrategy>(rng, preferredActions);
        }
        return std::shared_ptr<ActionSelectionStrategy>();
    }, py::arg("strategy_name"), py::arg("rng"), py::arg("preferred_actions"),
       "Create an action selection strategy");
}

// Main module binding function
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        An advanced Python interface for the NLM neural simulation framework.
        Provides enhanced modularity, plugin support, and advanced configuration management.
    )pbdoc";
    
    // Register enhanced core classes
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system)pbdoc")
        .def(py::init<>())
        .def("loadFromDict", &nlm::loadConfigFromPythonDict,
             py::arg("config_dict"), "Load configuration from Python dictionary")
        .def("saveToDict", [](const Config& self) {
            py::dict dict;
            for (const auto& key : self.getKeys()) {
                auto stringVal = self.get<std::string>(key);
                if (stringVal.has_value()) {
                    dict[key.c_str()] = stringVal.value();
                } else {
                    auto intVal = self.get<int64_t>(key);
                    if (intVal.has_value()) {
                        dict[key.c_str()] = intVal.value();
                    } else {
                        auto doubleVal = self.get<double>(key);
                        if (doubleVal.has_value()) {
                            dict[key.c_str()] = doubleVal.value();
                        }
                    }
                }
            }
            return dict;
        }, "Convert configuration to Python dictionary");
    
    // Register enhanced AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "EnhancedAgentBrain", 
        py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, 
             py::arg("reward"), py::arg("predicted_reward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation)
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity)
        .def("enableDevelopment", &AgentBrain::enableDevelopment)
        .def("enableCuriosity", &AgentBrain::enableCuriosity)
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getBrain", &AgentBrain::getBrain);
    
    // Register enhanced factory functions
    registerEnhancedFactoryFunctions(m);
    
    // Register example usage
    m.def("example_enhanced_usage", []() {
        std::cout << R"PYTHON
# Enhanced NLM usage example
import pynlm

# Create enhanced configuration
config_dict = {
    "neuron_count": 5000,
    "region_count": 4,
    "simulation_timestep": 0.0005,
    "log_level": "DEBUG",
    "curiosity_enabled": True,
    "development_enabled": True,
    "reward_modulation_enabled": True
}

# Create enhanced brain
brain = pynlm.createPythonBrain(config_dict)
brain.initialize()

# Create enhanced agent brain
agent_brain = pynlm.createEnhancedAgentBrain(brain)

# Set up action strategy
agent_brain.setActionStrategy("Preference")

# Add sensory processing step
agent_brain.addProcessingStep("vision_enhanced",
    lambda percept: print("Processing vision: " + len(percept.getVision())))

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step} completed")

# Get enhanced status
print("=== Enhanced Status ===")
print(f"Developmental stage: {agent_brain.getDevelopmentalStage()}")
print(f"Neuromodulation level: {agent_brain.getNeuromodulationLevel()}")
print(f"Performance report: {agent_brain.getPerformanceReport()}")

brain.reset()
PYTHON" << std::endl;
    }, "Show enhanced usage example");
    
    m.def("version", []() {
        return "pynlm_enhanced 0.1.0 (Modularity Enhanced)";
    }, "Get version information");
}

} // namespace nlm

// Main function for enhanced testing
int main(int argc, char** argv) {
    nlm::Config config;
    
    // Load from command line arguments
    if (!config.loadFromArgs(argc, argv)) {
        if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
            config.printHelp();
        }
        return 1;
    }
    
    // Create brain and initialize
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return 1;
    }
    
    // Create enhanced agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Set up enhanced features
    agentBrain->enableCuriosity(true);
    agentBrain->enableDevelopment(true);
    agentBrain->enableRewardModulation(true);
    
    // Show enhanced usage example
    std::cout << "Enhanced NLM AgentBrain initialized" << std::endl;
    std::cout << "Features enabled:" << std::endl;
    std::cout << "  - Curiosity-driven exploration" << std::endl;
    std::cout << "  - Developmental processes" << std::endl;
    std::cout << "  - Reward modulation" << std::endl;
    std::cout << "  - Enhanced sensory processing" << std::endl;
    std::cout << "  - Action strategy selection" << std::endl;
    std::cout << "  - Neuromodulation coordination" << std::endl;
    
    // Run simulation
    for (size_t step = 0; step < config.getOr<size_t>("max_simulation_steps", 100); ++step) {
        brain->step(step);
        
        if (step % 50 == 0) {
            std::cout << "Step " << step << " completed" << std::endl;
            agentBrain->reset();
        }
    }
    
    std::cout << "Enhanced simulation completed successfully" << std::endl;
    return 0;
}

#endif // AGENT_BRAIN_ENHANCED
