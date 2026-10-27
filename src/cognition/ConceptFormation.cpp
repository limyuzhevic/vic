#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
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
    
    // Advanced novelty detection using multi-scale pattern matching
    bool isNovel = checkNoveltyAtMultipleScales(pattern);
    
    if (!isNovel) {
        // Pattern matches existing concept - update it
        size_t matchingConcept = findConceptForPattern(pattern);
        if (matchingConcept > 0) {
            updateConceptWithTemporalWeighting(matchingConcept, pattern, features, reward, currentTime);
            return matchingConcept;
        }
    }
    
    // For novel patterns, collect more evidence before committing to a concept
    // This implements gradual concept formation
    size_t candidateConcept = getOrCreateTemporaryConcept(pattern, features, reward, currentTime);
    
    if (candidateConcept > 0) {
        // Only create stable concepts after sufficient evidence
        DiscoveredConcept* concept = nullptr;
        for (auto& c : concepts_) {
            if (c.id == candidateConcept) {
                concept = &c;
                break;
            }
        }
        
        if (concept && concept->totalObservations >= stabilityWindow_ && 
            concept->avgStability >= stabilityThreshold_) {
            // Concept has enough evidence and stability - promote to permanent
            return concept->id;
        }
        
        // Still collecting evidence
        return candidateConcept;
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

bool ConceptFormation::checkNoveltyAtMultipleScales(const std::vector<float>& pattern) const {
    if (concepts_.empty()) return true;
    
    // Multi-scale novelty detection
    bool novelAtSmallScale = true;
    bool novelAtMediumScale = true;
    bool novelAtLargeScale = true;
    
    // Small scale: check against all concept prototypes
    for (const auto& concept : concepts_) {
        float sim = computeSimilarity(pattern, concept.prototype);
        if (sim >= formationThreshold_ * 0.7f) {  // Softer threshold at small scale
            novelAtSmallScale = false;
            break;
        }
    }
    
    // Medium scale: check against aggregated concept statistics
    if (novelAtSmallScale) {
        for (const auto& concept : concepts_) {
            // Compute average prototype of concept
            std::vector<float> avgPrototype = concept.prototype;
            if (!avgPrototype.empty()) {
                float avgWeight = 1.0f / avgPrototype.size();
                for (size_t i = 0; i < avgPrototype.size(); ++i) {
                    avgPrototype[i] *= avgWeight;
                }
                
                float sim = computeSimilarity(pattern, avgPrototype);
                if (sim >= formationThreshold_ * 0.8f) {  // Medium threshold
                    novelAtMediumScale = false;
                    break;
                }
            }
        }
    }
    
    // Large scale: check against all stored instances
    if (novelAtMediumScale) {
        size_t instancesToCheck = std::min(size_t(20), stabilityWindow_);
        
        for (const auto& concept : concepts_) {
            if (concept.instances.size() < instancesToCheck) continue;
            
            // Sample some instances from this concept
            size_t startIdx = concept.instances.size() - instancesToCheck;
            std::vector<float> samplePattern;
            
            for (size_t i = startIdx; i < concept.instances.size(); ++i) {
                if (!samplePattern.empty() && samplePattern.size() == pattern.size()) break;
                
                if (!samplePattern.empty()) {
                    for (size_t j = 0; j < pattern.size() && j < samplePattern.size(); ++j) {
                        samplePattern[j] = (samplePattern[j] + concept.instances[i].pattern[j]) / 2.0f;
                    }
                } else {
                    samplePattern = concept.instances[i].pattern;
                }
            }
            
            if (!samplePattern.empty()) {
                float sim = computeSimilarity(pattern, samplePattern);
                if (sim >= formationThreshold_ * 0.9f) {  // Large scale threshold
                    novelAtLargeScale = false;
                    break;
                }
            }
        }
    }
    
    return novelAtSmallScale && novelAtMediumScale && novelAtLargeScale;
}

size_t ConceptFormation::getOrCreateTemporaryConcept(
    const std::vector<float>& pattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime) {
    
    // Find the most similar temporary concept
    size_t bestTempConcept = 0;
    float bestSimilarity = 0.0f;
    
    // Look for recently created concepts (temporary ones)
    for (auto& concept : concepts_) {
        // Only consider concepts that are not yet stable
        if (!isConceptStable(concept.id)) {
            float sim = computeSimilarity(pattern, concept.prototype);
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestTempConcept = concept.id;
            }
        }
    }
    
    if (bestTempConcept > 0) {
        // Update existing temporary concept
        DiscoveredConcept* tempConcept = nullptr;
        for (auto& c : concepts_) {
            if (c.id == bestTempConcept) {
                tempConcept = &c;
                break;
            }
        }
        
        if (tempConcept) {
            updateConceptWithTemporalWeighting(bestTempConcept, pattern, features, reward, currentTime);
            return bestTempConcept;
        }
    }
    
    // Create new temporary concept (less strict requirements)
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
    instance.stability = 0.5f;  // Start with lower stability
    
    concept.instances.push_back(instance);
    concept.avgStability = 0.5f;
    concept.categoryHint = "temporary";
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created temporary concept " + std::to_string(concept.id));
    
    return concept.id;
}

