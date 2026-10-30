#include "Performance.hpp"

namespace nlm {

// SIMDVectorOps implementation

// SIMD optimized matrix operations for neural network computations
void SIMDVectorOps::matrixMultiplySIMD(const std::vector<std::vector<float>>& A,
                                       const std::vector<std::vector<float>>& B,
                                       std::vector<std::vector<float>>& C) {
    size_t rowsA = A.size();
    if (rowsA == 0) return;
    size_t colsA = A[0].size();
    size_t rowsB = B.size();
    size_t colsB = B[0].size();

    C.resize(rowsA, std::vector<float>(colsB, 0.0f));

    // Use naive matrix multiplication with potential for SIMD optimization
    // For now, implement standard matrix multiplication
    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < colsA; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// ParallelProcessor implementation

// Process neurons in parallel using threads
void ParallelProcessor::processNeuronsParallel(std::vector<Neuron*>& neurons,
                                               std::function<void(Neuron*)> processor,
                                               size_t maxThreads) {
    size_t neuronCount = neurons.size();
    if (neuronCount == 0) return;

    size_t threadsToUse = std::min(maxThreads, neuronCount);
    if (threadsToUse == 0) return;

    size_t neuronsPerThread = neuronCount / threadsToUse;

    std::vector<std::thread> threads;
    threads.reserve(threadsToUse);

    for (size_t t = 0; t < threadsToUse; ++t) {
        size_t startIdx = t * neuronsPerThread;
        size_t endIdx = (t == threadsToUse - 1) ? neuronCount : (t + 1) * neuronsPerThread;

        threads.emplace_back([&, startIdx, endIdx]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                if (neurons[i]) {
                    processor(neurons[i]);
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// Process synapses in parallel with SIMD optimization
void ParallelProcessor::processSynapsesParallel(std::vector<Synapse*>& synapses,
                                                std::function<void(Synapse*, size_t)> processor,
                                                size_t maxThreads) {
    size_t synapseCount = synapses.size();
    if (synapseCount == 0) return;

    size_t threadsToUse = std::min(maxThreads, synapseCount);
    if (threadsToUse == 0) return;

    size_t synapsesPerThread = synapseCount / threadsToUse;

    std::vector<std::thread> threads;
    threads.reserve(threadsToUse);

    for (size_t t = 0; t < threadsToUse; ++t) {
        size_t startIdx = t * synapsesPerThread;
        size_t endIdx = (t == threadsToUse - 1) ? synapseCount : (t + 1) * synapsesPerThread;

        threads.emplace_back([&, startIdx, endIdx]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                if (synapses[i]) {
                    processor(synapses[i], i);
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// Event-driven parallel processing
void ParallelProcessor::processEventsParallel(std::vector<std::function<void()>>& events,
                                              size_t maxThreads) {
    size_t eventCount = events.size();
    if (eventCount == 0) return;

    size_t threadsToUse = std::min(maxThreads, eventCount);
    if (threadsToUse == 0) return;

    size_t eventsPerThread = eventCount / threadsToUse;

    std::vector<std::thread> threads;
    threads.reserve(threadsToUse);

    for (size_t t = 0; t < threadsToUse; ++t) {
        size_t startIdx = t * eventsPerThread;
        size_t endIdx = (t == threadsToUse - 1) ? eventCount : (t + 1) * eventsPerThread;

        threads.emplace_back([&, startIdx, endIdx]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                try {
                    if (i < events.size() && events[i]) {
                        events[i]();
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error processing event " << i << ": " << e.what() << std::endl;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

} // namespace nlm
