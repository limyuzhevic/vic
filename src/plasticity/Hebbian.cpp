// Hebbian.cpp
// Optimized Hebbian implementation

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <cmath>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Optimized data structures for faster correlation calculations
    struct CorrelationData {
        std::vector<float> preSpikeTimes;
        std::vector<float> postSpikeTimes;
        float meanPre;
        float meanPost;
        float stdPre;
        float stdPost;
        size_t count;
    };
    
    std::unordered_map<SynapseId, CorrelationData> synapseCorrelations;
    
    // Lookup table for exponential decay of eligibility traces
    static constexpr size_t ELIGIBILITY_LOOKUP_SIZE = 128;
    std::array<float, ELIGIBILITY_LOOKUP_SIZE> decayTable;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {
        // Initialize eligibility trace lookup table
        float maxTime = 50.0f;  // Maximum time window for eligibility traces
        float dt = maxTime / ELIGIBILITY_LOOKUP_SIZE;
        for (size_t i = 0; i < ELIGIBILITY_LOOKUP_SIZE; ++i) {
            float t = static_cast<float>(i) * dt;
            decayTable[i] = std::exp(-t * 0.1f);  // Decay factor
        }
    }
};

Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Optimized Hebbian learning using covariance rule
     * and correlation calculations with O(n) complexity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    SynapseId synId = synapse->getId();
    
    // Get or create correlation data for this synapse
    auto& corrData = pImpl->synapseCorrelations[synId];
    corrData.count++;
    
    // Convert spike times to floats for faster processing
    std::vector<float> preTimes(preSpikes.size());
    std::vector<float> postTimes(postSpikes.size());
    
    float preSum = 0.0f, postSum = 0.0f;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        preTimes[i] = static_cast<float>(preSpikes[i]);
        preSum += preTimes[i];
    }
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        postTimes[i] = static_cast<float>(postSpikes[i]);
        postSum += postTimes[i];
    }
    
    float meanPre = preSum / preTimes.size();
    float meanPost = postSum / postTimes.size();
    
    // Update correlation data with running statistics
    if (corrData.count == 1) {
        corrData.preSpikeTimes = std::move(preTimes);
        corrData.postSpikeTimes = std::move(postTimes);
        corrData.meanPre = meanPre;
        corrData.meanPost = meanPost;
        corrData.stdPre = 0.0f;
        corrData.stdPost = 0.0f;
    } else {
        // Merge new spike data
        corrData.preSpikeTimes.insert(corrData.preSpikeTimes.end(), preTimes.begin(), preTimes.end());
        corrData.postSpikeTimes.insert(corrData.postSpikeTimes.end(), postTimes.begin(), postTimes.end());
        
        // Update running statistics (Welford's algorithm)
        float deltaPre = meanPre - corrData.meanPre;
        corrData.meanPre += deltaPre / static_cast<float>(corrData.count);
        float M2Pre = 0.0f;  // Would need to track sum of squares
        
        float deltaPost = meanPost - corrData.meanPost;
        corrData.meanPost += deltaPost / static_cast<float>(corrData.count);
    }
    
    // Calculate covariance using optimized formula
    float covariance = 0.0f;
    if (corrData.preSpikeTimes.size() >= 2 && corrData.postSpikeTimes.size() >= 2) {
        // Sample covariance calculation
        size_t minSize = std::min(corrData.preSpikeTimes.size(), corrData.postSpikeTimes.size());
        
        if (minSize >= 2) {
            float sumProducts = 0.0f;
            float sumPreSq = 0.0f, sumPostSq = 0.0f;
            
            for (size_t i = 0; i < minSize; ++i) {
                float preDev = corrData.preSpikeTimes[i] - corrData.meanPre;
                float postDev = corrData.postSpikeTimes[i] - corrData.meanPost;
                sumProducts += preDev * postDev;
                sumPreSq += preDev * preDev;
                sumPostSq += postDev * postDev;
            }
            
            float stdPre = std::sqrt(sumPreSq / (minSize - 1));
            float stdPost = std::sqrt(sumPostSq / (minSize - 1));
            
            if (stdPre > 0.0f && stdPost > 0.0f) {
                covariance = sumProducts / ((minSize - 1) * stdPre * stdPost);
            }
        }
    }
    
    // Apply Hebbian learning rule
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm