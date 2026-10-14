#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    std::mt19937 rng;
    
    Impl() : brain(nullptr) {
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
    // Initialize with random seed for neural variation
    std::random_device rd;
    pImpl->rng.seed(rd());
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Enable concept formation for neural dynamics integration
    NLM_LOG_INFO("ConceptFormation initialized (neural concept emergence)");
    
    // Create initial concept for sensory input patterns
    if (brain_) {
        // Create concept for basic sensory patterns
        std::vector<float> initialPattern(10, 0.5f);
        std::vector<float> initialFeatures(5, 0.5f);
        presentExperience(initialPattern, initialFeatures, 0.0f, 0);
    }
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty() || !brain_) return 0;
    
    // Convert neural pattern to concept features using Hebbian learning
    std::vector<float> neuralFeatures = extractNeuralFeatures(pattern);
    
    // Enhance features with neuromodulation
    applyNeuromodulationToFeatures(neuralFeatures, reward);
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(neuralFeatures);
    
    if (matchingConcept > 0) {
        // Update existing concept
        updateConcept(matchingConcept, pattern, neuralFeatures, reward);
        
        // Neural consolidation through spike timing
        consolidateConcept(matchingConcept, currentTime);
        
        return matchingConcept;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(neuralFeatures, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConcept(pattern, neuralFeatures, reward);
    }
    
    return 0;  // Not yet classifiable
}

std::vector<float> ConceptFormation::extractNeuralFeatures(const std::vector<float>& pattern) {
    std::vector<float> features;
    features.reserve(8);
    
    if (pattern.empty()) return features;
    
    // Compute neural features from pattern using biologically plausible mechanisms
    
    // 1. Mean activation (global excitability)
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    features.push_back(mean);
    
    // 2. Variance (neural diversity)
    float variance = 0.0f;
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    features.push_back(std::sqrt(variance));
    
    // 3. Kurtosis (peakedness of activation)
    float kurtosis = 0.0f;
    for (float val : pattern) {
        float normalized = (val - mean) / std::sqrt(std::max(variance, 0.001f));
        kurtosis += normalized * normalized * normalized * normalized;
    }
    kurtosis -= 3.0f; // Excess kurtosis
    features.push_back(kurtosis);
    
    // 4. Skewness (asymmetry)
    float skewness = 0.0f;
    float stdDev = std::sqrt(std::max(variance, 0.001f));
    for (float val : pattern) {
        float normalized = (val - mean) / stdDev;
        skewness += normalized * normalized * normalized;
    }
    skewness /= pattern.size();
    features.push_back(skewness);
    
    // 5. Maximum activation (focus/saliency)
    float maxActivation = *std::max_element(pattern.begin(), pattern.end());
    features.push_back(maxActivation);
    
    // 6. Minimum activation
    float minActivation = *std::min_element(pattern.begin(), pattern.end());
    features.push_back(minActivation);
    
    // 7. Temporal coherence (if we have history)
    float coherence = computeTemporalCoherence(pattern);
    features.push_back(coherence);
    
    // 8. Reward prediction error (for learning)
    float rpe = computeRewardPredictionError(pattern);
    features.push_back(rpe);
    
    return features;
}

void ConceptFormation::applyNeuromodulationToFeatures(std::vector<float>& features, float reward) {
    // Apply dopaminergic neuromodulation to feature salience
    if (!brain_ || features.size() < 8) return;
    
    Dopamine* dopamine = brain_->getDopamine();
    if (!dopamine) return;
    
    float dopamineLevel = dopamine->getLevel();
    
    // Modulate feature weights based on dopamine
    for (size_t i = 0; i < std::min<size_t>(features.size(), 5); ++i) {
        // Higher dopamine increases feature salience for positive rewards
        float modulation = 1.0f + dopamineLevel * (reward > 0 ? 0.5f : -0.2f);
        features[i] *= modulation;
    }
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                      const std::vector<float>& features,
                                      float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    
    // Initialize prototype with neural noise for biological realism
    std::uniform_real_distribution<float> noiseDist(-0.1f, 0.1f);
    concept.prototype = pattern;
    for (float& val : concept.prototype) {
        val += noiseDist(pImpl->rng) * std::sqrt(val * (1.0f - val));  // Sigmoid-like noise
    }
    
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
    
    // Add neural connections to related concepts (if any)
    findAndAddAssociatedConcepts(concept);
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " (category: " + concept.categoryHint + ")");
    
    return concept.id;
}

