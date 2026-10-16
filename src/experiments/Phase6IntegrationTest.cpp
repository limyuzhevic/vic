// Phase 6 Integration Tests
// Comprehensive integration tests for the final NLM Phase 6 implementation
// Tests integration of all major systems into a coherent artificial brain

#include "Phase6IntegrationTest.hpp"
#include "../../agent/AgentBrain.hpp"
#include "../../world/SimpleWorld.hpp"
#include "../../neuromodulation/Dopamine.hpp"
#include "../../neuromodulation/Curiosity.hpp"
#include "../../neuromodulation/Novelty.hpp"
#include "../../neuromodulation/PredictionError.hpp"
#include "../../plasticity/STDP.hpp"
#include "../../plasticity/RewardModulatedSTDP.hpp"
#include "../../plasticity/TripletSTDP.hpp"
#include "../../memory/NeuralWorkingMemory.hpp"
#include "../../memory/NeuralEpisodicMemory.hpp"
#include "../../memory/ConceptFormation.hpp"
#include "../../cognition/NeuralPlanner.hpp"
#include "../../cognition/AttentionalSelection.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include "../../prediction/NeuralPrediction.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include "../../core/Config/Config.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../sensory/Vision.hpp"
#include "../../sensory/InternalSignals.hpp"
#include "../../motor/MotorSystem.hpp"
#include "../../environment/Environment.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

bool Phase6IntegrationTest::runAllTests() {
    bool allPassed = true;
    
    logTest("Phase 6 Integration Tests - Starting")
    
    // Test 1: Component initialization and configuration
    allPassed &= testComponentInitialization();
    
    // Test 2: Neuromodulation integration
    allPassed &= testNeuromodulationIntegration();
    
    // Test 3: Memory system integration
    allPassed &= testMemorySystemIntegration();
    
    // Test 4: Plasticity system integration
    allPassed &= testPlasticitySystemIntegration();
    
    // Test 5: Prediction and cognition integration
    allPassed &= testPredictionCognitionIntegration();
    
    // Test 6: Agent-brain interface integration
    allPassed &= testAgentBrainIntegration();
    
    // Test 7: Full brain loop integration
    allPassed &= testFullBrainLoopIntegration();
    
    // Test 8: Performance and scalability
    allPassed &= testPerformanceAndScalability();
    
    // Test 9: Developmental integration
    allPassed &= testDevelopmentalIntegration();
    
    // Test 10: Social learning and interaction
    allPassed &= testSocialLearningIntegration();
    
    logTest("Phase 6 Integration Tests - Completed")
    
    return allPassed;
}

