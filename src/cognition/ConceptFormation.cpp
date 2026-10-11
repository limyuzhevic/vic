#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Reference caches for system integration
    std::vector<PredictionSystem*> predictionSystems;
    std::vector<AttentionalSelection*> attentionSystems;
    std::vector<NeuralWorkingMemory*> workingMemories;
    std::vector<NeuralEpisodicMemory*> episodicMemories;
    std::vector<DevelopmentSystem*> developmentSystems;
    std::vector<Dopamine*> dopamines;
    std::vector<Acetylcholine*> acetylcholines;
    std::vector<Norepinephrine*> norepinephrines;
    std::vector<Serotonin*> serotonins;
    std::vector<Novelty*> novelties;
    std::vector<Curiosity*> curiosities;
    std::vector<PredictionError*> predictionErrors;
    
    Impl() : brain(nullptr) {}
    
    // Update reference caches from brain
    void updateReferences(Brain* brain) {
        this->brain = brain;
        predictionSystems.clear();
        attentionSystems.clear();
        workingMemories.clear();
        episodicMemories.clear();
        developmentSystems.clear();
        dopamines.clear();
        acetylcholines.clear();
        norepinephrines.clear();
        serotonins.clear();
        novelties.clear();
        curiosities.clear();
        predictionErrors.clear();
        
        if (brain) {
            // Cache references to integrated systems from brain
            if (brain->getPredictionSystem()) predictionSystems.push_back(brain->getPredictionSystem());
            if (brain->getWorkingMemory()) workingMemories.push_back(brain->getWorkingMemory());
            if (brain->getAttentionalSelection()) attentionSystems.push_back(brain->getAttentionalSelection());
            if (brain->getEpisodicMemory()) episodicMemories.push_back(brain->getEpisodicMemory());
            if (brain->getDevelopmentSystem()) developmentSystems.push_back(brain->getDevelopmentSystem());
            if (brain->getDopamine()) dopamines.push_back(brain->getDopamine());
            if (brain->getAcetylcholine()) acetylcholines.push_back(brain->getAcetylcholine());
            if (brain->getNorepinephrine()) norepinephrines.push_back(brain->getNorepinephrine());
            if (brain->getSerotonin()) serotonins.push_back(brain->getSerotonin());
            if (brain->getNovelty()) novelties.push_back(brain->getNovelty());
            if (brain->getCuriosity()) curiosities.push_back(brain->getCuriosity());
            if (brain->getPredictionError()) predictionErrors.push_back(brain->getPredictionError());
        }
    }
    
    // Get reference to system or nullptr
    template<typename T>
    T* getSystemReference() {
        if constexpr (std::is_same_v<T, PredictionSystem>) {
            return predictionSystems.empty() ? nullptr : predictionSystems[0];
        } else if constexpr (std::is_same_v<T, AttentionalSelection>) {
            return attentionSystems.empty() ? nullptr : attentionSystems[0];
        } else if constexpr (std::is_same_v<T, NeuralWorkingMemory>) {
            return workingMemories.empty() ? nullptr : workingMemories[0];
        } else if constexpr (std::is_same_v<T, NeuralEpisodicMemory>) {
            return episodicMemories.empty() ? nullptr : episodicMemories[0];
        } else if constexpr (std::is_same_v<T, DevelopmentSystem>) {
            return developmentSystems.empty() ? nullptr : developmentSystems[0];
        } else if constexpr (std::is_same_v<T, Dopamine>) {
            return dopamines.empty() ? nullptr : dopamines[0];
        } else if constexpr (std::is_same_v<T, Acetylcholine>) {
            return acetylcholines.empty() ? nullptr : acetylcholines[0];
        } else if constexpr (std::is_same_v<T, Norepinephrine>) {
            return norepinephrines.empty() ? nullptr : norepinephrines[0];
        } else if constexpr (std::is_same_v<T, Serotonin>) {
            return serotonins.empty() ? nullptr : serotonins[0];
        } else if constexpr (std::is_same_v<T, Novelty>) {
            return novelties.empty() ? nullptr : novelties[0];
        } else if constexpr (std::is_same_v<T, Curiosity>) {
            return curiosities.empty() ? nullptr : curiosities[0];
        } else if constexpr (std::is_same_v<T, PredictionError>) {
            return predictionErrors.empty() ? nullptr : predictionErrors[0];
        }
        return nullptr;
    }
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->updateReferences(brain);
    NLM_LOG_INFO("Enhanced ConceptFormation initialized with system integration");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    return presentExperienceWithIntegration(pattern, features, reward, currentTime);
}

