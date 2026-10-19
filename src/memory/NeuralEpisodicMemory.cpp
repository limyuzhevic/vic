#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Neural encoding and storage
    std::vector<NeuronId> hippocampalNeurons;
    std::vector<NeuronId> neocorticalNeurons;
    std::vector<std::vector<float>> hippocampalPatternMatrix;
    std::vector<std::vector<float>> neocorticalPatternMatrix;
    
    // Replay and consolidation state
    std::deque<uint64_t> replayQueue;
    std::vector<uint64_t> consolidatedIds;
    SimulationStep nextReplayStep;
    
    // Source monitoring
    std::unordered_map<uint64_t, std::vector<float>> sourceConfidenceMaps;
    std::unordered_map<uint64_t, std::vector<NeuronId>> sourceNeuralSignatures;
    
    // Schema integration
    std::unordered_map<std::string, std::vector<float>> schemaPrototypes;
    std::vector<std::pair<uint64_t, std::string>> schemaBindings;
    
    // Pattern completion dynamics
    std::vector<float> attractorPositions;
    std::vector<float> attractorVelocities;
    std::vector<float> attractorEnergies;
    
    Impl() : brain(nullptr), nextReplayStep(0) {
        hippocampalPatternMatrix.reserve(100);
        neocorticalPatternMatrix.reserve(1000);
        attractorPositions.reserve(100);
        attractorVelocities.reserve(100);
        attractorEnergies.reserve(100);
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(10000)
    , replayEnabled_(true)
    , consolidationEnabled_(true)
    , temporalCompressionEnabled_(true)
    , hippocampalNeocorticalEnabled_(true)
    , prioritizationEnabled_(true)
    , selfReferentialProcessing_(true)
    , emotionalIntegrationEnabled_(true)
    , patternCompletionEnabled_(true)
    , interferenceManagementEnabled_(true)
    , schemaIntegrationEnabled_(true)
    , schemaMatchingEnabled_(true)
    , expansionFactor_(1.5f)
    , replayFrequency_(0.1f)
    , completionThreshold_(0.6f)
    , interferenceThreshold_(0.3f)
    , selfImportanceThreshold_(0.5f)
    , currentReconstructionEnergy_(0.0f)
    , lastReplayStep_(0)
    , lastConsolidationStep_(0)
    , totalConsolidations_(0)
    , totalReplays_(0)
    , totalUpdates_(0)
    , averageEncodingStrength_(0.0f)
    , averageRecallAccuracy_(0.0f) {
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural components
    if (brain_) {
        // Create hippocampal and neocortical neural populations
        size_t hippocampalSize = 100;
        size_t neocorticalSize = 1000;
        
        // Initialize pattern matrices
        pImpl->hippocampalPatternMatrix.resize(hippocampalSize, std::vector<float>(100, 0.0f));
        pImpl->neocorticalPatternMatrix.resize(neocorticalSize, std::vector<float>(1000, 0.0f));
        
        pImpl->attractorPositions.resize(100, 0.0f);
        pImpl->attractorVelocities.resize(100, 0.0f);
        pImpl->attractorEnergies.resize(100, 0.0f);
    }
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with enhanced capabilities");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Check if episode meets storage criteria
    if (!shouldStoreEpisode(episode)) {
        return;
    }
    
    // Create enhanced episode with comprehensive encoding
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    stored.lastAccess = 0;
    stored.consolidated = false;
    stored.active = true;
    stored.consolidationTime = 0;
    stored.lastReplay = 0;
    stored.replayCount = 0;
    stored.consolidationStrength = 0.5f;
    
    // Apply rich contextual encoding
    encodeEventWithContext(stored);
    updateTemporalContext(stored);
    updateSpatialContext(stored);
    updateSelfReferentialContext(stored);
    updateEmotionalContext(stored);
    updateSchemaContext(stored);
    
    // Encode as neural pattern
    encodeAsNeuralPattern(stored);
    
    // Apply memory gating based on priority
    gateMemoryStorage(stored);
    
    // Store the episode
    episodes_.push_back(stored);
    
    // Update statistics
    trackEncodingQuality(stored);
    updateStatistics();
    
    // Enforce capacity limits
    enforceCapacityLimits();
    
    // Schedule potential replay
    if (replayEnabled_) {
        float replayProbability = replayFrequency_ * stored.consolidationStrength;
        if (getRandomFloat(0, 1.0f) < replayProbability) {
            pImpl->replayQueue.push_back(stored.id);
        }
    }
    
    // Check for interference
    if (interferenceManagementEnabled_) {
        detectAndResolveInterference(stored);
    }
    
    NLM_LOG_DEBUG("Stored episode {} with id {} (priority: {:.3f})", 
                  episodes_.size(), stored.id, computeMemoryPriority(stored));
}

void NeuralEpisodicMemory::encodeEventWithContext(const EpisodicMemoryItem& episode) {
    // Enhanced encoding with contextual binding
    float baseEncoding = 1.0f;
    
    // Bind what (action, reward, prediction error)
    float actionEncoding = 1.0f;
    if (episode.action != ActionType::Wait) {
        actionEncoding = 0.8f;
    }
    
    float rewardEncoding = std::min(episode.reward, 1.0f) * 0.7f;
    float predictionEncoding = std::min(episode.predictionError, 1.0f) * 0.5f;
    
    // Bind where (spatial context)
    float spatialEncoding = 1.0f - std::sqrt(episode.positionX * episode.positionX + 
                                           episode.positionY * episode.positionY) / 100.0f;
    
    // Bind when (temporal context)
    float temporalEncoding = 1.0f - (episode.timestamp % 100) / 100.0f;
    
    // Bind self-referential context
    float selfEncoding = episode.selfReferential ? 0.9f : 0.3f;
    if (episode.selfReferential) {
        selfEncoding *= episode.selfImportance;
    }
    
    // Bind emotional context
    float emotionalEncoding = (episode.emotionalValence + 1.0f) * 0.5f * 
                             (episode.emotionalArousal + 1.0f) * 0.5f;
    
    // Bind schema context
    float schemaEncoding = 0.0f;
    for (const auto& schema : episode.associatedSchemas) {
        schemaEncoding += 0.3f;
    }
    
    // Combine contextual encodings
    float contextualStrength = (actionEncoding * 0.2f + rewardEncoding * 0.15f +
                               predictionEncoding * 0.15f + spatialEncoding * 0.15f +
                               temporalEncoding * 0.15f + selfEncoding * 0.1f +
                               emotionalEncoding * 0.1f + schemaEncoding * 0.05f);
    
    stored.precision = std::min(contextualStrength, 1.0f);
    stored.specificity = stored.precision * 0.8f;
    
    // Update feature weights based on context importance
    stored.featureWeights.clear();
    stored.featureWeights.resize(50, 0.0f);
    stored.featureWeights[0] = actionEncoding;
    stored.featureWeights[1] = spatialEncoding;
    stored.featureWeights[2] = temporalEncoding;
    stored.featureWeights[3] = selfEncoding;
    stored.featureWeights[4] = emotionalEncoding;
}

void NeuralEpisodicMemory::updateTemporalContext(EpisodicMemoryItem& episode) const {
    if (!temporalCompressionEnabled_) {
        return;
    }
    
    // Apply temporal compression/expansion
    float timeSinceCreation = static_cast<float>(episode.age);
    
    // Recent memories (0-10 steps): compressed
    if (timeSinceCreation <= 10.0f) {
        episode.temporalContextStart = episode.timestamp;
        episode.temporalContextEnd = episode.timestamp + static_cast<SimulationStep>(10.0f * expansionFactor_);
    }
    // Middle memories (11-100 steps): normal
    else if (timeSinceCreation <= 100.0f) {
        episode.temporalContextStart = episode.timestamp;
        episode.temporalContextEnd = episode.timestamp;
    }
    // Old memories (101+ steps): expanded
    else {
        episode.temporalContextStart = episode.timestamp - static_cast<SimulationStep>(50.0f * (1.0f / expansionFactor_));
        episode.temporalContextEnd = episode.timestamp + static_cast<SimulationStep>(50.0f * expansionFactor_);
    }
}

void NeuralEpisodicMemory::updateSpatialContext(EpisodicMemoryItem& episode) const {
    // Update spatial context based on position
    episode.spatialContextRadius = 50.0f;  // Fixed radius for now
    
    // Encode spatial features
    float spatialVector[4] = {
        std::cos(episode.orientation),
        std::sin(episode.orientation),
        episode.positionX / 100.0f,
        episode.positionY / 100.0f
    };
    
    episode.featureWeights[5] = spatialVector[0];
    episode.featureWeights[6] = spatialVector[1];
    episode.featureWeights[7] = spatialVector[2];
    episode.featureWeights[8] = spatialVector[3];
}

void NeuralEpisodicMemory::updateSelfReferentialContext(EpisodicMemoryItem& episode) const {
    if (!selfReferentialProcessing_) {
        return;
    }
    
    // Encode self-referential content
    if (episode.selfReferential) {
        // Create self-representation based on action and outcome
        episode.selfRepresentation.clear();
        episode.selfRepresentation.resize(50, 0.0f);
        
        // Encode action in self-representation
        float actionValue = 0.0f;
        switch (episode.action) {
            case ActionType::Wait: actionValue = 0.0f; break;
            case ActionType::Move: actionValue = 0.5f; break;
            case ActionType::Attack: actionValue = 0.8f; break;
            case ActionType::Defend: actionValue = 0.6f; break;
            case ActionType::Explore: actionValue = 0.7f; break;
            case ActionType::Interact: actionValue = 0.9f; break;
            case ActionType::None: actionValue = 0.1f; break;
        }
        
        episode.selfRepresentation[0] = actionValue;
        episode.selfRepresentation[1] = episode.reward;
        episode.selfRepresentation[2] = episode.novelty;
        
        // Encode self-importance
        episode.selfImportance = computeSelfImportance(episode);
        
        // Update feature weights
        episode.featureWeights[9] = episode.selfImportance;
    }
}

void NeuralEpisodicMemory::updateEmotionalContext(EpisodicMemoryItem& episode) const {
    if (!emotionalIntegrationEnabled_) {
        return;
    }
    
    // Encode emotional content
    episode.emotionalValence = getRandomFloat(-1.0f, 1.0f);
    episode.emotionalArousal = getRandomFloat(0.0f, 1.0f);
    
    // Encode reward prediction
    episode.rewardPrediction = episode.reward * (1.0f - episode.predictionError);
    
    // Update feature weights for emotional features
    episode.featureWeights[10] = episode.emotionalValence;
    episode.featureWeights[11] = episode.emotionalArousal;
    episode.featureWeights[12] = episode.rewardPrediction;
}

void NeuralEpisodicMemory::updateSchemaContext(EpisodicMemoryItem& episode) const {
    if (!schemaIntegrationEnabled_ || !schemaMatchingEnabled_) {
        return;
    }
    
    // Match episode to existing schemas
    episode.associatedSchemas.clear();
    episode.schemaActivation.clear();
    
    // Simple schema matching based on action and reward
    if (episode.reward > 0.5f && episode.action != ActionType::Wait) {
        episode.associatedSchemas.push_back("reward-seeking");
        episode.schemaActivation.push_back(0.8f);
    }
    
    if (episode.action == ActionType::Explore) {
        episode.associatedSchemas.push_back("exploration");
        episode.schemaActivation.push_back(0.9f);
    }
    
    if (episode.novelty > 0.7f) {
        episode.associatedSchemas.push_back("novelty");
        episode.schemaActivation.push_back(0.7f);
    }
    
    if (episode.selfReferential) {
        episode.associatedSchemas.push_back("self");
        episode.schemaActivation.push_back(0.85f);
    }
    
    // Update feature weights for schema features
    for (size_t i = 0; i < episode.associatedSchemas.size(); ++i) {
        episode.featureWeights[13 + i] = episode.schemaActivation[i];
    }
}

void NeuralEpisodicMemory::encodeAsNeuralPattern(EpisodicMemoryItem& episode) {
    // Encode episode as neural pattern in hippocampal and neocortical systems
    if (!brain_) {
        return;
    }
    
    // Create hippocampal pattern (sparse, highly specific)
    episode.hippocampalPattern.clear();
    episode.hippocampalPattern.resize(50, INVALID_NEURON_ID);
    episode.hippocampalActivation.clear();
    episode.hippocampalActivation.resize(50, 0.0f);
    
    for (size_t i = 0; i < 50 && i < episode.activeNeurons.size(); ++i) {
        episode.hippocampalPattern[i] = episode.activeNeurons[i];
        episode.hippocampalActivation[i] = episode.neuronActivations[i];
    }
    
    // Create neocortical pattern (distributed, generalized)
    episode.neocorticalPattern.clear();
    episode.neocorticalPattern.resize(200, INVALID_NEURON_ID);
    episode.neocorticalActivation.clear();
    episode.neocorticalActivation.resize(200, 0.0f);
    
    // Distribute activation across neocortical neurons based on episode features
    if (!episode.sensoryState.empty()) {
        for (size_t i = 0; i < 200; ++i) {
            float weight = 0.0f;
            
            // Distribute based on sensory features
            for (size_t j = 0; j < episode.sensoryState.size() && j < 50; ++j) {
                float featureValue = episode.sensoryState[j];
                float position = static_cast<float>(i) / 200.0f * 50.0f;
                float distance = std::abs(position - static_cast<float>(j));
                weight += featureValue * std::exp(-distance * 0.1f);
            }
            
            // Distribute based on action
            float actionWeight = 0.0f;
            switch (episode.action) {
                case ActionType::Wait: actionWeight = 0.1f; break;
                case ActionType::Move: actionWeight = 0.3f; break;
                case ActionType::Attack: actionWeight = 0.8f; break;
                case ActionType::Defend: actionWeight = 0.6f; break;
                case ActionType::Explore: actionWeight = 0.7f; break;
                case ActionType::Interact: actionWeight = 0.9f; break;
                case ActionType::None: actionWeight = 0.2f; break;
            }
            
            weight += actionWeight * 0.2f;
            weight += episode.reward * 0.3f;
            
            if (weight > 0.01f) {
                episode.neocorticalPattern[i] = NeuronId(static_cast<uint64_t>(i) + 50000);
                episode.neocorticalActivation[i] = weight;
            }
        }
    }
}

void NeuralEpisodicMemory::gateMemoryStorage(EpisodicMemoryItem& episode) {
    if (!prioritizationEnabled_) {
        return;
    }
    
    // Compute memory priority
    float priority = computeMemoryPriority(episode);
    
    // Gate storage based on priority and capacity
    if (priority < priorityWeights_.getOverallPriority() * 0.3f && 
        episodes_.size() >= maxEpisodes_ * 0.8f) {
        // Don't store low-priority episode if capacity is nearly full
        NLM_LOG_DEBUG("Gating storage for low priority episode ({:.3f})", priority);
        return;
    }
    
    // Apply priority-based encoding strength
    episode.precision = priority * 0.5f + 0.5f;
    episode.consolidationStrength = priority;
}

float NeuralEpisodicMemory::computeMemoryPriority(const EpisodicMemoryItem& episode) const {
    MemoryPriority priority;
    
    // Recency priority
    priority.temporalPriority = 1.0f / (episode.age + 1.0f);
    if (priority.temporalPriority > 1.0f) priority.temporalPriority = 1.0f;
    
    // Reward priority
    priority.rewardPriority = episode.reward;
    
    // Novelty priority
    priority.noveltyPriority = episode.novelty;
    
    // Self-referential priority
    priority.selfPriority = episode.selfReferential ? 
        (episode.selfImportance * 0.8f + 0.2f) : 0.0f;
    
    // Emotional priority
    priority.emotionalPriority = computeEmotional salience(episode);
    
    // Schema priority
    priority.schemaPriority = 0.0f;
    for (const auto& activation : episode.schemaActivation) {
        priority.schemaPriority += activation * 0.3f;
    }
    
    // Interference priority
    priority.interferencePriority = 1.0f - episode.interferenceScore;
    
    return priority.getOverallPriority();
}

void NeuralEpisodicMemory::detectAndResolveInterference(EpisodicMemoryItem& episode) {
    if (!interferenceManagementEnabled_) {
        return;
    }
    
    // Detect interference from existing episodes
    for (const auto& existingEpisode : episodes_) {
        if (existingEpisode.id == episode.id) continue;
        
        // Compute interference score
        float interference = 0.0f;
        
        // Temporal interference
        float timeDiff = std::abs(static_cast<float>(episode.timestamp) - 
                                 static_cast<float>(existingEpisode.timestamp));
        if (timeDiff < 10.0f) {
            interference += 0.3f * (10.0f - timeDiff) / 10.0f;
        }
        
        // Spatial interference
        float dx = episode.positionX - existingEpisode.positionX;
        float dy = episode.positionY - existingEpisode.positionY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < 20.0f) {
            interference += 0.4f * (20.0f - dist) / 20.0f;
        }
        
        // Similarity interference
        float similarity = 0.0f;
        if (!episode.sensoryState.empty() && !existingEpisode.sensoryState.empty() &&
            episode.sensoryState.size() == existingEpisode.sensoryState.size()) {
            for (size_t i = 0; i < episode.sensoryState.size(); ++i) {
                similarity += std::abs(episode.sensoryState[i] - existingEpisode.sensoryState[i]);
            }
            similarity /= static_cast<float>(episode.sensoryState.size());
        }
        
        interference += 0.3f * (1.0f - similarity);
        
        // Update interference score
        if (interference > episode.interferenceScore) {
            episode.interferers.insert(existingEpisode.id);
        }
    }
    
    // Reduce interference from source episodes
    for (uint64_t interfererId : episode.interferers) {
        for (auto& otherEpisode : episodes_) {
            if (otherEpisode.id == interfererId || otherEpisode.id == episode.id) {
                continue;
            }
            
            // Weak existing episode if it interferes
            if (otherEpisode.interferenceScore > 0.1f) {
                otherEpisode.interferenceScore *= 0.9f;
            }
        }
    }
}

