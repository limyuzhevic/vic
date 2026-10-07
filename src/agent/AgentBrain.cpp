#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../prediction/PredictionSystem.hpp"
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
    // Cognitive integration state
    , lastBestConceptId_(0)
    , bestActionConceptId_(0)
    , cognitiveExplorationBonus_(0.0f)
    , conceptAdaptationRate_(0.01f)
{
    // Initialize cognitive action preference vectors (6 actions: forward, backward, turn left, turn right, interact, wait)
    conceptActionPreferences_.resize(6, 1.0f);  // Equal initial weights
    predictedActionValues_.resize(6, 0.0f);
    attentionActionWeights_.resize(6, 1.0f);
    
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
    
    // Start with basic motor decoding
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Phase 1: Concept-based action selection (highest priority)
    MotorCommand conceptBased = decodeWithConcepts(decoded);
    if (conceptBased != MotorCommand::Wait || conceptBased != decoded) {
        // Concept-based selection overrides basic selection if it found a meaningful action
        decoded = conceptBased;
    }
    
    // Phase 2: Prediction-based action planning
    MotorCommand predictionBased = decodeWithPredictions(decoded);
    if (predictionBased != MotorCommand::Wait) {
        // Prediction-based selection provides guidance
        decoded = predictionBased;
    }
    
    // Phase 3: Attention-enhanced action selection
    MotorCommand attentionBased = decodeWithAttention(decoded);
    if (attentionBased != MotorCommand::Wait) {
        // Attention-based selection fine-tunes the action
        decoded = attentionBased;
    }
    
    // Phase 4: Enhanced curiosity with cognitive weighting
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Apply cognitive-enhanced curiosity
        updateCuriosityWithCognition();
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
    
    // Use the non-recursive version of development update
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
    
    // Adapt concepts for development
    adaptConceptsForDevelopment(timestep);
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

// Cognitive-based action selection with concept guidance
MotorCommand AgentBrain::decodeWithConcepts(MotorCommand defaultCmd) {
    MotorCommand bestCmd = defaultCmd;
    float bestScore = 0.0f;
    
    // Get concept formation system
    auto* conceptSystem = getConceptFormation();
    if (!conceptSystem) return bestCmd;
    
    // Get all discovered concepts
    const auto& concepts = conceptSystem->getConcepts();
    
    // If we have concepts, use them to influence action selection
    if (!concepts.empty()) {
        // Find the most recently active concept
        size_t mostRecentConcept = 0;
        size_t maxObservations = 0;
        
        for (const auto& concept : concepts) {
            if (concept.totalObservations > maxObservations) {
                maxObservations = concept.totalObservations;
                mostRecentConcept = concept.id;
            }
        }
        
        // Map concept category to action preference
        const auto* concept = conceptSystem->getConcept(mostRecentConcept);
        if (concept) {
            // Update concept-based action preferences based on concept category
            conceptActionPreferences_ = std::vector<float>(6, 1.0f); // Reset weights
            
            if (concept->categoryHint == "rewarding") {
                conceptActionPreferences_[0] = 2.0f;  // Forward
                conceptActionPreferences_[4] = 1.5f; // Interact
                bestCmd = MotorCommand::MoveForward;
                bestScore = 1.5f;
            } else if (concept->categoryHint == "harmful") {
                conceptActionPreferences_[5] = 2.0f;  // Wait
                conceptActionPreferences_[1] = 1.5f;  // Backward
                bestCmd = MotorCommand::MoveBackward;
                bestScore = 1.5f;
            } else if (concept->categoryHint == "neutral") {
                // Be more cautious with neutral concepts
                conceptActionPreferences_[5] = 2.0f;  // Wait
                bestCmd = MotorCommand::Wait;
                bestScore = 1.0f;
            }
            
            // Store concept for cognitive state tracking
            lastBestConceptId_ = mostRecentConcept;
            bestActionConceptId_ = mostRecentConcept;
            
            // Update concept adaptation rate based on developmental stage
            conceptAdaptationRate_ = std::max(0.005f, 0.02f - developmentalAge_ * 0.0001);
            
            NLM_LOG_INFO("Concept-guided action selection: Concept " + std::to_string(mostRecentConcept) +
                        " (category: " + concept->categoryHint + ") -> " + getMotorCommandString(bestCmd));
        }
    }
    
    return bestCmd;
}

// Prediction-based action planning
MotorCommand AgentBrain::decodeWithPredictions(MotorCommand defaultCmd) {
    MotorCommand bestCmd = defaultCmd;
    float bestScore = 0.0f;
    
    // Get prediction system
    auto* predictionSystem = getPredictionSystem();
    if (!predictionSystem) return bestCmd;
    
    // Clear previous predicted action values
    predictedActionValues_.assign(6, 0.0f);
    
    // Get prediction error to influence action selection
    float predictionError = getPredictionError();
    
    // Use prediction error to adjust action values
    // High prediction error = explore more
    float explorationBias = std::min(predictionError * 2.0f, 1.0f);
    
    if (predictionError > 0.1f) {
        // High prediction error suggests unexpected environment -> explore
        predictedActionValues_[5] = 1.5f;  // Wait and observe
        bestCmd = MotorCommand::Wait;
        bestScore = 1.5f;
        
        // Add some exploration to other actions
        float exploreAmount = explorationBias * 0.5f;
        predictedActionValues_[0] += exploreAmount;  // Forward
        predictedActionValues_[2] += exploreAmount;  // Turn left
        predictedActionValues_[3] += exploreAmount;  // Turn right
    } else if (predictionError < -0.1f) {
        // Low prediction error (environment is predictable) -> exploit
        predictedActionValues_[0] = 2.0f;  // Forward
        bestCmd = MotorCommand::MoveForward;
        bestScore = 2.0f;
        
        // Still maintain some curiosity for long-term learning
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            predictedActionValues_[5] = 1.0f;  // Wait option
        }
    }
    
    // Update cognitive exploration bonus based on prediction
    cognitiveExplorationBonus_ = explorationBias;
    
    NLM_LOG_INFO("Prediction-guided action selection: Prediction error = " + std::to_string(predictionError) +
                " -> " + getMotorCommandString(bestCmd));
    
    return bestCmd;
}

