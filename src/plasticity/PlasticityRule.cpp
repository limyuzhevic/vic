#include "PlasticityRule.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <random>

namespace nlm {

// PlasticityRule implementation
bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// HebbianRule implementation
struct HebbianRule::Impl {
    float learningRate = 0.01f;
    float weightRangeMin = -1.0f;
    float weightRangeMax = 1.0f;
    bool enableBoundsChecking = true;
    float weightChangeLimit = 0.1f;
    
    // For O(n) optimization - track recent spike counts
    std::unordered_map<NeuronId, size_t> preSpikeCounts;
    std::unordered_map<NeuronId, size_t> postSpikeCounts;
    Timestamp lastResetTime = 0.0;
    
    // Enhanced Hebbian parameters
    float potentiationFactor = 1.0f;
    float depressionFactor = 0.95f;
    bool useSlidingWindow = true;
    Timestamp spikeWindow = 50.0f;  // 50ms window
    
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> weightDist;
};

HebbianRule::HebbianRule() : pImpl(new Impl()) {
    pImpl->weightDist = std::uniform_real_distribution<float>(-0.1f, 0.1f);
}

HebbianRule::~HebbianRule() {
    delete pImpl;
}

void HebbianRule::update(Synapse* synapse, 
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!isEnabled() || !synapse) return;
    
    float currentWeight = synapse->getWeight();
    Timestamp currentTime = dt;  // Simplified - would use actual current time
    
    // O(n) optimization: Use spike counts instead of pairwise comparison
    NeuronId preNeuron = synapse->getSourceNeuron().index();
    NeuronId postNeuron = synapse->getDestinationNeuron().index();
    
    // Count spikes within sliding window
    size_t preCount = 0, postCount = 0;
    
    if (pImpl->useSlidingWindow && !preSpikes.empty()) {
        // Count spikes in recent window
        preCount = std::count_if(preSpikes.begin(), preSpikes.end(),
            [currentTime, window = pImpl->spikeWindow](Timestamp t) {
                return (currentTime - t) < window;
            });
    } else {
        preCount = preSpikes.size();
    }
    
    if (pImpl->useSlidingWindow && !postSpikes.empty()) {
        postCount = std::count_if(postSpikes.begin(), postSpikes.end(),
            [currentTime, window = pImpl->spikeWindow](Timestamp t) {
                return (currentTime - t) < window;
            });
    } else {
        postCount = postSpikes.size();
    }
    
    // Compute weight change using enhanced Hebbian rule
    float weightChange = 0.0f;
    
    if (preCount > 0 && postCount > 0) {
        // Coincidence detection - both neurons fired
        float coincidenceFactor = static_cast<float>(std::min(preCount, postCount)) / 
                                  static_cast<float>(std::max(1, static_cast<int>(preSpikes.size() + postSpikes.size())));
        
        // Apply potentiation with learning rate
        weightChange += pImpl->learningRate * pImpl->potentiationFactor * coincidenceFactor;
        
        // Store spike counts for normalization
        pImpl->preSpikeCounts[preNeuron] += preCount;
        pImpl->postSpikeCounts[postNeuron] += postCount;
    } else if (preCount > 0 || postCount > 0) {
        // Unilateral firing - depression
        float depressionFactor = static_cast<float>(std::max(preCount, postCount)) / 
                                 static_cast<float>(std::max(1, static_cast<int>(preSpikes.size() + postSpikes.size())));
        weightChange -= pImpl->learningRate * pImpl->depressionFactor * depressionFactor;
    }
    
    // Apply weight bounds checking
    float newWeight = currentWeight + weightChange;
    
    if (pImpl->enableBoundsChecking) {
        newWeight = std::clamp(newWeight, pImpl->weightRangeMin, pImpl->weightRangeMax);
    }
    
    // Apply learning rate limit to prevent large jumps
    float maxWeightChange = pImpl->weightChangeLimit;
    float actualWeightChange = newWeight - currentWeight;
    
    if (std::abs(actualWeightChange) > maxWeightChange) {
        newWeight = currentWeight + 
                   (actualWeightChange > 0.0f ? maxWeightChange : -maxWeightChange);
    }
    
    // Apply the weight change
    applyWeightChange(synapse, newWeight - currentWeight);
    
    // Update internal statistics
    synapse->setEligibilityTrace(synapse->getEligibilityTrace() + 0.5f * coincidenceFactor);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse || !isEnabled()) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds
    if (pImpl->enableBoundsChecking) {
        newWeight = std::clamp(newWeight, pImpl->weightRangeMin, pImpl->weightRangeMax);
    }
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 1.0f));
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// AntiHebbianRule implementation
void AntiHebbianRule::update(Synapse* synapse, 
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!isEnabled() || !synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Apply Anti-Hebbian learning: decrease weight when neurons fire together
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Compute depression based on coincidence
        float coincidenceFactor = 0.5f;  // Simplified - could be enhanced
        float weightChange = -pImpl->depressionFactor * coincidenceFactor;
        
        // Apply bounds checking
        float newWeight = std::clamp(currentWeight + weightChange, 
                                     pImpl->weightRangeMin, pImpl->weightRangeMax);
        
        applyWeightChange(synapse, newWeight - currentWeight);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse || !isEnabled()) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + delta, 
                                 pImpl->weightRangeMin, pImpl->weightRangeMax);
    
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

