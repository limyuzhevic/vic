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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain is not set");
        return;
    }
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize underlying brain");
        return;
    }
    
    // Validate world dimensions
    int width = world.getWidth();
    int height = world.getHeight();
    int visionWidth = world.getVisionWidth();
    int visionHeight = world.getVisionHeight();
    
    if (width <= 0 || height <= 0) {
        NLM_LOG_ERROR("Invalid world dimensions: width=" + std::to_string(width) + 
                     ", height=" + std::to_string(height));
        return;
    }
    
    if (visionWidth <= 0 || visionHeight <= 0) {
        NLM_LOG_ERROR("Invalid vision dimensions: visionWidth=" + std::to_string(visionWidth) +
                     ", visionHeight=" + std::to_string(visionHeight));
        return;
    }
    
    // Resize vision buffer to match world dimensions
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

float AgentBrain::calculateMotorActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) {
        NLM_LOG_WARNING("Cannot calculate motor activity: neuron group is empty");
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t validNeurons = 0;
    
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("Null neuron pointer encountered in motor activity calculation");
            continue;
        }
        
        // Validate neuron state before accessing
        if (!n->getState().isValid()) {
            NLM_LOG_WARNING("Invalid neuron state encountered");
            continue;
        }
        
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        validNeurons++;
    }
    
    if (validNeurons == 0) {
        NLM_LOG_ERROR("No valid neurons for motor activity calculation");
        return 0.0f;
    }
    
    return sum / validNeurons;
}

MotorCommand AgentBrain::findBestMotorCommand(
    const std::vector<MotorCommand>& commands,
    const std::vector<float>& activities,
    float bestActivity,
    MotorCommand defaultCmd
) {
    if (commands.empty()) {
        NLM_LOG_ERROR("Cannot find best motor command: commands vector is empty");
        return MotorCommand::Wait;
    }
    
    if (activities.empty()) {
        NLM_LOG_ERROR("Cannot find best motor command: activities vector is empty");
        return MotorCommand::Wait;
    }
    
    if (commands.size() != activities.size()) {
        NLM_LOG_WARNING("Command and activity vector size mismatch: " + 
                        std::to_string(commands.size()) + " vs " + std::to_string(activities.size()));
    }
    
    MotorCommand best = defaultCmd;
    float bestActivityValue = bestActivity;
    
    for (size_t i = 0; i < commands.size() && i < activities.size(); ++i) {
        if (activities[i] > bestActivityValue) {
            bestActivityValue = activities[i];
            best = commands[i];
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivityValue < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain is not initialized");
        return MotorCommand::Wait;
    }
    
    // Validate motor neuron groups exist
    if (motorForward_.empty() && motorBackward_.empty() && 
        motorTurnLeft_.empty() && motorTurnRight_.empty() && 
        motorInteract_.empty() && motorWait_.empty()) {
        NLM_LOG_WARNING("No motor neurons configured for decoding");
        return MotorCommand::Wait;
    }
    
    // Calculate activity in each motor group using helper functions
    float forwardAct = calculateMotorActivity(motorForward_);
    float backwardAct = calculateMotorActivity(motorBackward_);
    float leftAct = calculateMotorActivity(motorTurnLeft_);
    float rightAct = calculateMotorActivity(motorTurnRight_);
    float interactAct = calculateMotorActivity(motorInteract_);
    float waitAct = calculateMotorActivity(motorWait_);
    
    // Find best motor command
    MotorCommand defaultCmd = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    std::vector<MotorCommand> commands = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    MotorCommand bestCmd = findBestMotorCommand(commands, activities, bestActivity, defaultCmd);
    
    return bestCmd;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain is not initialized");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with validation
    if (curiosityEnabled_) {
        if (curiosityLevel_ > 1.0f) {
            NLM_LOG_WARNING("Curiosity level exceeds maximum: " + std::to_string(curiosityLevel_));
            curiosityLevel_ = 1.0f;
        }
        
        if (curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
    }
    
    return decoded;
}

MotorCommand AgentBrain::selectWithCuriosityHelper(MotorCommand defaultCmd) {
    // Validate curiosity level
    if (curiosityLevel_ < 0.0f) {
        NLM_LOG_WARNING("Curiosity level is negative: " + std::to_string(curiosityLevel_));
        curiosityLevel_ = 0.0f;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        // Cap explore chance to prevent excessive randomness
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            return selectRandomMotorCommand();
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    return selectWithCuriosityHelper(defaultCmd);
}

float AgentBrain::getMaxMotorActivity(const std::vector<float>& activities) {
    if (activities.empty()) {
        NLM_LOG_WARNING("Cannot get max motor activity: activities vector is empty");
        return 0.0f;
    }
    
    float maxActivity = 0.0f;
    for (float activity : activities) {
        if (activity > maxActivity) {
            maxActivity = activity;
        }
    }
    
    // Cap max activity to prevent overflow
    return std::min(maxActivity, 10.0f);
}

MotorCommand AgentBrain::selectRandomMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot select random motor command: brain is not initialized");
        return MotorCommand::Wait;
    }
    
    // Validate random generator exists
    if (!brain_->getRandomGenerator()) {
        NLM_LOG_ERROR("Cannot select random motor command: random generator is null");
        return MotorCommand::Wait;
    }
    
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

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain is not initialized");
        return;
    }
    
    if (brain_->getTotalNeuronCount() == 0) {
        NLM_LOG_WARNING("Cannot process sensory input: brain has no neurons");
        return;
    }
    
    // Process different sensory modalities
    processVisionInput(percept.getVision());
    processTouchInput(percept.getTouch());
    processInternalInput(percept.getInternal());
    processProprioceptionInput(percept.getProprioception());
    
    // Update novelty and curiosity
    updateNoveltyAndCuriosity(percept.getVision());
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    if (!brain_ || sensoryVision_.empty()) {
        return;
    }
    
    // Validate input
    if (vision.empty()) {
        NLM_LOG_WARNING("Received empty vision input");
        // Clear vision neurons if vision is empty
        for (Neuron* neuron : sensoryVision_) {
            if (neuron) neuron->injectCurrent(0.0f);
        }
        return;
    }
    
    // Cap the scale factor to prevent excessive current injection
    const float visionScale = std::min(5.0f, 50.0f / 255.0f);
    
    // Process vision input safely
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Clamp the vision value to valid range
            float normalizedVision = std::clamp(vision[i], 0.0f, 1.0f);
            float current = normalizedVision * visionScale;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    if (sensoryTouch_.empty()) return;
    
    // Validate input
    if (touch.empty()) {
        NLM_LOG_WARNING("Received empty touch input");
        return;
    }
    
    // Cap the scale factor to prevent excessive current injection
    const float touchScale = std::min(8.0f, 20.0f / 1.0f);
    
    // Process touch input safely
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Clamp the touch value to valid range
            float normalizedTouch = std::clamp(touch[i], -1.0f, 1.0f);
            float current = normalizedTouch * touchScale;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    if (sensoryInternal_.empty()) return;
    
    // Validate input
    if (internal.empty()) {
        NLM_LOG_WARNING("Received empty internal input");
        return;
    }
    
    // Use capped scale factors to prevent excessive current injection
    const float internalCenterScale = std::min(2.0f, 5.0f);
    const float internalScale = std::min(5.0f, 10.0f / 1.0f);
    
    // Process internal input safely
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Normalize internal signal to [-1, 1] range
            float normalized = (internal[i] * internalCenterScale - 1.0f);
            normalized = std::clamp(normalized, -1.0f, 1.0f);
            float current = normalized * internalScale;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprioception) {
    if (sensoryProprioception_.empty()) return;
    
    // Validate input
    if (proprioception.empty()) {
        NLM_LOG_WARNING("Received empty proprioception input");
        return;
    }
    
    // Use capped scale factors to prevent excessive current injection
    const float proprioceptionCenterScale = std::min(2.0f, 3.0f);
    const float proprioceptionScale = std::min(3.0f, 6.0f / 1.0f);
    
    // Process proprioception input safely
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Normalize proprioception signal to [-1, 1] range
            float normalized = (proprioception[i] * proprioceptionCenterScale - 1.0f);
            normalized = std::clamp(normalized, -1.0f, 1.0f);
            float current = normalized * proprioceptionScale;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNoveltyAndCuriosity(const std::vector<float>& vision) {
    // Compute novelty (difference from previous vision)
    if (vision.empty()) {
        NLM_LOG_WARNING("Cannot compute novelty: vision input is empty");
        return;
    }
    
    if (previousVision_.empty()) {
        NLM_LOG_WARNING("Cannot compute novelty: previous vision buffer is empty");
        return;
    }
    
    float totalDiff = computeVisionDifference(vision);
    size_t validSize = std::min(vision.size(), previousVision_.size());
    
    if (validSize == 0) {
        NLM_LOG_WARNING("Cannot compute novelty: no valid vision pixels to compare");
        return;
    }
    
    noveltyLevel_ = totalDiff / static_cast<float>(validSize);
    
    // Cap novelty level to prevent overflow
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    
    // Decay novelty with configurable decay rate
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        updateCuriosityLevel();
    }
}

