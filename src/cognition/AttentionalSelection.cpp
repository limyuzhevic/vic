#include "AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AttentionalSelection::Impl {
    Brain* brain;
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
    float topDownGain;
    float bottomUpGain;
    float globalInhibition;
    std::vector<NeuronId> winners;
    std::vector<RegionId> attendedRegions;
    std::vector<float> neuralActivations;
    std::vector<float> attentionWeights;
    
    Impl() : brain(nullptr)
        , inhibitionStrength(0.5f)
        , excitationStrength(1.5f)
        , competitionThreshold(0.3f)
        , topDownGain(0.8f)
        , bottomUpGain(1.2f)
        , globalInhibition(0.5f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new Impl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->winners.clear();
    pImpl->attendedRegions.clear();
    
    // Initialize neural activation tracking
    if (brain) {
        size_t totalNeurons = brain->getTotalNeuronCount();
        pImpl->neuralActivations.resize(totalNeurons, 0.0f);
        pImpl->attentionWeights.resize(totalNeurons, 1.0f);
    }
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (!pImpl->brain) return;
    
    auto* regionPtr = pImpl->brain->getRegion(region);
    if (!regionPtr) return;
    
    pImpl->attendedRegions.clear();
    pImpl->attendedRegions.push_back(region);
    
    // Boost attention weights for neurons in this region
    for (auto& region : pImpl->brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            pImpl->attentionWeights[neuron->getId()] *= pImpl->topDownGain;
        }
    }
}

void AttentionalSelection::releaseAttention() {
    pImpl->attendedRegions.clear();
    
    // Reset attention weights
    for (auto& weight : pImpl->attentionWeights) {
        weight = 1.0f;
    }
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return pImpl->attendedRegions;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = std::max(0.0f, std::min(1.0f, strength));
}

void AttentionalSelection::setExcitationStrength(float strength) {
    pImpl->excitationStrength = std::max(0.0f, std::min(2.0f, strength));
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold = std::max(0.0f, std::min(1.0f, threshold));
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    if (neuron.index() >= pImpl->neuralActivations.size()) {
        return 0.0f;
    }
    return pImpl->inhibitionStrength * pImpl->neuralActivations[neuron.index()];
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    if (neuron.index() >= pImpl->attentionWeights.size()) {
        return 0.0f;
    }
    return pImpl->excitationStrength * pImpl->attentionWeights[neuron.index()];
}

void AttentionalSelection::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update neural activations based on recent activity
    updateNeuralActivations();
    
    // Apply competition and select winners
    selectAttentionalWinners();
    
    // Update attention dynamics
    pImpl->globalInhibition = std::max(0.2f, std::min(0.8f, pImpl->globalInhibition + 0.01f * dt));
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(pImpl->winners.begin(), pImpl->winners.end(), neuron) != pImpl->winners.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    pImpl->attentionWeights[neuron.index()] *= (1.0f + pImpl->topDownGain * biasStrength);
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    pImpl->attentionWeights[neuron.index()] *= (1.0f + pImpl->bottomUpGain * salienceStrength);
}

void AttentionalSelection::integratePredictionWithSensory(const PredictionSystem& predictionSystem,
                                                           const SensoryInput& currentInput,
                                                           std::vector<NeuronId>& attendedNeurons) {
    if (!pImpl->brain) return;
    
    // Get prediction error to modulate attention
    float predictionError = predictionSystem.getPredictionError();
    
    // High prediction error should increase attentional flexibility
    if (predictionError > pImpl->competitionThreshold) {
        // Increase global inhibition to allow more competition
        pImpl->globalInhibition *= 1.5f;
        
        // Apply bottom-up attention to neurons with high prediction error
        for (size_t i = 0; i < currentInput.getData().size() && i < pImpl->neuralActivations.size(); ++i) {
            float input = currentInput.getData()[i];
            if (std::abs(input) > 0.5f) {
                applyBottomUpSalience(NeuronId(i), std::abs(input));
            }
        }
    } else {
        // Normal attention focusing
        pImpl->globalInhibition = 0.5f;
    }
    
    // Select attended neurons based on integrated prediction and sensory input
    std::vector<NeuronId> sensoryNeurons = getSensoryNeurons();
    attendedNeurons = processCompetition(sensoryNeurons);
}

