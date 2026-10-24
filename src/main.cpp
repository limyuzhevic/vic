// NLM (熙然) - Neural Learning Machine
// Generic Neural Computation Framework
//
// This is a generic neural computation framework that can be configured
// for different experimental phases (Phase 1, Phase 2, Phase 3, etc.)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <functional>

using namespace nlm;

// Forward declarations for generic test functions
class GenericNeuralTest;
using TestFunction = std::function<bool(std::shared_ptr<Brain>, const Config&)>;

// Generic test registry
class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    // Register a test function
    void registerTest(const std::string& name, const std::string& description, 
                      TestFunction test, bool enabled = true) {
        tests_[name] = Test{name, description, test, enabled};
    }
    
    // Get all registered tests
    std::vector<Test> getAllTests() const {
        std::vector<Test> tests;
        for (const auto& pair : tests_) {
            tests.push_back(pair.second);
        }
        return tests;
    }
    
    // Get a specific test
    const Test* getTest(const std::string& name) const {
        auto it = tests_.find(name);
        if (it != tests_.end()) return &it->second;
        return nullptr;
    }
    
    // Enable/disable a test
    void setTestEnabled(const std::string& name, bool enabled) {
        auto it = tests_.find(name);
        if (it != tests_.end()) {
            it->second.enabled = enabled;
        }
    }
    
    // Clear all tests
    void clear() {
        tests_.clear();
    }
    
    // Get tests enabled from configuration
    std::vector<Test> getEnabledTests(const Config& config) const {
        std::vector<Test> enabledTests;
        for (const auto& pair : tests_) {
            const auto& test = pair.second;
            if (test.enabled) {
                // Check if test is enabled in configuration
                std::string enableKey = "test_" + pair.first + "_enabled";
                if (!config.has(enableKey) || config.getOr<bool>(enableKey, true)) {
                    enabledTests.push_back(test);
                }
            }
        }
        return enabledTests;
    }
    
private:
    struct Test {
        std::string name;
        std::string description;
        TestFunction function;
        bool enabled;
    };
    
    std::map<std::string, Test> tests_;
    
    TestRegistry() = default;
    ~TestRegistry() = default;
    TestRegistry(const TestRegistry&) = delete;
    TestRegistry& operator=(const TestRegistry&) = delete;
};

// Generic neural computation tests
namespace GenericTests {
    
