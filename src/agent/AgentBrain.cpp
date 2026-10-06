#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

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
    , conceptBasedSelectionEnabled_(true)
    , plannerIntegrated_(false)
    , lastAction_(ActionType::Wait)
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
        
        // Try to initialize neural planner integration
        if (brain_->getPlanner()) {
            plannerIntegrated_ = true;
            brain_->getPlanner()->setActionQuality(ActionType::MoveForward, 0.5f);
            brain_->getPlanner()->setActionQuality(ActionType::MoveBackward, 0.5f);
            brain_->getPlanner()->setActionQuality(ActionType::TurnLeft, 0.5f);
            brain_->getPlanner()->setActionQuality(ActionType::TurnRight, 0.5f);
            brain_->getPlanner()->setActionQuality(ActionType::Interact, 0.5f);
            brain_->getPlanner()->setActionQuality(ActionType::Wait, 0.5f);
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    conceptBasedSelectionEnabled_ = true;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Initialize concept-based action selection with neural planner
    if (brain_ && brain_->getConceptFormation()) {
        NLM_LOG_INFO("Concept formation system initialized for cognitive integration");
    }
    
    if (plannerIntegrated_) {
        NLM_LOG_INFO("Neural planner integrated for action planning");
    }
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
        
        // Update novelty detection in brain
        if (brain_->getNovelty()) {
            brain_->getNovelty()->setNoveltyLevel(noveltyLevel_);
        }
    }
    
    // Update curiosity and novelty integration
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Update brain's curiosity system
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->setCuriosityLevel(curiosityLevel_);
        }
        
        // Enhance novelty detection with concept formation
        if (brain_ && brain_->getConceptFormation()) {
            // Use concept formation to identify truly novel experiences
            std::vector<float> sensoryPattern;
            
            // Combine all sensory modalities
            for (float val : vision) sensoryPattern.push_back(val);
            for (float val : touch) sensoryPattern.push_back(val);
            for (float val : intern) sensoryPattern.push_back(val);
            for (float val : proprio) sensoryPattern.push_back(val);
            
            // Check if this pattern is novel relative to learned concepts
            if (!sensoryPattern.empty()) {
                size_t matchingConcept = brain_->getConceptFormation()->getMatchingConcept(
                    sensoryPattern, 0.8f);  // High threshold for "familiar"
                
                // If no matching concept, it's truly novel
                if (matchingConcept == 0 || !brain_->getConceptFormation()->getConcept(matchingConcept)) {
                    // Enhance novelty signal for truly novel experiences
                    noveltyLevel_ *= 1.5f;  // Boost novelty for completely new patterns
                    curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.3f);  // Boost curiosity
                }
            }
        }
        
        // Integrate with episodic memory for temporal novelty
        if (brain_ && brain_->getEpisodicMemory()) {
            auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(3);
            if (recentEpisodes.size() >= 2) {
                const auto* prev = recentEpisodes[1];
                const auto* curr = recentEpisodes[0];
                
                // Compute pattern change between recent episodes
                float patternChange = 0.0f;
                if (!prev->pattern.empty() && !curr->pattern.empty()) {
                    for (size_t i = 0; i < std::min(prev->pattern.size(), curr->pattern.size()); ++i) {
                        patternChange += std::abs(prev->pattern[i] - curr->pattern[i]);
                    }
                    patternChange /= std::min(prev->pattern.size(), curr->pattern.size());
                }
                
                // High pattern change indicates temporal novelty
                if (patternChange > 0.3f) {
                    curiosityLevel_ *= 1.2f;  // Boost curiosity for temporal novelty
                }
            }
        }
    }
    
    // Apply neuromodulation to brain's cognitive systems
    if (dopamineLevel_ != 0.0f) {
        if (brain_->getDopamine()) {
            brain_->getDopamine()->setLevel(dopamineLevel_);
        }
        
        // Modulate concept formation with prediction error
        if (brain_ && brain_->getConceptFormation() && brain_->getPredictionErrorSignal()) {
            float predictionError = brain_->getPredictionErrorSignal()->getPredictionError();
            if (std::abs(predictionError) > 0.1f) {  // Significant prediction error
                // Concept formation is more active with prediction errors
                const auto& concepts = brain_->getConceptFormation()->getConcepts();
                for (size_t i = 0; i < concepts.size(); ++i) {
                    // Update concept stability based on prediction error
                    brain_->getConceptFormation()->updateConcept(
                        i, concepts[i].prototype, concepts[i].features, 
                        std::abs(predictionError));
                }
            }
        }
        
        // Modulate neural planner with prediction error
        if (plannerIntegrated_ && brain_->getPlanner()) {
            brain_->getPlanner()->updatePlanQuality(
                std::vector<ActionType>{lastAction_}, 
                std::vector<ActionType>{lastAction_}, 
                std::abs(predictionError));
        }
    }
}
    
    // Update concept formation with current sensory input
    if (brain_ && brain_->getConceptFormation()) {
        // Convert sensory percept to neural pattern for concept formation
        std::vector<float> sensoryPattern;
        
        // Combine all sensory modalities
        for (float val : vision) sensoryPattern.push_back(val);
        for (float val : touch) sensoryPattern.push_back(val);
        for (float val : intern) sensoryPattern.push_back(val);
        for (float val : proprio) sensoryPattern.push_back(val);
        
        // Extract features for concept formation
        std::vector<float> features;
        if (!sensoryPattern.empty()) {
            float mean = 0.0f, variance = 0.0f;
            for (float val : sensoryPattern) {
                mean += val;
            }
            mean /= sensoryPattern.size();
            
            for (float val : sensoryPattern) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= sensoryPattern.size();
            
            features.push_back(mean);
            features.push_back(variance);
            features.push_back(*std::max_element(sensoryPattern.begin(), sensoryPattern.end()));
            features.push_back(*std::min_element(sensoryPattern.begin(), sensoryPattern.end()));
        }
        
        // Present to concept formation system
        brain_->getConceptFormation()->presentExperience(sensoryPattern, features, 
                                                        curiosityLevel_, brain_->getCurrentStep());
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply concept-based action selection if enabled
    if (conceptBasedSelectionEnabled_ && brain_->getConceptFormation()) {\n        decoded = applyConceptBasedSelection(decoded);
    }
    
    // Apply neural planner if integrated
    if (plannerIntegrated_) {
        decoded = applyNeuralPlanner(decoded);
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

MotorCommand AgentBrain::applyConceptBasedSelection(MotorCommand defaultCmd) {
    if (!brain_ || !brain_->getConceptFormation()) {
        return defaultCmd;
    }
    
    // Get current neural activity as pattern for concept matching
    std::vector<float> currentPattern;
    
    // Sample from active neurons (simplified approach)
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                for (Neuron* neuron : pop->getNeurons()) {
                    float activity = std::abs(neuron->getState().membranePotential - 
                                             neuron->getState().restingPotential) / 20.0f;
                    currentPattern.push_back(activity);
                }
            }
        }
    }
    
    if (currentPattern.empty()) {
        return defaultCmd;
    }
    
    // Find concept that matches current neural pattern
    size_t matchingConcept = brain_->getConceptFormation()->getMatchingConcept(
        currentPattern, 0.6f);  // Lower threshold for motor selection
    
    if (matchingConcept > 0) {
        // Concept-based action selection
        // For now, map concepts to motor commands
        // In a real implementation, this would use the concept's stored associations
        switch (matchingConcept % 6) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::applyNeuralPlanner(MotorCommand defaultCmd) {
    if (!brain_ || !brain_->getPlanner() || !plannerIntegrated_) {
        return defaultCmd;
    }
    
    // Get current brain state as neural pattern
    std::vector<float> currentState;
    
    // Collect current neural activity patterns
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory || type == NeuronType::Internal) {
                for (Neuron* neuron : pop->getNeurons()) {
                    float activity = std::abs(neuron->getState().membranePotential - 
                                             neuron->getState().restingPotential) / 20.0f;
                    currentState.push_back(activity);
                }
            }
        }
    }
    
    // Use neural planner to select action based on current state and goals
    if (!currentState.empty()) {
        // Set a simple goal based on current state
        brain_->getPlanner()->setCurrentGoal(currentState);
        
        // Plan action
        ActionType plannedAction = brain_->getPlanner()->planAction(
            currentState, curiosityLevel_ * 0.5f + 0.3f);  // Use curiosity as goal preference
        
        // Convert ActionType to MotorCommand
        switch (plannedAction) {
            case ActionType::MoveForward: return MotorCommand::MoveForward;
            case ActionType::MoveBackward: return MotorCommand::MoveBackward;
            case ActionType::TurnLeft: return MotorCommand::TurnLeft;
            case ActionType::TurnRight: return MotorCommand::TurnRight;
            case ActionType::Interact: return MotorCommand::Interact;
            default: return defaultCmd;
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
    
    // Update brain's prediction error system
    if (brain_->getPredictionErrorSignal()) {
        brain_->getPredictionErrorSignal()->recordError(std::abs(predictionError_), brain_->getCurrentStep());
    }
    
    // Update brain's dopamine system
    if (brain_->getDopamine()) {
        brain_->getDopamine()->setLevel(dopamineLevel_);
    }
    
    // Update planning quality based on reward prediction
    if (plannerIntegrated_) {
        // Get current planned actions (simplified - would need better tracking)
        std::vector<ActionType> plannedActions = {lastAction_};
        std::vector<ActionType> actualActions = {lastAction_};
        brain_->getPlanner()->updatePlanQuality(plannedActions, actualActions, predictionError_);
    }
    
    // Update concept formation with reward prediction error
    if (brain_ && brain_->getConceptFormation()) {
        // Use prediction error to update concept stability
        const auto& concepts = brain_->getConceptFormation()->getConcepts();
        if (!concepts.empty()) {
            size_t conceptId = concepts.size() - 1;  // Use latest concept
            brain_->getConceptFormation()->updateConcept(conceptId, std::vector<float>(), 
                                                         std::vector<float>(), predictionError_);
        }
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
    
    // Update brain's development system
    if (brain_->getDevelopmentSystem()) {
        brain_->getDevelopmentSystem()->update(this, *brain_->getRandomGenerator(), timestep);
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
    conceptBasedSelectionEnabled_ = true;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset brain systems
    if (brain_) {
        brain_->reset();
    }
}

// Get brain pointer
Brain* AgentBrain::getBrain() { return brain_.get(); }

// Configure concept-based action selection
void AgentBrain::enableConceptBasedSelection(bool enable) {
    conceptBasedSelectionEnabled_ = enable;
}

bool AgentBrain::isConceptBasedSelectionEnabled() const {
    return conceptBasedSelectionEnabled_;
}

// Get concept-based action selection status
bool AgentBrain::isPlannerIntegrated() const {
    return plannerIntegrated_;
}

} // namespace nlm
