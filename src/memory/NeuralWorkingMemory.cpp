#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// AttentionalSelection Implementation
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

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

// NeuralWorkingMemory Integration Implementations

void NeuralWorkingMemory::integrateWithAttention() {
    if (!brain_) return;
    
    auto* attention = brain_->getAttention();
    if (!attention) return;
    
    // Working memory strengths modulate attention via competition
    std::vector<NeuronId> memoryNeurons = getMemoryNeurons();
    attention->processCompetition(memoryNeurons, 0.5f - getMemoryActivity() * 0.3f);
    
    // Attention modulates working memory encoding
    float attentionStrength = attention->getMemoryActivity() ? 1.0f : 0.0f;
    
    // Enhance encoding when attention is high
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        memoryActivations_[i] *= (1.0f + attentionStrength * 0.5f);
    }
}

void NeuralWorkingMemory::updateAttentionThroughMemory() {
    if (!brain_) return;
    
    auto* attention = brain_->getAttention();
    if (!attention) return;
    
    // Use working memory strength to modulate attention parameters
    float memoryActivity = getMemoryActivity();
    
    if (memoryActivity > 0.7f) {
        attention->setInhibitionStrength(0.3f);  // Reduce inhibition for high memory activity
        attention->setExcitationStrength(2.0f);  // Enhance excitation
    } else if (memoryActivity < 0.3f) {
        attention->setInhibitionStrength(0.7f);  // Increase inhibition for low memory activity
        attention->setExcitationStrength(1.0f);  // Reduce excitation
    }
}

void NeuralWorkingMemory::integrateWithNeuromodulation() {
    // All neuromodulators influence working memory
    if (brain_) {
        auto* dopamine = brain_->getDopamine();
        auto* novelty = brain_->getNovelty();
        auto* acetylcholine = brain_->getAcetylcholine();
        auto* norepinephrine = brain_->getNorepinephrine();
        auto* serotonin = brain_->getSerotonin();
        
        // Dopamine controls decay rate, enhances retention
        if (dopamine) {
            setDecayRate(0.01f + 0.09f * dopamine->getLevel());
            strengthenMemory(1.0f + dopamine->getLevel() * 0.5f);
        }
        
        // Novelty enhances encoding and capacity
        if (novelty) {
            size_t currentCapacity = getCapacity();
            setCapacity(currentCapacity + static_cast<size_t>(novelty->getLevel() * 10));
            
            // Novelty also boosts current encoding
            for (auto& activation : memoryActivations_) {
                activation *= (1.0f + novelty->getLevel() * 0.3f);
            }
        }
        
        // Acetylcholine modulates attention and encoding strength
        if (acetylcholine) {
            float attentionGain = acetylcholine->getLevel() * 2.0f;
            float encodingGain = 1.0f + acetylcholine->getLevel() * 0.5f;
            
            // Apply attention and encoding gains
            if (brain_->getAttention()) {
                brain_->getAttention()->setInhibitionStrength(0.5f - attentionGain * 0.3f);
                brain_->getAttention()->setExcitationStrength(1.5f + attentionGain * 0.5f);
            }
            
            for (auto& activation : memoryActivations_) {
                activation *= encodingGain;
            }
        }
        
        // Norepinephrine regulates arousal and competition threshold
        if (norepinephrine) {
            float arousalLevel = norepinephrine->getLevel();
            
            // Set competition threshold based on arousal
            if (brain_->getAttention()) {
                brain_->getAttention()->setCompetitionThreshold(0.3f + arousalLevel * 0.4f);
            }
            
            // Enhance competition when arousal is high
            if (arousalLevel > 0.5f) {
                runCompetition();
            }
        }
        
        // Serotonin modulates reward prediction error for memory consolidation
        if (serotonin) {
            // Use prediction error from dopamine as a proxy for serotonin signaling
            if (dopamine) {
                float predictionError = std::abs(dopamine->getLevel() - 0.5f);
                serotonin->modulateRewardPredictionError(predictionError);
                
                // Consolidate memory based on serotonin level
                if (predictionError > 0.3f) {
                    float consolidationStrength = serotonin->getLevel() * 2.0f;
                    strengthenMemory(consolidationStrength);
                }
            }
        }
    }
}

