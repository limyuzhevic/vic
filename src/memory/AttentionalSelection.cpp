#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    class Brain* brain;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
    
    // Neuromodulation state
    float achLevel_;
    float neArousalLevel_;
    float neVigilanceLevel_;
    float serotoninLevel_;
    
    Impl()
        : brain(nullptr)
        , inhibitionStrength_(0.5f)
        , excitationStrength_(1.0f)
        , competitionThreshold_(0.5f)
        , achLevel_(0.0f)
        , neArousalLevel_(0.0f)
        , neVigilanceLevel_(0.5f)
        , serotoninLevel_(0.5f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("AttentionalSelection system initialized");
}

void AttentionalSelection::setAChLevel(float level) {
    pImpl->achLevel_ = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG("ACh attention level set to " + std::to_string(pImpl->achLevel_));
}

void AttentionalSelection::setNELevel(float arousal, float vigilance) {
    pImpl->neArousalLevel_ = std::clamp(arousal, 0.0f, 1.0f);
    pImpl->neVigilanceLevel_ = std::clamp(vigilance, 0.0f, 1.0f);
    NLM_LOG_DEBUG("NE arousal level set to " + std::to_string(pImpl->neArousalLevel_) + ", vigilance: " + std::to_string(pImpl->neVigilanceLevel_));
}

void AttentionalSelection::setSerotoninLevel(float level) {
    pImpl->serotoninLevel_ = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Serotonin (5-HT) level set to " + std::to_string(pImpl->serotoninLevel_));
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                            float globalInhibition) {
    // Adjust competition based on neuromodulators
    float achBoost = 1.0f + pImpl->achLevel_ * 0.5f;  // ACh enhances competition focus
    float neBoost = 1.0f + pImpl->neArousalLevel_ * 0.3f;  // NE increases vigilance
    float serotoninMod = 1.0f - pImpl->serotoninLevel_ * 0.3f;  // Serotonin reduces impulsivity
    
    // Calculate effective salience based on neuromodulatory state
    std::vector<float> effectiveSalience;
    effectiveSalience.reserve(competitors.size());
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        float baseSalience = neuronSalience_[i];
        
        // Apply neuromodulatory effects
        float neuromodMod = achBoost * neBoost * serotoninMod;
        
        float effectiveSal = baseSalience * neuromodMod;
        
        // Apply top-down bias
        auto biasIt = pImpl->topDownBias_.find(competitors[i].index());
        if (biasIt != pImpl->topDownBias_.end()) {
            effectiveSal *= (1.0f + biasIt->second);
        }
        
        // Apply bottom-up salience
        auto salienceIt = pImpl->bottomUpSalience_.find(competitors[i].index());
        if (salienceIt != pImpl->bottomUpSalience_.end()) {
            effectiveSal *= (1.0f + salienceIt->second);
        }
        
        // Apply global inhibition
        effectiveSal *= (1.0f - globalInhibition);
        
        effectiveSalience.push_back(effectiveSal);
    }
    
    // Select winners based on effective salience
    pImpl->winners_.clear();
    
    float threshold = competitionThreshold_ * pImpl->neVigilanceLevel_; // NE affects threshold
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (effectiveSalience[i] > threshold) {
            pImpl->winners_.push_back(competitors[i]);
        }
    }
    
    // If no winners, select top competitor
    if (pImpl->winners_.empty() && !effectiveSalience.empty()) {
        size_t maxIndex = std::max_element(effectiveSalience.begin(), effectiveSalience.end()) - effectiveSalience.begin();
        if (maxIndex < competitors.size()) {
            pImpl->winners_.push_back(competitors[maxIndex]);
        }
    }
    }
    
    return pImpl->winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    pImpl->attendedRegions_.clear();
    pImpl->attendedRegions_.push_back(region);
    
    // Boost attention to this region using neuromodulators
    float attentionBoost = 1.0f + pImpl->achLevel_ * 0.8f; // ACh strongly amplifies focused attention
    NLM_LOG_DEBUG("Attention focused on region " + std::to_string(region.index()) + " (boost: " + std::to_string(attentionBoost) + ")");
}

void AttentionalSelection::releaseAttention() {
    pImpl->attendedRegions_.clear();
    NLM_LOG_DEBUG("Attention released from all regions");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return pImpl->attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength_ = std::clamp(strength, 0.0f, 1.0f);
}

void AttentionalSelection::setExcitationStrength(float strength) {
    pImpl->excitationStrength_ = std::clamp(strength, 0.0f, 1.0f);
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    // Apply neuromodulatory modulation to inhibition
    float baseInhibition = pImpl->inhibitionStrength_;
    float achMod = 1.0f - pImpl->achLevel_ * 0.3f;  // ACh reduces global inhibition
    float neMod = 1.0f + pImpl->neArousalLevel_ * 0.2f;  // NE increases focused inhibition
    
    return baseInhibition * achMod * neMod;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    // Apply neuromodulatory modulation to excitation
    float baseExcitation = pImpl->excitationStrength_;
    float achMod = 1.0f + pImpl->achLevel_ * 0.5f;  // ACh enhances excitation for attended items
    float neMod = 1.0f + pImpl->neVigilanceLevel_ * 0.3f;  // NE enhances signal detection
    float serotoninMod = 1.0f + pImpl->serotoninLevel_ * 0.1f;  // Serotonin stabilizes excitation
    
    return baseExcitation * achMod * neMod * serotoninMod;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Update attention dynamics based on neuromodulatory state
    
    // ACh maintains focused attention
    if (pImpl->achLevel_ > 0.0f) {
        // ACh promotes sustained attention
        for (auto& bias : pImpl->topDownBias_) {
            bias.second = std::min(1.0f, bias.second + 0.01f * static_cast<float>(dt));
        }
    }
    
    // NE regulates arousal and vigilance
    pImpl->neVigilanceLevel_ = std::max(0.1f, pImpl->neVigilanceLevel_ - 0.05f * static_cast<float>(dt));
    
    // Serotonin regulates emotional state and impulsivity
    if (pImpl->serotoninLevel_ > 0.5f) {
        // High serotonin promotes stable attention
        pImpl->competitionThreshold_ = std::min(0.8f, pImpl->competitionThreshold_ + 0.01f * static_cast<float>(dt));
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(pImpl->winners_.begin(), pImpl->winners_.end(), neuron) != pImpl->winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    pImpl->topDownBias_[neuron.index()] = std::max(pImpl->topDownBias_[neuron.index()], biasStrength);
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    pImpl->bottomUpSalience_[neuron.index()] = std::max(pImpl->bottomUpSalience_[neuron.index()], salienceStrength);
}

void AttentionalSelection::reset() {
    pImpl->winners_.clear();
    pImpl->topDownBias_.clear();
    pImpl->bottomUpSalience_.clear();
    pImpl->attendedRegions_.clear();
    NLM_LOG_DEBUG("AttentionalSelection reset");
}

} // namespace nlm
