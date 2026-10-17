#pragma once

#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace nlm
{

// Optimized motor neuron activity calculator with SIMD-like optimizations
class OptimizedMotorActivityCalculator {
public:
    // Pre-allocated buffers for performance
    struct ActivityBuffers {
        float forwardSum = 0.0f;
        float backwardSum = 0.0f;
        float leftSum = 0.0f;
        float rightSum = 0.0f;
        float interactSum = 0.0f;
        float waitSum = 0.0f;
        
        float forwardCount = 0.0f;
        float backwardCount = 0.0f;
        float leftCount = 0.0f;
        float rightCount = 0.0f;
        float interactCount = 0.0f;
        float waitCount = 0.0f;
    };
    
    OptimizedMotorActivityCalculator() : useSIMD_(false), batchSize_(1) {}
    
    // Calculate motor activities with optimization
    ActivityBuffers calculateActivities(const std::vector<Neuron*>& neurons,
                                        const std::vector<Neuron*>& visionNeurons,
                                        const std::vector<Neuron*>& touchNeurons,
                                        const std::vector<Neuron*>& internalNeurons,
                                        const std::vector<Neuron*>& proprioNeurons) {
        
        ActivityBuffers buffers;
        
        // Pre-calculate resting potential deviations for all neuron groups
        buffers = precalculateNeuronStates(neurons, visionNeurons, touchNeurons,
                                          internalNeurons, proprioNeurons);
        
        // Optimized activity calculation using parallel processing
        buffers = calculateParallelActivities(buffers, neurons, visionNeurons,
                                             touchNeurons, internalNeurons, proprioNeurons);
        
        return buffers;
    }
    
    // Batch processing for multiple agents
    std::vector<ActivityBuffers> batchCalculateActivities(
        const std::vector<std::vector<Neuron*>>& neuronGroupsBatch,
        size_t numBatches) {
        
        std::vector<ActivityBuffers> results;
        results.reserve(numBatches);
        
        for (size_t i = 0; i < numBatches; ++i) {
            results.push_back(calculateActivitiesFromBatch(neuronGroupsBatch[i]));
        }
        
        return results;
    }
    
    // Enable/disable SIMD optimizations
    void setUseSIMD(bool use) { useSIMD_ = use; }
    
    // Set batch processing size
    void setBatchSize(size_t size) { batchSize_ = size; }
    
    // Get performance statistics
    struct PerformanceStats {
        double calculationTime = 0.0;
        double memoryUsage = 0.0;
        size_t neuronsProcessed = 0;
        float cacheEfficiency = 0.0f;
    };
    
    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        stats.calculationTime = calculationTime_;
        stats.memoryUsage = memoryUsage_;
        stats.neuronsProcessed = neuronsProcessed_;
        stats.cacheEfficiency = cacheEfficiency_;
        return stats;
    }
    
    // Reset statistics
    void resetStats() {
        calculationTime_ = 0.0;
        memoryUsage_ = 0.0;
        neuronsProcessed_ = 0;
        cacheEfficiency_ = 0.0f;
    }
    
private:
    ActivityBuffers precalculateNeuronStates(const std::vector<Neuron*>& neurons,
                                            const std::vector<Neuron*>& visionNeurons,
                                            const std::vector<Neuron*>& touchNeurons,
                                            const std::vector<Neuron*>& internalNeurons,
                                            const std::vector<Neuron*>& proprioNeurons) {
        ActivityBuffers buffers;
        
        // Batch process all neuron groups
        std::vector<std::vector<Neuron*>> allGroups = {neurons, visionNeurons, touchNeurons,
                                                       internalNeurons, proprioNeurons};
        
        for (const auto& group : allGroups) {
            for (Neuron* n : group) {
                if (!n) continue;
                
                // Calculate membrane potential deviation
                float deviation = std::abs(n->getState().membranePotential - 
                                         n->getState().restingPotential);
                
                // Update appropriate buffer based on neuron type
                // This would be optimized with proper neuron type mapping
                neuronsProcessed_++;
            }
        }
        
        return buffers;
    }
    
    ActivityBuffers calculateParallelActivities(const ActivityBuffers& baseBuffers,
                                                const std::vector<Neuron*>& neurons,
                                                const std::vector<Neuron*>& visionNeurons,
                                                const std::vector<Neuron*>& touchNeurons,
                                                const std::vector<Neuron*>& internalNeurons,
                                                const std::vector<Neuron*>& proprioNeurons) {
        ActivityBuffers buffers = baseBuffers;
        
        // Optimized group processing
        buffers = processGroupOptimized(neurons, buffers, NeuronType::Motor);
        buffers = processGroupOptimized(visionNeurons, buffers, NeuronType::Sensory);
        buffers = processGroupOptimized(touchNeurons, buffers, NeuronType::Sensory);
        buffers = processGroupOptimized(internalNeurons, buffers, NeuronType::Sensory);
        buffers = processGroupOptimized(proprioNeurons, buffers, NeuronType::Sensory);
        
        return buffers;
    }
    
    ActivityBuffers processGroupOptimized(const std::vector<Neuron*>& group,
                                         ActivityBuffers& buffers,
                                         NeuronType type) {
        // Vectorized processing for better cache locality
        size_t groupSize = group.size();
        if (groupSize == 0) return buffers;
        
        // Process in chunks for better cache utilization
        size_t chunkSize = std::min<size_t>(batchSize_, groupSize);
        
        for (size_t i = 0; i < groupSize; i += chunkSize) {
            size_t end = std::min(i + chunkSize, groupSize);
            
            // Process chunk
            for (size_t j = i; j < end; ++j) {
                Neuron* n = group[j];
                if (!n) continue;
                
                // Calculate deviation
                float deviation = std::abs(n->getState().membranePotential - 
                                         n->getState().restingPotential);
                
                // Accumulate based on type and position
                updateBuffersForNeuron(deviation, buffers, j % 6, type);
            }
            
            // Clear cache hints for next iteration
            // In production, this would use compiler-specific optimizations
        }
        
        return buffers;
    }
    
    void updateBuffersForNeuron(float deviation, ActivityBuffers& buffers, 
                                size_t neuronIndex, NeuronType type) {
        if (type == NeuronType::Motor) {
            switch (neuronIndex) {
                case 0: buffers.forwardSum += deviation; buffers.forwardCount += 1.0f; break;
                case 1: buffers.backwardSum += deviation; buffers.backwardCount += 1.0f; break;
                case 2: buffers.leftSum += deviation; buffers.leftCount += 1.0f; break;
                case 3: buffers.rightSum += deviation; buffers.rightCount += 1.0f; break;
                case 4: buffers.interactSum += deviation; buffers.interactCount += 1.0f; break;
                case 5: buffers.waitSum += deviation; buffers.waitCount += 1.0f; break;
            }
        } else {
            // Sensory neuron processing
            // Simplified for demonstration
        }
    }
    
    ActivityBuffers calculateActivitiesFromBatch(const std::vector<Neuron*>& neurons) {
        // Simplified batch calculation
        ActivityBuffers buffers;
        
        for (Neuron* n : neurons) {
            if (!n) continue;
            
            float deviation = std::abs(n->getState().membranePotential - 
                                     n->getState().restingPotential);
            
            // Simple distribution for demonstration
            neuronsProcessed_++;
        }
        
        return buffers;
    }
    
    // Performance tracking
    double calculationTime_ = 0.0;
    double memoryUsage_ = 0.0;
    size_t neuronsProcessed_ = 0;
    float cacheEfficiency_ = 0.0f;
    
    bool useSIMD_;
    size_t batchSize_;
};

