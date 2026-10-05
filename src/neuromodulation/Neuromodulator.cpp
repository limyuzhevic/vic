#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float plasticityModifier;
    float neuromodulatoryIndex;
    float receptorDensity;
    float reuptakeRate;
    float synthesisRate;
    
    // Dopamine neuron dynamics
    float firingRate;
    float calciumLevel;
    float dHPotential;
    
    // Receptor states
    float d1ReceptorActivation;
    float d2ReceptorActivation;
    float d5ReceptorActivation;
    
    // Time integration
    float timeSinceLastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , adaptationRate(0.05f), plasticityModifier(1.0f), neuromodulatoryIndex(0.0f)
        , receptorDensity(1.0f), reuptakeRate(0.2f), synthesisRate(0.1f)
        , firingRate(0.0f), calciumLevel(0.0f), dHPotential(0.0f)
        , d1ReceptorActivation(0.0f), d2ReceptorActivation(0.0f), d5ReceptorActivation(0.0f)
        , timeSinceLastUpdate(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: D1 receptor enhances LTP, D2 enhances LTD
    // D1 activation promotes synaptic strengthening, D2 promotes weakening
    float d1Effect = pImpl->d1ReceptorActivation * 0.7f;  // D1 enhances LTP
    float d2Effect = pImpl->d2ReceptorActivation * 0.7f;  // D2 enhances LTD
    
    // Net effect on plasticity
    float netEffect = d1Effect - d2Effect;
    float plasticityFactor = 0.5f + netEffect * 0.5f;  // Range: 0.0 to 1.0
    
    return std::clamp(plasticityFactor, 0.1f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate = dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Receptor dynamics
    float decayRate = pImpl->decayRate * 0.5f;
    pImpl->d1ReceptorActivation = std::max(0.0f, pImpl->d1ReceptorActivation - decayRate * static_cast<float>(dt));
    pImpl->d2ReceptorActivation = std::max(0.0f, pImpl->d2ReceptorActivation - decayRate * static_cast<float>(dt));
    pImpl->d5ReceptorActivation = std::max(0.0f, pImpl->d5ReceptorActivation - decayRate * static_cast<float>(dt));
    
    // Neuromodulatory index tracks recent dopamine activity
    pImpl->neurotransmitterIndex *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected reward
    // Burst of dopamine proportional to reward prediction error
    float burst = reward * pImpl->releaseRate;
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Activate D1 receptors (reward prediction)
    pImpl->d1ReceptorActivation = std::min(1.0f, pImpl->d1ReceptorActivation + burst * 0.5f);
    
    // Can also affect D2 receptors depending on context
    if (reward < 0.0f) {
        pImpl->d2ReceptorActivation = std::min(1.0f, pImpl->d2ReceptorActivation + (-reward) * 0.3f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Positive error = better than expected = dopamine increase
    // Negative error = worse than expected = dopamine decrease
    
    // The primary mechanism for dopamine signaling prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // D1 receptor responds to positive prediction errors (better than expected)
    if (error > 0.0f) {
        pImpl->d1ReceptorActivation = std::min(1.0f, pImpl->d1ReceptorActivation + error * 0.4f);
    } else {
        // D2 receptor responds to negative prediction errors (worse than expected)
        pImpl->d2ReceptorActivation = std::min(1.0f, pImpl->d2ReceptorActivation + (-error) * 0.4f);
    }
}

void Dopamine::updateReceptorStates(const class NeuralRegion* region) {
    // Update receptor activation based on neural activity in the region
    if (!region) return;
    
    // D1 receptors are typically found on dendritic spines, enhancing LTP
    // D2 receptors are often found on axon initial segments, affecting spike timing
    
    // For now, we'll update receptor states based on regional activity
    // In a real implementation, this would involve more complex modeling
    
    float regionalActivity = calculateRegionalActivity(region);
    
    // D1 receptors are more active with sustained excitation
    pImpl->d1ReceptorActivation = std::min(1.0f, pImpl->d1ReceptorActivation + regionalActivity * 0.05f);
    
    // D2 receptors respond to burst activity patterns
    // This would require more sophisticated temporal processing
    float burstIndicator = std::min(1.0f, regionalActivity * 2.0f);
    pImpl->d2ReceptorActivation = std::min(1.0f, pImpl->d2ReceptorActivation + burstIndicator * 0.02f);
}

float Dopamine::getD1Effect() const {
    return pImpl->d1ReceptorActivation;
}

float Dopamine::getD2Effect() const {
    return pImpl->d2ReceptorActivation;
}

float Dopamine::calculateRegionalActivity(const class NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    // Simple approximation: average firing rate of region neurons
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& pop : region->getPopulations()) {
        for (const auto* neuron : pop->getNeurons()) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
            neuronCount++;
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0f;
}

std::string Dopamine::getStateDescription() const {
    std::stringstream ss;
    ss << "Dopamine level: " << pImpl->level << std::endl;
    ss << "D1 receptor activation: " << pImpl->d1ReceptorActivation << std::endl;
    ss << "D2 receptor activation: " << pImpl->d2ReceptorActivation << std::endl;
    ss << "Plasticity factor: " << getPlasticityFactor() << std::endl;
    return ss.str();
}

} // namespace nlm
