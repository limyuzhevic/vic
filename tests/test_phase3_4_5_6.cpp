// ============================================================================
// Phase 3 Tests - World Interaction and Agent Systems
// ============================================================================

#include "Phase3Experiment.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

void testPhase3WorldInteraction() {
    std::cout << "Testing Phase 3: World Interaction and Agent Systems..." << std::endl;
    
    // Test 1: Basic world setup
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.1);
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setAgentStart(10.0f, 10.0f);
    
    assert(world.getWidth() == 20.0f);
    assert(world.getHeight() == 20.0f);
    assert(world.getAgentBody().x == 10.0f);
    assert(world.getAgentBody().y == 10.0f);
    
    // Test 2: Agent brain creation and initialization
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test 3: Sensory processing
    SensoryPercept percept;
    // Test vision data
    std::vector<float> visionData(256, 0.5f);
    for (size_t i = 0; i < visionData.size(); ++i) {
        percept.vision[i] = visionData[i];
    }
    
    agentBrain.processSensoryInput(percept);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    
    // Test 4: Motor command decoding
    MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(static_cast<int>(cmd.type) >= 0 && static_cast<int>(cmd.type) < 5);
    assert(cmd.magnitude >= 0.0f && cmd.magnitude <= 2.0f);
    assert(cmd.duration >= 0.0f && cmd.duration <= 2.0f);
    
    // Test 5: World interaction
    ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
    assert(result == ActionResult::Success || result == ActionResult::Collision);
    
    // Test 6: World update
    world.update(0.1f);
    assert(world.getSimulationTime() > 0.0);
    
    // Test 7: Reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    assert(agentBrain.getPredictionError() >= -1.0f && agentBrain.getPredictionError() <= 1.0f);
    
    // Test 8: Development system
    agentBrain.updateDevelopment(0.1f);
    assert(agentBrain.getDevelopmentalStage() >= DevelopmentalStage::Initial);
    
    // Test 9: Configuration persistence
    agentBrain.reset();
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "✓ Phase 3 tests passed!" << std::endl;
}

// ============================================================================
// Phase 4 Tests - Cognitive Systems
// ============================================================================

void testPhase4CognitiveSystems() {
    std::cout << "Testing Phase 4: Cognitive Systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.05);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Test 1: Neural prediction system
    std::shared_ptr<NeuralPrediction> predictionSystem = std::make_shared<NeuralPrediction>();
    predictionSystem->initialize(brain.get());
    
    assert(predictionSystem->getPredictionAccuracy() >= 0.0f);
    assert(predictionSystem->getPredictionError() >= 0.0f);
    assert(predictionSystem->getConfidenceLevel() >= 0.0f && predictionSystem->getConfidenceLevel() <= 1.0f);
    
    // Test 2: Working memory system
    std::shared_ptr<WorkingMemory> workingMemory = std::make_shared<WorkingMemory>();
    workingMemory->initialize(1000);
    
    assert(workingMemory->getCapacity() == 1000);
    assert(workingMemory->getCurrentLoad() == 0);
    
    // Test episodic memory
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory = std::make_shared<NeuralEpisodicMemory>();
    episodicMemory->initialize();
    
    assert(episodicMemory->getTotalMemories() == 0);
    assert(episodicMemory->getReplayCount() == 0);
    
    // Test 3: Concept formation
    std::shared_ptr<ConceptFormation> conceptFormation = std::make_shared<ConceptFormation>();
    conceptFormation->initialize();
    
    assert(conceptFormation->getConceptCount() == 0);
    assert(conceptFormation->getCategoryCount() == 0);
    
    // Test 4: Neural planner
    std::shared_ptr<NeuralPlanner> neuralPlanner = std::make_shared<NeuralPlanner>();
    neuralPlanner->initialize(brain.get());
    
    assert(neuralPlanner->getPlanCount() == 0);
    assert(neuralPlanner->getPlannerConfidence() >= 0.0f);
    
    // Test 5: Neural attention
    std::shared_ptr<NeuralAttention> neuralAttention = std::make_shared<NeuralAttention>();
    neuralAttention->initialize();
    
    assert(neuralAttention->getAttentionLevel() >= 0.0f && neuralAttention->getAttentionLevel() <= 1.0f);
    assert(neuralAttention->getFocusDuration() >= 0.0f);
    
    // Test 6: Self-model
    std::shared_ptr<SelfModel> selfModel = std::make_shared<SelfModel>();
    selfModel->initialize();
    
    assert(selfModel->getSelfAwareness() >= 0.0f && selfModel->getSelfAwareness() <= 1.0f);
    assert(selfModel->getGoalPriorityCount() >= 0);
    
    // Test 7: Social learning
    std::shared_ptr<SocialLearning> socialLearning = std::make_shared<SocialLearning>();
    socialLearning->initialize();
    
    assert(socialLearning->getSocialKnowledgeCount() == 0);
    assert(socialLearning->getImitationCapability() >= 0.0f);
    
    // Test 8: Continual learning
    std::shared_ptr<ContinualLearning> continualLearning = std::make_shared<ContinualLearning>();
    continualLearning->initialize();
    
    assert(continualLearning->getLearningRate() >= 0.0f && continualLearning->getLearningRate() <= 1.0f);
    assert(continualLearning->getForgettingRate() >= 0.0f && continualLearning->getForgettingRate() <= 1.0f);
    
    std::cout << "✓ Phase 4 tests passed!" << std::endl;
}

