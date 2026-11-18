#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Internal) {
                    for (Neuron* n : pop->getNeurons()) {
                        internalNeurons_.push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

// Memory system access methods

NeuralWorkingMemory* AgentBrain::getWorkingMemory() {
    return brain_ && brain_->getWorkingMemory() ? brain_->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* AgentBrain::getEpisodicMemory() {
    return brain_ && brain_->getEpisodicMemory() ? brain_->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* AgentBrain::getAssociativeMemory() {
    return brain_ && brain_->getAssociativeMemory() ? brain_->getAssociativeMemory() : nullptr;
}

// Phase 6 text encoder: a teacher word becomes a sparse sensory
// pattern, never a string the brain can match on. Lower-case, trim,
// FNV-1-a hash, then splitmix to pick 4 distinct neurons out of 16
// dedicated text neurons. Different words get different (overlapping,
// as in real sensory codes) patterns; the same word always gets the
// same pattern, which is what makes paired learning possible at all.
// The pattern is injected on sensoryVision_ neurons 16..30 — the
// tail vision neurons that vision itself never uses (vision is 16
// rays, the vision group has 31 neurons), so text has its own
// channel and cannot masquerade as a visual scene.
std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        // splitmix64
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    rates.reserve(internalNeurons_.size());
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    return rates;
}

std::vector<float> AgentBrain::conceptPattern() const {
    std::vector<float> pat;
    for (float v : lastVision_) pat.push_back(1.0f - v);
    for (float t : lastTouch_) pat.push_back(t);
    return pat;
}

size_t AgentBrain::getConceptCount() const {
    return brain_ && brain_->getConceptFormation() ? brain_->getConceptFormation()->getConceptCount() : 0;
}

float AgentBrain::getSelfModelConfidence() const {
    if (!selfModelReady_) return 0.0f;
    float sum = 0.0f;
    int n = 0;
    for (int a = 0; a < 6; ++a) {
        float c = selfModel_.getSelfModelConfidence(static_cast<ActionType>(a));
        if (c > 0.0f) { sum += c; ++n; }
    }
    return n ? sum / n : 0.0f;
}

size_t AgentBrain::getSocialObservationCount() const {
    return socialReady_ ? socialLearning_.getObservationCount() : 0;
}

// Add other method implementations here as needed...

} // namespace nlm
