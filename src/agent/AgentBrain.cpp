#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

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
    , isPaused_(false)
    , isStopped_(false)
    , learningEnabled_(true)
    , learningRate_(0.01f)
    , explorationRate_(0.1f)
    , maxMemorySize_(1000000)
    , curiosityThreshold_(0.5f)
    , adaptationSpeed_(0.01f)
    , adaptationAccumulator_(0.0f)
    , performanceMonitoringEnabled_(false)
    , performanceThreshold_(0.5f)
    , peakPerformanceScore_(0.0f)
    , stepCount_(0)
    , totalFiringRate_(0.0f)
    , firingCount_(0)
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
    
    stepCount_ = 0;
    totalFiringRate_ = 0.0f;
    firingCount_ = 0;
    recentPerformance_.clear();
    adaptationAccumulator_ = 0.0f;
    
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
        
        // Update adaptation accumulator
        if (curiosityLevel_ > curiosityThreshold_) {
            adaptationAccumulator_ += curiosityLevel_ * adaptationSpeed_;
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    stepCount_++;
    
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
        float exploreChance = curiosityLevel_ * explorationRate_;  // Up to explorationRate_% random
        
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
                // Apply reward-modulated weight change with learning rate
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * learningRate_;
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

double AgentBrain::getUptime() const {
    if (startTime_.time_since_epoch().count() == 0) return 0.0;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime_);
    return duration.count();
}

double AgentBrain::getTotalTime() const {
    if (stopTime_.time_since_epoch().count() == 0 && startTime_.time_since_epoch().count() == 0) return 0.0;
    
    auto endTime = stopTime_.time_since_epoch().count() != 0 ? stopTime_ : std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
    return duration.count();
}

float AgentBrain::getAverageFiringRate() const {
    if (firingCount_ == 0) return 0.0f;
    return totalFiringRate_ / firingCount_;
}

float AgentBrain::getSynapticStability() const {
    if (!brain_) return 1.0f;
    
    float totalStability = 0.0f;
    size_t synapseCount = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Stability based on weight change rate and eligibility trace
            float eligibility = syn->getEligibilityTrace();
            float weightChange = std::abs(syn->getWeight() - syn->getInitialWeight());
            
            float stability = 1.0f - std::min(1.0f, (weightChange + std::abs(eligibility)) * 10.0f);
            totalStability += stability;
            synapseCount++;
        }
    }
    
    return synapseCount > 0 ? totalStability / synapseCount : 1.0f;
}

std::vector<float> AgentBrain::getWeightStatistics() const {
    std::vector<float> weights;
    
    if (!brain_) return weights;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            weights.push_back(syn->getWeight());
        }
    }
    
    return weights;
}

std::string AgentBrain::getPerformanceMetrics() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Agent Status: " << (isRunning() ? "RUNNING" : (isPaused() ? "PAUSED" : "STOPPED")) << "\n";
    ss << "Uptime: " << getUptime() << "s\n";
    ss << "Total Time: " << getTotalTime() << "s\n";
    ss << "Steps Processed: " << stepCount_ << "\n";
    ss << "Learning Rate: " << learningRate_ << "\n";
    ss << "Exploration Rate: " << explorationRate_ << "\n";
    ss << "Curiosity Level: " << curiosityLevel_ << "\n";
    ss << "Prediction Error: " << predictionError_ << "\n";
    ss << "Adaptation Accumulator: " << adaptationAccumulator_ << "\n";
    ss << "Average Firing Rate: " << getAverageFiringRate() << "\n";
    ss << "Synaptic Stability: " << getSynapticStability() << "\n";
    ss << "Performance Score: " << getPerformanceScore() << "\n";
    ss << "Memory Usage: " << (maxMemorySize_ > 0 ? (stepCount_ * sizeof(float) / maxMemorySize_ * 100.0f) : 0.0f) << "%\n";
    
    return ss.str();
}

