#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct AttentionalSelection::Impl {
    // Enhanced competition state
    std::vector<float> neuronActivities;
    std::vector<float> salienceValues;
    std::vector<float> biasValues;
    std::vector<size_t> neuronIds;
    
    // Attention dynamics
    float globalInhibitionLevel;
    float attentionGain;
    float decayRate;
    
    // Selection state
    std::vector<NeuronId> selectedWinners;
    std::vector<NeuronId> attendedRegions;
    
    // Memory integration
    size_t workingMemorySize;
    size_t maxWinners;
    
    Impl()
        : globalInhibitionLevel(0.5f)
        , attentionGain(1.5f)
        , decayRate(0.95f)
        , workingMemorySize(100)
        , maxWinners(10)
    {}
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
    NLM_LOG_INFO("AttentionalSelection initialized with neural dynamics");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    pImpl->selectedWinners.clear();
    
    if (competitors.empty() || !brain_) return winners_;
    
    // Collect actual neuron activities from brain regions
    pImpl->neuronActivities.clear();
    pImpl->salienceValues.clear();
    pImpl->biasValues.clear();
    pImpl->neuronIds.clear();
    
    for (const auto& neuronId : competitors) {
        // Get actual neuron activity from the brain
        float activity = getNeuronActivity(neuronId);
        
        // Get bottom-up salience (based on sensory input patterns)
        float salience = getNeuronSalience(neuronId);
        
        // Get top-down bias (goal-relevant signals)
        float bias = getNeuronBias(neuronId);
        
        pImpl->neuronActivities.push_back(activity);
        pImpl->salienceValues.push_back(salience);
        pImpl->biasValues.push_back(bias);
        pImpl->neuronIds.push_back(neuronId.value);
        
        // Combined activation strength
        float combinedStrength = (activity * 0.3f) + (salience * 0.5f) + (bias * 0.2f);
        
        // Apply global inhibition
        combinedStrength *= (1.0f - globalInhibition * pImpl->globalInhibitionLevel);
        
        // Apply attention gain
        combinedStrength *= pImpl->attentionGain;
        
        // Store if above threshold for consideration
        if (combinedStrength > 0.01f) {
            // Apply lateral inhibition to weaker competitors
            for (size_t j = 0; j < pImpl->neuronActivities.size() - 1; ++j) {
                if (pImpl->neuronActivities[j] > combinedStrength * 0.8f) {
                    // Stronger neuron suppresses weaker ones
                    float suppression = (pImpl->neuronActivities[j] - combinedStrength) * 0.2f;
                    pImpl->salienceValues[j] = std::max(0.0f, pImpl->salienceValues[j] - suppression);
                }
            }
        }
    }
    
    // Select winners based on enhanced criteria
    float threshold = competitionThreshold_ * pImpl->globalInhibitionLevel;
    
    for (size_t i = 0; i < pImpl->neuronActivities.size(); ++i) {
        float finalStrength = pImpl->neuronActivities[i] * 
                            (1.0f + pImpl->salienceValues[i]) * 
                            (1.0f + pImpl->biasValues[i]);
        
        if (finalStrength >= threshold) {
            winners_.push_back(competitors[i]);
            pImpl->selectedWinners.push_back(competitors[i]);
            
            // Apply excitation to winners (boost their activity)
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 3.0f);
            }
            
            // Update attended regions based on winners
            if (competitors[i].getId() < 1000) {  // Assuming region IDs < 1000
                RegionId regionId = static_cast<RegionId>(competitors[i].getId() / 100);
                focusOnRegion(regionId);
            }
        }
    }
    
    // Decay attention over time
    update(pImpl->decayRate);
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
        
        // Amplify activity in attended region
        if (brain_) {
            for (auto& reg : brain_->getRegions()) {
                if (reg->getId() == region) {
                    for (auto* neuron : reg->getAllNeurons()) {
                        brain_->injectCurrent(neuron->getId(), attentionGain * 2.0f);
                    }
                    break;
                }
            }
        }
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    winners_.clear();
    pImpl->selectedWinners.clear();
    
    // Reset all neuron biases to zero (free attention)
    for (auto& pair : topDownBias_) {
        pair.second *= 0.1f;  // Strong decay
    }
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
    pImpl->globalInhibitionLevel = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
    pImpl->attentionGain = strength;
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
        return excitationStrength_ * 2.0f;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time for dynamic attention
    for (auto& pair : topDownBias_) {
        pair.second *= 0.95f;
    }
    
    for (auto& pair : bottomUpSalience_) {
        pair.second *= 0.9f;
    }
    
    // Decay neuron activities (forgetting)
    for (auto& activity : pImpl->neuronActivities) {
        activity *= 0.98f;
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
    pImpl->selectedWinners.clear();
    pImpl->neuronActivities.clear();
    pImpl->salienceValues.clear();
    pImpl->biasValues.clear();
    pImpl->neuronIds.clear();
}

// Helper methods for getting actual neural values
float AttentionalSelection::getNeuronActivity(NeuronId neuronId) const {
    if (!brain_) return 0.0f;
    
    // Search through all regions for this neuron
    for (const auto& region : brain_->getRegions()) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getId() == neuronId) {
                // Get membrane potential relative to resting potential
                const auto& state = neuron->getState();
                return std::abs(state.membranePotential - state.restingPotential) / 100.0f;
            }
        }
    }
    return 0.0f;
}

float AttentionalSelection::getNeuronSalience(NeuronId neuronId) const {
    // Get from salience map, decay over time
    auto it = bottomUpSalience_.find(neuronId.value);
    if (it != bottomUpSalience_.end()) {
        return std::min(1.0f, it->second);
    }
    
    // Default salience based on neuron activity if no history
    return getNeuronActivity(neuronId) * 0.5f;
}

float AttentionalSelection::getNeuronBias(NeuronId neuronId) const {
    // Get from bias map, decay over time
    auto it = topDownBias_.find(neuronId.value);
    if (it != topDownBias_.end()) {
        return std::min(1.0f, it->second);
    }
    return 0.0f;
}

} // namespace nlm
