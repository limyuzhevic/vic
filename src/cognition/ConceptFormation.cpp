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
    
    // Initialize concept abstraction from episodic memory
    setAbstractionEnabled(true);
    
    // Setup neuromodulation integration
    setNoveltyDetectionEnabled(true);
    
    // Setup working memory integration
    setWorkingMemoryIntegrationEnabled(true);
    
    // Setup attention system integration
    setAttentionIntegrationEnabled(true);
    
    // Setup prediction system integration
    setPredictionIntegrationEnabled(true);
    
    // Setup neural planner integration
    setPlannerIntegrationEnabled(true);
    
    // Setup episodic memory storage
    setEpisodicMemoryStorageEnabled(true);
    
    NLM_LOG_INFO("ConceptFormation initialized with enhanced integration");
}

void ConceptFormation::abstractionFromEpisodicMemory() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Retrieve relevant episodes from episodic memory for concept abstraction
    size_t currentStep = brain_->getCurrentTime();
    auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(20);
    
    // Extract patterns from episodes for concept formation
    std::vector<std::vector<float>> episodePatterns;
    for (const auto* episode : recentEpisodes) {
        if (!episode->sensoryState.empty()) {
            episodePatterns.push_back(episode->sensoryState);
        }
    }
    
    if (episodePatterns.size() < 2) return; // Need enough data for abstraction
    
    // Perform pattern abstraction using clustering
    std::vector<float> abstractedPattern = computePatternAbstraction(episodePatterns);
    
    // Create concept from abstracted pattern
    size_t conceptId = createConceptFromPattern(
        abstractedPattern,
        extractFeaturesFromEpisodes(recentEpisodes),
        0.5f // Average reward for abstracted concept
    );
    
    // Store abstracted concept
    if (conceptId > 0) {
        // Enhance concept with episodic context
        enhanceConceptWithEpisodicContext(conceptId, recentEpisodes);
    }
}

void ConceptFormation::storeConceptInEpisodicMemory(size_t conceptId) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Create episodic memory entry for the concept
    EpisodicMemoryItem conceptMemory;
    conceptMemory.timestamp = brain_->getCurrentTime();
    
    // Store concept prototype as sensory state
    conceptMemory.sensoryState = concept->prototype;
    
    // Store concept features
    conceptMemory.sensoryFeatures = concept->prototype;
    
    // Add concept metadata to features
    conceptMemory.contextualFeatures.resize(8);
    conceptMemory.contextualFeatures[0] = concept->totalObservations;  // Number of observations
    conceptMemory.contextualFeatures[1] = concept->avgStability;     // Stability score
    conceptMemory.contextualFeatures[2] = concept->instances.size();   // Instance count
    
    if (!concept->categoryHint.empty()) {
        // Convert category hint to feature (simple encoding)
        conceptMemory.contextualFeatures[3] = concept->categoryHint == "rewarding" ? 1.0f :
                                              concept->categoryHint == "harmful" ? -1.0f : 0.0f;
    }
    
    // Store concept ID as reward signal for retrieval
    conceptMemory.reward = conceptId;  // Use concept ID as reward for storage
    
    // Store novelty and neuromodulation levels
    conceptMemory.novelty = calculateConceptNovelty(conceptId);
    conceptMemory.dopamineLevel = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    conceptMemory.curiosityLevel = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    
    // Store prediction system data
    if (brain_->getPredictionSystem()) {
        conceptMemory.predictionError = brain_->getPredictionSystem()->getPredictionError();
        conceptMemory.predictionConfidence = brain_->getPredictionSystem()->getConfidence();
    }
    
    // Store working memory content
    if (brain_->getWorkingMemory()) {
        const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
        conceptMemory.workingMemoryContent.reserve(memoryNeurons.size());
        for (const auto& neuron : memoryNeurons) {
            float activation = brain_->getWorkingMemory()->getNeuronActivation(neuron);
            conceptMemory.workingMemoryContent.push_back(activation);
        }
    }
    
    // Store in episodic memory
    brain_->getEpisodicMemory()->storeEpisode(conceptMemory);
}

void ConceptFormation::integrateWithNeuralPlanner() {
    if (!brain_ || !brain_->getNeuralPlanner()) return;
    
    // Get current concepts
    const auto& concepts = getConcepts();
    if (concepts.empty()) return;
    
    // Select most stable and relevant concepts for planning
    std::vector<float> abstractGoals;
    size_t selectedCount = 0;
    
    for (const auto& concept : concepts) {
        if (isConceptStable(concept.id) && concept.totalObservations > 3) {
            // Convert concept properties to abstract goals
            float stability = getConceptStability(concept.id);
            float generalization = getGeneralizationAbility(concept.id);
            float relevance = (stability + generalization) / 2.0f;
            
            // Add goal representation based on concept properties
            abstractGoals.push_back(stability);
            abstractGoals.push_back(generalization);
            abstractGoals.push_back(relevance);
            
            selectedCount++;
            if (selectedCount >= 5) break; // Limit number of goals
        }
    }
    
    if (!abstractGoals.empty()) {
        brain_->getNeuralPlanner()->integrateWithConceptFormation(abstractGoals);
    }
}

