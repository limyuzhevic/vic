#include "WorkingMemoryIntegration.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

WorkingMemoryIntegration::WorkingMemoryIntegration()
    : enableEpisodicSync_(true)
    , enableAssociativeSync_(true)
    , enablePredictionIntegration_(true)
    , enableAttentionCoordination_(true)
    , enableNeuromodulationIntegration_(true)
    , enableDevelopmentCoordination_(true)
    , enableLearningDrivenIntegration_(true)
    , syncInterval_(100)
    , predictionErrorThreshold_(0.3f)
    , curiosityThreshold_(0.5f)
    , noveltyThreshold_(0.7f)
    , attentionGainThreshold_(0.5f)
    , neuromodulationThreshold_(0.5f)
{
}

WorkingMemoryIntegration::~WorkingMemoryIntegration() {
}

void WorkingMemoryIntegration::initialize(Brain* brain,
                                         NeuralEpisodicMemory* episodicMemory,
                                         NeuralAssociativeMemory* associativeMemory,
                                         PredictionSystem* predictionSystem,
                                         AttentionalSelection* attentionSystem,
                                         DevelopmentSystem* developmentSystem,
                                         Dopamine* dopamine,
                                         Curiosity* curiosity,
                                         Novelty* novelty,
                                         PredictionError* predictionError) {
    brain_ = brain;
    episodicMemory_ = episodicMemory;
    associativeMemory_ = associativeMemory;
    predictionSystem_ = predictionSystem;
    attentionSystem_ = attentionSystem;
    developmentSystem_ = developmentSystem;
    dopamine_ = dopamine;
    curiosity_ = curiosity;
    novelty_ = novelty;
    predictionError_ = predictionError;
}

void WorkingMemoryIntegration::synchronizeWithEpisodicMemory(EnhancedWorkingMemory& workingMemory,
                                                             const std::vector<float>& currentPattern) {
    if (!isInitialized() || !enableEpisodicSync_ || !episodicMemory_) return;
    
    // Convert current pattern to episodic memory item
    // In a full implementation, this would create a proper EpisodicMemoryItem
    // For now, this is a simplified interface
    
    // Profile the operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    stats_.episodicMemorySyncs++;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    stats_.totalSyncTime += duration;
}

void WorkingMemoryIntegration::synchronizeWithAssociativeMemory(EnhancedWorkingMemory& workingMemory,
                                                                const std::vector<float>& currentPattern) {
    if (!isInitialized() || !enableAssociativeSync_ || !associativeMemory_) return;
    
    stats_.associativeMemorySyncs++;
}

void WorkingMemoryIntegration::integrateWithPredictionSystem(EnhancedWorkingMemory& workingMemory,
                                                             const std::vector<float>& predictedPattern,
                                                             float predictionError) {
    if (!isInitialized() || !enablePredictionIntegration_ || !predictionSystem_) return;
    
    if (std::abs(predictionError) > predictionErrorThreshold_) {
        // Strong prediction error - update working memory
        workingMemory.store(predictedPattern, 1.0f, false);  // Not transient
        stats_.predictionIntegrations++;
    }
}

void WorkingMemoryIntegration::coordinateWithAttention(EnhancedWorkingMemory& workingMemory,
                                                       const std::vector<NeuronId>& attendedNeurons) {
    if (!isInitialized() || !enableAttentionCoordination_ || !attentionSystem_) return;
    
    float attentionGain = static_cast<float>(attendedNeurons.size()) / 100.0f;  // Normalized
    
    if (attentionGain > attentionGainThreshold_) {
        // Apply attention to winning neurons
        for (const NeuronId& neuron : attendedNeurons) {
            if (workingMemory.isWinning(neuron)) {
                // Apply attention boost
                workingMemory.storeToNeuron(neuron, workingMemory.getNeuronActivation(neuron) * 1.5f);
            }
        }
    }
    
    stats_.attentionalCoordinations++;
}