float NeuralEpisodicMemory::computeInterferenceScore(const EpisodicMemoryItem& episode) const {
    float score = 0.0f;
    
    for (const auto& otherEpisode : episodes_) {
        if (otherEpisode.id == episode.id) continue;
        
        // Compute interference from other episodes
        float interference = 0.0f;
        
        // Temporal proximity
        float timeDiff = std::abs(static_cast<float>(episode.timestamp) - 
                                 static_cast<float>(otherEpisode.timestamp));
        if (timeDiff < 20.0f) {
            interference += 0.5f * (20.0f - timeDiff) / 20.0f;
        }
        
        // Spatial proximity
        float dx = episode.positionX - otherEpisode.positionX;
        float dy = episode.positionY - otherEpisode.positionY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < 15.0f) {
            interference += 0.5f * (15.0f - dist) / 15.0f;
        }
        
        score += interference;
    }
    
    return std::min(score, 1.0f);
}

void NeuralEpisodicMemory::reduceInterference(const EpisodicMemoryItem& source,
                                             EpisodicMemoryItem& target) const {
    // Reduce interference in target episode based on source episode
    float interference = 0.0f;
    
    // Compute interference from source
    float timeDiff = std::abs(static_cast<float>(target.timestamp) - 
                             static_cast<float>(source.timestamp));
    if (timeDiff < 10.0f) {
        interference += 0.5f * (10.0f - timeDiff) / 10.0f;
    }
    
    float dx = target.positionX - source.positionX;
    float dy = target.positionY - source.positionY;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 10.0f) {
        interference += 0.5f * (10.0f - dist) / 10.0f;
    }
    
    if (interference > 0.0f) {
        // Weaken target episode's features slightly
        for (float& weight : target.featureWeights) {
            weight *= (1.0f - interference * 0.1f);
        }
        
        target.precision *= (1.0f - interference * 0.05f);
        target.specificity *= (1.0f - interference * 0.05f);
    }
}

