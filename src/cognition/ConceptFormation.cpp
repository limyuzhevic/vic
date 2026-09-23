#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    float curiosityFactor;
    float noveltyThreshold;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), curiosityFactor(0.5f), noveltyThreshold(0.3f) {
        std::random_device rd;
        rng.seed(rd());
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
    // Initialize with better default parameters
    pImpl->curiosityFactor = 0.5f;
    pImpl->noveltyThreshold = 0.3f;
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with a seed concept if brain is available
    if (brain_) {
        std::vector<float> seedPattern(brain_->getSensoryInputSize(), 0.3f);
        std::normal_distribution<float> noise(0.0f, 0.1f);
        for (float& val : seedPattern) {
            val += noise(pImpl->rng);
            val = std::max(0.0f, std::min(val, 1.0f));
        }
        
        // Create a "self" concept to bootstrap concept formation
        size_t selfConceptId = createConcept(seedPattern, std::vector<float>(seedPattern.size(), 0.5f), 0.0f);
        NLM_LOG_INFO("ConceptFormation: Created bootstrap concept with ID " + std::to_string(selfConceptId));
    }
    
    NLM_LOG_INFO("ConceptFormation initialized with neural substrate");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                         const std::vector<float>& features,
                                         float reward,
                                         SimulationStep currentTime) {
    if (pattern.empty() || !brain_) return 0;
    
    // Add curiosity-driven exploration to pattern
    float curiosityBonus = 0.0f;
    if (pImpl->brain && concepts_.size() > 1) {
        // Calculate novelty relative to existing concepts
        float novelty = 0.0f;
        float minSim = 1.0f;
        for (const auto& concept : concepts_) {
            float sim = computeSimilarity(pattern, concept.prototype);
            if (sim < minSim) minSim = sim;
        }
        
        // Higher curiosity for novel patterns
        curiosityBonus = (1.0f - minSim) * pImpl->curiosityFactor;
        
        // Slightly modify pattern based on curiosity for exploration
        if (curiosityBonus > pImpl->noveltyThreshold) {
            NLM_LOG_DEBUG("High curiosity pattern detected: novelty = " + std::to_string(curiosityBonus));
        }
    }
    
    // Check if this pattern matches any existing concept with higher accuracy
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept with confidence scoring
        DiscoveredConcept& concept = const_cast<DiscoveredConcept&>(*getConcept(matchingConcept));
        
        // Calculate concept stability and update
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        if (similarity >= formationThreshold_) {
            // Strong match - update concept
            updateConcept(matchingConcept, pattern, features, reward);
            
            // Add observation count bonus for stable concepts
            float stabilityBonus = getConceptStability(matchingConcept) * 0.1f;
            NLM_LOG_DEBUG("Concept " + std::to_string(matchingConcept) + 
                         " updated with similarity " + std::to_string(similarity) +
                         " and stability bonus " + std::to_string(stabilityBonus));
            
            return matchingConcept;
        } else {
            // Weak match - check if it might be a new variant
            if (isNovel(pattern, formationThreshold_ * 0.8f)) {
                // Create new concept for variant
                float variantQuality = similarity * 0.5f + (1.0f - similarity) * 0.5f;
                return createConcept(pattern, features, reward * variantQuality);
            }
        }
    }
    
    // No matching concept found - check if it's truly novel
    if (isNovel(pattern, formationThreshold_)) {
        // Higher threshold for novel concept formation
        if (concepts_.size() < 3 || currentTime % 100 == 0) {
            // Only create concepts occasionally at startup
            return createConcept(pattern, features, reward);
        }
    }
    
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                      const std::vector<float>& features,
                                      float reward) {
    if (pattern.empty()) return 0;
    
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    concept.avgStability = 1.0f;
    
    // Derive category hint from pattern properties and reward
    std::string category;
    
    // Calculate pattern statistics for categorization
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    // Classification based on pattern properties
    if (reward > 0.5f) {
        if (mean > 0.7f) category = "high_value_reward";
        else if (mean > 0.3f) category = "moderate_reward";
        else category = "low_value_reward";
    } else if (reward < -0.3f) {
        if (variance > 0.2f) category = "volatile_hazard";
        else category = "stable_hazard";
    } else {
        if (pattern.size() > 50) category = "complex_environment";
        else if (mean < 0.5f) category = "neutral_low";
        else category = "neutral_high";
    }
    
    concept.categoryHint = category;
    
    // Initialize with first instance
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " with category '" + concept.categoryHint + "', stability " + std::to_string(concept.avgStability));
    
    return concept.id;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                     const std::vector<float>& features, float reward) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Add new instance
            ConceptInstance instance;
            instance.pattern = newPattern;
            instance.features = features;
            instance.observationCount = 1;
            instance.lastObserved = concept.totalObservations;
            instance.avgReward = reward;
            
            concept.instances.push_back(instance);
            concept.totalObservations++;
            
            // Update prototype with learning rate
            updatePrototype(conceptId, newPattern);
            
            // Update stability based on recent instances
            float newStability = 1.0f;
            if (concept.instances.size() > 1) {
                // Calculate average similarity to current prototype
                float totalSim = 0.0f;
                size_t recentCount = std::min(concept.instances.size(), stabilityWindow_);
                
                for (size_t i = concept.instances.size() - recentCount; 
                     i < concept.instances.size(); ++i) {
                    totalSim += computeSimilarity(concept.instances[i].pattern, concept.prototype);
                }
                
                newStability = totalSim / recentCount;
                
                // Smooth stability update
                concept.avgStability = concept.avgStability * 0.8f + newStability * 0.2f;
            }
            
            // Update instance stability
            concept.instances.back().stability = newStability;
            
            // Adjust concept strength based on reward prediction accuracy
            if (reward > 0.5f && newStability > 0.8f) {
                // Reinforce strong concept with positive reward
                for (auto& inst : concept.instances) {
                    inst.stability = std::min(1.0f, inst.stability + 0.05f);
                }
            } else if (reward < -0.3f && newStability < 0.4f) {
                // Weaken weak concepts with negative reward
                for (auto& inst : concept.instances) {
                    inst.stability = std::max(0.1f, inst.stability - 0.05f);
                }
            }
            
            break;
        }
    }
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Hebbian-like prototype update with adaptive learning rate
            float learningRate = 0.15f;  // Adaptive based on stability
            float stability = concept.avgStability;
            learningRate = learningRate * stability * 0.8f + 0.04f;
            
            if (concept.prototype.size() == newInstance.size() && concept.prototype.size() > 0) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    // Exponential moving average for prototype
                    float oldVal = concept.prototype[i];
                    float newVal = newInstance[i];
                    
                    // Preserve diversity while converging
                    float pullTowardNew = (newVal - oldVal) * learningRate;
                    float diversityPreservation = 0.05f * (oldVal - 0.5f);  // Pull toward center
                    
                    concept.prototype[i] = oldVal + pullTowardNew + diversityPreservation;
                    
                    // Keep values in reasonable range
                    concept.prototype[i] = std::max(0.0f, std::min(concept.prototype[i], 1.0f));
                }
            }
            break;
        }
    }
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    if (concepts_.empty() || pattern.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (const auto& concept : concepts_) {
        float sim = computeSimilarity(pattern, concept.prototype);
        
        // Consider concept if similarity exceeds threshold
        if (sim > bestSimilarity && sim >= formationThreshold_) {
            bestSimilarity = sim;
            bestConcept = concept.id;
            
            // Early exit if we have a perfect match
            if (sim > 0.95f) break;
        }
    }
    
    return bestConcept;
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.size() < 2 || b.size() < 2) return 0.0f;
    
    // Use multiple similarity metrics for robustness
    float cosineSim = 0.0f, euclideanDist = 0.0f, correlation = 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA > 0.0001f && normB > 0.0001f) {
        cosineSim = dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    // Normalized Euclidean distance (inverted)
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        euclideanDist += diff * diff;
    }
    euclideanDist = std::exp(-euclideanDist / a.size()); // Convert to similarity
    
    // Pearson correlation coefficient
    float meanA = std::accumulate(a.begin(), a.end(), 0.0f) / a.size();
    float meanB = std::accumulate(b.begin(), b.end(), 0.0f) / b.size();
    
    float cov = 0.0f, varA = 0.0f, varB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float aDiff = a[i] - meanA;
        float bDiff = b[i] - meanB;
        cov += aDiff * bDiff;
        varA += aDiff * aDiff;
        varB += bDiff * bDiff;
    }
    
    if (varA > 0.0001f && varB > 0.0001f) {
        correlation = cov / (std::sqrt(varA) * std::sqrt(varB));
    }
    
    // Combine metrics with weighted average
    float combinedSimilarity = cosineSim * 0.5f + euclideanDist * 0.3f + correlation * 0.2f;
    
    // Apply sigmoid to emphasize differences
    return 1.0f / (1.0f + std::exp(-10.0f * (combinedSimilarity - 0.5f)));
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
}