void NeuralWorkingMemory::integrateWithSensory(const std::vector<float>& sensoryInput) {
    if (!brain_ || sensoryInput.empty()) return;
    
    // Sensory inputs directly feed into working memory storage
    std::vector<NeuronId> sensoryNeurons = memoryNeurons_;
    
    for (size_t i = 0; i < std::min(sensoryInput.size(), sensoryNeurons.size()); ++i) {
        float sensoryValue = sensoryInput[i];
        
        if (sensoryValue > 0.5f) {
            // Strong sensory input -> store in working memory
            storeToNeuron(sensoryNeurons[i], sensoryValue);
        }
    }
}

void NeuralWorkingMemory::updateSensoryFromMemory() {
    if (!brain_) return;
    
    // Working memory modulates sensory processing
    float memoryActivity = getMemoryActivity();
    
    // High memory activity enhances sensory gain
    if (memoryActivity > 0.6f) {
        // Increase sensory neuron excitability
        for (auto* neuron : brain_->getSensoryNeurons()) {
            neuron->injectCurrent(0.5f);
        }
    } else if (memoryActivity < 0.2f) {
        // Low memory activity reduces sensory gain
        for (auto* neuron : brain_->getSensoryNeurons()) {
            neuron->injectCurrent(-0.2f);
        }
    }
}

void NeuralWorkingMemory::integrateWithPrediction() {
    if (!brain_) return;
    
    auto* predictionSystem = brain_->getPredictionSystem();
    auto* predictionError = brain_->getPredictionErrorSignal();
    
    if (!predictionSystem || !predictionError) return;
    
    // Prediction error signals modulate working memory consolidation
    float predError = predictionError->getMagnitude();
    
    if (predError > 0.5f) {
        // High prediction error -> enhance memory consolidation
        strengthenMemory(1.5f + predError * 0.5f);
        
        // Also boost capacity temporarily
        size_t originalCapacity = getCapacity();
        setCapacity(originalCapacity + static_cast<size_t>(predError * 5));
    }
    
    // Working memory informs prediction system
    std::vector<float> memoryPattern = retrieve();
    
    // Update prediction system with current working memory state
    // In a real implementation, this would update the prediction model
}

void NeuralWorkingMemory::updatePredictionFromMemory() {
    if (!brain_) return;
    
    auto* predictionSystem = brain_->getPredictionSystem();
    auto* predictionError = brain_->getPredictionErrorSignal();
    
    if (!predictionSystem || !predictionError) return;
    
    // Working memory activity updates prediction error
    float memoryActivity = getMemoryActivity();
    
    // Use memory activity as prediction error signal
    // Higher memory activity suggests prediction is correct
    float prediction = std::min(1.0f, memoryActivity * 2.0f);
    
    // Compute prediction error between memory and expected activity
    predictionError->computeError(prediction, memoryActivity);
}

void NeuralWorkingMemory::modulateCapacity() {
    if (!brain_) return;
    
    // Modulate capacity based on neuromodulators
    size_t baseCapacity = 100;
    
    // Dopamine: increases retention and capacity
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        float dopamineFactor = 1.0f + dopamine->getLevel() * 0.5f;
        baseCapacity = static_cast<size_t>(baseCapacity * dopamineFactor);
    }
    
    // Novelty: enhances capacity
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        float noveltyFactor = 1.0f + novelty->getLevel() * 0.3f;
        baseCapacity = static_cast<size_t>(baseCapacity * noveltyFactor);
    }
    
    // Acetylcholine: modulates encoding and capacity
    auto* acetylcholine = brain_->getAcetylcholine();
    if (acetylcholine) {
        float achFactor = 1.0f + acetylcholine->getLevel() * 0.2f;
        baseCapacity = static_cast<size_t>(baseCapacity * achFactor);
    }
    
    // Norepinephrine: affects capacity through arousal
    auto* norepinephrine = brain_->getNorepinephrine();
    if (norepinephrine) {
        float neFactor = 1.0f + norepinephrine->getLevel() * 0.4f;
        baseCapacity = static_cast<size_t>(baseCapacity * neFactor);
    }
    
    // Ensure capacity doesn't exceed reasonable limits
    baseCapacity = std::min(baseCapacity, 500U);
    
    setCapacity(baseCapacity);
}

