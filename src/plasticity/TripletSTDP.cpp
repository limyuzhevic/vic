#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;          // A+ for pre-post potentiation
    float ltdWeight;          // A- for post-pre depression
    float tripletLTPWeight;   // Ax for pre-pre-post potentiation
    float tripletLTDWeight;   // Ay for post-post-pre depression
    float timeConstant;       // τ for exponential decay
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    // Triplet history buffers
    std::vector<Timestamp> recentPreSpikes;   // Last N pre-synaptic spikes
    std::vector<Timestamp> recentPostSpikes;  // Last N post-synaptic spikes
    static constexpr size_t MAX_TRIPLET_HISTORY = 10;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tripletLTPWeight(0.005f), 
             tripletLTDWeight(0.006f), timeConstant(20.0f), 
             minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse) return;
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Add new spikes to history
    for (Timestamp t : preSpikes) {
        pImpl->recentPreSpikes.push_back(t);
        if (pImpl->recentPreSpikes.size() > Impl::MAX_TRIPLET_HISTORY) {
            pImpl->recentPreSpikes.erase(pImpl->recentPreSpikes.begin());
        }
    }
    
    for (Timestamp t : postSpikes) {
        pImpl->recentPostSpikes.push_back(t);
        if (pImpl->recentPostSpikes.size() > Impl::MAX_TRIPLET_HISTORY) {
            pImpl->recentPostSpikes.erase(pImpl->recentPostSpikes.begin());
        }
    }
    
    // Compute pair-wise STDP contributions (standard STDP)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            if (deltaT > 0) {
                // Pre before post: potentiation
                float delta = pImpl->ltpWeight * std::exp(-deltaT / tau);
                totalDelta += delta;
            } else if (deltaT < 0) {
                // Post before pre: depression
                float delta = -pImpl->ltdWeight * std::exp(deltaT / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Compute triplet contributions
    totalDelta += computeTripletContributions(tau);
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        applyWeightChange(synapse, totalDelta);
    }
    
    // Clean up history
    resetActivityHistory();
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTripletLTPWeight(float weight) {
    pImpl->tripletLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getTripletLTPWeight() const {
    return pImpl->tripletLTPWeight;
}

void TripletSTDP::setTripletLTDWeight(float weight) {
    pImpl->tripletLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getTripletLTDWeight() const {
    return pImpl->tripletLTDWeight;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

float TripletSTDP::computeTripletContributions(float tau) {
    float tripletDelta = 0.0f;
    
    if (pImpl->recentPreSpikes.size() >= 3 && pImpl->recentPostSpikes.size() >= 1) {
        // Check for pre-pre-post triplet: t_pre1 < t_pre2 < t_post
        for (size_t i = 0; i < pImpl->recentPreSpikes.size() - 2; ++i) {
            for (size_t j = i + 1; j < pImpl->recentPreSpikes.size() - 1; ++j) {
                for (size_t k = 0; k < pImpl->recentPostSpikes.size(); ++k) {
                    if (pImpl->recentPreSpikes[i] < pImpl->recentPreSpikes[j] &&
                        pImpl->recentPreSpikes[j] < pImpl->recentPostSpikes[k]) {
                        float t1 = static_cast<float>(pImpl->recentPreSpikes[i]);
                        float t2 = static_cast<float>(pImpl->recentPreSpikes[j]);
                        float t3 = static_cast<float>(pImpl->recentPostSpikes[k]);
                        
                        float dt1 = t2 - t1;  // pre-pre interval
                        float dt2 = t3 - t2;  // pre-post interval
                        
                        // Pre-pre-post potentiation
                        tripletDelta += pImpl->tripletLTPWeight * 
                                       std::exp(-dt1 / tau) * std::exp(-dt2 / tau);
                    }
                }
            }
        }
    }
    
    if (pImpl->recentPostSpikes.size() >= 3 && pImpl->recentPreSpikes.size() >= 1) {
        // Check for post-post-pre triplet: t_post1 < t_post2 < t_pre
        for (size_t i = 0; i < pImpl->recentPostSpikes.size() - 2; ++i) {
            for (size_t j = i + 1; j < pImpl->recentPostSpikes.size() - 1; ++j) {
                for (size_t k = 0; k < pImpl->recentPreSpikes.size(); ++k) {
                    if (pImpl->recentPostSpikes[i] < pImpl->recentPostSpikes[j] &&
                        pImpl->recentPostSpikes[j] < pImpl->recentPreSpikes[k]) {
                        float t1 = static_cast<float>(pImpl->recentPostSpikes[i]);
                        float t2 = static_cast<float>(pImpl->recentPostSpikes[j]);
                        float t3 = static_cast<float>(pImpl->recentPreSpikes[k]);
                        
                        float dt1 = t2 - t1;  // post-post interval
                        float dt2 = t3 - t2;  // post-pre interval
                        
                        // Post-post-pre depression
                        tripletDelta -= pImpl->tripletLTDWeight * 
                                       std::exp(-dt1 / tau) * std::exp(-dt2 / tau);
                    }
                }
            }
        }
    }
    
    return tripletDelta;
}

void TripletSTDP::recordPreSpike(Timestamp t) {
    pImpl->recentPreSpikes.push_back(t);
    if (pImpl->recentPreSpikes.size() > Impl::MAX_TRIPLET_HISTORY) {
        pImpl->recentPreSpikes.erase(pImpl->recentPreSpikes.begin());
    }
}

void TripletSTDP::recordPostSpike(Timestamp t) {
    pImpl->recentPostSpikes.push_back(t);
    if (pImpl->recentPostSpikes.size() > Impl::MAX_TRIPLET_HISTORY) {
        pImpl->recentPostSpikes.erase(pImpl->recentPostSpikes.begin());
    }
}

void TripletSTDP::resetActivityHistory() {
    pImpl->recentPreSpikes.clear();
    pImpl->recentPostSpikes.clear();
}

} // namespace nlm