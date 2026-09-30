#include "SleepState.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

SleepStateManager::SleepStateManager()
    : currentState_(SleepState::Wake)
    , targetState_(SleepState::Wake)
    , transitionProgress_(0.0f)
    , transitionTimer_(0.0f)
    , timeInCurrentState_(0.0f)
    , totalSleepTime_(0.0f)
    , circadianPhase_(0.0f)
    , circadianPeriod_(24.0f * 60.0f * 60.0f)  // 24 hours in seconds
    , wakeTime_(0.0f)
    , sleepPressure_(0.0f)
    , sleepDebt_(0.0f)
    , wakeDuration_(0.0f)
    , transitionSpeed_(1.0f)
    , minimumStateDuration_(10.0f)  // 10 seconds minimum per state
    , timeSinceWake_(0.0f)
    , sleepEfficiency_(0.0f)
    , memoryReplayCount_(0)
    , lastReplayTime_(0.0f)
    , quietSleepTimer_(0.0f)
    , deepSleepTimer_(0.0f)
    , remSleepTimer_(0.0f)
    , brain_(nullptr)
    , neuromodulationIntegrated_(false)
    , memoryReplayActive_(false) {
    
    // Initialize state characteristics for each sleep state
    SleepStateCharacteristics quiet;
    quiet.replayFrequency = 0.5f;          // Moderate replay during quiet sleep
    quiet.consolidationIntensity = 0.3f;   // Initial consolidation
    quiet.synapticDownscaling = 0.2f;
    quiet.reorganizationStrength = 0.4f;
    quiet.memoryIntegration = 0.3f;
    quiet.emotionalProcessing = 0.1f;
    quiet.neuralExcitability = 1.2f;
    quiet.oscillationFrequency = 8.0f;    // Delta wave range (8-12 Hz)
    quiet.requiresAttention = false;
    quiet.requiresDopamine = false;
    quiet.requiresAcetylcholine = true;
    quiet.typicalDuration = 30.0f;         // 30 seconds typical
    quiet.transitionInTime = 2.0f;
    quiet.transitionOutTime = 2.0f;
    stateCharacteristics_[SleepState::QuietSleep] = quiet;
    
    SleepStateCharacteristics deep;
    deep.replayFrequency = 0.3f;           // Lower replay, more focus on downscaling
    deep.consolidationIntensity = 0.8f;    // High consolidation
    deep.synapticDownscaling = 0.8f;
    deep.reorganizationStrength = 0.9f;
    deep.memoryIntegration = 0.6f;
    deep.emotionalProcessing = 0.2f;
    deep.neuralExcitability = 1.0f;
    deep.oscillationFrequency = 0.5f;     // Slow wave sleep (0.5-4 Hz)
    deep.requiresAttention = false;
    deep.requiresDopamine = true;
    deep.requiresAcetylcholine = false;
    deep.typicalDuration = 60.0f;          // 60 seconds typical
    deep.transitionInTime = 3.0f;
    deep.transitionOutTime = 3.0f;
    stateCharacteristics_[SleepState::DeepSleep] = deep;
    
    SleepStateCharacteristics rem;
    rem.replayFrequency = 0.8f;            // High replay during REM sleep
    rem.consolidationIntensity = 0.6f;     // Integration focus
    rem.synapticDownscaling = 0.3f;
    rem.reorganizationStrength = 0.7f;
    rem.memoryIntegration = 0.9f;
    rem.emotionalProcessing = 0.8f;
    rem.neuralExcitability = 1.5f;
    rem.oscillationFrequency = 4.0f;      // Theta waves (4-8 Hz)
    rem.requiresAttention = true;
    rem.requiresDopamine = true;
    rem.requiresAcetylcholine = true;
    rem.typicalDuration = 45.0f;           // 45 seconds typical
    rem.transitionInTime = 2.0f;
    rem.transitionOutTime = 2.0f;
    stateCharacteristics_[SleepState::REMSleep] = rem;
    
    SleepStateCharacteristics wake;
    wake.replayFrequency = 0.0f;            // No replay during wake
    wake.consolidationIntensity = 0.0f;
    wake.synapticDownscaling = 0.0f;
    wake.reorganizationStrength = 0.3f;
    wake.memoryIntegration = 0.8f;
    wake.emotionalProcessing = 0.5f;
    wake.neuralExcitability = 2.0f;
    wake.oscillationFrequency = 10.0f;    // Beta waves (10-30 Hz)
    wake.requiresAttention = true;
    wake.requiresDopamine = true;
    wake.requiresAcetylcholine = true;
    wake.typicalDuration = 0.0f;           // No minimum during wake
    wake.transitionInTime = 1.0f;
    wake.transitionOutTime = 1.0f;
    stateCharacteristics_[SleepState::Wake] = wake;
    
    NLM_LOG_INFO("SleepStateManager initialized");
}

