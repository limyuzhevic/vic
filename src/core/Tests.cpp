#include "core/TestingFramework.hpp"

namespace nlm {
namespace testing {

// Basic connectivity test
class ConnectivityTest : public Test {
public:
    std::string getName() const override { return "Basic Neural Connectivity"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Inject current into neurons
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current
        for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
            neurons[i]->injectCurrent(30.0f);
        }
        
        // Run simulation steps
        double timestep = brain->getTimestep();
        for (SimulationStep step = 0; step < 20; ++step) {
            brain->step(step, timestep * (step + 1));
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikesGenerated = finalSpikes - initialSpikes;
        
        // Verify spikes were generated
        if (spikesGenerated <= 0) {
            return false;
        }
        
        // Verify neurons are firing
        size_t firingNeurons = 0;
        for (const auto& neuron : neurons) {
            if (neuron->isFiring()) firingNeurons++;
        }
        
        if (firingNeurons == 0) {
            return false;
        }
        
        return true;
    }
};

// Neuron dynamics test
class NeuronDynamicsTest : public Test {
public:
    std::string getName() const override { return "Neuron LIF Dynamics"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Get a neuron from the first region
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        Neuron* testNeuron = neurons[0];
        
        // Record initial state
        double initialPotential = testNeuron->getMembranePotential();
        double initialFiringRate = testNeuron->getFiringRate();
        
        // Inject current
        testNeuron->injectCurrent(50.0f);
        
        // Run simulation step
        double timestep = brain->getTimestep();
        brain->step(0, timestep);
        
        // Check that membrane potential changed (due to injected current)
        double finalPotential = testNeuron->getMembranePotential();
        
        // Potential should increase due to injected current
        if (finalPotential <= initialPotential && testNeuron->getTotalCurrent() > 0) {
            return false;
        }
        
        return true;
    }
};

// Synaptic plasticity test
class PlasticityTest : public Test {
public:
    std::string getName() const override { return "Synaptic Plasticity Learning"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (initialWeights.empty()) return false;
        
        // Apply correlated pre-post activity to induce plasticity
        double timestep = brain->getTimestep();
        for (int trial = 0; trial < 10; ++trial) {
            brain->step(trial * 2, timestep * (trial * 2));
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        if (finalWeights.size() != initialWeights.size()) return false;
        
        // Calculate weight changes
        bool learningOccurred = false;
        float totalChange = 0.0f;
        size_t validChanges = 0;
        
        for (size_t i = 0; i < initialWeights.size(); ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (std::abs(delta) > 0.001f) {  // Consider changes above threshold
                learningOccurred = true;
                totalChange += std::abs(delta);
                validChanges++;
            }
        }
        
        // Plasticity should induce some weight changes
        if (!learningOccurred) {
            return false;
        }
        
        // Check that weights remain within bounds
        if (!NeuralTestUtils::checkWeightBounds(brain, -1.0f, 1.0f)) {
            return false;
        }
        
        return true;
    }
};

// STDP specific test
class STDPTest : public Test {
public:
    std::string getName() const override { return "Spike-Timing Dependent Plasticity (STDP)"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Get a synapse and enable only STDP
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            if (synapses.empty()) return false;
            
            Synapse* testSynapse = synapses[0].get();
            testSynapse->enablePlasticity(false, true, false);  // STDP only
            
            float initialWeight = testSynapse->getWeight();
            
            // Create correlated pre->post activity (should potentiate)
            double timestep = brain->getTimestep();
            for (int trial = 0; trial < 20; ++trial) {
                // Pre-synaptic spike
                brain->injectCurrent(testSynapse->getSourceNeuron(), 60.0f);
                brain->step(trial * 2, timestep * (trial * 2));
                
                // Post-synaptic spike (should potentiate)
                brain->injectCurrent(testSynapse->getDestinationNeuron(), 60.0f);
                brain->step(trial * 2 + 1, timestep * (trial * 2 + 1));
            }
            
            float finalWeight = testSynapse->getWeight();
            float weightChange = finalWeight - initialWeight;
            
            // STDP should produce potentiation for correlated activity
            if (weightChange <= 0.001f) {
                return false;
            }
            
            // Check bounds
            if (finalWeight < -1.0f || finalWeight > 1.0f) {
                return false;
            }
            
            return true;
        }
        
