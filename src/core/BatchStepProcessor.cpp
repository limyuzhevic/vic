#include "BatchStepProcessor.hpp"
#include "../brain/Brain.hpp"
#include <thread>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

BatchStepProcessor::BatchStepProcessor(const Config& config)
    : config_(config)
    , processing_(false)
    , progress_(0.0f)
    , simdEnabled_(config.enableSIMD)
    , multithreadingEnabled_(config.enableMultithreading)
    , memoryPoolEnabled_(config.enableMemoryPool)
{
    stats_.startTime = std::chrono::high_resolution_clock::now();
    
    if (memoryPoolEnabled_) {
        // Initialize memory pools for batch operations
        neuronStatePool_ = std::make_unique<MemoryPool<float>>(1024, 4);
        synapseStatePool_ = std::make_unique<MemoryPool<uint8_t>>(1024, 4);
    }
    
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(config_.batchSize / 256);
    }
    
    NLM_LOG_INFO("BatchStepProcessor initialized: batchSize=" + std::to_string(config_.batchSize) +
                 ", SIMD=" + std::to_string(simdEnabled_) +
                 ", MultiThreading=" + std::to_string(multithreadingEnabled_));
}

BatchStepProcessor::~BatchStepProcessor() {
    resetProcessingState();
}

BatchStepProcessor::StepResult BatchStepProcessor::processBatch(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    double currentTime
) {
    return processBatchInternal(brain, startStep, endStep, currentTime, false);
}

BatchStepProcessor::StepResult BatchStepProcessor::processBatchWithTime(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    Timestamp startTime,
    Timestamp endTime
) {
    // Calculate batch size based on time span
    SimulationStep targetEndStep = endStep;
    if (endTime > 0.0) {
        double timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        targetEndStep = static_cast<SimulationStep>(endTime / timestep);
    }
    
    return processBatchInternal(brain, startStep, targetEndStep, startTime, false);
}

BatchStepProcessor::StepResult BatchStepProcessor::processBatchWithEarlyTermination(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    double currentTime
) {
    return processBatchInternal(brain, startStep, endStep, currentTime, true);
}

BatchStepProcessor::StepResult BatchStepProcessor::processBatchWithCheckpointing(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    double currentTime,
    const std::string& checkpointDir,
    uint64_t checkpointInterval
) {
    StepResult result = processBatchInternal(brain, startStep, endStep, currentTime, true);
    
    // Save checkpoints during batch processing
    if (result.stepsProcessed > 0) {
        for (SimulationStep step = startStep; step <= result.endStep; step += checkpointInterval) {
            if (step >= startStep && step <= result.endStep) {
                saveBatchCheckpoint(brain, step, checkpointDir);
                result.checkpointCount++;
            }
        }
    }
    
    return result;
}

std::future<BatchStepProcessor::StepResult> BatchStepProcessor::processBatchAsync(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    double currentTime
) {
    auto task = [this, brain, startStep, endStep, currentTime]() -> StepResult {
        return processBatchInternal(brain, startStep, endStep, currentTime, true);
    };
    
    if (multithreadingEnabled_) {
        return parallelProcessor_->getThreadPool().submit(task);
    } else {
        // Fallback to std::async
        return std::async(std::launch::async, task);
    }
}

float BatchStepProcessor::getProgress() const {
    return progress_.load(std::memory_order_relaxed);
}

bool BatchStepProcessor::isProcessing() const {
    return processing_.load(std::memory_order_relaxed);
}

BatchStepProcessor::ProcessingStats BatchStepProcessor::getStats() const {
    return stats_;
}