SleepStateManager::~SleepStateManager() {
    NLM_LOG_INFO("SleepStateManager destroyed");
}

void SleepStateManager::initialize(Brain* brain) {
    brain_ = brain;
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize SleepStateManager: brain is null");
        return;
    }
    
    // Start in Wake state
    transitionToState(SleepState::Wake);
    
    NLM_LOG_INFO("SleepStateManager initialized with brain reference");
}

void SleepStateManager::update(float dt, float currentWakeTime) {
    if (!brain_) return;
    
    // Update wake time and circadian phase
    wakeTime_ = currentWakeTime;
    updateCircadianPhase(dt);
    
    // Update sleep pressure and debt
    updateSleepPressure(dt);
    
    // Update state transition timer
    if (transitionTimer_ > 0.0f) {
        transitionTimer_ -= dt * transitionSpeed_;
        transitionProgress_ = std::max(0.0f, 1.0f - (transitionTimer_ / getStateCharacteristics(targetState_).transitionInTime));
    }
    
    // Check if we should transition to target state
    if (transitionTimer_ <= 0.0f && transitionProgress_ >= 1.0f) {
        SleepState oldState = currentState_;
        currentState_ = targetState_;
        transitionProgress_ = 0.0f;
        targetState_ = SleepState::Wake;  // Reset until explicitly set
        
        onStateEnter(currentState_);
        onStateExit(oldState);
        
        timeInCurrentState_ = 0.0f;
        
        // Reset state-specific timers
        switch (currentState_) {
            case SleepState::QuietSleep:
                quietSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::DeepSleep:
                deepSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::REMSleep:
                remSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::Wake:
                wakeDuration_ = 0.0f;  // Reset wake timer
                break;
        }
    }
    
    // Update time in current state
    timeInCurrentState_ += dt;
    
    // Update state-specific timers
    if (isSleeping()) {
        switch (currentState_) {
            case SleepState::QuietSleep:
                quietSleepTimer_ = std::max(0.0f, quietSleepTimer_ - dt);
                break;
            case SleepState::DeepSleep:
                deepSleepTimer_ = std::max(0.0f, deepSleepTimer_ - dt);
                break;
            case SleepState::REMSleep:
                remSleepTimer_ = std::max(0.0f, remSleepTimer_ - dt);
                break;
            case SleepState::Wake:
                wakeDuration_ += dt;
                timeSinceWake_ += dt;
                break;
        }
    }
    
    // Update sleep efficiency
    updateSleepEfficiency();
    
    // Process current state
    if (currentState_ == SleepState::QuietSleep) processQuietSleep();
    else if (currentState_ == SleepState::DeepSleep) processDeepSleep();
    else if (currentState_ == SleepState::REMSleep) processREMSleep();
    else if (currentState_ == SleepState::Wake) processWake();
    
    // Integrate with other systems
    integrateWithNeuromodulation();
    integrateWithMemoryReplay();
    integrateWithWorkingMemory();
    
    // Check if we should transition based on timers and homeostatic pressure
    updateStateTransitions(dt);
}