void NeuralEpisodicMemory::applyTemporalCompression(EpisodicMemoryItem& episode) {
    if (!temporalCompressionEnabled_) {
        return;
    }
    
    // Apply temporal compression based on age
    float ageRatio = static_cast<float>(episode.age) / 100.0f;
    
    if (ageRatio < 0.5f) {
        // Compress recent memories
        episode.temporalContextEnd = episode.timestamp + static_cast<SimulationStep>(10.0f * expansionFactor_);
    } else if (ageRatio < 1.0f) {
        // Normal range
        episode.temporalContextEnd = episode.timestamp;
    } else {
        // Expand old memories
        episode.temporalContextEnd = episode.timestamp + static_cast<SimulationStep>(50.0f * expansionFactor_);
    }
}

void NeuralEpisodicMemory::applyTemporalExpansion(EpisodicMemoryItem& episode) const {
    // Apply temporal expansion for retrieval
    episode.temporalContextStart = std::max(0ULL, episode.timestamp - 
                                           static_cast<SimulationStep>(50.0f * (1.0f / expansionFactor_)));
    episode.temporalContextEnd = episode.timestamp + 
                                 static_cast<SimulationStep>(50.0f * expansionFactor_);
}

void NeuralEpisodicMemory::encodeSelfReferentialContent(EpisodicMemoryItem& episode) {
    if (!selfReferentialProcessing_) {
        return;
    }
    
    episode.selfReferential = (getRandomFloat(0, 1.0f) < 0.3f);
    
    if (episode.selfReferential) {
        // Compute self-importance
        episode.selfImportance = computeSelfImportance(episode);
        
        // Create self-representation
        episode.selfRepresentation.clear();
        episode.selfRepresentation.resize(50, 0.0f);
        
        // Encode action in self-representation
        switch (episode.action) {
            case ActionType::Wait: episode.selfRepresentation[0] = 0.0f; break;
            case ActionType::Move: episode.selfRepresentation[0] = 0.5f; break;
            case ActionType::Attack: episode.selfRepresentation[0] = 0.8f; break;
            case ActionType::Defend: episode.selfRepresentation[0] = 0.6f; break;
            case ActionType::Explore: episode.selfRepresentation[0] = 0.7f; break;
            case ActionType::Interact: episode.selfRepresentation[0] = 0.9f; break;
            case ActionType::None: episode.selfRepresentation[0] = 0.1f; break;
        }
        
        episode.selfRepresentation[1] = episode.reward;
        episode.selfRepresentation[2] = episode.novelty;
        
        // Update feature weights
        episode.featureWeights[9] = episode.selfImportance;
    }
}