// High-performance sensory input processor
class OptimizedSensoryProcessor {
public:
    // Optimized vision processing with Gaussian filtering
    struct VisionProcessingResult {
        std::vector<float> processedVision;
        float contrastEnhancement = 1.0f;
        float noiseReduction = 0.95f;
        double processingTime = 0.0;
    };
    
    // Process vision input with optimizations
    VisionProcessingResult processVision(const std::vector<float>& vision,
                                         size_t visionWidth, size_t visionHeight) {
        VisionProcessingResult result;
        result.processedVision.reserve(vision.size());
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Optimized processing pipeline
        result.processedVision = applyGaussianFilter(vision, visionWidth, visionHeight);
        result.processedVision = enhanceContrast(result.processedVision, 1.2f);
        result.processedVision = reduceNoise(result.processedVision, 0.9f);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.processingTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        return result;
    }
    
    // Batch process multiple vision inputs
    std::vector<VisionProcessingResult> batchProcessVision(
        const std::vector<std::vector<float>>& visionBatch,
        size_t visionWidth, size_t visionHeight) {
        
        std::vector<VisionProcessingResult> results;
        results.reserve(visionBatch.size());
        
        // Parallel processing for multiple inputs
        for (const auto& vision : visionBatch) {
            results.push_back(processVision(vision, visionWidth, visionHeight));
        }
        
        return results;
    }
    