double AgentBrain::getStepCount() const {
    return stepCount_;
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
    adaptationAccumulator_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::checkpoint(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to create checkpoint file: " + filename);
        return;
    }
    
    // Save brain state
    if (brain_) {
        brain_->saveToFile(filename + ".brain");
    }
    
    // Save agent state
    NLM_LOG_INFO("Checkpoint saved to: " + filename);
}

void AgentBrain::restore(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open checkpoint file: " + filename);
        return;
    }
    
    // Restore brain state
    if (brain_) {
        brain_->loadFromFile(filename + ".brain");
    }
    
    NLM_LOG_INFO("Checkpoint restored from: " + filename);
}

void AgentBrain::saveState(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to save state file: " + filename);
        return;
    }
    
    // Simple serialization of key agent state
    file.write(reinterpret_cast<const char*>(&stepCount_), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(&totalFiringRate_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&firingCount_), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(float));
    file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(double));
    file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(float));
    
    NLM_LOG_INFO("Agent state saved to: " + filename);
}

void AgentBrain::loadState(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to load state file: " + filename);
        return;
    }
    
    // Simple deserialization
    file.read(reinterpret_cast<char*>(&stepCount_), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&totalFiringRate_), sizeof(float));
    file.read(reinterpret_cast<char*>(&firingCount_), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(float));
    file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(float));
    file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(float));
    file.read(reinterpret_cast<char*>(&predictionError_), sizeof(float));
    file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(float));
    file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(double));
    file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(float));
    
    NLM_LOG_INFO("Agent state loaded from: " + filename);
}

bool AgentBrain::checkPerformance() const {
    if (!performanceMonitoringEnabled_) return true;
    
    float performance = getPerformanceScore();
    if (performance < performanceThreshold_) {
        if (performanceAlertCallback_) {
            performanceAlertCallback_(performance);
        }
        NLM_LOG_WARNING("Performance below threshold: " + std::to_string(performance));
        return false;
    }
    
    return true;
}

float AgentBrain::getPerformanceScore() const {
    float score = 0.0f;
    
    // Component scores (weighted)
    float curiosityScore = std::min(1.0f, curiosityLevel_ / 0.5f) * 0.2f;
    float predictionScore = std::min(1.0f, std::abs(predictionError_) / 0.1f) * 0.3f;
    float stabilityScore = getSynapticStability() * 0.2f;
    float firingScore = std::min(1.0f, getAverageFiringRate() / 10.0f) * 0.2f;
    float adaptationScore = std::min(1.0f, adaptationAccumulator_ / 1.0f) * 0.1f;
    
    float totalScore = curiosityScore + predictionScore + stabilityScore + firingScore + adaptationScore;
    
    // Update peak score
    peakPerformanceScore_ = std::max(peakPerformanceScore_, totalScore);
    
    // Decay recent performance
    recentPerformance_.push_back(totalScore);
    if (recentPerformance_.size() > 100) {
        recentPerformance_.erase(recentPerformance_.begin());
    }
    
    return totalScore;
}

std::string AgentBrain::getAgentCommandHelp() {
    std::stringstream ss;
    ss << "=== Agent Brain Command Help ===\n";
    ss << "Core Control:\n";
    ss << "  start           - Start the agent (resume from pause)\n";
    ss << "  stop            - Stop the agent (save state)\n";
    ss << "  pause           - Pause the agent (save state)\n";
    ss << "  status          - Show agent status\n";
    ss << "  reset           - Reset agent state\n";
    ss << "\n";
    ss << "Learning Control:\n";
    ss << "  learning [on|off] - Enable/disable learning\n";
    ss << "  lrate <value>   - Set learning rate (0.0-1.0)\n";
    ss << "  explore <value> - Set exploration rate (0.0-1.0)\n";
    ss << "\n";
    ss << "State Management:\n";
    ss << "  checkpoint <file> - Save checkpoint\n";
    ss << "  restore <file>    - Restore checkpoint\n";
    ss << "  save <file>       - Save agent state\n";
    ss << "  load <file>       - Load agent state\n";
    ss << "\n";
    ss << "Performance:\n";
    ss << "  monitor [on|off] - Enable/disable performance monitoring\n";
    ss << "  stats             - Show detailed statistics\n";
    ss << "  perf              - Show performance metrics\n";
    ss << "\n";
    ss << "Debug:\n";
    ss << "  help             - Show this help\n";
    ss << "  verbose          - Toggle verbose logging\n";
    ss << "  validate         - Validate agent configuration\n";
    
    return ss.str();
}