float AgentBrain::computeVisionDifference(const std::vector<float>& vision) const {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        totalDiff += std::abs(vision[i] - previousVision_[i]);
    }
    return totalDiff;
}

void AgentBrain::updateCuriosityLevel() {
    const float curiosityNoveltyFactor = 2.0f;
    const float curiosityPredictionErrorFactor = 0.5f;
    curiosityLevel_ = noveltyLevel_ * curiosityNoveltyFactor + 
                    std::abs(predictionError_) * curiosityPredictionErrorFactor;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate activity in each motor group using helper functions
    float forwardAct = calculateMotorActivity(motorForward_);
    float backwardAct = calculateMotorActivity(motorBackward_);
    float leftAct = calculateMotorActivity(motorTurnLeft_);
    float rightAct = calculateMotorActivity(motorTurnRight_);
    float interactAct = calculateMotorActivity(motorInteract_);
    float waitAct = calculateMotorActivity(motorWait_);
    
    // Find best motor command
    MotorCommand defaultCmd = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    std::vector<MotorCommand> commands = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    MotorCommand bestCmd = findBestMotorCommand(commands, activities, bestActivity, defaultCmd);
    
    return bestCmd;
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

MotorCommand AgentBrain::selectWithCuriosityHelper(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            return selectRandomMotorCommand();
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

float AgentBrain::calculateMotorActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (!n) continue;
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::findBestMotorCommand(
    const std::vector<MotorCommand>& commands,
    const std::vector<float>& activities,
    float bestActivity,
    MotorCommand defaultCmd
) {
    MotorCommand best = defaultCmd;
    float bestActivityValue = bestActivity;
    
    for (size_t i = 0; i < commands.size() && i < activities.size(); ++i) {
        if (activities[i] > bestActivityValue) {
            bestActivityValue = activities[i];
            best = commands[i];
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivityValue < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

float AgentBrain::getMaxMotorActivity(const std::vector<float>& activities) {
    if (activities.empty()) return 0.0f;
    
    float maxActivity = 0.0f;
    for (float activity : activities) {
        if (activity > maxActivity) {
            maxActivity = activity;
        }
    }
    return maxActivity;
}

MotorCommand AgentBrain::selectRandomMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
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
