# Test for NLM Experiment Systems
# This file tests Phase 3-6 experiment systems

#include "tests/common/TestCommon.hpp"
#include "src/experiments/Phase3Experiment.hpp"
#include "src/experiments/Phase4Experiment.hpp"
#include "src/experiments/Phase5Experiment.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include <cassert>
#include <iostream>

class ExperimentSystemTests {
public:
    static void runAllTests() {
        std::cout << "=== NLM Experiment System Tests ===" << std::endl;
        
        runPhase3Tests();
        runPhase4Tests();
        runPhase5Tests();
        runPhase6Tests();
        runMetricsTests();
        runIntegrationTests();
        
        std::cout << "\n=== All Experiment System Tests Passed! ===" << std::endl;
    }
    
private:
    static void runPhase3Tests() {
        std::cout << "\n--- Phase 3 Tests ---" << std::endl;
        
        // Create test configuration
        auto config = TestCommon::createTestConfig();
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test temporal prediction experiment
        nlm::Phase3TemporalPredictionExperiment temporalExp;
        nlm::Phase3Results results = temporalExp.run(brain.get(), 100);
        
        // Validate results
        assert(results.temporalAccuracy >= 0.0f && "Temporal accuracy should be non-negative");
        assert(results.spikeCount > 0 && "Should have spikes in temporal prediction");
        
        std::cout << "  ✓ Temporal Prediction test passed" << std::endl;
        
        // Test sensory processing experiment
        nlm::Phase3SensoryProcessingExperiment sensoryExp;
        results = sensoryExp.run(brain.get(), 100);
        
        assert(results.sensorAccuracy >= 0.0f && "Sensor accuracy should be non-negative");
        assert(results.featureExtractionQuality >= 0.0f && "Feature extraction quality should be non-negative");
        
        std::cout << "  ✓ Sensory Processing test passed" << std::endl;
        
        // Test motor control experiment
        nlm::Phase3MotorControlExperiment motorExp;
        results = motorExp.run(brain.get(), 100);
        
        assert(results.motorAccuracy >= 0.0f && "Motor accuracy should be non-negative");
        assert(results.actionSelectionQuality >= 0.0f && "Action selection quality should be non-negative");
        
        std::cout << "  ✓ Motor Control test passed" << std::endl;
        
        std::cout << "  ✓ All Phase 3 tests passed" << std::endl;
    }
    
    static void runPhase4Tests() {
        std::cout << "\n--- Phase 4 Tests ---" << std::endl;
        
        // Create test configuration
        auto config = TestCommon::createTestConfig();
        config->set("neuron_count", size_t(500));
        config->set("region_count", size_t(2));
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test all Phase 4 experiments
        nlm::Phase4TemporalPredictionExperiment temporalExp;
        nlm::Phase4Results results = temporalExp.run(brain.get(), 50);
        
        assert(results.predictionErrorInitial >= 0.0f && "Initial prediction error should be non-negative");
        assert(results.predictionErrorFinal >= 0.0f && "Final prediction error should be non-negative");
        assert(results.predictionAccuracyImprovement >= 0.0f && "Prediction improvement should be non-negative");
        
        std::cout << "  ✓ Temporal Prediction test passed" << std::endl;
        
        // Test working memory experiment
        nlm::Phase4WorkingMemoryExperiment memoryExp;
        results = memoryExp.run(brain.get(), 50);
        
        assert(results.workingMemoryRetentionInitial >= 0.0f && "Initial retention should be non-negative");
        assert(results.workingMemoryRetentionFinal >= results.workingMemoryRetentionInitial && 
               "Retention should not decrease");
        assert(results.workingMemoryCapacity > 0.0f && "Capacity should be positive");
        
        std::cout << "  ✓ Working Memory test passed" << std::endl;
        
        // Test episodic memory experiment
        nlm::Phase4EpisodicRecallExperiment episodicExp;
        results = episodicExp.run(brain.get(), 50);
        
        assert(results.episodicRecallAccuracy >= 0.0f && "Recall accuracy should be non-negative");
        assert(results.episodesStored > 0 && "Should store episodes");
        assert(results.episodicInfluence >= 0.0f && "Episodic influence should be non-negative");
        
        std::cout << "  ✓ Episodic Memory test passed" << std::endl;
        
        // Test concept formation experiment
        nlm::Phase4ConceptFormationExperiment conceptExp;
        results = conceptExp.run(brain.get(), 50);
        
        assert(results.conceptsFormed >= 0 && "Should form concepts");
        assert(results.conceptStability >= 0.0f && "Concept stability should be non-negative");
        assert(results.generalizationAbility >= 0.0f && "Generalization ability should be non-negative");
        
        std::cout << "  ✓ Concept Formation test passed" << std::endl;
        
        // Test attention experiment
        nlm::Phase4AttentionExperiment attentionExp;
        results = attentionExp.run(brain.get(), 50);
        
        assert(results.attentionSelectivity >= 0.0f && "Attention selectivity should be non-negative");
        assert(results.distractionResistance >= 0.0f && "Distraction resistance should be non-negative");
        
        std::cout << "  ✓ Attention test passed" << std::endl;
        
        // Test planning experiment
        nlm::Phase4PlanningExperiment planningExp;
        results = planningExp.run(brain.get(), 25);
        
        assert(results.planningAccuracy >= 0.0f && "Planning accuracy should be non-negative");
        assert(results.planningConfidence >= 0.0f && "Planning confidence should be non-negative");
        
        std::cout << "  ✓ Planning test passed" << std::endl;
        
        // Test self-model experiment
        nlm::Phase4SelfModelExperiment selfExp;
        results = selfExp.run(brain.get(), 50);
        
        assert(results.selfPredictionAccuracy >= 0.0f && "Self prediction accuracy should be non-negative");
        assert(results.bodyAwareness >= 0.0f && "Body awareness should be non-negative");
        
        std::cout << "  ✓ Self-Model test passed" << std::endl;
        
        // Test social learning experiment
        nlm::Phase4SocialLearningExperiment socialExp;
        results = socialExp.run(brain.get(), 25);
        
        assert(results.imitationAccuracy >= 0.0f && "Imitation accuracy should be non-negative");
        assert(results.observationsFromOthers >= 0 && "Should observe from others");
        
        std::cout << "  ✓ Social Learning test passed" << std::endl;
        
        // Test continual learning experiment
        nlm::Phase4ContinualLearningExperiment continualExp;
        results = continualExp.run(brain.get(), 50);
        
        assert(results.behaviorImprovement >= 0.0f && "Behavior improvement should be non-negative");
        assert(results.transferPerformance >= 0.0f && "Transfer performance should be non-negative");
        
        std::cout << "  ✓ Continual Learning test passed" << std::endl;
        
        std::cout << "  ✓ All Phase 4 tests passed" << std::endl;
    }
    
