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
    , memoryStorageEnabled_(true)
    , predictionEnabled_(true)
    , cognitionEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , episodeActive_(false)
    , currentStepInEpisode_(0)
    , currentPlanStep_(0)
    , attentionLevel_(0.5f)
    , explorationDrive_(0.5f)
{
    // Initialize integrated system references
    if (brain_) {
        // Get system pointers from brain
        workingMemory_ = std::shared_ptr<NeuralWorkingMemory>(brain_->getWorkingMemory());
        episodicMemory_ = std::shared_ptr<NeuralEpisodicMemory>(brain_->getEpisodicMemory());
        associativeMemory_ = std::shared_ptr<NeuralAssociativeMemory>(brain_->getAssociativeMemory());
        predictionSystem_ = std::shared_ptr<PredictionSystem>(brain_->getPredictionSystem());
        planner_ = std::shared_ptr<NeuralPlanner>(brain_->getPlanner());
        conceptFormation_ = std::shared_ptr<ConceptFormation>(brain_->getConceptFormation());
        attention_ = std::shared_ptr<AttentionalSelection>(brain_->getAttention());
        dopamine_ = std::shared_ptr<Dopamine>(brain_->getDopamine());
        curiosity_ = std::shared_ptr<Curiosity>(brain_->getCuriosity());
        novelty_ = std::shared_ptr<Novelty>(brain_->getNovelty());
        predictionError_ = std::shared_ptr<PredictionError>(brain_->getPredictionErrorSignal());
        
        // Initialize motor and sensory neuron groups
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
    episodeActive_ = true;
    currentStepInEpisode_ = 0;
    attentionLevel_ = 0.5f;
    explorationDrive_ = 0.5f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Log integrated system status
    if (workingMemory_) {
        NLM_LOG_INFO("Working memory: capacity set for " + std::to_string(workingMemory_->getCapacity()) + " traces");
    }
    if (predictionSystem_) {
        NLM_LOG_INFO("Prediction system: initialized for sensory prediction");
    }
    if (conceptFormation_) {
        NLM_LOG_INFO("Concept formation: initialized for pattern discovery");
    }
    if (attention_) {
        NLM_LOG_INFO("Attention system: initialized for competitive selection");
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
    if (!brain_ || !episodeActive_) return;
    
    currentStepInEpisode_++;
    
    // Store in working memory (first 100 steps)
    if (memoryStorageEnabled_ && currentStepInEpisode_ < 100 && workingMemory_) {
        workingMemory_->storeCurrentState();
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Update prediction system (encode sensory input patterns)
    if (predictionEnabled_ && predictionSystem_) {
        // Encode current visual input as prediction target
        predictionSystem_->encodeObservation(vision);
        
        // Compute prediction error
        float prediction = predictionSystem_->predictNextStep();
        predictionError_ = std::abs(prediction - (vision.empty() ? 0.0f : vision[0]));
    }
    
    // Update attention based on sensory input
    if (cognitionEnabled_ && attention_) {
        // Create attention vector from sensory input
        std::vector<float> attentionVector;
        attentionVector.reserve(vision.size() + percept.getTouch().size() + 
                               percept.getInternal().size() + percept.getProprioception().size());
        
        attentionVector.insert(attentionVector.end(), vision.begin(), vision.end());
        attentionVector.insert(attentionVector.end(), percept.getTouch().begin(), percept.getTouch().end());
        attentionVector.insert(attentionVector.end(), percept.getInternal().begin(), percept.getInternal().end());
        attentionVector.insert(attentionVector.end(), percept.getProprioception().begin(), percept.getProprioception().end());
        
        attention_->updateAttention(attentionVector);
        attentionLevel_ = attention_->getAttentionLevel();
    }
    
    // Update concept formation with sensory patterns
    if (cognitionEnabled_ && conceptFormation_) {
        conceptFormation_->processExperience(vision);
    }
    
    // Update novelty (difference from previous vision)
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        explorationDrive_ = curiosityLevel_;
    }
    
    // Store experience in episodic memory
    if (memoryStorageEnabled_ && episodicMemory_) {
        EpisodicMemoryItem episode;
        episode.timestamp = currentStepInEpisode_;
        episode.reward = 0.0f; // Will be set later
        episode.visionData = vision;
        
        // Store episode
        episodicMemory_->storeEpisode(episode);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_ || !episodeActive_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply planning if enabled
    if (cognitionEnabled_ && planner_ && !plannedSequence_.empty()) {
        if (currentPlanStep_ < plannedSequence_.size()) {
            decoded = plannedSequence_[currentPlanStep_++];
        } else {
            plannedSequence_.clear();
            currentPlanStep_ = 0;
        }
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply attention modulation
    if (attentionLevel_ < 0.3f) {
        // Low attention = more random exploration
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
    if (!brain_ || !episodeActive_ || !rewardModulationEnabled_) return;
    
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
    
    // Update neuromodulation systems
    if (dopamine_) {
        dopamine_->setLevel(dopamineLevel_);
        dopamine_->update(0.1f);
    }
    
    if (predictionError_) {
        predictionError_->setError(predictionError_);
        predictionError_->update(0.1f);
    }
    
    if (curiosity_) {
        curiosity_->setLevel(curiosityLevel_);
        curiosity_->update(0.1f);
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
    
    // Update episodic memory with reward
    if (memoryStorageEnabled_ && episodicMemory_ && !episodicMemory_->getEpisodes().empty()) {
        // Update the most recent episode with reward
        auto episodes = episodicMemory_->getEpisodes();
        if (!episodes.empty()) {
            episodes.back()->reward = reward;
            episodicMemory_->updateEpisode(episodes.back().get(), reward);
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update development system
    if (attention_) {
        attention_->update(timestep);
    }
    if (curiosity_) {
        curiosity_->update(timestep);
    }
    
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
    
    // Update attention system for developmental stage
    if (attention_) {
        float attentionMod = (developmentalAge_ < 300.0) ? 1.5f : 0.8f;
        attention_->setInhibitionStrength(0.5f / attentionMod);
        attention_->setExcitationStrength(1.5f * attentionMod);
    }
    
    // Update concept formation for developmental stage
    if (conceptFormation_) {
        conceptFormation_->setLearningRate(0.01f * plasticityModifier_);
    }
    
    // Update prediction system for developmental stage
    if (predictionSystem_) {
        predictionSystem_->setPredictionConfidence(0.5f + 0.5f * plasticityModifier_);
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
    episodeActive_ = true;
    currentStepInEpisode_ = 0;
    attentionLevel_ = 0.5f;
    explorationDrive_ = 0.5f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear planned sequences
    plannedSequence_.clear();
    currentPlanStep_ = 0;
    
    // Reset episodic memory if available
    if (episodicMemory_) {
        episodicMemory_->clear();
    }
}

// Integrated system access methods
NeuralWorkingMemory* AgentBrain::getWorkingMemory() const {
    return workingMemory_.get();
}

NeuralEpisodicMemory* AgentBrain::getEpisodicMemory() const {
    return episodicMemory_.get();
}

NeuralAssociativeMemory* AgentBrain::getAssociativeMemory() const {
    return associativeMemory_.get();
}

PredictionSystem* AgentBrain::getPredictionSystem() const {
    return predictionSystem_.get();
}

NeuralPlanner* AgentBrain::getPlanner() const {
    return planner_.get();
}

ConceptFormation* AgentBrain::getConceptFormation() const {
    return conceptFormation_.get();
}

AttentionalSelection* AgentBrain::getAttention() const {
    return attention_.get();
}

Dopamine* AgentBrain::getDopamine() const {
    return dopamine_.get();
}

Curiosity* AgentBrain::getCuriosity() const {
    return curiosity_.get();
}

Novelty* AgentBrain::getNovelty() const {
    return novelty_.get();
}

PredictionError* AgentBrain::getPredictionErrorSignal() const {
    return predictionError_.get();
}

void AgentBrain::advanceDevelopment() {
    updateDevelopment(0.1f); // Advance by small timestep
}

void AgentBrain::triggerMemoryConsolidation() {
    if (!episodicMemory_) return;
    
    // Consolidate important memories, remove weak ones
    episodicMemory_->consolidate(0.3f);
    
    // Update working memory with consolidated patterns
    if (workingMemory_) {
        workingMemory_->updateConsolidatedPatterns();
    }
}

void AgentBrain::triggerMemoryReplay() {
    if (!episodicMemory_) return;
    
    // Get episodes for replay (every 100 steps)
    auto episodesToReplay = episodicMemory_->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        episodicMemory_->replayEpisode(episode);
    }
}

void AgentBrain::updateSystemParameters() {
    // Update all system parameters based on current state
    if (dopamine_) {
        dopamine_->update(0.1f);
    }
    if (curiosity_) {
        curiosity_->update(0.1f);
    }
    if (novelty_) {
        novelty_->update(0.1f);
    }
    if (predictionError_) {
        predictionError_->update(0.1f);
    }
    if (attention_) {
        attention_->update(0.1f);
    }
    if (conceptFormation_) {
        conceptFormation_->update(0.1f);
    }
    if (predictionSystem_) {
        predictionSystem_->update(0.1f);
    }
}

void AgentBrain::storeExperience(const SensoryPercept& percept, MotorCommand action, float reward) {
    if (!memoryStorageEnabled_) return;
    
    // Store in episodic memory
    if (episodicMemory_) {
        EpisodicMemoryItem episode;
        episode.timestamp = currentStepInEpisode_;
        episode.reward = reward;
        episode.action = action;
        episode.visionData = percept.getVision();
        episode.touchData = percept.getTouch();
        episode.internalData = percept.getInternal();
        episode.proprioceptionData = percept.getProprioception();
        
        episodicMemory_->storeEpisode(episode);
    }
    
    // Update associative memory
    if (associativeMemory_) {
        // Create pattern from current percept
        std::vector<float> pattern;
        pattern.reserve(percept.getVision().size() + action * 1000); // Scale action
        
        // Add visual pattern
        pattern.insert(pattern.end(), percept.getVision().begin(), percept.getVision().end());
        
        // Add action encoding
        float actionValue = static_cast<float>(action) / 7.0f; // Normalize to [0,1]
        pattern.push_back(actionValue);
        pattern.push_back(reward);
        
        associativeMemory_->associate(pattern, reward);
    }
    
    // Update concept formation
    if (conceptFormation_) {
        conceptFormation_->processExperience(percept.getVision());
    }
    
    // Update prediction system
    if (predictionSystem_) {
        predictionSystem_->recordOutcome(reward);
    }
}

void AgentBrain::retrieveMemoryPattern(const NeuralWorkingMemory::MemoryPattern& pattern) {
    if (!workingMemory_) return;
    
    // Store pattern in working memory
    workingMemory_->storePattern(pattern);
    
    // Trigger attention if pattern is novel
    if (attention_) {
        attention_->processNovelPattern(pattern);
    }
    
    // Update concept if pattern contains new information
    if (conceptFormation_) {
        conceptFormation_->integratePattern(pattern);
    }
}

void AgentBrain::updateConceptFromExperience(const SensoryPercept& percept) {
    if (!conceptFormation_) return;
    
    // Extract features from percept for concept formation
    std::vector<float> features;
    
    // Vision features
    if (!percept.getVision().empty()) {
        // Simple feature extraction: average, variance, max, min
        float sum = 0.0f;
        float sumSq = 0.0f;
        float max = percept.getVision()[0];
        float min = percept.getVision()[0];
        
        for (float val : percept.getVision()) {
            sum += val;
            sumSq += val * val;
            max = std::max(max, val);
            min = std::min(min, val);
        }
        
        int n = percept.getVision().size();
        float avg = sum / n;
        float variance = (sumSq / n) - (avg * avg);
        
        features.push_back(avg);
        features.push_back(variance);
        features.push_back(max);
        features.push_back(min);
    }
    
    // Add other sensory features
    for (float val : percept.getTouch()) features.push_back(val);
    for (float val : percept.getInternal()) features.push_back(val);
    for (float val : percept.getProprioception()) features.push_back(val);
    
    // Update concept formation with features
    conceptFormation_->addFeatureVector(features);
}

std::vector<MotorCommand> AgentBrain::planSequence(int depth) {
    if (!planner_ || !cognitionEnabled_) return {};
    
    plannedSequence_ = planner_->generatePlan(depth);
    currentPlanStep_ = 0;
    
    return plannedSequence_;
}

} // namespace nlm