void ConceptFormation::integrateWithNeuromodulation() {
    if (!brain_) return;
    
    // Get neuromodulation levels
    float dopamine = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    float curiosity = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    float novelty = brain_->getNovelty() ? brain_->getNovelty()->getLevel() : 0.0f;
    
    // Calculate concept novelty detection based on neuromodulation
    float neuromodulationSignal = (dopamine * 0.4f) + (curiosity * 0.3f) + (novelty * 0.3f);
    
    // Update concept formation parameters based on neuromodulation
    setFormationThreshold(0.5f + neuromodulationSignal * 0.2f);
    setStabilityThreshold(0.6f + neuromodulationSignal * 0.15f);
    
    // Trigger concept refinement when neuromodulation is high
    if (neuromodulationSignal > 0.7f) {
        refineConceptsBasedOnNeuromodulation(neuromodulationSignal);
    }
}

void ConceptFormation::integrateWithAttentionSystem() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Get attention weights and salience for concept focusing
    const auto& winners = brain_->getAttention()->getWinners();
    if (winners.empty()) return;
    
    // Focus concept formation on attended concepts
    float attentionFocus = 0.0f;
    size_t attendedConceptCount = 0;
    
    for (const auto& concept : concepts_) {
        // Check if concept matches attended neurons
        bool isAttended = false;
        for (size_t i = 0; i < concept.prototype.size() && i < winners.size(); ++i) {
            if (concept.prototype[i] > 0.5f) {
                // Check if this prototype neuron is in attention winners
                // (This is a simplified check - real implementation would use neural IDs)
                if (i < winners.size()) {
                    isAttended = true;
                    break;
                }
            }
        }
        
        if (isAttended) {
            attentionFocus += concept.avgStability;
            attendedConceptCount++;
        }
    }
    
    if (attendedConceptCount > 0) {
        attentionFocus /= attendedConceptCount;
        
        // Apply attention-based salience to concepts
        for (auto& concept : concepts_) {
            if (isConceptStable(concept.id)) {
                // Boost stability of attended concepts
                float attentionBonus = attentionFocus * 0.2f;
                concept.avgStability = std::min(1.0f, concept.avgStability + attentionBonus);
            }
        }
    }
}

void ConceptFormation::abstractionFromEpisodicMemory() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Retrieve relevant episodes from episodic memory for concept abstraction
    size_t currentStep = brain_->getCurrentTime();
    auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(20);
    
    // Extract patterns from episodes for concept formation
    std::vector<std::vector<float>> episodePatterns;
    for (const auto* episode : recentEpisodes) {
        if (!episode->sensoryState.empty()) {
            episodePatterns.push_back(episode->sensoryState);
        }
    }
    
    if (episodePatterns.size() < 2) return; // Need enough data for abstraction
    
    // Perform pattern abstraction using clustering
    std::vector<float> abstractedPattern = computePatternAbstraction(episodePatterns);
    
    // Create concept from abstracted pattern
    size_t conceptId = createConceptFromPattern(
        abstractedPattern,
        extractFeaturesFromEpisodes(recentEpisodes),
        0.5f // Average reward for abstracted concept
    );
    
    // Store abstracted concept
    if (conceptId > 0) {
        // Enhance concept with episodic context
        enhanceConceptWithEpisodicContext(conceptId, recentEpisodes);
    }
}

