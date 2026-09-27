#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace nlm {

// Safe access macros for pImpl members
#define pImpl_safe pImpl_safe_impl

// Helper function for safe pImpl access
Impl* safe_pimpl_access(Impl* ptr) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pImpl access in ConceptFormation.cpp");
        return nullptr;
    }
    return ptr;
}

// Helper function for safe brain access
Brain* safe_brain_access(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Null Brain pointer in ConceptFormation.cpp");
        return nullptr;
    }
    return brain;
}

// Helper function for concept validation
bool validate_concept_parameters(const std::vector<float>& pattern,
                                 const std::vector<float>& features,
                                 float reward) {
    if (pattern.empty()) {
        NLM_LOG_ERROR("ConceptFormation: pattern cannot be empty");
        return false;
    }
    
    if (!validate_non_negative(reward, "reward", "ConceptFormation::presentExperience")) {
        NLM_LOG_ERROR("ConceptFormation: invalid reward value");
        return false;
    }
    
    return true;
}

// Helper function for similarity validation
bool validate_similarity(float similarity, const std::string& context) {
    if (similarity < 0.0f || similarity > 1.0f) {
        NLM_LOG_ERROR("Invalid similarity value in " + context + ": " + std::to_string(similarity));
        return false;
    }
    return true;
}

struct ConceptFormation::Impl {
    Brain* brain;
    std::vector<float> lastSensoryPattern;
    
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
    auto* safe_brain = safe_brain_access(brain);
    if (!safe_brain) {
        NLM_LOG_ERROR("ConceptFormation::initialize failed: null brain pointer");
        return;
    }
    
    pImpl->brain = safe_brain;
    pImpl->lastSensoryPattern.clear();
    brain_ = safe_brain;
    
    // Biological inspiration: Concept formation mimics category learning in the brain,
    // particularly hippocampal-neocortical interactions. Concepts are formed through
    // pattern completion and generalization, similar to how the brain extracts
    // regularities from experience. The implementation uses Hebbian-like learning
    // and stability mechanisms to ensure robust concept formation.
}

