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
                        // Distribute sensory neurons - make configurable via config system
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Use config values if available, otherwise default to 4 types
                        size_t sensoryDistributionCount = 4;
                        size_t actionDistributionCount = 6;
                        
                        switch (idx % sensoryDistributionCount) {
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
    
    // Update concept formation with current sensory input
    ConceptFormation* conceptFormation = nullptr;
    if (brain_) {
        conceptFormation = brain_->getConceptFormation();
    }
    
    if (conceptFormation) {
        // Create a comprehensive sensory state for concept formation
        std::vector<float> sensoryState;
        
        // Add vision data
        if (!vision.empty()) {
            sensoryState.insert(sensoryState.end(), vision.begin(), vision.end());
        }
        
        // Add touch data
        if (!touch.empty()) {
            // Normalize touch values
            std::vector<float> normalizedTouch;
            for (float t : touch) {
                float normalized = (t + 1.0f) * 0.5f;  // Normalize from [-1,1] to [0,1]
                normalizedTouch.push_back(normalized);
            }
            sensoryState.insert(sensoryState.end(), normalizedTouch.begin(), normalizedTouch.end());
        }
        
        // Add internal signals
        if (!intern.empty()) {
            std::vector<float> normalizedIntern;
            for (float i : intern) {
                float normalized = (i + 1.0f) * 0.5f;  // Normalize from [-1,1] to [0,1]
                normalizedIntern.push_back(normalized);
            }
            sensoryState.insert(sensoryState.end(), normalizedIntern.begin(), normalizedIntern.end());
        }
        
        // Add proprioception
        if (!proprio.empty()) {
            std::vector<float> normalizedProprio;
            for (float p : proprio) {
                float normalized = (p + 1.0f) * 0.5f;  // Normalize from [-1,1] to [0,1]
                normalizedProprio.push_back(normalized);
            }
            sensoryState.insert(sensoryState.end(), normalizedProprio.begin(), normalizedProprio.end());
        }
        
        // Feed sensory state to concept formation system
        conceptFormation->processSensoryInput(sensoryState);
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }

// Include NeuralPlanner for advanced planning
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/SelfModel.hpp"
#include "../sensory/SensoryInput.hpp"

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Check if we have a NeuralPlanner available (from brain)
    NeuralPlanner* planner = brain_->getPlanner();
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    SelfModel* selfModel = brain_->getSelfModel();
    
    // If all cognition systems are available and connected, use NeuralPlanner
    if (planner && conceptFormation && selfModel && 
        planner->getBrain() && planner->getBrain()->getTotalNeuronCount() > 0) {
        
        // Get current brain state representation for planning
        std::vector<float> currentState;
        
        // Try to get a state representation from the brain
        // For now, we use a simple representation based on neural activity
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            currentState.resize(50, 0.0f);  // Default 50-dimensional state
            size_t idx = 0;
            
            // Get neural activity patterns
            for (const auto& pop : region->getPopulations()) {
                if (idx >= currentState.size()) break;
                
                float avgActivity = 0.0f;
                for (const auto* neuron : pop->getNeurons()) {
                    avgActivity += std::abs(neuron->getState().membranePotential - 
                                           neuron->getState().restingPotential);
                }
                if (!pop->getNeurons().empty()) {
                    avgActivity /= pop->getNeurons().size();
                }
                currentState[idx++] = avgActivity;
            }
            
            // Add working memory content if available
            if (brain_->getWorkingMemory()) {
                const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
                size_t memIdx = 0;
                for (size_t i = 0; i < std::min(currentState.size() - idx, memoryNeurons.size()); ++i) {
                    float activation = brain_->getWorkingMemory()->getNeuronActivation(memoryNeurons[i]);
                    currentState[idx++] = activation;
                }
            }
            
            // Add neuromodulation state
            if (brain_->getDopamine()) {
                float dopamineLevel = brain_->getDopamine()->getLevel();
                currentState[std::min<size_t>(idx, currentState.size() - 1)] = dopamineLevel;
                idx++;
            }
        }
        
        // Use the planner to select action
        MotorCommand plannedAction = planner->planAction(currentState, expectedReward_);
        
        // If planning returned a valid action (not Wait), use it
        if (plannedAction != MotorCommand::Wait) {
            NLM_LOG_INFO("NeuralPlanner selected action: " + std::to_string(static_cast<int>(plannedAction)));
            
            // Store the planned action in self-model for learning
            if (selfModel) {
                // Convert current brain state to vector for self-model
                std::vector<float> brainState = getBrainStateFromBrain(brain_);
                std::vector<float> actionEffect;
                
                // Predict outcome of the action
                if (!brainState.empty()) {
                    actionEffect = selfModel->predictActionConsequence(
                        plannedAction, brainState);
                }
                
                // Record the planned action and its predicted effect
                selfModel->recordSelfAction(plannedAction, brainState, actionEffect);
            }
            
            // Apply curiosity-based exploration to planned action
            if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
                plannedAction = selectWithCuriosity(plannedAction);
            }
            
            return plannedAction;
        }
    }
    
    // Fallback to simple motor neuron decoding if planner not available
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

} // namespace nlm

