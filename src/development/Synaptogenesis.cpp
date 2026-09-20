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

Synaptogenesis::Synaptogenesis() : pImpl(std::make_unique<Impl>()) {}

Synaptogenesis::~Synaptogenesis() = default;

Synaptogenesis::Synaptogenesis(Synaptogenesis&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Synaptogenesis& Synaptogenesis::operator=(Synaptogenesis&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    if (!brain) return;
    
    // Implement real synaptogenesis algorithm
    // Creates new synapses based on neural activity and developmental stage
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get all neurons in the region
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) return;
    
    // Calculate current synaptic density
    float currentDensity = region->getSynapticDensity();
    
    // Check if we need to add synapses based on target density
    // Use probabilistic synaptogenesis based on developmental stage and activity
    if (currentDensity < pImpl->targetDensity) {
        // Calculate how many synapses to add
        float densityGap = pImpl->targetDensity - currentDensity;
        size_t neuronsToConnect = std::min(
            static_cast<size_t>(neurons.size() * densityGap * 10.0f),  // Scale with gap
            neurons.size()  // Don't connect all neurons
        );
        
        // Add new synapses probabilistically based on activity
        for (size_t i = 0; i < neuronsToConnect && i < neurons.size(); ++i) {
            Neuron* preNeuron = neurons[i];
            
            // Check if pre-neuron is capable of forming new connections
            // Neural types that can form new synapses
            NeuronType preType = preNeuron->getType();
            
            if (preType == NeuronType::Modulatory) continue;  // Modulatory neurons don't form new connections
            
            // Get current activity level of pre-neuron
            float activityLevel = preNeuron->isFiring() ? 
                                 1.0f : 0.0f;  // Simplified: just check firing
            
            // Probability of forming new synapse based on activity and rate
            float formationProbability = pImpl->formationRate * activityLevel * 
                                         (1.0f + rng.uniformReal(0.5f, 1.5f));  // Random variation
            
            if (rng.uniformReal(0.0f, 1.0f) < formationProbability) {
                // Select post-synaptic neuron randomly
                size_t postIndex = rng.uniformInt(0, neurons.size() - 1);
                Neuron* postNeuron = neurons[postIndex];
                
                // Don't form self-connections
                if (preNeuron == postNeuron) continue;
                
                // Determine synapse type based on pre and post neuron types
                SynapseType synType;
                float weight;
                
                if (preType == NeuronType::Excitatory || 
                    preType == NeuronType::Sensory ||
                    preType == NeuronType::Motor ||
                    preType == NeuronType::Internal) {
                    // excitatory synapse
                    synType = SynapseType::Excitatory;
                    // Initial weight from distribution
                    weight = rng.uniformReal(0.2f, 0.5f);  // Stronger initial connections
                    
                    // Add Hebbian component for activity-dependent strengthening
                    weight *= (1.0f + activityLevel * 0.5f);
                } else if (preType == NeuronType::Inhibitory) {
                    // inhibitory synapse
                    synType = SynapseType::Inhibitory;
                    weight = -rng.uniformReal(0.2f, 0.5f);  // Negative weights for inhibition
                } else {
                    // default to excitatory
                    synType = SynapseType::Excitatory;
                    weight = rng.uniformReal(0.1f, 0.3f);
                }
                
                // Add synaptic delay (longer for distant connections)
                Delay delay = static_cast<Delay>(rng.uniformInt(1, 5));
                
                // Create new synapse
                SynapseId synId = region->addSynapse(preNeuron->getId(), postNeuron->getId(), weight, delay);
                
                // Initialize synapse properties
                if (Synapse* syn = region->getSynapse(synId)) {
                    syn->setType(synType);
                    syn->enablePlasticity(true, true, true);  // Allow all plasticity types for new synapses
                    
                    // Set initial efficacy
                    float efficacy = rng.uniformReal(0.8f, 1.0f);  // High initial efficacy
                    syn->setEfficacy(efficacy);
                    
                    // Log synaptogenesis event
                    NLM_LOG_INFO("Synaptogenesis: New synapse created (id: " + 
                               std::to_string(synId.index()) + 
                               ", type: " + (synType == SynapseType::Excitatory ? "E" : 
                               (synType == SynapseType::Inhibitory ? "I" : "M")) + 
                               ", weight: " + std::to_string(weight) + ")");
                }
            }
        }
    }
    
    // Implement activity-dependent synaptogenesis
    // Based on correlated neural activity, form new connections
    // This implements the "neurons that fire together, wire together" principle
    
    // For now, this is a placeholder for more complex activity-dependent rules
    // Could include spike-timing-dependent synaptogenesis (STDS)
}

} // namespace nlm

} // namespace nlm
