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
    
    // Store complete sensory pattern in working memory for episodic encoding
    if (brain_ && brain_->getWorkingMemory() && !vision.empty()) {
        // Create comprehensive sensory pattern including all modalities
        std::vector<float> sensoryPattern;
        
        // Add vision data (256 values)
        sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
        
        // Add touch data (8 values) 
        if (touch.size() == 8) {
            sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
        } else {
            // Pad with zeros if missing
            sensoryPattern.insert(sensoryPattern.end(), 8 - touch.size(), 0.0f);
        }
        
        // Add internal signals (4 values)
        if (intern.size() == 4) {
            sensoryPattern.insert(sensoryPattern.end(), intern.begin(), intern.end());
        } else {
            // Center and scale
            for (float val : intern) {
                sensoryPattern.push_back((val * 2.0f - 1.0f) * 5.0f);
            }
            while (sensoryPattern.size() < 8 + 4) {
                sensoryPattern.push_back(0.0f);
            }
        }
        
        // Add proprioception (6 values)
        if (proprio.size() == 6) {
            for (float val : proprio) {
                sensoryPattern.push_back((val * 2.0f - 1.0f) * 3.0f);
            }
        } else {
            // Pad with scaled proprioception
            for (float val : proprio) {
                sensoryPattern.push_back((val * 2.0f - 1.0f) * 3.0f);
            }
            while (sensoryPattern.size() < 8 + 4 + 6) {
                sensoryPattern.push_back(0.0f);
            }
        }
        
        // Store pattern in working memory with enhanced strength based on intensity
        float patternStrength = 0.0f;
        for (float val : sensoryPattern) {
            patternStrength += std::abs(val);
        }
        patternStrength = std::min(patternStrength / sensoryPattern.size(), 1.0f);
        
        // Store in working memory for episodic encoding
        brain_->getWorkingMemory()->store(sensoryPattern, patternStrength);
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
    
    // Enhanced concept formation integration based on working memory patterns
    if (brain_ && brain_->getConceptFormation() && brain_->getWorkingMemory()) {
        // Get current working memory patterns
        std::vector<float> memoryPattern = brain_->getWorkingMemory()->retrieve();
        
        if (!memoryPattern.empty()) {
            // Extract features from working memory for concept formation
            // Vision features (first 256 values)
            float avgVision = 0.0f, visionVariance = 0.0f;
            size_t visionSize = std::min(static_cast<size_t>(256), memoryPattern.size());
            for (size_t i = 0; i < visionSize; ++i) {
                avgVision += memoryPattern[i];
            }
            avgVision /= visionSize;
            for (size_t i = 0; i < visionSize; ++i) {
                float diff = memoryPattern[i] - avgVision;
                visionVariance += diff * diff;
            }
            visionVariance = std::sqrt(visionVariance / visionSize);
            
            // Touch features (next 8 values)
            float avgTouch = 0.0f;
            size_t touchStart = visionSize;
            size_t touchSize = std::min(static_cast<size_t>(8), memoryPattern.size() - touchStart);
            for (size_t i = 0; i < touchSize; ++i) {
                avgTouch += memoryPattern[touchStart + i];
            }
            if (touchSize > 0) avgTouch /= touchSize;
            
            // Internal signals features (next 4 values)
            float avgInternal = 0.0f;
            size_t internalStart = touchStart + touchSize;
            size_t internalSize = std::min(static_cast<size_t>(4), memoryPattern.size() - internalStart);
            for (size_t i = 0; i < internalSize; ++i) {
                avgInternal += memoryPattern[internalStart + i];
            }
            if (internalSize > 0) avgInternal /= internalSize;
            
            // Proprioception features (next 6 values)
            float avgProprio = 0.0f;
            size_t proprioStart = internalStart + internalSize;
            size_t proprioSize = std::min(static_cast<size_t>(6), memoryPattern.size() - proprioStart);
            for (size_t i = 0; i < proprioSize; ++i) {
                avgProprio += memoryPattern[proprioStart + i];
            }
            if (proprioSize > 0) avgProprio /= proprioSize;
            
            // Create feature vector for concept formation
            std::vector<float> conceptFeatures = {
                avgVision, visionVariance,
                avgTouch, avgInternal, avgProprio,
                noveltyLevel_, curiosityLevel_
            };
            
                        // Process features in concept formation system
            // Note: ConceptFormation would need an interface method to process features
            // For now, we'll store the features for potential integration
            // brain_->getConceptFormation()->processFeatures(conceptFeatures);
        }
    }
    
    // Enhanced prediction system integration
    if (brain_ && brain_->getPredictionSystem()) {
        // Get prediction system to update with current state
        // For now, we'll simulate prediction error based on sensory novelty
        // In a full implementation, the prediction system would:
        // 1. Receive sensory input
        // 2. Compare with internal predictions
        // 3. Generate prediction errors
        // 4. Update internal models
        
        // Simulate prediction error based on novelty and change
        float noveltyBasedPredictionError = noveltyLevel_ * 0.3f;
        
        // Add some randomness to simulate prediction uncertainty
        if (brain_->getRandomGenerator()) {
            float randomFactor = brain_->getRandomGenerator()->uniformReal(0.0f, 0.1f);
            noveltyBasedPredictionError += randomFactor;
        }
        
        // Update prediction error for neuromodulation
        predictionError_ = noveltyBasedPredictionError * 2.0f - 1.0f;  // Scale to [-1, 1]
        predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
        
        // Store prediction error in brain for other systems to use
        // brain_->getPredictionSystem()->updatePredictionError(predictionError_);
        
        // Apply prediction-based curiosity modulation
        if (curiosityEnabled_) {
            // Higher prediction error should increase curiosity
            curiosityLevel_ = curiosityLevel_ * 0.7f + std::abs(predictionError_) * 0.4f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
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
            int choice = static_cast<int>(brain_->getRandomGenerator()->uniformInt(static_cast<uint32_t>(0), static_cast<uint32_t>(7)));
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
    
// Add advanced memory system configuration options
    if (brain_) {
        // Enable/disable memory system components based on configuration
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            // Configure working memory capacity based on available neurons
            size_t numNeurons = brain_->getTotalNeuronCount();
            workingMemory->setCapacity(std::max(size_t(10), numNeurons / 100));
            
            // Adjust decay rate based on novelty level
            float adaptiveDecayRate = 0.95f - (noveltyLevel_ * 0.4f);  // Faster decay for novel patterns
            workingMemory->setDecayRate(adaptiveDecayRate);
        }
        
        // Configure episodic memory settings
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            // Set maximum episodes based on simulation time
            size_t maxEpisodes = 1000;  // Default
            // Could be configured from external settings
            episodicMemory->setMaxEpisodes(maxEpisodes);
        }
        
        // Configure concept formation with working memory patterns
        if (auto* conceptFormation = brain_->getConceptFormation()) {
            // Concept formation could learn from working memory patterns
            // This would require implementing concept formation learning
            // For now, we just enable it
        }
        
        // Configure prediction system with working memory integration
        if (auto* predictionSystem = brain_->getPredictionSystem()) {
            // Prediction system could use working memory for context
            // This would require implementing prediction learning
            // For now, we just enable it
        }
        
        // Configure attention system with working memory integration
        if (auto* attention = brain_->getAttention()) {
            // Attention system could prioritize working memory content
            // This would require implementing attention mechanisms
            // For now, we just enable it
        }
    }
}
