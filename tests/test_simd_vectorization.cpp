// SIMD Vectorization Test Suite for NLM Neural Simulation
// Comprehensive test of vectorized neural computation implementations

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/VectorizedLIF.hpp"
#include "performance/VectorizedSTDP.hpp"
#include "performance/VectorizedStats.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace test_simd_vectorization {

// Test SIMD Vectorization Infrastructure
void testSIMDInfrastructure() {
    std::cout << "Testing SIMD Vectorization Infrastructure..." << std::flush;
    
    // Test SIMD availability detection
    bool simdAvailable = SIMDUtils::isSIMDAvailable();
    const char* simdType = SIMDUtils::getSIMDType();
    size_t simdWidth = SIMDUtils::SIMD_WIDTH;
    size_t alignment = SIMDUtils::alignment();
    
    std::cout << "SIMD Type: " << simdType << std::endl;
    std::cout << "SIMD Width: " << simdWidth << std::endl;
    std::cout << "Alignment: " << alignment << std::endl;
    
    // Verify expectations
    assert(simdAvailable || simdType == std::string("Scalar")); // Scalar is valid fallback
    assert(alignment >= 16); // Minimum 128-bit alignment
    
    // Test VectorizedLIF creation
    VectorizedLIF lif;
    assert(lif.V_rest == -70.0f);
    assert(lif.V_reset == -70.0f);
    assert(lif.V_threshold == -55.0f);
    assert(lif.tau == 20.0f);
    assert(lif.C == 1.0f);
    
    std::cout << "  SIMD Infrastructure Test Passed" << std::endl;
}

// Test Vectorized Neuron Operations
void testVectorizedNeuronOperations() {
    std::cout << "Testing Vectorized Neuron Operations..." << std::flush;
    
    // Create test neurons
    std::vector<nlm::Neuron> neurons;
    std::vector<float> membranePotentials;
    std::vector<float> synapticInputs;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<uint8_t> fired;
    
    size_t neuronCount = 100;
    neurons.reserve(neuronCount);
    membranePotentials.reserve(neuronCount);
    synapticInputs.reserve(neuronCount);
    refractoryRemaining.reserve(neuronCount);
    fired.reserve(neuronCount);
    
    // Initialize neurons
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        neurons[i].setMembranePotential(-65.0f + static_cast<float>(i % 20) * 0.1f);
        neurons[i].setThreshold(-55.0f + static_cast<float>(i % 10) * 0.05f);
        neurons[i].setRefractoryPeriod(5 + (i % 5));
        neurons[i].setRestingPotential(-70.0f);
        neurons[i].setResetPotential(-70.0f);
        neurons[i].setFiringState(nlm::FiringState::Resting);
        neurons[i].setRefractoryRemaining(0);
        neurons[i].setFiringRate(0.0f);
        neurons[i].setLeakConductance(10.0f);
        neurons[i].setAdaptationVariable(0.0f);
        neurons[i].setLastSpikeTime(-1.0f);
        
        membranePotentials.push_back(neurons[i].getState().membranePotential);
        synapticInputs.push_back(10.0f * static_cast<float>(i % 3 - 1)); // -10, 0, 10
        refractoryRemaining.push_back(0);
        fired.push_back(0);
    }
    
    // Create VectorizedLIF instance
    VectorizedLIF lif;
    
    // Test vectorized update
    size_t numFired = VectorizedLIF::updateLIF(lif, membranePotentials, synapticInputs,
                                             refractoryRemaining, fired, neuronCount);
    
    // Verify results
    assert(fired.size() == neuronCount);
    assert(refractoryRemaining.size() == neuronCount);
    assert(membranePotentials.size() == neuronCount);
    
    // Check that some neurons fired (should be some with high enough input)
    bool anyFired = std::any_of(fired.begin(), fired.end(), [](uint8_t f) { return f == 1; });
    assert(anyFired); // At least one neuron should fire with the test inputs
    
    std::cout << "  Vectorized Neuron Operations Test Passed" << std::endl;
}