float NeuralEpisodicMemory::computeSelfImportance(const EpisodicMemoryItem& episode) const {
    float importance = 0.0f;
    
    // Action importance
    switch (episode.action) {
        case ActionType::Wait: importance += 0.1f; break;
        case ActionType::Move: importance += 0.3f; break;
        case ActionType::Attack: importance += 0.8f; break;
        case ActionType::Defend: importance += 0.6f; break;
        case ActionType::Explore: importance += 0.7f; break;
        case ActionType::Interact: importance += 0.9f; break;
        case ActionType::None: importance += 0.2f; break;
    }
    
    // Reward importance
    importance += episode.reward * 0.5f;
    
    // Novelty importance
    importance += episode.novelty * 0.3f;
    
    // Emotional importance
    float emotionalImportance = (episode.emotionalValence + 1.0f) * 0.5f * 
                               (episode.emotionalArousal + 1.0f) * 0.5f;
    importance += emotionalImportance * 0.4f;
    
    return std::min(importance, 1.0f);
}

void NeuralEpisodicMemory::matchToSchemas(EpisodicMemoryItem& episode) {
    if (!schemaIntegrationEnabled_ || !schemaMatchingEnabled_) {
        return;
    }
    
    // Clear existing schema bindings
    episode.associatedSchemas.clear();
    episode.schemaActivation.clear();
    
    // Simple schema matching based on features
    if (episode.reward > 0.5f && episode.action != ActionType::Wait) {
        episode.associatedSchemas.push_back("reward-seeking");
        episode.schemaActivation.push_back(0.8f);
    }
    
    if (episode.action == ActionType::Explore) {
        episode.associatedSchemas.push_back("exploration");
        episode.schemaActivation.push_back(0.9f);
    }
    
    if (episode.novelty > 0.7f) {
        episode.associatedSchemas.push_back("novelty");
        episode.schemaActivation.push_back(0.7f);
    }
    
    if (episode.selfReferential) {
        episode.associatedSchemas.push_back("self");
        episode.schemaActivation.push_back(0.85f);
    }
}

