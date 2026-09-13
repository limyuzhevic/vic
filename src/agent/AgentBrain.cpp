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
        // First, collect all neurons by type from the brain
        std::vector<Neuron*> allMotorNeurons;
        std::vector<Neuron*> allSensoryNeurons;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        allMotorNeurons.push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        allSensoryNeurons.push_back(n);
                    }
                }
            }
        }
        
        // Distribute motor neurons into groups
        for (size_t i = 0; i < allMotorNeurons.size(); ++i) {
            size_t group = i % 6;  // 6 motor groups: forward, backward, left, right, interact, wait
            switch (group) {
                case 0: motorForward_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
                case 1: motorBackward_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
                case 2: motorTurnLeft_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
                case 3: motorTurnRight_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
                case 4: motorInteract_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
                case 5: motorWait_.push_back(std::shared_ptr<Neuron>(allMotorNeurons[i], [](Neuron*) {})); break;
            }
        }
        
        // Distribute sensory neurons into groups
        for (size_t i = 0; i < allSensoryNeurons.size(); ++i) {
            size_t group = i % 4;  // 4 sensory groups: vision, touch, internal, proprioception
            switch (group) {
                case 0: sensoryVision_.push_back(std::shared_ptr<Neuron>(allSensoryNeurons[i], [](Neuron*) {})); break;
                case 1: sensoryTouch_.push_back(std::shared_ptr<Neuron>(allSensoryNeurons[i], [](Neuron*) {})); break;
                case 2: sensoryInternal_.push_back(std::shared_ptr<Neuron>(allSensoryNeurons[i], [](Neuron*) {})); break;
                case 3: sensoryProprioception_.push_back(std::shared_ptr<Neuron>(allSensoryNeurons[i], [](Neuron*) {})); break;
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
        const auto& neuron = sensoryVision_[i];
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        const auto& neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        const auto& neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        const auto& neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
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
    
    // Integrate with brain's memory and prediction systems
    integrateSensoryInput(vision);
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
    auto calcActivity = [](const std::vector<std::shared_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& n : neurons) {
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

void AgentBrain::integrateSensoryInput(const std::vector<float>& sensoryPattern) {
    if (!brain_) return;
    
    // Access integrated systems
    auto* workingMemory = getWorkingMemory();
    auto* episodicMemory = getEpisodicMemory();
    auto* associativeMemory = getAssociativeMemory();
    auto* predictionSystem = getPredictionSystem();
    auto* attention = getAttention();
    auto* curiosity = getCuriosity();
    auto* novelty = getNovelty();
    auto* predictionError = getPredictionError();
    
    // Update working memory with sensory input
    if (workingMemory) {
        workingMemory->store(sensoryPattern, 1.0f);
    }
    
    // Update novelty detection
    if (novelty) {
        novelty->detectNovelty(sensoryPattern, previousVision_);
    }
    
    // Update prediction system with sensory input
    if (predictionSystem) {
        // The prediction system would process the sensory input
        // For now, we'll store it for future reference
        predictionSystem->storeSensoryObservation(sensoryPattern[0], 0);
    }
    
    // Apply attentional selection
    if (attention) {
        // Simple attentional modulation based on stimulus strength
        if (!sensoryPattern.empty() && sensoryPattern[0] > 0.5f) {
            attention->applyTopDownBias(sensoryVision_.empty() ? 0 : sensoryVision_[0]->getId(), sensoryPattern[0]);
        }
    }
    
    // Update curiosity system with novelty
    if (curiosity && novelty) {
        curiosity->update(novelty->getLevel(), predictionError_->getError(), 1.0f);
    }
    
    // Update episodic memory
    if (episodicMemory) {
        // Store current sensory state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0;  // Simplified
        episode.sensoryState = sensoryPattern;
        episode.reward = 0.0f;  // Will be updated by reward modulation
        episodicMemory->storeEpisode(episode);
    }
    
    // Update prediction error
    if (predictionError) {
        predictionError->computeError(predictionError_, predictionError_);
    }
}

void AgentBrain::integrateCuriosityDrivenLearning() {
    if (!brain_) return;
    
    auto* curiosity = getCuriosity();
    auto* novelty = getNovelty();
    auto* episodicMemory = getEpisodicMemory();
    
    if (curiosity && curiosityLevel_ > 0.3f) {
        // Apply curiosity-based exploration modulation
        // Get random generator from brain
        auto* rng = brain_->getRandomGenerator();
        if (rng) {
            // Higher curiosity increases exploration
            float exploreChance = curiosityLevel_ * 0.3f;
            float r = rng->uniformReal(0.0f, 1.0f);
            
            if (r < exploreChance) {
                // Trigger exploration behavior
                if (episodicMemory) {
                    // Rehearse or retrieve similar episodes to encourage novel behavior
                    auto recentEpisodes = episodicMemory->getRecentEpisodes(3);
                    if (!recentEpisodes.empty()) {
                        episodicMemory->replayEpisode(recentEpisodes[0]);
                    }
                }
            }
        }
    }
}

void AgentBrain::integrateMemoryReplay() {
    if (!brain_) return;
    
    auto* episodicMemory = getEpisodicMemory();
    if (episodicMemory && episodicMemory->isReplayEnabled()) {
        // Select episodes for replay based on relevance
        auto replayEpisodes = episodicMemory->getEpisodesForReplay(3);
        
        // Replay the selected episodes to consolidate memory
        for (const auto* episode : replayEpisodes) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

} // namespace nlm

// Advanced Curiosity and Memory Features Implementation

namespace nlm {

// Forward declarations for helper functions
float AgentBrain::calculatePatternStrength(const EpisodicMemoryItem& episode) const {
    if (episode.sensoryState.empty()) return 0.0f;
    
    // Calculate pattern strength based on coherence and reward
    float coherence = 1.0f;
    if (episode.sensoryState.size() > 1) {
        float avgVal = 0.0f;
        for (float val : episode.sensoryState) avgVal += val;
        avgVal /= episode.sensoryState.size();
        
        float variance = 0.0f;
        for (float val : episode.sensoryState) {
            variance += (val - avgVal) * (val - avgVal);
        }
        variance /= episode.sensoryState.size();
        
        // Lower variance = higher coherence
        coherence = std::max(0.0f, 1.0f - variance);
    }
    
    return coherence * (episode.reward + 0.1f);
}

std::vector<EpisodicMemoryItem> AgentBrain::getAllEpisodes() const {
    std::vector<EpisodicMemoryItem> allEpisodes;
    
    auto* episodicMemory = getEpisodicMemory();
    if (!episodicMemory) return allEpisodes;
    
    // Get episodes from all sources
    allEpisodes = episodicMemory->getRecentEpisodes(100); // Get up to 100 recent episodes
    
    return allEpisodes;
}

std::vector<EpisodicMemoryItem> AgentBrain::findSimilarEpisodes(const std::vector<float>& pattern, size_t maxCount) const {
    std::vector<EpisodicMemoryItem> similarEpisodes;
    auto allEpisodes = getAllEpisodes();
    
    if (allEpisodes.empty()) return similarEpisodes;
    
    std::vector<std::pair<float, EpisodicMemoryItem>> episodeSimilarities;
    for (const auto& episode : allEpisodes) {
        float similarity = 0.0f;
        size_t compareLen = std::min(pattern.size(), episode.sensoryState.size());
        
        for (size_t i = 0; i < compareLen; ++i) {
            similarity += 1.0f - std::abs(pattern[i] - episode.sensoryState[i]);
        }
        similarity /= std::max<size_t>(compareLen, 1);
        
        episodeSimilarities.push_back({similarity, episode});
    }
    
    std::sort(episodeSimilarities.begin(), episodeSimilarities.end(),
              [](const std::pair<float, EpisodicMemoryItem>& a,
                 const std::pair<float, EpisodicMemoryItem>& b) {
                  return a.first > b.first;
              });
    
    size_t count = std::min(maxCount, episodeSimilarities.size());
    for (size_t i = 0; i < count; ++i) {
        similarEpisodes.push_back(episodeSimilarities[i].second);
    }
    
    return similarEpisodes;
}

float AgentBrain::calculateReplayPriority(const EpisodicMemoryItem& episode) const {
    float priority = 0.0f;
    
    // Based on reward
    priority += episode.reward * 0.4f;
    
    // Based on novelty
    if (!episode.sensoryState.empty()) {
        float novelty = 0.0f;
        for (float val : episode.sensoryState) {
            novelty += val;
        }
        novelty /= episode.sensoryState.size();
        priority += novelty * 0.3f;
    }
    
    // Based on prediction error (if available)
    priority += std::abs(episode.reward - expectedReward_) * 0.3f;
    
    return priority;
}

float AgentBrain::calculateEpisodeStrength(const EpisodicMemoryItem& episode) const {
    float strength = 0.0f;
    
    // Based on reward
    strength += episode.reward * 0.5f;
    
    // Based on coherence (lower variance = higher strength)
    if (!episode.sensoryState.empty()) {
        float avg = 0.0f;
        for (float val : episode.sensoryState) avg += val;
        avg /= episode.sensoryState.size();
        
        float variance = 0.0f;
        for (float val : episode.sensoryState) {
            variance += (val - avg) * (val - avg);
        }
        variance /= episode.sensoryState.size();
        
        // Lower variance = higher coherence = higher strength
        strength += (1.0f - variance) * 0.5f;
    }
    
    return std::clamp(strength, 0.0f, 1.0f);
}

void AgentBrain::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!brain_ || !episode) return;
    
    // Replay an episode to strengthen learning and consolidate memory
    for (size_t i = 0; i < episode->sensoryState.size() && i < 10; ++i) {
        float current = episode->sensoryState[i] * 5.0f;
        injectCurrent(static_cast<NeuronId>(i), current);
    }
    
    // Apply episode reward to neuromodulation
    applyRewardModulation(episode->reward, expectedReward_);
    
    // Update learning success for meta-learning
    float learningSuccess = calculateLearningSuccess(episode);
    updateMetaLearningPlastictyModulation(learningSuccess);
}

float AgentBrain::calculateLearningSuccess(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0.0f;
    
    float success = episode->reward;
    
    // If reward is high but prediction error is low, learning was successful
    if (episode->reward > expectedReward_ && std::abs(predictionError_) < 0.1f) {
        success *= 1.5f;
    }
    
    return std::clamp(success, 0.0f, 1.0f);
}

void AgentBrain::completePattern(const EpisodicMemoryItem& episode, NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    for (size_t i = 0; i < episode.sensoryState.size() && i < 10; ++i) {
        float patternValue = episode.sensoryState[i];
        workingMemory->store(static_cast<NeuronId>(i), patternValue);
    }
}

void AgentBrain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (brain_) {
        brain_->injectCurrent(neuron, current);
    }
}

} // namespace nlm