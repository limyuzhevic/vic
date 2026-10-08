#include "../neuromodulation/Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                    // Current acetylcholine concentration
    float attentionBoost;           // Effect on attention
    float memoryConsolidation;      // Effect on memory consolidation
    float cholinergicInfluence;      // Overall influence on plasticity
    SimulationStep lastBurst;       // Time of last burst
    
    Impl()
        : level(0.0f)
        , attentionBoost(1.0f)
        , memoryConsolidation(1.5f)
        , cholinergicInfluence(1.0f)
        , lastBurst(0)
    {}
};

Acetylcholine::Acetylcholine()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Acetylcholine system initialized");
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(level, 1.0f));
    
    // Higher ACh boosts attention and memory consolidation
    pImpl->attentionBoost = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 1.0f;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attention-related processes
    return pImpl->cholinergicInfluence * pImpl->attentionBoost;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay acetylcholine level
    pImpl->level *= (1.0f - dt * 0.03f);
    
    // Gradual return of attention influence
    pImpl->attentionBoost = 1.0f + (pImpl->attentionBoost - 1.0f) * (1.0f - dt * 0.1f);
}

void Acetylcholine::signalAttention(float salience) {
    // Signal attention-worthy stimulus
    if (salience > 0.1f) {
        float burst = salience * 0.3f;
        setLevel(getLevel() + burst);
        
        NLM_LOG_INFO("Acetylcholine burst: salience=" + std::to_string(salience) + 
                     " level=" + std::to_string(getLevel()) + " attention_boost=" + std::to_string(pImpl->attentionBoost));
    }
}

void Acetylcholine::enableMemoryConsolidation() {
    // Enable memory consolidation processes
    setLevel(getLevel() + 0.2f);
    
    NLM_LOG_INFO("Memory consolidation enabled: ACh level=" + std::to_string(getLevel()));
}

} // namespace nlm
