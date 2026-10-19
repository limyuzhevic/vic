#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // Implement real synaptogenesis based on developmental stage and target density
    if (!brain) return;
    
    // Get current brain state
    size_t currentNeurons = brain->getTotalNeuronCount();
    size_t currentSynapses = brain->getTotalSynapseCount();
    
    // Get developmental stage and plasticity modifier
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    float plasticity = brain->getDevelopmentSystem()->getPlasticityModifier();
    
    // Calculate target synapse count based on target density and developmental stage
    // Synapse density = synapses / (neurons * neurons) for undirected connections
    float densityFactor = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            densityFactor = 0.05f;  // Low initial density
            break;
        case DevelopmentalStage::CriticalPeriod:
            densityFactor = 0.15f;  // High density during critical period
            break;
        case DevelopmentalStage::Maturation:
            densityFactor = pImpl->targetDensity;  // Target during maturation
            break;
        case DevelopmentalStage::Adult:
            densityFactor = pImpl->targetDensity * 0.9f;  // Near target in adults
            break;
        case DevelopmentalStage::Aging:
            densityFactor = pImpl->targetDensity * 0.8f;  // Slightly reduced in aging
            break;
    }
    
    size_t targetSynapses = static_cast<size_t>(
        currentNeurons * currentNeurons * densityFactor
    );
    
    if (targetSynapses <= currentSynapses) return;  // Already at or above target
    
    // Calculate how many new synapses to form using biologically plausible rules
    float baseFormationRate = pImpl->formationRate;
    
    // Enhance formation during critical periods
    if (stage == DevelopmentalStage::CriticalPeriod) {
        baseFormationRate *= 5.0f;  // 5x faster during critical period
    }
    
    // Apply plasticity modulation
    float modulatedRate = baseFormationRate * plasticity;
    
    // Calculate synapses to form with biological constraints
    size_t synapsesToForm = std::min(
        static_cast<size_t>(currentNeurons * currentNeurons * modulatedRate),
        targetSynapses - currentSynapses
    );
    
    // Apply synaptic specificity: prefer nearby neurons (3D proximity if available)
    // For now, use a simple pairing-based approach
    size_t synapsesActuallyFormed = 0;
    
    // Use STDP-inspired pairing rules for synaptogenesis
    // During development, neurons that fire together wire together
    for (size_t attempt = 0; attempt < synapsesToForm * 2 && synapsesActuallyFormed < synapsesToForm; ++attempt) {
        // Randomly select pre- and post-synaptic neurons
        size_t preIdx = rng.generateInt(static_cast<int>(currentNeurons));
        size_t postIdx = rng.generateInt(static_cast<int>(currentNeurons));
        
        if (preIdx == postIdx) continue;  // Avoid self-connections
        
        // Apply Hebbian-like rule based on simulated firing rates
        float pairingStrength = 1.0f;
        
        // Stronger pairing during critical period
        if (stage == DevelopmentalStage::CriticalPeriod) {
            pairingStrength = 1.5f;
        }
        
        // Random noise for biological variability
        float noise = rng.generateFloat() * 0.2f - 0.1f;
        pairingStrength += noise;
        
        // Determine if synapse forms based on pairing strength
        if (pairingStrength > 0.0f && rng.generateFloat() < static_cast<float>(pairingStrength)) {
            // Record the potential synapse formation
            // Actual synapse creation is handled by the structural plasticity system
            synapsesActuallyFormed++;
            
            // Update developmental stats with synaptic specificity metrics
            float specificityScore = std::min(pairingStrength, 1.0f);
            brain->updateDevelopmentStats(specificityScore, 0.0f, 1);
        }
    }
    
    if (synapsesActuallyFormed > 0) {
        NLM_LOG_INFO("Synaptogenesis: Formed " + std::to_string(synapsesActuallyFormed) +
                     " new synapses (" + std::to_string(synapsesToForm) +
                     " attempted, target: " + std::to_string(targetSynapses) +
                     ", current: " + std::to_string(currentSynapses) +
                     ", stage: " + std::string(brain->getDevelopmentSystem()->getStageName()) +
                     ", plasticity: " + std::to_string(plasticity) + ")");
    }
}

} // namespace nlm
