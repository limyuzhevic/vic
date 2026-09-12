#include "NeuralAttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAttentionalSelection::Impl {
    Brain* brain;
    
    // Neural competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    std::vector<float> neuronalActivation;
    
    Impl() : brain(nullptr) {}
};

NeuralAttentionalSelection::NeuralAttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
    // Initialize neural populations for competition
    neuronalActivation.reserve(100);
}

NeuralAttentionalSelection::~NeuralAttentionalSelection() = default;

void NeuralAttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with neural populations
    pImpl->neuronalActivation.clear();
    pImpl->competitionStrength.clear();
    pImpl->inhibitionLevel.clear();
    
    NLM_LOG_INFO("NeuralAttentionalSelection initialized");
}

std::vector<NeuronId> NeuralAttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                               float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) {
        return winners_;
    }
    
    // Initialize neural competition state
    size_t numCompetitors = competitors.size();
    pImpl->neuronalActivation.resize(numCompetitors, 0.0f);
    pImpl->competitionStrength.resize(numCompetitors, 0.0f);
    pImpl->inhibitionLevel.resize(numCompetitors, 0.0f);
    
    // Get current neural activations for competitor neurons
    for (size_t i = 0; i < numCompetitors; ++i) {
        NeuronId neuron = competitors[i];
        
        // Get actual neuron activation from brain
        if (brain_) {
            // Find the neuron and get its membrane potential
            auto* region = brain_->getRegion(RegionId(neuron.getId() / 1000));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (auto* n : neurons) {
                    if (n->getId() == neuron) {
                        // Normalize membrane potential to activation [0, 1]
                        float v = n->getMembranePotential();
                        float vRest = n->getRestingPotential();
                        pImpl->neuronalActivation[i] = std::max(0.0f, (v - vRest) / 50.0f);
                        break;
                    }
                }
            }
        }
        
        // Apply top-down bias
        auto biasIt = topDownBias_.find(neuron.value);
        float topDown = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        // Apply bottom-up salience
        auto salIt = bottomUpSalience_.find(neuron.value);
        float bottomUp = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        
        // Total input to neuron
        pImpl->competitionStrength[i] = pImpl->neuronalActivation[i] + topDown + bottomUp;
    }
    
    // Neural competition dynamics - lateral inhibition
    float totalInput = 0.0f;
    for (float strength : pImpl->competitionStrength) {
        totalInput += strength;
    }
    
    if (totalInput < 0.001f) {
        // No strong input - all neurons compete equally
        return competitors;
    }
    
    // Calculate normalized competition strengths
    std::vector<float> normalizedStrengths(numCompetitors);
    for (size_t i = 0; i < numCompetitors; ++i) {
        normalizedStrengths[i] = pImpl->competitionStrength[i] / totalInput;
    }
    
    // Apply lateral inhibition - strong neurons suppress weaker ones
    for (size_t i = 0; i < numCompetitors; ++i) {
        for (size_t j = 0; j < numCompetitors; ++j) {
            if (i == j) continue;
            
            // Stronger neurons inhibit weaker ones
            if (normalizedStrengths[i] > normalizedStrengths[j] + 0.1f) {
                // Calculate inhibition strength based on competition difference
                float inhibition = (normalizedStrengths[i] - normalizedStrengths[j]) * 
                                 globalInhibition * inhibitionStrength_;
                
                pImpl->inhibitionLevel[j] += inhibition;
                
                // Apply inhibition to the weaker neuron
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -inhibition * 5.0f);
                }
            }
        }
    }
    
    // Determine winners - neurons above competition threshold
    float threshold = competitionThreshold_ * totalInput / numCompetitors;
    
    for (size_t i = 0; i < numCompetitors; ++i) {
        // Final activation after inhibition
        float finalActivation = pImpl->competitionStrength[i] - pImpl->inhibitionLevel[i];
        
        if (finalActivation >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners (amplify winner response)
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 3.0f);
            }
        }
    }
    
    return winners_;
}

void NeuralAttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
        
        // Apply top-down bias to neurons in this region
        if (brain_) {
            auto* regionPtr = brain_->getRegion(region);
            if (regionPtr) {
                auto neurons = regionPtr->getAllNeurons();
                for (auto* neuron : neurons) {
                    applyTopDownBias( neuron->getId(), 2.0f);  // Strong bias
                }
            }
        }
    }
}

void NeuralAttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
}

std::vector<RegionId> NeuralAttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void NeuralAttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = std::max(0.0f, strength);
}

void NeuralAttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = std::max(0.0f, strength);
}

void NeuralAttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
}

float NeuralAttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float NeuralAttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void NeuralAttentionalSelection::update(TimestepDuration dt) {
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
    
    // Update neuronal activation dynamics
    for (float& act : pImpl->neuronalActivation) {
        act = std::max(0.0f, act - 0.05f * dt);  // Decay activation
    }
}

bool NeuralAttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void NeuralAttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void NeuralAttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void NeuralAttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
    pImpl->neuronalActivation.clear();
}

} // namespace nlm