void WorkingMemoryIntegration::integrateNeuromodulation(EnhancedWorkingMemory& workingMemory,
                                                         const Neuromodulator& signal) {
    if (!isInitialized() || !enableNeuromodulationIntegration_) return;
    
    float modulationStrength = signal.getStrength();
    
    if (modulationStrength > neuromodulationThreshold_) {
        // Apply neuromodulation to working memory
        if (signal.getType() == Neuromodulator::Type::Dopamine) {
            // Dopamine - strengthen rewarded memories
            workingMemory.strengthenMemory(1.2f);
        } else if (signal.getType() == Neuromodulator::Type::Acetylcholine) {
            // Acetylcholine - enhance attention and encoding
            // In a full implementation, this would increase trace importance
        } else if (signal.getType() == Neuromodulator::Type::Norepinephrine) {
            // Norepinephrine - increase alertness and memory strength
            workingMemory.strengthenMemory(1.1f);
        }
    }
    
    stats_.neuromodulationIntegrations++;
}

void WorkingMemoryIntegration::coordinateWithDevelopment(EnhancedWorkingMemory& workingMemory,
                                                         DevelopmentalStage developmentalStage) {
    if (!isInitialized() || !enableDevelopmentCoordination_ || !developmentSystem_) return;
    
    // Adjust working memory parameters based on developmental stage
    if (developmentalStage == DevelopmentalStage::CriticalPeriod) {
        // Critical period - enhanced plasticity and learning
        workingMemory.setDecayRate(0.005f);
        workingMemory.setCapacity(workingMemory.getCapacity() * 2);
    } else if (developmentalStage == DevelopmentalStage::Maturation) {
        // Maturation - stabilize memory
        workingMemory.setDecayRate(0.02f);
    } else if (developmentalStage == DevelopmentalStage::Adult) {
        // Adult - normal operation
        workingMemory.setDecayRate(0.01f);
    }
    
    stats_.developmentCoordinations++;
}

void WorkingMemoryIntegration::processPredictionError(EnhancedWorkingMemory& workingMemory,
                                                      float errorSignal,
                                                      const std::vector<float>& expectedPattern,
                                                      const std::vector<float>& actualPattern) {
    if (!isInitialized()) return;
    
    if (std::abs(errorSignal) > predictionErrorThreshold_) {
        // High prediction error - update working memory
        if (!actualPattern.empty()) {
            workingMemory.store(actualPattern, 1.0f, false);  // Not transient
        }
        
        // Apply prediction error learning
        // In a full implementation, this would use specific learning rules
        
        stats_.predictionErrorProcessings++;
    }
}

void WorkingMemoryIntegration::processCuriosity(EnhancedWorkingMemory& workingMemory,
                                                float curiosityLevel,
                                                const std::vector<float>& novelPattern) {
    if (!isInitialized() || !enableLearningDrivenIntegration_) return;
    
    if (curiosityLevel > curiosityThreshold_) {
        // High curiosity - explore new patterns
        if (!novelPattern.empty()) {
            workingMemory.store(novelPattern, curiosityLevel, true);  // Transient
        }
        
        stats_.curiosityProcessings++;
    }
}

void WorkingMemoryIntegration::processNovelty(EnhancedWorkingMemory& workingMemory,
                                             float noveltySignal,
                                             const std::vector<float>& surprisingPattern) {
    if (!isInitialized() || !enableLearningDrivenIntegration_) return;
    
    if (noveltySignal > noveltyThreshold_) {
        // High novelty - encode and strengthen
        if (!surprisingPattern.empty()) {
            workingMemory.store(surprisingPattern, 1.0f, false);  // Not transient
        }
        
        stats_.noveltyProcessings++;
    }
}

std::vector<float> WorkingMemoryIntegration::normalizePattern(const std::vector<float>& pattern) const {
    std::vector<float> normalized = pattern;
    
    if (normalized.empty()) return normalized;
    
    // Calculate mean
    float mean = std::accumulate(normalized.begin(), normalized.end(), 0.0f) / normalized.size();
    
    // Normalize to zero mean and unit variance
    float variance = 0.0f;
    for (float value : normalized) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= normalized.size();
    
    float stdDev = std::sqrt(variance);
    
    if (stdDev > 0.0f) {
        for (float& value : normalized) {
            value = (value - mean) / stdDev;
        }
    }
    
    return normalized;
}

