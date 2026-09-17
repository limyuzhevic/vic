#pragma once

/**
 * Standardized Test Workloads for Phase 4 Cognitive Systems
 *
 * This header defines standardized test workloads for benchmarking all Phase 4
 * cognitive systems. Each workload provides:
 *
 * - Standardized difficulty levels (Simple, Moderate, Complex, Challenging)
 * - Appropriate trial counts for meaningful statistical analysis
 * - Parameter ranges optimized for each cognitive system
 * - Reproducible test patterns
 * - Performance characteristics for benchmarking
 *
 * Key Features:
 * - Test patterns with known theoretical properties
 * - Difficulty scaling for progressive benchmarking
 * - Reproducible random seed usage
 * - Performance timing information
 * - Memory usage estimates
 * - Energy efficiency characteristics
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace nlm {

/**
 * Workload characteristics
 */
struct WorkloadCharacteristics {
    size_t estimatedNeurons;     // Estimated neural resources needed
    size_t estimatedMemoryMB;    // Estimated memory usage
    double estimatedTimePerTrial; // Estimated execution time per trial
    double energyPerTrial;       // Energy consumption per trial
    std::string complexity;      // "low", "medium", "high"
};

/**
 * Standardized test workload for Working Memory
 */
class WorkingMemoryWorkload {
public:
    enum class Difficulty {
        Simple,      // 10 patterns, 50 trials, fast retention
        Moderate,    // 20 patterns, 100 trials, medium retention
        Complex,     // 50 patterns, 200 trials, long retention
        Challenging   // 100 patterns, 500 trials, very long retention
    };
    
    struct WorkloadParams {
        size_t numPatterns;        // Number of distinct patterns to learn
        size_t trialsPerPattern;   // Trials per pattern for retention
        size_t maxDelaySteps;      // Maximum delay before recall
        float patternComplexity;   // 0.0-1.0, pattern entropy
        float retentionThreshold;  // Minimum retention for success
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numPatterns = 10;
                params.trialsPerPattern = 5;
                params.maxDelaySteps = 10;
                params.patternComplexity = 0.3f;
                params.retentionThreshold = 0.7f;
                break;
                
            case Difficulty::Moderate:
                params.numPatterns = 20;
                params.trialsPerPattern = 10;
                params.maxDelaySteps = 20;
                params.patternComplexity = 0.5f;
                params.retentionThreshold = 0.6f;
                break;
                
            case Difficulty::Complex:
                params.numPatterns = 50;
                params.trialsPerPattern = 20;
                params.maxDelaySteps = 50;
                params.patternComplexity = 0.7f;
                params.retentionThreshold = 0.5f;
                break;
                
            case Difficulty::Challenging:
                params.numPatterns = 100;
                params.trialsPerPattern = 40;
                params.maxDelaySteps = 100;
                params.patternComplexity = 0.9f;
                params.retentionThreshold = 0.4f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 1000;
                chars.estimatedMemoryMB = 50;
                chars.estimatedTimePerTrial = 0.1;
                chars.energyPerTrial = 10.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 5000;
                chars.estimatedMemoryMB = 200;
                chars.estimatedTimePerTrial = 0.5;
                chars.energyPerTrial = 50.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 20000;
                chars.estimatedMemoryMB = 800;
                chars.estimatedTimePerTrial = 2.0;
                chars.energyPerTrial = 200.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 100000;
                chars.estimatedMemoryMB = 4000;
                chars.estimatedTimePerTrial = 10.0;
                chars.energyPerTrial = 1000.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate test patterns for working memory
     */
    static std::vector<std::vector<float>> generateTestPatterns(
        size_t numPatterns,
        size_t patternSize,
        float complexity = 0.5f,
        uint64_t seed = 42) {
        
        std::vector<std::vector<float>> patterns;
        patterns.reserve(numPatterns);
        
        for (size_t i = 0; i < numPatterns; ++i) {
            std::vector<float> pattern(patternSize, 0.0f);
            
            // Determine sparsity based on complexity
            size_t numActive = static_cast<size_t>(patternSize * (0.3f + 0.4f * complexity));
            
            // Create pattern with active neurons
            for (size_t j = 0; j < numActive; ++j) {
                size_t idx = (i * 31 + j) % patternSize; // Simple pseudo-random
                pattern[idx] = 1.0f;
            }
            
            // Add some noise based on complexity
            if (complexity > 0.5f) {
                for (size_t j = 0; j < patternSize; ++j) {
                    if (pattern[j] == 0.0f) {
                        pattern[j] = (static_cast<float>(rand()) / RAND_MAX) * 0.1f;
                    }
                }
            }
            
            patterns.push_back(pattern);
        }
        
        return patterns;
    }
};

/**
 * Standardized test workload for Episodic Memory
 */
class EpisodicMemoryWorkload {
public:
    enum class Difficulty {
        Simple,      // 10 episodes, simple relationships
        Moderate,    // 50 episodes, moderate complexity
        Complex,     // 100 episodes, complex temporal relationships
        Challenging   // 500 episodes, very complex relationships
    };
    
