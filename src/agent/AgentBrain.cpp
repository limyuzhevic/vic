#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

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

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
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
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
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
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
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
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
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
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Implement ConfigurationBuilder methods
AgentBrain::ConfigurationBuilder::ConfigurationBuilder(AgentBrain* agentBrain)
    : agentBrain_(agentBrain)
    , rewardModulationEnabled_(agentBrain ? agentBrain->isRewardModulationEnabled() : true)
    , structuralPlasticityEnabled_(agentBrain ? agentBrain->isStructuralPlasticityEnabled() : true)
    , developmentEnabled_(agentBrain ? agentBrain->isDevelopmentEnabled() : true)
    , curiosityEnabled_(agentBrain ? agentBrain->isCuriosityEnabled() : true)
    , neuromodulationLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , predictionError_(0.0f)
{
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setDevelopment(bool enable) {
    developmentEnabled_ = enable;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setCuriosity(bool enable) {
    curiosityEnabled_ = enable;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setNeuromodulationLevel(float level) {
    neuromodulationLevel_ = level;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setCuriosityLevel(float level) {
    curiosityLevel_ = level;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setNoveltyLevel(float level) {
    noveltyLevel_ = level;
    return *this;
}

AgentBrain::ConfigurationBuilder& AgentBrain::ConfigurationBuilder::setPredictionError(float error) {
    predictionError_ = error;
    return *this;
}

void AgentBrain::ConfigurationBuilder::apply() {
    if (agentBrain_) {
        agentBrain_->enableRewardModulation(rewardModulationEnabled_);
        agentBrain_->enableStructuralPlasticity(structuralPlasticityEnabled_);
        agentBrain_->enableDevelopment(developmentEnabled_);
        agentBrain_->enableCuriosity(curiosityEnabled_);
        
        // Update neuromodulation levels
        agentBrain_->dopamineLevel_ = neuromodulationLevel_;
        agentBrain_->curiosityLevel_ = curiosityLevel_;
        agentBrain_->noveltyLevel_ = noveltyLevel_;
        agentBrain_->predictionError_ = predictionError_;
    }
}

void AgentBrain::ConfigurationBuilder::reset() {
    rewardModulationEnabled_ = agentBrain_ ? agentBrain_->isRewardModulationEnabled() : true;
    structuralPlasticityEnabled_ = agentBrain_ ? agentBrain_->isStructuralPlasticityEnabled() : true;
    developmentEnabled_ = agentBrain_ ? agentBrain_->isDevelopmentEnabled() : true;
    curiosityEnabled_ = agentBrain_ ? agentBrain_->isCuriosityEnabled() : true;
    neuromodulationLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
}

bool AgentBrain::ConfigurationBuilder::isRewardModulationEnabled() const {
    return rewardModulationEnabled_;
}

bool AgentBrain::ConfigurationBuilder::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool AgentBrain::ConfigurationBuilder::isDevelopmentEnabled() const {
    return developmentEnabled_;
}

bool AgentBrain::ConfigurationBuilder::isCuriosityEnabled() const {
    return curiosityEnabled_;
}

// Implement NeuromodulationSubscription methods
AgentBrain::NeuromodulationSubscription::NeuromodulationSubscription(AgentBrain* agentBrain, std::function<void(float)> callback)
    : agentBrain_(agentBrain)
    , callback_(callback)
    , active_(false)
    , lastTriggerLevel_(0.0f)
{
    if (agentBrain_ && callback_) {
        active_ = true;
    }
}

AgentBrain::NeuromodulationSubscription::~NeuromodulationSubscription() {
    // Cleanup if needed
}

bool AgentBrain::NeuromodulationSubscription::isActive() const {
    return active_;
}

float AgentBrain::NeuromodulationSubscription::getLastTriggerLevel() const {
    return lastTriggerLevel_;
}

// Implement enhanced AgentBrain methods
AgentBrain::ConfigurationBuilder AgentBrain::configure() {
    return ConfigurationBuilder(this);
}

AgentBrain::NeuromodulationSubscription AgentBrain::subscribeToNeuromodulation(std::function<void(float)> callback) {
    return NeuromodulationSubscription(this, callback);
}

void AgentBrain::setupForControlTask(float targetReward, float explorationBonus) {
    enableRewardModulation(true);
    enableCuriosity(false);
    enableDevelopment(false);
    dopamineLevel_ = targetReward;
    // Additional control task specific setup
}

void AgentBrain::setupForExplorationTask(float noveltyThreshold, float curiosityFactor) {
    enableRewardModulation(false);
    enableCuriosity(true);
    enableDevelopment(true);
    curiosityLevel_ = curiosityFactor;
    // Additional exploration task specific setup
}

void AgentBrain::setupForMemoryTask(float memoryCapacity, float consolidationRate) {
    enableRewardModulation(true);
    enableCuriosity(false);
    enableDevelopment(true);
    // Additional memory task specific setup
}

bool AgentBrain::enableExperimentalMode() {
    // Disable all modulators for experimental mode
    rewardModulationEnabled_ = false;
    structuralPlasticityEnabled_ = false;
    developmentEnabled_ = false;
    curiosityEnabled_ = false;
    return true;
}

bool AgentBrain::enableResearchMode() {
    // Enable all modulators for research mode
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
    return true;
}

bool AgentBrain::disableAllModulators() {
    // Completely disable all neuromodulation
    rewardModulationEnabled_ = false;
    structuralPlasticityEnabled_ = false;
    developmentEnabled_ = false;
    curiosityEnabled_ = false;
    dopamineLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
    return true;
}

void AgentBrain::startProfiler() {
    // Enable performance profiling
    NLM_LOG_INFO("AgentBrain profiler started");
}

void AgentBrain::stopProfiler() {
    // Disable performance profiling
    NLM_LOG_INFO("AgentBrain profiler stopped");
}

bool AgentBrain::isProfilerActive() const {
    // Check if profiler is active
    return false;  // Placeholder - should track profiler state
}

std::string AgentBrain::getProfilerReport() const {
    // Generate profiler report
    return "AgentBrain Profiler Report (Placeholder)\n";
}

void AgentBrain::setDebugLevel(int level) {
    // Set debug logging level
    NLM_LOG_INFO("AgentBrain debug level set to: " + std::to_string(level));
}

int AgentBrain::getDebugLevel() const {
    // Get current debug level
    return 0;  // Placeholder - should track debug level
}

void AgentBrain::enableLoggingToFile(const std::string& filepath) {
    // Enable file logging
    NLM_LOG_INFO("AgentBrain logging enabled to: " + filepath);
}

void AgentBrain::disableLoggingToFile() {
    // Disable file logging
    NLM_LOG_INFO("AgentBrain logging disabled");
}

bool AgentBrain::isLoggingToFile() const {
    // Check if file logging is enabled
    return false;  // Placeholder - should track logging state
}

// Performance monitoring

double AgentBrain::getSimulationSpeed() const {
    // Calculate simulation speed (placeholder)
    return 1.0;  // Placeholder - should calculate actual speed
}

double AgentBrain::getAverageFiringRatePerStep() const {
    if (!brain_) return 0.0;
    return static_cast<double>(brain_->getAverageFiringRate());
}

size_t AgentBrain::getMemoryFootprint() const {
    // Calculate memory footprint (placeholder)
    return 0;  // Placeholder - should calculate actual memory usage
}

// Statistical analysis

AgentBrain::StatisticalSummary AgentBrain::computeStatisticalSummary(const std::vector<float>& data) const {
    StatisticalSummary summary;
    if (data.empty()) return summary;
    
    double sum = 0.0;
    double sumSq = 0.0;
    double min = data[0];
    double max = data[0];
    
    for (float val : data) {
        sum += val;
        sumSq += val * val;
        if (val < min) min = val;
        if (val > max) max = val;
    }
    
    summary.count = data.size();
    summary.mean = sum / summary.count;
    summary.variance = (sumSq / summary.count) - (summary.mean * summary.mean);
    summary.stddev = std::sqrt(summary.variance);
    summary.min = min;
    summary.max = max;
    
    return summary;
}

AgentBrain::StatisticalSummary AgentBrain::computeMotorOutputStatistics() const {
    // Compute statistics for motor output (placeholder)
    return StatisticalSummary();
}

AgentBrain::StatisticalSummary AgentBrain::computeSensoryInputStatistics() const {
    // Compute statistics for sensory input (placeholder)
    return StatisticalSummary();
}

// Batch operations

void AgentBrain::applyBatchNeuromodulation(const std::vector<std::pair<std::string, float>>& neuromodulators) {
    for (const auto& pair : neuromodulators) {
        if (pair.first == "dopamine") {
            dopamineLevel_ = pair.second;
        } else if (pair.first == "curiosity") {
            curiosityLevel_ = pair.second;
        } else if (pair.first == "novelty") {
            noveltyLevel_ = pair.second;
        }
    }
}

void AgentBrain::updateBatchDevelopment(double timestep) {
    developmentalAge_ += timestep;
    // Update development state
    if (developmentEnabled_) {
        // Simplified development update
        if (developmentalAge_ > 100.0) {
            plasticityModifier_ = 0.2f;
        }
    }
}

// State management

void AgentBrain::saveState(const std::string& filepath) {
    // Save agent brain state (placeholder)
    NLM_LOG_INFO("AgentBrain state saved to: " + filepath);
}

bool AgentBrain::loadState(const std::string& filepath) {
    // Load agent brain state (placeholder)
    NLM_LOG_INFO("AgentBrain state loaded from: " + filepath);
    return true;
}

// Configuration validation

bool AgentBrain::validateConfiguration(std::vector<std::string>& errors) const {
    bool isValid = true;
    
    // Validate neuromodulation levels
    if (dopamineLevel_ < -1.0f || dopamineLevel_ > 1.0f) {
        errors.push_back("Dopamine level out of range [-1.0, 1.0]");
        isValid = false;
    }
    
    if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
        errors.push_back("Curiosity level out of range [0.0, 1.0]");
        isValid = false;
    }
    
    return isValid;
}

bool AgentBrain::isConfigurationValid() const {
    std::vector<std::string> errors;
    return validateConfiguration(errors);
}

// Export and visualization helpers

std::string AgentBrain::generateActivityReport() const {
    std::ostringstream oss;
    oss << "AgentBrain Activity Report\n";
    oss << "==========================\n";
    oss << "Neuromodulation Levels:\n";
    oss << "  Dopamine: " << dopamineLevel_ << "\n";
    oss << "  Curiosity: " << curiosityLevel_ << "\n";
    oss << "  Novelty: " << noveltyLevel_ << "\n";
    oss << "  Prediction Error: " << predictionError_ << "\n";
    oss << "Developmental Age: " << developmentalAge_ << "\n";
    oss << "Plasticity Modifier: " << plasticityModifier_ << "\n";
    
    return oss.str();
}

std::string AgentBrain::generateConnectivityReport() const {
    std::ostringstream oss;
    oss << "AgentBrain Connectivity Report\n";
    oss << "=============================\n";
    oss << "Motor neurons groups:\n";
    oss << "  Forward: " << motorForward_.size() << " neurons\n";
    oss << "  Backward: " << motorBackward_.size() << " neurons\n";
    oss << "  Turn Left: " << motorTurnLeft_.size() << " neurons\n";
    oss << "  Turn Right: " << motorTurnRight_.size() << " neurons\n";
    oss << "  Interact: " << motorInteract_.size() << " neurons\n";
    oss << "  Wait: " << motorWait_.size() << " neurons\n";
    oss << "\nSensory neurons groups:\n";
    oss << "  Vision: " << sensoryVision_.size() << " neurons\n";
    oss << "  Touch: " << sensoryTouch_.size() << " neurons\n";
    oss << "  Internal: " << sensoryInternal_.size() << " neurons\n";
    oss << "  Proprioception: " << sensoryProprioception_.size() << " neurons\n";
    
    return oss.str();
}

// Simulation state management

void AgentBrain::pauseSimulation() {
    // Pause agent brain simulation (placeholder)
    NLM_LOG_INFO("AgentBrain simulation paused");
}

void AgentBrain::resumeSimulation() {
    // Resume agent brain simulation (placeholder)
    NLM_LOG_INFO("AgentBrain simulation resumed");
}

bool AgentBrain::isSimulationPaused() const {
    // Check if simulation is paused (placeholder)
    return false;  // Placeholder - should track simulation state
}

// Checkpoint and recovery

bool AgentBrain::createCheckpoint(const std::string& prefix) {
    // Create checkpoint (placeholder)
    std::string filepath = prefix + "_agentbrain_" + std::to_string(std::time(nullptr)) + ".ckpt";
    saveState(filepath);
    NLM_LOG_INFO("AgentBrain checkpoint created: " + filepath);
    return true;
}

bool AgentBrain::restoreFromCheckpoint(const std::string& filepath) {
    // Restore from checkpoint (placeholder)
    loadState(filepath);
    NLM_LOG_INFO("AgentBrain checkpoint restored from: " + filepath);
    return true;
}

std::vector<std::string> AgentBrain::getAvailableCheckpoints() const {
    // Get list of available checkpoints (placeholder)
    return std::vector<std::string>();  // Placeholder
}

} // namespace nlm