// Helper function to extract brain state for planning
std::vector<float> AgentBrain::getBrainStateFromBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return std::vector<float>();
    }
    
    // Get first region if available
    NeuralRegion* region = brain->getRegion(RegionId(1));
    if (!region) {
        return std::vector<float>();
    }
    
    // Extract neural activity patterns
    std::vector<float> state;
    state.reserve(100);  // Pre-allocate space
    
    // Get population activity levels
    for (const auto& pop : region->getPopulations()) {
        float avgActivity = 0.0f;
        float minActivity = 1000.0f;  // Large initial value
        float maxActivity = 0.0f;
        int activeNeurons = 0;
        
        for (const auto* neuron : pop->getNeurons()) {
            float activity = std::abs(neuron->getState().membranePotential - 
                                    neuron->getState().restingPotential);
            avgActivity += activity;
            
            if (activity < minActivity) minActivity = activity;
            if (activity > maxActivity) maxActivity = activity;
            
            if (neuron->isFiring()) {
                activeNeurons++;
            }
        }
        
        if (!pop->getNeurons().empty()) {
            avgActivity /= pop->getNeurons().size();
        }
        
        // Add multiple features per population: avg, min, max, active ratio
        state.push_back(avgActivity);  // Average activity
        state.push_back(minActivity);  // Minimum activity
        state.push_back(maxActivity);  // Maximum activity
        state.push_back(static_cast<float>(activeNeurons) / pop->getNeurons().size());  // Active ratio
        
        // Add population type indicator
        int typeId = static_cast<int>(pop->getNeuronType());
        state.push_back(static_cast<float>(typeId));  // Neuron type
    }
    
    // Add working memory state
    if (brain->getWorkingMemory()) {
        const auto& memoryNeurons = brain->getWorkingMemory()->getMemoryNeurons();
        for (size_t i = 0; i < std::min(size_t(10), memoryNeurons.size()); ++i) {
            float activation = brain->getWorkingMemory()->getNeuronActivation(memoryNeurons[i]);
            state.push_back(activation);
        }
    }
    
    // Add neuromodulation state
    if (brain->getDopamine()) {
        state.push_back(brain->getDopamine()->getLevel());
    } else {
        state.push_back(0.0f);
    }
    
    if (brain->getNovelty()) {
        state.push_back(brain->getNovelty()->getLevel());
    } else {
        state.push_back(0.0f);
    }
    
    // Add developmental state
    state.push_back(static_cast<float>(brain->getDevelopmentalStage()));
    
    // Add temporal information
    state.push_back(static_cast<float>(brain->getTotalSpikeCount()));  // Spike history
    
    // Normalize to [0, 1] range for better compatibility with learning algorithms
    if (!state.empty()) {
        float maxVal = *std::max_element(state.begin(), state.end());
        if (maxVal > 0.0f) {
            for (float& val : state) {
                val = std::min(val / maxVal, 1.0f);  // Clamp to 1.0
            }
        }
    }
    
    return state;
}