    struct WorkloadParams {
        size_t numEpisodes;        // Total number of episodes to store
        size_t retrievalSteps;     // Steps between retrieval attempts
        float episodeComplexity;   // 0.0-1.0, sensory pattern complexity
        float temporalSpacing;    // Average time between episodes
        float rewardVariability;  // 0.0-1.0, variability in rewards
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numEpisodes = 10;
                params.retrievalSteps = 5;
                params.episodeComplexity = 0.3f;
                params.temporalSpacing = 10.0f;
                params.rewardVariability = 0.5f;
                break;
                
            case Difficulty::Moderate:
                params.numEpisodes = 50;
                params.retrievalSteps = 10;
                params.episodeComplexity = 0.5f;
                params.temporalSpacing = 20.0f;
                params.rewardVariability = 0.7f;
                break;
                
            case Difficulty::Complex:
                params.numEpisodes = 100;
                params.retrievalSteps = 20;
                params.episodeComplexity = 0.7f;
                params.temporalSpacing = 50.0f;
                params.rewardVariability = 0.8f;
                break;
                
            case Difficulty::Challenging:
                params.numEpisodes = 500;
                params.retrievalSteps = 50;
                params.episodeComplexity = 0.9f;
                params.temporalSpacing = 100.0f;
                params.rewardVariability = 0.9f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 2000;
                chars.estimatedMemoryMB = 100;
                chars.estimatedTimePerTrial = 0.2;
                chars.energyPerTrial = 20.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 10000;
                chars.estimatedMemoryMB = 500;
                chars.estimatedTimePerTrial = 1.0;
                chars.energyPerTrial = 100.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 50000;
                chars.estimatedMemoryMB = 2500;
                chars.estimatedTimePerTrial = 5.0;
                chars.energyPerTrial = 500.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 200000;
                chars.estimatedMemoryMB = 10000;
                chars.estimatedTimePerTrial = 25.0;
                chars.energyPerTrial = 2500.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate episodic memory items
     */
    static std::vector<EpisodicMemoryItem> generateMemoryItems(
        size_t numEpisodes,
        size_t sensorySize,
        float complexity = 0.5f,
        uint64_t seed = 42) {
        
        std::vector<EpisodicMemoryItem> episodes;
        episodes.reserve(numEpisodes);
        
        for (size_t i = 0; i < numEpisodes; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.age = 0;
            
            // Generate sensory state based on complexity
            std::vector<float> sensory(sensorySize, 0.0f);
            size_t numActive = static_cast<size_t>(sensorySize * (0.2f + 0.6f * complexity));
            
            for (size_t j = 0; j < numActive; ++j) {
                size_t idx = (i * 37 + j) % sensorySize;
                sensory[idx] = 1.0f;
            }
            
            // Add noise
            if (complexity > 0.5f) {
                for (size_t j = 0; j < sensorySize; ++j) {
                    if (sensory[j] == 0.0f) {
                        sensory[j] = (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
                    } else {
                        sensory[j] += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
                        sensory[j] = std::clamp(sensory[j], 0.0f, 1.0f);
                    }
                }
            }
            
            episode.sensoryState = sensory;
            episode.reward = (static_cast<float>(rand()) / RAND_MAX) * 1.0f;
            episode.positionX = static_cast<float>((i * 13) % 100) / 100.0f;
            episode.positionY = static_cast<float>((i * 17) % 100) / 100.0f;
            episode.orientation = (static_cast<float>(rand()) / RAND_MAX) * 6.28f;
            episode.action = static_cast<ActionType>(i % 6);
            
            episodes.push_back(episode);
        }
        
        return episodes;
    }
};

/**
 * Standardized test workload for Prediction System
 */
class PredictionWorkload {
public:
    enum class Difficulty {
        Simple,      // 2 patterns, short prediction horizon
        Moderate,    // 5 patterns, medium horizon
        Complex,     // 10 patterns, long horizon
        Challenging   // 20 patterns, very long horizon
    };
    