BatchStepProcessor::StepResult BatchStepProcessor::processBatchInternal(
    Brain* brain,
    SimulationStep startStep,
    SimulationStep endStep,
    double currentTime,
    bool enableEarlyTermination
) {
    std::lock_guard<std::mutex> lock(processingMutex_);
    
    if (processing_) {
        NLM_LOG_WARN("Batch processing already in progress");
        return StepResult{};
    }
    
    processing_ = true;
    resetProcessingState();
    
    // Calculate batch parameters
    size_t batchSize = std::min(static_cast<size_t>(endStep - startStep + 1), config_.batchSize);
    size_t maxSteps = config_.maxBatchSize;
    
    if (batchSize > maxSteps) {
        NLM_LOG_WARN("Requested batch size " + std::to_string(batchSize) + 
                    " exceeds maximum " + std::to_string(maxSteps) +
                    ", using maximum");
        batchSize = maxSteps;
    }
    
    // Initialize processing state
    auto& brainImpl = *brain->pImpl;
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    
    initializeNeuronState(neuronCount);
    initializeSynapseState(synapseCount);
    
    // Update statistics
    stats_.startTime = std::chrono::high_resolution_clock::now();
    stats_.stepsCompleted.store(0, std::memory_order_relaxed);
    stats_.neuronsProcessed.store(0, std::memory_order_relaxed);
    stats_.spikesGenerated.store(0, std::memory_order_relaxed);
    stats_.synapticEvents.store(0, std::memory_order_relaxed);
    
    StepResult result;
    result.startStep = startStep;
    result.terminatedEarly = false;
    
    // Process each step in the batch
    Timestamp timestep = brainImpl.timestep;
    SimulationStep currentStep = startStep;
    
    for (size_t stepIdx = 0; stepIdx < batchSize && currentStep <= endStep; ++stepIdx) {
        // Check early termination
        if (enableEarlyTermination) {
            if (config_.shouldTerminate && config_.shouldTerminate()) {
                result.terminatedEarly = true;
                NLM_LOG_INFO("Batch processing terminated early by callback");
                break;
            }
            
            float progress = static_cast<float>(stepIdx) / static_cast<float>(batchSize);
            if (progress >= config_.earlyTerminationThreshold) {
                NLM_LOG_INFO("Batch processing threshold reached (" + 
                           std::to_string(progress * 100.0f) + "%)");
                result.terminatedEarly = true;
                break;
            }
        }
        
        // Update progress
        progress_ = static_cast<float>(stepIdx + 1) / static_cast<float>(batchSize);
        if (config_.progressCallback) {
            config_.progressCallback(progress_);
        }
        
        // Process single step
        processSingleStep(brain, currentStep, currentTime, neuronCount, synapseCount);
        
        // Update statistics
        stats_.stepsCompleted.fetch_add(1, std::memory_order_relaxed);
        stats_.neuronsProcessed.fetch_add(neuronCount, std::memory_order_relaxed);
        stats_.spikesGenerated.fetch_add(brainImpl.totalSpikesThisStep, std::memory_order_relaxed);
        stats_.synapticEvents.fetch_add(brainImpl.totalSpikesThisStep * 2, std::memory_order_relaxed); // Approximate
        
        // Update time
        currentStep++;
        currentTime += timestep;
        
        // Collect memory usage periodically
        if (stepIdx % 100 == 0) {
            stats_.memoryUsage.fetch_add(collectMemoryUsage(), std::memory_order_relaxed);
        }
    }
    
    // Record final statistics
    stats_.endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stats_.endTime - stats_.startTime);
    
    result.endStep = currentStep - 1;
    result.stepsProcessed = stats_.stepsCompleted.load(std::memory_order_relaxed);
    result.totalNeurons = neuronCount;
    result.totalSpikes = stats_.spikesGenerated.load(std::memory_order_relaxed);
    result.totalSynapticEvents = stats_.synapticEvents.load(std::memory_order_relaxed);
    result.startTime = std::chrono::duration<double>(stats_.startTime.time_since_epoch()).count();
    result.endTime = std::chrono::duration<double>(stats_.endTime.time_since_epoch()).count();
    result.cpuTime = stats_.cpuTime.load(std::memory_order_relaxed);
    result.wallTime = duration.count() / 1e6; // Convert to seconds
    
    processing_ = false;
    
    NLM_LOG_INFO("Batch processing complete: steps=" + std::to_string(result.stepsProcessed) +
                 ", spikes=" + std::to_string(result.totalSpikes) +
                 ", time=" + std::to_string(result.wallTime) + "s");
    
    return result;
}