// SpatialRepresentation Implementation
struct SpatialRepresentation::Impl {
    Brain* brain;
    float explorationDrive;
    float placeCellLearningRate;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), explorationDrive(0.5f), placeCellLearningRate(0.1f) {
        std::random_device rd;
        rng.seed(rd());
    }
};

SpatialRepresentation::SpatialRepresentation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentX_(0)
    , currentY_(0)
    , gridSpacing_(1.0f)
{
    pImpl->explorationDrive = 0.5f;
    pImpl->placeCellLearningRate = 0.1f;
}

SpatialRepresentation::~SpatialRepresentation() = default;

void SpatialRepresentation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Create initial place cells at origin
    PlaceCell originCell;
    originCell.neuron = NeuronId(50000);
    originCell.x = 0.0f;
    originCell.y = 0.0f;
    originCell.fieldRadius = 1.0f;
    originCell.activation = 1.0f;
    
    placeCells_.push_back(originCell);
    
    NLM_LOG_INFO("SpatialRepresentation initialized with neural substrate");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    currentX_ = x;
    currentY_ = y;
    lastSensoryCue_ = sensoryCues;
    
    // Check for existing place cells nearby
    bool hasNearbyCell = false;
    float closestDistance = 1000.0f;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < closestDistance) {
            closestDistance = dist;
        }
        
        if (dist < pc.fieldRadius) {
            hasNearbyCell = true;
            
            // Update existing place cell
            pc.activation = std::min(1.0f, pc.activation + 0.1f);
            break;
        }
    }
    
    // Create new place cell if no existing one is nearby
    if (!hasNearbyCell && brain_) {
        // Add exploration-driven noise to new place cell formation
        std::normal_distribution<float> noiseX(0.0f, 0.2f * pImpl->explorationDrive);
        std::normal_distribution<float> noiseY(0.0f, 0.2f * pImpl->explorationDrive);
        
        float newX = x + noiseX(pImpl->rng);
        float newY = y + noiseY(pImpl->rng);
        
        PlaceCell newCell;
        newCell.neuron = NeuronId(placeCells_.size() + 50000);
        newCell.x = newX;
        newCell.y = newY;
        newCell.fieldRadius = 0.5f * (1.0f - pImpl->explorationDrive) + 1.0f * pImpl->explorationDrive;
        newCell.activation = 1.0f;
        
        placeCells_.push_back(newCell);
        
        // Inject current to strengthen this place cell
        brain_->injectCurrent(newCell.neuron, 3.0f * pImpl->explorationDrive);
        
        NLM_LOG_DEBUG("SpatialRepresentation: Created new place cell at (" + std::to_string(newX) + ", " + std::to_string(newY) + ")");
    }
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    return {currentX_, currentY_};
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    currentX_ += dx * pImpl->explorationDrive;
    currentY_ += dy * pImpl->explorationDrive;
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    float totalActivation = 0.0f;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        // Gaussian place field
        if (dist < pc.fieldRadius * 3.0f) {
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
    float sequenceLearningRate;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), sequenceLearningRate(0.1f) {
        std::random_device rd;
        rng.seed(rd());
    }
};

