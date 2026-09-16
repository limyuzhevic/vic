#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Additional integration with other brain systems
    AttentionalSelection* attention;
    NeuralWorkingMemory* workingMemory;
    
    Impl() : brain(nullptr), attention(nullptr), workingMemory(nullptr) {}
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

void ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // INTEGRATION 1: Connect concept formation to brain for neural pattern analysis
    // Analyze current brain state for context
    std::vector<float> brainContext = getBrainContext();
    
    // Combine experience with brain context
    std::vector<float> combinedPattern = combinePatterns(pattern, brainContext);
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(combinedPattern);
    
    if (matchingConcept > 0) {
        // Update existing concept with brain context
        updateConcept(matchingConcept, combinedPattern, features, reward, brainContext);
        return matchingConcept;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(combinedPattern, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConcept(combinedPattern, features, reward);
    }
    
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    // INTEGRATION 3: Link concept formation to working memory for concept storage and retrieval
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

// INTEGRATION 1: Connect concept formation to brain for neural pattern analysis
void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                     const std::vector<float>& features, float reward,
                                     const std::vector<float>& brainContext) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
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

// INTEGRATION 1: Connect concept formation to brain for neural pattern analysis
std::vector<float> ConceptFormation::getBrainContext() const {
    if (!brain_ || !brain_->getWorkingMemory()) return {};
    
    // Get neural activity patterns from working memory as brain context
    std::vector<float> context;
    const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
    
    for (size_t i = 0; i < memoryNeurons.size() && i < 256; ++i) {
        context.push_back(brain_->getWorkingMemory()->getNeuronActivation(memoryNeurons[i]));
    }
    
    // Fill with zeros if not enough data
    while (context.size() < 256) {
        context.push_back(0.0f);
    }
    
    return context;
}

// INTEGRATION 1: Connect concept formation to brain for neural pattern analysis
std::vector<float> ConceptFormation::combinePatterns(const std::vector<float>& pattern,
                                                     const std::vector<float>& context) const {
    if (pattern.empty() || context.empty()) return pattern;
    
    // Simple combination: weighted average
    std::vector<float> combined;
    combined.reserve(std::min(pattern.size(), context.size()));
    
    size_t minSize = std::min(pattern.size(), context.size());
    for (size_t i = 0; i < minSize; ++i) {
        // Combine with 70% pattern, 30% context for enrichment
        combined.push_back(pattern[i] * 0.7f + context[i] * 0.3f);
    }
    
    return combined;
}

// INTEGRATION 2: Integrate concept formation with attention system for concept-based selection
size_t ConceptFormation::getConceptBasedOnAttention() const {
    if (!pImpl->brain || !pImpl->attention) return 0;
    
    // Get attended neurons from attention system
    const auto& winners = pImpl->attention->getWinners();
    
    if (winners.empty()) return 0;
    
    // Find concept with highest similarity to attended neurons' activation patterns
    size_t bestConcept = 0;
    float bestScore = 0.0f;
    
    for (const auto& concept : concepts_) {
        float score = 0.0f;
        
        // For each concept instance, check similarity to attended neurons
        for (const auto& instance : concept.instances) {
            // Simple heuristic: if concept prototype overlaps with attended neurons
            for (size_t i = 0; i < std::min(instance.pattern.size(), 10); ++i) {
                if (instance.pattern[i] > 0.5f) {
                    score += 0.1f;
                }
        
        if (score > bestScore) {
            bestScore = score;
            bestConcept = concept.id;
        }
    
    return bestConcept;
}

// INTEGRATION 2: Integrate concept formation with attention system for concept-based selection
void ConceptFormation::applyAttentionToConcept(size_t conceptId) {
    if (!pImpl->brain || !pImpl->attention || conceptId == 0) return;
    
    const auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Apply attention bias to neurons in the concept's prototype
    for (size_t i = 0; i < concept->prototype.size() && i < 10; ++i) {
        if (concept->prototype[i] > 0.5f) {
            // Create a virtual neuron ID for concept index
            NeuronId virtualNeuron(conceptId * 1000 + i);
            // Apply top-down bias
            pImpl->attention->applyTopDownBias(virtualNeuron, 1.0f);
        }

// INTEGRATION 3: Link concept formation to working memory for concept storage and retrieval
void ConceptFormation::storeConceptInWorkingMemory(size_t conceptId) {
    if (!pImpl->brain || !pImpl->workingMemory || conceptId == 0) return;
    
    const auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Store concept prototype in working memory
    std::vector<float> conceptPattern = concept->prototype;
    
    // Add category hint as an additional feature
    conceptPattern.push_back(static_cast<float>(concept->categoryHint == "rewarding" ? 1.0f :
                                               concept->categoryHint == "harmful" ? -1.0f : 0.0f));
    
    // Store with appropriate strength based on stability
    pImpl->workingMemory->store(conceptPattern, concept->avgStability);
}

// INTEGRATION 3: Link concept formation to working memory for concept storage and retrieval
size_t ConceptFormation::retrieveConceptFromWorkingMemory(const std::vector<float>& pattern) const {
    if (!pImpl->brain || !pImpl->workingMemory || pattern.empty()) return 0;
    
    // Get current working memory content
    std::vector<float> memoryContent = pImpl->workingMemory->retrieve();
    
    // Find stored concept that best matches pattern
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    // This is a simplified approach - in reality, you'd need to track which patterns
    // correspond to which concept IDs in working memory
    for (size_t i = 0; i < memoryContent.size() && i < concepts_.size(); ++i) {
        const auto& concept = concepts_[i];
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestConcept = concept.id;
        }
    
    return bestConcept;
}

// INTEGRATION 3: Link concept formation to working memory for concept storage and retrieval
std::vector<size_t> ConceptFormation::getConceptsInWorkingMemory() const {
    std::vector<size_t> conceptIds;
    
    if (!pImpl->brain || !pImpl->workingMemory) return conceptIds;
    
    // Get memory content to infer which concepts are active
    std::vector<float> memoryContent = pImpl->workingMemory->retrieve();
    
    // Match memory patterns to known concepts
    for (const auto& concept : concepts_) {
        // Check if any of the concept's prototypes are present in memory
        for (const auto& instance : concept.instances) {
            float similarity = computeSimilarity(instance.pattern, memoryContent);
            if (similarity > 0.3f) { // Threshold for considering concept "in memory"
                conceptIds.push_back(concept.id);
                break;
            }
    
    return conceptIds;
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
            break;
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
        
        if (simA > 0.8f && simB > 0.8f) {
            // Update existing link
            link.strength = std::min(1.0f, link.strength + 0.1f);
            link.observationCount++;
            return;
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
    
    return std::min(1.0f, static_cast<float>(matchingCount) / 3.0f);
}

void TemporalRelation::clear() {
    sequences_.clear();
}

} // namespace nlm