    struct WorkloadParams {
        size_t numPatterns;        // Number of distinct patterns to learn
        size_t predictionHorizon; // How many steps ahead to predict
        float sequenceStability;  // How predictable the sequences are
        float noiseLevel;         // Amount of noise in observations
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numPatterns = 2;
                params.predictionHorizon = 3;
                params.sequenceStability = 0.9f;
                params.noiseLevel = 0.1f;
                break;
                
            case Difficulty::Moderate:
                params.numPatterns = 5;
                params.predictionHorizon = 5;
                params.sequenceStability = 0.8f;
                params.noiseLevel = 0.2f;
                break;
                
            case Difficulty::Complex:
                params.numPatterns = 10;
                params.predictionHorizon = 10;
                params.sequenceStability = 0.7f;
                params.noiseLevel = 0.3f;
                break;
                
            case Difficulty::Challenging:
                params.numPatterns = 20;
                params.predictionHorizon = 20;
                params.sequenceStability = 0.6f;
                params.noiseLevel = 0.5f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 2000;
                chars.estimatedMemoryMB = 100;
                chars.estimatedTimePerTrial = 0.3;
                chars.energyPerTrial = 30.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 10000;
                chars.estimatedMemoryMB = 500;
                chars.estimatedTimePerTrial = 1.5;
                chars.energyPerTrial = 150.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 50000;
                chars.estimatedMemoryMB = 2500;
                chars.estimatedTimePerTrial = 7.5;
                chars.energyPerTrial = 750.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 200000;
                chars.estimatedMemoryMB = 10000;
                chars.estimatedTimePerTrial = 37.5;
                chars.energyPerTrial = 3750.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate temporal sequences for prediction
     */
    static std::vector<std::vector<float>> generateTemporalSequences(
        size_t numPatterns,
        size_t sequenceLength,
        float stability = 0.8f,
        uint64_t seed = 42) {
        
        std::vector<std::vector<float>> sequences;
        sequences.reserve(numPatterns);
        
        for (size_t i = 0; i < numPatterns; ++i) {
            std::vector<float> sequence(sequenceLength, 0.0f);
            
            // Create predictable sequence
            for (size_t j = 0; j < sequenceLength; ++j) {
                // Base pattern with some predictability
                float base = std::sin(static_cast<float>(j) * 0.1f);
                base = (base + 1.0f) * 0.5f;
                
                // Add stability factor (how much it changes)
                if (j > 0) {
                    float change = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * (1.0f - stability);
                    base += change;
                }
                
                // Add noise
                float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * stability;
                base += noise;
                
                sequence[j] = std::clamp(base, 0.0f, 1.0f);
            }
            
            sequences.push_back(sequence);
        }
        
        return sequences;
    }
};

/**
 * Standardized test workload for Concept Formation
 */
class ConceptFormationWorkload {
public:
    enum class Difficulty {
        Simple,      // 5 concepts, high similarity
        Moderate,    // 10 concepts, medium similarity
        Complex,     // 20 concepts, low similarity
        Challenging   // 50 concepts, very low similarity
    };
    
    struct WorkloadParams {
        size_t numConcepts;        // Number of concepts to discover
        size_t instancesPerConcept; // Instances per concept for stability
        float conceptSimilarity;  // Expected similarity between concepts
        float variationRange;    // Range of variation within a concept
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numConcepts = 5;
                params.instancesPerConcept = 20;
                params.conceptSimilarity = 0.8f;
                params.variationRange = 0.2f;
                break;
                
            case Difficulty::Moderate:
                params.numConcepts = 10;
                params.instancesPerConcept = 30;
                params.conceptSimilarity = 0.6f;
                params.variationRange = 0.3f;
                break;
                
            case Difficulty::Complex:
                params.numConcepts = 20;
                params.instancesPerConcept = 40;
                params.conceptSimilarity = 0.4f;
                params.variationRange = 0.5f;
                break;
                