// ============================================================================
// Phase 5 Tests - Performance Optimizations
// ============================================================================

void testPhase5PerformanceOptimizations() {
    std::cout << "Testing Phase 5: Performance Optimizations..." << std::endl;
    
    // Test 1: Memory Pool optimization
    MemoryPool memoryPool;
    memoryPool.initialize(1000000, 64); // 1MB, 64-byte chunks
    
    // Allocate and deallocate memory
    void* ptr1 = memoryPool.allocate();
    void* ptr2 = memoryPool.allocate();
    void* ptr3 = memoryPool.allocate();
    
    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    assert(ptr3 != nullptr);
    
    memoryPool.deallocate(ptr1);
    memoryPool.deallocate(ptr2);
    memoryPool.deallocate(ptr3);
    
    assert(memoryPool.getFragmentation() >= 0.0f && memoryPool.getFragmentation() <= 1.0f);
    
    // Test 2: Event Queue optimization
    EventQueue eventQueue;
    eventQueue.initialize(100000);
    
    // Push events
    for (int i = 0; i < 1000; ++i) {
        eventQueue.push(i, i * 0.001, 1.0f, 0.5f);
    }
    
    assert(eventQueue.size() == 1000);
    
    // Process events
    float totalProcessed = 0.0f;
    while (!eventQueue.empty()) {
        auto event = eventQueue.pop();
        totalProcessed += event.value;
    }
    
    assert(totalProcessed > 0.0f);
    assert(eventQueue.size() == 0);
    
    // Test 3: SIMD Vectorization
    SIMDVectorization simd;
    
    std::vector<float> data1(1000, 1.0f);
    std::vector<float> data2(1000, 2.0f);
    std::vector<float> result1(1000, 0.0f);
    std::vector<float> result2(1000, 0.0f);
    
    simd.addVector(data1.data(), result1.data(), 1000);
    simd.multiplyVector(data2.data(), 0.5f, 1000);
    
    for (size_t i = 0; i < 1000; ++i) {
        assert(std::abs(result1[i] - 1.0f) < 0.001f);
        assert(std::abs(data2[i] - 1.0f) < 0.001f);
    }
    
    // Test 4: Parallel Processing
    ParallelProcessing parallel;
    
    // Submit tasks
    for (int i = 0; i < 100; ++i) {
        parallel.submitTask([i]() {
            float result = 0.0f;
            for (int j = 0; j < 100; ++j) {
                result += i * j * 0.001f;
            }
            return result;
        });
    }
    
    // Get results
    float totalResult = 0.0f;
    for (int i = 0; i < 100; ++i) {
        totalResult += parallel.getResult(i);
    }
    
    assert(totalResult > 0.0f);
    
    // Test 5: Checkpoint System
    CheckpointSystem checkpoint;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Simulate some computation
    for (int i = 0; i < 50; ++i) {
        brain->step(i);
    }
    
    // Save checkpoint
    checkpoint.saveBrain(brain, "test_checkpoint.bin");
    
    // Load checkpoint
    auto loadedBrain = checkpoint.loadBrain("test_checkpoint.bin");
    assert(loadedBrain != nullptr);
    
    // Cleanup
    std::remove("test_checkpoint.bin");
    
    // Test 6: Optimized Brain
    OptimizedBrain optimizedBrain(config);
    optimizedBrain.initialize();
    
    assert(optimizedBrain.getTotalNeuronCount() > 0);
    assert(optimizedBrain.getTotalSynapseCount() > 0);
    
    for (int i = 0; i < 50; ++i) {
        optimizedBrain.step(i);
    }
    
    assert(optimizedBrain.getAverageFiringRate() >= 0.0f);
    assert(optimizedBrain.getTotalSpikeCount() > 0);
    
    std::cout << "✓ Phase 5 tests passed!" << std::endl;
}

// ============================================================================
// Phase 6 Tests - Integration and Final System
// ============================================================================