void SleepStateManager::transitionToState(SleepState newState, float transitionDuration) {
    if (newState == currentState_) return;  // Already in this state
    
    // Ensure valid state transitions
    bool validTransition = false;
    
    if (currentState_ == SleepState::Wake && (newState == SleepState::QuietSleep || newState == SleepState::DeepSleep)) {
        validTransition = true;  // Can sleep from wake
    } else if (currentState_ == SleepState::QuietSleep && newState == SleepState::DeepSleep) {
        validTransition = true;  // Quiet -> Deep progression
    } else if (currentState_ == SleepState::DeepSleep && newState == SleepState::REMSleep) {
        validTransition = true;  // Deep -> REM progression
    } else if (currentState_ == SleepState::REMSleep && newState == SleepState::QuietSleep) {
        validTransition = true;  // REM -> back to quiet
    } else if (currentState_ == SleepState::QuietSleep && newState == SleepState::Wake) {
        validTransition = true;  // Can wake from sleep
    }
    
    if (!validTransition) {
        NLM_LOG_WARNING(std::string("Invalid sleep state transition from ") + 
                       std::to_string(static_cast<int>(currentState_)) + 
                       std::string(" to ") + std::to_string(static_cast<int>(newState)));
        return;
    }
    
    targetState_ = newState;
    transitionTimer_ = transitionDuration;
    transitionProgress_ = 0.0f;
    
    NLM_LOG_INFO(std::string("Sleep state transition: ") + 
                std::to_string(static_cast<int>(currentState_)) + 
                std::string(" -> ") + std::to_string(static_cast<int>(targetState_)));
}

void SleepStateManager::addSleepPressure(float pressure) {
    sleepPressure_ = std::min(sleepPressure_ + pressure, 10.0f);  // Cap at 10
    NLM_LOG_DEBUG(std::string("Sleep pressure increased to ") + std::to_string(sleepPressure_));
}

void SleepStateManager::releaseSleepPressure(float pressure) {
    sleepPressure_ = std::max(0.0f, sleepPressure_ - pressure);
    NLM_LOG_DEBUG(std::string("Sleep pressure decreased to ") + std::to_string(sleepPressure_));
}

void SleepStateManager::addSleepDebt(float debt) {
    sleepDebt_ = std::min(sleepDebt_ + debt, 100.0f);  // Cap at 100
}

void SleepStateManager::reduceSleepDebt(float debt) {
    sleepDebt_ = std::max(0.0f, sleepDebt_ - debt);
}

void SleepStateManager::integrateWithNeuromodulation() {
    if (!neuromodulationIntegrated_) {
        // Setup neuromodulation for current state
        const auto& characteristics = getStateCharacteristics(currentState_);
        
        if (brain_) {
            if (characteristics.requiresDopamine) {
                if (auto* dopamine = brain_->getDopamine()) {
                    dopamine->setLevel(0.8f);  // High dopamine for sleep states (except wake)
                }
            } else {
                if (auto* dopamine = brain_->getDopamine()) {
                    dopamine->setLevel(0.1f);  // Low baseline during wake
                }
            }
            
            NLM_LOG_DEBUG(std::string("Neuromodulation integrated for ") + 
                         std::to_string(static_cast<int>(currentState_)));
        }
        
        neuromodulationIntegrated_ = true;
    }
}

void SleepStateManager::integrateWithMemoryReplay() {
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    if (characteristics.replayFrequency > 0.0f && brain_) {
        float timeSinceLastReplay = brain_->getConfig() ? 
            brain_->getConfig()->getOr<double>("simulation_time", 0.0) - lastReplayTime_ : 0.0f;
        
        if (timeSinceLastReplay > (1.0f / characteristics.replayFrequency)) {
            executeMemoryReplay();
            memoryReplayCount_++;
            lastReplayTime_ = timeSinceLastReplay;
            memoryReplayActive_ = true;
            
            NLM_LOG_DEBUG(std::string("Memory replay executed for ") + 
                         std::to_string(static_cast<int>(currentState_)));
        }
    } else {
        memoryReplayActive_ = false;
    }
}

