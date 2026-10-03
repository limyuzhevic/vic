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
    // Initialize motor and sensory neuron groups (preserved for backward compatibility)
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

void AgentBrain::processCognitivePipeline(const SensoryPercept& percept, double timestep) {
    if (!brain_) return;
    
    // Step 1: Store in working memory for transient processing
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        // Convert percept to neural pattern for working memory
        std::vector<float> pattern;
        const auto& vision = percept.getVision();
        pattern.insert(pattern.end(), vision.begin(), vision.end());
        
        const auto& touch = percept.getTouch();
        pattern.insert(pattern.end(), touch.begin(), touch.end());
        
        const auto& internal = percept.getInternal();
        pattern.insert(pattern.end(), internal.begin(), internal.end());
        
        const auto& proprio = percept.getProprioception();
        pattern.insert(pattern.end(), proprio.begin(), proprio.end());
        
        workingMemory->store(pattern, 1.0f);
        
        // Update working memory dynamics
        workingMemory->update(timestep);
    }
    
    // Step 2: Apply attentional selection to prioritize processing
    if (auto* attention = brain_->getAttention()) {
        // Get attended regions based on current cognitive state
        std::vector<NeuronId> competitors;
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            const auto& memoryNeurons = workingMemory->getMemoryNeurons();
            competitors.insert(competitors.end(), memoryNeurons.begin(), memoryNeurons.end());
        }
        
        // Process competition and select attention winners
        std::vector<NeuronId> winners = attention->processCompetition(competitors, 0.5f);
        
        // Apply attention to selected regions
        for (NeuronId winner : winners) {
            // Update percept's attention weight based on selection strength
            float attentionStrength = attention->getInhibitionFor(winner);
            percept.setAttentionWeight(attentionStrength);
        }
    }
    
    // Step 3: Form concepts through pattern discovery
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        // Update concept formation with current sensory input
        conceptFormation->update(timestep);
        
        // Get concept relevance for current percept
        size_t conceptCount = conceptFormation->getConceptCount();
        for (size_t i = 0; i < conceptCount; ++i) {
            float relevance = conceptFormation->getConceptStability(i);
            percept.setConceptRelevance(i, relevance);
        }
    }
    
    // Step 4: Generate action plans through neural planning
    if (auto* planner = brain_->getPlanner()) {
        // Update planner with current state and goals
        planner->update(timestep);
        
        // Get current action plan
        std::vector<float> actionPlan = planner->getCurrentGoal();
        
        // Store action plan in percept for motor system access
        // Note: percept doesn't store action plans directly, but this information
        // is available through Brain's planning system
    }
    
    // Step 5: Integrate with neuromodulation systems
    if (auto* novelty = brain_->getNovelty()) {
        novelty->update(timestep);
    }
    
    if (auto* curiosity = brain_->getCuriosity()) {
        curiosity->update(timestep);
    }
    
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        predictionError->update(timestep);
    }
    
    // Update local neuromodulation state from Brain systems
    if (auto* novelty = brain_->getNovelty()) {
        noveltyLevel_ = novelty->getLevel();
    }
    
    if (auto* curiosity = brain_->getCuriosity()) {
        curiosityLevel_ = curiosity->getLevel();
    }
    
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        predictionError_ = predictionError->getError();
    }
}

std::vector<float> AgentBrain::getActionPlan() const {
    if (!brain_) return std::vector<float>();
    
    // Get action plan from Brain's neural planner
    if (auto* planner = brain_->getPlanner()) {
        return planner->getCurrentGoal();
    }
    
    // Fallback: generate simple reactive plan
    std::vector<float> simplePlan;
    // Add current motivational state
    simplePlan.push_back(dopamineLevel_);
    simplePlan.push_back(curiosityLevel_);
    simplePlan.push_back(noveltyLevel_);
    simplePlan.push_back(1.0f - std::abs(predictionError_)); // Stability factor
    
    return simplePlan;
}

void AgentBrain::updateCognitiveSystems(double timestep) {
    if (!brain_) return;
    
    // Update all Brain cognitive systems
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        workingMemory->update(timestep);
    }
    
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        episodicMemory->update(timestep);
    }
    
    if (auto* associativeMemory = brain_->getAssociativeMemory()) {
        associativeMemory->update(timestep);
    }
    
    if (auto* predictionSystem = brain_->getPredictionSystem()) {
        predictionSystem->update(timestep);
    }
    
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        conceptFormation->update(timestep);
    }
    
    if (auto* attention = brain_->getAttention()) {
        attention->update(timestep);
    }
    
    if (auto* planner = brain_->getPlanner()) {
        planner->update(timestep);
    }
    
    if (auto* developmentSystem = brain_->getDevelopmentSystem()) {
        developmentSystem->update(brain_.get(), timestep);
    }
    
    // Update neuromodulation systems
    if (auto* novelty = brain_->getNovelty()) {
        novelty->update(timestep);
    }
    
    if (auto* curiosity = brain_->getCuriosity()) {
        curiosity->update(timestep);
    }
    
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        predictionError->update(timestep);
    }
    
    // Update development system
    if (developmentEnabled_) {
        // Get development stage and update if needed
        if (auto* devSystem = brain_->getDevelopmentSystem()) {
            devSystem->update(brain_.get(), timestep);
        }
    }
}