bool AgentBrain::processCommand(const std::string& command, const std::string& arg) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "start") {
        start();
        if (stateChangeCallback_) {
            stateChangeCallback_("Agent started");
        }
        NLM_LOG_INFO("Agent started");
        return true;
    }
    else if (cmd == "stop") {
        stop();
        if (stateChangeCallback_) {
            stateChangeCallback_("Agent stopped");
        }
        NLM_LOG_INFO("Agent stopped");
        return true;
    }
    else if (cmd == "pause") {
        pause();
        if (stateChangeCallback_) {
            stateChangeCallback_("Agent paused");
        }
        NLM_LOG_INFO("Agent paused");
        return true;
    }
    else if (cmd == "resume") {
        resume();
        if (stateChangeCallback_) {
            stateChangeCallback_("Agent resumed");
        }
        NLM_LOG_INFO("Agent resumed");
        return true;
    }
    else if (cmd == "status") {
        NLM_LOG_INFO(getPerformanceMetrics());
        return true;
    }
    else if (cmd == "reset") {
        reset();
        if (stateChangeCallback_) {
            stateChangeCallback_("Agent reset");
        }
        NLM_LOG_INFO("Agent reset");
        return true;
    }
    else if (cmd == "learning") {
        if (arg == "on" || arg == "true") {
            enableLearning(true);
            NLM_LOG_INFO("Learning enabled");
        }
        else if (arg == "off" || arg == "false") {
            enableLearning(false);
            NLM_LOG_INFO("Learning disabled");
        }
        else {
            NLM_LOG_WARNING("Invalid argument for learning command. Use 'on' or 'off'");
        }
        return true;
    }
    else if (cmd == "lrate") {
        try {
            float rate = std::stof(arg);
            setLearningRate(rate);
            NLM_LOG_INFO("Learning rate set to: " + arg);
        }
        catch (...) {
            NLM_LOG_WARNING("Invalid learning rate value: " + arg);
        }
        return true;
    }
    else if (cmd == "explore") {
        try {
            float rate = std::stof(arg);
            setExplorationRate(rate);
            NLM_LOG_INFO("Exploration rate set to: " + arg);
        }
        catch (...) {
            NLM_LOG_WARNING("Invalid exploration rate value: " + arg);
        }
        return true;
    }
    else if (cmd == "checkpoint") {
        if (!arg.empty()) {
            checkpoint(arg);
        }
        else {
            NLM_LOG_WARNING("Checkpoint file not specified");
        }
        return true;
    }
    else if (cmd == "restore") {
        if (!arg.empty()) {
            restore(arg);
        }
        else {
            NLM_LOG_WARNING("Checkpoint file not specified");
        }
        return true;
    }
    else if (cmd == "save") {
        if (!arg.empty()) {
            saveState(arg);
        }
        else {
            NLM_LOG_WARNING("Save file not specified");
        }
        return true;
    }
    else if (cmd == "load") {
        if (!arg.empty()) {
            loadState(arg);
        }
        else {
            NLM_LOG_WARNING("Load file not specified");
        }
        return true;
    }
    else if (cmd == "monitor") {
        if (arg == "on" || arg == "true") {
            enablePerformanceMonitoring(true);
            NLM_LOG_INFO("Performance monitoring enabled");
        }
        else if (arg == "off" || arg == "false") {
            enablePerformanceMonitoring(false);
            NLM_LOG_INFO("Performance monitoring disabled");
        }
        else {
            NLM_LOG_WARNING("Invalid argument for monitor command. Use 'on' or 'off'");
        }
        return true;
    }
    else if (cmd == "stats") {
        AgentStats stats = collectStats();
        NLM_LOG_INFO("=== Agent Statistics ===");
        NLM_LOG_INFO("Elapsed Time: " + std::to_string(stats.elapsedTime) + "s");
        NLM_LOG_INFO("Steps Processed: " + std::to_string(stats.stepsProcessed));
        NLM_LOG_INFO("Average Firing Rate: " + std::to_string(stats.averageFiringRate));
        NLM_LOG_INFO("Synaptic Stability: " + std::to_string(stats.synapticStability));
        NLM_LOG_INFO("Learning Progress: " + std::to_string(stats.learningProgress));
        NLM_LOG_INFO("Performance Score: " + std::to_string(stats.performanceScore));
        NLM_LOG_INFO("Memory Usage: " + std::to_string(stats.memoryUsage) + " bytes");
        NLM_LOG_INFO("Total Spikes: " + std::to_string(stats.totalSpikes));
        return true;
    }
    else if (cmd == "perf") {
        NLM_LOG_INFO(getPerformanceMetrics());
        return true;
    }
    else if (cmd == "verbose") {
        // Toggle verbose logging (implementation depends on logger)
        NLM_LOG_INFO("Verbose logging toggled (not implemented)");
        return true;
    }
    else if (cmd == "validate") {
        if (validateConfiguration()) {
            NLM_LOG_INFO("Configuration validation passed");
        }
        else {
            NLM_LOG_WARNING("Configuration validation failed: " + getValidationErrors());
        }
        return true;
    }
    
    NLM_LOG_WARNING("Unknown command: " + command);
    return false;
}

