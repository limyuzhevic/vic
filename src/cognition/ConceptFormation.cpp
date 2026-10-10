// ConceptFormation.cpp
// Implementation of concept discovery and category formation

#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ConceptFormation::Impl {
    // Clustering centroids for concept formation
    std::vector<std::vector<float>> centroids;
    
    // Sum of squared errors for stability assessment
    std::vector<float> sse;
    
    // Observations since last centroid update
    std::vector<size_t> observationCount;
    
    // Last time concept was updated
    std::vector<SimulationStep> lastUpdateTime;
    
    Impl() {}
};

ConceptFormation::ConceptFormation() : pImpl(new Impl) {
    nextConceptId_ = 0;
    formationThreshold_ = 0.7f;
    stabilityThreshold_ = 0.8f;
    stabilityWindow_ = 50;
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
    // Check if pattern belongs to existing concept
    size_t existingConcept = getMatchingConcept(pattern);
    
    if (existingConcept != 0) {
        // Update existing concept
        updateConcept(existingConcept, pattern, features, reward);
        lastUpdateTime_[existingConcept] = currentTime;
        return existingConcept;
    }
    
    // Check if pattern is stable enough to form new concept
    if (isNovel(pattern) && !isConceptStable(concepts_.size())) {
        return createConcept(pattern, features, reward);
    }
    
    // Too early to decide - return 0 (indeterminate)
    return 0;
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    size_t conceptId = concepts_.size();
    
    // Create new concept
    DiscoveredConcept concept;
    concept.id = conceptId;
    concept.prototype = pattern;
    concept.categoryHint = deriveCategoryHint(pattern, features);
    concept.totalObservations = 1;
    concept.avgStability = 1.0f;
    
    // Create first instance
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;  // Will be set when presentExperience is called
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    
    // Update internal structures
    concepts_.push_back(concept);
    pImpl->centroids.push_back(pattern);
    pImpl->sse.push_back(0.0f);
    pImpl->observationCount.push_back(1);
    pImpl->lastUpdateTime.push_back(0);
    
    nextConceptId_ = conceptId + 1;
    
    NLM_LOG_INFO("ConceptFormation created new concept " + std::to_string(conceptId) +
                 " with prototype size " + std::to_string(pattern.size()));
    
    return conceptId;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    if (concepts_.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (size_t i = 0; i < concepts_.size(); ++i) {
        // Compute similarity with concept prototype
        float similarity = computeSimilarity(pattern, concepts_[i].prototype);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestConcept = i;
        }
    }
    
    if (bestSimilarity > similarityThreshold) {
        return bestConcept;
    }
    
    return 0;  // No match
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    if (conceptId >= concepts_.size()) return;
    
    DiscoveredConcept& concept = concepts_[conceptId];
    
    // Update prototype using Hebbian averaging
    updatePrototype(conceptId, newPattern);
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;  // Will be set by caller
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    concept.totalObservations++;
    
    // Update internal structures
    pImpl->observationCount[conceptId]++;
    
    // Check stability
    if (isConceptStable(conceptId)) {
        concept.avgStability = 1.0f;
    }
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    if (conceptId >= concepts_.size()) return;
    
    DiscoveredConcept& concept = concepts_[conceptId];
    
    // Hebbian-style prototype update
    for (size_t i = 0; i < concept.prototype.size(); ++i) {
        if (i < newInstance.size()) {
            concept.prototype[i] = (concept.prototype[i] * concept.totalObservations + newInstance[i]) / 
                                 (concept.totalObservations + 1);
        }
    }
    
    // Update internal SSE
    if (pImpl->centroids.size() > conceptId) {
        pImpl->centroids[conceptId] = concept.prototype;
        pImpl->sse[conceptId] = computeSSE(concept);
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    if (conceptId >= concepts_.size() || conceptId >= pImpl->observationCount.size()) {
        return false;
    }
    
    // Check if concept has been observed enough times
    if (pImpl->observationCount[conceptId] < stabilityWindow_) {
        return false;
    }
    
    // Check if SSE is low (stable)
    if (pImpl->sse.size() <= conceptId) return false;
    
    float avgSSEPerFeature = pImpl->sse[conceptId] / concepts_[conceptId].prototype.size();
    return avgSSEPerFeature < stabilityThreshold_;
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    if (conceptId < concepts_.size()) {
        return &concepts_[conceptId];
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    if (conceptId < concepts_.size()) {
        return concepts_[conceptId].prototype;
    }
    return std::vector<float>();
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    static const std::vector<ConceptInstance> empty;
    if (conceptId < concepts_.size()) {
        return concepts_[conceptId].instances;
    }
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    if (conceptId < concepts_.size()) {
        return concepts_[conceptId].avgStability;
    }
    return 0.0f;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    if (conceptA >= concepts_.size() || conceptB >= concepts_.size()) return;
    if (conceptA == conceptB) return;
    
    // Merge concept B into concept A
    DiscoveredConcept& targetConcept = concepts_[conceptA];
    DiscoveredConcept& sourceConcept = concepts_[conceptB];
    
    // Merge instances
    targetConcept.instances.insert(targetConcept.instances.end(),
                                   sourceConcept.instances.begin(),
                                   sourceConcept.instances.end());
    
    // Update prototype
    updatePrototype(conceptA, sourceConcept.prototype);
    
    // Update other fields
    targetConcept.categoryHint = mergeCategoryHints(targetConcept.categoryHint, sourceConcept.categoryHint);
    targetConcept.totalObservations += sourceConcept.totalObservations;
    
    // Remove source concept
    concepts_.erase(concepts_.begin() + conceptB);
    
    // Remove internal structures
    pImpl->centroids.erase(pImpl->centroids.begin() + conceptB);
    pImpl->sse.erase(pImpl->sse.begin() + conceptB);
    pImpl->observationCount.erase(pImpl->observationCount.begin() + conceptB);
    pImpl->lastUpdateTime.erase(pImpl->lastUpdateTime.begin() + conceptB);
    
    // Update concept IDs
    for (auto& concept : concepts_) {
        if (concept.id > conceptB) {
            concept.id--;
        }
    }
    
    nextConceptId_ = concepts_.size();
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    if (conceptId >= concepts_.size()) return 0.0f;
    
    const auto& concept = concepts_[conceptId];
    if (concept.instances.empty()) return 0.0f;
    
    // Compute variance of instances
    std::vector<float> prototype = concept.prototype;
    float totalVariance = 0.0f;
    
    for (const auto& instance : concept.instances) {
        float variance = computeVariance(instance.pattern, prototype);
        totalVariance += variance;
    }
    
    float avgVariance = totalVariance / concept.instances.size();
    return std::max(0.0f, 1.0f - avgVariance);  // Convert to ability score
}

std::string ConceptFormation::deriveCategoryHint(const std::vector<float>& pattern,
                                               const std::vector<float>& features) {
    // Simple category hint based on pattern properties
    if (pattern.empty()) return "unknown";
    
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = computeVariance(pattern, std::vector<float>(pattern.size(), mean));
    
    if (variance < 0.01f) {
        return "stable";
    } else if (mean > 0.7f) {
        return "high_value";
    } else if (mean < 0.3f) {
        return "low_value";
    } else if (variance > 0.25f) {
        return "variable";
    } else {
        return "neutral";
    }
}

std::string ConceptFormation::mergeCategoryHints(const std::string& hint1, const std::string& hint2) {
    if (hint1 == hint2) return hint1;
    if (hint1 == "unknown") return hint2;
    if (hint2 == "unknown") return hint1;
    
    // Try to find common ground
    if ((hint1 == "stable" || hint1 == "variable") && (hint2 == "stable" || hint2 == "variable")) {
        return "stable";
    }
    
    if ((hint1 == "high_value" || hint1 == "low_value") && (hint2 == "high_value" || hint2 == "low_value")) {
        return (hint1 == "high_value") ? "high_value" : "low_value";
    }
    
    return "mixed";
}

float ConceptFormation::computeSSE(const DiscoveredConcept& concept) const {
    float sse = 0.0f;
    if (concept.instances.empty()) return 0.0f;
    
    float mean = std::accumulate(concept.prototype.begin(), concept.prototype.end(), 0.0f) / concept.prototype.size();
    
    for (const auto& instance : concept.instances) {
        for (size_t i = 0; i < concept.prototype.size() && i < instance.pattern.size(); ++i) {
            float diff = concept.prototype[i] - instance.pattern[i];
            sse += diff * diff;
        }
    }
    
    return sse;
}

float ConceptFormation::computeVariance(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float meanA = std::accumulate(a.begin(), a.end(), 0.0f) / a.size();
    float variance = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - meanA;
        variance += diff * diff;
    }
    
    return variance / a.size();
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern, float similarityThreshold) const {
    return getMatchingConcept(pattern, similarityThreshold) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern);
}