void ConceptFormation::updateConceptWithTemporalWeighting(
    size_t conceptId,
    const std::vector<float>& newPattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime) {
    
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Compute temporal weighting for new instance
    float temporalWeight = 1.0f;
    if (concept->totalObservations > 0) {
        temporalWeight = std::exp(-static_cast<float>(currentTime - concept->totalObservations) / 100.0f);
    }
    
    // Add new instance with temporal weighting
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward * temporalWeight;  // Weight by temporal recency
    
    // Compute stability with temporal decay
    float stabilityBoost = 1.0f;
    if (concept->instances.size() > 1) {
        // Recent instances contribute more to stability
        float recentInstances = std::min(static_cast<float>(concept->instances.size()), 5.0f);
        float olderInstances = static_cast<float>(concept->instances.size()) - recentInstances;
        
        stabilityBoost = recentInstances / (recentInstances + olderInstances * 0.5f);
    }
    
    instance.stability = std::min(1.0f, concept->avgStability * stabilityBoost * temporalWeight);
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype with Hebbian learning and temporal weighting
    updatePrototypeWithHebbianLearning(conceptId, newPattern, temporalWeight);
    
    // Update concept-level statistics
    concept->avgStability = (concept->avgStability * 0.7f) + 
                           (instance.stability * 0.3f * temporalWeight);
    
    // Update category hint based on recent experience
    updateCategoryHint(conceptId, reward, temporalWeight);
    
    // Check for concept merging if this concept becomes too stable
    if (instance.stability > stabilityThreshold_ * 1.5f) {
        checkAndMergeSimilarConcepts(conceptId);
    }
}

void ConceptFormation::updatePrototypeWithHebbianLearning(
    size_t conceptId,
    const std::vector<float>& newPattern,
    float temporalWeight) {
    
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Adaptive learning rate based on temporal weight and stability
            float learningRate = 0.1f * temporalWeight;
            
            if (concept.prototype.size() == newPattern.size()) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    // Hebbian update: strengthen connections for correlated patterns
                    float correlation = concept.prototype[i] * newPattern[i];
                    float plasticity = std::max(0.0f, correlation) * learningRate;
                    
                    concept.prototype[i] += plasticity;
                    
                    // Keep within reasonable bounds
                    concept.prototype[i] = std::max(0.0f, std::min(concept.prototype[i], 1.0f));
                }
            }
            break;
        }
    }
}

void ConceptFormation::updateCategoryHint(size_t conceptId, float reward, float temporalWeight) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Update category hint based on recent reward patterns
    std::vector<std::string> newHints;
    
    if (reward > 0.5f) {
        newHints.push_back("rewarding");
    } else if (reward < -0.3f) {
        newHints.push_back("harmful");
    }
    
    if (!newHints.empty()) {
        // Weight hints by recency
        concept->categoryHint = newHints[0];
        
        // Add to hint history for long-term learning
        if (!concept->categoryHint.empty() && concept->categoryHint != "temporary") {
            // In full implementation, would track hint evolution over time
        }
    }
}

void ConceptFormation::checkAndMergeSimilarConcepts(size_t conceptId) {
    DiscoveredConcept* primaryConcept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            primaryConcept = &c;
            break;
        }
    }
    
    if (!primaryConcept) return;
    
    // Find concepts that are similar enough to merge
    std::vector<size_t> toMerge;
    
    for (auto& other : concepts_) {
        if (other.id == conceptId) continue;
        
        // Check similarity between prototypes
        float sim = computeSimilarity(primaryConcept->prototype, other.prototype);
        
        if (sim > 0.8f) {  // High similarity suggests they might be the same concept
            toMerge.push_back(other.id);
        }
    }
    
    if (!toMerge.empty()) {
        // Merge all similar concepts
        for (size_t otherId : toMerge) {
            DiscoveredConcept* otherConcept = nullptr;
            for (auto& c : concepts_) {
                if (c.id == otherId) {
                    otherConcept = &c;
                    break;
                }
            }
            
            if (otherConcept) {
                // Merge other into primary
                for (const auto& instance : otherConcept->instances) {
                    primaryConcept->instances.push_back(instance);
                    primaryConcept->totalObservations++;
                }
                
                // Average the prototypes
                if (primaryConcept->prototype.size() == otherConcept->prototype.size()) {
                    for (size_t i = 0; i < primaryConcept->prototype.size(); ++i) {
                        primaryConcept->prototype[i] = 
                            (primaryConcept->prototype[i] + otherConcept->prototype[i]) * 0.5f;
                    }
                }
                
                // Update stability
                primaryConcept->avgStability = 
                    (primaryConcept->avgStability + otherConcept->avgStability) * 0.5f;
                
                // Remove other concept
                concepts_.erase(
                    std::remove_if(concepts_.begin(), concepts_.end(),
                        [otherId](const DiscoveredConcept& c) { return c.id == otherId; }),
                    concepts_.end()
                );
            }
        }
        
        NLM_LOG_INFO("ConceptFormation: Merged " + std::to_string(toMerge.size()) + 
                     " concepts into " + std::to_string(conceptId));
    }
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