void NeuralWorkingMemory::modulateDecayRate() {
    if (!brain_) return;
    
    // Modulate decay rate based on neuromodulators
    float baseDecayRate = 0.01f;
    
    // Dopamine: controls decay rate, enhances retention
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        baseDecayRate = 0.01f + 0.09f * dopamine->getLevel();
    }
    
    // Serotonin: modulates reward prediction error for memory consolidation
    auto* serotonin = brain_->getSerotonin();
    if (serotonin) {
        baseDecayRate *= (1.0f + serotonin->getLevel() * 0.3f);
    }
    
    // Novelty: enhances retention
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        baseDecayRate *= (1.0f - novelty->getLevel() * 0.2f);  // Novelty reduces decay
    }
    
    setDecayRate(baseDecayRate);
}

void NeuralWorkingMemory::modulateEncodingStrength() {
    if (!brain_) return;
    
    // Modulate encoding strength based on neuromodulators
    float baseEncodingStrength = 1.0f;
    
    // Acetylcholine: modulates encoding strength
    auto* acetylcholine = brain_->getAcetylcholine();
    if (acetylcholine) {
        baseEncodingStrength *= (1.0f + acetylcholine->getLevel() * 0.5f);
    }
    
    // Novelty: enhances encoding
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        baseEncodingStrength *= (1.0f + novelty->getLevel() * 0.3f);
    }
    
    // Norepinephrine: affects encoding through arousal
    auto* norepinephrine = brain_->getNorepinephrine();
    if (norepinephrine) {
        baseEncodingStrength *= (1.0f + norepinephrine->getLevel() * 0.4f);
    }
    
    // Apply encoding strength to current memory traces
    for (auto& activation : memoryActivations_) {
        activation *= baseEncodingStrength;
    }
}

void NeuralWorkingMemory::applyNeuromodulation() {
    // Apply all neuromodulatory effects
    modulateCapacity();
    modulateDecayRate();
    modulateEncodingStrength();
    
    // Run competition affected by neuromodulators
    runCompetition();
}

void NeuralWorkingMemory::setDopamineLevel(float level) {
    if (brain_ && brain_->getDopamine()) {
        brain_->getDopamine()->setLevel(level);
    }
}

void NeuralWorkingMemory::setNoveltyLevel(float level) {
    if (brain_ && brain_->getNovelty()) {
        brain_->getNovelty()->setLevel(level);
    }
}

void NeuralWorkingMemory::setAcetylcholineLevel(float level) {
    if (brain_ && brain_->getAcetylcholine()) {
        brain_->getAcetylcholine()->setLevel(level);
    }
}

void NeuralWorkingMemory::setNorepinephrineLevel(float level) {
    if (brain_ && brain_->getNorepinephrine()) {
        brain_->getNorepinephrine()->setLevel(level);
    }
}

void NeuralWorkingMemory::setSerotoninLevel(float level) {
    if (brain_ && brain_->getSerotonin()) {
        brain_->getSerotonin()->setLevel(level);
    }
}

float NeuralWorkingMemory::getDopamineLevel() const {
    if (brain_ && brain_->getDopamine()) {
        return brain_->getDopamine()->getLevel();
    }
    return 0.0f;
}

float NeuralWorkingMemory::getNoveltyLevel() const {
    if (brain_ && brain_->getNovelty()) {
        return brain_->getNovelty()->getLevel();
    }
    return 0.0f;
}

float NeuralWorkingMemory::getAcetylcholineLevel() const {
    if (brain_ && brain_->getAcetylcholine()) {
        return brain_->getAcetylcholine()->getLevel();
    }
    return 0.0f;
}

float NeuralWorkingMemory::getNorepinephrineLevel() const {
    if (brain_ && brain_->getNorepinephrine()) {
        return brain_->getNorepinephrine()->getLevel();
    }
    return 0.0f;
}

float NeuralWorkingMemory::getSerotoninLevel() const {
    if (brain_ && brain_->getSerotonin()) {
        return brain_->getSerotonin()->getLevel();
    }
    return 0.0f;
}

} // namespace nlm