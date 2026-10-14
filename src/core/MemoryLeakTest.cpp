#include "MemoryManagementTestSuite.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

MemoryLeakTest::MemoryLeakTest() : MemoryManagementTest("Memory Leak Test", 
    "Tests for memory leaks in object construction and destruction") {}

void MemoryLeakTest::setup() {
    recordStartSnapshot();
    createObjects(100);
}

void MemoryLeakTest::cleanup() {
    releaseObjects();
    recordEndSnapshot();
}

TestResult MemoryLeakTest::run() {
    TestResult result("Memory Leak Test", description_);
    
    try {
        setup();
        
        // Force garbage collection
        forceGarbageCollection();
        
        // Check memory leaks
        bool hasLeaks = checkMemoryLeaks();
        
        cleanup();
        
        if (hasLeaks) {
            result.setError("Memory leaks detected");
            result.setPassed(false);
        } else {
            result.setPassed(true);
            result.setPerformance(0.1, getMemoryDelta()); // Dummy values
        }
        
    } catch (const std::exception& e) {
        cleanup();
        result.setError(std::string("Exception: ") + e.what());
        result.setPassed(false);
    }
    
    return result;
}

void MemoryLeakTest::createObjects(size_t count) {
    neurons_.clear();
    synapses_.clear();
    neuromodulators_.clear();
    plasticityRules_.clear();
    
    for (size_t i = 0; i < count; ++i) {
        try {
            neurons_.emplace_back(std::make_unique<Neuron>(NeuronId(i)));
            synapses_.emplace_back(std::make_unique<Synapse>(SynapseId(i), 
                                                           NeuronId(i), NeuronId(i + 1)));
            neuromodulators_.emplace_back(std::make_unique<Curiosity>());
            plasticityRules_.emplace_back(std::make_unique<STDP>());
        } catch (...) {
            // Skip failed allocations
        }
    }
}

void MemoryLeakTest::releaseObjects() {
    neurons_.clear();
    synapses_.clear();
    neuromodulators_.clear();
    plasticityRules_.clear();
    
    forceGarbageCollection();
}

} // namespace nlm