void BatchStepProcessor::initializeNeuronState(size_t neuronCount) {
    neuronState_.initialize(neuronCount);
    
    // Copy neuron states from brain
    auto& brainImpl = *pImpl; // This would need Brain reference in actual implementation
    // For now, just initialize with default values
    for (size_t i = 0; i < neuronCount; ++i) {
        neuronState_.membranePotential[i] = -70.0f;
        neuronState_.restingPotential[i] = -70.0f;
        neuronState_.threshold[i] = -55.0f;
        neuronState_.resetPotential[i] = -70.0f;
        neuronState_.leakConductance[i] = 10.0f;
        neuronState_.firingState[i] = 0;
        neuronState_.refractoryRemaining[i] = 0;
        neuronState_.refractoryPeriod[i] = 5;
        neuronState_.lastSpikeTime[i] = -1.0f;
    }
}

void BatchStepProcessor::initializeSynapseState(size_t synapseCount) {
    synapseState_.initialize(synapseCount);
    
    // Copy synapse states from brain
    for (size_t i = 0; i < synapseCount; ++i) {
        synapseState_.weight[i] = 0.5f;
        synapseState_.delay[i] = 1;
        synapseState_.synapseType[i] = 0;
        synapseState_.plasticityFlags[i] = 0;
        synapseState_.eligibilityTrace[i] = 0.0f;
        synapseState_.efficacy[i] = 1.0f;
        synapseState_.shortTermDepression[i] = 1.0f;
        synapseState_.shortTermFacilitation[i] = 0.0f;
    }
}

void BatchStepProcessor::resetProcessingState() {
    neuronState_.clear();
    synapseState_.clear();
    progress_ = 0.0f;
    
    if (neuronStatePool_) {
        // Clear memory pools
    }
}

size_t BatchStepProcessor::updateNeuronsBatch(
    const NeuronProcessingState& neuronState,
    const std::vector<NeuronId>& activeNeurons,
    size_t begin,
    size_t end,
    double currentTime,
    TimestepDuration timestep
) {
    size_t numFired = 0;
    
    if (simdEnabled_ && end - begin >= lifVectorizer_.SIMD_WIDTH) {
        // Vectorized processing
        return lifVectorizer_.update(
            neuronState.membranePotential,
            neuronState.synapticInput,
            neuronState.refractoryRemaining,
            neuronState.firingState,
            end - begin
        );
    } else {
        // Scalar processing for small batches
        for (size_t i = begin; i < end; ++i) {
            if (neuronState.refractoryRemaining[i] > 0) {
                neuronState.refractoryRemaining[i]--;
                neuronState.membranePotential[i] = neuronState.restingPotential[i];
                neuronState.firingState[i] = 0;
            } else {
                float leak = (neuronState.restingPotential[i] - neuronState.membranePotential[i]) / lifVectorizer_.tau;
                float input = neuronState.synapticInput[i] / lifVectorizer_.C;
                neuronState.membranePotential[i] += timestep * 1000.0f * (leak + input);
                
                if (neuronState.membranePotential[i] >= neuronState.threshold[i]) {
                    neuronState.membranePotential[i] = neuronState.resetPotential[i];
                    neuronState.refractoryRemaining[i] = static_cast<uint32_t>(lifVectorizer_.refractoryDecay);
                    neuronState.firingState[i] = 1;
                    ++numFired;
                }
            }
        }
    }
    
    return numFired;
}

void BatchStepProcessor::processSynapticInputsBatch(
    const SynapseProcessingState& synapseState,
    NeuronProcessingState& neuronState,
    const std::vector<NeuronId>& activeNeurons,
    size_t begin,
    size_t end
) {
    // Clear synaptic inputs
    std::fill(neuronState.synapticInput.begin(), neuronState.synapticInput.end(), 0.0f);
    
    // Process synaptic inputs (placeholder implementation)
    for (size_t i = begin; i < end; ++i) {
        // This would integrate with brain's synaptic connectivity
        // For now, just placeholder
    }
}