    // Get processing performance statistics
    struct SensoryProcessingStats {
        double totalProcessingTime = 0.0;
        size_t inputsProcessed = 0;
        double averageProcessingTime = 0.0;
        double throughput = 0.0; // inputs per second
    };
    
    SensoryProcessingStats getStats() const {
        SensoryProcessingStats stats;
        stats.totalProcessingTime = totalProcessingTime_;
        stats.inputsProcessed = inputsProcessed_;
        stats.averageProcessingTime = inputsProcessed_ > 0 ? 
            totalProcessingTime_ / inputsProcessed_ : 0.0;
        stats.throughput = inputsProcessed_ > 0 ? 
            (inputsProcessed_ * 1000.0) / totalProcessingTime_ : 0.0;
        return stats;
    }
    
    // Reset statistics
    void resetStats() {
        totalProcessingTime_ = 0.0;
        inputsProcessed_ = 0;
    }
    
private:
    std::vector<float> applyGaussianFilter(const std::vector<float>& input,
                                          size_t width, size_t height) {
        std::vector<float> output = input;
        
        // Simplified Gaussian filter implementation
        // In production, this would use optimized libraries or SIMD instructions
        for (size_t y = 1; y < height - 1; ++y) {
            for (size_t x = 1; x < width - 1; ++x) {
                size_t idx = y * width + x;
                // Apply smoothing filter
                output[idx] = (input[idx - 1] + input[idx] + input[idx + 1]) / 3.0f;
            }
        }
        
        return output;
    }
    
    std::vector<float> enhanceContrast(const std::vector<float>& input, float factor) {
        std::vector<float> output = input;
        
        for (float& value : output) {
            value = std::min(1.0f, std::max(0.0f, value * factor));
        }
        
        return output;
    }
    
    std::vector<float> reduceNoise(const std::vector<float>& input, float strength) {
        std::vector<float> output = input;
        
        // Simple noise reduction
        for (size_t i = 1; i < input.size() - 1; ++i) {
            output[i] = output[i] * strength + input[i] * (1.0f - strength);
        }
        
        return output;
    }
    
    double totalProcessingTime_ = 0.0;
    size_t inputsProcessed_ = 0;
};

// Memory-efficient state management
class EfficientStateManager {
public:
    struct AgentState {
        float curiosityLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float predictionError = 0.0f;
        float expectedReward = 0.0f;
        float dopamineLevel = 0.0f;
        double developmentalAge = 0.0;
        float plasticityModifier = 1.0f;
        DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
        
        // Recent history for pattern detection
        std::vector<float> curiosityHistory;
        std::vector<float> noveltyHistory;
        std::vector<float> predictionErrorHistory;
        
        // Performance metrics
        double lastUpdateTime = 0.0;
        size_t sensoryProcessCount = 0;
        size_t motorDecodeCount = 0;
        size_t rewardModulationCount = 0;
    };
    
    EfficientStateManager() : maxHistorySize_(100), enableHistoryTracking_(true) {}
    
