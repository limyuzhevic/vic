#include "Sleep.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct Sleep::Impl {
    class Brain* brain;
    SleepState currentState;
    TimestepDuration stateStartTime;
    TimestepDuration stateDuration;
    TimestepDuration sleepDepth;
    std::vector<std::string> consolidatedMemories;
    float consolidationStrength;
    
    // Sleep schedule in simulation seconds
    TimestepDuration awakeDuration;
    TimestepDuration nremDuration;
    TimestepDuration remDuration;
    
    Impl() 
        : brain(nullptr)
        , currentState(SleepState::Awake)
        , stateStartTime(0.0)
        , stateDuration(0.0)
        , sleepDepth(0.0f)
        , consolidationStrength(0.5f)
        , awakeDuration(60.0)
        , nremDuration(120.0)
        , remDuration(60.0) {}
};

Sleep::Sleep() : pImpl(new Impl) {}

Sleep::~Sleep() = default;

void Sleep::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Sleep system initialized");
}

Sleep::SleepState Sleep::getState() const {
    return pImpl->currentState;
}

void Sleep::update(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Update time in current state
    pImpl->stateStartTime += dt;
    
    // Check if we should transition states
    if (pImpl->currentState == SleepState::Awake) {
        pImpl->sleepDepth = 0.0f;
        if (pImpl->stateStartTime >= pImpl->awakeDuration) {
            // Transition to NREM sleep
            pImpl->currentState = SleepState::NREM;
            pImpl->stateStartTime = 0.0;
            pImpl->sleepDepth = 0.3f;
            NLM_LOG_INFO("Entering NREM sleep (deep rest)");
        }
    } else if (pImpl->currentState == SleepState::NREM) {
        pImpl->sleepDepth = std::min(1.0f, pImpl->stateStartTime / pImpl->nremDuration * 2.0f);
        if (pImpl->stateStartTime >= pImpl->nremDuration) {
            // Transition to REM sleep
            pImpl->currentState = SleepState::REM;
            pImpl->stateStartTime = 0.0;
            pImpl->sleepDepth = 0.7f;
            NLM_LOG_INFO("Entering REM sleep (dreaming)");
        }
    } else if (pImpl->currentState == SleepState::REM) {
        pImpl->sleepDepth = std::max(0.0f, 1.0f - pImpl->stateStartTime / pImpl->remDuration);
        if (pImpl->stateStartTime >= pImpl->remDuration) {
            // Transition back to awake
            pImpl->currentState = SleepState::Awake;
            pImpl->stateStartTime = 0.0;
            pImpl->sleepDepth = 0.0f;
            NLM_LOG_INFO("Waking up");
        }
    }
    
    // Perform memory consolidation during sleep
    if (pImpl->currentState != SleepState::Awake) {
        // NREM is good for declarative memory consolidation
        if (pImpl->currentState == SleepState::NREM) {
            float consolidationFactor = pImpl->sleepDepth * pImpl->consolidationStrength;
            // This would integrate with episodic memory system
            // For now, just log the consolidation happening
            NLM_LOG_INFO("NREM consolidation active (depth: " + std::to_string(pImpl->sleepDepth) + ")");
        } else if (pImpl->currentState == SleepState::REM) {
            // REM is good for procedural memory and emotional processing
            float consolidationFactor = pImpl->sleepDepth * pImpl->consolidationStrength * 0.5f;
            NLM_LOG_INFO("REM consolidation active (depth: " + std::to_string(pImpl->sleepDepth) + ")");
        }
    }
}

float Sleep::getSleepDepth() const {
    return pImpl->sleepDepth;
}

std::string Sleep::getCurrentStageName() const {
    switch (pImpl->currentState) {
        case SleepState::Awake: return "Awake";
        case SleepState::NREM: return "NREM (Deep Rest)";
        case SleepState::REM: return "REM (Dreaming)";
        case SleepState::Transition: return "Transition";
        default: return "Unknown";
    }
}

TimestepDuration Sleep::getTimeInCurrentState() const {
    return pImpl->stateStartTime;
}

float Sleep::getConsolidationRate() const {
    if (pImpl->currentState == SleepState::Awake) return 0.0f;
    return pImpl->sleepDepth * pImpl->consolidationStrength;
}

void Sleep::setSleepSchedule(float awakeDuration, float nremDuration, float remDuration) {
    pImpl->awakeDuration = awakeDuration;
    pImpl->nremDuration = nremDuration;
    pImpl->remDuration = remDuration;
    NLM_LOG_INFO("Sleep schedule set: Awake " + std::to_string(awakeDuration) + 
                 "s, NREM " + std::to_string(nremDuration) + "s, REM " + std::to_string(remDuration) + "s");
}

void Sleep::setConsolidationStrength(float strength) {
    pImpl->consolidationStrength = std::clamp(strength, 0.0f, 1.0f);
}

const std::vector<std::string>& Sleep::getConsolidatedMemories() const {
    return pImpl->consolidatedMemories;
}

void Sleep::reset() {
    pImpl->currentState = SleepState::Awake;
    pImpl->stateStartTime = 0.0;
    pImpl->sleepDepth = 0.0f;
    pImpl->consolidatedMemories.clear();
    NLM_LOG_INFO("Sleep system reset");
}

bool Sleep::isSleeping() const {
    return pImpl->currentState != SleepState::Awake;
}

} // namespace nlm