void SleepStateManager::integrateWithWorkingMemory() {
    if (brain_ && brain_->getWorkingMemory()) {
        auto& workingMemory = *brain_->getWorkingMemory();
        
        switch (currentState_) {
            case SleepState::QuietSleep:
                // Mild decay of working memory
                workingMemory.setDecayRate(0.05f);
                break;
            case SleepState::DeepSleep:
                // Stronger decay to clear weak traces
                workingMemory.setDecayRate(0.15f);
                break;
            case SleepState::REMSleep:
                // Moderate decay with potential integration
                workingMemory.setDecayRate(0.08f);
                break;
            case SleepState::Wake:
                // Minimal decay during wake
                workingMemory.setDecayRate(0.01f);
                break;
        }
    }
}

void SleepStateManager::processQuietSleep() {
    // Light sleep: restoration and initial consolidation
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    // Neural maintenance and energy restoration
    if (brain_) {
        // Inject mild current to promote slow oscillation synchronization
        brain_->injectCurrentToNeurons(NeuronType::Internal, characteristics.neuralExcitability * 0.5f);
        
        // Encourage theta wave generation
        if (timeInCurrentState_ > quietSleepTimer_ * 0.3f) {
            // Second third of quiet sleep - more active replay
            scheduleMemoryReplay();
        }
    }
    
    // Sleep debt reduction during quiet sleep
    reduceSleepDebt(0.5f);
    
    NLM_LOG_DEBUG(std::string("Processing QuietSleep: ") + 
                 std::to_string(quietSleepTimer_) + std::string(" remaining"));
}

void SleepStateManager::processDeepSleep() {
    // Deep sleep: synaptic downscaling and system consolidation
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    // Strong synaptic downscaling
    if (brain_) {
        // Inject moderate current to promote slow wave synchronization
        brain_->injectCurrentToNeurons(NeuronType::Internal, characteristics.neuralExcitability * 0.3f);
        
        // Reorganize memory connections
        if (brain_->getAssociativeMemory()) {
            auto& associativeMem = *brain_->getAssociativeMemory();
            // Weaker associations may be pruned during deep sleep
            if (timeInCurrentState_ > deepSleepTimer_ * 0.5f) {
                scheduleMemoryReplay();
            }
        }
    }
    
    // Significant sleep debt reduction during deep sleep
    reduceSleepDebt(1.0f);
    
    NLM_LOG_DEBUG(std::string("Processing DeepSleep: ") + 
                 std::to_string(deepSleepTimer_) + std::string(" remaining"));
}

void SleepStateManager::processREMSleep() {
    // REM sleep: memory integration and emotional processing
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    // Integration of memory networks
    if (brain_) {
        // Higher excitability for memory reorganization
        brain_->injectCurrentToNeurons(NeuronType::Internal, characteristics.neuralExcitability * 0.8f);
        
        // Integrate episodic memories
        if (brain_->getEpisodicMemory() && characteristics.memoryIntegration > 0.5f) {
            scheduleMemoryReplay();
        }
        
        // Emotional processing support
        if (auto* novelty = brain_->getNovelty()) {
            novelty->setLevel(0.6f);  // Moderate novelty during REM for emotional processing
        }
    }
    
    // Moderate sleep debt reduction during REM sleep
    reduceSleepDebt(0.7f);
    
    NLM_LOG_DEBUG(std::string("Processing REMSleep: ") + 
                 std::to_string(remSleepTimer_) + std::string(" remaining"));
}

void SleepStateManager::processWake() {
    // Wake state: normal learning and activity
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    // Boost neural activity for cognition
    if (brain_) {
        brain_->injectCurrentToNeurons(NeuronType::Internal, characteristics.neuralExcitability * 1.5f);
        
        // Clear excess sleep pressure through wake maintenance
        if (sleepPressure_ > 5.0f) {
            releaseSleepPressure(0.5f * timeInCurrentState_);
        }
    }
    
    // Wake maintenance adds to wake duration
    if (brain_ && timeInCurrentState_ > 1.0f) {
        wakeDuration_ += timeInCurrentState_;
        timeSinceWake_ += timeInCurrentState_;
    }
    
    NLM_LOG_DEBUG(std::string("Processing Wake: ") + 
                 std::to_string(wakeDuration_) + std::string(" total wake time"));
}