void ConceptFormation::clear() {
    concepts_.clear();
    pImpl->centroids.clear();
    pImpl->sse.clear();
    pImpl->observationCount.clear();
    pImpl->lastUpdateTime.clear();
    nextConceptId_ = 0;
}

// SpatialRepresentation.cpp implementation
SpatialRepresentation::SpatialRepresentation() {
    brain_ = nullptr;
    currentX_ = 0.0f;
    currentY_ = 0.0f;
    gridSpacing_ = 1.0f;
}

SpatialRepresentation::~SpatialRepresentation() = default;

void SpatialRepresentation::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("SpatialRepresentation initialized");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    lastSensoryCue_ = sensoryCues;
    
    // Check if this is a novel position
    float minDistance = 1000.0f;
    NeuronId nearestNeuron = INVALID_NEURON_ID;
    
    for (const auto& placeCell : placeCells_) {
        float distance = std::sqrt(std::pow(x - placeCell.x, 2) + std::pow(y - placeCell.y, 2));
        if (distance < minDistance) {
            minDistance = distance;
            nearestNeuron = placeCell.neuron;
        }
    }
    
    // If position is new, create new place cell
    if (minDistance > placeCellFieldRadius_ * 2.0f || placeCells_.empty()) {
        PlaceCell newCell;
        newCell.neuron = NeuronId(pImpl->placeCells.size() + 20000);
        newCell.x = x;
        newCell.y = y;
        newCell.fieldRadius = placeCellFieldRadius_;
        newCell.activation = 0.0f;
        
        placeCells_.push_back(newCell);
        
        // If we have enough place cells, try to form grid
        if (placeCells_.size() >= minPlaceCellsForGrid_) {
            assessGridFormation();
        }
        
        NLM_LOG_INFO("SpatialRepresentation created new place cell at (" + 
                     std::to_string(x) + ", " + std::to_string(y) + ")");
    } else {
        // Update existing place cell activation
        for (auto& placeCell : placeCells_) {
            if (placeCell.neuron == nearestNeuron) {
                // Place cell should fire when within its field
                float distance = std::sqrt(std::pow(x - placeCell.x, 2) + std::pow(y - placeCell.y, 2));
                if (distance <= placeCell.fieldRadius) {
                    placeCell.activation = 1.0f - (distance / placeCell.fieldRadius);
                } else {
                    placeCell.activation = 0.0f;
                }
                break;
            }
        }
    }
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    currentX_ += dx;
    currentY_ += dy;
    
    // Update place cell activations
    for (auto& placeCell : placeCells_) {
        float distance = std::sqrt(std::pow(currentX_ - placeCell.x, 2) + std::pow(currentY_ - placeCell.y, 2));
        if (distance <= placeCell.fieldRadius) {
            placeCell.activation = 1.0f - (distance / placeCell.fieldRadius);
        } else {
            placeCell.activation *= 0.9f;  // Decay over time
        }
    }
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    return std::make_pair(currentX_, currentY_);
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    for (const auto& placeCell : placeCells_) {
        float distance = std::sqrt(std::pow(x - placeCell.x, 2) + std::pow(y - placeCell.y, 2));
        if (distance <= placeCell.fieldRadius) {
            return 1.0f - (distance / placeCell.fieldRadius);
        }
    }
    
    return 0.0f;
}

std::vector<NeuronId> SpatialRepresentation::getPlaceNeuronsNear(float x, float y, float radius) const {
    std::vector<NeuronId> result;
    
    for (const auto& placeCell : placeCells_) {
        float distance = std::sqrt(std::pow(x - placeCell.x, 2) + std::pow(y - placeCell.y, 2));
        if (distance <= radius) {
            result.push_back(placeCell.neuron);
        }
    }
    
    return result;
}

void SpatialRepresentation::clear() {
    placeCells_.clear();
    currentX_ = 0.0f;
    currentY_ = 0.0f;
    gridSpacing_ = 1.0f;
    lastSensoryCue_.clear();
}

void SpatialRepresentation::assessGridFormation() {
    if (placeCells_.size() < 3) return;
    
    // Compute average distances between place cells
    std::vector<float> distances;
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        for (size_t j = i + 1; j < placeCells_.size(); ++j) {
            float dist = std::sqrt(std::pow(placeCells_[i].x - placeCells_[j].x, 2) +
                                  std::pow(placeCells_[i].y - placeCells_[j].y, 2));
            distances.push_back(dist);
        }
    }
    
    if (distances.empty()) return;
    
    float avgDistance = std::accumulate(distances.begin(), distances.end(), 0.0f) / distances.size();
    gridSpacing_ = avgDistance / 2.0f;  // Rough estimate
}