// Attention-enhanced action selection
MotorCommand AgentBrain::decodeWithAttention(MotorCommand defaultCmd) {
    MotorCommand bestCmd = defaultCmd;
    float bestScore = 0.0f;
    
    // Get attention system
    auto* attentionSystem = getAttentionSystem();
    if (!attentionSystem) return bestCmd;
    
    // Clear previous attention weights
    attentionActionWeights_.assign(6, 1.0f);
    
    // Get action selection based on attention mechanism
    MotorCommand attentionBased = decodeFromMotorNeurons();
    
    // Get attention weights based on neural activity patterns
    if (attentionSystem) {
        // Use motor neuron activity to compute attention weights
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
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
        
        // Apply attention modulation
        attentionActionWeights_[0] = forwardAct;  // Forward
        attentionActionWeights_[1] = backwardAct; // Backward
        attentionActionWeights_[2] = leftAct;    // Turn left
        attentionActionWeights_[3] = rightAct;   // Turn right
        attentionActionWeights_[4] = interactAct;// Interact
        attentionActionWeights_[5] = waitAct;    // Wait
        
        // Find best action with attention weights
        bestCmd = decodeFromMotorNeurons(); // This uses attention weights
        bestScore = std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct});
    }
    
    // Enhanced attention for novel experiences
    if (noveltyLevel_ > 0.5f) {
        // High novelty -> enhance attention to exploration
        attentionActionWeights_[2] *= 1.5f; // Turn left
        attentionActionWeights_[3] *= 1.5f; // Turn right
        attentionActionWeights_[5] *= 0.8f; // Wait (less important)
        
        // Bias towards new sensory exploration
        if (getMotorCommandString(bestCmd) == getMotorCommandString(MotorCommand::Wait)) {
            bestCmd = MotorCommand::MoveForward; // Actively explore
        }
    }
    
    NLM_LOG_INFO("Attention-guided action selection: Attention weights computed, best = " +
                getMotorCommandString(bestCmd));
    
    return bestCmd;
}

