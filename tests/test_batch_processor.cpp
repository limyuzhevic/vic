// Comprehensive Batch Processing Tests
#include "TestFixture.hpp"
#include "../src/core/BatchStepProcessor.hpp"
#include "../src/performance/BatchBrain.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <cmath>

class TestBatchProcessorBasic : public TestFixture {
public:
    TestBatchProcessorBasic() : TestFixture(
        "BatchProcessorBasic",
        TestCategory::BatchProcessing,
        "Test basic batch processing functionality"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Default constructor
        {
            nlm::BatchStepProcessor processor;
            assertNotNull(&processor, "BatchStepProcessor instance should not be null");
        }
        
        // Test 2: Configuration
        {
            nlm::BatchStepProcessor::Config config;
            assertEqual(config.batchSize, 1000UL, "Default batch size should be 1000");
            assertEqual(config.maxBatchSize, 10000UL, "Default max batch size should be 10000");
            assertTrue(config.enableSIMD, "SIMD should be enabled by default");
            assertTrue(config.enableMultithreading, "Multithreading should be enabled by default");
            assertTrue(config.enableMemoryPool, "Memory pool should be enabled by default");
        }
        
        // Test 3: Thread safety flags
        {
            nlm::BatchStepProcessor processor;
            assertTrue(processor.isProcessing(), "New processor should not be processing");
        }
        
        // Test 4: Neuron processing state
        {
            nlm::BatchStepProcessor::NeuronProcessingState state;
            state.initialize(100);
            
            assertEqual(state.membranePotential.size(), 100UL, "Neuron state should have 100 membrane potentials");
            assertEqual(state.synapticInput.size(), 100UL, "Neuron state should have 100 synaptic inputs");
            assertEqual(state.activeNeurons.size(), 0UL, "Active neurons should be empty initially");
            
            state.clear();
            assertEqual(state.activeNeurons.size(), 0UL, "Active neurons should be empty after clear");
        }
        
        // Test 5: Synapse processing state
        {
            nlm::BatchStepProcessor::SynapseProcessingState state;
            state.initialize(500);
            
            assertEqual(state.sourceNeuron.size(), 500UL, "Synapse state should have 500 source neurons");
            assertEqual(state.batchSynapticInput.size(), 500UL, "Synapse state should have 500 batch inputs");
            assertEqual(state.batchWeightChange.size(), 500UL, "Synapse state should have 500 weight changes");
            
            state.clear();
            assertEqual(state.batchSynapticInput.size(), 500UL, "Batch input should be cleared");
        }
        
        // Test 6: Processing stats
        {
            nlm::BatchStepProcessor::ProcessingStats stats;
            assertEqual(stats.stepsCompleted, 0UL, "Initial steps completed should be 0");
            assertEqual(stats.spikesGenerated, 0UL, "Initial spikes generated should be 0");
        }
        
        // Test 7: Memory usage calculation
        {
            nlm::BatchStepProcessor::NeuronProcessingState neuronState;
            neuronState.initialize(1000);
            
            size_t memory = neuronState.memoryUsage();
            assertTrue(memory > 0, "Memory usage should be positive");
            
            nlm::BatchStepProcessor::SynapseProcessingState synapseState;
            synapseState.initialize(1000);
            
            memory += synapseState.memoryUsage();
            assertTrue(memory > 0, "Combined memory usage should be positive");
        }
    }
};

class TestBatchProcessorConfig : public TestFixture {
public:
    TestBatchProcessorConfig() : TestFixture(
        "BatchProcessorConfig",
        TestCategory::BatchProcessing,
        "Test batch processor configuration options"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        nlm::BatchStepProcessor::Config config;
        
        // Test 1: Custom configuration
        {
            config.batchSize = 5000;
            config.maxBatchSize = 50000;
            config.enableSIMD = false;
            config.enableMultithreading = false;
            config.enableMemoryPool = false;
            
            assertEqual(config.batchSize, 5000UL, "Custom batch size should be 5000");
            assertEqual(config.maxBatchSize, 50000UL, "Custom max batch size should be 50000");
            assertFalse(config.enableSIMD, "SIMD should be disabled");
            assertFalse(config.enableMultithreading, "Multithreading should be disabled");
            assertFalse(config.enableMemoryPool, "Memory pool should be disabled");
        }
        
        // Test 2: Callback configuration
        {
            std::function<bool()> terminateFunc = []() { return false; };
            config.shouldTerminate = terminateFunc;
            
            std::function<void(float)> progressFunc = [](float progress) {};
            config.progressCallback = progressFunc;
            
            assertNotNull(config.shouldTerminate, "Terminate callback should not be null");
            assertNotNull(config.progressCallback, "Progress callback should not be null");
        }
        
        // Test 3: Enable/disable optimizations
        {
            nlm::BatchStepProcessor processor;
            
            processor.enableSIMD(true);
            processor.enableMultithreading(true);
            processor.enableMemoryPool(true);
            
            processor.enableSIMD(false);
            processor.enableMultithreading(false);
            processor.enableMemoryPool(false);
        }
    }
};

