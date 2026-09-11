#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct ConceptFormation::Impl {
    // Integration with brain for neural dynamics
    Brain* brain;
    
    // Concept emergence tracking
    std::vector<float> conceptActivation;
    std::vector<float> conceptStability;
    std::vector<float> conceptCoherence;
    
    // Pattern memory and Hebbian learning
    std::vector<std::vector<float>> patternMatrix;
    std::vector<std::vector<float>> synapticWeights;
    std::vector<float> patternEnergies;
    
    // Time and development
    SimulationStep lastUpdateTime;
    SimulationStep currentTime;
    
    // Random number generation for novelty
    std::mt19937 rng;
    std::normal_distribution<float> noveltyDist;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> stabilityDist;
    
    // Learning parameters
    float learningRate;
    float noveltyThreshold;
    float coherenceThreshold;
    float stabilityThreshold;
    float patternThreshold;
    
    // Integration with brain systems
    float neuromodulationLevel;
    float predictionError;
    float developmentalStage;
    
    Impl()
        : brain(nullptr)
        , lastUpdateTime(0)
        , currentTime(0)
        , learningRate(0.01f)
        , noveltyThreshold(0.5f)
        , coherenceThreshold(0.7f)
        , stabilityThreshold(0.6f)
        , patternThreshold(0.3f)
        , neuromodulationLevel(1.0f)
        , predictionError(0.0f)
        , developmentalStage(0.0f)
        , rng(std::random_device{}())
        , noveltyDist(0.0f, 0.5f)
        , uniformDist(0.0f, 1.0f)
        , stabilityDist(0.5f, 0.2f)
    {
        // Initialize brain integration
        conceptActivation.reserve(1000);
        conceptStability.reserve(1000);
        conceptCoherence.reserve(1000);
        patternMatrix.reserve(1000);
        synapticWeights.reserve(1000);
        patternEnergies.reserve(1000);
    }
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl())
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
    
    // Initialize concept formation with brain parameters
    pImpl->patternThreshold = brain_->getConfig()->getOr<float>("concept_pattern_threshold", 0.3f);
    pImpl->noveltyThreshold = brain_->getConfig()->getOr<float>("concept_novelty_threshold", 0.5f);
    pImpl->coherenceThreshold = brain_->getConfig()->getOr<float>("concept_coherence_threshold", 0.7f);
    pImpl->learningRate = brain_->getConfig()->getOr<float>("concept_learning_rate", 0.01f);
    
    // Set up random generator with brain seed
    uint64_t seed = 42;
    if (auto seedOpt = brain_->getConfig()->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng.seed(seed);
    
    NLM_LOG_INFO("ConceptFormation initialized with brain integration");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    pImpl->currentTime = currentTime;
    
    if (pattern.empty() || !brain_) {
        return 0;
    }
    
    // Update brain integration state
    pImpl->predictionError = std::abs(reward);
    pImpl->neuromodulationLevel = std::clamp(pImpl->neuromodulationLevel + reward * 0.01f, 0.1f, 2.0f);
    
    // Store pattern in matrix for Hebbian learning
    pImpl->patternMatrix.push_back(pattern);
    pImpl->patternEnergies.push_back(reward);
    
    // Update synaptic weights through Hebbian learning
    updateHebbianWeights(pattern, reward);
    
    // Check for concept emergence
    size_t matchingConcept = getMatchingConcept(pattern, 0.6f);
    
    if (matchingConcept > 0) {
        // Enhance existing concept with new instance
        updateConcept(matchingConcept, pattern, features, reward);
        
        // Update concept activation based on pattern similarity
        float conceptActivation = computePatternActivation(pattern, matchingConcept);
        pImpl->conceptActivation.push_back(conceptActivation);
        
        return matchingConcept;
    }
    
    // Check if this pattern is novel enough to form a new concept
    if (isNovel(pattern, formationThreshold_)) {
        // But wait for enough evidence before committing to a concept
        // This mimics biological concept formation through multiple exposures
        
        // Create concept if pattern is sufficiently distinct
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
    
    // Create initial instance with neural energy and stability
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = pImpl->currentTime;
    instance.avgReward = reward;
    instance.stability = computePatternStability(pattern);
    
    concept.instances.push_back(instance);
    concept.avgStability = instance.stability;
    
    // Determine emergent category based on neural properties
    concept.categoryHint = determineEmergentCategory(pattern, reward, instance.stability);
    
    // Find similar concepts for association
    size_t similarConcept = getMatchingConcept(pattern, 0.5f);  // Lower threshold for association
    if (similarConcept > 0) {
        concept.associatedConceptIds.push_back(similarConcept);
    }
    
    concepts_.push_back(concept);
    
    // Initialize neural activation and coherence
    pImpl->conceptActivation.push_back(1.0f);
    pImpl->conceptStability.push_back(instance.stability);
    pImpl->conceptCoherence.push_back(1.0f);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " with stability " + std::to_string(instance.stability) +
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
    
    if (!concept) {
        NLM_LOG_WARNING("Attempted to update non-existent concept " + std::to_string(conceptId));
        return;
    }
    
    // Update prototype through Hebbian averaging
    updatePrototype(conceptId, newPattern);
    
    // Add new instance with neural properties
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = pImpl->currentTime;
    instance.avgReward = reward;
    instance.stability = computePatternStability(newPattern);
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update average stability
    float totalStability = 0.0f;
    for (const auto& inst : concept->instances) {
        totalStability += inst.stability;
    }
    concept->avgStability = totalStability / concept->instances.size();
    
    // Update category hint based on cumulative experience
    updateCategoryHint(concept, reward);
}

void ConceptFormation::updateHebbianWeights(const std::vector<float>& pattern, float reward) {
    // Hebbian learning: strengthen connections between co-activated patterns
    float learningWeight = pImpl->learningRate * std::abs(reward) * pImpl->neuromodulationLevel;
    
    size_t patternIdx = pImpl->patternMatrix.size() - 1;
    
    // Update synaptic weights with recently stored patterns
    for (size_t i = 0; i < pImpl->patternMatrix.size() - 1; ++i) {
        float similarity = computePatternSimilarity(pattern, pImpl->patternMatrix[i]);
        
        // Hebbian rule: strengthen weights for similar patterns
        float weightChange = learningWeight * similarity * pImpl->patternEnergies[i];
        pImpl->synapticWeights[i][patternIdx] += weightChange;
        pImpl->synapticWeights[patternIdx][i] += weightChange;
    }
    
    // Extend weight matrix
    if (patternIdx < pImpl->synapticWeights.size()) {
        pImpl->synapticWeights[patternIdx].resize(pImpl->patternMatrix.size(), 0.0f);
    } else {
        pImpl->synapticWeights.push_back(std::vector<float>(pImpl->patternMatrix.size(), 0.0f));
    }
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    if (concepts_.empty() || pattern.empty()) {
        return 0;
    }
    
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    // Only return match if above threshold
    if (bestSimilarity >= similarityThreshold) {
        return bestMatch;
    }
    
    return 0;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    DiscoveredConcept* c1 = nullptr;
    DiscoveredConcept* c2 = nullptr;
    
    for (auto& c : concepts_) {
        if (c.id == conceptA) c1 = &c;
        if (c.id == conceptB) c2 = &c;
    }
    
    if (!c1 || !c2 || conceptA == conceptB) {
        return;
    }
    
    // Merge concept B into concept A
    for (const auto& instance : c2->instances) {
        c1->instances.push_back(instance);
    }
    
    c1->totalObservations += c2->totalObservations;
    c1->associatedConceptIds.push_back(c2->id);
    
    // Update prototype as weighted average
    float totalWeight = static_cast<float>(c1->instances.size() + c2->instances.size());
    std::vector<float> mergedPrototype(c1->prototype.size(), 0.0f);
    
    // Add c1's prototype with its weight
    for (size_t i = 0; i < c1->prototype.size(); ++i) {
        mergedPrototype[i] += c1->prototype[i] * (c1->instances.size() / totalWeight);
    }
    
    // Add c2's prototype with its weight
    for (size_t i = 0; i < c2->prototype.size(); ++i) {
        if (i < mergedPrototype.size()) {
            mergedPrototype[i] += c2->prototype[i] * (c2->instances.size() / totalWeight);
        }
    }
    
    c1->prototype = mergedPrototype;
    
    // Recalculate average stability
    float totalStability = 0.0f;
    for (const auto& inst : c1->instances) {
        totalStability += inst.stability;
    }
    c1->avgStability = totalStability / c1->instances.size();
    
    // Update category hint
    updateCategoryHint(c1, c2->avgReward);
    
    // Remove concept B
    concepts_.erase(std::remove_if(concepts_.begin(), concepts_.end(),
                                   [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
                    concepts_.end());
    
    NLM_LOG_INFO("ConceptFormation: Merged concept " + std::to_string(conceptB) +
                 " into concept " + std::to_string(conceptA));
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    float cosineSimilarity = dotProduct / (normA * normB);
    
    // Add stability weight
    return cosineSimilarity * 0.8f + pImpl->uniformDist(pImpl->rng) * 0.2f;
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                              float similarityThreshold) const {
    if (pattern.empty() || concepts_.empty()) {
        return true;
    }
    
    float maxSimilarity = 0.0f;
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > maxSimilarity) {
            maxSimilarity = similarity;
        }
    }
    
    return maxSimilarity < similarityThreshold;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept || concept->instances.size() < 2) {
        return;
    }
    
    // Hebbian update: weighted average with more weight on recent instances
    float totalWeight = 0.0f;
    std::vector<float> newPrototype(concept->prototype.size(), 0.0f);
    
    // Weight older instances less (decay with time)
    for (const auto& instance : concept->instances) {
        float ageWeight = 1.0f / (1.0f + (pImpl->currentTime - instance.lastObserved) * 0.01f);
        totalWeight += ageWeight;
        
        for (size_t i = 0; i < newInstance.size() && i < concept->prototype.size(); ++i) {
            newPrototype[i] += newInstance[i] * ageWeight;
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t i = 0; i < newPrototype.size(); ++i) {
            newPrototype[i] /= totalWeight;
        }
    }
    
    concept->prototype = newPrototype;
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* concept = nullptr;
    for (const auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) {
        return false;
    }
    
    // Compute stability based on multiple factors
    float avgInstanceStability = concept->avgStability;
    
    // Check how many observations it has
    bool hasEnoughObservations = concept->totalObservations >= stabilityWindow_;
    
    // Check stability of activation
    float activationStability = 1.0f;
    if (!pImpl->conceptActivation.empty()) {
        float meanActivation = 0.0f;
        for (float activation : pImpl->conceptActivation) {
            meanActivation += activation;
        }
        meanActivation /= pImpl->conceptActivation.size();
        
        float variance = 0.0f;
        for (float activation : pImpl->conceptActivation) {
            float diff = activation - meanActivation;
            variance += diff * diff;
        }
        variance /= pImpl->conceptActivation.size();
        
        activationStability = 1.0f / (1.0f + variance);
    }
    
    // Overall stability score
    float stabilityScore = avgInstanceStability * 0.4f + 
                          (hasEnoughObservations ? 1.0f : 0.0f) * 0.3f +
                          activationStability * 0.3f;
    
    return stabilityScore >= stabilityThreshold_;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = nullptr;
    for (const auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) {
        return 0.0f;
    }
    
    return concept->avgStability;
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
    if (!concept) {
        return std::vector<float>();
    }
    
    return concept->prototype;
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) {
        static const std::vector<ConceptInstance> empty;
        return empty;
    }
    
    return concept->instances;
}

