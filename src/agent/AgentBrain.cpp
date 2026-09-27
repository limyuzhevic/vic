#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , norepinephrineLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , attentionEnhancement_(1.0f)
    , workingMemoryEnhancement_(1.0f)
    , learningEnhancement_(1.0f)
    , decisionStability_(1.0f)
    , explorationDrive_(0.5f)
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
    
    // Initialize enhanced neuromodulators
    norepinephrineLevel_ = 0.0f;
    acetylcholineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    attentionEnhancement_ = 1.0f;
    workingMemoryEnhancement_ = 1.0f;
    learningEnhancement_ = 1.0f;
    decisionStability_ = 1.0f;
    explorationDrive_ = 0.5f;
    
    NLM_LOG_INFO("AgentBrain initialized with enhanced neuromodulation");
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update enhanced neuromodulators based on current state
    updateEnhancedNeuromodulators(vision, touch, intern, vision.size(), dt);
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
    
    // Update enhanced neuromodulators based on current state
    updateEnhancedNeuromodulators(vision, touch, intern, vision.size(), dt);
}

void AgentBrain::updateEnhancedNeuromodulators(const std::vector<float>& vision,
                                            const std::vector<float>& touch,
                                            const std::vector<float>& intern,
                                            size_t visionSize,
                                            double dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update Norepinephrine based on novelty and prediction error
    float noveltySensitivity = 0.7f;
    float predictionErrorSensitivity = 0.3f;
    
    // NE responds to novel and surprising events (arousal)
    float noveltyResponse = noveltyLevel_ * noveltySensitivity;
    float predictionErrorResponse = std::abs(predictionError_) * predictionErrorSensitivity;
    
    norepinephrineLevel_ = std::min(1.0f, norepinephrineLevel_ + 
                                   (std::max(noveltyResponse, predictionErrorResponse) - norepinephrineLevel_) * 0.1f * dtFloat);
    
    // NE also responds to sustained attention (focus)
    if (visionSize > 0) {
        float avgVision = 0.0f;
        for (float v : vision) avgVision += v;
        avgVision /= visionSize;
        
        if (avgVision > 0.5f) {
            norepinephrineLevel_ = std::min(1.0f, norepinephrineLevel_ + 0.05f * dtFloat);
        }
    }
    
    // Update Acetylcholine based on working memory load and novelty
    float workingMemoryLoad = std::min(1.0f, static_cast<float>(vision.size()) / 100.0f);
    float achMemoryResponse = workingMemoryLoad * 0.4f;
    float achNoveltyResponse = noveltyLevel_ * 0.3f;
    
    acetylcholineLevel_ = std::min(1.0f, acetylcholineLevel_ + 
                                  (achMemoryResponse + achNoveltyResponse - acetylcholineLevel_) * 0.08f * dtFloat);
    
    // Update Serotonin based on prediction error and reward signals
    float serotoninPredictionErrorResponse = std::abs(predictionError_) * 0.5f;
    float serotoninRewardResponse = (predictionError_ > 0.0f) ? predictionError_ * 0.3f : 0.0f;
    
    serotoninLevel_ = std::max(0.0f, serotoninLevel_ + 
                              (serotoninPredictionErrorResponse + serotoninRewardResponse - serotoninLevel_) * 0.06f * dtFloat);
    
    // Compute integrated neuromodulatory effects
    attentionEnhancement_ = (1.0f + norepinephrineLevel_ * 1.5f) * (1.0f + acetylcholineLevel_ * 0.7f);
    workingMemoryEnhancement_ = (1.0f + acetylcholineLevel_ * 1.5f);
    learningEnhancement_ = (1.0f + serotoninLevel_ * 1.2f);
    decisionStability_ = (1.0f + serotoninLevel_ * 0.8f);
    explorationDrive_ = norepinephrineLevel_ * 0.6f + curiosityLevel_ * 0.4f;
    
    // Log significant neuromodulatory changes
    if (norepinephrineLevel_ > 0.3f || acetylcholineLevel_ > 0.3f || serotoninLevel_ > 0.3f) {
        NLM_LOG_INFO("Enhanced neuromodulation: NE=" + std::to_string(norepinephrineLevel_) + 
                     ", ACh=" + std::to_string(acetylcholineLevel_) + 
                     ", 5-HT=" + std::to_string(serotoninLevel_));
    }
}
    
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
// Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update enhanced neuromodulators based on current state
    updateEnhancedNeuromodulators(vision, touch, intern, vision.size(), dt);
}

// ========== PERSISTENCE SYSTEM ==========

void AgentBrain::enablePersistence(const std::string& directory) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot enable persistence: Brain not available");
        return;
    }
    
    // Get persistence manager from brain
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot enable persistence: Persistence manager not available");
        return;
    }
    
    // Set the persistence directory
    persistenceManager->setBaseDirectory(directory);
    NLM_LOG_INFO("Persistence enabled with directory: " + directory);
}

