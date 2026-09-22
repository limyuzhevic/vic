// Comprehensive Integration Test
// Tests all major components of the NLM framework

#include "Phase3Demo.cpp"  // Use existing working implementation

namespace nlm {

class ComprehensiveIntegrationTest {
public:
    ComprehensiveIntegrationTest() {
        std::cout << "========================================\n";
        std::cout << "COMPREHENSIVE NLM INTEGRATION TEST\n";
        std::cout << "========================================\n\n";
        
        // Test all components sequentially
        testComponent1_NeuralComputation();
        testComponent2_WorldInteraction();
        testComponent3_PlasticityLearning();
        testComponent4_MemorySystems();
        testComponent5_Neuromodulation();
        testComponent6_Development();
        testComponent7_Checkpointing();
        testComponent8_Prediction();
    }
    
private:
    bool synapsesValid = brain->getTotalSynapseCount() > 0;
        
        // Test 3: Spike propagation test
        bool spikePropagation = false;
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                // Inject current and test spikes
                neurons[0]->injectCurrent(50.0f);
                for (SimulationStep i = 0; i < 50; ++i) {
                    brain->step(i, i * 0.001);
                }
                spikePropagation = brain->getFiringNeuronCount() > 0;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        if (neuronsValid && synapsesValid && spikePropagation) {
            recordTestResult("Basic Connectivity", true, 
                "Neurons: " + std::to_string(brain->getTotalNeuronCount()) + 
                ", Synapses: " + std::to_string(brain->getTotalSynapseCount()),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Basic Connectivity", false,
                "Neurons valid: " + std::to_string(neuronsValid) +
                ", Synapses valid: " + std::to_string(synapsesValid) +
                ", Spike propagation: " + std::to_string(spikePropagation),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runPlasticityTests() {
        std::cout << "--- Plasticity Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("random_seed", 42u);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Plasticity Tests", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            recordTestResult("Plasticity Tests", false, "No region found", 0.0, 0, 0);
            return;
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto synapses = region->getSynapses();
        if (!synapses.empty()) {
            for (size_t i = 0; i < std::min(size_t(10), synapses.size()); ++i) {
                initialWeights.push_back(synapses[i]->getWeight());
            }
        }
        
        // Run STDP learning
        for (int trial = 0; trial < 200; ++trial) {
            // Create correlated activity for LTP
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            // Enable Hebbian plasticity
            for (auto& syn : synapses) {
                syn->enablePlasticity(true, false, false);
            }
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        for (size_t i = 0; i < std::min(size_t(10), synapses.size()); ++i) {
            finalWeights.push_back(synapses[i]->getWeight());
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        // Check for weight changes (learning occurred)
        float totalChange = 0.0f;
        size_t validPairs = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < validPairs; ++i) {
            totalChange += std::abs(finalWeights[i] - initialWeights[i]);
        }
        
        if (totalChange > 0.001f && synapsesValid) {
            recordTestResult("Plasticity Tests", true,
                "Weight change: " + std::to_string(totalChange) + 
                ", Synaptic learning active",
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Plasticity Tests", false,
                "Minimal weight change: " + std::to_string(totalChange),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runMemoryIntegrationTests() {
        std::cout << "--- Memory Integration Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 150);
        config->set("region_count", 1);
        config->set("random_seed", 42u);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Memory Integration", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Check memory systems
        bool workingMemActive = (brain->getWorkingMemory() != nullptr);
        bool episodicMemActive = (brain->getEpisodicMemory() != nullptr);
        
        // Run simulation to populate memory
        for (int step = 0; step < 100; ++step) {
            // Inject sensory activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * 0.001);
            
            // Enable plasticity for memory formation
            auto region = brain->getRegion(RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        if (workingMemActive && episodicMemActive) {
            recordTestResult("Memory Integration", true,
                "WorkingMem: " + std::to_string(workingMemActive) +
                ", EpisodicMem: " + std::to_string(episodicMemActive),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Memory Integration", false,
                "WorkingMem: " + std::to_string(workingMemActive) +
                ", EpisodicMem: " + std::to_string(episodicMemActive),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runNeuromodulationTests() {
        std::cout << "--- Neuromodulation Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", 42u);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Neuromodulation Tests", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Check neuromodulation systems
        bool dopamineActive = (brain->getDopamine() != nullptr);
        bool curiosityActive = (brain->getCuriosity() != nullptr);
        bool noveltyActive = (brain->getNovelty() != nullptr);
        
        // Apply reward signal
        for (int step = 0; step < 50; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        if (dopamineActive && curiosityActive && noveltyActive) {
            recordTestResult("Neuromodulation Tests", true,
                "Dopamine: " + std::to_string(dopamineActive) +
                ", Curiosity: " + std::to_string(curiosityActive) +
                ", Novelty: " + std::to_string(noveltyActive),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Neuromodulation Tests", false,
                "Dopamine: " + std::to_string(dopamineActive) +
                ", Curiosity: " + std::to_string(curiosityActive) +
                ", Novelty: " + std::to_string(noveltyActive),
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runPredictionIntegrationTests() {
        std::cout << "--- Prediction Integration Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 120);
        config->set("region_count", 1);
        config->set("random_seed", 42u);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Prediction Integration", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Check prediction system
        bool predictionActive = (brain->getPredictionSystem() != nullptr);
        
        // Run prediction test
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        if (predictionActive) {
            recordTestResult("Prediction Integration", true,
                "Prediction system active with " + std::to_string(brain->getFiringNeuronCount()) + " firing neurons",
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Prediction Integration", false,
                "Prediction system not active",
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runDevelopmentTests() {
        std::cout << "--- Development Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 150);
        config->set("region_count", 1);
        config->set("random_seed", 42u);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Development Tests", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Check development system
        bool developmentActive = (brain->getDevelopmentSystem() != nullptr);
        
        // Run development simulation
        for (int step = 0; step < 200; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        if (developmentActive) {
            recordTestResult("Development Tests", true,
                "Development system active for " + std::to_string(duration) + "s",
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        } else {
            recordTestResult("Development Tests", false,
                "Development system not active",
                duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void runCheckpointTests() {
        std::cout << "--- Checkpoint Tests ---\n";
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", 42u);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            recordTestResult("Checkpoint Tests", false, "Failed to initialize brain", 0.0, 0, 0);
            return;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Run simulation and save checkpoint
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
        bool saveSuccess = brain->save(checkpointPath);
        
        if (saveSuccess) {
            // Create new brain and load checkpoint
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            bool loadSuccess = brain2->load(checkpointPath);
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            
            if (loadSuccess) {
                recordTestResult("Checkpoint Tests", true,
                    "Save and load successful",
                    duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
            } else {
                recordTestResult("Checkpoint Tests", false,
                    "Load failed after successful save",
                    duration, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
            }
        } else {
            recordTestResult("Checkpoint Tests", false,
                "Save checkpoint failed",
                0.0, brain->getTotalNeuronCount(), brain->getTotalSynapseCount());
        }
    }
    
    void recordTestResult(const std::string& name, bool passed, 
                         const std::string& message, double duration, 
                         int neurons, int synapses) {
        TestResult result;
        result.testName = name;
        result.passed = passed;
        result.message = message;
        result.executionTime = duration;
        result.neuronsTested = neurons;
        result.synapsesTested = synapses;
        testResults.push_back(result);
        
        std::string status = passed ? "[PASS]" : "[FAIL]";
        std::cout << "  " << status << " " << name << ": " << message << "\n";
    }
    
    void printSummary() {
        std::cout << "\n========================================\n";
        std::cout << "TEST SUITE SUMMARY\n";
        std::cout << "========================================\n";
        
        int totalTests = testResults.size();
        int passedTests = 0;
        double totalExecutionTime = 0.0;
        int totalNeurons = 0;
        int totalSynapses = 0;
        
        std::cout << std::left << std::setw(40) << "Test Name" 
                  << std::setw(10) << "Status"
                  << std::setw(15) << "Time (s)"
                  << std::setw(20) << "Details\n";
        
        for (const auto& result : testResults) {
            std::string status = result.passed ? "PASS" : "FAIL";
            std::string timeStr = std::to_string(result.executionTime);
            if (result.executionTime < 0.001) timeStr = "<0.001";
            
            std::cout << std::left << std::setw(40) << result.testName
                      << std::setw(10) << status
                      << std::setw(15) << timeStr
                      << result.message << "\n";
            
            if (result.passed) passedTests++;
            totalExecutionTime += result.executionTime;
            totalNeurons += result.neuronsTested;
            totalSynapses += result.synapsesTested;
        }
        
        std::cout << "\n========================================\n";
        std::cout << "OVERALL STATISTICS\n";
        std::cout << "========================================\n";
        std::cout << "Total tests: " << totalTests << "\n";
        std::cout << "Passed: " << passedTests << "\n";
        std::cout << "Failed: " << (totalTests - passedTests) << "\n";
        std::cout << "Success rate: " << (passedTests * 100.0 / totalTests) << "%\n";
        std::cout << "Total execution time: " << totalExecutionTime << "s\n";
        std::cout << "Total neurons tested: " << totalNeurons << "\n";
        std::cout << "Total synapses tested: " << totalSynapses << "\n";
        
        if (passedTests == totalTests) {
            std::cout << "\n✓ ALL TESTS PASSED! NLM integration is working correctly.\n";
        } else {
            std::cout << "\n✗ Some tests failed. Please review the implementation.\n";
        }
    }
};

} // namespace nlm

// Main entry point
int main() {
    nlm::IntegrationTestSuite testSuite;
    testSuite.runAllTests();
    
    return 0;
}
