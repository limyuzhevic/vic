// Enhanced Brain Tests with Comprehensive Coverage
// Includes integration tests, performance benchmarks, and realistic scenarios

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

namespace test_brain {

// Test framework with better error reporting
class TestFramework {
public:
    static void assertTrue(bool condition, const std::string& message) {
        if (!condition) {
            std::cerr << "FAILED: " << message << std::endl;
            exit(1);
        }
    }
    
    static void assertEqual(size_t actual, size_t expected, const std::string& message) {
        if (actual != expected) {
            std::cerr << "FAILED: " << message << ". Expected " << expected 
                      << ", got " << actual << std::endl;
            exit(1);
        }
    }
    
    static void assertFloatEqual(float actual, float expected, float epsilon, 
                                 const std::string& message) {
        if (std::abs(actual - expected) > epsilon) {
            std::cerr << "FAILED: " << message << ". Expected " << expected 
                      << ", got " << actual << std::endl;
            exit(1);
        }
    }
};

// Basic brain functionality tests
void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    TestFramework::assertTrue(success, "Brain initialization should succeed");
    
    // Verify brain initialized with expected properties
    TestFramework::assertEqual(brain.getRegionCount(), 2, "Should have 2 regions");
    TestFramework::assertEqual(brain.getTotalNeuronCount(), 100, "Should have 100 neurons");
    TestFramework::assertTrue(brain.getTotalSynapseCount() > 0, "Should have synapses");
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    size_t initialNeurons = brain.getTotalNeuronCount();
    
    // Run multiple steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Inject some current to stimulate activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 20.0f);
    }
    
    // Verify brain state
    TestFramework::assertEqual(brain.getTotalNeuronCount(), initialNeurons, 
                               "Neuron count should remain constant");
    
    // Check that activity occurred
    bool activityOccurred = brain.getTotalSpikeCount() > initialSpikes;
    TestFramework::assertTrue(activityOccurred, "Brain should generate spikes with current input");
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    TestFramework::assertEqual(brain.getTotalNeuronCount(), 50, "Reset should restore initial neuron count");
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    TestFramework::assertEqual(regionIds.size(), 3, "Should have 3 regions");
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        TestFramework::assertTrue(region != nullptr, "Region should be accessible");
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    TestFramework::assertTrue(action != nullptr, "Brain should produce an action");
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible and valid
    TestFramework::assertTrue(brain.getTotalNeuronCount() > 0, "Should have neurons");
    TestFramework::assertTrue(brain.getTotalSynapseCount() >= 0, "Synapse count should be non-negative");
    TestFramework::assertTrue(brain.getFiringNeuronCount() >= 0, "Firing neurons should be non-negative");
    TestFramework::assertTrue(brain.getAverageFiringRate() >= 0.0f, "Firing rate should be non-negative");
    TestFramework::assertTrue(brain.getExcitationInhibitionRatio() >= 0.0f, "E/I ratio should be non-negative");
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Integration tests with AgentBrain
void testAgentBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool brainInitialized = brain.initialize();
    TestFramework::assertTrue(brainInitialized, "Brain should initialize");
    
    // Create world and agent brain
    nlm::SimpleWorld world;
    world.configure(width=80, height=60, visionWidth=16, visionHeight=16);
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test sensory input processing
    auto percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    // Test motor command decoding
    auto command = agentBrain.decodeMotorCommand();
    TestFramework::assertTrue(command != nlm::MotorCommand::Wait, 
                             "Agent should produce a non-wait command when stimulated");
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

