#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

// Update from working memory content
void AttentionalSelection::updateFromWorkingMemory(const NeuralWorkingMemory& wm) {
    NLM_LOG_INFO("AttentionalSelection: Updating from working memory");
    
    // Use working memory content to influence attention selection
    std::vector<float> wmContent = wm.retrieve();
    
    if (!wmContent.empty()) {
        // Calculate average activation
        float avgActivation = 0.0f;
        for (float val : wmContent) {
            avgActivation += std::abs(val);
        }
        avgActivation /= wmContent.size();
        
        // Higher activation increases attention focus
        float attentionFocus = std::min(1.0f, avgActivation * 2.0f);
        
        // Update winner selection based on memory content
        // Neurons with higher activation in working memory receive more attention
        // This would be implemented in processCompetition
    }
}

// Update from prediction system
void AttentionalSelection::updateFromPrediction(const PredictionSystem& prediction) {
    NLM_LOG_INFO("AttentionalSelection: Updating from prediction system");
    
    float predictionConfidence = prediction.getConfidence();
    float predictionError = prediction.getPredictionError();
    
    // High confidence predictions increase focus
    if (predictionConfidence > 0.7f) {
        // Strengthen focus on predicted states
        // This would be implemented in applyToMemorySystem
    }
    
    // High prediction error may trigger exploration/novelty-seeking attention
    if (predictionError > 0.5f) {
        // Apply novelty-based attention to explore alternatives
        // This would be implemented in applyToMemorySystem
    }
}

// Apply attention to memory system
void AttentionalSelection::applyToMemorySystem(NeuralWorkingMemory& wm, NeuralEpisodicMemory& em) {
    NLM_LOG_INFO("AttentionalSelection: Applying to memory system");
    
    // This method applies attentional selection to influence memory processing
    // In a real implementation, this would:
    
    // 1. Select which memory traces receive attention (winners from processCompetition)
    const auto& winners = getWinners();
    
    // 2. Strengthen attention-winners in working memory
    for (NeuronId neuron : winners) {
        float currentActivation = wm.getNeuronActivation(neuron);
        // Strengthen by adding attention bias
        wm.storeToNeuron(neuron, currentActivation * (1.0f + attentionBias_));
    }
    
    // 3. Apply attention to episodic memory consolidation
    // Neurons with high attention receive enhanced consolidation
    for (NeuronId neuron : winners) {
        // This would call episode relevance updates in episodic memory
        // em.updateRelevance(neuron, attentionBias_);
    }
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
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
        }
    }
    
    return winners_;
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
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
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
}

} // namespace nlm