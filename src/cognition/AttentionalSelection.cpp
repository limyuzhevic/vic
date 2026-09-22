// AttentionalSelection Implementation
// Phase 6: Neural Attention System
// Implements competitive attention using lateral inhibition and gain modulation
// NOT Transformer attention - uses biologically inspired mechanisms

#include "AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>

namespace nlm {

struct AttentionalSelection::Impl {
    // Competition and selection dynamics
    static constexpr float GLOBAL_INHIBITION_BASE = 0.5f;     // Base global inhibition
    static constexpr float EXCITATION_THRESHOLD = 0.8f;      // Threshold for excitation
    static constexpr float INHIBITION_DECAY = 0.95f;           // Decay rate of inhibition
    static constexpr float SALIENCE_GAIN = 2.0f;               // Gain for bottom-up salience
    static constexpr float BIAS_GAIN = 1.5f;                   // Gain for top-down bias
    static constexpr float SPATIAL_INHIBITION_RADIUS = 5.0f;   // Spatial influence radius
    static constexpr float PRIORITY_UPDATE_RATE = 0.1f;        // Rate of priority updates
    
    // State tracking
    std::vector<float> neuronPriorities;        // Current attention priority per neuron
    std::vector<float> neuronInhibition;       // Inhibition level per neuron
    std::vector<float> neuronExcitation;       // Excitation level per neuron
    std::vector<RegionId> neuronRegions;        // Region assignment for spatial effects
    
    // Winner selection state
    std::vector<NeuronId> currentWinners;      // Current attended neurons
    std::vector<float> winnerStrengths;         // Strength of current winners
    
    // Salience and bias storage
    std::unordered_map<uint64_t, float> neuronSalience;    // Bottom-up salience per neuron
    std::unordered_map<uint64_t, float> neuronBias;        // Top-down bias per neuron
    
    // Time integration
    float timeSinceLastUpdate;                    // Time since last attention update
    
    Impl() : timeSinceLastUpdate(0.0f) {
        // Initialize neuron arrays (will be resized as neurons are added)
    }
};

AttentionalSelection::AttentionalSelection() : pImpl(std::make_unique<Impl>()) {
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize with empty state
    pImpl->neuronPriorities.clear();
    pImpl->neuronInhibition.clear();
    pImpl->neuronExcitation.clear();
    pImpl->neuronRegions.clear();
    pImpl->currentWinners.clear();
    pImpl->winnerStrengths.clear();
    
    // Set default parameters
    pImpl->inhibitionStrength_ = GLOBAL_INHIBITION_BASE;
    pImpl->excitationStrength_ = 1.0f;
    pImpl->competitionThreshold_ = EXCITATION_THRESHOLD;
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                                float globalInhibition) {
    // Process competition among neural populations and select winners
    if (competitors.empty() || !brain_) {
        return {};
    }
    
    // Update neuron state for competitors
    updateNeuronStates(competitors);
    
    // Calculate competition scores
    std::vector<float> scores;
    scores.reserve(competitors.size());
    
    for (const auto& neuronId : competitors) {
        float score = calculateCompetitionScore(neuronId);
        scores.push_back(score);
    }
    
    // Select winners using competitive process
    selectWinners(scores);
    
    // Return current winners
    return pImpl->currentWinners;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    // Apply attention to a specific brain region
    if (!brain_) {
        return;
    }
    
    // Get all neurons in target region
    auto* targetRegion = brain_->getRegion(region);
    if (!targetRegion) {
        return;
    }
    
    // Find neurons in this region
    for (const auto& pop : targetRegion->getPopulations()) {
        for (auto* neuron : pop->getNeurons()) {
            if (neuron) {
                // Boost priority for neurons in this region
                pImpl->neuronPriorities[neuron->getId().index()] *= 2.0f;
                
                // Set as attended
                pImpl->neuronInhibition[neuron->getId().index()] = 0.0f;  // No inhibition
                pImpl->neuronExcitation[neuron->getId().index()] = 1.5f;  // High excitation
            }
        }
    }
    
    // Update attended regions
    pImpl->attendedRegions_.push_back(region);
    
    NLM_LOG_INFO("Attention focused on region " + std::to_string(region.index()));
}

void AttentionalSelection::releaseAttention() {
    // Release attention - allow all regions equal processing
    // Reset inhibition and excitation for all neurons
    for (size_t i = 0; i < pImpl->neuronInhibition.size(); ++i) {
        pImpl->neuronInhibition[i] = GLOBAL_INHIBITION_BASE;
        pImpl->neuronExcitation[i] = 1.0f;
    }
    
    pImpl->attendedRegions_.clear();
    pImpl->currentWinners.clear();
    
    NLM_LOG_INFO("Attention released - all regions equal");
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return pImpl->attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength_ = std::max(0.0f, std::min(1.0f, strength));
    
    // Update neuron inhibition levels
    for (size_t i = 0; i < pImpl->neuronInhibition.size(); ++i) {
        pImpl->neuronInhibition[i] = pImpl->inhibitionStrength_;
    }
    
    NLM_LOG_INFO(std::string("Inhibition strength set to ") + std::to_string(pImpl->inhibitionStrength_));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    pImpl->excitationStrength_ = std::max(0.0f, std::min(2.0f, strength));
    
    // Update neuron excitation levels
    for (size_t i = 0; i < pImpl->neuronExcitation.size(); ++i) {
        pImpl->neuronExcitation[i] = pImpl->excitationStrength_;
    }
    
    NLM_LOG_INFO(std::string("Excitation strength set to ") + std::to_string(pImpl->excitationStrength_));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
    NLM_LOG_INFO(std::string("Competition threshold set to ") + std::to_string(pImpl->competitionThreshold_));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    if (neuron.index() >= pImpl->neuronInhibition.size()) {
        return GLOBAL_INHIBITION_BASE;
    }
    return pImpl->neuronInhibition[neuron.index()];
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    if (neuron.index() >= pImpl->neuronExcitation.size()) {
        return 1.0f;
    }
    return pImpl->neuronExcitation[neuron.index()];
}

void AttentionalSelection::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate += dt;
    
