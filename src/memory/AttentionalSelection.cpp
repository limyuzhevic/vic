#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

struct AttentionalSelection::Impl {
    // Prediction integration
    float predictionErrorSignal;
    float predictionConfidence;
    float attentionFromPrediction;
    
    // Working memory integration
    float attentionToWorkingMemory;
    float workingMemoryAttendedNeurons;
    
    // Concept formation integration
    float conceptInfluenceOnAttention;
    size_t attendedConcepts;
    
    // Neuromodulation integration
    float dopamineInfluence;
    float acetylcholineInfluence;
    float norepinephrineInfluence;
    float serotoninInfluence;
    
    // Replay integration
    float attentionDuringReplay;
    size_t replayAttentionTargets;
    
    // Development integration
    float developmentalAttentionalFocus;
    
    // Learning integration
    float attentionBasedLearningStrength;
    std::vector<NeuronId> learningHistory;
    
    Impl() : predictionErrorSignal(0.0f)
           , predictionConfidence(0.5f)
           , attentionFromPrediction(0.0f)
           , attentionToWorkingMemory(0.0f)
           , workingMemoryAttendedNeurons(0.0f)
           , conceptInfluenceOnAttention(0.0f)
           , attendedConcepts(0)
           , dopamineInfluence(0.0f)
           , acetylcholineInfluence(0.0f)
           , norepinephrineInfluence(0.0f)
           , serotoninInfluence(0.0f)
           , attentionDuringReplay(0.0f)
           , replayAttentionTargets(0)
           , developmentalAttentionalFocus(1.0f)
           , attentionBasedLearningStrength(0.0f) {}
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
    
    // Compute activity levels for competitors with integration
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    // Check for integration signals
    float predictionError = getPredictionErrorFromSystem();
    float neuromodulationInfluence = getNeuromodulationInfluence();
    size_t conceptCount = getConceptCount();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        // Integrate with prediction errors
        if (predictionError > 0.1f) {
            salience += predictionError * 0.5f;
        }
        
        // Integrate with neuromodulation
        salience *= (1.0f + neuromodulationInfluence * 0.3f);
        
        // Integrate with concept influence
        if (conceptCount > 0) {
            salience *= (1.0f + conceptCount * 0.05f);
        }
        
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
    
    // Apply attention-based learning
    applyAttentionBasedLearning(competitors, activities);
    
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
    
    // Update integration states
    pImpl->predictionErrorSignal *= 0.95f;
    pImpl->attentionBasedLearningStrength *= 0.9f;
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
    
    // Reset integration states
    pImpl->predictionErrorSignal = 0.0f;
    pImpl->attentionBasedLearningStrength = 0.0f;
    pImpl->learningHistory.clear();
}

// Enhancement Integration Methods

// Prediction Integration
void AttentionalSelection::applyPredictionError(float error) {
    pImpl->predictionErrorSignal = std::min(1.0f, error * 1.5f);
    pImpl->attentionFromPrediction = error * 2.0f;
    
    // High prediction error increases attention saliency
    if (error > 0.5f) {
        for (auto& neuronId : winners_) {
            applyBottomUpSalience(neuronId, pImpl->predictionErrorSignal * 0.5f);
        }
    }
}

void AttentionalSelection::applyPredictionConfidence(float confidence) {
    pImpl->predictionConfidence = confidence;
    
    // High confidence refines attention focus
    if (confidence > 0.8f) {
        // Narrow attention to high-confidence predictions
        float confidenceBoost = confidence * 0.3f;
        for (auto& neuronId : winners_) {
            applyTopDownBias(neuronId, confidenceBoost);
        }
    }
}

void AttentionalSelection::applyWorkingMemoryIntegration(const NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    // Get attended neurons from working memory
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    pImpl->workingMemoryAttendedNeurons = memoryNeurons.size();
    
    // Store which working memory patterns are being attended
    for (const auto& neuronId : memoryNeurons) {
        if (isAttended(neuronId)) {
            float attentionBoost = 1.0f + pImpl->attentionToWorkingMemory * 0.5f;
            applyTopDownBias(neuronId, attentionBoost);
        }
    }
}

void AttentionalSelection::applyConceptIntegration(const ConceptFormation* conceptFormation) {
    if (!conceptFormation) return;
    
    // Use concept stability to modulate attention
    for (size_t conceptId = 1; conceptId <= 10; ++conceptId) {
        if (conceptFormation->getConceptStability(conceptId) > 0.8f) {
            pImpl->attendedConcepts++;
            // Focus attention on stable concepts
            float conceptBoost = conceptFormation->getConceptStability(conceptId) * 0.5f;
            applyTopDownBias(NeuronId(conceptId * 1000), conceptBoost);  // Example neuron IDs
        }
    }
}