            case Difficulty::Challenging:
                params.numConcepts = 50;
                params.instancesPerConcept = 60;
                params.conceptSimilarity = 0.2f;
                params.variationRange = 0.8f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 3000;
                chars.estimatedMemoryMB = 150;
                chars.estimatedTimePerTrial = 0.5;
                chars.energyPerTrial = 50.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 15000;
                chars.estimatedMemoryMB = 750;
                chars.estimatedTimePerTrial = 2.5;
                chars.energyPerTrial = 250.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 75000;
                chars.estimatedMemoryMB = 3750;
                chars.estimatedTimePerTrial = 12.5;
                chars.energyPerTrial = 1250.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 300000;
                chars.estimatedMemoryMB = 15000;
                chars.estimatedTimePerTrial = 62.5;
                chars.energyPerTrial = 6250.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate concept instances for testing
     */
    static std::vector<std::vector<float>> generateConceptInstances(
        size_t numConcepts,
        size_t instancesPerConcept,
        size_t featureSize,
        float similarity = 0.5f,
        float variation = 0.3f,
        uint64_t seed = 42) {
        
        std::vector<std::vector<float>> instances;
        instances.reserve(numConcepts * instancesPerConcept);
        
        for (size_t c = 0; c < numConcepts; ++c) {
            // Create concept prototype
            std::vector<float> prototype(featureSize, 0.0f);
            size_t numActive = static_cast<size_t>(featureSize * 0.3f);
            
            for (size_t j = 0; j < numActive; ++j) {
                size_t idx = (c * 31 + j) % featureSize;
                prototype[idx] = 1.0f;
            }
            
            for (size_t i = 0; i < instancesPerConcept; ++i) {
                std::vector<float> instance = prototype;
                
                // Add variation
                for (size_t j = 0; j < featureSize; ++j) {
                    if (instance[j] == 1.0f) {
                        // Occasional dropout
                        if ((static_cast<float>(rand()) / RAND_MAX) < variation * 0.3f) {
                            instance[j] = 0.0f;
                        }
                    } else {
                        // Occasional addition
                        if ((static_cast<float>(rand()) / RAND_MAX) < variation * 0.2f) {
                            instance[j] = (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
                        }
                    }
                }
                
                // Add concept-to-concept similarity
                if (c > 0 && similarity > 0.0f) {
                    // Copy some features from previous concept
                    size_t numShared = static_cast<size_t>(featureSize * similarity);
                    for (size_t j = 0; j < numShared; ++j) {
                        size_t idx = (c * 17 + j) % featureSize;
                        if (prototype[idx] > 0.5f) {
                            instance[idx] = prototype[idx] * 0.7f + instance[idx] * 0.3f;
                        }
                    }
                }
                
                instances.push_back(instance);
            }
        }
        
        return instances;
    }
};

/**
 * Standardized test workload for Attention System
 */
class AttentionWorkload {
public:
    enum class Difficulty {
        Simple,      // 3 competitors, clear winner
        Moderate,    // 5 competitors, moderate competition
        Complex,     // 10 competitors, high competition
        Challenging   // 20 competitors, very high competition
    };
    
    struct WorkloadParams {
        size_t numCompetitors;    // Number of competing items
        size_t competitionRounds; // Number of competition rounds
        float salienceVaried;     // Whether saliences vary across rounds
        float inhibitionStrength; // Strength of mutual inhibition
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numCompetitors = 3;
                params.competitionRounds = 20;
                params.salienceVaried = false;
                params.inhibitionStrength = 0.5f;
                break;
                
            case Difficulty::Moderate:
                params.numCompetitors = 5;
                params.competitionRounds = 50;
                params.salienceVaried = true;
                params.inhibitionStrength = 0.6f;
                break;
                
            case Difficulty::Complex:
                params.numCompetitors = 10;
                params.competitionRounds = 100;
                params.salienceVaried = true;
                params.inhibitionStrength = 0.7f;
                break;
                