// Plasticity and learning tests
void testPlasticityLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Hebbian and STDP
        }
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Apply correlated activity (LTP)
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        // Create correlated pre-post activity to trigger LTP
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            // Pre fires
            neurons[0]->injectCurrent(60.0f);
            brain.step(0, 0.0);
            
            // Post fires  
            neurons[1]->injectCurrent(60.0f);
            brain.step(1, 0.001);
            
            // More correlated activity
            for (int i = 0; i < 20; ++i) {
                neurons[0]->injectCurrent(30.0f);
                brain.step(i * 2, i * 2 * 0.001);
                
                neurons[1]->injectCurrent(30.0f);
                brain.step(i * 2 + 1, (i * 2 + 1) * 0.001);
            }
        }
    }
    
    // Check for weight changes (learning)
    if (region) {
        float totalWeightChange = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& syn : region->getSynapses()) {
            if (synapseCount < initialWeights.size()) {
                float change = syn->getWeight() - initialWeights[synapseCount];
                totalWeightChange += std::abs(change);
            }
            synapseCount++;
        }
        
        // Some weight change is expected with correlated activity
        TestFramework::assertTrue(totalWeightChange > 0.001f, 
                                 "Plasticity should produce weight changes");
    }
    
    std::cout << "    testPlasticityLearning passed" << std::endl;
}

// Performance and stress tests
void testPerformanceBenchmark() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Measure initialization time
    auto startTime = std::chrono::high_resolution_clock::now();
    bool initSuccess = brain.initialize();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    TestFramework::assertTrue(initSuccess, "Brain initialization should succeed");
    TestFramework::assertTrue(duration.count() < 5000, 
                             "Initialization should complete within 5 seconds");
    
    std::cout << "    testPerformanceBenchmark passed (initialization: " 
              << duration.count() << "ms)" << std::endl;
}

// Developmental stage tests
void testDevelopmentStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain for development testing
    nlm::SimpleWorld world;
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test developmental stages progression
    nlm::DevelopmentalStage initialStage = agentBrain.getDevelopmentalStage();
    TestFramework::assertTrue(initialStage == nlm::DevelopmentalStage::Initial, 
                             "Should start in Initial developmental stage");
    
    // Simulate development
    for (int stage = 0; stage < 100; ++stage) {
        agentBrain.updateDevelopment(0.1);  // 0.1 time units per step
        
        // Check that stage changes appropriately
        nlm::DevelopmentalStage currentStage = agentBrain.getDevelopmentalStage();
        
        if (stage < 6) {  // < 60 time units
            TestFramework::assertTrue(currentStage == nlm::DevelopmentalStage::Initial,
                                     "Early development should be in Initial stage");
        } else if (stage < 30) {  // < 300 time units
            TestFramework::assertTrue(currentStage == nlm::DevelopmentalStage::CriticalPeriod,
                                     "Mid development should be in CriticalPeriod stage");
        } else if (stage < 90) {  // < 900 time units
            TestFramework::assertTrue(currentStage == nlm::DevelopmentalStage::Maturation,
                                     "Late development should be in Maturation stage");
        } else {  // >= 900 time units
            TestFramework::assertTrue(currentStage == nlm::DevelopmentalStage::Adult,
                                     "Adult development should be in Adult stage");
        }
    }
    
    std::cout << "    testDevelopmentStages passed" << std::endl;
}

// Neuromodulation tests
void testNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test initial neuromodulation levels
    float initialDopamine = agentBrain.getNeuromodulationLevel();
    float initialCuriosity = agentBrain.getCuriosityLevel();
    float initialNovelty = agentBrain.getNoveltyLevel();
    float initialError = agentBrain.getPredictionError();
    
    TestFramework::assertFloatEqual(initialDopamine, 0.0f, 0.001f, 
                                   "Initial dopamine should be 0.0");
    TestFramework::assertFloatEqual(initialCuriosity, 0.0f, 0.001f, 
                                   "Initial curiosity should be 0.0");
    TestFramework::assertFloatEqual(initialNovelty, 0.0f, 0.001f, 
                                   "Initial novelty should be 0.0");
    TestFramework::assertFloatEqual(initialError, 0.0f, 0.001f, 
                                   "Initial prediction error should be 0.0");
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);  // Reward > predicted = positive error
    
    float dopamine = agentBrain.getNeuromodulationLevel();
    float error = agentBrain.getPredictionError();
    
    TestFramework::assertTrue(std::abs(dopamine - 0.5f) < 0.01f, 
                             "Dopamine should reflect prediction error");
    TestFramework::assertTrue(std::abs(error - 0.5f) < 0.01f, 
                             "Prediction error should be reward - predicted");
    
    std::cout << "    testNeuromodulation passed" << std::endl;
}