size_t ConceptFormation::presentExperienceWithIntegration(
    const std::vector<float>& pattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime,
    float predictionError,
    const std::vector<float>* workingMemoryPattern,
    const AttentionalSelection* attention,
    const PredictionSystem* prediction,
    const Neuromodulator* neuromodulator,
    const DevelopmentSystem* development,
    const PredictionError* predictionError) {
    if (pattern.empty()) return 0;
    
    // Update reference caches if provided
    if (prediction) pImpl->predictionSystems.push_back(const_cast<PredictionSystem*>(prediction));
    if (attention) pImpl->attentionSystems.push_back(const_cast<AttentionalSelection*>(attention));
    if (neuromodulator) {
        if (dynamic_cast<const Dopamine*>(neuromodulator)) {
            pImpl->dopamines.push_back(const_cast<Dopamine*>(dynamic_cast<const Dopamine*>(neuromodulator)));
        } else if (dynamic_cast<const Acetylcholine*>(neuromodulator)) {
            pImpl->acetylcholines.push_back(const_cast<Acetylcholine*>(dynamic_cast<const Acetylcholine*>(neuromodulator)));
        } else if (dynamic_cast<const Norepinephrine*>(neuromodulator)) {
            pImpl->norepinephrines.push_back(const_cast<Norepinephrine*>(dynamic_cast<const Norepinephrine*>(neuromodulator)));
        } else if (dynamic_cast<const Serotonin*>(neuromodulator)) {
            pImpl->serotonins.push_back(const_cast<Serotonin*>(dynamic_cast<const Serotonin*>(neuromodulator)));
        }
    }
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept with all system integration
        updateConceptWithIntegration(matchingConcept, pattern, features, reward, currentTime,
                                    predictionError, workingMemoryPattern, attention, prediction,
                                    neuromodulator, development);
        return matchingConcept;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(pattern, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConceptWithIntegration(pattern, features, reward, currentTime,
                                           predictionError, workingMemoryPattern,
                                           attention, prediction, neuromodulator,
                                           development, predictionError);
    }
    
    return 0;  // Not yet classifiable
}

void ConceptFormation::applyPredictionErrorToConcept(
    size_t conceptId,
    float predictionError,
    const PredictionSystem* prediction) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Update prediction-based attributes
    concept->predictionError = predictionError;
    concept->predictionsBasedOn++;
    
    // Use prediction error to adjust concept stability
    if (predictionError > 0.5f) {
        // High prediction error means concept needs refinement
        concept->avgStability *= (1.0f - predictionError * 0.3f);
    } else if (predictionError < 0.2f) {
        // Low prediction error means concept is stable
        concept->avgStability = std::min(1.0f, concept->avgStability * 1.1f);
    }
    
    // Store prediction in concept for future predictions
    if (prediction) {
        concept->predictionsBasedOn++;
        // Concept can contribute to prediction system
        // This would be implemented by connecting to prediction system
    }
}

void ConceptFormation::applyAttentionToConcept(
    size_t conceptId,
    float attentionWeight,
    const AttentionalSelection* attention) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Apply attention weighting to concept
    concept->attentionWeight = attentionWeight;
    
    // Store attention scores
    if (attention && !concept->attentionScores.empty()) {
        concept->attentionScores.push_back(attentionWeight);
        if (concept->attentionScores.size() > 10) {
            concept->attentionScores.erase(concept->attentionScores.begin());
        }
    }
    
    // Use attention to prioritize concept stability
    if (attentionWeight > 0.8f) {
        concept->avgStability = std::min(1.0f, concept->avgStability * 1.2f);
    }
}