void NeuralEpisodicMemory::activateSchemas(const std::vector<std::string>& schemas,
                                           EpisodicMemoryItem& episode) {
    for (const auto& schema : schemas) {
        // Find or create schema prototype
        auto it = pImpl->schemaPrototypes.find(schema);
        if (it == pImpl->schemaPrototypes.end()) {
            // Create new schema prototype
            pImpl->schemaPrototypes[schema] = std::vector<float>(50, 0.0f);
            it = pImpl->schemaPrototypes.find(schema);
        }
        
        // Update schema prototype with episode features
        std::vector<float>& prototype = it->second;
        for (size_t i = 0; i < std::min(episode.featureWeights.size(), prototype.size()); ++i) {
            prototype[i] += episode.featureWeights[i] * 0.1f;
        }
        
        // Normalize
        float norm = 0.0f;
        for (float value : prototype) {
            norm += value * value;
        }
        norm = std::sqrt(norm);
        if (norm > 0.0f) {
            for (float& value : prototype) {
                value /= norm;
            }
        }
    }
}

void NeuralEpisodicMemory::integrateEmotionalContent(EpisodicMemoryItem& episode) {
    // Integrate emotional content into memory encoding
    float emotionalVector[3] = {
        (episode.emotionalValence + 1.0f) * 0.5f,
        (episode.emotionalArousal + 1.0f) * 0.5f,
        episode.rewardPrediction
    };
    
    // Add emotional features to episode
    episode.featureWeights[15] = emotionalVector[0];
    episode.featureWeights[16] = emotionalVector[1];
    episode.featureWeights[17] = emotionalVector[2];
    
    // Update self-representation with emotional content
    if (episode.selfReferential && episode.selfRepresentation.size() >= 3) {
        episode.selfRepresentation[10] = emotionalVector[0];
        episode.selfRepresentation[11] = emotionalVector[1];
        episode.selfRepresentation[12] = emotionalVector[2];
    }
}

float NeuralEpisodicMemory::computeEmotional salience(const EpisodicMemoryItem& episode) const {
    // Compute emotional salience based on valence and arousal
    return (episode.emotionalValence + 1.0f) * 0.5f * 
           (episode.emotionalArousal + 1.0f) * 0.5f;
}

void NeuralEpisodicMemory::transferToHippocampalIndex(const EpisodicMemoryItem& episode) {
    // Transfer episode to hippocampal index for pattern completion
    if (!hippocampalNeocorticalEnabled_ || !brain_) {
        return;
    }
    
    // Create hippocampal index entry
    uint64_t indexId = episode.id;
    
    // Store hippocampal pattern
    pImpl->hippocampalPatternMatrix[indexId % pImpl->hippocampalPatternMatrix.size()] = 
        episode.hippocampalActivation;
    
    // Create neural connections
    for (size_t i = 0; i < episode.hippocampalPattern.size(); ++i) {
        if (episode.hippocampalPattern[i] != INVALID_NEURON_ID) {
            pImpl->hippocampalNeurons.push_back(episode.hippocampalPattern[i]);
        }
    }
}

void NeuralEpisodicMemory::updateNeocorticalRepresentation(const EpisodicMemoryItem& episode) {
    // Update neocortical representation based on hippocampal input
    if (!hippocampalNeocorticalEnabled_ || !brain_) {
        return;
    }
    
    // Update neocortical pattern matrix
    size_t index = episode.id % pImpl->neocorticalPatternMatrix.size();
    pImpl->neocorticalPatternMatrix[index] = episode.neocorticalActivation;
    
    // Create neural connections
    for (size_t i = 0; i < episode.neocorticalPattern.size(); ++i) {
        if (episode.neocorticalPattern[i] != INVALID_NEURON_ID) {
            pImpl->neocorticalNeurons.push_back(episode.neocorticalPattern[i]);
        }
    }
}

void NeuralEpisodicMemory::synchronizeWithNeocorticalActivity() {
    // Synchronize hippocampal and neocortical activity
    if (!hippocampalNeocorticalEnabled_) {
        return;
    }
    
    // Simple synchronization: average activations
    for (size_t i = 0; i < std::min(pImpl->hippocampalPatternMatrix.size(), 
                                   pImpl->neocorticalPatternMatrix.size()); ++i) {
        // Interleave patterns
        for (size_t j = 0; j < pImpl->neocorticalPatternMatrix[i].size(); ++j) {
            if (j < pImpl->hippocampalPatternMatrix[i].size()) {
                float avgActivation = (pImpl->hippocampalPatternMatrix[i][j] + 
                                    pImpl->neocorticalPatternMatrix[i][j]) * 0.5f;
                pImpl->neocorticalPatternMatrix[i][j] = avgActivation;
            }
        }
    }
}

void NeuralEpisodicMemory::triggerReplay(EpisodicMemoryItem& episode) {
    if (!replayEnabled_ || !brain_) {
        return;
    }
    
    // Trigger replay of episode
    episode.replayCount++;
    episode.lastReplay = episode.age;
    
    // Reactivate hippocampal pattern
    for (size_t i = 0; i < episode.hippocampalActivation.size(); ++i) {
        if (i < episode.hippocampalPattern.size() && 
            episode.hippocampalPattern[i] != INVALID_NEURON_ID) {
            
            float activation = episode.hippocampalActivation[i];
            brain_->injectCurrent(episode.hippocampalPattern[i], activation * 3.0f);
        }
    }
    
    // Update consolidation strength
    episode.consolidationStrength = std::min(episode.consolidationStrength * 1.1f, 1.0f);
    
    totalReplays_++;
}