void SleepStateManager::scheduleMemoryReplay() {
    if (!brain_) return;
    
    const auto& characteristics = getStateCharacteristics(currentState_);
    
    // Select appropriate episodes based on current sleep state
    size_t replayCount = static_cast<size_t>(characteristics.replayFrequency);
    
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Request episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(std::min(replayCount, 3));
        
        if (!episodesToReplay.empty()) {
            std::vector<size_t> episodeIds;
            for (size_t i = 0; i < std::min(episodesToReplay.size(), episodesToReplay.size()); ++i) {
                // Get index of episode in memory
                for (size_t j = 0; j < episodicMemory->getEpisodeCount(); ++j) {
                    if (episodesToReplay[i] == episodicMemory->getEpisode(j)) {
                        episodeIds.push_back(j);
                        break;
                    }
                }
            }
            
            if (!episodeIds.empty()) {
                episodicMemory->replaySequence(episodeIds);
            }
        }
    }
}

void SleepStateManager::executeMemoryReplay() {
    if (!brain_) return;
    
    // This method is called when scheduled memory replay should be executed
    // The actual replay is handled by individual memory systems (episodic memory)
    memoryReplayActive_ = true;
    
    NLM_LOG_DEBUG("Memory replay executed");
}

void SleepStateManager::updateStateTransitions(float dt) {
    bool shouldTransition = false;
    
    // Check minimum duration requirement
    if (timeInCurrentState_ < minimumStateDuration_) {
        return;  // Not enough time in current state
    }
    
    // Check if state-specific timer has expired
    if (isSleeping()) {
        switch (currentState_) {
            case SleepState::QuietSleep:
                if (quietSleepTimer_ <= 0.0f) shouldTransition = true;
                break;
            case SleepState::DeepSleep:
                if (deepSleepTimer_ <= 0.0f) shouldTransition = true;
                break;
            case SleepState::REMSleep:
                if (remSleepTimer_ <= 0.0f) shouldTransition = true;
                break;
            case SleepState::Wake:
                // Check if it's time to sleep based on circadian rhythm and pressure
                float circadianSleepProb = getCircadianSleepProbability(wakeTime_);
                float homeostaticSleepNeed = getHomeostaticSleepPressure();
                
                if (homeostaticSleepNeed > 5.0f && 
                    (rand() / (float)RAND_MAX) < (circadianSleepProb * homeostaticSleepNeed / 10.0f)) {
                    shouldTransition = true;
                }
                break;
        }
    }
    
    // Auto-transition to next state in cycle
    if (shouldTransition) {
        SleepState nextState = getNextState();
        if (nextState != currentState_) {
            // Ensure minimum duration before sleeping
            if (nextState != SleepState::Wake) {
                if (timeSinceWake_ < minimumStateDuration_ * 2.0f) {
                    return;  // Need to stay awake longer
                }
            }
            
            transitionToState(nextState);
        }
    }
}

SleepState SleepStateManager::getNextState() const {
    switch (currentState_) {
        case SleepState::Wake:
            return SleepState::QuietSleep;  // Start with light sleep
        case SleepState::QuietSleep:
            return SleepState::DeepSleep;   // Progress to deep sleep
        case SleepState::DeepSleep:
            return SleepState::REMSleep;    // Progress to REM sleep
        case SleepState::REMSleep:
            return SleepState::QuietSleep;  // Cycle back to quiet sleep
        default:
            return SleepState::Wake;        // Default to wake
    }
}

void SleepStateManager::updateCircadianPhase(float dt) {
    // Simple circadian rhythm simulation
    // More sophisticated implementation would use actual circadian biology
    circadianPhase_ += 2.0f * M_PI * dt / circadianPeriod_;
    
    if (circadianPhase_ > 2.0f * M_PI) {
        circadianPhase_ -= 2.0f * M_PI;
    }
}