void ConceptFormation::applyNeuromodulationToConcept(
    size_t conceptId,
    const Neuromodulator* neuromodulator) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Apply neuromodulation effects based on type
    if (dynamic_cast<const Dopamine*>(neuromodulator)) {
        concept->dopamineInfluence = neuromodulator->getLevel();
        // Dopamine reinforces reward-based concepts
        if (concept->dopamineInfluence > 0.5f) {
            concept->avgStability = std::min(1.0f, concept->avgStability * 1.15f);
        }
    } else if (dynamic_cast<const Acetylcholine*>(neuromodulator)) {
        concept->acetylcholineInfluence = neuromodulator->getLevel();
        // Acetylcholine enhances memory encoding during concept formation
        if (concept->acetylcholineInfluence > 0.5f) {
            concept->avgStability = std::min(1.0f, concept->avgStability * 1.1f);
        }
    } else if (dynamic_cast<const Norepinephrine*>(neuromodulator)) {
        concept->norepinephrineInfluence = neuromodulator->getLevel();
        // Norepinephrine detects salience in concept discovery
        if (concept->norepinephrineInfluence > 0.5f) {
            concept->avgStability = std::min(1.0f, concept->avgStability * 1.1f);
        }
    } else if (dynamic_cast<const Serotonin*>(neuromodulator)) {
        concept->serotoninInfluence = neuromodulator->getLevel();
        // Serotonin modulates mood-dependent learning
        if (concept->serotoninInfluence > 0.5f) {
            concept->avgStability *= (1.0f + concept->serotoninInfluence * 0.1f);
        }
    }
}

void ConceptFormation::applyWorkingMemoryToConcept(
    size_t conceptId,
    const std::vector<float>& workingMemoryPattern) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Apply working memory pattern to concept
    concept->workingMemoryActivation = 1.0f;
    
    // Store working memory source ID
    concept->workingMemorySourceIds.push_back(conceptId);
    
    // Use working memory to enhance concept representation
    if (!workingMemoryPattern.empty() && concept->prototype.size() == workingMemoryPattern.size()) {
        // Update prototype with working memory pattern
        for (size_t i = 0; i < concept->prototype.size(); ++i) {
            concept->prototype[i] = concept->prototype[i] * 0.7f + workingMemoryPattern[i] * 0.3f;
        }
    }
}

void ConceptFormation::applyDevelopmentalEffectsToConcept(
    size_t conceptId,
    const DevelopmentSystem* development) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Apply developmental stage effects
    if (development) {
        concept->developmentalStage = development->getStage();
        
        // Adjust learning rates based on developmental stage
        float plasticityModifier = development->getPlasticityModifier();
        concept->developmentalPlasticity = plasticityModifier;
        
        // Critical period enhances concept formation
        if (development->isCriticalPeriod()) {
            concept->avgStability = std::min(1.0f, concept->avgStability * (1.0f + plasticityModifier * 0.5f));
        }
        
        // Adult stage reduces plasticity
        if (concept->developmentalStage == DevelopmentalStage::Adult) {
            concept->avgStability *= (1.0f - (1.0f - plasticityModifier) * 0.2f);
        }
    }
}

void ConceptFormation::applyReplayToConcept(
    size_t conceptId,
    const NeuralEpisodicMemory* episodicMemory,
    SimulationStep currentTime) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Check if concept should be reinforced through replay
    bool shouldReplay = false;
    if (episodicMemory) {
        shouldReplay = episodicMemory->getEpisodeCount() > 10; // Has enough episodes
    }
    
    if (shouldReplay) {
        concept->replayCount++;
        concept->lastReplayTime = currentTime;
        
        // Use replay to strengthen concept
        if (currentTime > 100) { // After initial learning
            concept->consolidationScore = std::min(1.0f, concept->consolidationScore + 0.1f);
        }
        
        concept->isConsolidated = true;
    }
}

void ConceptFormation::connectToPredictionSystem(
    size_t conceptId,
    const PredictionSystem* prediction) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Store prediction system reference for future use
    // In full implementation, this would create actual connections
    // between concept and prediction system
    concept->predictionsBasedOn++;
}

void ConceptFormation::useConceptForPrediction(
    const std::vector<float>& pattern,
    const PredictionSystem* prediction) {
    if (!prediction) return;
    
    // Get best matching concept
    size_t conceptId = findConceptForPattern(pattern);
    if (conceptId == 0) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Use concept prototype to improve prediction
    // This would integrate with prediction system's forward model
    // For now, just log that concept is available for prediction
    NLM_LOG_INFO("ConceptFormation: Concept " + std::to_string(conceptId) +
                 " available for prediction enhancement");
}