void NeuralEpisodicMemory::consolidateWithNeocortex(EpisodicMemoryItem& episode) {
    if (!hippocampalNeocorticalEnabled_ || !brain_) {
        return;
    }
    
    // Consolidate with neocortex based on replay
    float consolidationFactor = episode.consolidationStrength * episode.replayCount;
    
    // Update neocortical representation
    episode.neocorticalActivation.clear();
    episode.neocorticalActivation.resize(200, 0.0f);
    
    // Spread neocortical activation
    for (size_t i = 0; i < 200; ++i) {
        float activation = 0.0f;
        
        // Get contribution from hippocampal pattern
        size_t hippIndex = i % episode.hippocampalActivation.size();
        activation += episode.hippocampalActivation[hippIndex] * 0.3f;
        
        // Add general neocortical activation
        activation += 0.1f;
        
        // Add reward-based modulation
        activation += episode.reward * 0.2f;
        
        episode.neocorticalActivation[i] = activation;
    }
    
    // Mark as consolidated if strong enough
    if (consolidationFactor > 0.7f) {
        episode.consolidated = true;
        pImpl->consolidatedIds.push_back(episode.id);
        totalConsolidations_++;
    }
}

void NeuralEpisodicMemory::updateMemoryTrace(EpisodicMemoryItem& episode) {
    // Update memory trace based on new information (reconsolidation)
    float decayFactor = 0.95f;
    
    // Apply decay to trace
    for (float& activation : episode.hippocampalActivation) {
        activation *= decayFactor;
    }
    
    for (float& activation : episode.neocorticalActivation) {
        activation *= decayFactor;
    }
    
    // Increase age
    episode.age++;
    
    // Update last access time
    episode.lastAccess = episode.age;
    
    totalUpdates_++;
}

void NeuralEpisodicMemory::activateAttractorDynamics(const std::vector<float>& pattern) {
    // Activate attractor dynamics for pattern completion
    if (!patternCompletionEnabled_) {
        return;
    }
    
    patternCompletionState_.currentPattern = pattern;
    patternCompletionState_.attractorState = pattern;
    patternCompletionState_.converged = false;
    patternCompletionState_.completionSteps = 0;
    
    // Initialize attractor dynamics
    for (size_t i = 0; i < pImpl->attractorPositions.size(); ++i) {
        pImpl->attractorPositions[i] = pattern[i % pattern.size()];
        pImpl->attractorVelocities[i] = 0.0f;
        pImpl->attractorEnergies[i] = 0.0f;
    }
}

void NeuralEpisodicMemory::completePattern(const std::vector<float>& partialPattern,
                                           std::vector<float>& completed) const {
    // Complete pattern using attractor dynamics
    if (!patternCompletionEnabled_) {
        completed = partialPattern;
        return;
    }
    
    completed.clear();
    completed.resize(partialPattern.size(), 0.0f);
    
    // Use nearest attractor pattern
    float bestMatch = 0.0f;
    size_t bestAttractor = 0;
    
    for (size_t i = 0; i < pImpl->hippocampalPatternMatrix.size(); ++i) {
        const auto& attractor = pImpl->hippocampalPatternMatrix[i];
        if (attractor.empty()) continue;
        
        float similarity = 0.0f;
        for (size_t j = 0; j < std::min(partialPattern.size(), attractor.size()); ++j) {
            similarity += std::abs(partialPattern[j] - attractor[j]);
        }
        similarity = 1.0f - similarity / std::min(partialPattern.size(), attractor.size());
        
        if (similarity > bestMatch) {
            bestMatch = similarity;
            bestAttractor = i;
        }
    }
    
    // Complete pattern using best attractor
    const auto& bestAttractor = pImpl->hippocampalPatternMatrix[bestAttractor];
    if (!bestAttractor.empty()) {
        completed = bestAttractor;
    } else {
        completed = partialPattern;
    }
}

void NeuralEpisodicMemory::update(SimulationStep step, Timestamp time) {
    // Update memory system
    updateEpisodeAges();
    
    // Process replay queue
    std::vector<uint64_t> toReplay;
    for (uint64_t episodeId : pImpl->replayQueue) {
        // Find episode
        for (auto& episode : episodes_) {
            if (episode.id == episodeId) {
                triggerReplay(episode);
                toReplay.push_back(episodeId);
                break;
            }
        }
    }
    
    // Remove processed replays
    for (uint64_t episodeId : toReplay) {
        pImpl->replayQueue.erase(
            std::remove(pImpl->replayQueue.begin(), pImpl->replayQueue.end(), episodeId),
            pImpl->replayQueue.end()
        );
    }
    
    // Update pattern completion
    if (patternCompletionEnabled_ && !patternCompletionState_.currentPattern.empty()) {
        patternCompletionState_.completionSteps++;
        
        // Simple attractor convergence check
        if (patternCompletionState_.completionSteps > 10) {
            patternCompletionState_.converged = true;
        }
    }
    
    // Clean up inactive episodes
    if (step % 100 == 0) {
        cleanupInactiveEpisodes();
    }
    
    // Update hippocampal-neocortical synchronization
    if (hippocampalNeocorticalEnabled_ && step % 50 == 0) {
        synchronizeWithNeocorticalActivity();
    }
    
    // Apply temporal compression/expansion
    if (temporalCompressionEnabled_) {
        for (auto& episode : episodes_) {
            applyTemporalCompression(episode);
        }
    }
    
    // Update statistics
    updateStatistics();
}

void NeuralEpisodicMemory::integrateWithNeurons() {
    // Integrate with neural substrate
    if (!brain_) {
        return;
    }
    
    // Inject current based on memory traces
    for (auto& episode : episodes_) {
        // Reactivate based on consolidation strength
        if (episode.consolidated && episode.consolidationStrength > 0.7f) {
            for (size_t i = 0; i < episode.hippocampalPattern.size(); ++i) {
                if (episode.hippocampalPattern[i] != INVALID_NEURON_ID &&
                    episode.hippocampalActivation[i] > 0.1f) {
                    
                    float current = episode.hippocampalActivation[i] * 
                                   episode.consolidationStrength * 2.0f;
                    brain_->injectCurrent(episode.hippocampalPattern[i], current);
                }
            }
        }
    }
}

void NeuralEpisodicMemory::synchronizeWithHippocampalSystem() {
    // Synchronize with hippocampal system
    if (!hippocampalNeocorticalEnabled_ || !brain_) {
        return;
    }
    
    // Transfer consolidated memories to neocortex
    for (auto& episode : episodes_) {
        if (episode.consolidated) {
            // Update neocortical representation
            updateNeocorticalRepresentation(episode);
        }
    }
}

