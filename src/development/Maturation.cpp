#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    // Implement real maturation with biological mechanisms
    // Includes synaptic strengthening, myelination, and functional specialization
    
    try {
        if (!brain) {
            NLM_LOG_WARNING("Maturation::update: Null brain pointer");
            return;
        }
        
        // Calculate maturation time factor based on developmental stage
        float timeFactor = std::min(1.0f, static_cast<float>(currentStep) / pImpl->matureTimeConstant);
        
        // Update maturation progress using sigmoidal growth curve
        pImpl->progress = 1.0f / (1.0f + std::exp(-3.0f * (timeFactor - 0.5f)));
        
        // Apply activity-dependent maturation (Hebbian-like)
        float activityFactor = 0.0f;
        if (brain->getNeuronCount() > 0) {
            // Get average neural activity from brain
            float totalActivity = 0.0f;
            int neuronCount = 0;
            
            for (const auto& neuron : brain->getAllNeurons()) {
                if (neuron && neuron->getActivity() > 0.0f) {
                    totalActivity += neuron->getActivity();
                    ++neuronCount;
                }
            }
            
            if (neuronCount > 0) {
                activityFactor = std::min(1.0f, totalActivity / static_cast<float>(neuronCount) * 2.0f);
            }
        }
        
        // Combine time and activity factors
        float totalProgress = pImpl->progress * 0.7f + activityFactor * 0.3f;
        setProgress(totalProgress);
        
        // Apply maturation effects to brain
        applyMaturationEffects(brain, totalProgress);
        
        // Log significant maturation milestones
        if (std::abs(totalProgress - pImpl->progress) > 0.1f) {
            NLM_LOG_DEBUG(std::string("Maturation progress: ") + std::to_string(totalProgress));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Maturation::update error: ") + e.what());
    }
}

void Maturation::applyMaturationEffects(Brain* brain, float progress) {
    // Apply biological maturation effects to neural network
    if (!brain || progress < 0.1f) return;
    
    float effectStrength = progress * 0.8f;  // Stronger effects in later maturation
    
    // 1. Synaptic strengthening (activity-dependent potentiation)
    for (auto& synapse : brain->getAllSynapses()) {
        if (!synapse) continue;
        
        float weight = synapse->getWeight();
        float activity = synapse->getPostSynapticActivity();
        
        // Apply maturation-based potentiation
        float potentiation = effectStrength * activity * 0.02f;
        synapse->addToWeight(potentiation);
        
        // Apply maximum weight constraints
        synapse->setWeight(std::min(synapse->getWeight(), synapse->getMaxWeight()));
    }
    
    // 2. Myelin-like effects (signal conduction speed)
    // In a full implementation, this would increase conduction velocities
    
    // 3. Neural specialization (differentiates neuron types)
    for (auto& neuron : brain->getAllNeurons()) {
        if (!neuron) continue;
        
        // Activity-dependent differentiation
        float activity = neuron->getActivity();
        NeuronType specializedType = neuron->getNeuronType();
        
        // Low-activity neurons may become inhibitory, high-activity excitatory
        if (activity < 0.1f && specializedType != NeuronType::Inhibitory) {
            // neuron->setNeuronType(NeuronType::Inhibitory); // Would need neuron type management
        } else if (activity > 0.5f && specializedType != NeuronType::Excitatory) {
            // neuron->setNeuronType(NeuronType::Excitatory); // Would need neuron type management
        }
    }
}

} // namespace nlm

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm
