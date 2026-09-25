#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
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
    NLM_LOG_INFO("ConceptFormation initialized");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept
        updateConcept(matchingConcept, pattern, features, reward);
        return matchingConcept;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(pattern, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConcept(pattern, features, reward);
    }
    
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
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
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " (category: " + concept.categoryHint + ")");
    
    return concept.id;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
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
    
    // Update stability
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
    
    // Remove B
    concepts_.erase(
        std::remove_if(concepts_.begin(), concepts_.end(),
            [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
        concepts_.end()
    );
    
    NLM_LOG_INFO("ConceptFormation: Merged concepts " + std::to_string(conceptA) +
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

void ConceptFormation::processNeuralPatterns() {
    if (!brain_ || !brain_->getWorkingMemory()) return;
    
    // Get current neural activity patterns from working memory
    auto workingMemory = brain_->getWorkingMemory();
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    
    if (memoryNeurons.empty()) return;
    
    // Extract pattern from neural activity
    std::vector<float> currentPattern;
    float totalActivation = 0.0f;
    
    for (NeuronId neuronId : memoryNeurons) {
        float activation = workingMemory->getNeuronActivation(neuronId);
        currentPattern.push_back(activation);
        totalActivation += activation;
    }
    
    if (totalActivation > 0.1f) {
        // Normalize pattern
        for (float& val : currentPattern) {
            val /= totalActivation;
        }
        
        // Get current reward from neuromodulation
        float currentReward = 0.0f;
        if (brain_->getDopamine()) {
            currentReward = brain_->getDopamine()->getLevel();
        }
        
        // Present experience to concept formation system
        presentExperience(currentPattern, currentPattern, currentReward, 
                         static_cast<SimulationStep>(brain_->getCurrentStep()));
    }
}

void ConceptFormation::integrateWithEpisodicMemory() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto episodicMemory = brain_->getEpisodicMemory();
    auto episodes = episodicMemory->getEpisodes();
    
    // Extract patterns from episodic memory to discover concepts
    for (const auto* episode : episodes) {
        if (!episode) continue;
        
        // Create neural pattern from episode
        std::vector<float> episodePattern;
        episodePattern.reserve(episode->sensoryState.size() + episode->activeNeurons.size());
        
        // Add sensory state features
        for (float val : episode->sensoryState) {
            episodePattern.push_back(val);
        }
        
        // Add neural activation features
        for (float val : episode->neuronActivations) {
            episodePattern.push_back(val / 20.0f); // Normalize
        }
        
        // Get reward from episode
        float episodeReward = episode->resultingReward > 0.0f ? 
                             episode->resultingReward : episode->reward;
        
        if (!episodePattern.empty()) {
            // Present to concept formation
            presentExperience(episodePattern, episodePattern, episodeReward,
                             episode->timestamp);
        }
    }
}

void ConceptFormation::integrateWithPredictionSystem() {
    if (!brain_ || !brain_->getPredictionSystem()) return;
    
    auto predictionSystem = brain_->getPredictionSystem();
    
    // Get prediction confidence and error
    float confidence = predictionSystem->getConfidence();
    float predictionError = predictionSystem->getPredictionError();
    
    // Adjust concept formation threshold based on prediction
    // High confidence = more stable concepts (higher threshold)
    // High error = more novel concepts (lower threshold)
    float thresholdAdjustment = confidence * 0.3f - predictionError * 0.2f;
    
    float newThreshold = formationThreshold_ + thresholdAdjustment;
    newThreshold = std::max(0.3f, std::min(0.9f, newThreshold));
    
    setFormationThreshold(newThreshold);
}

void ConceptFormation::integrateWithAttention() {
    if (!brain_) return;
    
    auto attention = brain_->getAttention();
    if (!attention) return;
    
    // Get attended neurons
    const auto& winners = attention->getWinners();
    
    if (winners.empty()) return;
    
    // Get activation levels of attended neurons
    std::vector<float> attendedActivations;
    
    for (NeuronId neuronId : winners) {
        float activation = 0.0f;
        if (brain_->getWorkingMemory()) {
            activation = brain_->getWorkingMemory()->getNeuronActivation(neuronId);
        }
        attendedActivations.push_back(activation);
    }
    
    // Normalize activations
    float maxActivation = *std::max_element(attendedActivations.begin(), 
                                            attendedActivations.end());
    if (maxActivation > 0.0f) {
        for (float& val : attendedActivations) {
            val /= maxActivation;
        }
        
        // Present attended patterns to concept formation
        presentExperience(attendedActivations, attendedActivations, 
                         maxActivation, 
                         static_cast<SimulationStep>(brain_->getCurrentStep()));
    }
}

void ConceptFormation::applyNeuromodulationEffects() {
    if (!brain_) return;
    
    float noveltyLevel = 0.5f;
    if (brain_->getNovelty()) {
        noveltyLevel = brain_->getNovelty()->getLevel();
    }
    
    float curiosityLevel = 0.5f;
    if (brain_->getCuriosity()) {
        curiosityLevel = brain_->getCuriosity()->getLevel();
    }
    
    // Novelty and curiosity affect concept formation
    // High novelty = form new concepts more readily
    // High curiosity = explore more variations before committing to concepts
    
    // Adjust formation threshold based on neuromodulation
    float noveltyEffect = noveltyLevel * 0.4f;
    float curiosityEffect = curiosityLevel * 0.3f;
    
    float thresholdAdjustment = curiosityEffect - noveltyEffect;
    float newThreshold = formationThreshold_ + thresholdAdjustment;
    
    // Higher curiosity means we're more exploratory, so lower threshold
    // Higher novelty means we're more surprised, so higher threshold (need more evidence)
    setFormationThreshold(std::max(0.3f, std::min(0.8f, newThreshold)));
}

void ConceptFormation::updateConceptStability() {
    // Periodically update concept stability based on recent experience
    if (concepts_.empty()) return;
    
    // Check if we need to stabilize concepts
    for (auto& concept : concepts_) {
        if (concept.totalObservations > 10) {  // Need enough observations
            float currentStability = getConceptStability(concept.id);
            
            // Stability increases with consistency, decreases with variance
            float newStability = 0.8f;  // Base stability
            
            if (!concept.instances.empty()) {
                // Calculate variance from prototype
                float totalVariance = 0.0f;
                for (const auto& instance : concept.instances) {
                    float sim = computeSimilarity(instance.pattern, concept.prototype);
                    totalVariance += (1.0f - sim);
                }
                
                float avgVariance = totalVariance / concept.instances.size();
                newStability = std::max(0.0f, 1.0f - avgVariance * 2.0f);
            }
            
            // Smooth update
            concept.avgStability = concept.avgStability * 0.8f + newStability * 0.2f;
            
            // Update instance stability
            for (auto& instance : concept.instances) {
                instance.stability = newStability;
            }
        }
    }
}

void ConceptFormation::pruneUnstableConcepts() {
    // Remove concepts that are too unstable
    auto stableConcepts = concepts_;
    concepts_.clear();
    
    for (const auto& concept : stableConcepts) {
        if (isConceptStable(concept.id)) {
            concepts_.push_back(concept);
        }
    }
    
    // Reassign concept IDs
    for (size_t i = 0; i < concepts_.size(); ++i) {
        concepts_[i].id = i + 1;
    }
    
    if (concepts_.size() < stableConcepts.size()) {
        NLM_LOG_INFO("ConceptFormation pruned unstable concepts from " +
                     std::to_string(stableConcepts.size()) + " to " +
                     std::to_string(concepts_.size()));
    }
}