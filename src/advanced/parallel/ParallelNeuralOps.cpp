#include "ParallelNeuralOps.hpp"
#include <random>
#include <algorithm>

namespace nlm {

// ThreadPool implementation
ThreadPool::ThreadPool(size_t numThreads)
    : stop(false), numThreads(numThreads) {
    threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]() { 
                        return stop || !tasks.empty(); 
                    });
                    if (stop && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

// ParallelNeuralOps implementation
void ParallelNeuralOps::detectSpikesParallel(const float* potentials, bool* spikeFlags, 
                                           float threshold, size_t count) {
    #pragma omp parallel for
    for (size_t i = 0; i < count; ++i) {
        spikeFlags[i] = (potentials[i] > threshold);
    }
}

void ParallelNeuralOps::updatePotentialsParallel(float* potentials, const float* inputs,
                                                 float leakConductance, float timeConstant,
                                                 size_t count) {
    const float dt = 0.001f;
    const float inv_tau = 1.0f / timeConstant;
    
    #pragma omp parallel for
    for (size_t i = 0; i < count; ++i) {
        float leak_term = (0.0f - potentials[i]) * inv_tau;
        float syn_term = inputs[i];
        potentials[i] += dt * (leak_term + syn_term);
        potentials[i] = std::clamp(potentials[i], -100.0f, 50.0f);
    }
}

void ParallelNeuralOps::updateWeightsParallel(std::vector<float>& weights,
                                              const std::vector<float>& preSpikes,
                                              const std::vector<float>& postSpikes,
                                              const std::vector<float>& eligibility,
                                              float learningRate,
                                              float& processingTime) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t count = weights.size();
    
    #pragma omp parallel for
    for (size_t i = 0; i < count; ++i) {
        float stup = 0.01f * postSpikes[i] * eligibility[i];
        float stdp = -0.012f * preSpikes[i] * eligibility[i];
        weights[i] += learningRate * (stup + stdp);
        weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    processingTime = std::chrono::duration<float, std::milli>(end - start).count();
}

void ParallelNeuralOps::neuralComputationWorkStealing(const std::vector<float>& input,
                                                    std::vector<float>& output,
                                                    size_t blockSize) {
    size_t n = input.size();
    
    #pragma omp parallel for
    for (size_t block = 0; block < (n + blockSize - 1) / blockSize; ++block) {
        size_t startIdx = block * blockSize;
        size_t endIdx = std::min(startIdx + blockSize, n);
        
        for (size_t i = startIdx; i < endIdx; ++i) {
            float val = input[i];
            output[i] = val * (1.0f / (1.0f + std::exp(-val)));
            
            std::mt19937 rng(static_cast<unsigned int>(i));
            std::uniform_real_distribution<float> dist(-0.005f, 0.005f);
            output[i] += dist(rng);
        }
    }
}

void ParallelNeuralOps::parallelSpikePropagation(const std::vector<size_t>& preSpikeNeurons,
                                                 std::vector<std::vector<size_t>>& synapseLists,
                                                 std::vector<float>& weights,
                                                 float& processingTime) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t totalUpdates = 0;
    
    #pragma omp parallel for reduction(+:totalUpdates)
    for (size_t i = 0; i < preSpikeNeurons.size(); ++i) {
        size_t neuronId = preSpikeNeurons[i];
        if (synapseLists[neuronId].size() > 0) {
            for (size_t j = 0; j < synapseLists[neuronId].size(); ++j) {
                size_t synapseId = synapseLists[neuronId][j];
                weights[synapseId] = std::min(weights[synapseId] + 0.001f, 1.0f);
                totalUpdates++;
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    processingTime = std::chrono::duration<float, std::milli>(end - start).count();
}

std::vector<float> ParallelNeuralOps::parallelMemoryAllocation(size_t size, float value) {
    std::vector<float> result(size);
    
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        result[i] = value;
    }
    
    return result;
}

template<typename T>
T ParallelNeuralOps::parallelReduce(const std::vector<T>& data, T initial) {
    T result = initial;
    #pragma omp parallel for reduction(+:result)
    for (size_t i = 0; i < data.size(); ++i) {
        result += data[i];
    }
    return result;
}

void ParallelNeuralOps::parallelNeuralSimulation(const std::vector<float>& inputs,
                                                std::vector<float>& outputs,
                                                ThreadPool& threadPool,
                                                float& processingTime) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t chunkSize = std::max<size_t>(1, inputs.size() / threadPool.getThreadCount());
    std::vector<std::future<void>> futures;
    
    for (size_t i = 0; i < threadPool.getThreadCount(); ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = (i == threadPool.getThreadCount() - 1) ? 
                      inputs.size() : (i + 1) * chunkSize;
        
        if (startIdx >= inputs.size()) break;
        
        futures.push_back(threadPool.enqueue([&, startIdx, endIdx]() {
            for (size_t j = startIdx; j < endIdx; ++j) {
                float input = inputs[j];
                outputs[j] = input * (1.0f / (1.0f + std::exp(-input)));
            }
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    processingTime = std::chrono::duration<float, std::milli>(end - start).count();
}

// ParallelEventDriven implementation
void ParallelEventDriven::processEventsParallel() {
    std::vector<Event> localEvents;
    {
        std::unique_lock<std::mutex> lock(eventMutex);
        eventCondition.wait(lock, [this]() { return !events.empty() || stop; });
        
        if (stop && events.empty()) {
            return;
        }
        
        while (!events.empty()) {
            localEvents.push_back(events.top());
            events.pop();
        }
    }
    
    std::sort(localEvents.begin(), localEvents.end());
    
    #pragma omp parallel for
    for (size_t i = 0; i < localEvents.size(); ++i) {
        processSpikeEvent(localEvents[i].neuronId, localEvents[i].timestamp, localEvents[i].weight);
    }
}

void ParallelEventDriven::stopProcessing() {
    std::lock_guard<std::mutex> lock(eventMutex);
    stop = true;
    eventCondition.notify_all();
}

} // namespace nlm