    // Decay inhibition levels
    for (size_t i = 0; i < pImpl->neuronInhibition.size(); ++i) {
        pImpl->neuronInhibition[i] *= INHIBITION_DECAY;
    }
    
    // Update neuron priorities based on competition
    updateNeuronPriorities();
    
    // Apply spatial inhibition effects
    applySpatialInhibition();
    
    // Update attention winners
    selectWinners();
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(pImpl->currentWinners.begin(), pImpl->currentWinners.end(), neuron) != pImpl->currentWinners.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    if (biasStrength != 0.0f) {
        pImpl->neuronBias[neuron.index()] += biasStrength * BIAS_GAIN;
        
        // Update neuron priority
        if (neuron.index() < pImpl->neuronPriorities.size()) {
            pImpl->neuronPriorities[neuron.index()] *= (1.0f + biasStrength);
        }
        
        NLM_LOG_INFO(std::string("Top-down bias applied to neuron ") + 
                    std::to_string(neuron.index()) + std::string(": ") + 
                    std::to_string(biasStrength));
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    if (salienceStrength != 0.0f) {
        pImpl->neuronSalience[neuron.index()] = salienceStrength * SALIENCE_GAIN;
        
        // Update neuron priority based on salience
        if (neuron.index() < pImpl->neuronPriorities.size()) {
            pImpl->neuronPriorities[neuron.index()] *= (1.0f + salienceStrength);
        }
        
        NLM_LOG_INFO(std::string("Bottom-up salience applied to neuron ") + 
                    std::to_string(neuron.index()) + std::string(": ") + 
                    std::to_string(salienceStrength));
    }
}

void AttentionalSelection::reset() {
    // Reset all attention state
    pImpl->neuronPriorities.clear();
    pImpl->neuronInhibition.clear();
    pImpl->neuronExcitation.clear();
    pImpl->neuronRegions.clear();
    pImpl->neuronSalience.clear();
    pImpl->neuronBias.clear();
    
    pImpl->currentWinners.clear();
    pImpl->winnerStrengths.clear();
    pImpl->attendedRegions_.clear();
    
    // Reset parameters
    pImpl->inhibitionStrength_ = GLOBAL_INHIBITION_BASE;
    pImpl->excitationStrength_ = 1.0f;
    pImpl->competitionThreshold_ = EXCITATION_THRESHOLD;
    
    NLM_LOG_INFO("AttentionalSelection reset");
}

void AttentionalSelection::updateNeuronStates(const std::vector<NeuronId>& competitors) {
    // Update internal state for competing neurons
    for (const auto& neuronId : competitors) {
        // Ensure neuron state arrays are large enough
        size_t idx = neuronId.index();
        if (idx >= pImpl->neuronPriorities.size()) {
            pImpl->neuronPriorities.resize(idx + 1, 0.0f);
            pImpl->neuronInhibition.resize(idx + 1, GLOBAL_INHIBITION_BASE);
            pImpl->neuronExcitation.resize(idx + 1, 1.0f);
        }
        
        // Get region for spatial effects
        pImpl->neuronRegions.resize(idx + 1);
    }
}

void AttentionalSelection::updateNeuronPriorities() {
    // Update attention priorities based on current state
    for (size_t i = 0; i < pImpl->neuronPriorities.size(); ++i) {
        // Base priority on current excitation and salience
        float basePriority = pImpl->neuronExcitation[i];
        
        // Add top-down bias
        auto biasIt = pImpl->neuronBias.find(i);
        if (biasIt != pImpl->neuronBias.end()) {
            basePriority *= (1.0f + biasIt->second);
        }
        
        // Add bottom-up salience
        auto salienceIt = pImpl->neuronSalience.find(i);
        if (salienceIt != pImpl->neuronSalience.end()) {
            basePriority *= (1.0f + salienceIt->second);
        }
        
        // Apply global inhibition
        basePriority *= (1.0f - pImpl->neuronInhibition[i]);
        
        // Time-based decay
        basePriority *= std::pow(0.99f, pImpl->timeSinceLastUpdate);
        
        // Clamp to valid range
        pImpl->neuronPriorities[i] = std::max(0.0f, std::min(10.0f, basePriority));
    }
}

void AttentionalSelection::applySpatialInhibition() {
    // Apply spatial inhibition effects between neurons in same region
    // Neurons in same region inhibit each other
    
    // Group neurons by region
    std::unordered_map<RegionId, std::vector<size_t>> regionToNeuronIndices;
    
    for (size_t i = 0; i < pImpl->neuronRegions.size(); ++i) {
        if (pImpl->neuronRegions[i].index() != 0) {  // Valid region
            regionToNeuronIndices[pImpl->neuronRegions[i]].push_back(i);
        }
    }
    
    // Apply lateral inhibition within regions
    for (const auto& entry : regionToNeuronIndices) {
        const auto& neuronIndices = entry.second;
        if (neuronIndices.size() < 2) {
            continue;
        }
        
        // Find winner (highest priority)
        size_t winnerIdx = 0;
        float maxPriority = pImpl->neuronPriorities[neuronIndices[0]];
        
        for (size_t j = 1; j < neuronIndices.size(); ++j) {
            size_t idx = neuronIndices[j];
            if (pImpl->neuronPriorities[idx] > maxPriority) {
                maxPriority = pImpl->neuronPriorities[idx];
                winnerIdx = j;
            }
        }
        
        // Apply inhibitory surround: other neurons get reduced priority
        for (size_t j = 0; j < neuronIndices.size(); ++j) {
            if (j != winnerIdx) {
                size_t idx = neuronIndices[j];
                pImpl->neuronPriorities[idx] *= 0.5f;  // Inhibition
            }
        }
    }
}

float AttentionalSelection::calculateCompetitionScore(const NeuronId& neuron) const {
    // Calculate competition score for a neuron
    size_t idx = neuron.index();
    if (idx >= pImpl->neuronPriorities.size()) {
        return 0.0f;
    }
    
    // Base score on priority
    float score = pImpl->neuronPriorities[idx];
    
    // Apply competition threshold
    if (score < pImpl->competitionThreshold_) {
        score = 0.0f;
    }
    
    // Normalize based on excitation level
    if (pImpl->neuronExcitation[idx] > 0.0f) {
        score /= pImpl->neuronExcitation[idx];
    }
    
    return score;
}

void AttentionalSelection::selectWinners() {
    // Select winning neurons based on current priorities
    pImpl->currentWinners.clear();
    pImpl->winnerStrengths.clear();
    
    if (pImpl->neuronPriorities.empty()) {
        return;
    }
    
    // Calculate scores for all neurons
    std::vector<std::pair<size_t, float>> scoredNeurons;
    for (size_t i = 0; i < pImpl->neuronPriorities.size(); ++i) {
        float score = calculateCompetitionScore(NeuronId(static_cast<NeuronId::ValueType>(i)));
        if (score > 0.0f) {
            scoredNeurons.emplace_back(i, score);
        }
    }
    
    // Sort by score (highest first)
    std::sort(scoredNeurons.begin(), scoredNeurons.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Select top 1-3 winners based on score distribution
    float maxScore = scoredNeurons.empty() ? 0.0f : scoredNeurons[0].second;
    float threshold = maxScore * 0.7f;  // Select neurons with >70% of max score
    
    for (const auto& pair : scoredNeurons) {
        if (pair.second >= threshold && pImpl->currentWinners.size() < 3) {
            pImpl->currentWinners.push_back(NeuronId(static_cast<NeuronId::ValueType>(pair.first)));
            pImpl->winnerStrengths.push_back(pair.second / maxScore);
        }
    }
    
    // If no winners selected, pick highest scoring neuron
    if (pImpl->currentWinners.empty() && !scoredNeurons.empty()) {
        pImpl->currentWinners.push_back(NeuronId(static_cast<NeuronId::ValueType>(scoredNeurons[0].first)));
        pImpl->winnerStrengths.push_back(1.0f);
    }
}

std::string AttentionalSelection::getStats() const {
    std::ostringstream oss;
    oss << "=== AttentionalSelection Statistics ===\n";
    oss << "Total neurons tracked: " << pImpl->neuronPriorities.size() << "\n";
    oss << "Current winners: " << pImpl->currentWinners.size() << "\n";
    oss << "Inhibition strength: " << pImpl->inhibitionStrength_ << "\n";
    oss << "Excitation strength: " << pImpl->excitationStrength_ << "\n";
    oss << "Competition threshold: " << pImpl->competitionThreshold_ << "\n";
    
    if (!pImpl->currentWinners.empty()) {
        oss << "Winner priorities: ";
        for (float strength : pImpl->winnerStrengths) {
            oss << strength << " ";
        }
        oss << "\n";
    }
    
    float avgPriority = 0.0f;
    if (!pImpl->neuronPriorities.empty()) {
        for (float priority : pImpl->neuronPriorities) {
            avgPriority += priority;
        }
        avgPriority /= pImpl->neuronPriorities.size();
    }
    
    oss << "Average neuron priority: " << avgPriority << "\n";
    oss << "Attended regions: " << pImpl->attendedRegions_.size() << "\n";
    
    return oss.str();
}

} // namespace nlm