void ConceptFormation::storeConceptInEpisodicMemory(size_t conceptId) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Create episodic memory entry for the concept
    EpisodicMemoryItem conceptMemory;
    conceptMemory.timestamp = brain_->getCurrentTime();
    
    // Store concept prototype as sensory state
    conceptMemory.sensoryState = concept->prototype;
    
    // Store concept features
    conceptMemory.sensoryFeatures = concept->prototype;
    
    // Add concept metadata to features
    conceptMemory.contextualFeatures.resize(8);
    conceptMemory.contextualFeatures[0] = concept->totalObservations;  // Number of observations
    conceptMemory.contextualFeatures[1] = concept->avgStability;     // Stability score
    conceptMemory.contextualFeatures[2] = concept->instances.size();   // Instance count
    
    if (!concept->categoryHint.empty()) {
        // Convert category hint to feature (simple encoding)
        conceptMemory.contextualFeatures[3] = concept->categoryHint == "rewarding" ? 1.0f :
                                              concept->categoryHint == "harmful" ? -1.0f : 0.0f;
    }
    
    // Store concept ID as reward signal for retrieval
    conceptMemory.reward = conceptId;  // Use concept ID as reward for storage
    
    // Store novelty and neuromodulation levels
    conceptMemory.novelty = calculateConceptNovelty(conceptId);
    conceptMemory.dopamineLevel = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    conceptMemory.curiosityLevel = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    
    // Store prediction system data
    if (brain_->getPredictionSystem()) {
        conceptMemory.predictionError = brain_->getPredictionSystem()->getPredictionError();
        conceptMemory.predictionConfidence = brain_->getPredictionSystem()->getConfidence();
    }
    
    // Store working memory content
    if (brain_->getWorkingMemory()) {
        const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
        conceptMemory.workingMemoryContent.reserve(memoryNeurons.size());
        for (const auto& neuron : memoryNeurons) {
            float activation = brain_->getWorkingMemory()->getNeuronActivation(neuron);
            conceptMemory.workingMemoryContent.push_back(activation);
        }
    }
    
    // Store in episodic memory
    brain_->getEpisodicMemory()->storeEpisode(conceptMemory);
}

void ConceptFormation::integrateWithNeuralPlanner() {
    if (!brain_ || !brain_->getNeuralPlanner()) return;
    
    // Get current concepts
    const auto& concepts = getConcepts();
    if (concepts.empty()) return;
    
    // Select most stable and relevant concepts for planning
    std::vector<float> abstractGoals;
    size_t selectedCount = 0;
    
    for (const auto& concept : concepts) {
        if (isConceptStable(concept.id) && concept.totalObservations > 3) {
            // Convert concept properties to abstract goals
            float stability = getConceptStability(concept.id);
            float generalization = getGeneralizationAbility(concept.id);
            float relevance = (stability + generalization) / 2.0f;
            
            // Add goal representation based on concept properties
            abstractGoals.push_back(stability);
            abstractGoals.push_back(generalization);
            abstractGoals.push_back(relevance);
            
            selectedCount++;
            if (selectedCount >= 5) break; // Limit number of goals
        }
    }
    
    if (!abstractGoals.empty()) {
        brain_->getNeuralPlanner()->integrateWithConceptFormation(abstractGoals);
    }
}

void ConceptFormation::integrateWithNeuromodulation() {
    if (!brain_) return;
    
    // Get neuromodulation levels
    float dopamine = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    float curiosity = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
    float novelty = brain_->getNovelty() ? brain_->getNovelty()->getLevel() : 0.0f;
    
    // Calculate concept novelty detection based on neuromodulation
    float neuromodulationSignal = (dopamine * 0.4f) + (curiosity * 0.3f) + (novelty * 0.3f);
    
    // Update concept formation parameters based on neuromodulation
    setFormationThreshold(0.5f + neuromodulationSignal * 0.2f);
    setStabilityThreshold(0.6f + neuromodulationSignal * 0.15f);
    
    // Trigger concept refinement when neuromodulation is high
    if (neuromodulationSignal > 0.7f) {
        refineConceptsBasedOnNeuromodulation(neuromodulationSignal);
    }
}

void ConceptFormation::integrateWithAttentionSystem() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Get attention weights and salience for concept focusing
    const auto& winners = brain_->getAttention()->getWinners();
    if (winners.empty()) return;
    
    // Focus concept formation on attended concepts
    float attentionFocus = 0.0f;
    size_t attendedConceptCount = 0;
    
    for (const auto& concept : concepts_) {
        // Check if concept matches attended neurons
        bool isAttended = false;
        for (size_t i = 0; i < concept.prototype.size() && i < winners.size(); ++i) {
            if (concept.prototype[i] > 0.5f) {
                // Check if this prototype neuron is in attention winners
                // (This is a simplified check - real implementation would use neural IDs)
                if (i < winners.size()) {
                    isAttended = true;
                    break;
                }
            }
        }
        
        if (isAttended) {
            attentionFocus += concept.avgStability;
            attendedConceptCount++;
        }
    }
    
    if (attendedConceptCount > 0) {
        attentionFocus /= attendedConceptCount;
        
        // Apply attention-based salience to concepts
        for (auto& concept : concepts_) {
            if (isConceptStable(concept.id)) {
                // Boost stability of attended concepts
                float attentionBonus = attentionFocus * 0.2f;
                concept.avgStability = std::min(1.0f, concept.avgStability + attentionBonus);
            }
        }
    }
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