            case Difficulty::Challenging:
                params.numCompetitors = 20;
                params.competitionRounds = 200;
                params.salienceVaried = true;
                params.inhibitionStrength = 0.8f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 1000;
                chars.estimatedMemoryMB = 50;
                chars.estimatedTimePerTrial = 0.1;
                chars.energyPerTrial = 10.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 5000;
                chars.estimatedMemoryMB = 250;
                chars.estimatedTimePerTrial = 0.5;
                chars.energyPerTrial = 50.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 20000;
                chars.estimatedMemoryMB = 1000;
                chars.estimatedTimePerTrial = 2.0;
                chars.energyPerTrial = 200.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 100000;
                chars.estimatedMemoryMB = 5000;
                chars.estimatedTimePerTrial = 10.0;
                chars.energyPerTrial = 1000.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate competitor neurons for attention
     */
    static std::vector<NeuronId> generateCompetitors(
        size_t numCompetitors,
        uint64_t seed = 42) {
        
        std::vector<NeuronId> competitors;
        competitors.reserve(numCompetitors);
        
        for (size_t i = 0; i < numCompetitors; ++i) {
            // Create unique neuron IDs
            competitors.push_back(NeuronId(10000 + i));
        }
        
        return competitors;
    }
};

/**
 * Standardized test workload for Planning System
 */
class PlanningWorkload {
public:
    enum class Difficulty {
        Simple,      // 3 actions, 2 steps
        Moderate,    // 5 actions, 3 steps
        Complex,     // 10 actions, 5 steps
        Challenging   // 15 actions, 8 steps
    };
    
    struct WorkloadParams {
        size_t numActions;        // Number of possible actions
        size_t planningDepth;    // How many steps ahead to plan
        float actionQualitySpread; // Range of action qualities
        float rewardUncertainty;   // How uncertain rewards are
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numActions = 3;
                params.planningDepth = 2;
                params.actionQualitySpread = 0.5f;
                params.rewardUncertainty = 0.3f;
                break;
                
            case Difficulty::Moderate:
                params.numActions = 5;
                params.planningDepth = 3;
                params.actionQualitySpread = 0.6f;
                params.rewardUncertainty = 0.4f;
                break;
                
            case Difficulty::Complex:
                params.numActions = 10;
                params.planningDepth = 5;
                params.actionQualitySpread = 0.7f;
                params.rewardUncertainty = 0.5f;
                break;
                
            case Difficulty::Challenging:
                params.numActions = 15;
                params.planningDepth = 8;
                params.actionQualitySpread = 0.8f;
                params.rewardUncertainty = 0.6f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 2000;
                chars.estimatedMemoryMB = 100;
                chars.estimatedTimePerTrial = 0.2;
                chars.energyPerTrial = 20.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 8000;
                chars.estimatedMemoryMB = 400;
                chars.estimatedTimePerTrial = 0.8;
                chars.energyPerTrial = 80.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 30000;
                chars.estimatedMemoryMB = 1500;
                chars.estimatedTimePerTrial = 3.0;
                chars.energyPerTrial = 300.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 120000;
                chars.estimatedMemoryMB = 6000;
                chars.estimatedTimePerTrial = 12.0;
                chars.energyPerTrial = 1200.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate action sequences for planning
     */
    static std::vector<std::vector<ActionType>> generateActionSequences(
        size_t numActions,
        size_t sequenceLength,
        uint64_t seed = 42) {
        
        std::vector<std::vector<ActionType>> sequences;
        sequences.reserve(10); // Generate a subset of possible sequences
        
        for (size_t i = 0; i < 10; ++i) {
            std::vector<ActionType> sequence;
            sequence.reserve(sequenceLength);
            
            for (size_t j = 0; j < sequenceLength; ++j) {
                ActionType action = static_cast<ActionType>(i * 7 + j * 11) % numActions;
                sequence.push_back(action);
            }
            
            sequences.push_back(sequence);
        }
        
        return sequences;
    }
};

/**
 * Standardized test workload for Social Learning
 */
class SocialLearningWorkload {
public:
    enum class Difficulty {
        Simple,      // 2 actions, simple imitation
        Moderate,    // 5 actions, moderate complexity
        Complex,     // 10 actions, high complexity
        Challenging   // 15 actions, very complex
    };
    
    struct WorkloadParams {
        size_t numActions;        // Number of actions to learn
        size_t observationRounds; // Number of observation rounds
        float imitationConfidence; // Confidence threshold for imitation
        float socialSignalStrength; // Strength of social signals
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numActions = 2;
                params.observationRounds = 20;
                params.imitationConfidence = 0.8f;
                params.socialSignalStrength = 0.9f;
                break;
                
            case Difficulty::Moderate:
                params.numActions = 5;
                params.observationRounds = 50;
                params.imitationConfidence = 0.7f;
                params.socialSignalStrength = 0.8f;
                break;
                
            case Difficulty::Complex:
                params.numActions = 10;
                params.observationRounds = 100;
                params.imitationConfidence = 0.6f;
                params.socialSignalStrength = 0.7f;
                break;
                