    static void runPhase5Tests() {
        std::cout << "\n--- Phase 5 Tests ---" << std::endl;
        
        // Create test configuration
        auto config = TestCommon::createTestConfig();
        config->set("neuron_count", size_t(300));
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test performance optimization experiments
        nlm::Phase5PerformanceExperiment perfExp;
        nlm::Phase5Results results = perfExp.run(brain.get(), 50);
        
        assert(results.memoryEfficiency > 0.0f && "Memory efficiency should be positive");
        assert(results.spikeEfficiency > 0.0f && "Spike efficiency should be positive");
        assert(results.computationalSpeed > 0.0f && "Computational speed should be positive");
        
        std::cout << "  ✓ Performance Optimization test passed" << std::endl;
        
        // Test neural plasticity experiments
        nlm::Phase5NeuralPlasticityExperiment plasticityExp;
        results = plasticityExp.run(brain.get(), 50);
        
        assert(results.plasticityEfficiency >= 0.0f && "Plasticity efficiency should be non-negative");
        assert(results.stability >= 0.0f && "Stability should be non-negative");
        assert(results.adaptationCapacity >= 0.0f && "Adaptation capacity should be non-negative");
        
        std::cout << "  ✓ Neural Plasticity test passed" << std::endl;
        
        // Test learning efficiency experiments
        nlm::Phase5LearningEfficiencyExperiment learningExp;
        results = learningExp.run(brain.get(), 50);
        
        assert(results.learningRate > 0.0f && "Learning rate should be positive");
        assert(results.generalization >= 0.0f && "Generalization should be non-negative");
        assert(results.fewShotLearning >= 0.0f && "Few-shot learning should be non-negative");
        
        std::cout << "  ✓ Learning Efficiency test passed" << std::endl;
        
        // Test scalability experiments
        nlm::Phase5ScalabilityExperiment scalingExp;
        results = scalingExp.run(brain.get(), 30);
        
        assert(results.scalability >= 0.0f && "Scalability should be non-negative");
        assert(results.parallelEfficiency >= 0.0f && "Parallel efficiency should be non-negative");
        assert(results.distributedPerformance >= 0.0f && "Distributed performance should be non-negative");
        
        std::cout << "  ✓ Scalability test passed" << std::endl;
        
        std::cout << "  ✓ All Phase 5 tests passed" << std::endl;
    }
    
    static void runPhase6Tests() {
        std::cout << "\n--- Phase 6 Tests ---" << std::endl;
        
        // Create test configuration
        auto config = TestCommon::createTestConfig();
        config->set("neuron_count", size_t(200));
        config->set("region_count", size_t(1));
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test integrated brain systems
        nlm::Phase6IntegratedExperiment integratedExp;
        nlm::Phase6Results results = integratedExp.run(brain.get(), 30, 50);
        
        assert(results.totalReward >= 0.0f && "Total reward should be non-negative");
        assert(results.episodesStored > 0 && "Should store episodes");
        assert(results.conceptsFormed >= 0 && "Should form concepts");
        assert(results.planningConfidence >= 0.0f && "Planning confidence should be non-negative");
        assert(results.bodyAwareness >= 0.0f && "Body awareness should be non-negative");
        
        std::cout << "  ✓ Integrated Phase 6 test passed" << std::endl;
        
        // Test checkpoint system
        nlm::Phase6CheckpointExperiment checkpointExp;
        results = checkpointExp.run(brain.get(), 10, 20);
        
        assert(results.checkpointAccuracy >= 0.9f && "Checkpoint accuracy should be high");
        assert(results.saveLoadTime < 10.0f && "Save/load time should be reasonable");
        
        std::cout << "  ✓ Checkpoint test passed" << std::endl;
        
        // Test replay and consolidation
        nlm::Phase6ReplayExperiment replayExp;
        results = replayExp.run(brain.get(), 20, 40);
        
        assert(results.replayAccuracy >= 0.0f && "Replay accuracy should be non-negative");
        assert(results.consolidationQuality >= 0.0f && "Consolidation quality should be non-negative");
        
        std::cout << "  ✓ Replay and Consolidation test passed" << std::endl;
        
        std::cout << "  ✓ All Phase 6 tests passed" << std::endl;
    }
    