bool AgentBrain::validateConfiguration() const {
    std::vector<std::string> errors;
    
    if (learningRate_ < 0.0f || learningRate_ > 1.0f) {
        errors.push_back("Learning rate must be between 0.0 and 1.0");
    }
    
    if (explorationRate_ < 0.0f || explorationRate_ > 1.0f) {
        errors.push_back("Exploration rate must be between 0.0 and 1.0");
    }
    
    if (curiosityThreshold_ < 0.0f || curiosityThreshold_ > 1.0f) {
        errors.push_back("Curiosity threshold must be between 0.0 and 1.0");
    }
    
    if (adaptationSpeed_ < 0.0f || adaptationSpeed_ > 1.0f) {
        errors.push_back("Adaptation speed must be between 0.0 and 1.0");
    }
    
    if (performanceThreshold_ < 0.0f || performanceThreshold_ > 1.0f) {
        errors.push_back("Performance threshold must be between 0.0 and 1.0");
    }
    
    // Store errors for later retrieval
    validationErrors_ = errors;
    
    return errors.empty();
}

std::string AgentBrain::getValidationErrors() const {
    std::stringstream ss;
    for (size_t i = 0; i < validationErrors_.size(); ++i) {
        ss << validationErrors_[i];
        if (i < validationErrors_.size() - 1) ss << ", ";
    }
    return ss.str();
}

AgentBrain::AgentStats AgentBrain::collectStats() const {
    AgentStats stats;
    stats.elapsedTime = getUptime();
    stats.stepsProcessed = stepCount_;
    stats.averageFiringRate = getAverageFiringRate();
    stats.synapticStability = getSynapticStability();
    stats.learningProgress = adaptationAccumulator_ / 10.0f; // Normalize
    stats.performanceScore = getPerformanceScore();
    stats.memoryUsage = maxMemorySize_;
    stats.totalSpikes = firingCount_;
    return stats;
}

} // namespace nlm