void AttentionalSelection::applyConceptFormation(const ConceptFormation& conceptFormation,
                                                 const std::vector<NeuronId>& attendedNeurons) {
    if (!pImpl->brain) return;
    
    // Get concept activation levels for attended neurons
    std::vector<float> conceptActivations;
    for (NeuronId neuronId : attendedNeurons) {
        float conceptActivation = conceptFormation.getConceptActivation(neuronId);
        conceptActivations.push_back(conceptActivation);
        
        // Boost attention for neurons with strong concept activation
        if (conceptActivation > 0.7f) {
            applyTopDownBias(neuronId, 0.5f);
        }
    }
    
    // Update attention weights based on concept consistency
    updateConceptBasedAttention(conceptActivations);
}

void AttentionalSelection::storeAttendedInWorkingMemory(NeuralWorkingMemory* workingMemory,
                                                         const std::vector<NeuronId>& attendedNeurons) {
    if (!workingMemory || attendedNeurons.empty()) return;
    
    // Store attended neuron IDs in working memory
    for (NeuronId neuronId : attendedNeurons) {
        workingMemory->storeToNeuron(neuronId, getExcitationFor(neuronId));
    }
}

void AttentionalSelection::updateNeuralActivations() {
    if (!pImpl->brain) return;
    
    // Update neural activations based on recent firing
    pImpl->neuralActivations.clear();
    pImpl->neuralActivations.resize(pImpl->brain->getTotalNeuronCount(), 0.0f);
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* neuron : region->getAllNeurons()) {
            size_t neuronId = neuron->getId().index();
            if (neuronId < pImpl->neuralActivations.size()) {
                // Calculate activation based on membrane potential
                float activation = std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) /
                                  (neuron->getThreshold() - neuron->getRestingPotential());
                pImpl->neuralActivations[neuronId] = std::min(1.0f, activation);
            }
        }
    }
}

std::vector<NeuronId> AttentionalSelection::getSensoryNeurons() {
    std::vector<NeuronId> sensoryNeurons;
    
    if (!pImpl->brain) return sensoryNeurons;
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory) {
                sensoryNeurons.push_back(neuron->getId());
            }
        }
    }
    
    return sensoryNeurons;
}

void AttentionalSelection::selectAttentionalWinners() {
    pImpl->winners.clear();
    
    if (pImpl->brain->getTotalNeuronCount() == 0) return;
    
    // Calculate competition scores
    std::vector<std::pair<float, NeuronId>> scores;
    scores.reserve(pImpl->brain->getTotalNeuronCount());
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* neuron : region->getAllNeurons()) {
            float inhibition = getInhibitionFor(neuron->getId());
            float excitation = getExcitationFor(neuron->getId());
            
            // Calculate net score with competition
            float score = excitation - inhibition * pImpl->globalInhibition;
            
            // Apply attention modulation
            size_t neuronId = neuron->getId().index();
            if (neuronId < pImpl->attentionWeights.size()) {
                score *= pImpl->attentionWeights[neuronId];
            }
            
            scores.emplace_back(score, neuron->getId());
        }
    }
    
    // Select top winners based on scores
    std::sort(scores.begin(), scores.end(),
              [](const std::pair<float, NeuronId>& a, const std::pair<float, NeuronId>& b) {
                  return a.first > b.first;
              });
    
    // Select winners above threshold
    for (const auto& score : scores) {
        if (score.first > pImpl->competitionThreshold && 
            pImpl->winners.size() < (pImpl->brain->getTotalNeuronCount() / 10)) {
            pImpl->winners.push_back(score.second);
        }
    }
    
    // Update attended regions based on winners
    pImpl->attendedRegions.clear();
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto* neuron : region->getAllNeurons()) {
            if (std::find(pImpl->winners.begin(), pImpl->winners.end(), neuron->getId()) != pImpl->winners.end()) {
                if (std::find(pImpl->attendedRegions.begin(), pImpl->attendedRegions.end(), region->getId()) == pImpl->attendedRegions.end()) {
                    pImpl->attendedRegions.push_back(region->getId());
                }
                break;
            }
        }
    }
}

void AttentionalSelection::updateConceptBasedAttention(const std::vector<float>& conceptActivations) {
    if (conceptActivations.empty()) return;
    
    // Calculate concept consistency
    float avgConcept = 0.0f;
    for (float activation : conceptActivations) {
        avgConcept += activation;
    }
    avgConcept /= conceptActivations.size();
    
    // If concepts are consistent, increase attention focus
    if (avgConcept > 0.7f) {
        // Strengthen top-down attention
        pImpl->topDownGain *= 1.2f;
        pImpl->globalInhibition *= 0.8f;
    } else {
        // Allow more exploration
        pImpl->topDownGain *= 0.9f;
        pImpl->globalInhibition *= 1.1f;
    }
}

} // namespace nlm