void AttentionalSelection::applyNeuromodulationIntegration(const Dopamine* dopamine,
                                                            const Acetylcholine* acetylcholine,
                                                            const Norepinephrine* norepinephrine,
                                                            const Serotonin* serotonin) {
    // Collect neuromodulator influences
    if (dopamine) pImpl->dopamineInfluence = dopamine->getLevel();
    if (acetylcholine) pImpl->acetylcholineInfluence = acetylcholine->getLevel();
    if (norepinephrine) pImpl->norepinephrineInfluence = norepinephrine->getLevel();
    if (serotonin) pImpl->serotoninInfluence = serotonin->getLevel();
    
    // Apply combined neuromodulation effects
    float totalModulation = pImpl->dopamineInfluence + pImpl->acetylcholineInfluence +
                            pImpl->norepinephrineInfluence + pImpl->serotoninInfluence;
    
    if (totalModulation > 0.5f) {
        // Strong neuromodulation enhances attention
        for (auto& neuronId : winners_) {
            float modulationBoost = totalModulation * 0.4f;
            applyBottomUpSalience(neuronId, modulationBoost);
        }
    }
}

void AttentionalSelection::applyReplayIntegration(const NeuralEpisodicMemory* episodicMemory,
                                                  const std::vector<NeuronId>& replayTargets) {
    if (!episodicMemory) return;
    
    pImpl->replayAttentionTargets = replayTargets.size();
    
    // Guide replay through attended regions
    for (const auto& neuronId : replayTargets) {
        if (isAttended(neuronId)) {
            float replayBoost = 1.5f;
            applyTopDownBias(neuronId, replayBoost);
        }
    }
}

void AttentionalSelection::applyDevelopmentIntegration(const DevelopmentSystem* development) {
    if (!development) return;
    
    // Adjust attention span and focus based on developmental stage
    switch (development->getStage()) {
        case DevelopmentalStage::Initial:
            pImpl->developmentalAttentionalFocus = 1.0f;  // Broad attention
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->developmentalAttentionalFocus = 0.7f;  // Focused attention
            break;
        case DevelopmentalStage::Maturation:
            pImpl->developmentalAttentionalFocus = 0.5f;  // Balanced
            break;
        case DevelopmentalStage::Adult:
            pImpl->developmentalAttentionalFocus = 0.3f;  // Narrow focus
            break;
    }
    
    // Apply developmental effects to competition
    for (auto& neuronId : winners_) {
        float developmentalBoost = pImpl->developmentalAttentionalFocus * 0.3f;
        applyBottomUpSalience(neuronId, developmentalBoost);
    }
}

void AttentionalSelection::applyFeedbackLoops(const std::vector<NeuronId>& learningHistory) {
    pImpl->learningHistory = learningHistory;
    
    // Use attention-based learning for adaptation
    if (!learningHistory.empty()) {
        float learningStrength = std::min(1.0f, static_cast<float>(learningHistory.size()) * 0.01f);
        pImpl->attentionBasedLearningStrength = learningStrength;
        
        // Adjust competition threshold based on learning
        float newThreshold = competitionThreshold_ * (1.0f - learningStrength * 0.2f);
        setCompetitionThreshold(newThreshold);
        
        // Store learning history
        for (const auto& neuronId : learningHistory) {
            applyBottomUpSalience(neuronId, learningStrength * 0.5f);
        }
    }
}

// System integration helpers
float AttentionalSelection::getPredictionErrorFromSystem() const {
    if (brain_ && brain_->getPredictionSystem()) {
        return brain_->getPredictionSystem()->getPredictionError();
    }
    return 0.0f;
}

float AttentionalSelection::getNeuromodulationInfluence() const {
    float totalInfluence = 0.0f;
    if (brain_) {
        auto* dopamine = brain_->getDopamine();
        auto* acetylcholine = brain_->getAcetylcholine();
        auto* norepinephrine = brain_->getNorepinephrine();
        auto* serotonin = brain_->getSerotonin();
        
        if (dopamine) totalInfluence += dopamine->getLevel();
        if (acetylcholine) totalInfluence += acetylcholine->getLevel();
        if (norepinephrine) totalInfluence += norepinephrine->getLevel();
        if (serotonin) totalInfluence += serotonin->getLevel();
    }
    return totalInfluence;
}

size_t AttentionalSelection::getConceptCount() const {
    if (brain_ && brain_->getConceptFormation()) {
        // This would need to be implemented in ConceptFormation to count concepts
        // For now, return a placeholder value
        return 3; // Example: 3 concepts
    }
    return 0;
}

void AttentionalSelection::applyAttentionBasedLearning(const std::vector<NeuronId>& competitors,
                                                       const std::vector<float>& activities) {
    if (activities.empty()) return;
    
    // Find the best performer
    float maxActivity = 0.0f;
    size_t bestIndex = 0;
    for (size_t i = 0; i < activities.size(); ++i) {
        if (activities[i] > maxActivity) {
            maxActivity = activities[i];
            bestIndex = i;
        }
    }
    
    // Store the winning neuron ID for learning history
    if (maxActivity > 0.1f) {
        pImpl->learningHistory.push_back(competitors[bestIndex]);
        
        // Increase attention-based learning strength
        pImpl->attentionBasedLearningStrength = std::min(1.0f, 
            pImpl->attentionBasedLearningStrength + 0.05f);
        
        // Apply learning to attention modulation
        float learningBoost = pImpl->attentionBasedLearningStrength * 0.5f;
        for (auto& neuronId : winners_) {
            applyTopDownBias(neuronId, learningBoost);
        }
    }
}

} // namespace nlm