float SleepStateManager::getCircadianSleepProbability(float time) const {
    // Simplified circadian sleep probability based on time of day
    // Peak sleep probability around midnight (hours 0-4)
    float hours = fmod(time / 3600.0f, 24.0f);
    
    float sleepProbability = 0.0f;
    if (hours >= 22.0f || hours <= 4.0f) {
        // High probability during night hours
        float hourDiff = hours <= 4.0f ? (hours + 24.0f - 22.0f) : (hours - 22.0f);
        sleepProbability = 0.8f * std::exp(-hourDiff * hourDiff / 8.0f);
    } else {
        // Low probability during day hours
        float hourDiff = std::abs(hours - 14.0f);
        sleepProbability = 0.1f * std::exp(-hourDiff * hourDiff / 16.0f);
    }
    
    return sleepProbability;
}

float SleepStateManager::getHomeostaticSleepPressure() const {
    // Sleep pressure based on wake duration
    // Higher wake duration = higher pressure
    float wakePressure = std::min(wakeDuration_ / 3600.0f, 10.0f);  // 1 hour wake = pressure 1
    return wakePressure + sleepPressure_;
}

void SleepStateManager::updateSleepPressure(float dt) {
    if (currentState_ == SleepState::Wake) {
        // Wake: pressure builds up
        sleepPressure_ += dt * 0.1f;
        sleepPressure_ = std::min(sleepPressure_, 10.0f);
    } else {
        // Sleep: pressure is dissipated
        sleepPressure_ -= dt * 0.3f;
        sleepPressure_ = std::max(0.0f, sleepPressure_);
    }
}

void SleepStateManager::updateSleepEfficiency() {
    if (!brain_ || brain_->getTotalSpikeCount() == 0) return;
    
    float spikesPerStep = static_cast<float>(brain_->getTotalSpikeCount()) / 
                         (brain_->getConfig() ? 
                          brain_->getConfig()->getOr<size_t>("simulation_steps", 100) : 100);
    
    // Efficiency based on spike regularity and memory consolidation
    sleepEfficiency_ = 0.5f;  // Base efficiency
    
    if (currentState_ == SleepState::DeepSleep) {
        // Deep sleep tends to be more efficient
        sleepEfficiency_ = 0.8f;
    } else if (currentState_ == SleepState::REMSleep) {
        // REM sleep moderate efficiency
        sleepEfficiency_ = 0.6f;
    }
    
    // Adjust based on spike activity
    if (spikesPerStep > 10.0f) {
        sleepEfficiency_ *= 1.1f;  // High activity can indicate efficient processing
    }
    
    // Cap efficiency
    sleepEfficiency_ = std::min(sleepEfficiency_, 1.0f);
}

void SleepStateManager::onStateEnter(SleepState newState) {
    NLM_LOG_INFO(std::string("SleepStateManager entering state: ") + 
                std::to_string(static_cast<int>(newState)));
    
    // Reset integration flag
    neuromodulationIntegrated_ = false;
}

void SleepStateManager::onStateExit(SleepState oldState) {
    NLM_LOG_INFO(std::string("SleepStateManager exiting state: ") + 
                std::to_string(static_cast<int>(oldState)));
}

const SleepStateCharacteristics& SleepStateManager::getStateCharacteristics(SleepState state) const {
    auto it = stateCharacteristics_.find(state);
    if (it != stateCharacteristics_.end()) {
        return it->second;
    }
    
    // Return wake state characteristics if state not found (should not happen)
    static SleepStateCharacteristics defaultChar;
    return defaultChar;
}