float WorkingMemoryIntegration::computePatternDistance(const std::vector<float>& patternA,
                                                       const std::vector<float>& patternB) const {
    if (patternA.size() != patternB.size() || patternA.empty()) {
        return std::numeric_limits<float>::max();
    }
    
    float distance = 0.0f;
    for (size_t i = 0; i < patternA.size(); ++i) {
        float diff = patternA[i] - patternB[i];
        distance += diff * diff;
    }
    
    return std::sqrt(distance);
}

float WorkingMemoryIntegration::applyAttentionFilter(float signal,
                                                     const std::vector<NeuronId>& attendedNeurons) const {
    // Simple attention filter - gain based on number of attended neurons
    float gain = static_cast<float>(attendedNeurons.size()) / 100.0f;
    return signal * gain;
}

float WorkingMemoryIntegration::applyNeuromodulation(float signal,
                                                      const Neuromodulator& modulator) const {
    // Apply neuromodulation based on modulator type and strength
    float strength = modulator.getStrength();
    
    switch (modulator.getType()) {
        case Neuromodulator::Type::Dopamine:
            // Dopamine typically enhances reward-related signals
            return signal * (1.0f + strength * 0.5f);
        case Neuromodulator::Type::Acetylcholine:
            // Acetylcholine enhances attention and encoding
            return signal * (1.0f + strength * 0.3f);
        case Neuromodulator::Type::Norepinephrine:
            // Norepinephrine enhances alertness and signal-to-noise ratio
            return signal * (1.0f + strength * 0.4f);
        case Neuromodulator::Type::Serotonin:
            // Serotonin modulates mood and learning
            return signal * (1.0f + strength * 0.2f);
        case Neuromodulator::Type::Glutamate:
            // Glutamate is excitatory
            return signal * (1.0f + strength * 0.8f);
        case Neuromodulator::Type::GABA:
            // GABA is inhibitory
            return signal * std::max(0.0f, 1.0f - strength * 0.5f);
        default:
            return signal;
    }
}

void WorkingMemoryIntegration::broadcastToEpisodicMemory(const std::vector<float>& pattern) {
    // Broadcast to episodic memory system
    // In a full implementation, this would call episodic memory directly
}

void WorkingMemoryIntegration::broadcastToAssociativeMemory(const std::vector<float>& pattern) {
    // Broadcast to associative memory system
    // In a full implementation, this would call associative memory directly
}

void WorkingMemoryIntegration::broadcastToPredictionSystem(const std::vector<float>& pattern) {
    // Broadcast to prediction system
    // In a full implementation, this would call prediction system directly
}

void WorkingMemoryIntegration::broadcastToAttentionSystem(const std::vector<NeuronId>& neurons) {
    // Broadcast to attention system
    // In a full implementation, this would call attention system directly
}

void WorkingMemoryIntegration::broadcastToDevelopmentSystem(DevelopmentalStage stage) {
    // Broadcast to development system
    // In a full implementation, this would call development system directly
}

void WorkingMemoryIntegration::processLearningSignal(float signal,
                                                    EnhancedWorkingMemory& workingMemory) {
    // Process learning signals for working memory optimization
    // This is a simplified implementation
    
    if (signal > 0.0f) {
        // Positive learning signal - strengthen memory
        workingMemory.strengthenMemory(1.1f);
    } else {
        // Negative learning signal - decay memory
        workingMemory.setDecayRate(workingMemory.getDecayRate() * 1.1f);
    }
}

void WorkingMemoryIntegration::processErrorSignal(float error,
                                                 EnhancedWorkingMemory& workingMemory) {
    // Process error signals for working memory adjustment
    // This is a simplified implementation
    
    if (std::abs(error) > predictionErrorThreshold_) {
        // High error - reset and re-learn
        workingMemory.clear();
    }
}

} // namespace nlm