TemporalRelation::TemporalRelation()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    pImpl->sequenceLearningRate = 0.1f;
}

TemporalRelation::~TemporalRelation() = default;

void TemporalRelation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                      const std::vector<float>& eventB,
                                      float reward) {
    // Find existing sequence or create new one
    for (auto& link : sequences_) {
        if (link.fromEvent.size() == eventA.size() && link.toEvent.size() == eventB.size()) {
            float similarityA = 0.0f, similarityB = 0.0f;
            
            // Calculate pattern similarities
            similarityA = computePatternSimilarity(link.fromEvent, eventA);
            similarityB = computePatternSimilarity(link.toEvent, eventB);
            
            if (similarityA > 0.8f && similarityB > 0.8f) {
                // Update existing sequence
                link.strength = std::min(1.0f, link.strength + pImpl->sequenceLearningRate * reward);
                link.observationCount++;
                
                // Adjust strength based on reward sign
                if (reward > 0.5f) {
                    link.strength = std::min(1.0f, link.strength + 0.1f);
                } else if (reward < -0.3f) {
                    link.strength = std::max(0.0f, link.strength - 0.1f);
                }
                
                return;
            }
        }
    }
    
    // Create new sequence link
    SequenceLink newLink;
    newLink.fromEvent = eventA;
    newLink.toEvent = eventB;
    newLink.strength = 0.3f + reward * 0.1f;
    newLink.observationCount = 1;
    
    sequences_.push_back(newLink);
}

std::vector<float> TemporalRelation::predictNext(const std::vector<float>& eventA) const {
    float bestSimilarity = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() == eventA.size()) {
            float similarity = computePatternSimilarity(link.fromEvent, eventA);
            
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestPrediction = link.toEvent;
            }
        }
    }
    
    return bestPrediction;
}

std::vector<float> TemporalRelation::predictPrevious(const std::vector<float>& eventB) const {
    float bestSimilarity = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.toEvent.size() == eventB.size()) {
            float similarity = computePatternSimilarity(link.toEvent, eventB);
            
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestPrediction = link.fromEvent;
            }
        }
    }
    
    return bestPrediction;
}

float TemporalRelation::getPredictionConfidence(const std::vector<float>& event) const {
    size_t matchingCount = 0;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() == event.size()) {
            float similarity = computePatternSimilarity(link.fromEvent, event);
            if (similarity > 0.7f) {
                matchingCount++;
            }
        }
    }
    
    return std::min(1.0f, static_cast<float>(matchingCount) / 3.0f);
}

void TemporalRelation::clear() {
    sequences_.clear();
}

float TemporalRelation::computePatternSimilarity(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA > 0.0001f && normB > 0.0001f) {
        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }
    
    return 0.0f;
}

} // namespace nlm