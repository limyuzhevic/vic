#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <functional>

namespace nlm {
namespace Parallel {

// Parallel configuration loading for multiple keys
template<typename F>
void forEachConfig(const std::vector<std::string>& keys, F func) {
    std::vector<std::thread> threads;
    std::mutex resultsMutex;
    std::vector<std::pair<std::string, ConfigResult>> results;
    
    auto processChunk = [&](size_t start, size_t end) {
        std::vector<std::pair<std::string, ConfigResult>> localResults;
        for (size_t i = start; i < end; ++i) {
            localResults.push_back({keys[i], func(keys[i])});
        }
        std::lock_guard<std::mutex> lock(resultsMutex);
        results.insert(results.end(), localResults.begin(), localResults.end());
    };
    
    // Determine number of threads to use
    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;
    
    size_t chunkSize = keys.size() / numThreads;
    
    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? keys.size() : start + chunkSize;
        threads.emplace_back(processChunk, start, end);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

// Parallel synaptic updates in a region
void updatePlasticityParallel(class NeuralRegion* region) {
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    std::vector<std::thread> threads;
    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;
    
    size_t chunkSize = synapses.size() / numThreads;
    
    auto processChunk = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            synapses[i]->step(0); // Simplified step call
        }
    };
    
    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? synapses.size() : start + chunkSize;
        threads.emplace_back(processChunk, start, end);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

// Parallel processing for vector operations
class ParallelProcessor {
public:
    // Parallel map operation
    template<typename InputIterator, typename OutputIterator, typename Function>
    static void parallelMap(InputIterator begin, InputIterator end,
                           OutputIterator out, Function func) {
        size_t numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 2;
        
        size_t total = std::distance(begin, end);
        size_t chunkSize = total / numThreads;
        std::vector<std::thread> threads;
        
        InputIterator current = begin;
        OutputIterator currentOut = out;
        
        for (size_t i = 0; i < numThreads; ++i) {
            size_t currentChunkSize = (i == numThreads - 1) ? 
                (total - i * chunkSize) : chunkSize;
            
            threads.emplace_back([&, currentChunkSize]() {
                InputIterator chunkBegin = current;
                OutputIterator chunkOut = currentOut;
                std::advance(current, currentChunkSize);
                std::advance(currentOut, currentChunkSize);
                
                for (size_t j = 0; j < currentChunkSize; ++j) {
                    *chunkOut++ = func(*chunkBegin++);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    // Parallel reduction operation
    template<typename InputIterator, typename T, typename BinaryOperation>
    static T parallelReduce(InputIterator begin, InputIterator end, T initial, 
                           BinaryOperation op) {
        size_t numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 2;
        
        size_t total = std::distance(begin, end);
        size_t chunkSize = total / numThreads;
        std::vector<std::thread> threads;
        std::vector<T> partialResults(numThreads, initial);
        
        InputIterator current = begin;
        
        for (size_t i = 0; i < numThreads; ++i) {
            size_t currentChunkSize = (i == numThreads - 1) ? 
                (total - i * chunkSize) : chunkSize;
            
            threads.emplace_back([&, currentChunkSize, i]() {
                T result = initial;
                InputIterator chunkBegin = current;
                for (size_t j = 0; j < currentChunkSize; ++j) {
                    result = op(result, *chunkBegin++);
                }
                partialResults[i] = result;
            });
            
            std::advance(current, currentChunkSize);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Combine partial results
        T finalResult = initial;
        for (const T& partial : partialResults) {
            finalResult = op(finalResult, partial);
        }
        
        return finalResult;
    }
};

} // namespace Parallel
} // namespace nlm