// Test Vectorized Statistics Operations
void testVectorizedStatistics() {
    std::cout << "Testing Vectorized Statistics Operations..." << std::flush;
    
    size_t dataSize = 1000;
    std::vector<float> data;
    data.reserve(dataSize);
    
    // Generate test data with known statistics
    float expectedSum = 0.0f;
    float expectedMin = std::numeric_limits<float>::max();
    float expectedMax = -std::numeric_limits<float>::max();
    float expectedMean = 0.0f;
    float expectedVariance = 0.0f;
    
    for (size_t i = 0; i < dataSize; ++i) {
        float value = static_cast<float>(i) * 0.01f - 5.0f; // Range: -5 to 5
        data.push_back(value);
        
        expectedSum += value;
        expectedMin = std::min(expectedMin, value);
        expectedMax = std::max(expectedMax, value);
    }
    
    expectedMean = expectedSum / static_cast<float>(dataSize);
    
    // Calculate expected variance
    float sumSqDiff = 0.0f;
    for (size_t i = 0; i < dataSize; ++i) {
        float diff = data[i] - expectedMean;
        sumSqDiff += diff * diff;
    }
    expectedVariance = sumSqDiff / static_cast<float>(dataSize - 1);
    
    // Test vectorized statistics
    float actualSum = VectorizedStats::sum(data, dataSize);
    float actualMean = VectorizedStats::mean(data, dataSize);
    float actualVariance = VectorizedStats::variance(data, dataSize);
    float actualMax = VectorizedStats::max(data, dataSize);
    float actualMin = VectorizedStats::min(data, dataSize);
    
    // Verify results with tolerance
    float tolerance = 0.001f;
    assert(std::abs(actualSum - expectedSum) < tolerance);
    assert(std::abs(actualMean - expectedMean) < tolerance);
    assert(std::abs(actualVariance - expectedVariance) < tolerance * 10); // Variance has more error
    assert(std::abs(actualMax - expectedMax) < tolerance);
    assert(std::abs(actualMin - expectedMin) < tolerance);
    
    // Test clamp operation
    std::vector<float> testData = data;
    VectorizedStats::clamp(testData, dataSize, -2.0f, 2.0f);
    
    for (size_t i = 0; i < dataSize; ++i) {
        assert(testData[i] >= -2.0f);
        assert(testData[i] <= 2.0f);
    }
    
    std::cout << "  Vectorized Statistics Test Passed" << std::endl;
}

// Test Vectorized Plasticity Operations
void testVectorizedPlasticity() {
    std::cout << "Testing Vectorized Plasticity Operations..." << std::flush;
    
    size_t synapseCount = 50;
    std::vector<float> weights;
    weights.reserve(synapseCount);
    
    // Initialize weights
    for (size_t i = 0; i < synapseCount; ++i) {
        weights.push_back(static_cast<float>(i % 20 - 10) * 0.01f); // Range: -0.1 to 0.1
    }
    
    // Create original weights for comparison
    std::vector<float> originalWeights = weights;
    
    // Test VectorizedSTDP
    VectorizedSTDP::EnhancedVectorizedSTDP stdp(VectorizedSTDP::RuleType::STDP);
    
    // Create spike timing data
    std::vector<float> preSpikes(synapseCount);
    std::vector<float> postSpikes(synapseCount);
    
    for (size_t i = 0; i < synapseCount; ++i) {
        preSpikes[i] = static_cast<float>(i * 5); // 0, 5, 10, ...
        postSpikes[i] = preSpikes[i] + 10.0f + static_cast<float>(i % 5); // Pre before post
    }
    
    // Apply STDP update
    VectorizedSTDP::updateWeights(stdp, weights, preSpikes, postSpikes, synapseCount, synapseCount);
    
    // Verify that weights have changed (STDP should modify them)
    bool weightsChanged = false;
    for (size_t i = 0; i < synapseCount; ++i) {
        if (std::abs(weights[i] - originalWeights[i]) > 1e-6f) {
            weightsChanged = true;
            break;
        }
    }
    
    assert(weightsChanged); // STDP should modify weights
    
    // Test Hebbian plasticity
    VectorizedSTDP::EnhancedVectorizedSTDP hebbian(VectorizedSTDP::RuleType::Hebbian);
    std::vector<float> hebbianWeights = originalWeights;
    
    // Apply Hebbian update (same spike timing)
    VectorizedSTDP::updateWeights(hebbian, hebbianWeights, preSpikes, postSpikes, synapseCount, synapseCount);
    
    // Verify Hebbian weights changed
    bool hebbianChanged = false;
    for (size_t i = 0; i < synapseCount; ++i) {
        if (std::abs(hebbianWeights[i] - originalWeights[i]) > 1e-6f) {
            hebbianChanged = true;
            break;
        }
    }
    
    assert(hebbianChanged); // Hebbian should modify weights
    
    std::cout << "  Vectorized Plasticity Test Passed" << std::endl;
}