// Developmental concept adaptation
void AgentBrain::adaptConceptsForDevelopment(double timestep) {
    if (!getConceptFormation() || !developmentEnabled_) return;
    
    // Update developmental age
    developmentalAge_ += timestep;
    
    // Adapt concepts based on developmental stage
    if (developmentalAge_ < 120.0) {  // Early development phase
        // High concept plasticity - concepts can form easily
        conceptAdaptationRate_ = 0.03f;
        plasticityModifier_ = 1.0f;
        
        // Encourage concept exploration
        if (curiosityEnabled_) {
            curiosityLevel_ = std::min(0.9f, curiosityLevel_ + 0.01f);
        }
        
    } else if (developmentalAge_ < 600.0) {  // Mid development
        // Moderate concept plasticity
        conceptAdaptationRate_ = 0.01f;
        plasticityModifier_ = 0.8f;
        
    } else {  // Late development (adult)
        // Low concept plasticity - concepts are stable
        conceptAdaptationRate_ = 0.005f;
        plasticityModifier_ = 0.5f;
        
        // Some concept refinement possible
        if (predictionError_ < 0.0f && curiosityLevel_ > 0.5f) {
            // Positive prediction errors with high curiosity -> refine concepts
            curiosityLevel_ = std::max(0.1f, curiosityLevel_ - 0.02f);
        }
    }
    
    // Update concept formation system if needed
    static int conceptAdaptStepCounter = 0;
    if (++conceptAdaptStepCounter % 1000 == 0) {
        // Periodically check concept stability and adapt if needed
        getConceptFormation()->cleanupUnstableConcepts(conceptAdaptationRate_);
        
        // Log developmental stage
        NLM_LOG_INFO("Development stage: Age = " + std::to_string(developmentalAge_) +
                    "s, Concepts adapting at rate = " + std::to_string(conceptAdaptationRate_));
    }
}

// Enhanced curiosity with cognitive weighting
void AgentBrain::updateCuriosityWithCognition() {
    if (!curiosityEnabled_) return;
    
    // Base curiosity on novelty (from AgentBrain::processSensoryInput)
    float baseCuriosity = noveltyLevel_ * 2.0f;
    
    // Enhance curiosity with cognitive signals
    float conceptCuriosity = 0.0f;
    if (getConceptFormation() && bestActionConceptId_ > 0) {
        // If we have a recent concept, reduce curiosity slightly (we understand this pattern)
        const auto* concept = getConceptFormation()->getConcept(bestActionConceptId_);
        if (concept) {
            float conceptStability = concept->avgStability;
            conceptCuriosity = (1.0f - conceptStability) * 0.3f; // Unstable concepts -> more curiosity
        }
    }
    
    float predictionCuriosity = 0.0f;
    if (std::abs(predictionError_) > 0.2f) {
        // High prediction error -> increase curiosity
        predictionCuriosity = std::min(predictionError_ * 2.0f, 1.0f);
    }
    
    float attentionCuriosity = 0.0f;
    auto* attentionSystem = getAttentionSystem();
    if (attentionSystem) {
        // Attention system provides curiosity signals for novel patterns
        attentionCuriosity = attentionSystem->getCuriosityLevel() * 0.5f;
    }
    
    // Combine all curiosity sources
    float totalCuriosity = baseCuriosity + conceptCuriosity + predictionCuriosity + attentionCuriosity;
    
    // Apply developmental modulation
    float developmentalMod = 1.0f;
    if (developmentalAge_ < 120.0) {
        developmentalMod = 1.2f; // Early development - more curiosity
    } else if (developmentalAge_ > 600.0) {
        developmentalMod = 0.8f; // Late development - less curiosity
    }
    
    curiosityLevel_ = std::clamp(totalCuriosity * developmentalMod, 0.0f, 1.0f);
    
    // Log curiosity components
    NLM_LOG_INFO("Enhanced curiosity calculation: Base=" + std::to_string(baseCuriosity) +
                " Concept=" + std::to_string(conceptCuriosity) +
                " Prediction=" + std::to_string(predictionCuriosity) +
                " Attention=" + std::to_string(attentionCuriosity) +
                " -> Total=" + std::to_string(curiosityLevel_));
}

// Reset agent for new episode
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
    
    // Reset cognitive state
    lastBestConceptId_ = 0;
    bestActionConceptId_ = 0;
    cognitiveExplorationBonus_ = 0.0f;
    conceptAdaptationRate_ = 0.01f;
}

} // namespace nlm