// BCMRule implementation
struct BCMRule::Impl {
    float theta = 25.0f;          // Threshold parameter
    float eta = 0.01f;           // Learning rate
    float phi = 0.02f;           // Depression rate
    float weightRangeMin = -1.0f;
    float weightRangeMax = 1.0f;
    
    std::vector<float> postSpikeHistory;
    Timestamp lastResetTime = 0.0;
};

BCMRule::BCMRule() : pImpl(new Impl()) {}

BCMRule::~BCMRule() {
    delete pImpl;
}

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!isEnabled() || !synapse) return;
    
    float currentWeight = synapse->getWeight();
    Timestamp currentTime = dt;  // Simplified
    
    // Update BCM variables
    pImpl->postSpikeHistory.push_back(currentWeight * currentWeight); // Simplified
    
    // Apply BCM learning rule
    float hebbianTerm = 0.0f;
    float bcmTerm = 0.0f;
    
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Hebbian component
        hebbianTerm = pImpl->eta * currentWeight;
        
        // BCM component - sliding window average of post-synaptic activity
        float avgPostActivity = 0.0f;
        if (pImpl->postSpikeHistory.size() > 10) {
            avgPostActivity = std::accumulate(pImpl->postSpikeHistory.begin(), 
                                             pImpl->postSpikeHistory.end(), 0.0f) / 
                            static_cast<float>(pImpl->postSpikeHistory.size());
        }
        
        // Apply BCM rule
        float bcmFactor = (avgPostActivity - pImpl->theta);
        bcmTerm = -pImpl->phi * bcmFactor;
        
        float weightChange = hebbianTerm + bcmTerm;
        
        // Apply bounds checking
        float newWeight = std::clamp(currentWeight + weightChange,
                                     pImpl->weightRangeMin, pImpl->weightRangeMax);
        
        applyWeightChange(synapse, newWeight - currentWeight);
    }
    
    // Clean up old spike history (keep last 100 entries)
    if (pImpl->postSpikeHistory.size() > 100) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse || !isEnabled()) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + delta,
                                 pImpl->weightRangeMin, pImpl->weightRangeMax);
    
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

// Additional helper functions for enhanced functionality
namespace {
    
    // Factory function to create plasticity rules
    std::unique_ptr<PlasticityRule> createPlasticityRule(PlasticityRuleType type) {
        switch (type) {
            case PlasticityRuleType::Hebbian:
                return std::make_unique<HebbianRule>();
            case PlasticityRuleType::AntiHebbian:
                return std::make_unique<AntiHebbianRule>();
            case PlasticityRuleType::BCM:
                return std::make_unique<BCMRule>();
            default:
                NLM_LOG_ERROR("Unknown plasticity rule type: " + std::to_string(static_cast<int>(type)));
                return nullptr;
        }
    }
    
    // Configuration structure for plasticity rules
    struct PlasticityConfig {
        PlasticityRuleType type;
        float learningRate = 0.01f;
        float weightMin = -1.0f;
        float weightMax = 1.0f;
        bool enableBounds = true;
        bool useSlidingWindow = true;
        float spikeWindow = 50.0f;
        float theta = 25.0f;     // BCM parameter
        float eta = 0.01f;      // BCM learning rate
        float phi = 0.02f;      // BCM depression rate
    };
    
    // Plugin architecture registration
    class PlasticityPluginManager {
    public:
        using PluginFactory = std::function<std::unique_ptr<PlasticityRule>(const PlasticityConfig&)>;
        
        void registerPlugin(const std::string& name, PluginFactory factory) {
            plugins_[name] = factory;
        }
        
        std::unique_ptr<PlasticityRule> createPlugin(const std::string& name, const PlasticityConfig& config) {
            auto it = plugins_.find(name);
            if (it != plugins_.end()) {
                return it->second(config);
            }
            return nullptr;
        }
        
        std::vector<std::string> getPluginNames() const {
            std::vector<std::string> names;
            for (const auto& pair : plugins_) {
                names.push_back(pair.first);
            }
            return names;
        }
        
    private:
        std::unordered_map<std::string, PluginFactory> plugins_;
    };
    
    // Global plugin manager instance
    PlasticityPluginManager& getPluginManager() {
        static PlasticityPluginManager manager;
        return manager;
    }
    
} // anonymous namespace

} // namespace nlm
