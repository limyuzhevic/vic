#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, AgentConfig::createDefaultConfig())
{
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronDistribution() {
    // Clear existing neuron groups
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
    sensoryAudio_.clear();
    
    if (!brain_) {
        NLM_LOG_ERROR("No brain available for neuron distribution");
        return;
    }
    
    // Get total neuron counts by type
    size_t totalMotorNeurons = 0;
    size_t totalSensoryNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            size_t neuronCount = pop->getNeurons().size();
            
            if (type == NeuronType::Motor) {
                totalMotorNeurons += neuronCount;
            } else if (type == NeuronType::Sensory) {
                totalSensoryNeurons += neuronCount;
            }
        }
    }
    
    if (totalMotorNeurons == 0 || totalSensoryNeurons == 0) {
        NLM_LOG_ERROR("No motor or sensory neurons found in brain");
        return;
    }
    
    // Calculate target neuron counts based on configuration ratios
    size_t targetMotorForward = static_cast<size_t>(totalMotorNeurons * config_.distribution.motorForward);
    size_t targetMotorBackward = static_cast<size_t>(totalMotorNeurons * config_.distribution.motorBackward);
    size_t targetMotorTurnLeft = static_cast<size_t>(totalMotorNeurons * config_.distribution.motorTurnLeft);
    size_t targetMotorTurnRight = static_cast<size_t>(totalMotorNeurons * config_.distribution.motorTurnRight);
    size_t targetMotorInteract = static_cast<size_t>(totalMotorNeurons * config_.distribution.motorInteract);
    size_t targetMotorWait = totalMotorNeurons - (targetMotorForward + targetMotorBackward + 
                                                 targetMotorTurnLeft + targetMotorTurnRight + 
                                                 targetMotorInteract);
    
    size_t targetSensoryVision = static_cast<size_t>(totalSensoryNeurons * config_.distribution.sensoryVision);
    size_t targetSensoryTouch = static_cast<size_t>(totalSensoryNeurons * config_.distribution.sensoryTouch);
    size_t targetSensoryInternal = static_cast<size_t>(totalSensoryNeurons * config_.distribution.sensoryInternal);
    size_t targetSensoryProprioception = totalSensoryNeurons - (targetSensoryVision + targetSensoryTouch + 
                                                              targetSensoryInternal);
    
    // Collect neurons by type and distribute them
    std::vector<Neuron*> motorNeurons;
    std::vector<Neuron*> sensoryNeurons;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            auto& neurons = pop->getNeurons();
            
            if (type == NeuronType::Motor) {
                motorNeurons.insert(motorNeurons.end(), neurons.begin(), neurons.end());
            } else if (type == NeuronType::Sensory) {
                sensoryNeurons.insert(sensoryNeurons.end(), neurons.begin(), neurons.end());
            }
        }
    }
    
    // Shuffle neurons for better distribution
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(motorNeurons.begin(), motorNeurons.end(), g);
    std::shuffle(sensoryNeurons.begin(), sensoryNeurons.end(), g);
    
    // Assign motor neurons to groups
    size_t idx = 0;
    for (size_t i = 0; i < targetMotorForward && idx < motorNeurons.size(); ++i) {
        motorForward_.push_back(motorNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetMotorBackward && idx < motorNeurons.size(); ++i) {
        motorBackward_.push_back(motorNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetMotorTurnLeft && idx < motorNeurons.size(); ++i) {
        motorTurnLeft_.push_back(motorNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetMotorTurnRight && idx < motorNeurons.size(); ++i) {
        motorTurnRight_.push_back(motorNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetMotorInteract && idx < motorNeurons.size(); ++i) {
        motorInteract_.push_back(motorNeurons[idx++]);
    }
    
    // Remaining motor neurons go to Wait group
    while (idx < motorNeurons.size()) {
        motorWait_.push_back(motorNeurons[idx++]);
    }
    
    // Assign sensory neurons to groups
    idx = 0;
    for (size_t i = 0; i < targetSensoryVision && idx < sensoryNeurons.size(); ++i) {
        sensoryVision_.push_back(sensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetSensoryTouch && idx < sensoryNeurons.size(); ++i) {
        sensoryTouch_.push_back(sensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < targetSensoryInternal && idx < sensoryNeurons.size(); ++i) {
        sensoryInternal_.push_back(sensoryNeurons[idx++]);
    }
    
    // Remaining sensory neurons go to Proprioception group
    while (idx < sensoryNeurons.size()) {
        sensoryProprioception_.push_back(sensoryNeurons[idx++]);
    }
    
    // Log distribution results
    NLM_LOG_INFO("Neuron distribution: Motor groups - Forward: " + std::to_string(motorForward_.size()) +
                 ", Backward: " + std::to_string(motorBackward_.size()) +
                 ", Left: " + std::to_string(motorTurnLeft_.size()) +
                 ", Right: " + std::to_string(motorTurnRight_.size()) +
                 ", Interact: " + std::to_string(motorInteract_.size()) +
                 ", Wait: " + std::to_string(motorWait_.size()));
    
    NLM_LOG_INFO("Sensory distribution: Vision: " + std::to_string(sensoryVision_.size()) +
                 ", Touch: " + std::to_string(sensoryTouch_.size()) +
                 ", Internal: " + std::to_string(sensoryInternal_.size()) +
                 ", Proprioception: " + std::to_string(sensoryProprioception_.size()));
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize based on config
    config_.sensory.visionWidth = std::min(config_.sensory.visionWidth, 
                                           static_cast<size_t>(world.getVisionWidth()));
    config_.sensory.visionHeight = std::min(config_.sensory.visionHeight, 
                                           static_cast<size_t>(world.getVisionHeight()));
    
    size_t visionSize = config_.sensory.visionWidth * config_.sensory.visionHeight;
    previousVision_.resize(visionSize, 0.0f);
    
    developmentalAge_ = config_.development.initialAge;
    plasticityModifier_ = config_.development.plasticityInitial;
    
    sampleStepCount_ = 0;
    
    NLM_LOG_INFO("AgentBrain initialized with config - Vision: " + 
                 std::to_string(config_.sensory.visionWidth) + "x" + 
                 std::to_string(config_.sensory.visionHeight) +
                 ", Touch: " + std::to_string(config_.sensory.touchPoints) +
                 ", Internal: " + std::to_string(config_.sensory.internalSignals) +
                 ", Proprioception: " + std::to_string(config_.sensory.proprioceptionPoints));
}

size_t AgentBrain::getSensoryInputSize() const {
    size_t total = config_.sensory.visionWidth * config_.sensory.visionHeight +
                  config_.sensory.touchPoints +
                  config_.sensory.internalSignals +
                  config_.sensory.proprioceptionPoints;
    
    if (config_.sensory.enableAudio && config_.sensory.audioChannels > 0) {
        total += config_.sensory.audioChannels;
    }
    
    return total;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Based on motor group distribution
    size_t activeMotorGroups = 0;
    if (motorForward_.size() > 0) activeMotorGroups++;
    if (motorBackward_.size() > 0) activeMotorGroups++;
    if (motorTurnLeft_.size() > 0) activeMotorGroups++;
    if (motorTurnRight_.size() > 0) activeMotorGroups++;
    if (motorInteract_.size() > 0) activeMotorGroups++;
    if (motorWait_.size() > 0) activeMotorGroups++;
    
    return activeMotorGroups;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process vision input
    if (config_.sensory.enableVisionProcessing && !percept.getVision().empty()) {
        processVisionInput(percept.getVision());
    }
    
    // Process touch input
    if (!percept.getTouch().empty()) {
        processTouchInput(percept.getTouch());
    }
    
    // Process internal signals
    if (!percept.getInternal().empty()) {
        processInternalInput(percept.getInternal());
    }
    
    // Process proprioception
    if (!percept.getProprioception().empty()) {
        processProprioceptionInput(percept.getProprioception());
    }
    
    // Process audio if enabled
    if (config_.sensory.enableAudio && !percept.getAudio().empty()) {
        processAudioInput(percept.getAudio());
    }
    
    // Compute novelty and curiosity
    computeNoveltyAndCuriosity(percept.getVision());
    
    // Record sample if enabled
    if (samplingEnabled_) {
        recordSample("sensory", percept.getAllSignals());
    }
    
    // Update performance metrics
    updatePerformanceMetrics();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Apply normalization if configured
    std::vector<float> processedVision = vision;
    if (config_.sensory.normalizeVision) {
        float maxVal = *std::max_element(processedVision.begin(), processedVision.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : processedVision) val *= scale;
        }
    }
    
    // Scale according to config
    for (size_t i = 0; i < sensoryVision_.size() && i < processedVision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = processedVision[i] * config_.sensory.visionScale;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    std::vector<float> processedTouch = touch;
    if (config_.sensory.normalizeTouch) {
        float maxVal = *std::max_element(processedTouch.begin(), processedTouch.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : processedTouch) val *= scale;
        }
    }
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < processedTouch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = processedTouch[i] * config_.sensory.touchScale;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    std::vector<float> processedInternal = internal;
    if (config_.sensory.normalizeInternal) {
        // Normalize to -1.0 to 1.0 range
        for (float& val : processedInternal) {
            val = (val * 2.0f - 1.0f); // Assuming input is 0.0-1.0
        }
    }
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < processedInternal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = processedInternal[i] * config_.sensory.internalScale;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    std::vector<float> processedProprio = proprio;
    if (config_.sensory.normalizeProprioception) {
        for (float& val : processedProprio) {
            val = (val * 2.0f - 1.0f); // Assuming input is 0.0-1.0
        }
    }
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < processedProprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = processedProprio[i] * config_.sensory.proprioceptionScale;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processAudioInput(const std::vector<float>& audio) {
    // Audio processing (placeholder for future enhancement)
    if (config_.sensory.enableAudio && !sensoryAudio_.empty()) {
        // Scale audio according to config
        float audioScale = config_.sensory.audioChannels > 0 ? 
                           config_.sensory.visionScale : 1.0f;
        
        for (size_t i = 0; i < sensoryAudio_.size() && i < audio.size(); ++i) {
            if (sensoryAudio_[i]) {
                float current = audio[i] * audioScale;
                sensoryAudio_[i]->injectCurrent(current);
            }
        }
    }
}

void AgentBrain::computeNoveltyAndCuriosity(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and apply decay
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * config_.neuromodulation.curiosityAmplification +
                         std::abs(predictionError_) * config_.neuromodulation.errorAmplification;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > config_.neuromodulation.curiosityThreshold) {
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
        float exploreChance = curiosityLevel_ * config_.neuromodulation.explorationChance;
        
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
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                                config_.neuromodulation.dopamineClampMin,
                                config_.neuromodulation.dopamineClampMax);
    
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
    if (developmentalAge_ < config_.development.criticalPeriodEnd) {  // ~5 minutes
        plasticityModifier_ = config_.development.plasticityCritical;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.development.maturationEnd) {  // ~15 minutes
        plasticityModifier_ = config_.development.plasticityMature;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = config_.development.plasticityAdult;  // Adult - more stable
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

void AgentBrain::recordSample(const std::string& type, const std::vector<float>& data) {
    sampleStepCount_++;
    
    if (config_.sampling.maxSampleHistory > 0 && 
        sampleHistory_.size() >= config_.sampling.maxSampleHistory) {
        sampleHistory_.erase(sampleHistory_.begin());
    }
    
    sampleHistory_.push_back(std::make_pair(type, data));
    
    // Log sampling
    if (config_.sampling.recordSpikes) {
        NLM_LOG_DEBUG("Sampled " + type + " data: " + std::to_string(data.size()) + " values at step " + 
                     std::to_string(sampleStepCount_));
    }
}

void AgentBrain::updatePerformanceMetrics() {
    double currentTime = developmentalAge_;  // Using age as time proxy
    
    // Update performance metrics
    currentMetrics_.timeStep = currentTime;
    
    // Calculate average neural activity
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                totalActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
                neuronCount++;
            }
        }
    }
    
    currentMetrics_.averageActivity = neuronCount > 0 ? totalActivity / neuronCount : 0.0f;
    currentMetrics_.spikeCount = brain_->getTotalSpikeCount();
    
    // Simple CPU/memory estimation (placeholder)
    currentMetrics_.cpuUsage = 0.5f; // Placeholder
    currentMetrics_.memoryUsage = 0.5f; // Placeholder
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = config_.development.initialAge;
    plasticityModifier_ = config_.development.plasticityInitial;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear samples
    sampleHistory_.clear();
    sampleStepCount_ = 0;
    
    // Reset config file path if needed
    if (useConfigFile_) {
        // Reload config from file if it exists
        configFilePath_ = config_.file.defaultConfigFile;
        // Would load from file here
    }
}

} // namespace nlm