            case Difficulty::Challenging:
                params.numActions = 15;
                params.observationRounds = 200;
                params.imitationConfidence = 0.5f;
                params.socialSignalStrength = 0.6f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 2000;
                chars.estimatedMemoryMB = 100;
                chars.estimatedTimePerTrial = 0.3;
                chars.energyPerTrial = 30.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 8000;
                chars.estimatedMemoryMB = 400;
                chars.estimatedTimePerTrial = 1.2;
                chars.energyPerTrial = 120.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 30000;
                chars.estimatedMemoryMB = 1500;
                chars.estimatedTimePerTrial = 4.8;
                chars.energyPerTrial = 480.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 120000;
                chars.estimatedMemoryMB = 6000;
                chars.estimatedTimePerTrial = 19.2;
                chars.energyPerTrial = 1920.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate observed agent actions
     */
    static std::vector<std::pair<ActionType, std::vector<float>>> generateObservedActions(
        size_t numActions,
        size_t observationRounds,
        size_t stateSize,
        uint64_t seed = 42) {
        
        std::vector<std::pair<ActionType, std::vector<float>>> observations;
        observations.reserve(observationRounds);
        
        for (size_t i = 0; i < observationRounds; ++i) {
            ActionType action = static_cast<ActionType>(i % numActions);
            
            std::vector<float> state(stateSize, 0.0f);
            size_t activeNeurons = (i % 5) + 1;
            
            for (size_t j = 0; j < activeNeurons; ++j) {
                size_t idx = (i * 13 + j) % stateSize;
                state[idx] = (static_cast<float>(rand()) / RAND_MAX) * 0.8f + 0.1f;
            }
            
            observations.push_back(std::make_pair(action, state));
        }
        
        return observations;
    }
};

/**
 * Standardized test workload for Self-Model
 */
class SelfModelWorkload {
public:
    enum class Difficulty {
        Simple,      // 3 actions, simple consequences
        Moderate,    // 5 actions, moderate complexity
        Complex,     // 10 actions, high complexity
        Challenging   // 15 actions, very complex
    };
    
    struct WorkloadParams {
        size_t numActions;        // Number of actions to learn
        size_t consequenceRounds; // Number of consequence observations
        float predictionAccuracy; // Target prediction accuracy
        float confidenceThreshold; // Confidence threshold for learning
    };
    
    static WorkloadParams getParams(Difficulty difficulty) {
        WorkloadParams params;
        
        switch (difficulty) {
            case Difficulty::Simple:
                params.numActions = 3;
                params.consequenceRounds = 30;
                params.predictionAccuracy = 0.8f;
                params.confidenceThreshold = 0.7f;
                break;
                
            case Difficulty::Moderate:
                params.numActions = 5;
                params.consequenceRounds = 60;
                params.predictionAccuracy = 0.7f;
                params.confidenceThreshold = 0.6f;
                break;
                
            case Difficulty::Complex:
                params.numActions = 10;
                params.consequenceRounds = 100;
                params.predictionAccuracy = 0.6f;
                params.confidenceThreshold = 0.5f;
                break;
                
            case Difficulty::Challenging:
                params.numActions = 15;
                params.consequenceRounds = 150;
                params.predictionAccuracy = 0.5f;
                params.confidenceThreshold = 0.4f;
                break;
        }
        
        return params;
    }
    
    static WorkloadCharacteristics getCharacteristics(Difficulty difficulty) {
        WorkloadCharacteristics chars;
        
        switch (difficulty) {
            case Difficulty::Simple:
                chars.estimatedNeurons = 2000;
                chars.estimatedMemoryMB = 100;
                chars.estimatedTimePerTrial = 0.2;
                chars.energyPerTrial = 20.0;
                chars.complexity = "low";
                break;
                
            case Difficulty::Moderate:
                chars.estimatedNeurons = 8000;
                chars.estimatedMemoryMB = 400;
                chars.estimatedTimePerTrial = 0.8;
                chars.energyPerTrial = 80.0;
                chars.complexity = "medium";
                break;
                
            case Difficulty::Complex:
                chars.estimatedNeurons = 30000;
                chars.estimatedMemoryMB = 1500;
                chars.estimatedTimePerTrial = 3.0;
                chars.energyPerTrial = 300.0;
                chars.complexity = "high";
                break;
                
            case Difficulty::Challenging:
                chars.estimatedNeurons = 120000;
                chars.estimatedMemoryMB = 6000;
                chars.estimatedTimePerTrial = 12.0;
                chars.energyPerTrial = 1200.0;
                chars.complexity = "very_high";
                break;
        }
        
        return chars;
    }
    