void NeuralEpisodicMemory::updateNeocorticalIntegrations() {
    // Update neocortical integrations
    if (!hippocampalNeocorticalEnabled_) {
        return;
    }
    
    // Update schema prototypes based on memory content
    for (auto& episode : episodes_) {
        for (const auto& schema : episode.associatedSchemas) {
            activateSchemas({schema}, episode);
        }
    }
}

float NeuralEpisodicMemory::computeContextualSimilarity(
    const std::vector<float>& query,
    const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) {
        return 0.0f;
    }
    
    if (query.size() != episode.sensoryState.size()) {
        return 0.0f;
    }
    
    // Enhanced similarity computation with contextual weighting
    float similarity = 0.0f;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < query.size(); ++i) {
        float weight = 1.0f;
        
        // Apply feature weights
        if (i < episode.featureWeights.size()) {
            weight = episode.featureWeights[i];
        }
        
        // Apply temporal weighting
        float temporalWeight = 1.0f;
        float timeDiff = std::abs(static_cast<float>(episode.timestamp) - 
                                 static_cast<float>(episode.age));
        if (timeDiff < 50.0f) {
            temporalWeight = 1.0f - timeDiff / 50.0f;
        }
        
        // Apply spatial weighting
        float spatialWeight = 1.0f;
        float dx = query[i] - episode.sensoryState[i];
        float dy = (i < 50) ? (0.0f - dx) : (0.0f - dx); // Simplified spatial
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < 20.0f) {
            spatialWeight = 1.0f - dist / 20.0f;
        }
        
        // Compute similarity with contextual weights
        float featureSimilarity = 1.0f - std::abs(query[i] - episode.sensoryState[i]);
        similarity += featureSimilarity * weight * temporalWeight * spatialWeight;
        totalWeight += weight * temporalWeight * spatialWeight;
    }
    
    if (totalWeight > 0.0f) {
        return similarity / totalWeight;
    }
    
    return 0.0f;
}

bool NeuralEpisodicMemory::matchesContextualCriteria(
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    const std::vector<std::string>* featureQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery,
    float minRelevance) const {
    
    // Check sensory similarity
    if (sensoryQuery && computeContextualSimilarity(*sensoryQuery, episode) < minRelevance) {
        return false;
    }
    
    // Check feature matching
    if (featureQuery) {
        bool featureMatch = false;
        for (const auto& feature : *featureQuery) {
            for (size_t i = 0; i < episode.featureDescriptors.size(); ++i) {
                if (episode.featureDescriptors[i] == feature) {
                    featureMatch = true;
                    break;
                }
            }
        }
        if (!featureMatch) {
            return false;
        }
    }
    
    // Check temporal criteria
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    // Check action criteria
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
}

void NeuralEpisodicMemory::cleanupInactiveEpisodes() {
    // Remove inactive episodes
    size_t initialSize = episodes_.size();
    
    for (auto it = episodes_.begin(); it != episodes_.end(); ) {
        if (!it->active || it->age > 1000) {
            it = episodes_.erase(it);
        } else {
            ++it;
        }
    }
    
    if (episodes_.size() < initialSize) {
        NLM_LOG_DEBUG("Cleaned up {} inactive episodes", initialSize - episodes_.size());
    }
}

void NeuralEpisodicMemory::compressOldEpisodes() {
    // Compress old episodes to save memory
    if (!temporalCompressionEnabled_) {
        return;
    }
    
    // Find episodes to compress
    for (auto& episode : episodes_) {
        if (episode.age > 500 && !episode.consolidated) {
            // Apply temporal compression
            applyTemporalCompression(episode);
            
            // Update compression tracking
            TemporalCompression compression;
            compression.originalStart = episode.timestamp - static_cast<SimulationStep>(50.0f);
            compression.originalEnd = episode.timestamp + static_cast<SimulationStep>(50.0f);
            compression.compressedStart = episode.temporalContextStart;
            compression.compressedEnd = episode.temporalContextEnd;
            compression.compressionRatio = expansionFactor_;
            compression.expansionFactor = 1.0f / expansionFactor_;
            
            pImpl->temporalCompressions_.push_back(compression);
            
            // Mark for consolidation
            episode.consolidated = true;
        }
    }
}

void NeuralEpisodicMemory::updateEpisodeAges() {
    // Update ages of all episodes
    for (auto& episode : episodes_) {
        episode.age++;
    }
}

void NeuralEpisodicMemory::enforceCapacityLimits() {
    // Remove oldest episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        // Remove oldest episode
        if (!episodes_.empty()) {
            episodes_.pop_front();
        }
    }
}

bool NeuralEpisodicMemory::shouldStoreEpisode(const EpisodicMemoryItem& episode) const {
    // Determine if episode should be stored based on multiple criteria
    float priority = computeMemoryPriority(episode);
    
    // Always store self-referential episodes
    if (episode.selfReferential && priority > selfImportanceThreshold_) {
        return true;
    }
    
    // Always store high-priority episodes
    if (priority > 0.8f) {
        return true;
    }
    
    // Store if memory has capacity
    if (episodes_.size() < maxEpisodes_) {
        return true;
    }
    
    // Store if episode is novel
    if (episode.novelty > 0.7f) {
        return true;
    }
    
    // Store if episode has emotional content
    if (computeEmotional salience(episode) > 0.5f) {
        return true;
    }
    
    return false;
}

void NeuralEpisodicMemory::removeEpisode(size_t index) {
    if (index < episodes_.size()) {
        episodes_.erase(episodes_.begin() + index);
    }
}

void NeuralEpisodicMemory::removeEpisodeById(uint64_t episodeId) {
    for (auto it = episodes_.begin(); it != episodes_.end(); ++it) {
        if (it->id == episodeId) {
            episodes_.erase(it);
            return;
        }
    }
}