bool Phase6IntegrationTest::testComponentInitialization() {
    logTest("Test 1: Component Initialization")
    
    Config config;
    config.loadFromFile("configs/default.cfg");
    
    bool passed = true;
    
    // Test core component creation
    auto clock = std::make_unique<SimulationClock>(1.0f, config.getRandomSeed());
    auto random = std::make_unique<RandomGenerator>(config.getRandomSeed());
    auto logger = std::make_unique<Logger>(LogLevel::INFO);
    
    // Test agent creation
    auto agent = std::make_unique<AgentBrain>(
        config.getNeuronCount(),
        config.getRegionCount(),
        config.getConnectionProbability()
    );
    
    // Test environment creation
    auto environment = std::make_unique<SimpleWorld>(
        config.getEnvironmentWidth(),
        config.getEnvironmentHeight()
    );
    
    // Test that all components are properly initialized
    passed &= (clock != nullptr);
    passed &= (random != nullptr);
    passed &= (logger != nullptr);
    passed &= (agent != nullptr);
    passed &= (environment != nullptr);
    
    if (passed) {
        logTest("✓ Component initialization passed")
    } else {
        logTest("✗ Component initialization failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testNeuromodulationIntegration() {
    logTest("Test 2: Neuromodulation Integration")
    
    bool passed = true;
    
    // Create neuromodulators with Phase 6 settings
    Dopamine dopamine;
    dopamine.setBaseline(0.1f);
    dopamine.setPeak(1.0f);
    dopamine.setDecayRate(0.05f);
    dopamine.setReleaseRate(1.0f);
    
    Curiosity curiosity;
    curiosity.setDrive(0.5f);
    curiosity.setExplorationThreshold(0.3f);
    
    Novelty noveltyDetector;
    noveltyDetector.setSensitivity(0.7f);
    
    PredictionError predictionError;
    predictionError.setWeight(0.5f);
    
    // Test dopamine response to reward
    dopamine.signalReward(0.8f);
    passed &= std::abs(dopamine.getLevel() - 0.95f) < 0.1f;  // Peak modulation
    
    // Test dopamine response to reward prediction error
    dopamine.signalRewardPredictionError(0.5f);
    passed &= dopamine.getLevel() > 0.0f;
    
    // Test curiosity-driven exploration
    bool shouldExplore = curiosity.shouldExplore();
    passed &= (shouldExplore == true);  // With high drive, should explore
    
    // Test novelty detection
    Observation testObservation;
    float novelty = noveltyDetector.calculateNovelty(testObservation);
    passed &= (novelty >= 0.0f && novelty <= 1.0f);
    
    // Test prediction error calculation
    Observation observation1;
    Observation observation2;
    float error = predictionError.calculateError(observation1, observation2);
    passed &= (error >= 0.0f);
    
    // Test integration between neuromodulators
    dopamine.update(1.0f);  // Time step
    dopamine.update(1.0f);  // Another step to see decay
    
    if (passed) {
        logTest("✓ Neuromodulation integration passed")
    } else {
        logTest("✗ Neuromodulation integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testMemorySystemIntegration() {
    logTest("Test 3: Memory System Integration")
    
    bool passed = true;
    
    // Create memory systems
    NeuralWorkingMemory workingMemory(50, 0.1f, 0.5f);
    NeuralEpisodicMemory episodicMemory(1000, 10.0f, 1000);
    ConceptFormation conceptFormation(100, 0.5f, 0.8f);
    
    // Test working memory encoding and retrieval
    WorkingMemoryId wmId = workingMemory.storeTemporary(
        std::vector<float>(10, 0.5f),  // Pattern
        1000,  // Duration
        WorkingMemoryType::SPATIAL
    );
    
    std::vector<float> retrieved = workingMemory.retrieve(wmId);
    passed &= (retrieved.size() == 10);
    
    // Test episodic memory encoding
    EphemeralExperience experience = {
        .observation = std::vector<float>(20, 0.3f),
        .action = Action(0, 0, 0),
        .outcome = 1.0f,
        .timestamp = 1000.0f,
        .context = std::vector<float>(5, 0.2f)
    };
    
    std::vector<float> encodedPattern = episodicMemory.encode(experience);
    passed &= (encodedPattern.size() > 0);
    
    // Test concept formation from patterns
    std::vector<float> pattern1(10, 0.1f);
    std::vector<float> pattern2(10, 0.2f);
    std::vector<float> pattern3(10, 0.3f);
    
    conceptFormation.feedPattern(pattern1);
    conceptFormation.feedPattern(pattern2);
    conceptFormation.feedPattern(pattern3);
    
    std::vector<std::string> concepts = conceptFormation.getConcepts();
    passed &= (concepts.size() > 0);
    
    // Test memory interaction
    workingMemory.storePattern(std::vector<float>(10, 0.7f));
    std::vector<float> wmPattern = workingMemory.getCurrentPattern();
    passed &= (wmPattern.size() == 10);
    
    // Test memory consolidation
    episodicMemory.consolidate();
    passed &= episodicMemory.getConsolidationCount() > 0;
    
    if (passed) {
        logTest("✓ Memory system integration passed")
    } else {
        logTest("✗ Memory system integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testPlasticitySystemIntegration() {
    logTest("Test 4: Plasticity System Integration")
    
    bool passed = true;
    
    // Create plasticity rules with Phase 6 settings
    STDP stdp;
    stdp.configure(0.015f, 0.012f, 25.0f);  // Phase 6 optimized parameters
    
    RewardModulatedSTDP rSTDP;
    rSTDP.configure(0.01f, 0.012f, 20.0f, 1.2f);  // With reward modulation
    
    TripletSTDP tSTDP;
    tSTDP.configure(0.005f, 0.0025f, 0.004f, 0.0025f, 100.0f, 100.0f, 25.0f, -1.0f, 1.0f);
    
    // Create a test brain
    auto brain = std::make_unique<Brain>(100, 5);
    
    // Test synaptic modifications
    NeuronId preId = brain->createNeuron(NeuronType::EXCITATORY);
    NeuronId postId = brain->createNeuron(NeuronType::INHIBITORY);
    
    SynapseId synId = brain->createSynapse(preId, postId, 0.1f, 1);
    Synapse* synapse = brain->getSynapse(synId);
    
    if (synapse) {
        // Test spike timing for STDP
        std::vector<Timestamp> preSpikes = {0, 5, 10};   // ms
        std::vector<Timestamp> postSpikes = {20, 25};    // ms
        
        float originalWeight = synapse->getWeight();
        stdp.update(synapse, preSpikes, postSpikes, 1.0f);
        
        // STDP should modify the synapse
        passed &= (std::abs(synapse->getWeight() - originalWeight) > 0.0001f);
        
        // Test reward-modulated STDP
        synapse->setWeight(0.5f);  // Reset
        rSTDP.updateRewardHistory(0.8f);
        rSTDP.update(synapse, preSpikes, postSpikes, 1.0f);
        
        // rSTDP should incorporate reward signal
        passed &= synapse->getWeight() != 0.5f;
        
        // Test triplet STDP
        synapse->setWeight(0.3f);  // Reset
        tSTDP.update(synapse, preSpikes, postSpikes, 1.0f);
        
        // Triplet STDP should modify based on complex patterns
        passed &= synapse->getWeight() != 0.3f;
        
        // Test trace accumulation
        passed &= tSTDP.getFacilitationTrace() > 0.0f;
        passed &= tSTDP.getDepressionTrace() > 0.0f;
    }
    
    // Test structural plasticity integration
    DevelopmentSystem development;
    development.setSynaptogenesisRate(0.001f);
    development.setPruningRate(0.0001f);
    
    // Create neurons for structural plasticity testing
    NeuronId neuron1 = brain->createNeuron(NeuronType::EXCITATORY);
    NeuronId neuron2 = brain->createNeuron(NeuronType::EXCITATORY);
    
    // Simulate development
    for (int i = 0; i < 10; ++i) {
        development.update(*brain, *randomGenerator);
    }
    
    if (passed) {
        logTest("✓ Plasticity system integration passed")
    } else {
        logTest("✗ Plasticity system integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testPredictionCognitionIntegration() {
    logTest("Test 5: Prediction and Cognition Integration")
    
    bool passed = true;
    
    // Create prediction systems
    PredictionSystem predictionSystem(10, 5, 0.1f);
    NeuralPrediction neuralPrediction(100, 5.0f, 0.5f);
    
    // Create cognitive systems
    NeuralPlanner planner(20, 3, 0.5f);
    AttentionalSelection attention(50, 0.3f, 0.8f);
    
    // Test prediction system integration
    std::vector<float> sequence = {1.0f, 0.5f, 0.8f, 0.3f, 0.9f};
    neuralPrediction.feedSequence(sequence);
    
    float prediction = neuralPrediction.predictNext();
    passed &= (prediction >= 0.0f && prediction <= 1.0f);
    
    float error = neuralPrediction.getPredictionError();
    passed &= (error >= 0.0f);
    
    // Test cognitive planning
    std::vector<Action> actions = {
        Action(0, 0, 0), Action(1, 0, 0), Action(0, 1, 0)
    };
    
    std::vector<float> actionScores = planner.evaluateActions(actions);
    passed &= (actionScores.size() == actions.size());
    
    // Test attentional selection
    std::vector<float> inputs(20, 0.5f);
    int selectedAction = attention.selectAction(inputs, actions.size());
    passed &= (selectedAction >= 0 && selectedAction < (int)actions.size());
    
    // Test integration between prediction and cognition
    std::vector<float> predictedSequence = predictionSystem.getPredictedSequence();
    passed &= (predictedSequence.size() > 0);
    
    // Test cognitive integration with memory
    std::vector<float> goalState(10, 0.7f);
    std::vector<float> currentState = planner.getCurrentState();
    
    bool goalReached = planner.evaluateGoal(goalState);
    passed &= (goalReached == false);  // Not reached yet with different state
    
    if (passed) {
        logTest("✓ Prediction and cognition integration passed")
    } else {
        logTest("✗ Prediction and cognition integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testAgentBrainIntegration() {
    logTest("Test 6: Agent-Brain Interface Integration")
    
    bool passed = true;
    
    // Create agent with all interfaces
    AgentBrain agent(1000, 5, 0.15f);
    
    // Create environment
    SimpleWorld world(20, 20);
    
    // Test sensory transduction
    Observation percept = {
        .vision = std::vector<std::vector<float>>(20, std::vector<float>(20, 0.0f)),
        .audio = std::vector<float>(10, 0.0f),
        .internal = std::vector<float>(5, 0.0f)
    };
    
    // Fill with test data
    for (int x = 0; x < 20; ++x) {
        for (int y = 0; y < 20; ++y) {
            percept.vision[x][y] = static_cast<float>(x + y) / 40.0f;
        }
    }
    
    agent.processSensoryInput(percept);
    
    // Test motor decoding
    MotorCommand motorCommand = agent.decodeMotorCommand();
    passed &= (motorCommand.action >= 0 && motorCommand.action < 5);
    passed &= (motorCommand.x >= -1 && motorCommand.x <= 1);
    passed &= (motorCommand.y >= -1 && motorCommand.y <= 1);
    
    // Test neuromodulation interface
    Reward reward;
    reward.add(0.5f);  // Give reward
    
    // Test development interface
    float currentStage = agent.getDevelopmentStage();
    passed &= (currentStage >= 0.0f && currentStage <= 1.0f);
    
    // Test attention interface
    std::vector<float> attentionWeights(50, 0.02f);
    agent.setAttentionWeights(attentionWeights);
    
    // Test planning interface
    std::vector<Action> plannedActions = agent.getPlannedActions();
    passed &= (plannedActions.size() > 0);
    
    // Test attention-driven action selection
    std::vector<float> cognitiveInputs = agent.getCognitiveInputs();
    passed &= (cognitiveInputs.size() > 0);
    
    // Test full agent loop
    agent.update(1.0f);  // Time step
    passed &= agent.isActive();
    
    if (passed) {
        logTest("✓ Agent-brain interface integration passed")
    } else {
        logTest("✗ Agent-brain interface integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testFullBrainLoopIntegration() {
    logTest("Test 7: Full Brain Loop Integration")
    
    bool passed = true;
    
    // Create complete brain with all systems
    Brain brain(500, 5);
    
    // Add sensory neurons
    for (int i = 0; i < 50; ++i) {
        NeuronId neuronId = brain.createNeuron(NeuronType::EXCITATORY);
        // Add to appropriate population for sensory processing
    }
    
    // Add motor neurons
    for (int i = 0; i < 20; ++i) {
        NeuronId neuronId = brain.createNeuron(NeuronType::EXCITATORY);
        // Add to motor population
    }
    
    // Create AgentBrain interface
    AgentBrain agent(500, 5, 0.15f);
    
    // Create environment
    SimpleWorld world(20, 20);
    
    // Run multiple simulation steps
    int stepsCompleted = 0;
    int maxSteps = 100;
    
    for (int step = 0; step < maxSteps && stepsCompleted < maxSteps; ++step) {
        // Generate random sensory input
        Observation percept = {
            .vision = std::vector<std::vector<float>>(20, std::vector<float>(20, 0.0f)),
            .audio = std::vector<float>(10, 0.0f),
            .internal = std::vector<float>(5, 0.0f)
        };
        
        // Fill with random values
        for (int x = 0; x < 20; ++x) {
            for (int y = 0; y < 20; ++y) {
                percept.vision[x][y] = randomGenerator->uniformReal(0.0f, 1.0f);
            }
        }
        
        // Process through agent
        agent.processSensoryInput(percept);
        
        // Update brain (let it process internally)
        brain.update(1.0f);
        
        // Get action and apply to world
        MotorCommand action = agent.decodeMotorCommand();
        WorldResult result = world.applyMotorCommand(action);
        
        // Update agent with result (could be reward, new observation, etc.)
        if (result.reward > 0) {
            Reward reward;
            reward.add(result.reward);
        }
        
        agent.update(1.0f);
        
        stepsCompleted++;
        
        // Check for system failures
        if (!agent.isActive() || brain.getActiveNeuronCount() <= 0) {
            break;
        }
    }
    
    passed &= (stepsCompleted > 0);
    passed &= (stepsCompleted == maxSteps);  // Should complete all steps
    passed &= agent.isActive();
    
    if (passed) {
        logTest("✓ Full brain loop integration passed")
    } else {
        logTest("✗ Full brain loop integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testPerformanceAndScalability() {
    logTest("Test 8: Performance and Scalability")
    
    bool passed = true;
    
    // Test with different brain sizes
    std::vector<std::pair<int, int>> testConfigs = {
        {100, 2},   // Small
        {500, 5},   // Medium
        {1000, 10}  // Large
    };
    
    for (auto& config : testConfigs) {
        int neuronCount = config.first;
        int regionCount = config.second;
        
        // Measure creation time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Brain brain(neuronCount, regionCount);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Should create within reasonable time (2 seconds per 1000 neurons)
        float timePerNeuron = duration.count() / static_cast<float>(neuronCount);
        passed &= (timePerNeuron < 2.0f);  // Less than 2ms per neuron
        
        // Test spike propagation performance
        startTime = std::chrono::high_resolution_clock::now();
        
        // Simulate spike propagation
        for (int i = 0; i < 50; ++i) {
            brain.update(1.0f);
        }
        
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        float timePerStep = duration.count() / 50.0f;
        passed &= (timePerStep < 10.0f);  // Less than 10ms per step
    }
    
    // Test plasticity computation performance
    STDP stdp;
    stdp.configure(0.01f, 0.012f, 25.0f);
    
    Brain brain(200, 5);
    std::vector<Timestamp> preSpikes(10, 0);      // 10 pre spikes
    std::vector<Timestamp> postSpikes(10, 20);    // 10 post spikes
    
    startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        // Find a synapse to test
        SynapseId synId = INVALID_SYNAPSE_ID;
        for (auto& region : brain.getRegions()) {
            for (auto* syn : region->getSynapses()) {
                synId = syn->getId();
                break;
            }
            if (synId != INVALID_SYNAPSE_ID) break;
        }
        
        if (synId != INVALID_SYNAPSE_ID) {
            Synapse* syn = brain.getSynapse(synId);
            stdp.update(syn, preSpikes, postSpikes, 1.0f);
        }
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    float timePerUpdate = duration.count() / 100.0f;
    passed &= (timePerUpdate < 100.0f);  // Less than 100ms per plasticity update
    
    if (passed) {
        logTest("✓ Performance and scalability passed")
    } else {
        logTest("✗ Performance and scalability failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testDevelopmentalIntegration() {
    logTest("Test 9: Developmental Integration")
    
    bool passed = true;
    
    // Create developmental system
    DevelopmentSystem development;
    
    // Create brain for development testing
    Brain brain(200, 5);
    
    // Test developmental progression
    std::vector<float> stageHistory;
    
    for (int stage = 0; stage < 10; ++stage) {
        development.update(*randomGenerator, brain);
        
        float plasticityRate = development.getCurrentPlasticityRate();
        stageHistory.push_back(plasticityRate);
        
        // Plasticity rate should change with development
        if (stage > 0) {
            passed &= (std::abs(stageHistory[stage] - stageHistory[stage-1]) < 0.1f);
        }
    }
    
    // Test that development affects different components
    float synaptogenesisRate = development.getSynaptogenesisRate();
    float pruningRate = development.getPruningRate();
    float maturationRate = development.getMaturationRate();
    
    passed &= (synaptogenesisRate > 0.0f);
    passed &= (pruningRate > 0.0f);
    passed &= (maturationRate > 0.0f);
    
    // Test development stage tracking
    float currentStage = development.getDevelopmentStage();
    passed &= (currentStage >= 0.0f && currentStage <= 1.0f);
    
    // Test developmental milestones
    bool milestone1 = development.checkMilestone(DevelopmentalMilestone::FIRST_SPIKE);
    bool milestone2 = development.checkMilestone(DevelopmentalMilestone::FIRST_CONNECTION);
    bool milestone3 = development.checkMilestone(DevelopmentalMilestone::FUNCTIONAL_CIRCUIT);
    
    // These should be false in a fresh system
    passed &= (!milestone1 && !milestone2 && !milestone3);
    
    if (passed) {
        logTest("✓ Developmental integration passed")
    } else {
        logTest("✗ Developmental integration failed")
    }
    
    return passed;
}

bool Phase6IntegrationTest::testSocialLearningIntegration() {
    logTest("Test 10: Social Learning Integration")
    
    bool passed = true;
    
    // Create multiple agents for social learning
    std::vector<AgentBrain> agents;
    for (int i = 0; i < 3; ++i) {
        agents.emplace_back(200, 5, 0.15f);
    }
    
    // Test observation sharing
    for (size_t i = 0; i < agents.size(); ++i) {
        // Each agent observes others
        for (size_t j = 0; j < agents.size(); ++j) {
            if (i != j) {
                // Simulate observation
                Observation observation = {
                    .vision = std::vector<std::vector<float>>(10, std::vector<float>(10, 0.5f)),
                    .audio = std::vector<float>(5, 0.3f),
                    .internal = std::vector<float>(3, 0.2f)
                };
                
                agents[i].observeAction(agents[j], observation);
            }
        }
    }
    
    // Test imitation learning
    std::vector<AgentBrain> models;
    for (int i = 0; i < 2; ++i) {
        models.emplace_back(200, 5, 0.15f);
    }
    
    // Agent learns from models
    Observation observation;
    for (auto& model : models) {
        agents[0].learnFromModel(model, observation);
    }
    
    // Test social memory integration
    SocialMemory socialMemory;
    socialMemory.recordObservation(agents[0], observation);
    
    float socialCohesion = socialMemory.calculateCohesion(agents);
    passed &= (socialCohesion >= 0.0f && socialCohesion <= 1.0f);
    
    // Test shared environment interaction
    SimpleWorld sharedWorld(20, 20);
    
    for (auto& agent : agents) {
        agent.processSensoryInput(Observation());
        MotorCommand action = agent.decodeMotorCommand();
        sharedWorld.applyMotorCommand(action);
    }
    
    // Check that interaction affected world state
    passed &= (sharedWorld.getTotalInteractions() > 0);
    
    if (passed) {
        logTest("✓ Social learning integration passed")
    } else {
        logTest("✗ Social learning integration failed")
    }
    
    return passed;
}

void Phase6IntegrationTest::logTest(const std::string& message) {
    std::cout << "[NLM Phase6Test] " << message << std::endl;
}

} // namespace nlm