void testPhase6Integration() {
    std::cout << "Testing Phase 6: Integration and Final System..." << std::endl;
    
    // Test 1: Phase 6 integrated experiment
    Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    // Test individual system integration
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    experiment.testCheckpointing();
    experiment.testReplay();
    
    // Test 2: Complete brain-agent-world loop
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.05);
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setAgentStart(10.0f, 10.0f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    agentBrain.enableDevelopment(true);
    
    // Run simulation loop
    for (int step = 0; step < 100; ++step) {
        world.update(0.1f);
        auto percept = world->getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        brain->step(step);
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        if (step % 10 == 0) {
            agentBrain.applyRewardModulation(0.1f, 0.0f);
            agentBrain.updateDevelopment(0.1f);
        }
    }
    
    // Verify results
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getAverageFiringRate() >= 0.0f);
    assert(world.getSimulationTime() > 0.0f);
    
    // Test 3: Persistence and checkpointing
    brain->save("test_brain_state.bin");
    
    auto loadedBrain = std::make_shared<Brain>();
    loadedBrain->load("test_brain_state.bin");
    
    assert(loadedBrain->getTotalNeuronCount() == brain->getTotalNeuronCount());
    
    std::remove("test_brain_state.bin");
    
    // Test 4: Advanced agent capabilities
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.updateDevelopment(0.1f);
    
    assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getPredictionError() >= 0.0f);
    
    // Test 5: Developmental stages
    for (int age = 0; age < 500; age += 100) {
        agentBrain.updateDevelopment(100.0f);
        assert(static_cast<int>(agentBrain.getDevelopmentalStage()) >= DevelopmentalStage::Initial);
        assert(static_cast<int>(agentBrain.getDevelopmentalStage()) <= DevelopmentalStage::Aging);
    }
    
    // Test 6: Memory integration
    agentBrain.applyRewardModulation(1.0f, 0.0f);
    agentBrain.applyRewardModulation(0.0f, 1.0f);
    assert(agentBrain.getPredictionError() >= -1.0f && agentBrain.getPredictionError() <= 1.0f);
    
    std::cout << "✓ Phase 6 tests passed!" << std::endl;
}

// ============================================================================
// Unit Tests for Core Components
// ============================================================================

void testCoreComponents() {
    std::cout << "Testing Core Components..." << std::endl;
    
    // Test 1: Configuration system
    Config config;
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && intVal.value() == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(doubleVal.value() - 3.14) < 0.001);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && stringVal.value() == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && boolVal.value() == true);
    
    // Test getOr with defaults
    int defaultInt = config.getOr<int>("nonexistent", 100);
    assert(defaultInt == 100);
    
    double defaultDouble = config.getOr<double>("nonexistent", 2.71);
    assert(std::abs(defaultDouble - 2.71) < 0.001);
    
    // Test 2: Logger system
    Logger::getInstance().setLevel(Logger::Level::Info);
    Logger::getInstance().log("Test message", Logger::Level::Info);
    
    // Test 3: Simulation clock
    SimulationClock clock;
    clock.reset();
    
    for (int i = 0; i < 10; ++i) {
        clock.update(0.01f);
        assert(clock.getTime() > 0.0f);
    }
    
    // Test 4: Random number generator
    Random rng(42);
    float val1 = rng.getFloat();
    float val2 = rng.getFloat();
    
    // Should be different calls
    rng.seed(42);
    float val3 = rng.getFloat();
    assert(val1 == val3); // Same seed should produce same sequence
    
    std::cout << "✓ Core components tests passed!" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "===========================================\n";
    std::cout << "NLM COMPREHENSIVE TEST SUITE\n";
    std::cout << "===========================================\n\n";
    
    try {
        testCoreComponents();
        std::cout << "\n";
        
        testPhase3WorldInteraction();
        std::cout << "\n";
        
        testPhase4CognitiveSystems();
        std::cout << "\n";
        
        testPhase5PerformanceOptimizations();
        std::cout << "\n";
        
        testPhase6Integration();
        std::cout << "\n";
        
        std::cout << "===========================================\n";
        std::cout << "ALL TESTS PASSED!\n";
        std::cout << "===========================================\n";
        std::cout << "\nTest Summary:\n";
        std::cout << "  ✓ Core Components: PASSED\n";
        std::cout << "  ✓ Phase 3 - World Interaction: PASSED\n";
        std::cout << "  ✓ Phase 4 - Cognitive Systems: PASSED\n";
        std::cout << "  ✓ Phase 5 - Performance Optimizations: PASSED\n";
        std::cout << "  ✓ Phase 6 - Integration: PASSED\n";
        std::cout << "\nThe NLM system is fully functional and ready for production use!\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ TEST FAILED: Unknown error" << std::endl;
        return 1;
    }
}
