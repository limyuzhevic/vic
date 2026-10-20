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
    
    // Integrate with brain memory systems for proper episode tracking
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->setMaxEpisodes(10000);
        brain_->getEpisodicMemory()->setConsolidationInterval(100);
    }
    
    // Initialize working memory capacity based on brain size
    if (brain_ && brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->setCapacity(brain_->getTotalNeuronCount() / 20);
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Log integrated systems status
    NLM_LOG_INFO("Integrated Systems:");
    NLM_LOG_INFO("  Working Memory: " + std::string(brain_->getWorkingMemory() ? "YES" : "NO"));
    NLM_LOG_INFO("  Episodic Memory: " + std::string(brain_->getEpisodicMemory() ? "YES" : "NO"));
    NLM_LOG_INFO("  Episodic Episodes: " + std::to_string(brain_->getEpisodicMemory() ? brain_->getEpisodicMemory()->getEpisodeCount() : 0));
    NLM_LOG_INFO("  Associative Memory: " + std::string(brain_->getAssociativeMemory() ? "YES" : "NO"));
    NLM_LOG_INFO("  Prediction System: " + std::string(brain_->getPredictionSystem() ? "YES" : "NO"));
    NLM_LOG_INFO("  Planner: " + std::string(brain_->getPlanner() ? "YES" : "NO"));
    NLM_LOG_INFO("  Concept Formation: " + std::string(brain_->getConceptFormation() ? "YES" : "NO"));
    NLM_LOG_INFO("  Attention System: " + std::string(brain_->getAttention() ? "YES" : "NO"));
    NLM_LOG_INFO("  Development System: " + std::string(brain_->getDevelopmentSystem() ? "YES" : "NO"));
    NLM_LOG_INFO("  Dopamine: " + std::string(brain_->getDopamine() ? "YES" : "NO"));
    NLM_LOG_INFO("  Curiosity: " + std::string(brain_->getCuriosity() ? "YES" : "NO"));
    NLM_LOG_INFO("  Novelty: " + std::string(brain_->getNovelty() ? "YES" : "NO"));
    NLM_LOG_INFO("  Prediction Error: " + std::string(brain_->getPredictionErrorSignal() ? "YES" : "NO"));
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
    
    // Update curiosity and engage integrated systems
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Engage concept formation if novel input
        if (brain_->getConceptFormation() && noveltyLevel_ > 0.1f) {
            // Concept formation processes patterns from current sensory input
            // This helps the agent identify new patterns and generalize
            brain_->getConceptFormation()->update(this, *brain_->getRandomGenerator());
        }
        
        // Engage attention system
        if (brain_->getAttention()) {
            // Attention selects important sensory features for processing
            brain_->getAttention()->update(brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f));
            
            // Apply attention to working memory
            if (brain_->getWorkingMemory() && !brain_->getWorkingMemory()->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = brain_->getWorkingMemory()->getMemoryNeurons();
                brain_->getAttention()->processCompetition(competitors);
            }
        }
    }
    
    // Store current sensory state in working memory
    if (brain_->getWorkingMemory() && !vision.empty()) {
        // Create a sensory pattern representation
        for (size_t i = 0; i < vision.size() && i < sensoryVision_.size(); ++i) {
            if (sensoryVision_[i]) {
                // Store visual feature in working memory
                brain_->getWorkingMemory()->storeToNeuron(
                    sensoryVision_[i]->getId(), 
                    vision[i]
                );
            }
        }
    }
    
    // Store episode in episodic memory
    if (brain_->getEpisodicMemory() && developmentalAge_ > 100.0) {  // Store every 100 steps
        // Create an episodic memory entry
        EpisodicMemoryItem episode;
        episode.timestamp = developmentalAge_;
        episode.reward = dopamineLevel_;
        
        // Store active neurons (firing during this step)
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store reward in episode
        episode.reward = dopamineLevel_;
        
        brain_->getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Update prediction system if available
    if (brain_->getPredictionSystem()) {
        // Prediction system would process temporal sequences
        // For now, just track prediction error
        float prediction = 0.0f;  // Would be computed from learned patterns
        predictionError_ = std::abs(prediction - noveltyLevel_);
    }
    
    // Update attention with current competition
    if (brain_->getAttention() && !sensoryVision_.empty()) {
        // Create a simple competition based on sensory neuron activity
        std::vector<NeuronId> activeNeurons;
        for (Neuron* neuron : sensoryVision_) {
            if (neuron && neuron->isFiring()) {
                activeNeurons.push_back(neuron->getId());
            }
        }
        brain_->getAttention()->processCompetition(activeNeurons);
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
    
    // Integrate with working memory
    if (brain_->getWorkingMemory() && predictionError_ != 0.0f) {
        // Store prediction error in working memory for learning
        // This creates a teaching signal for memory consolidation
        brain_->getWorkingMemory()->storePredictionError(predictionError_);
    }
    
    // Update episodic memory with reward
    if (brain_->getEpisodicMemory() && predictedReward_ != 0.0f) {
        // Create an error-based memory trace
        EpisodicMemoryItem rewardEpisode;
        rewardEpisode.timestamp = developmentalAge_;
        rewardEpisode.reward = predictionError_;
        
        // Store reward prediction error in memory
        brain_->getEpisodicMemory()->storeEpisode(rewardEpisode);
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
        
        // Synaptogenesis during early development
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0002f);
        }
        
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        // Maturation continues
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00001f);
        }
        
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        
        // Pruning continues, synaptogenesis slows
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00005f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00002f);
        }
        
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        
        // Pruning continues at low rates
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00001f);
            brain_->getStructuralPlasticity()->setPruningRate(0.000005f);
        }
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
    
    // Update prediction system with developmental stage
    if (brain_->getPredictionSystem() && developmentalAge_ > 60.0) {
        // Prediction system can learn temporal patterns over time
        brain_->getPredictionSystem()->update(brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f));
    }
    
    // Update concept formation
    if (brain_->getConceptFormation() && developmentalAge_ > 120.0) {
        // Concept formation refines learned patterns over time
        brain_->getConceptFormation()->update(this, *brain_->getRandomGenerator());
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
    
    // Reset brain and all integrated systems
    if (brain_) {
        brain_->reset();
        
        // Clear memory systems
        if (brain_->getWorkingMemory()) {
            brain_->getWorkingMemory()->clear();
        }
        
        if (brain_->getEpisodicMemory()) {
            brain_->getEpisodicMemory()->clear();
        }
        
        if (brain_->getAssociativeMemory()) {
            brain_->getAssociativeMemory()->clear();
        }
    }
}

} // namespace nlm