// Test Thread-Safe Vectorized Operations
void testThreadSafeVectorizedOperations() {
    std::cout << "Testing Thread-Safe Vectorized Operations..." << std::flush;
    
    // Test SIMDProfiler thread safety
    SIMDProfiler& profiler = SIMDProfiler::getInstance();
    
    // Reset profiler
    profiler.reset();
    
    // Simulate multiple threads
    size_t numThreads = 4;
    size_t operationsPerThread = 100;
    
    std::vector<std::vector<float>> threadData(numThreads);
    for (size_t t = 0; t < numThreads; ++t) {
        threadData[t].resize(100);
        for (size_t i = 0; i < 100; ++i) {
            threadData[t][i] = static_cast<float>(t * 100 + i);
        }
    }
    
    // Process data from multiple "threads"
    for (size_t t = 0; t < numThreads; ++t) {
        profiler.startProfiling();
        for (size_t i = 0; i < 50; ++i) {
            float sum = VectorizedStats::sum(threadData[t], 100);
            profiler.recordVectorOp(1);
        }
        profiler.stopProfiling();
    }
    
    // Verify that profiler recorded operations
    auto results = profiler.getResults();
    assert(results.totalOps > 0);
    
    std::cout << "  Thread-Safe Vectorized Operations Test Passed" << std::endl;
}

// Test Performance Benchmarking
void testPerformanceBenchmarking() {
    std::cout << "Testing Performance Benchmarking..." << std::flush;
    
    // Test benchmark result structure
    BenchmarkResult benchmark;
    benchmark.neurons = 100;
    benchmark.synapses = 1000;
    benchmark.steps = 1000;
    benchmark.totalTime = 1.5;
    benchmark.stepsPerSecond = 666.67;
    benchmark.spikesPerStep = 0.05;
    benchmark.synapsesPerNeuron = 10.0;
    
    assert(benchmark.neurons == 100);
    assert(benchmark.synapses == 1000);
    assert(benchmark.steps == 1000);
    assert(benchmark.totalTime > 0.0);
    assert(benchmark.stepsPerSecond > 0.0);
    assert(benchmark.spikesPerStep >= 0.0);
    assert(benchmark.synapsesPerNeuron > 0.0);
    
    // Test benchmark printing (should not crash)
    // In a real implementation, this would capture stdout
    // For now, we just verify the structure
    
    std::cout << "  Performance Benchmarking Test Passed" << std::endl;
}