std::string SleepStateManager::serialize() const {
    std::ostringstream oss;
    oss << "currentState:" << static_cast<int>(currentState_) << "\n";
    oss << "targetState:" << static_cast<int>(targetState_) << "\n";
    oss << "transitionProgress:" << transitionProgress_ << "\n";
    oss << "transitionTimer:" << transitionTimer_ << "\n";
    oss << "timeInCurrentState:" << timeInCurrentState_ << "\n";
    oss << "totalSleepTime:" << totalSleepTime_ << "\n";
    oss << "circadianPhase:" << circadianPhase_ << "\n";
    oss << "circadianPeriod:" << circadianPeriod_ << "\n";
    oss << "wakeTime:" << wakeTime_ << "\n";
    oss << "sleepPressure:" << sleepPressure_ << "\n";
    oss << "sleepDebt:" << sleepDebt_ << "\n";
    oss << "wakeDuration:" << wakeDuration_ << "\n";
    oss << "timeSinceWake:" << timeSinceWake_ << "\n";
    oss << "sleepEfficiency:" << sleepEfficiency_ << "\n";
    oss << "memoryReplayCount:" << memoryReplayCount_ << "\n";
    oss << "lastReplayTime:" << lastReplayTime_ << "\n";
    oss << "quietSleepTimer:" << quietSleepTimer_ << "\n";
    oss << "deepSleepTimer:" << deepSleepTimer_ << "\n";
    oss << "remSleepTimer:" << remSleepTimer_ << "\n";
    
    return oss.str();
}

bool SleepStateManager::deserialize(const std::string& data) {
    try {
        std::istringstream iss(data);
        std::string line;
        
        while (std::getline(iss, line)) {
            size_t colonPos = line.find(":");
            if (colonPos == std::string::npos) continue;
            
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            if (key == "currentState") {
                currentState_ = static_cast<SleepState>(std::stoi(value));
            } else if (key == "targetState") {
                targetState_ = static_cast<SleepState>(std::stoi(value));
            } else if (key == "transitionProgress") {
                transitionProgress_ = std::stof(value);
            } else if (key == "transitionTimer") {
                transitionTimer_ = std::stof(value);
            } else if (key == "timeInCurrentState") {
                timeInCurrentState_ = std::stof(value);
            } else if (key == "totalSleepTime") {
                totalSleepTime_ = std::stof(value);
            } else if (key == "circadianPhase") {
                circadianPhase_ = std::stof(value);
            } else if (key == "circadianPeriod") {
                circadianPeriod_ = std::stof(value);
            } else if (key == "wakeTime") {
                wakeTime_ = std::stof(value);
            } else if (key == "sleepPressure") {
                sleepPressure_ = std::stof(value);
            } else if (key == "sleepDebt") {
                sleepDebt_ = std::stof(value);
            } else if (key == "wakeDuration") {
                wakeDuration_ = std::stof(value);
            } else if (key == "timeSinceWake") {
                timeSinceWake_ = std::stof(value);
            } else if (key == "sleepEfficiency") {
                sleepEfficiency_ = std::stof(value);
            } else if (key == "memoryReplayCount") {
                memoryReplayCount_ = static_cast<size_t>(std::stoull(value));
            } else if (key == "lastReplayTime") {
                lastReplayTime_ = std::stof(value);
            } else if (key == "quietSleepTimer") {
                quietSleepTimer_ = std::stof(value);
            } else if (key == "deepSleepTimer") {
                deepSleepTimer_ = std::stof(value);
            } else if (key == "remSleepTimer") {
                remSleepTimer_ = std::stof(value);
            }
        }
        
        // Reset state-specific timers based on current state
        switch (currentState_) {
            case SleepState::QuietSleep:
                quietSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::DeepSleep:
                deepSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::REMSleep:
                remSleepTimer_ = getStateCharacteristics(currentState_).typicalDuration;
                break;
            case SleepState::Wake:
                break;  // Keep wake duration as-is
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error deserializing SleepStateManager: ") + e.what());
        return false;
    }
}

void SleepStateManager::configure(const std::map<std::string, float>& settings) {
    auto it = settings.find("transition_speed");
    if (it != settings.end()) {
        transitionSpeed_ = std::clamp(it->second, 0.0f, 2.0f);
    }
    
    it = settings.find("minimum_state_duration");
    if (it != settings.end()) {
        minimumStateDuration_ = std::max(1.0f, it->second);  // At least 1 second
    }
    
    it = settings.find("circadian_period");
    if (it != settings.end()) {
        circadianPeriod_ = std::max(1.0f, it->second);
    }
    
    NLM_LOG_INFO("SleepStateManager configured");
}

} // namespace nlm