bool AgentBrain::saveBrainState(const std::string& filename) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot save brain state: Brain not available");
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot save brain state: Persistence manager not available");
        return false;
    }
    
    // Save in binary format for efficiency
    bool success = persistenceManager->saveBrainState(filename, true,
                                                      CheckpointSystem::CompressionLevel::Balanced,
                                                      true);
    
    if (success) {
        NLM_LOG_INFO("Brain state saved: " + filename);
    }
    
    return success;
}

bool AgentBrain::loadBrainState(const std::string& filename) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot load brain state: Brain not available");
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot load brain state: Persistence manager not available");
        return false;
    }
    
    // Load from binary format
    bool success = persistenceManager->loadBrainState(filename, true);
    
    if (success) {
        NLM_LOG_INFO("Brain state loaded: " + filename);
    }
    
    return success;
}

bool AgentBrain::isPersistentStateAvailable(const std::string& filename) const {
    if (!brain_) {
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        return false;
    }
    
    return persistenceManager->isPersistentStateAvailable(filename);
}

PersistentBrainStateInfo AgentBrain::getPersistentStateInfo(const std::string& filename) const {
    PersistentBrainStateInfo info;
    
    if (!brain_) {
        return info;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        return info;
    }
    
    return persistenceManager->getPersistentStateInfo(filename, true);
}

std::string AgentBrain::createBackup() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot create backup: Brain not available");
        return "";
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot create backup: Persistence manager not available");
        return "";
    }
    
    // Create backup with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "backup_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".bml";
    std::string backupName = ss.str();
    
    if (persistenceManager->createBackup(backupName)) {
        NLM_LOG_INFO("Backup created: " + backupName);
        return backupName;
    }
    
    return "";
}

bool AgentBrain::restoreFromBackup(const std::string& backupName) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot restore from backup: Brain not available");
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot restore from backup: Persistence manager not available");
        return false;
    }
    
    bool success = persistenceManager->restoreFromBackup(backupName);
    
    if (success) {
        NLM_LOG_INFO("Restored from backup: " + backupName);
    }
    
    return success;
}

std::vector<std::string> AgentBrain::listBrainStates() const {
    if (!brain_) {
        return std::vector<std::string>();
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        return std::vector<std::string>();
    }
    
    return persistenceManager->listBrainStates();
}

bool AgentBrain::isPersistenceAvailable() const {
    if (!brain_) {
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    return persistenceManager != nullptr;
}

std::vector<CheckpointManager::CheckpointInfo> AgentBrain::listCheckpoints() const {
    std::vector<CheckpointManager::CheckpointInfo> checkpoints;
    
    if (!brain_) {
        return checkpoints;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        return checkpoints;
    }
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    if (!checkpointManager) {
        return checkpoints;
    }
    
    return checkpointManager->listCheckpoints();
}

bool AgentBrain::saveCheckpoint(const std::string& name) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot save checkpoint: Brain not available");
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot save checkpoint: Persistence manager not available");
        return false;
    }
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    if (!checkpointManager) {
        NLM_LOG_ERROR("Cannot save checkpoint: Checkpoint manager not available");
        return false;
    }
    
    bool success = checkpointManager->saveImmediately(name);
    
    if (success) {
        NLM_LOG_INFO("Checkpoint saved: " + (name.empty() ? "default" : name));
    }
    
    return success;
}

bool AgentBrain::loadCheckpoint(const std::string& name) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot load checkpoint: Brain not available");
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot load checkpoint: Persistence manager not available");
        return false;
    }
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    if (!checkpointManager) {
        NLM_LOG_ERROR("Cannot load checkpoint: Checkpoint manager not available");
        return false;
    }
    
    bool success = checkpointManager->load(name);
    
    if (success) {
        NLM_LOG_INFO("Checkpoint loaded: " + (name.empty() ? "default" : name));
    }
    
    return success;
}

void AgentBrain::pruneOldCheckpoints(size_t keepCount) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot prune checkpoints: Brain not available");
        return;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        NLM_LOG_ERROR("Cannot prune checkpoints: Persistence manager not available");
        return;
    }
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    if (!checkpointManager) {
        NLM_LOG_ERROR("Cannot prune checkpoints: Checkpoint manager not available");
        return;
    }
    
    checkpointManager->pruneOldCheckpoints(keepCount);
    NLM_LOG_INFO("Pruned checkpoints, keeping: " + std::to_string(keepCount));
}

bool AgentBrain::shouldSaveCheckpoint() const {
    if (!brain_) {
        return false;
    }
    
    auto* persistenceManager = brain_->getPersistenceManager();
    if (!persistenceManager) {
        return false;
    }
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    if (!checkpointManager) {
        return false;
    }
    
    uint64_t currentStep = brain_->getConfig()->getOr<uint64_t>("current_step", 0);
    double currentTime = 0.0; // TODO: Get actual simulation time
    
    return checkpointManager->shouldSave(currentStep);
}

} // namespace nlm
