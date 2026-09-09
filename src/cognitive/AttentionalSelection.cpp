#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    std::vector<float> activationHistory;
    
    // Weight management
    std::vector<float> weightHistory;
    float movingAverageWeight;
    float weightVariance;
    
    // Integration state
    float timeSinceLastUpdate;
    bool isStale;
    
    Impl() : movingAverageWeight(0.0f), weightVariance(0.0f), 
             timeSinceLastUpdate(0.0f), isStale(true) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
    , totalAttentionWeight_(0.0f)
    , weightsNormalized_(false)
    , lastUpdateTime_(0.0f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Initialize attention weights with uniform distribution
    attentionWeights_.resize(100, 1.0f); // Assume max 100 neurons
    totalAttentionWeight_ = attentionWeights_.size() * 1.0f;
    weightsNormalized_ = false;
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Normalize attention weights
    normalizeAttentionWeights();
    
    // Compute activity levels for competitors using attention weights
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        // Apply attention weights for integration with working memory
        float weightIndex = static_cast<float>(neuron.value) / 1000.0f; // Normalize neuron ID
        size_t weightIdx = static_cast<size_t>(weightIndex * attentionWeights_.size()) % attentionWeights_.size();
        float attentionWeight = attentionWeights_[weightIdx];
        
        // Combined activation with attention weighting
        activities[i] = (salience + bias) * attentionWeight;
        totalActivity += activities[i];
        
        // Store in implementation for tracking
        pImpl->activationHistory.push_back(activities[i]);
        if (pImpl->activationHistory.size() > 100) {
            pImpl->activationHistory.erase(pImpl->activationHistory.begin());
        }
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        winners_ = competitors;
        return winners_;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
            }
            
            // Update attention weight based on selection strength
            float selectionStrength = activities[i] / totalActivity;
            updateAttentionWeight(competitors[i], selectionStrength);
        }
    }
    
    return winners_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time
    for (auto& s : bottomUpSalience_) {
        s *= 0.95f;
    }
    
    for (auto& b : topDownBias_) {
        b *= 0.98f;
    }
    
    // Decay inhibition
    for (auto& i : pImpl->inhibitionLevel) {
        i *= 0.9f;
    }
    
    // Update time tracking
    float currentTime = static_cast<float>(dt);
    pImpl->timeSinceLastUpdate = currentTime - lastUpdateTime_;
    lastUpdateTime_ = currentTime;
    
    // Decay attention weights gradually to prevent runaway excitation
    for (auto& weight : attentionWeights_) {
        weight *= 0.995f;
    }
    
    pImpl->isStale = false;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::max(0.0f, std::min(strength, 2.0f)); // Clamp to [0, 2]
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::max(0.0f, std::min(strength, 5.0f)); // Clamp to [0, 5]
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::max(0.0f, std::min(threshold, 1.0f)); // Clamp to [0, 1]
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
    
    // Reset attention weights
    std::fill(attentionWeights_.begin(), attentionWeights_.end(), 1.0f);
    totalAttentionWeight_ = attentionWeights_.size() * 1.0f;
    weightsNormalized_ = false;
    
    pImpl->isStale = true;
}

void AttentionalSelection::normalizeAttentionWeights() {
    if (weightsNormalized_) return;
    
    float sum = std::accumulate(attentionWeights_.begin(), attentionWeights_.end(), 0.0f);
    if (sum > 0.0f) {
        for (auto& weight : attentionWeights_) {
            weight /= sum;
        }
        totalAttentionWeight_ = 1.0f;
        weightsNormalized_ = true;
    } else {
        // Fallback to uniform weights
        std::fill(attentionWeights_.begin(), attentionWeights_.end(), 1.0f);
        totalAttentionWeight_ = attentionWeights_.size() * 1.0f;
        weightsNormalized_ = true;
    }
}

void AttentionalSelection::updateAttentionWeight(NeuronId neuron, float selectionStrength) {
    float weightIndex = static_cast<float>(neuron.value) / 1000.0f; // Normalize neuron ID
    size_t weightIdx = static_cast<size_t>(weightIndex * attentionWeights_.size()) % attentionWeights_.size();
    
    // Update weight with selection strength
    attentionWeights_[weightIdx] += selectionStrength * 0.1f;
    
    // Ensure weight doesn't exceed bounds
    attentionWeights_[weightIdx] = std::max(0.1f, std::min(attentionWeights_[weightIdx], 5.0f));
    
    // Mark as not normalized
    weightsNormalized_ = false;
    
    // Update implementation statistics
    pImpl->weightHistory.push_back(attentionWeights_[weightIdx]);
    if (pImpl->weightHistory.size() > 100) {
        pImpl->weightHistory.erase(pImpl->weightHistory.begin());
    }
    
    // Update moving average and variance
    if (!pImpl->weightHistory.empty()) {
        float sum = std::accumulate(pImpl->weightHistory.begin(), pImpl->weightHistory.end(), 0.0f);
        pImpl->movingAverageWeight = sum / pImpl->weightHistory.size();
        
        float sqSum = 0.0f;
        for (float w : pImpl->weightHistory) {
            float diff = w - pImpl->movingAverageWeight;
            sqSum += diff * diff;
        }
        pImpl->weightVariance = sqSum / pImpl->weightHistory.size();
    }
}

} // namespace nlm