// TemporalRelation.cpp implementation
TemporalRelation::TemporalRelation() {
    brain_ = nullptr;
}

TemporalRelation::~TemporalRelation() = default;

void TemporalRelation::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("TemporalRelation initialized");
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                      const std::vector<float>& eventB,
                                      float reward) {
    SequenceLink link;
    link.fromEvent = eventA;
    link.toEvent = eventB;
    link.strength = 1.0f;
    link.observationCount = 1;
    
    sequences_.push_back(link);
    
    // Modulate strength based on reward
    if (reward > 0.0f) {
        link.strength *= (1.0f + reward);
    }
}

std::vector<float> TemporalRelation::predictNext(const std::vector<float>& eventA) const {
    float bestStrength = 0.0f;
    std::vector<float> bestConsequence;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() != eventA.size()) continue;
        
        float similarity = computeSimilarity(eventA, link.fromEvent);
        
        if (similarity > bestStrength) {
            bestStrength = similarity;
            bestConsequence = link.toEvent;
        }
    }
    
    if (bestStrength > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return std::vector<float>();  // No prediction
}

std::vector<float> TemporalRelation::predictPrevious(const std::vector<float>& eventB) const {
    float bestStrength = 0.0f;
    std::vector<float> bestPrevious;
    
    for (const auto& link : sequences_) {
        if (link.toEvent.size() != eventB.size()) continue;
        
        float similarity = computeSimilarity(eventB, link.toEvent);
        
        if (similarity > bestStrength) {
            bestStrength = similarity;
            bestPrevious = link.fromEvent;
        }
    }
    
    if (bestStrength > 0.5f && !bestPrevious.empty()) {
        return bestPrevious;
    }
    
    return std::vector<float>();  // No prediction
}

float TemporalRelation::getPredictionConfidence(const std::vector<float>& event) const {
    float totalStrength = 0.0f;
    
    for (const auto& link : sequences_) {
        float similarityA = computeSimilarity(event, link.fromEvent);
        float similarityB = computeSimilarity(event, link.toEvent);
        totalStrength += (similarityA + similarityB) / 2.0f;
    }
    
    return std::min(1.0f, totalStrength / sequences_.size());
}

void TemporalRelation::clear() {
    sequences_.clear();
}

} // namespace nlm