    // Test 1: Basic neural connectivity (phase-agnostic)
    bool testBasicConnectivity(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain region found for basic connectivity test");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons (configurable)
        NLM_LOG_INFO("  Injecting current into " + std::to_string(std::min(size_t(10), neurons.size())) + " neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            float current = config.getOr<float>("test_basic_current", 50.0f);
            neurons[i]->injectCurrent(current);  // Strong excitatory input
        }
        
        // Run a few steps (configurable)
        size_t steps = config.getOr<size_t>("test_basic_steps", 50);
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            return false;
        }
    }
    
    // Test 2: Plasticity learning experiment (phase-agnostic)
    bool testPlasticityExperiment(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Plasticity Learning Experiment ===");
        
        // Create lightweight experiment struct
        struct LightweightLearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed;
            
            LightweightLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s) {}
            
            void recordInitialState() {
                // Simple initial state recording
                NLM_LOG_INFO("Initial state recorded:");
                NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
            }
            
            void applyPlasticityStimulus(size_t steps) {
                // Apply repeated input pattern to stimulate learning
                NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
                
                for (SimulationStep step = 0; step < steps; ++step) {
                    // Create input pattern - inject current into sensory neurons
                    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    }
                    
                    brain->step(step, step * 0.001);
                    
                    // Log progress every 100 steps
                    if (step % 100 == 0) {
                        NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                    " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                    " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                    }
                }
            }
            
            void recordFinalState() {
                NLM_LOG_INFO("Final state recorded:");
                NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        };
        
        LightweightLearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply plasticity stimulus
        size_t plasticitySteps = config.getOr<size_t>("test_plasticity_steps", 1000);
        experiment.applyPlasticityStimulus(plasticitySteps);
        
        // Record final state
        experiment.recordFinalState();
        
        return true;
    }
    
    // Test 3: STDP verification (phase-agnostic)
    bool testStdpVerification(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain region found for STDP test");
            return false;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        auto neurons = region->getAllNeurons();
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            return false;
        }
    }
    
    // Test 4: Memory system integration (phase-agnostic)
    bool testMemoryIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Memory System Integration ===");
        
        // Check if memory systems are present and connected
        bool hasWorkingMemory = false;
        bool hasEpisodicMemory = false;
        bool hasAssociativeMemory = false;
        
        // Check working memory
        if (brain->getWorkingMemory()) {
            hasWorkingMemory = true;
            NLM_LOG_INFO("  ✓ Working memory system present");
        }
        
        // Check episodic memory  
        if (brain->getEpisodicMemory()) {
            hasEpisodicMemory = true;
            NLM_LOG_INFO("  ✓ Episodic memory system present");
        }
        
        // Check associative memory
        if (brain->getAssociativeMemory()) {
            hasAssociativeMemory = true;
            NLM_LOG_INFO("  ✓ Associative memory system present");
        }
        
        if (hasWorkingMemory && hasEpisodicMemory && hasAssociativeMemory) {
            NLM_LOG_INFO("  ✓ All memory systems integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Some memory systems missing or not integrated");
            return false;
        }
    }
    
    // Test 5: Neuromodulation integration (phase-agnostic)
    bool testNeuromodulationIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Neuromodulation Integration ===");
        
        bool hasDopamine = false;
        bool hasCuriosity = false;
        bool hasNovelty = false;
        bool hasPredictionError = false;
        
        // Check neuromodulation systems
        if (brain->getNeuromodulator(NeuromodulatorType::Dopamine)) {
            hasDopamine = true;
            NLM_LOG_INFO("  ✓ Dopamine neuromodulation present");
        }
        
        if (brain->getNeuromodulator(NeuromodulatorType::Curiosity)) {
            hasCuriosity = true;
            NLM_LOG_INFO("  ✓ Curiosity neuromodulation present");
        }
        
        if (brain->getNeuromodulator(NeuromodulatorType::Novelty)) {
            hasNovelty = true;
            NLM_LOG_INFO("  ✓ Novelty neuromodulation present");
        }
        
        if (brain->getPredictionErrorSystem()) {
            hasPredictionError = true;
            NLM_LOG_INFO("  ✓ Prediction error system present");
        }
        
        if (hasDopamine && hasCuriosity && hasNovelty && hasPredictionError) {
            NLM_LOG_INFO("  ✓ All neuromodulation systems integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Some neuromodulation systems missing or not integrated");
            return false;
        }
    }
    
    // Test 6: Development system integration (phase-agnostic)
    bool testDevelopmentIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Development System Integration ===");
        
        bool hasDevelopmentSystem = false;
        bool hasPlasticityRules = false;
        bool hasStructuralPlasticity = false;
        
        // Check development system
        if (brain->getDevelopmentSystem()) {
            hasDevelopmentSystem = true;
            NLM_LOG_INFO("  ✓ Development system present");
        }
        
        // Check plasticity rules
        if (brain->getPlasticitySystem()) {
            hasPlasticityRules = true;
            NLM_LOG_INFO("  ✓ Plasticity rules present");
        }
        
        // Check structural plasticity
        if (brain->getStructuralPlasticity()) {
            hasStructuralPlasticity = true;
            NLM_LOG_INFO("  ✓ Structural plasticity present");
        }
        
        if (hasDevelopmentSystem && hasPlasticityRules && hasStructuralPlasticity) {
            NLM_LOG_INFO("  ✓ All development systems integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Some development systems missing or not integrated");
            return false;
        }
    }
    
    // Test 7: Prediction system integration (phase-agnostic)
    bool testPredictionIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Prediction System Integration ===");
        
        bool hasPredictionSystem = false;
        bool hasPredictionError = false;
        bool hasActionSelection = false;
        
        // Check prediction system
        if (brain->getPredictionSystem()) {
            hasPredictionSystem = true;
            NLM_LOG_INFO("  ✓ Prediction system present");
        }
        
        // Check prediction error
        if (brain->getPredictionErrorSystem()) {
            hasPredictionError = true;
            NLM_LOG_INFO("  ✓ Prediction error system present");
        }
        
        // Check action selection
        if (brain->getActionSelectionSystem()) {
            hasActionSelection = true;
            NLM_LOG_INFO("  ✓ Action selection system present");
        }
        
        if (hasPredictionSystem && hasPredictionError && hasActionSelection) {
            NLM_LOG_INFO("  ✓ All prediction systems integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Some prediction systems missing or not integrated");
            return false;
        }
    }
    
    // Test 8: Cognitive system integration (phase-agnostic)
    bool testCognitiveIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Cognitive System Integration ===");
        
        bool hasAttention = false;
        bool hasPlanning = false;
        bool hasConceptFormation = false;
        bool hasSelfModel = false;
        
        // Check attention system
        if (brain->getAttentionSystem()) {
            hasAttention = true;
            NLM_LOG_INFO("  ✓ Attention system present");
        }
        
        // Check planning system
        if (brain->getPlanningSystem()) {
            hasPlanning = true;
            NLM_LOG_INFO("  ✓ Planning system present");
        }
        
        // Check concept formation
        if (brain->getConceptFormationSystem()) {
            hasConceptFormation = true;
            NLM_LOG_INFO("  ✓ Concept formation system present");
        }
        
        // Check self model
        if (brain->getSelfModelSystem()) {
            hasSelfModel = true;
            NLM_LOG_INFO("  ✓ Self model system present");
        }
        
        if (hasAttention && hasPlanning && hasConceptFormation && hasSelfModel) {
            NLM_LOG_INFO("  ✓ All cognitive systems integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Some cognitive systems missing or not integrated");
            return false;
        }
    }
    
    // Test 9: Checkpoint system integration (phase-agnostic)
    bool testCheckpointIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Checkpoint System Integration ===");
        
        bool hasCheckpointSystem = false;
        
        // Check checkpoint system
        if (brain->getCheckpointSystem()) {
            hasCheckpointSystem = true;
            NLM_LOG_INFO("  ✓ Checkpoint system present");
        }
        
        if (hasCheckpointSystem) {
            NLM_LOG_INFO("  ✓ Checkpoint system integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Checkpoint system missing or not integrated");
            return false;
        }
    }
    
    // Test 10: Replay system integration (phase-agnostic)
    bool testReplayIntegration(std::shared_ptr<Brain> brain, const Config& config) {
        NLM_LOG_INFO("=== Generic Test: Replay System Integration ===");
        
        bool hasReplaySystem = false;
        
        // Check replay system
        if (brain->getReplaySystem()) {
            hasReplaySystem = true;
            NLM_LOG_INFO("  ✓ Replay system present");
        }
        
        if (hasReplaySystem) {
            NLM_LOG_INFO("  ✓ Replay system integrated with brain");
            return true;
        } else {
            NLM_LOG_INFO("  ! Replay system missing or not integrated");
            return false;
        }
    }
}

// Generic banner that can be configured
void printGenericBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Generic Neural Computation Framework                     ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This framework implements real spiking neural computation.  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}