    /**
     * Generate action-consequence pairs
     */
    static std::vector<std::tuple<ActionType, std::vector<float>, std::vector<float>>> generateActionConsequences(
        size_t numActions,
        size_t consequenceRounds,
        size_t stateSize,
        float accuracy = 0.7f,
        uint64_t seed = 42) {
        
        std::vector<std::tuple<ActionType, std::vector<float>, std::vector<float>>> consequences;
        consequences.reserve(consequenceRounds);
        
        for (size_t i = 0; i < consequenceRounds; ++i) {
            ActionType action = static_cast<ActionType>(i % numActions);
            
            std::vector<float> beforeState(stateSize, 0.5f);
            for (size_t j = 0; j < stateSize; ++j) {
                if ((static_cast<float>(rand()) / RAND_MAX) < accuracy) {
                    beforeState[j] = (static_cast<float>(rand()) / RAND_MAX) * 0.8f + 0.1f;
                }
            }
            
            std::vector<float> afterState = beforeState;
            // Modify state based on action
            if (action == ActionType::MoveForward) {
                for (size_t j = 0; j < stateSize / 2; ++j) {
                    afterState[j] = afterState[j] * 0.9f;
                }
            } else if (action == ActionType::Interact) {
                for (size_t j = stateSize / 2; j < stateSize; ++j) {
                    afterState[j] = afterState[j] * 0.8f;
                }
            }
            
            consequences.push_back(std::make_tuple(action, beforeState, afterState));
        }
        
        return consequences;
    }
};

/**
 * Get standardized workload for a specific system
 */
template<typename WorkloadClass>
static WorkloadClass getWorkloadForDifficulty(typename WorkloadClass::Difficulty difficulty) {
    return WorkloadClass();
}

/**
 * Get system name for workload type
 */
static std::string getSystemName(const std::string& workloadType) {
    if (workloadType == "WorkingMemory") return "Working Memory";
    if (workloadType == "EpisodicMemory") return "Episodic Memory";
    if (workloadType == "Prediction") return "Prediction System";
    if (workloadType == "ConceptFormation") return "Concept Formation";
    if (workloadType == "Attention") return "Attention System";
    if (workloadType == "Planning") return "Planning System";
    if (workloadType == "SelfModel") return "Self-Model";
    if (workloadType == "SocialLearning") return "Social Learning";
    return "Unknown";
}

/**
 * Get default configuration for a cognitive system
 */
static std::unordered_map<std::string, std::string> getDefaultConfig(const std::string& systemName) {
    std::unordered_map<std::string, std::string> config;
    
    if (systemName == "Working Memory") {
        config["decay_rate"] = "0.01";
        config["capacity"] = "100";
        config["rehearsal_strength"] = "0.5";
    } else if (systemName == "Episodic Memory") {
        config["max_episodes"] = "1000";
        config["replay_enabled"] = "true";
        config["relevance_decay"] = "0.05";
    } else if (systemName == "Prediction System") {
        config["prediction_horizon"] = "10";
        config["memory_size"] = "1000";
        config["confidence_threshold"] = "0.7";
    } else if (systemName == "Concept Formation") {
        config["formation_threshold"] = "0.7";
        config["stability_window"] = "50";
        config["generalization_range"] = "0.3";
    } else if (systemName == "Attention System") {
        config["inhibition_strength"] = "0.5";
        config["competition_threshold"] = "0.3";
        config["salience_scale"] = "1.0";
    } else if (systemName == "Planning System") {
        config["planning_depth"] = "5";
        config["confidence_threshold"] = "0.6";
        config["action_evaluation_method"] = "predictive";
    } else if (systemName == "Self-Model") {
        config["body_awareness_rate"] = "0.01";
        config["self_other_distinction"] = "0.8";
        config["capability_learning_rate"] = "0.02";
    } else if (systemName == "Social Learning") {
        config["imitation_threshold"] = "0.7";
        config["observation_memory"] = "100";
        config["social_reward_weight"] = "0.5";
    }
    
    return config;
}

} // namespace nlm