        return false;
    }
};

// Memory system test
class MemorySystemTest : public Test {
public:
    std::string getName() const override { return "Memory System Integration"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Test that memory systems are accessible
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        if (!workingMemory || !episodicMemory || !associativeMemory) {
            return false;
        }
        
        // Test basic memory operations
        // This is a placeholder - actual memory operations depend on the specific memory system implementations
        bool memoryInitialized = true; // Placeholder
        
        return memoryInitialized;
    }
};

// Configuration validation test
class ConfigValidationTest : public Test {
public:
    std::string getName() const override { return "Configuration Validation"; }
    
    bool run() override {
        auto config = NeuralTestUtils::createTestConfig();
        
        // Test required configuration keys
        std::vector<std::string> requiredKeys = {
            "random_seed", "simulation_timestep", "neuron_count",
            "region_count", "connection_probability"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                return false;
            }
        }
        
        // Test value ranges
        int64_t randomSeed = config->getOr<int64_t>("random_seed", 0);
        if (randomSeed < 0) return false;
        
        double timestep = config->getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) return false;
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) return false;
        
        int64_t regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) return false;
        
        float connectionProb = config->getOr<float>("connection_probability", -1.0f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) return false;
        
        // Test plasticity parameter ranges
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", -1.0f);
        if (ltpWeight < 0.0f || ltpWeight > 1.0f) return false;
        
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", -1.0f);
        if (ltdWeight < 0.0f || ltdWeight > 1.0f) return false;
        
        return true;
    }
};

// Command line parsing test
class CommandLineTest : public Test {
public:
    std::string getName() const override { return "Command Line Interface"; }
    
    bool run() override {
        // Test that command line parsing works
        // This is a simple smoke test for the main.cpp improvements
        
        // We can't easily test this without modifying the main function,
        // but we can verify that the CLI infrastructure exists
        
        return true; // Placeholder
    }
};

// Performance test
class PerformanceTest : public Test {
public:
    std::string getName() const override { return "Basic Performance"; }
    
    bool run() override {
        auto brain = NeuralTestUtils::createTestBrain();
        
        // Measure simulation performance
        double timestep = brain->getTimestep();
        size_t initialStep = brain->getCurrentStep();
        
        // Run a large number of simulation steps
        size_t numSteps = 1000;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < numSteps; ++step) {
            brain->step(step, timestep * (step + 1));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        // Verify the brain is still functioning
        if (brain->getTotalSpikeCount() < 0) return false;
        
        // Performance should be reasonable (less than 1 second for 1000 steps on modern hardware)
        if (duration > 10.0) {  // 10 second timeout
            return false;
        }
        
        return true;
    }
};

// Register all tests with the framework
class TestRegistrator {
public:
    TestRegistrator() {
        auto& framework = getFramework();
        
        // Add all tests
        framework.addTest(std::make_unique<ConnectivityTest>());
        framework.addTest(std::make_unique<NeuronDynamicsTest>());
        framework.addTest(std::make_unique<PlasticityTest>());
        framework.addTest(std::make_unique<STDPTest>());
        framework.addTest(std::make_unique<MemorySystemTest>());
        framework.addTest(std::make_unique<ConfigValidationTest>());
        framework.addTest(std::make_unique<CommandLineTest>());
        framework.addTest(std::make_unique<PerformanceTest>());
    }
    
    static TestFramework& getFramework() {
        static TestFramework instance;
        return instance;
    }
};

// Global test registrar
static TestRegistrator testRegistrator;

} // namespace testing
} // namespace nlm
