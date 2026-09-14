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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain not available");
        return;
    }
    
    // Validate percept data
    if (!percept.getVision().empty()) {
        size_t visionSize = percept.getVisionWidth() * percept.getVisionHeight();
        if (percept.getVision().size() != visionSize) {
            NLM_LOG_WARNING("Vision data size mismatch: expected " + std::to_string(visionSize) +
                         " but got " + std::to_string(percept.getVision().size()));
        }
    }
    
    if (percept.getTouch().size() > 8) {
        NLM_LOG_WARNING("Touch data exceeds expected size of 8: " + std::to_string(percept.getTouch().size()));
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!sensoryVision_.empty()) {
        bool allNeuronsValid = true;
        for (size_t i = 0; i < sensoryVision_.size(); ++i) {
            if (!sensoryVision_[i]) {
                NLM_LOG_ERROR("Null vision neuron encountered at index " + std::to_string(i));
                allNeuronsValid = false;
            }
        }
        
        if (!allNeuronsValid) {
            NLM_LOG_ERROR("Cannot process vision input due to invalid neurons");
        } else {
            for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
                if (sensoryVision_[i]) {
                    // Inject current proportional to vision intensity
                    float current = vision[i] * 5.0f;  // Scale factor
                    sensoryVision_[i]->injectCurrent(current);
                }
            }
        }
    } else {
        NLM_LOG_WARNING("No vision neurons available for input processing");
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!sensoryTouch_.empty()) {
        bool allNeuronsValid = true;
        for (size_t i = 0; i < sensoryTouch_.size(); ++i) {
            if (!sensoryTouch_[i]) {
                NLM_LOG_ERROR("Null touch neuron encountered at index " + std::to_string(i));
                allNeuronsValid = false;
            }
        }
        
        if (!allNeuronsValid) {
            NLM_LOG_ERROR("Cannot process touch input due to invalid neurons");
        } else {
            for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
                if (sensoryTouch_[i]) {
                    float current = touch[i] * 8.0f;  // Collision signal
                    sensoryTouch_[i]->injectCurrent(current);
                }
            }
        }
    } else {
        NLM_LOG_WARNING("No touch neurons available for input processing");
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!sensoryInternal_.empty()) {
        bool allNeuronsValid = true;
        for (size_t i = 0; i < sensoryInternal_.size(); ++i) {
            if (!sensoryInternal_[i]) {
                NLM_LOG_ERROR("Null internal neuron encountered at index " + std::to_string(i));
                allNeuronsValid = false;
            }
        }
        
        if (!allNeuronsValid) {
            NLM_LOG_ERROR("Cannot process internal signals due to invalid neurons");
        } else {
            for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
                if (sensoryInternal_[i]) {
                    float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                    sensoryInternal_[i]->injectCurrent(current);
                }
            }
        }
    } else {
        NLM_LOG_WARNING("No internal neurons available for input processing");
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!sensoryProprioception_.empty()) {
        bool allNeuronsValid = true;
        for (size_t i = 0; i < sensoryProprioception_.size(); ++i) {
            if (!sensoryProprioception_[i]) {
                NLM_LOG_ERROR("Null proprioception neuron encountered at index " + std::to_string(i));
                allNeuronsValid = false;
            }
        }
        
        if (!allNeuronsValid) {
            NLM_LOG_ERROR("Cannot process proprioception due to invalid neurons");
        } else {
            for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
                if (sensoryProprioception_[i]) {
                    float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                    sensoryProprioception_[i]->injectCurrent(current);
                }
            }
        }
    } else {
        NLM_LOG_WARNING("No proprioception neurons available for input processing");
    }
    
    // Store sensory pattern in working memory for later recall
    if (brain_) {
        // Convert percept to vector pattern for working memory
        std::vector<float> sensoryPattern;
        
        // Vision pattern
        sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
        
        // Touch pattern  
        sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
        
        // Internal pattern
        sensoryPattern.insert(sensoryPattern.end(), intern.begin(), intern.end());
        
        // Proprioception pattern
        sensoryPattern.insert(sensoryPattern.end(), proprio.begin(), proprio.end());
        
        // Store in working memory if available
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            if (!workingMemory->store(sensoryPattern, 0.5f)) {
                NLM_LOG_ERROR("Failed to store sensory pattern in working memory");
            }
        }
        }
        
        // Update prediction system if available
        if (auto* predictionSystem = brain_->getPredictionSystem()) {
            if (!predictionSystem->updatePrediction(sensoryPattern)) {
                NLM_LOG_WARNING("Failed to update prediction system");
            }
        }
        
        // Update novelty detection with current percept
        if (auto* novelty = brain_->getNovelty()) {
            if (!novelty->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001))) {
                NLM_LOG_WARNING("Failed to update novelty detector");
            }
        }
        
        // Apply neuromodulation to working memory traces
        if (auto* dopamine = brain_->getDopamine()) {
            if (!dopamine->applyToWorkingMemory(brain_->getWorkingMemory())) {
                NLM_LOG_WARNING("Failed to apply dopamine modulation to working memory");
            }
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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain not available");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Validate decoded command
    if (decoded == MotorCommand::Wait) {
        NLM_LOG_DEBUG("Motor command decoded as Wait (no activity)");
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        MotorCommand curiosityCmd = selectWithCuriosity(decoded);
        if (curiosityCmd != decoded) {
            NLM_LOG_DEBUG("Curiosity-based exploration changed command from " + 
                         std::to_string(static_cast<int>(decoded)) + " to " +
                         std::to_string(static_cast<int>(curiosityCmd)));
        }
        decoded = curiosityCmd;
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
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_WARNING("Reward modulation disabled or brain not available");
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("Invalid reward value: " + std::to_string(reward));
        return;
    }
    
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_ERROR("Invalid predicted reward value: " + std::to_string(predictedReward));
        return;
    }
    
    // Clip rewards to reasonable range to prevent instability
    const float MAX_REWARD = 10.0f;
    const float MIN_REWARD = -10.0f;
    
    if (reward > MAX_REWARD || reward < MIN_REWARD) {
        NLM_LOG_WARNING("Reward value " + std::to_string(reward) + " outside expected range [" +
                      std::to_string(MIN_REWARD) + ", " + std::to_string(MAX_REWARD) + "]");
        reward = std::clamp(reward, MIN_REWARD, MAX_REWARD);
    }
    
    if (predictedReward > MAX_REWARD || predictedReward < MIN_REWARD) {
        NLM_LOG_WARNING("Predicted reward value " + std::to_string(predictedReward) + 
                      " outside expected range [" + std::to_string(MIN_REWARD) + ", " +
                      std::to_string(MAX_REWARD) + "]");
        predictedReward = std::clamp(predictedReward, MIN_REWARD, MAX_REWARD);
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    bool synapsesUpdated = false;
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_ERROR("Null synapse encountered during reward modulation");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                synapsesUpdated = true;
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    if (!synapsesUpdated) {
        NLM_LOG_DEBUG("No synapses with eligibility traces to update");
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        if (!stdp->setLTPWeight(0.01f * plasticityFactor)) {
            NLM_LOG_WARNING("Failed to set LTP weight in STDP");
        }
        if (!stdp->setLTDWeight(0.012f * plasticityFactor)) {
            NLM_LOG_WARNING("Failed to set LTD weight in STDP");
        }
    } else {
        NLM_LOG_WARNING("STDP system not available for plasticity modulation");
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

} // namespace nlm