void BatchStepProcessor::applyPlasticityBatch(
    const SynapseProcessingState& synapseState,
    size_t begin,
    size_t end,
    double currentTime
) {
    // Apply STDP and Hebbian plasticity
    // This would integrate with brain's plasticity systems
    for (size_t i = begin; i < end; ++i) {
        // Placeholder for plasticity updates
    }
}

bool BatchStepProcessor::saveBatchCheckpoint(
    const Brain* brain,
    SimulationStep step,
    const std::string& checkpointDir
) {
    std::string filepath = checkpointDir + "/batch_checkpoint_step_" + 
                           std::to_string(step) + ".nlm";
    return brain->save(filepath);
}

bool BatchStepProcessor::loadBatchCheckpoint(
    Brain* brain,
    const std::string& checkpointPath,
    SimulationStep& targetStep
) {
    // Extract step from filename
    std::string filename = std::filesystem::path(checkpointPath).filename().string();
    try {
        size_t pos = filename.find_last_of('_');
        if (pos != std::string::npos && pos + 1 < filename.size()) {
            std::string stepStr = filename.substr(pos + 1);
            size_t dotPos = stepStr.find_last_of('.');
            if (dotPos != std::string::npos) {
                stepStr = stepStr.substr(0, dotPos);
                targetStep = std::stoull(stepStr);
            }
        }
    } catch (...) {
        return false;
    }
    
    return brain->load(checkpointPath);
}

void BatchStepProcessor::processSingleStep(
    Brain* brain,
    SimulationStep currentStep,
    double currentTime,
    size_t neuronCount,
    size_t synapseCount
) {
    // Simplified single step processing for batch processing
    // This would integrate with the full Brain::step() logic
    auto& brainImpl = *brain->pImpl;
    
    // Update neurons (simplified)
    size_t activeCount = 0;
    size_t firingCount = 0;
    
    for (size_t i = 0; i < neuronCount; ++i) {
        // Check if neuron is active (placeholder)
        if (neuronState_.synapticInput[i] > 0.1f) {
            activeCount++;
            neuronState_.activeNeurons.push_back(i);
            
            // Simple LIF update
            if (neuronState_.refractoryRemaining[i] > 0) {
                neuronState_.refractoryRemaining[i]--;
                neuronState_.membranePotential[i] = neuronState_.restingPotential[i];
            } else {
                float leak = (neuronState_.restingPotential[i] - neuronState_.membranePotential[i]) / 20.0f;
                float input = neuronState_.synapticInput[i] / 1.0f;
                neuronState_.membranePotential[i] += static_cast<float>(brainImpl.timestep) * 1000.0f * (leak + input);
                
                if (neuronState_.membranePotential[i] >= neuronState_.threshold[i]) {
                    neuronState_.membranePotential[i] = neuronState_.resetPotential[i];
                    neuronState_.refractoryRemaining[i] = 5;
                    neuronState_.firingState[i] = 1;
                    firingCount++;
                    neuronState_.firingNeurons.push_back(i);
                }
            }
        }
    }
    
    // Update statistics
    brainImpl.totalSpikesThisStep = firingCount;
    brainImpl.totalSpikesTotal += firingCount;
    
    // Update working memory (placeholder)
    if (brainImpl.workingMemory) {
        for (uint32_t idx : neuronState_.firingNeurons) {
            brainImpl.workingMemory->storeToNeuron(
                static_cast<NeuronId>(idx),
                std::abs(neuronState_.membranePotential[idx] - neuronState_.restingPotential[idx]) / 10.0f
            );
        }
    }
}

double BatchStepProcessor::collectMemoryUsage() {
    double usage = 0.0;
    usage += neuronState_.memoryUsage();
    usage += synapseState_.memoryUsage();
    if (neuronStatePool_) {
        usage += neuronStatePool_->memoryUsage();
    }
    if (synapseStatePool_) {
        usage += synapseStatePool_->memoryUsage();
    }
    return usage;
}

} // namespace nlm