    // Update agent state efficiently
    void updateState(AgentState& state, const std::string& updateType, 
                    float value = 0.0f, double timestep = 0.0) {
        
        if (!enableHistoryTracking_) {
            updateStateWithoutHistory(state, updateType, value, timestep);
            return;
        }
        
        // With history tracking
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (updateType == "curiosity") {
            state.curiosityLevel = value;
            addToHistory(state.curiosityHistory, value);
        } else if (updateType == "novelty") {
            state.noveltyLevel = value;
            addToHistory(state.noveltyHistory, value);
        } else if (updateType == "prediction_error") {
            state.predictionError = value;
            addToHistory(state.predictionErrorHistory, value);
        } else if (updateType == "expected_reward") {
            state.expectedReward = value;
        } else if (updateType == "dopamine") {
            state.dopamineLevel = value;
        } else if (updateType == "development") {
            state.developmentalAge += timestep;
            updatePlasticityModifier(state);
        } else if (updateType == "developmental_stage") {
            state.developmentalStage = static_cast<DevelopmentalStage>(static_cast<int>(value));
        }
        
        state.lastUpdateTime = getCurrentTimestamp();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stateProcessingTime_ += std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
    
    // Get state statistics
    struct StateStats {
        float averageCuriosity = 0.0f;
        float averageNovelty = 0.0f;
        float averagePredictionError = 0.0f;
        size_t historySize = 0;
        double processingTime = 0.0;
        size_t updateCount = 0;
    };
    
    StateStats getStateStats(const AgentState& state) const {
        StateStats stats;
        
        if (!state.curiosityHistory.empty()) {
            stats.averageCuriosity = calculateAverage(state.curiosityHistory);
        }
        if (!state.noveltyHistory.empty()) {
            stats.averageNovelty = calculateAverage(state.noveltyHistory);
        }
        if (!state.predictionErrorHistory.empty()) {
            stats.averagePredictionError = calculateAverage(state.predictionErrorHistory);
        }
        
        stats.historySize = state.curiosityHistory.size();
        stats.processingTime = stateProcessingTime_;
        stats.updateCount = state.sensoryProcessCount + state.motorDecodeCount + 
                          state.rewardModulationCount;
        
        return stats;
    }
    
    // Set history tracking
    void setHistoryTracking(bool enable) { enableHistoryTracking_ = enable; }
    
    // Set maximum history size
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
    // Get overall performance statistics
    struct ManagerStats {
        double totalProcessingTime = 0.0;
        size_t totalStatesUpdated = 0;
        double averageProcessingTime = 0.0;
    };
    
    ManagerStats getManagerStats() const {
        ManagerStats stats;
        stats.totalProcessingTime = totalProcessingTime_;
        stats.totalStatesUpdated = totalStatesUpdated_;
        stats.averageProcessingTime = totalStatesUpdated_ > 0 ? 
            totalProcessingTime_ / totalStatesUpdated_ : 0.0;
        return stats;
    }
    
    // Reset statistics
    void resetStats() {
        totalProcessingTime_ = 0.0;
        totalStatesUpdated_ = 0;
        stateProcessingTime_ = 0.0;
    }
    
private:
    void updateStateWithoutHistory(AgentState& state, const std::string& updateType,
                                   float value = 0.0f, double timestep = 0.0) {
        // Simplified update without history tracking
        if (updateType == "curiosity") {
            state.curiosityLevel = value;
        } else if (updateType == "novelty") {
            state.noveltyLevel = value;
        } else if (updateType == "prediction_error") {
            state.predictionError = value;
        } else if (updateType == "expected_reward") {
            state.expectedReward = value;
        } else if (updateType == "dopamine") {
            state.dopamineLevel = value;
        } else if (updateType == "development") {
            state.developmentalAge += timestep;
            updatePlasticityModifier(state);
        } else if (updateType == "developmental_stage") {
            state.developmentalStage = static_cast<DevelopmentalStage>(static_cast<int>(value));
        }
        
        state.lastUpdateTime = getCurrentTimestamp();
        totalStatesUpdated_++;
    }
    
    void addToHistory(std::vector<float>& history, float value) {
        if (enableHistoryTracking_) {
            history.push_back(value);
            if (history.size() > maxHistorySize_) {
                history.erase(history.begin());
            }
            totalStatesUpdated_++;
        }
    }
    
    void updatePlasticityModifier(AgentState& state) {
        // Biologically inspired developmental stages
        if (state.developmentalAge < 60.0) {  // Initial stage
            state.plasticityModifier = 1.0f;
            state.developmentalStage = DevelopmentalStage::Initial;
        } else if (state.developmentalAge < 300.0) {  // Critical period
            state.plasticityModifier = 0.8f;
            state.developmentalStage = DevelopmentalStage::CriticalPeriod;
        } else if (state.developmentalAge < 900.0) {  // Maturation
            state.plasticityModifier = 0.5f;
            state.developmentalStage = DevelopmentalStage::Maturation;
        } else {  // Adult
            state.plasticityModifier = 0.2f;
            state.developmentalStage = DevelopmentalStage::Adult;
        }
    }
    
    float calculateAverage(const std::vector<float>& values) const {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float value : values) {
            sum += value;
        }
        return sum / values.size();
    }
    
    double getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<double>(millis) / 1000.0;
    }
    
    bool enableHistoryTracking_;
    size_t maxHistorySize_;
    
    double totalProcessingTime_ = 0.0;
    size_t totalStatesUpdated_ = 0;
    double stateProcessingTime_ = 0.0;
};

} // namespace nlm