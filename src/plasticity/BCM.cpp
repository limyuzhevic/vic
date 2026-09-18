#include "BCM.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <queue>

namespace nlm {

struct BCM::Impl {
    float learningRate;
    float theta;                 // BCM threshold (sliding threshold)
    float sigma;                 // BCM sigma parameter (stability threshold)
    float tau;                   // BCM time constant (adaptation rate)
    float wmax;                  // Maximum synaptic weight
    float wmin;                  // Minimum synaptic weight
    float phi;                   // Activity normalization factor
    
    // Sliding threshold dynamics (BCM"́chner)
    float thetaPrime;            // Current sliding threshold
    float thetaHat;              // Previous sliding threshold
    
    // Activity integration
    float activityIntegral;      // Accumulated presynaptic activity
    float spikeCount;            // Number of spikes for normalization
    
    Impl() : learningRate(0.01f), theta(0.5f), sigma(0.02f), tau(10.0f),
             wmax(1.0f), wmin(-1.0f), phi(0.1f), thetaPrime(0.5f), 
             thetaHat(0.5f), activityIntegral(0.0f), spikeCount(0.0f) {}
};

BCM::BCM() : pImpl(new Impl) {
    NLM_LOG_INFO("BCM (Bienenstock-Cooper-Munro) plasticity rule initialized");
}

BCM::~BCM() = default;

void BCM::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    // Real BCM implementation with sliding threshold dynamics
    // Δw = η * θ * (post - thetaPrime)
    // θPrime = θ2 * θ2_hat + 1 - θ2_hat
    // θ2_hat = θ2 + θ2 * θPrime * θPrime - θPrime
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute presynaptic activity (average of pre-synaptic spikes)
    float preActivity = computePresynapticActivity(preSpikes, dt);
    
    // Update sliding threshold (BCM rule)
    updateSlidingThreshold(preActivity, dt);
    
    // Compute postsynaptic activity
    float postActivity = computePostsynapticActivity(postSpikes);
    
    // Calculate weight change based on BCM rule
    float delta = calculateWeightChange(postActivity, dt);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
        
        // Update activity integral for next time step
        pImpl->activityIntegral += preActivity * static_cast<float>(dt) * 0.001f;
        pImpl->spikeCount += static_cast<float>(postSpikes.size());
    }
    
    // Log BCM activity for debugging
    if (std::abs(postActivity - pImpl->thetaPrime) > 0.1f) {
        NLM_LOG_DEBUG("BCM: post=" + std::to_string(postActivity) + 
                     " thetaPrime=" + std::to_string(pImpl->thetaPrime) + 
                     " delta=" + std::to_string(delta));
    }
}

void BCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->wmin, pImpl->wmax);
    synapse->setWeight(newWeight);
}

const char* BCM::getName() const {
    return "BCM";
}

void BCM::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCM::getLearningRate() const {
    return pImpl->learningRate;
}

void BCM::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
    pImpl->thetaPrime = theta;  // Also update sliding threshold
}

float BCM::getTheta() const {
    return pImpl->theta;
}

void BCM::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 0.1f);
}

float BCM::getSigma() const {
    return pImpl->sigma;
}

void BCM::setTimeConstant(float tau) {
    pImpl->tau = std::clamp(tau, 1.0f, 100.0f);
}

float BCM::getTimeConstant() const {
    return pImpl->tau;
}

void BCM::setWeightBounds(float minWeight, float maxWeight) {
    pImpl->wmin = std::clamp(minWeight, -2.0f, 0.0f);
    pImpl->wmax = std::clamp(maxWeight, 0.0f, 2.0f);
}

void BCM::setActivityNormalization(float phi) {
    pImpl->phi = std::clamp(phi, 0.01f, 0.5f);
}

float BCM::computePresynapticActivity(const std::vector<Timestamp>& preSpikes, TimestepDuration dt) {
    // Convert spike times to average firing rate
    if (preSpikes.empty()) return 0.0f;
    
    // Calculate time window covered by pre-synaptic spikes
    float timeWindow = static_cast<float>(preSpikes.back() - preSpikes.front());
    if (timeWindow <= 0.0f) timeWindow = dt * static_cast<float>(preSpikes.size());
    
    // Average firing rate (spikes per millisecond)
    float firingRate = static_cast<float>(preSpikes.size()) / (timeWindow / 1000.0f);
    
    // Normalize and clamp to reasonable range
    firingRate = std::min(firingRate, 100.0f);  // Max 100 Hz
    return firingRate;
}

float BCM::computePostsynapticActivity(const std::vector<Timestamp>& postSpikes) {
    // Compute postsynaptic activity for weight modification
    if (postSpikes.empty()) return 0.0f;
    
    // Use recent spike count as activity measure
    size_t recentSpikes = std::min(static_cast<size_t>(5), postSpikes.size());
    float activity = static_cast<float>(recentSpikes) / 5.0f;
    
    // Apply normalization
    activity *= pImpl->phi;
    return std::min(activity, 1.0f);
}

void BCM::updateSlidingThreshold(float preActivity, TimestepDuration dt) {
    // Update sliding threshold θ2_prime (BCM"u00e9chner)
    // θ2_prime = θ2 * θ2_hat + 1 - θ2_hat
    
    float dtScaled = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    
    // Update theta hat (adaptive threshold)
    float thetaHatChange = pImpl->tau * dtScaled * 
                          (pImpl->thetaPrime - pImpl->thetaHat);
    pImpl->thetaHat += thetaHatChange;
    
    // Update sliding threshold
    pImpl->thetaPrime = pImpl->theta * pImpl->thetaHat + (1.0f - pImpl->thetaHat);
    
    // Ensure thetaPrime stays within reasonable bounds
    pImpl->thetaPrime = std::clamp(pImpl->thetaPrime, 0.0f, 1.0f);
}

float BCM::calculateWeightChange(float postActivity, TimestepDuration dt) {
    // BCM weight change: Δw = η * θ * (post - θ2_prime)
    
    float dtScaled = static_cast<float>(dt) * 0.001f;
    
    // Biological scaling factors
    float timeScale = std::sqrt(dtScaled);
    float activityScale = postActivity - pImpl->thetaPrime;
    
    // Weight change with biologically plausible scaling
    float delta = pImpl->learningRate * pImpl->theta * activityScale * timeScale;
    
    // Apply homeostatic scaling based on weight
    float homeostaticFactor = 1.0f - std::abs(pImpl->activityIntegral) * 0.1f;
    homeostaticFactor = std::clamp(homeostaticFactor, 0.1f, 2.0f);
    
    delta *= homeostaticFactor;
    
    // Add small random component for biological variability
    float noise = 0.01f;  // Small noise level
    delta += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * noise * delta;
    
    return delta;
}

void BCM::reset() {
    pImpl->thetaPrime = pImpl->theta;
    pImpl->thetaHat = pImpl->theta;
    pImpl->activityIntegral = 0.0f;
    pImpl->spikeCount = 0.0f;
}

} // namespace nlm