float AgentBrain::getWorkingMemoryActivity() const {
    if (!brain_) return 0.0f;
    if (auto* workingMemory = brain_->getWorkingMemory()) {
        return workingMemory->getMemoryActivity();
    }
    return 0.0f;
}

float AgentBrain::getPlanningConfidence() const {
    if (!brain_) return 0.0f;
    if (auto* planner = brain_->getPlanner()) {
        return planner->getPlanningConfidence();
    }
    return 0.0f;
}

float AgentBrain::getConceptStability() const {
    if (!brain_) return 0.0f;
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        // Get average concept stability
        size_t conceptCount = conceptFormation->getConceptCount();
        if (conceptCount == 0) return 0.0f;
        
        float totalStability = 0.0f;
        for (size_t i = 0; i < conceptCount; ++i) {
            totalStability += conceptFormation->getConceptStability(i);
        }
        return totalStability / conceptCount;
    }
    return 0.0f;
}

float AgentBrain::getAttentionFocus() const {
    if (!brain_) return 0.0f;
    if (auto* attention = brain_->getAttention()) {
        const auto& winners = attention->getWinners();
        return (winners.empty()) ? 0.0f : 1.0f;
    }
    return 0.0f;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Use Brain's neural planner for action planning (new integrated approach)
    if (auto* planner = brain_->getPlanner()) {
        // Get action plan from Brain's planner
        std::vector<float> actionPlan = planner->getCurrentGoal();
        
        // Convert action plan to MotorCommand
        // Action plan is a vector<float> representing action space
        if (!actionPlan.empty()) {
            // Find best action based on planning confidence
            float planningConfidence = planner->getPlanningConfidence();
            
            // Use motor neuron activity as backup if planner not confident
            if (planningConfidence > 0.5f) {
                // Use planned action (simplified mapping)
                int actionIndex = (actionPlan[0] * getMotorOutputSize()) + 0.5f;
                actionIndex = std::clamp(actionIndex, 0, static_cast<int>(getMotorOutputSize()) - 1);
                
                // Convert to MotorCommand (basic mapping)
                switch (actionIndex) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    default: return MotorCommand::Wait;
                }
            }
        }
    }
    
    // Fallback to local motor decoding (original functionality preserved)
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
    
    // Use Brain's dopamine system for proper neuromodulation integration
    if (auto* dopamine = brain_->getDopamine()) {
        // Update dopamine level based on prediction error
        float newDopamineLevel = predictionError_;
        newDopamineLevel = std::clamp(newDopamineLevel, -1.0f, 1.0f);
        dopamine->setLevel(newDopamineLevel);
        
        // Signal reward/prediction error to dopamine system
        dopamine->signalRewardPredictionError(predictionError_);
        
        // Update local state for backward compatibility
        dopamineLevel_ = newDopamineLevel;
        
        // Use Brain's neuromodulation system with a simple signal structure
        // Create a simple neuromodulation signal structure
        struct NeuromodulationSignal {
            float level;
            float concentration;
            const Neuromodulator* target;
            int effect;
            
            NeuromodulationSignal() : level(0.0f), concentration(0.0f), target(nullptr), effect(0) {}
        };
        
        NeuromodulationSignal neuromodSignal;
        neuromodSignal.level = predictionError_;
        neuromodSignal.concentration = predictionError_;
        neuromodSignal.target = dopamine;
        neuromodSignal.effect = 1; // Plasticity
        
        brain_->applyNeuromodulation(neuromodSignal);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    // Use Brain's development system for integrated developmental changes
    if (auto* devSystem = brain_->getDevelopmentSystem()) {
        devSystem->update(brain_.get(), timestep);
        
        // Update local development state from Brain's system
        developmentalAge_ = devSystem->getDevelopmentalAge();
        plasticityModifier_ = devSystem->getPlasticityModifier();
        
        // Set developmental stage in Brain
        DevelopmentalStage stage = devSystem->getStage();
        brain_->setDevelopmentalStage(stage);
    } else {
        // Fallback to original local development logic if Brain system not available
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
    // Clear local state (some state is now managed by Brain's cognitive systems)
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision for novelty detection
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