void ConceptFormation::update(const std::vector<float>& sensoryPattern, const std::vector<float>& neuralActivity, float reward) {
    try {
        // Update internal state with current sensory and neural patterns
        pImpl->lastSensoryPattern = sensoryPattern;
        
        // Process this experience to form/update concepts
        // The neuralActivity represents the current brain state
        size_t conceptId = presentExperience(neuralActivity, neuralActivity, reward, 0);
        
        if (conceptId > 0) {
            const DiscoveredConcept* concept = getConcept(conceptId);
            if (concept) {
                NLM_LOG_INFO("ConceptFormation: Updated concept " + std::to_string(conceptId) +
                             " with stability " + std::to_string(concept->avgStability));
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::update failed: exception - " + std::string(e.what()));
    }
}

void ConceptFormation::updateWithPredictionError(float predictionError) {
    try {
        // Update concepts based on prediction error
        // High prediction error may indicate the need to form new concepts
        // or update existing ones to better match reality
        
        for (auto& c : concepts_) {
            // Adjust concept stability based on prediction error
            if (predictionError > 0.5f) {  // High error - increase plasticity
                // Increase plasticity of stable concepts
                if (c.avgStability > 0.8f) {
                    c.avgStability *= 0.95f;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::updateWithPredictionError failed: exception - " + std::string(e.what()));
    }
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    // Validate parameters
    if (!validate_concept_parameters(pattern, features, reward)) {
        return 0;
    }
    
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::presentExperience failed: exception - " + std::string(e.what()));
        return 0;
    }
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::createConcept failed: exception - " + std::string(e.what()));
        return 0;
    }
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    try {
        DiscoveredConcept* concept = nullptr;
        for (auto& c : concepts_) {
            if (c.id == conceptId) {
                concept = &c;
                break;
            }
        }
        
        if (!concept) return;
        
        // Validate parameters
        if (!validate_concept_parameters(newPattern, features, reward)) {
            return;
        }
        
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
        
    try {
        // Update prototype using exponential moving average (Hebbian-like)
        // This implements biological Hebbian learning where synaptic connections
        // are strengthened between co-active neurons. The exponential moving average
        // gives more weight to recent experiences while retaining influence from
        // older memories, reflecting how the brain continuously updates its
        // internal models through experience.
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::updateConcept failed: exception - " + std::string(e.what()));
    }
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::updatePrototype failed: exception - " + std::string(e.what()));
    }
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    try {
        for (const auto& concept : concepts_) {
            if (concept.id == conceptId) {
                return &concept;
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::getConcept failed: exception - " + std::string(e.what()));
        return nullptr;
    }
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    try {
        const DiscoveredConcept* concept = getConcept(conceptId);
        if (concept) {
            return concept->prototype;
        }
        return {};
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::getConceptPrototype failed: exception - " + std::string(e.what()));
        return {};
    }
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(
    size_t conceptId) const {
    static std::vector<ConceptInstance> empty;
    try {
        const DiscoveredConcept* concept = getConcept(conceptId);
        if (concept) {
            return concept->instances;
        }
        return empty;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::getConceptInstances failed: exception - " + std::string(e.what()));
        return empty;
    }
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    try {
        const DiscoveredConcept* concept = getConcept(conceptId);
        if (concept) {
            return concept->avgStability;
        }
        return 0.0f;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::getConceptStability failed: exception - " + std::string(e.what()));
        return 0.0f;
    }
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::mergeConcepts failed: exception - " + std::string(e.what()));
    }
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                             float similarityThreshold) const {
    try {
        return findConceptForPattern(pattern) == 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::isNovel failed: exception - " + std::string(e.what()));
        return true; // Assume novel for safety
    }
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::findConceptForPattern failed: exception - " + std::string(e.what()));
        return 0;
    }
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                          const std::vector<float>& b) const {
    try {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        // Cosine similarity
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        float similarity = dot / (std::sqrt(normA) * std::sqrt(normB));
        
        // Validate similarity
        if (!validate_similarity(similarity, "computeSimilarity")) {
            return 0.0f;
        }
        
        return similarity;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::computeSimilarity failed: exception - " + std::string(e.what()));
        return 0.0f;
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    try {
        const DiscoveredConcept* concept = getConcept(conceptId);
        if (!concept) return false;
        
        return concept->avgStability >= stabilityThreshold_;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::isConceptStable failed: exception - " + std::string(e.what()));
        return false;
    }
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::getGeneralizationAbility failed: exception - " + std::string(e.what()));
        return 0.0f;
    }
}

void ConceptFormation::clear() {
    try {
        concepts_.clear();
        nextConceptId_ = 1;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConceptFormation::clear failed: exception - " + std::string(e.what()));
    }
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
    auto* safe_brain = safe_brain_access(brain);
    if (!safe_brain) {
        NLM_LOG_ERROR("SpatialRepresentation::initialize failed: null brain pointer");
        return;
    }
    
    pImpl->brain = safe_brain;
    brain_ = safe_brain;
    NLM_LOG_INFO("SpatialRepresentation initialized");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("SpatialRepresentation::recordPosition failed: exception - " + std::string(e.what()));
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
    try {
        placeCells_.clear();
        currentX_ = 0;
        currentY_ = 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("SpatialRepresentation::clear failed: exception - " + std::string(e.what()));
    }
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
    auto* safe_brain = safe_brain_access(brain);
    if (!safe_brain) {
        NLM_LOG_ERROR("TemporalRelation::initialize failed: null brain pointer");
        return;
    }
    
    pImpl->brain = safe_brain;
    brain_ = safe_brain;
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                       const std::vector<float>& eventB,
                                       float reward) {
    try {
        // Validate parameters
        if (!validate_non_negative(reward, "reward", "TemporalRelation::recordSequence")) {
            NLM_LOG_ERROR("TemporalRelation::recordSequence failed: invalid reward value");
            return;
        }
        
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("TemporalRelation::recordSequence failed: exception - " + std::string(e.what()));
    }
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
    try {
        sequences_.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("TemporalRelation::clear failed: exception - " + std::string(e.what()));
    }
}

} // namespace nlm