void ConceptFormation::clear() {
    concepts_.clear();
    pImpl->conceptActivation.clear();
    pImpl->conceptStability.clear();
    pImpl->conceptCoherence.clear();
    pImpl->patternMatrix.clear();
    pImpl->synapticWeights.clear();
    pImpl->patternEnergies.clear();
    nextConceptId_ = 1;
    
    NLM_LOG_INFO("ConceptFormation: All concepts cleared");
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) {
        return 0.0f;
    }
    
    // Compute how well the concept handles variations
    float totalSimilarity = 0.0f;
    size_t comparisonCount = 0;
    
    for (const auto& instance : concept->instances) {
        // Compare instances with each other
        for (const auto& other : concept->instances) {
            if (&instance != &other) {
                float similarity = computeSimilarity(instance.pattern, other.pattern);
                totalSimilarity += similarity;
                comparisonCount++;
            }
        }
    }
    
    if (comparisonCount == 0) {
        return 0.0f;
    }
    
    float avgSimilarity = totalSimilarity / comparisonCount;
    
    // Generalization ability: how similar are instances?
    // Lower similarity means better generalization to variations
    return 1.0f - avgSimilarity;
}

float ConceptFormation::computePatternStability(const std::vector<float>& pattern) const {
    // Compute pattern stability based on neural properties
    float energy = 0.0f;
    float coherence = 0.0f;
    
    // Energy from pattern magnitude
    for (float value : pattern) {
        energy += value * value;
    }
    energy = std::sqrt(energy) / pattern.size();
    
    // Coherence from internal consistency
    if (pattern.size() >= 3) {
        for (size_t i = 0; i < pattern.size() - 2; ++i) {
            float trend = (pattern[i + 2] - pattern[i]) / 2.0f;
            coherence += 1.0f - std::abs(trend);
        }
        coherence /= (pattern.size() - 2);
    }
    
    // Add some noise for exploration
    float noise = pImpl->stabilityDist(pImpl->rng);
    
    // Combine with noise for stability
    return std::clamp((energy * 0.7f + coherence * 0.3f + noise) / 2.0f, 0.0f, 1.0f);
}