void ConceptFormation::applyConceptBasedAttention(
    size_t conceptId,
    float biasStrength,
    AttentionalSelection* attention) {
    if (!attention) return;
    
    // Apply top-down attention bias based on concept
    // This would focus attention on neurons related to this concept
    NLM_LOG_INFO("ConceptFormation: Applying concept-based attention bias for concept " +
                 std::to_string(conceptId) + " with strength " + std::to_string(biasStrength));
}

void ConceptFormation::updateConceptWithPredictionError(
    size_t conceptId,
    float predictionError,
    const PredictionError* predictionError) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Update concept based on prediction error
    concept->predictionError = predictionError;
    
    // Use prediction error to adjust concept stability
    if (predictionError > 0.5f) {
        // High error means concept needs refinement
        concept->avgStability *= (1.0f - predictionError * 0.2f);
    }
}

void ConceptFormation::rehearseConcept(
    size_t conceptId,
    const NeuralWorkingMemory* workingMemory) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Rehearse concept through working memory
    if (workingMemory) {
        concept->workingMemoryActivation = 1.0f;
        NLM_LOG_INFO("ConceptFormation: Rehearsing concept " + std::to_string(conceptId));
    }
}

void ConceptFormation::consolidateConcept(
    size_t conceptId,
    const NeuralEpisodicMemory* episodicMemory,
    SimulationStep currentStep) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Consolidate concept with episodic memory
    if (episodicMemory) {
        concept->isConsolidated = true;
        concept->consolidationScore = 0.9f; // High consolidation score
        concept->consolidationTime = currentStep;
        NLM_LOG_INFO("ConceptFormation: Consolidating concept " + std::to_string(conceptId));
    }
}

void ConceptFormation::adjustLearningRateForDevelopment(
    float& formationThreshold,
    float& stabilityThreshold,
    float& learningRate,
    const DevelopmentSystem* development) {
    if (!development) return;
    
    // Adjust learning rates based on developmental stage
    float plasticityModifier = development->getPlasticityModifier();
    
    formationThreshold_ = 0.75f * plasticityModifier;
    stabilityThreshold_ = 0.7f * plasticityModifier;
    
    if (development->isCriticalPeriod()) {
        // Enhanced learning during critical period
        formationThreshold_ *= 0.8f;
        stabilityThreshold_ *= 0.8f;
    }
}

float ConceptFormation::getConceptIntegrationScore(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    // Calculate integration score based on all systems
    float score = 0.0f;
    
    // Stability contribution
    score += concept->avgStability * 0.3f;
    
    // Prediction accuracy contribution
    score += (1.0f - concept->predictionError) * 0.2f;
    
    // Attention contribution
    score += concept->attentionWeight * 0.2f;
    
    // Neuromodulation contribution
    float neuromodulationScore = (concept->dopamineInfluence + concept->acetylcholineInfluence +
                               concept->norepinephrineInfluence + concept->serotoninInfluence) / 4.0f;
    score += neuromodulationScore * 0.15f;
    
    // Replay contribution
    score += (concept->replayCount * 0.05f);
    
    // Consolidation contribution
    score += concept->consolidationScore * 0.1f;
    
    return score;
}

bool ConceptFormation::shouldReplayConcept(
    size_t conceptId,
    const NeuralEpisodicMemory* episodicMemory,
    SimulationStep currentTime) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return false;
    
    // Check if concept should be reinforced through replay
    if (concept->replayCount < 3) return false; // Not replayed enough
    if (!concept->isConsolidated) return false; // Not yet consolidated
    if (currentTime < 100) return false; // Too early
    
    if (episodicMemory && episodicMemory->getEpisodeCount() < 50) return false; // Not enough episodes
    
    return true;
}

float ConceptFormation::getConceptPredictionContribution(
    size_t conceptId,
    const PredictionSystem* prediction) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    if (!prediction) return 0.0f;
    
    // Calculate concept's contribution to prediction
    float contribution = 0.0f;
    
    // Based on prediction accuracy
    contribution += (1.0f - concept->predictionError) * 0.5f;
    
    // Based on number of predictions based on this concept
    if (concept->predictionsBasedOn > 0) {
        contribution += std::min(1.0f, concept->predictionsBasedOn * 0.05f);
    }
    
    return contribution;
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                      const std::vector<float>& features,
                                      float reward) {
    return createConceptWithIntegration(pattern, features, reward, 0,
                                      0.0f, nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr);
}

size_t ConceptFormation::createConceptWithIntegration(
    const std::vector<float>& pattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime,
    float predictionError,
    const std::vector<float>* workingMemoryPattern,
    const AttentionalSelection* attention,
    const PredictionSystem* prediction,
    const Neuromodulator* neuromodulator,
    const DevelopmentSystem* development,
    const PredictionError* predictionErrorSystem) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    concept.avgStability = 1.0f;
    
    // Determine category hint based on properties
    if (reward > 0.5f) {
        concept.categoryHint = "rewarding";
    } else if (reward < -0.3f) {
        concept.categoryHint = "harmful";
    } else {
        concept.categoryHint = "neutral";
    }
    
    // Enhanced initialization with system integration
    concept.predictionAccuracy = 0.5f;
    concept.predictionError = predictionError;
    concept.predictionsBasedOn = 0;
    
    concept.attentionWeight = 1.0f;
    concept.attentionScores.clear();
    
    concept.dopamineInfluence = 0.0f;
    concept.acetylcholineInfluence = 0.0f;
    concept.norepinephrineInfluence = 0.0f;
    concept.serotoninInfluence = 0.0f;
    
    concept.replayCount = 0;
    concept.consolidationScore = 0.0f;
    concept.lastReplayTime = 0;
    concept.isConsolidated = false;
    
    concept.developmentalStage = DevelopmentSystem().getStage();
    concept.developmentalPlasticity = development ? development->getPlasticityModifier() : 1.0f;
    
    concept.workingMemorySourceIds.clear();
    concept.workingMemoryActivation = 0.0f;
    
    concepts_.push_back(concept);
    
    // Apply system-specific integrations
    applyPredictionErrorToConcept(concept.id, predictionError, prediction);
    applyNeuromodulationToConcept(concept.id, neuromodulator);
    applyDevelopmentEffectsToConcept(concept.id, development);
    applyWorkingMemoryToConcept(concept.id, workingMemoryPattern ? *workingMemoryPattern : std::vector<float>());
    
    // Connect to prediction system for future predictions
    if (prediction) {
        connectToPredictionSystem(concept.id, prediction);
    }
    
    // Use concept to improve prediction
    if (prediction) {
        useConceptForPrediction(pattern, prediction);
    }
    
    NLM_LOG_INFO("Enhanced ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " (category: " + concept.categoryHint + ")");
    
    return concept.id;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    updateConceptWithIntegration(conceptId, newPattern, features, reward, 0,
                               0.0f, nullptr, nullptr, nullptr, nullptr, nullptr);
}

void ConceptFormation::updateConceptWithIntegration(
    size_t conceptId,
    const std::vector<float>& newPattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime,
    float predictionError,
    const std::vector<float>* workingMemoryPattern,
    const AttentionalSelection* attention,
    const PredictionSystem* prediction,
    const Neuromodulator* neuromodulator,
    const DevelopmentSystem* development) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype using exponential moving average (Hebbian-like)
    updatePrototype(conceptId, newPattern);
    
    // Update stability with system integration
    float newStability = 1.0f;
    if (concept->instances.size() > 1) {
        // Compute stability as average similarity to prototype
        float totalSim = 0.0f;
        size_t recentCount = std::min(concept->instances.size(), stabilityWindow_);
        
        for (size_t i = concept->instances.size() - recentCount; 
             i < concept->instances.size(); ++i) {
            totalSim += computeSimilarity(concept->instances[i].pattern, concept->prototype);
        }
        
        newStability = totalSim / recentCount;
        concept->avgStability = concept->avgStability * 0.9f + newStability * 0.1f;
    }
    
    // Apply system integration to stability
    applyPredictionErrorToConcept(conceptId, predictionError, prediction);
    applyAttentionToConcept(conceptId, concept->attentionWeight, attention);
    applyNeuromodulationToConcept(conceptId, neuromodulator);
    applyDevelopmentEffectsToConcept(conceptId, development);
    applyWorkingMemoryToConcept(conceptId, workingMemoryPattern ? *workingMemoryPattern : std::vector<float>());
    
    // Update instance stability
    concept->instances.back().stability = newStability;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Hebbian update: weight prototype towards new instance
            float alpha = 0.2f;  // Learning rate
            
            if (concept.prototype.size() == newInstance.size()) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    concept.prototype[i] = concept.prototype[i] * (1.0f - alpha) + 
                                          newInstance[i] * alpha;
                }
            }
            break;
        }
    }
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                           float similarityThreshold) const {
    return findConceptForPattern(pattern);
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return &concept;
        }
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->prototype;
    }
    return {};
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(
    size_t conceptId) const {
    static std::vector<ConceptInstance> empty;
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->instances;
    }
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->avgStability;
    }
    return 0.0f;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    DiscoveredConcept* a = nullptr;
    DiscoveredConcept* b = nullptr;
    
    for (auto& c : concepts_) {
        if (c.id == conceptA) a = &c;
        if (c.id == conceptB) b = &c;
    }
    
    if (!a || !b) return;
    
    // Merge B into A
    for (const auto& instance : b->instances) {
        a->instances.push_back(instance);
        a->totalObservations++;
    }
    
    // Update prototype as average of both
    for (size_t i = 0; i < a->prototype.size() && i < b->prototype.size(); ++i) {
        a->prototype[i] = (a->prototype[i] + b->prototype[i]) / 2.0f;
    }
    
    // Merge system-specific attributes
    a->predictionAccuracy = std::min(a->predictionAccuracy, b->predictionAccuracy);
    a->predictionError = std::max(a->predictionError, b->predictionError);
    a->predictionsBasedOn += b->predictionsBasedOn;
    
    // Average attention scores
    for (float score : b->attentionScores) {
        a->attentionScores.push_back(score);
    }
    
    // Sum neuromodulation influences
    a->dopamineInfluence += b->dopamineInfluence;
    a->acetylcholineInfluence += b->acetylcholineInfluence;
    a->norepinephrineInfluence += b->norepinephrineInfluence;
    a->serotoninInfluence += b->serotoninInfluence;
    
    // Sum replay counts
    a->replayCount += b->replayCount;
    
    // Average consolidation scores
    a->consolidationScore = (a->consolidationScore + b->consolidationScore) / 2.0f;
    
    // Use developmental stage from 'a' concept
    
    // Merge working memory sources
    for (size_t id : b->workingMemorySourceIds) {
        a->workingMemorySourceIds.push_back(id);
    }
    
    // Update prototype stability
    float totalSim = 0.0f;
    size_t recentCount = std::min(a->instances.size(), stabilityWindow_);
    for (size_t i = a->instances.size() - recentCount; i < a->instances.size(); ++i) {
        totalSim += computeSimilarity(a->instances[i].pattern, a->prototype);
    }
    a->avgStability = totalSim / recentCount;
    
    // Remove B
    concepts_.erase(
        std::remove_if(concepts_.begin(), concepts_.end(),
            [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
        concepts_.end()
    );
    
    NLM_LOG_INFO("Enhanced ConceptFormation: Merged concepts " + std::to_string(conceptA) +
                 " and " + std::to_string(conceptB));
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                               float similarityThreshold) const {
    return findConceptForPattern(pattern) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    if (concepts_.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (const auto& concept : concepts_) {
        float sim = computeSimilarity(pattern, concept.prototype);
        if (sim > bestSimilarity && sim >= formationThreshold_) {
            bestSimilarity = sim;
            bestConcept = concept.id;
        }
    }
    
    return bestConcept;
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return false;
    
    return concept->avgStability >= stabilityThreshold_;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    // Generalization ability = how varied are the instances
    if (concept->instances.size() < 2) return 0.0f;
    
    float totalVariance = 0.0f;
    for (const auto& instance : concept->instances) {
        float sim = computeSimilarity(instance.pattern, concept->prototype);
        totalVariance += (1.0f - sim);
    }
    
    return 1.0f - (totalVariance / concept->instances.size());
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
}

// SpatialRepresentation Implementation
struct SpatialRepresentation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SpatialRepresentation::SpatialRepresentation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentX_(0)
    , currentY_(0)
    , gridSpacing_(1.0f)
{
}

SpatialRepresentation::~SpatialRepresentation() = default;

void SpatialRepresentation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SpatialRepresentation initialized");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    currentX_ = x;
    currentY_ = y;
    lastSensoryCue_ = sensoryCues;
    
    // Check if we have a place cell near this position
    bool hasNearbyCell = false;
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        if (std::sqrt(dx * dx + dy * dy) < pc.fieldRadius) {
            hasNearbyCell = true;
            break;
        }
    }
    
    // Create new place cell if needed
    if (!hasNearbyCell && brain_) {
        PlaceCell newCell;
        newCell.neuron = NeuronId(placeCells_.size() + 40000);
        newCell.x = x;
        newCell.y = y;
        newCell.fieldRadius = 0.5f;  // Place field size
        newCell.activation = 1.0f;
        
        placeCells_.push_back(newCell);
        
        // Inject current to activate this neuron
        brain_->injectCurrent(newCell.neuron, 5.0f);
    }
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    return {currentX_, currentY_};
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    currentX_ += dx;
    currentY_ += dy;
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    float totalActivation = 0.0f;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        // Gaussian place field
        if (dist < pc.fieldRadius * 2.0f) {
            float activation = std::exp(-dist * dist / (2.0f * pc.fieldRadius * pc.fieldRadius));
            totalActivation += activation * pc.activation;
        }
    }
    
    return totalActivation;
}