    static void runMetricsTests() {
        std::cout << "\n--- Metrics Tests ---" << std::endl;
        
        // Test brain metrics collection
        auto config = TestCommon::createTestConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Run some simulation steps
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        nlm::Metrics metrics;
        nlm::MetricsResult result = metrics.calculate(brain.get());
        
        assert(result.neuronCount > 0 && "Should have neurons");
        assert(result.synapseCount > 0 && "Should have synapses");
        assert(result.averageFiringRate >= 0.0f && "Average firing rate should be non-negative");
        assert(result.excitationInhibitionRatio > 0.0f && "E/I ratio should be positive");
        assert(result.totalSpikeCount > 0 && "Should have total spikes");
        
        std::cout << "  ✓ Brain Metrics test passed" << std::endl;
        
        // Test experiment metrics
        nlm::ExperimentMetrics expMetrics;
        nlm::ExperimentMetricsResult expResult = expMetrics.calculate(brain.get(), 100);
        
        assert(expResult.performanceScore >= 0.0f && "Performance score should be non-negative");
        assert(expResult.efficiencyScore >= 0.0f && "Efficiency score should be non-negative");
        assert(expResult.accuracyScore >= 0.0f && "Accuracy score should be non-negative");
        
        std::cout << "  ✓ Experiment Metrics test passed" << std::endl;
        
        std::cout << "  ✓ All Metrics tests passed" << std::endl;
    }
    
    static void runIntegrationTests() {
        std::cout << "\n--- Integration Tests ---" << std::endl;
        
        // Test complete brain with all subsystems
        auto config = TestCommon::createTestConfig();
        config->set("neuron_count", size_t(250));
        config->set("region_count", size_t(2));
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize() && "Brain initialization failed");
        
        // Test agent system integration
        auto agent = nlm::createAgentBrain(brain);
        assert(agent && "Agent creation failed");
        
        auto world = nlm::createSimpleWorld();
        assert(world && "World creation failed");
        
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        // Run integration simulation
        for (size_t step = 0; step < 50; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on position
            auto body = world->getAgentBody();
            float reward = (body.x - 10.0) * (body.x - 10.0) + 
                          (body.y - 10.0) * (body.y - 10.0);
            agent->applyRewardModulation(reward, 0.0);
        }
        
        assert(brain->getTotalSpikeCount() > 0 && "Should have spike activity");
        assert(agent->getCuriosityLevel() >= 0.0f && "Curiosity should be non-negative");
        assert(agent->getNoveltyLevel() >= 0.0f && "Novelty should be non-negative");
        
        std::cout << "  ✓ Agent-Brain-World Integration test passed" << std::endl;
        
        // Test neuromodulation integration
        assert(brain->getDopamine() && "Dopamine system should be active");
        assert(brain->getCuriosity() && "Curiosity system should be active");
        assert(brain->getNovelty() && "Novelty system should be active");
        assert(brain->getPredictionErrorSignal() && "Prediction error system should be active");
        
        std::cout << "  ✓ Neuromodulation Integration test passed" << std::endl;
        
        // Test memory systems integration
        assert(brain->getWorkingMemory() && "Working memory should be active");
        assert(brain->getEpisodicMemory() && "Episodic memory should be active");
        assert(brain->getAssociativeMemory() && "Associative memory should be active");
        
        std::cout << "  ✓ Memory Systems Integration test passed" << std::endl;
        
        // Test prediction and cognition systems integration
        assert(brain->getPredictionSystem() && "Prediction system should be active");
        assert(brain->getPlanner() && "Planner should be active");
        assert(brain->getConceptFormation() && "Concept formation should be active");
        assert(brain->getAttention() && "Attention system should be active");
        
        std::cout << "  ✓ Prediction-Cognition Integration test passed" << std::endl;
        
        // Test development system integration
        assert(brain->getDevelopmentSystem() && "Development system should be active");
        
        std::cout << "  ✓ Development System Integration test passed" << std::endl;
        
        std::cout << "  ✓ All Integration tests passed" << std::endl;
    }
};

// Test runner for experiment systems
void runExperimentSystemTests() {
    ExperimentSystemTests::runAllTests();
}

// Entry point for test compilation
int main() {
    runExperimentSystemTests();
    return 0;
}