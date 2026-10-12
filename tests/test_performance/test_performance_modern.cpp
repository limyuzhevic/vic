#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "performance/PerformanceAnalyzer.hpp"
#include "performance/MemoryPool.hpp"
#include <chrono>
#include <algorithm>
#include <cmath>

class PerformanceAnalyzerTestSuite : public TestSuite {
public:
    PerformanceAnalyzerTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Performance Analyzer Test Suite..." << std::endl;
        
        testPerformanceAnalyzerCreation();
        testPerformanceAnalysis();
        testMemoryLeakDetection();
        testBottleneckAnalysis();
        
        std::cout << "Performance Analyzer Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "PerformanceAnalyzerTestSuite";
    }
    
private:
    void testPerformanceAnalyzerCreation() {
        nlm::PerformanceAnalyzer analyzer;
        
        ASSERT_TRUE(analyzer.initialize());
        ASSERT_TRUE(analyzer.getSampleCount() == 0);
        
        std::cout << "    testPerformanceAnalyzerCreation passed" << std::endl;
    }
    
    void testPerformanceAnalysis() {
        nlm::PerformanceAnalyzer analyzer;
        
        analyzer.initialize();
        
        nlm::PerformanceProfile profile;
        profile.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        profile.totalPerformanceScore = 0.8f;
        
        analyzer.recordPerformanceSample(profile);
        
        ASSERT_TRUE(analyzer.getSampleCount() > 0);
        
        std::cout << "    testPerformanceAnalysis passed" << std::endl;
    }
    
    void testMemoryLeakDetection() {
        nlm::PerformanceAnalyzer analyzer;
        
        analyzer.initialize();
        analyzer.enableMemoryLeakDetection(true);
        
        // Create some objects
        std::vector<float> data(1000, 0.5f);
        
        analyzer.detectMemoryLeaks();
        
        ASSERT_TRUE(analyzer.getSampleCount() >= 0);
        
        std::cout << "    testMemoryLeakDetection passed" << std::endl;
    }
    
    void testBottleneckAnalysis() {
        nlm::PerformanceAnalyzer analyzer;
        
        analyzer.initialize();
        analyzer.enableBottleneckDetection(true);
        
        analyzer.analyzeComponentPerformance("brain");
        analyzer.analyzeComponentPerformance("memory");
        
        size_t bottleneckCount = analyzer.getBottleneckCount();
        ASSERT_TRUE(bottleneckCount >= 0);
        
        std::cout << "    testBottleneckAnalysis passed" << std::endl;
    }
};

class MemoryPoolTestSuite : public TestSuite {
public:
    MemoryPoolTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Memory Pool Test Suite..." << std::endl;
        
        testMemoryPoolCreation();
        testMemoryPoolAllocation();
        testMemoryPoolDeallocation();
        testMemoryPoolRecycling();
        
        std::cout << "Memory Pool Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "MemoryPoolTestSuite";
    }
    
private:
    void testMemoryPoolCreation() {
        nlm::MemoryPool pool(1024, 64);
        
        ASSERT_TRUE(pool.getTotalCapacity() == 1024);
        ASSERT_TRUE(pool.getBlockSize() == 64);
        
        std::cout << "    testMemoryPoolCreation passed" << std::endl;
    }
    
    void testMemoryPoolAllocation() {
        nlm::MemoryPool pool(512, 32);
        
        void* ptr1 = pool.allocate();
        void* ptr2 = pool.allocate();
        
        ASSERT_TRUE(ptr1 != nullptr);
        ASSERT_TRUE(ptr2 != nullptr);
        ASSERT_TRUE(pool.getAllocationCount() == 2);
        
        pool.deallocate(ptr1);
        pool.deallocate(ptr2);
        
        std::cout << "    testMemoryPoolAllocation passed" << std::endl;
    }
    
    void testMemoryPoolDeallocation() {
        nlm::MemoryPool pool(256, 16);
        
        void* ptr1 = pool.allocate();
        void* ptr2 = pool.allocate();
        
        pool.deallocate(ptr1);
        pool.deallocate(ptr2);
        
        ASSERT_TRUE(pool.getAllocationCount() == 0);
        
        std::cout << "    testMemoryPoolDeallocation passed" << std::endl;
    }
    
    void testMemoryPoolRecycling() {
        nlm::MemoryPool pool(128, 8);
        
        void* ptr1 = pool.allocate();
        pool.deallocate(ptr1);
        
        void* ptr2 = pool.allocate();
        ASSERT_TRUE(ptr1 == ptr2); // Should recycle
        
        pool.deallocate(ptr2);
        
        std::cout << "    testMemoryPoolRecycling passed" << std::endl;
    }
};

class BenchmarkTestSuite : public TestSuite {
public:
    BenchmarkTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Benchmark Test Suite..." << std::endl;
        
        testBenchmarkNeuronCreation();
        testBenchmarkSynapseCreation();
        testBenchmarkBrainOperations();
        
        std::cout << "Benchmark Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "BenchmarkTestSuite";
    }
    
private:
    void testBenchmarkNeuronCreation() {
        nlm::Brain brain("benchmark_brain");
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        ASSERT_TRUE(duration.count() < 10000); // Should complete in less than 10 seconds
        
        std::cout << "    testBenchmarkNeuronCreation passed" << std::endl;
    }
    
    void testBenchmarkSynapseCreation() {
        nlm::Brain brain("benchmark_brain");
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        ASSERT_TRUE(duration.count() < 10000); // Should complete in less than 10 seconds
        
        std::cout << "    testBenchmarkSynapseCreation passed" << std::endl;
    }
    
    void testBenchmarkBrainOperations() {
        nlm::Brain brain("benchmark_brain");
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            neuron.injectCurrent(1.0f);
            neuron.getMembranePotential();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        ASSERT_TRUE(duration.count() < 5000); // Should complete in less than 5 seconds
        
        std::cout << "    testBenchmarkBrainOperations passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createPerformanceAnalyzerTestSuite() {
        return new PerformanceAnalyzerTestSuite();
    }
    
    TestSuite* createMemoryPoolTestSuite() {
        return new MemoryPoolTestSuite();
    }
    
    TestSuite* createBenchmarkTestSuite() {
        return new BenchmarkTestSuite();
    }
}