void ConceptFormation::findAndAddAssociatedConcepts(DiscoveredConcept& concept) {
    // Find conceptually similar existing concepts through neural connectivity
    std::vector<size_t> associatedIds;
    
    for (const auto& other : concepts_) {
        if (other.id == concept.id) continue;  // Skip itself
        
        float similarity = computeSimilarity(concept.prototype, other.prototype);
        if (similarity > 0.6f) {  // Threshold for conceptual association
            associatedIds.push_back(other.id);
        }
    }
    
    concept.associatedConceptIds = associatedIds;
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
    
    // Neural plasticity: instance stability based on similarity to prototype
    float similarity = computeSimilarity(newPattern, concept->prototype);
    instance.stability = similarity * (1.0f + std::abs(reward)) / 2.0f;  // Reward affects stability
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype using neural plasticity rules
    updatePrototype(conceptId, newPattern);
    
    // Update stability with exponential moving average
    float newStability = instance.stability;
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
            // Neural plasticity: weight prototype towards new instance with decreasing learning rate
            float alpha = 0.1f / std::sqrt(concept->totalObservations + 1);  // Decreasing learning rate
            
            if (concept.prototype.size() == newInstance.size()) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    // Apply sigmoid-like constraint for biological plausibility
                    float oldVal = concept.prototype[i];
                    float newVal = newInstance[i];
                    
                    // Smooth update with noise
                    float noise = (concept->totalObservations % 7 == 0) ? 
                                  (pImpl->rng() % 100 - 50) / 100.0f * 0.05f : 0.0f;
                    
                    concept.prototype[i] = oldVal * (1.0f - alpha) + 
                                          newVal * alpha + noise;
                    
                    // Constrain to biologically plausible range
                    concept.prototype[i] = std::max(0.0f, std::min(1.0f, concept.prototype[i]));
                }
            }
            break;
        }
    }
}

void ConceptFormation::consolidateConcept(size_t conceptId, SimulationStep currentTime) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept || !brain_) return;
    
    // Neural consolidation through replay and synaptic strengthening
    for (auto& instance : concept->instances) {
        // Decay with time
        if (currentTime > instance.lastObserved) {
            float ageFactor = std::exp(-(currentTime - instance.lastObserved) * 0.001f);
            instance.stability *= ageFactor;
            
            // Age affects instance relevance
            if (instance.stability < 0.3f) {
                instance.stability = 0.3f;  // Floor to prevent complete decay
            }
        }
    }
    
    // Update concept average stability
    float totalStability = 0.0f;
    for (const auto& instance : concept->instances) {
        totalStability += instance.stability;
    }
    concept->avgStability = totalStability / concept->instances.size();
}

float ConceptFormation::computeTemporalCoherence(const std::vector<float>& pattern) {
    // Simple coherence measure based on pattern regularity
    if (pattern.size() < 3) return 0.0f;
    
    // Check for oscillatory components
    float maxVal = *std::max_element(pattern.begin(), pattern.end());
    float minVal = *std::min_element(pattern.begin(), pattern.end());
    
    // Coherence is inverse of range (narrow patterns are more coherent)
    float range = maxVal - minVal;
    return 1.0f / (1.0f + range);
}

float ConceptFormation::computeRewardPredictionError(const std::vector<float>& pattern) {
    // Simple RPE approximation based on pattern magnitude
    float magnitude = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    
    // Patterns with high magnitude might indicate surprise/RPE
    return std::min(1.0f, magnitude / pattern.size());
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

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
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
    
    // Merge B into A with neural weighting
    float bWeight = 0.3f;  // Neural weighting factor
    
    for (const auto& instance : b->instances) {
        a->instances.push_back(instance);
        a->totalObservations++;
        
        // Neural update of prototype
        for (size_t i = 0; i < a->prototype.size() && i < instance.pattern.size(); ++i) {
            a->prototype[i] = a->prototype[i] * (1.0f - bWeight) + 
                            instance.pattern[i] * bWeight;
        }
    }
    
    // Update stability
    float bAvgStability = 0.0f;
    for (const auto& instance : b->instances) {
        bAvgStability += instance.stability;
    }
    bAvgStability /= b->instances.size();
    
    a->avgStability = (a->avgStability * a->instances.size() + 
                      bAvgStability * b->instances.size()) / a->instances.size();
    
    // Remove B
    concepts_.erase(
        std::remove_if(concepts_.begin(), concepts_.end(),
                       [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
        concepts_.end()
    );
    
    NLM_LOG_INFO("ConceptFormation: Merged concepts " + std::to_string(conceptA) +
                 " and " + std::to_string(conceptB));
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
    
    // Add new instance with neural variation
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    // Add neural noise to instance for biological realism
    std::uniform_real_distribution<float> noiseDist(-0.05f, 0.05f);
    for (float& val : instance.pattern) {
        val += noiseDist(pImpl->rng);
    }
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype using neural plasticity
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
    
    // Update instance stability with reward modulation
    float rewardFactor = 1.0f + std::abs(reward);
    concept->instances.back().stability = newStability * rewardFactor;
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