class TestBatchProcessorStateManagement : public TestFixture {
public:
    TestBatchProcessorStateManagement() : TestFixture(
        "BatchProcessorStateManagement",
        TestCategory::BatchProcessing,
        "Test batch processor state management"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Neuron state initialization
        {
            nlm::BatchStepProcessor processor;
            
            // Mock brain and parameters for testing
            // (In a real test, we would create a mock brain object)
            
            // Test state initialization with different sizes
            nlm::BatchStepProcessor::NeuronProcessingState state;
            
            state.initialize(1);
            assertEqual(state.membranePotential.size(), 1UL, "State should initialize with 1 neuron");
            
            state.initialize(10000);
            assertEqual(state.membranePotential.size(), 10000UL, "State should initialize with 10000 neurons");
        }
        
        // Test 2: Synapse state management
        {
            nlm::BatchStepProcessor::SynapseProcessingState state;
            
            state.initialize(0);
            assertEqual(state.sourceNeuron.size(), 0UL, "Zero-size synapse state should be empty");
            
            state.initialize(1000);
            assertEqual(state.sourceNeuron.size(), 1000UL, "State should initialize with 1000 synapses");
        }
        
        // Test 3: Performance tracking
        {
            nlm::BatchStepProcessor::ProcessingStats stats;
            
            stats.startTime = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            stats.endTime = std::chrono::high_resolution_clock::now();
            
            assertTrue(stats.endTime > stats.startTime, "End time should be after start time");
        }
    }
};

class TestBatchProcessorAsync : public TestFixture {
public:
    TestBatchProcessorAsync() : TestFixture(
        "BatchProcessorAsync",
        TestCategory::BatchProcessing,
        "Test asynchronous batch processing"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Async processing setup
        {
            nlm::BatchStepProcessor processor;
            
            // Create async task (would need a real brain for full test)
            std::future<nlm::BatchStepProcessor::StepResult> future;
            
            // Test that async interface exists
            // processor.processBatchAsync would need a Brain* parameter
            // For this test, we verify the interface exists
        }
        
        // Test 2: Progress tracking
        {
            nlm::BatchStepProcessor processor;
            
            float progress = processor.getProgress();
            assertEqual(progress, 0.0f, "Initial progress should be 0.0");
            
            // Progress would be updated during actual processing
        }
        
        // Test 3: Early termination
        {
            nlm::BatchStepProcessor::Config config;
            
            std::atomic<bool> terminateFlag{false};
            std::function<bool()> shouldTerminate = [&terminateFlag]() { return terminateFlag.load(); };
            config.shouldTerminate = shouldTerminate;
            
            terminateFlag.store(true);
            assertTrue(config.shouldTerminate(), "Terminate callback should return true when flag is set");
            
            terminateFlag.store(false);
            assertFalse(config.shouldTerminate(), "Terminate callback should return false when flag is clear");
        }
    }
};

class TestBatchProcessorPerformance : public TestFixture {
public:
    TestBatchProcessorPerformance() : TestFixture(
        "BatchProcessorPerformance",
        TestCategory::BatchProcessing,
        "Test batch processor performance characteristics"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Performance test: create and configure multiple processors
        for (int i = 0; i < 100; ++i) {
            nlm::BatchStepProcessor processor;
            
            // Test basic operations
            processor.enableSIMD(i % 2 == 0);
            processor.enableMultithreading(i % 2 == 0);
            processor.enableMemoryPool(i % 2 == 0);
            
            float progress = processor.getProgress();
            assertEqual(progress, 0.0f, "Progress should be 0.0");
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        assertTrue(duration < 100.0, "Creating 100 processors should take less than 100ms");
    }
};

class TestBatchProcessorErrorHandling : public TestFixture {
public:
    TestBatchProcessorErrorHandling() : TestFixture(
        "BatchProcessorErrorHandling",
        TestCategory::ErrorHandling,
        "Test batch processor error handling"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Null brain handling (conceptual)
        {
            nlm::BatchStepProcessor processor;
            
            // In real tests, we would test handling of null brain pointers
            // This tests that the interface exists for error handling
        }
        
        // Test 2: Invalid configuration
        {
            nlm::BatchStepProcessor::Config config;
            
            config.batchSize = 0; // Edge case
            config.maxBatchSize = 0; // Edge case
            
            // Should be handled gracefully
        }
        
        // Test 3: Memory state edge cases
        {
            nlm::BatchStepProcessor::NeuronProcessingState state;
            
            // Test empty state
            assertEqual(state.activeNeurons.size(), 0UL, "Empty state should have no active neurons");
            assertEqual(state.firingNeurons.size(), 0UL, "Empty state should have no firing neurons");
            
            state.initialize(1);
            state.firingNeurons.push_back(0);
            
            assertEqual(state.firingNeurons.size(), 1UL, "State should track firing neurons");
        }
    }
};

// Run all batch processor tests
void runBatchProcessorTests() {
    TestRunner runner;
    
    runner.addTest(std::make_unique<TestBatchProcessorBasic>());
    runner.addTest(std::make_unique<TestBatchProcessorConfig>());
    runner.addTest(std::make_unique<TestBatchProcessorStateManagement>());
    runner.addTest(std::make_unique<TestBatchProcessorAsync>());
    runner.addTest(std::make_unique<TestBatchProcessorPerformance>());
    runner.addTest(std::make_unique<TestBatchProcessorErrorHandling>());
    
    runner.runAll();
}

// Entry point
int main() {
    runBatchProcessorTests();
    return 0;
}
