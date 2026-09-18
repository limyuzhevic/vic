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
    , cognitiveMode_(CognitiveMode::Passive)
    , attentionTarget_(INVALID_NEURON_ID)
    , planningDepth_(3)
    , curiosityThreshold_(0.3f)
    , attentionSpan_(1.0f)
{
    // Initialize cognitive system connections
    if (brain_) {
        // Connect cognitive systems to brain for access
        planner_ = brain_->getPlanner();
        conceptFormation_ = brain_->getConceptFormation();
        attention_ = brain_->getAttention();
        curiositySystem_ = brain_->getCuriosity();
        predictionSystem_ = brain_->getPredictionSystem();
        developmentSystem_ = brain_->getDevelopmentSystem();
        
        // Also maintain direct access to memory systems
        workingMemory_ = brain_->getWorkingMemory();
        episodicMemory_ = brain_->getEpisodicMemory();
        associativeMemory_ = brain_->getAssociativeMemory();
        
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

NeuralPlanner* AgentBrain::getPlanner() const { return planner_.get(); }
ConceptFormation* AgentBrain::getConceptFormation() const { return conceptFormation_.get(); }
AttentionalSelection* AgentBrain::getAttention() const { return attention_.get(); }
Curiosity* AgentBrain::getCuriositySystem() const { return curiositySystem_.get(); }
PredictionSystem* AgentBrain::getPredictionSystem() const { return predictionSystem_.get(); }
DevelopmentSystem* AgentBrain::getDevelopmentSystem() const { return developmentSystem_.get(); }

NeuralWorkingMemory* AgentBrain::getWorkingMemory() const { return workingMemory_.get(); }
NeuralEpisodicMemory* AgentBrain::getEpisodicMemory() const { return episodicMemory_.get(); }
NeuralAssociativeMemory* AgentBrain::getAssociativeMemory() const { return associativeMemory_.get(); }

void AgentBrain::setCognitiveMode(CognitiveMode mode) { cognitiveMode_ = mode; }
void AgentBrain::setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
void AgentBrain::setPlanningDepth(int depth) { planningDepth_ = depth; }
void AgentBrain::setAttentionSpan(float span) { attentionSpan_ = span; }

void AgentBrain::enableAdvancedPlanning(bool enable) { advancedPlanning_ = enable; }
void AgentBrain::enableConceptLearning(bool enable) { conceptLearning_ = enable; }
void AgentBrain::enableSocialLearning(bool enable) { socialLearning_ = enable; }

void AgentBrain::trackConcept(const std::string& concept) { concepts_.insert(concept); }
bool AgentBrain::hasConcept(const std::string& concept) const { return concepts_.find(concept) != concepts_.end(); }
size_t AgentBrain::getConceptCount() const { return concepts_.size(); }

void AgentBrain::trackSocialInteraction(const std::string& interaction) { socialInteractions_.insert(interaction); }
bool AgentBrain::hasSocialInteraction(const std::string& interaction) const { return socialInteractions_.find(interaction) != socialInteractions_.end(); }

void AgentBrain::recordPredictionError(float error) { 
    predictionErrors_.push_back(error);
    if (predictionErrors_.size() > 100) {
        predictionErrors_.erase(predictionErrors_.begin());
    }
    averagePredictionError_ = 0.0f;
    for (float e : predictionErrors_) averagePredictionError_ += e;
    averagePredictionError_ /= predictionErrors_.size();
}

float AgentBrain::getAveragePredictionError() const { return averagePredictionError_; }

void AgentBrain::processConceptLearning(const std::vector<float>& patterns) {
    if (!conceptLearning_ || !conceptFormation_) return;
    
    // Send patterns to concept formation for processing
    conceptFormation_->processPattern(patterns);
    
    // Track discovered concepts
    auto concepts = conceptFormation_->getActiveConcepts();
    for (const auto& concept : concepts) {
        trackConcept(concept.name);
    }
}

void AgentBrain::processSocialLearning(const std::vector<float>& socialPatterns) {
    if (!socialLearning_) return;
    
    // Process social patterns to learn interaction patterns
    // This would connect to agent's social learning mechanisms
    if (predictionSystem_) {
        predictionSystem_->updateWithSocialData(socialPatterns);
    }
    
    // Track observed social interactions
    // This would typically involve recording agent behaviors from environment
}

void AgentBrain::updatePlanningSystem(double timestep) {
    if (!advancedPlanning_ || !planner_) return;
    
    // Update neural planner with current state
    planner_->update(timestep);
    
    // Get planned actions and store in action values
    auto plans = planner_->getPlans();
    for (const auto& plan : plans) {
        actionValues_[plan.action] = plan.value;
    }
}

void AgentBrain::updateAttentionSystem(double timestep) {
    if (!attention_) return;
    
    // Update attention system
    attention_->update(timestep);
    
    // Get attentional focus
    auto focus = attention_->getAttentionalFocus();
    if (!focus.empty()) {
        attentionTarget_ = focus[0];  // Focus on first neuron
    }
}

void AgentBrain::saveState(std::ostream& stream) const {
    // Save agent state for checkpointing
    uint32_t version = 1;
    stream.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Save cognitive mode
    stream.write(reinterpret_cast<const char*>(&cognitiveMode_), sizeof(cognitiveMode_));
    
    // Save concept set
    size_t conceptCount = concepts_.size();
    stream.write(reinterpret_cast<const char*>(&conceptCount), sizeof(conceptCount));
    for (const auto& concept : concepts_) {
        size_t length = concept.length();
        stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
        stream.write(concept.c_str(), length);
    }
    
    // Save prediction errors
    size_t errorCount = predictionErrors_.size();
    stream.write(reinterpret_cast<const char*>(&errorCount), sizeof(errorCount));
    stream.write(reinterpret_cast<const char*>(predictionErrors_.data()),
                predictionErrors_.size() * sizeof(float));
}

void AgentBrain::loadState(std::istream& stream) {
    // Load agent state from checkpoint
    uint32_t version;
    stream.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    if (version != 1) return;  // Version mismatch
    
    // Load cognitive mode
    stream.read(reinterpret_cast<char*>(&cognitiveMode_), sizeof(cognitiveMode_));
    
    // Load concept set
    size_t conceptCount;
    stream.read(reinterpret_cast<char*>(&conceptCount), sizeof(conceptCount));
    concepts_.clear();
    for (size_t i = 0; i < conceptCount; ++i) {
        size_t length;
        stream.read(reinterpret_cast<char*>(&length), sizeof(length));
        std::string concept(length, ' ');
        stream.read(&concept[0], length);
        concepts_.insert(concept);
    }
    
    // Load prediction errors
    size_t errorCount;
    stream.read(reinterpret_cast<char*>(&errorCount), sizeof(errorCount));
    predictionErrors_.resize(errorCount);
    if (errorCount > 0) {
        stream.read(reinterpret_cast<char*>(predictionErrors_.data()),
                   errorCount * sizeof(float));
        averagePredictionError_ = 0.0f;
        for (float e : predictionErrors_) averagePredictionError_ += e;
        averagePredictionError_ /= predictionErrors_.size();
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
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
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
