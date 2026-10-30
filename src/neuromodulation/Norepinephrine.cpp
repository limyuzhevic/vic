#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float targetLevel;
    float arousalLevel;
    float vigilanceFactor;
    float stressResponse;
    float fightOrFlightLevel;
    float wakeDrive;
    float sleepDrive;
    float locusCoeruleusGain;
    float locusCoeruleusSensitivity;
    float noradrenergicTone;
    
    static constexpr float DEFAULT_BASELINE = 0.02f;
    static constexpr float DEFAULT_AROUSAL = 0.4f;
    static constexpr float DEFAULT_VIGILANCE = 0.3f;
    static constexpr float DEFAULT_STRESS_RESPONSE = 0.1f;
    static constexpr float DEFAULT_FIGHT_OR_FLIGHT = 0.2f;
    static constexpr float DEFAULT_WAKE_DRIVE = 0.8f;
    static constexpr float DEFAULT_SLEEP_DRIVE = 0.2f;
    static constexpr float DEFAULT_LOCUS_GAIN = 1.0f;
    static constexpr float DEFAULT_LOCUS_SENSITIVITY = 0.7f;
    static constexpr float DEFAULT_NORADENERGIC_TONE = 0.05f;
    
    Impl()
        : level(DEFAULT_BASELINE)
        , baseline(DEFAULT_BASELINE)
        , targetLevel(DEFAULT_BASELINE)
        , arousalLevel(DEFAULT_AROUSAL)
        , vigilanceFactor(DEFAULT_VIGILANCE)
        , stressResponse(DEFAULT_STRESS_RESPONSE)
        , fightOrFlightLevel(DEFAULT_FIGHT_OR_FLIGHT)
        , wakeDrive(DEFAULT_WAKE_DRIVE)
        , sleepDrive(DEFAULT_SLEEP_DRIVE)
        , locusCoeruleusGain(DEFAULT_LOCUS_GAIN)
        , locusCoeruleusSensitivity(DEFAULT_LOCUS_SENSITIVITY)
        , noradrenergicTone(DEFAULT_NORADENERGIC_TONE) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = pImpl->level;
    updateDerivedFunctions();
}

float Norepinephrine::getPlasticityFactor() const {
    float arousalEffect = pImpl->arousalLevel * 0.3f;
    float vigilanceEffect = pImpl->vigilanceFactor * 0.2f;
    float stressEffect = pImpl->stressResponse * 0.3f;
    float fightOrFlightEffect = pImpl->fightOrFlightLevel * 0.2f;
    
    float totalEffect = arousalEffect + vigilanceEffect + stressEffect + fightOrFlightEffect;
    float plasticityFactor = 1.0f + totalEffect * (1.0f - std::abs(totalEffect - 0.5f) * 2.0f);
    
    return std::max(0.5f, std::min(2.0f, plasticityFactor));
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeConstant = 1.0f / pImpl->locusCoeruleusGain;
    
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - (pImpl->level - pImpl->baseline) * 
                              static_cast<float>(dt) / timeConstant);
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + (pImpl->targetLevel - pImpl->level) * 
                              static_cast<float>(dt) / (timeConstant * 1.5f));
    }
    
    pImpl->noradrenergicTone = pImpl->baseline + pImpl->noradrenergicTone * 0.95f;
}

float Norepinephrine::getBaselineLevel() const {
    return pImpl->baseline;
}

void Norepinephrine::setTargetLevel(float target) {
    pImpl->targetLevel = std::clamp(target, 0.0f, 1.0f);
}

bool Norepinephrine::isActive() const {
    return pImpl->level > pImpl->baseline + 0.01f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
    updateDerivedFunctions();
}

void Norepinephrine::updateDerivedFunctions() {
    pImpl->arousalLevel = std::clamp(pImpl->level * 2.0f, 0.0f, 1.0f);
    pImpl->vigilanceFactor = std::min(1.0f, pImpl->level * 3.0f);
    pImpl->stressResponse = pImpl->level * 0.5f;
    pImpl->fightOrFlightLevel = std::min(1.0f, pImpl->level * 2.5f);
    pImpl->wakeDrive = std::clamp(pImpl->wakeDrive + (pImpl->level - 0.5f) * 0.1f, 0.0f, 1.0f);
    pImpl->sleepDrive = 1.0f - pImpl->wakeDrive;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setArousalLevel(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getVigilanceFactor() const {
    return pImpl->vigilanceFactor;
}

void Norepinephrine::setVigilanceFactor(float factor) {
    pImpl->vigilanceFactor = std::clamp(factor, 0.0f, 1.0f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressResponse = std::clamp(pImpl->stressResponse + stressLevel * 0.1f, 0.0f, 1.0f);
    
    float stressResponse = std::min(0.5f, pImpl->stressResponse * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
    
    if (stressLevel > 0.5f) {
        pImpl->fightOrFlightLevel = 1.0f;
    }
    
    NLM_LOG_INFO("Norepinephrine stress response: level " + std::to_string(pImpl->level));
}

float Norepinephrine::getStressResponse() const {
    return pImpl->stressResponse;
}

void Norepinephrine::setLocusCoeruleusGain(float gain) {
    pImpl->locusCoeruleusGain = std::max(0.1f, gain);
}

float Norepinephrine::getLocusCoeruleusSensitivity() const {
    return pImpl->locusCoeruleusSensitivity;
}

float Norepinephrine::getFightOrFlightLevel() const {
    return pImpl->fightOrFlightLevel;
}

void Norepinephrine::updateFightOrFlight(float threatLevel, TimestepDuration dt) {
    if (threatLevel > 0.7f) {
        pImpl->fightOrFlightLevel = 1.0f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + threatLevel * 0.5f);
        
        pImpl->sleepDrive = 0.0f;
        pImpl->wakeDrive = 1.0f;
        
        NLM_LOG_INFO("Fight-or-flight activated: threat level " + std::to_string(threatLevel));
    } else {
        pImpl->fightOrFlightLevel = std::max(0.0f, pImpl->fightOrFlightLevel - 0.1f * static_cast<float>(dt));
    }
}

void Norepinephrine::setWakeDrive(float drive) {
    pImpl->wakeDrive = std::clamp(drive, 0.0f, 1.0f);
    pImpl->sleepDrive = 1.0f - pImpl->wakeDrive;
}

float Norepinephrine::getWakeDrive() const {
    return pImpl->wakeDrive;
}

void Norepinephrine::setSleepDrive(float drive) {
    pImpl->sleepDrive = std::clamp(drive, 0.0f, 1.0f);
    pImpl->wakeDrive = 1.0f - pImpl->sleepDrive;
}

float Norepinephrine::getSleepDrive() const {
    return pImpl->sleepDrive;
}

} // namespace nlm