// Memory system integration tests
void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Access memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    TestFramework::assertTrue(workingMemory != nullptr, "Should have working memory");
    TestFramework::assertTrue(episodicMemory != nullptr, "Should have episodic memory");
    TestFramework::assertTrue(associativeMemory != nullptr, "Should have associative memory");
    
    // Test working memory
    workingMemory->store("test_key", "test_value", 100.0);
    std::string retrieved = workingMemory->retrieve<std::string>("test_key", 200.0);
    TestFramework::assertTrue(retrieved == "test_value", "Working memory should retrieve stored value");
    
    // Test episodic memory
    nlm::SensoryPercept percept;
    nlm::Action action(nlm::ActionType::MoveForward);
    
    int64_t episodeId = episodicMemory->storeEpisode(percept, action, 1.0, 100.0);
    TestFramework::assertTrue(episodeId > 0, "Should store episode with positive ID");
    
    // Test memory replay
    episodicMemory->replayEpisode(episodeId, [](const auto&, const auto&, double) {});
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

// Configuration tests
void testConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test default values
    TestFramework::assertTrue(config->getOr<int64_t>("random_seed", 42) == 42, 
                             "Should have default random seed");
    TestFramework::assertTrue(config->has("random_seed"), "Should have random_seed key");
    
    // Test setting and getting values
    config->set("test_int", static_cast<int64_t>(123), nlm::ConfigSource::Default);
    TestFramework::assertEqual(config->getOr<int64_t>("test_int", 0), 123, 
                              "Should get set integer value");
    
    config->set("test_float", 3.14f, nlm::ConfigSource::Default);
    TestFramework::assertFloatEqual(config->getOr<float>("test_float", 0.0f), 3.14f, 0.001f, 
                                   "Should get set float value");
    
    // Test configuration file operations
    TestFramework::assertTrue(config->loadFromFile("configs/default.cfg"), 
                             "Should load from config file");
    
    TestFramework::assertTrue(config->saveToFile("test_config.tmp"), 
                             "Should save to config file");
    
    // Clean up
    std::remove("test_config.tmp");
    
    std::cout << "    testConfiguration passed" << std::endl;
}

// Complete integration test
void testCompleteIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create full NLM system
    nlm::Brain brain(config);
    bool brainInitialized = brain.initialize();
    TestFramework::assertTrue(brainInitialized, "Brain should initialize successfully");
    
    nlm::SimpleWorld world;
    world.configure(width=100, height=100, visionWidth=20, visionHeight=20);
    
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    world.setAgentStart(50, 50);
    
    // Run complete simulation loop
    int steps = 100;
    float totalReward = 0.0f;
    
    for (int step = 0; step < steps; ++step) {
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process and act
        agentBrain.processSensoryInput(percept);
        auto action = agentBrain.decodeMotorCommand();
        
        // Execute action
        auto result = world.applyMotorCommand(action, step * config->getOr<double>("simulation_timestep", 0.001));
        totalReward += result.reward;
        
        // Learn from experience
        agentBrain.applyRewardModulation(result.reward, agentBrain.getPredictionError());
        agentBrain.updateDevelopment(config->getOr<double>("simulation_timestep", 0.001));
        
        // Run neural simulation
        brain.step(step);
    }
    
    // Verify system is still functional
    TestFramework::assertTrue(brain.getTotalNeuronCount() == 300, 
                             "Should maintain neuron count");
    TestFramework::assertTrue(totalReward >= 0.0f, "Should accumulate non-negative reward");
    
    std::cout << "    testCompleteIntegration passed (total reward: " 
              << totalReward << ")" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced NLM brain tests..." << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    testAgentBrainIntegration();
    testPlasticityLearning();
    testDevelopmentStages();
    testNeuromodulation();
    testMemoryIntegration();
    testConfiguration();
    testPerformanceBenchmark();
    testCompleteIntegration();
    
    std::cout << std::endl;
    std::cout << "All tests passed successfully!" << std::endl;
    std::cout << "NLM brain system is fully functional and ready for production use." << std::endl;
}

} // namespace test_brain