float ConceptFormation::computePatternActivation(const std::vector<float>& pattern, size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) {
        return 0.0f;
    }
    
    float similarity = computeSimilarity(pattern, concept->prototype);
    
    // Modulate by neuromodulation
    float modulation = 1.0f + pImpl->predictionError * 0.1f;
    
    return similarity * modulation;
}

std::string ConceptFormation::determineEmergentCategory(const std::vector<float>& pattern,
                                                      float reward,
                                                      float stability) const {
    // Derive category hint from pattern properties and reward
    std::string category;
    
    // High positive reward -> "rewarding"
    if (reward > 0.5f) {
        category = "rewarding";
    }
    // High negative reward -> "harmful"
    else if (reward < -0.3f) {
        category = "harmful";
    }
    // Stable patterns -> "stable"
    else if (stability > 0.8f) {
        category = "stable";
    }
    // Variable patterns -> "variable"
    else if (stability < 0.3f) {
        category = "variable";
    }
    // Medium reward and medium stability -> "neutral"
    else {
        category = "neutral";
    }
    
    return category;
}

void ConceptFormation::updateCategoryHint(DiscoveredConcept* concept, float reward) {
    // Gradually update category hint based on cumulative experience
    std::string newCategory;
    
    if (reward > 0.5f) {
        newCategory = "rewarding";
    } else if (reward < -0.3f) {
        newCategory = "harmful";
    } else {
        newCategory = "neutral";
    }
    
    // Smooth transition: mix old and new category
    if (concept->categoryHint != newCategory) {
        // Blend categories based on concept maturity
        float maturity = std::min(1.0f, static_cast<float>(concept->totalObservations) / 10.0f);
        
        // Gradually adopt new category
        if (maturity > 0.5f) {
            concept->categoryHint = newCategory;
        }
    }
}

} // namespace nlm