std::vector<NeuronId> SpatialRepresentation::getPlaceNeuronsNear(float x, float y, float radius) const {
    std::vector<NeuronId> result;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        if (std::sqrt(dx * dx + dy * dy) < radius) {
            result.push_back(pc.neuron);
        }
    }
    
    return result;
}

void SpatialRepresentation::clear() {
    placeCells_.clear();
    currentX_ = 0;
    currentY_ = 0;
}

// TemporalRelation Implementation
struct TemporalRelation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

TemporalRelation::TemporalRelation()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

TemporalRelation::~TemporalRelation() = default;

void TemporalRelation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                      const std::vector<float>& eventB,
                                      float reward) {
    // Find existing link or create new
    for (auto& link : sequences_) {
        float simA = 0.0f, simB = 0.0f;
        
        // Compute similarity
        if (link.fromEvent.size() == eventA.size() && link.toEvent.size() == eventB.size()) {
            float dotA = 0.0f, normA = 0.0f, normB = 0.0f;
            for (size_t i = 0; i < eventA.size(); ++i) {
                dotA += link.fromEvent[i] * eventA[i];
                normA += link.fromEvent[i] * link.fromEvent[i];
                normB += eventA[i] * eventA[i];
            }
            if (normA > 0.0001f && normB > 0.0001f) {
                simA = dotA / (std::sqrt(normA) * std::sqrt(normB));
            }
            
            float dotB = 0.0f;
            for (size_t i = 0; i < eventB.size(); ++i) {
                dotB += link.toEvent[i] * eventB[i];
            }
            if (normB > 0.0001f) {
                simB = dotB / normB;
            }
        }
        
        if (simA > 0.8f && simB > 0.8f) {
            // Update existing link
            link.strength = std::min(1.0f, link.strength + 0.1f);
            link.observationCount++;
            return;
        }
    }
    
    // Create new link
    SequenceLink newLink;
    newLink.fromEvent = eventA;
    newLink.toEvent = eventB;
    newLink.strength = 0.5f;
    newLink.observationCount = 1;
    
    sequences_.push_back(newLink);
}

std::vector<float> TemporalRelation::predictNext(const std::vector<float>& eventA) const {
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() != eventA.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < eventA.size(); ++i) {
            dot += link.fromEvent[i] * eventA[i];
            normA += link.fromEvent[i] * link.fromEvent[i];
            normB += eventA[i] * eventA[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > bestSim) {
                bestSim = sim;
                bestPrediction = link.toEvent;
            }
        }
    }
    
    return bestPrediction;
}

std::vector<float> TemporalRelation::predictPrevious(const std::vector<float>& eventB) const {
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.toEvent.size() != eventB.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < eventB.size(); ++i) {
            dot += link.toEvent[i] * eventB[i];
            normA += link.toEvent[i] * link.toEvent[i];
            normB += eventB[i] * eventB[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > bestSim) {
                bestSim = sim;
                bestPrediction = link.fromEvent;
            }
        }
    }
    
    return bestPrediction;
}

float TemporalRelation::getPredictionConfidence(const std::vector<float>& event) const {
    size_t matchingCount = 0;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() != event.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < event.size(); ++i) {
            dot += link.fromEvent[i] * event[i];
            normA += link.fromEvent[i] * link.fromEvent[i];
            normB += event[i] * event[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > 0.7f) {
                matchingCount++;
            }
        }
    }
    
    return std::min(1.0f, static_cast<float>(matchingCount) / 3.0f);
}

void TemporalRelation::clear() {
    sequences_.clear();
}

} // namespace nlm