void NeuralEpisodicMemory::updateStatistics() {
    // Update memory statistics
    if (episodes_.empty()) {
        return;
    }
    
    float totalPriority = 0.0f;
    float totalPrecision = 0.0f;
    float totalRecall = 0.0f;
    
    for (const auto& episode : episodes_) {
        totalPriority += computeMemoryPriority(episode);
        totalPrecision += episode.precision;
        totalRecall += episode.specificity;
    }
    
    size_t count = episodes_.size();
    averageEncodingStrength_ = totalPriority / count;
    averagePrecision_ = totalPrecision / count;
    averageRecall_ = totalRecall / count;
    
    // Smooth the averages
    averageEncodingStrength_ = averageEncodingStrength_ * 0.7f + 
                              getRandomFloat(0, 1.0f) * 0.3f;
    averagePrecision_ = averagePrecision_ * 0.7f + 
                       getRandomFloat(0, 1.0f) * 0.3f;
    averageRecall_ = averageRecall_ * 0.7f + 
                     getRandomFloat(0, 1.0f) * 0.3f;
}

void NeuralEpisodicMemory::trackEncodingQuality(const EpisodicMemoryItem& episode) {
    // Track encoding quality for statistics
    float encodingQuality = episode.precision * 0.5f + 
                           episode.specificity * 0.3f + 
                           episode.consolidationStrength * 0.2f;
    
    averageEncodingStrength_ = averageEncodingStrength_ * 0.9f + 
                              encodingQuality * 0.1f;
}

void NeuralEpisodicMemory::trackRecallAccuracy(const std::vector<const EpisodicMemoryItem*>& retrieved) const {
    // Track recall accuracy for statistics
    if (retrieved.empty()) {
        return;
    }
    
    float totalAccuracy = 0.0f;
    size_t count = 0;
    
    for (const auto* episode : retrieved) {
        if (episode) {
            // Simple accuracy estimate based on precision and specificity
            float accuracy = (episode->precision + episode->specificity) * 0.5f;
            totalAccuracy += accuracy;
            count++;
        }
    }
    
    if (count > 0) {
        float recallAccuracy = totalAccuracy / count;
        averageRecall_ = averageRecall_ * 0.9f + recallAccuracy * 0.1f;
    }
}

size_t NeuralEpisodicMemory::getConsolidatedCount() const {
    size_t count = 0;
    for (const auto& episode : episodes_) {
        if (episode.consolidated) {
            count++;
        }
    }
    return count;
}

size_t NeuralEpisodicMemory::getActiveCount() const {
    size_t count = 0;
    for (const auto& episode : episodes_) {
        if (episode.active) {
            count++;
        }
    }
    return count;
}

float NeuralEpisodicMemory::getAverageStrength() const {
    return averageEncodingStrength_;
}

float NeuralEpisodicMemory::getAveragePrecision() const {
    return averagePrecision_;
}

float NeuralEpisodicMemory::getAverageRecall() const {
    return averageRecall_;
}

void NeuralEpisodicMemory::logMemoryState() const {
    NLM_LOG_INFO("=== Episodic Memory State ===");
    NLM_LOG_INFO("Total episodes: {}", episodes_.size());
    NLM_LOG_INFO("Active episodes: {}", getActiveCount());
    NLM_LOG_INFO("Consolidated episodes: {}", getConsolidatedCount());
    NLM_LOG_INFO("Average encoding strength: {:.3f}", averageEncodingStrength_);
    NLM_LOG_INFO("Average precision: {:.3f}", averagePrecision_);
    NLM_LOG_INFO("Average recall: {:.3f}", averageRecall_);
    NLM_LOG_INFO("Replay enabled: {}", replayEnabled_);
    NLM_LOG_INFO("Consolidation enabled: {}", consolidationEnabled_);
    NLM_LOG_INFO("Temporal compression: {}", temporalCompressionEnabled_);
    NLM_LOG_INFO("Hippocampal-neocortical: {}", hippocampalNeocorticalEnabled_);
}

void NeuralEpisodicMemory::printMemoryStats() const {
    std::cout << "=== NeuralEpisodicMemory Statistics ===" << std::endl;
    std::cout << "Total Episodes: " << episodes_.size() << std::endl;
    std::cout << "Active Episodes: " << getActiveCount() << std::endl;
    std::cout << "Consolidated Episodes: " << getConsolidatedCount() << std::endl;
    std::cout << "Average Encoding Strength: " << std::fixed << std::setprecision(3) 
              << averageEncodingStrength_ << std::endl;
    std::cout << "Average Precision: " << std::fixed << std::setprecision(3) 
              << averagePrecision_ << std::endl;
    std::cout << "Average Recall: " << std::fixed << std::setprecision(3) 
              << averageRecall_ << std::endl;
    std::cout << "Replay Enabled: " << (replayEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Consolidation Enabled: " << (consolidationEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Temporal Compression: " << (temporalCompressionEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Hippocampal-Neocortical: " << (hippocampalNeocorticalEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Prioritization: " << (prioritizationEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Self-Referential: " << (selfReferentialProcessing_ ? "Yes" : "No") << std::endl;
    std::cout << "Emotional Integration: " << (emotionalIntegrationEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Pattern Completion: " << (patternCompletionEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Interference Management: " << (interferenceManagementEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Schema Integration: " << (schemaIntegrationEnabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Schema Matching: " << (schemaMatchingEnabled_ ? "Yes" : "No") << std::endl;
}

std::string NeuralEpisodicMemory::getMemorySummary() const {
    std::stringstream ss;
    ss << "NeuralEpisodicMemory Summary:" << std::endl;
    ss << "  Episodes: " << episodes_.size() << std::endl;
    ss << "  Active: " << getActiveCount() << std::endl;
    ss << "  Consolidated: " << getConsolidatedCount() << std::endl;
    ss << "  Encoding Strength: " << std::fixed << std::setprecision(3) << averageEncodingStrength_ << std::endl;
    ss << "  Precision: " << std::fixed << std::setprecision(3) << averagePrecision_ << std::endl;
    ss << "  Recall: " << std::fixed << std::setprecision(3) << averageRecall_ << std::endl;
    return ss.str();
}

// NeuralAssociativeMemory implementation
struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : patternNeurons_) {
        float simA = computeSimilarity(patternA, pn.second);
        float simB = computeSimilarity(patternB, pn.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                               const std::vector<float>& patternB,
                                               float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f) {
                nextNeurons.push_back(otherNeuron);
                visited.insert(otherNeuron);
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