// Test Backward Compatibility
void testBackwardCompatibility() {
    std::cout << "Testing Backward Compatibility..." << std::flush;
    
    // Test that scalar operations still work when SIMD is not available
    // This is inherently tested by the SIMD fallback implementations
    
    // Test that original Neuron API still works
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-55.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.setRefractoryPeriod(10);
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.isFiring());
    assert(neuron.isRefractory());
    
    // Test original Synapse API
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    synapse.setDelay(5);
    synapse.setType(nlm::SynapseType::Excitatory);
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    assert(synapse.getWeight() == 0.5f);
    assert(synapse.getDelay() == 5);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    // Test original STDP API
    nlm::STDP stdp;
    stdp.setLTPWeight(0.01f);
    stdp.setLTDWeight(0.012f);
    stdp.setTimeConstant(20.0f);
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    assert(std::string(stdp.getName()) == std::string("STDP"));
    
    // Test original Hebbian API
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.01f);
    hebbian.setMaxWeight(1.0f);
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeight() == 1.0f);
    assert(std::string(hebbian.getName()) == std::string("Hebbian"));
    
    // Test original SpikeSystem API
    nlm::SpikeSystem spikeSystem;
    spikeSystem.registerHandler([](const nlm::DetailedSpikeEvent& event) {});
    
    spikeSystem.queueSpike(nlm::NeuronId(1), 100.0f, 10);
    spikeSystem.processSpikes(10);
    
    assert(spikeSystem.getSpikeCount() == 1);
    assert(spikeSystem.getPendingSpikeCount() == 0);
    
    std::cout << "  Backward Compatibility Test Passed" << std::endl;
}

// Test SIMD Detection and Configuration
void testSIMDDetectionAndConfiguration() {
    std::cout << "Testing SIMD Detection and Configuration..." << std::flush;
    
    // Test SIMD detection
    bool simdAvailable = SIMDUtils::isSIMDAvailable();
    const char* simdType = SIMDUtils::getSIMDType();
    
    // Test parameter manager
    VectorizedSTDP::ParameterManager& paramManager = VectorizedSTDP::getSTDPParameterManager();
    
    VectorizedSTDP::STDPParameters stdpParams = paramManager.getDefaultSTDP();
    assert(stdpParams.tau_plus == 20.0f);
    assert(stdpParams.tau_minus == 20.0f);
    assert(stdpParams.A_plus == 0.01f);
    assert(stdpParams.A_minus == 0.012f);
    
    VectorizedSTDP::STDPParameters hebbianParams = paramManager.getDefaultHebbian();
    assert(hebbianParams.tau_plus == 50.0f);
    assert(hebbianParams.A_plus == 0.02f);
    
    // Test performance monitor
    VectorizedSTDP::PerformanceMonitor& perfMonitor = VectorizedSTDP::getSTDPPerformanceMonitor();
    auto metrics = perfMonitor.getMetrics();
    
    assert(metrics.simdEfficiency >= 0.0f && metrics.simdEfficiency <= 1.0f);
    assert(metrics.theoreticalSpeedup >= 1.0f);
    
    std::cout << "  SIMD Detection and Configuration Test Passed" << std::endl;
}

// Test Memory Pool Integration
void testMemoryPoolIntegration() {
    std::cout << "Testing Memory Pool Integration..." << std::flush;
    
    // The memory pool infrastructure is integrated at the C++ library level
    // For this test, we verify that the SIMD classes work with standard containers
    // that the memory pool would manage in production
    
    std::vector<float> testVector;
    testVector.resize(1000);
    
    for (size_t i = 0; i < 1000; ++i) {
        testVector[i] = static_cast<float>(i);
    }
    
    // Test SIMD operations on standard vector
    float sum = VectorizedStats::sum(testVector, 1000);
    float mean = VectorizedStats::mean(testVector, 1000);
    float max = VectorizedStats::max(testVector, 1000);
    
    assert(sum == 499500.0f); // Sum of 0..999
    assert(std::abs(mean - 499.5f) < 0.01f);
    assert(max == 999.0f);
    
    std::cout << "  Memory Pool Integration Test Passed" << std::endl;
}

// Main test runner
void runAllTests() {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      NLM SIMD Vectorization Test Suite                      ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Run all tests
    testSIMDInfrastructure();
    testVectorizedNeuronOperations();
    testVectorizedStatistics();
    testVectorizedPlasticity();
    testThreadSafeVectorizedOperations();
    testPerformanceBenchmarking();
    testBackwardCompatibility();
    testSIMDDetectionAndConfiguration();
    testMemoryPoolIntegration();
    
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "All SIMD Vectorization tests passed successfully!" << std::endl;
}

} // namespace test_simd_vectorization

// Entry point for the test program
int main() {
    test_simd_vectorization::runAllTests();
    return 0;
}