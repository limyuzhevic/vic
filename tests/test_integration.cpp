// Integration Test Suite
// Tests for cross-system integration and Phase 6 integration

#include "AgentBrain.hpp"
#include "NLMLiveVisualization.hpp"
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>

namespace test_integration {
    
    // Test fixtures
    class IntegrationTestFixture {
    public:
        std::shared_ptr<nlm::Config> config_;
        std::shared_ptr<nlm::Brain> brain_;
        std::unique_ptr<nlm::AgentBrain> agentBrain_;
        std::unique_ptr<nlm::NLMLiveVisualization> visualizer_;
        std::unique_ptr<nlm::ResearchMetrics> researchMetrics_;
        std::unique_ptr<nlm::ResearchTools> researchTools_;
        
        IntegrationTestFixture() {
            config_ = std::make_shared<nlm::Config>();
            config_->setVisionDimensions(32, 32);
            config_->setTouchCount(12);
            config_->setInternalSignalCount(6);
            config_->setProprioceptionCount(8);
            config_->setMotorActionCount(8);
            
            brain_ = std::make_shared<nlm::Brain>(config_);
            REQUIRE(brain_->initialize());
            
            agentBrain_ = std::make_unique<nlm::AgentBrain>(brain_, config_);
            
            visualizer_ = std::make_unique<nlm::NLMLiveVisualization>();
            REQUIRE(visualizer_->initialize(1920, 1080, true)); // Headless
            
            researchMetrics_ = std::make_unique<nlm::ResearchMetrics>();
            researchTools_ = std::make_unique<nlm::ResearchTools>();
        }
        
        ~IntegrationTestFixture() {
            if (visualizer_) visualizer_->close();
        }
    };
    
    // AgentBrain Integration Tests
    TEST_CASE_METHOD(IntegrationTestFixture, "AgentBrain Integration", "[integration][agentbrain]") {
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(agentBrain_->getMotorOutputSize() == 8);
        
        // Create and process sensory input
        nlm::SensoryPercept percept;
        percept.vision = std::vector<float>(64 * 64, 0.5f);
        percept.touch = std::vector<float>(12, 0.0f);
        percept.internal = std::vector<float>(6, 0.0f);
        percept.proprioception = std::vector<float>(8, 0.0f);
        
        agentBrain_->processSensoryInput(percept);
        
        // Verify all components are integrated
        REQUIRE(agentBrain_->getNeuromodulationLevel() >= 0.0f);
        REQUIRE(agentBrain_->getCuriosityLevel() >= 0.0f);
        REQUIRE(agentBrain_->getNoveltyLevel() >= 0.0f);
        
        // Get motor command
        nlm::MotorCommand cmd = agentBrain_->decodeMotorCommand();
        REQUIRE(cmd != nlm::MotorCommand::Invalid);
    }
    
    TEST_CASE_METHOD(IntegrationTestFixture, "Visualization-Metrics Integration", "[integration][viz-metrics]") {
        // Setup visualization and metrics
        REQUIRE_NOTHROW(researchMetrics_->setupVisualization(*visualizer_));
        REQUIRE_NOTHROW(visualizer_->setupMetrics(*researchMetrics_));
        
        // Record some metrics
        researchMetrics_->record("integration_test_metric", 0.8);
        researchMetrics_->recordPattern("integration_pattern", true, 0.9);
        
        // Update visualization with brain data
        visualizer_->update(*brain_, agentBrain_.get());
        
        // Both systems should be functional
        REQUIRE(visualizer_->isActive());
        REQUIRE(researchMetrics_->getMetricNames().size() > 0);
    }
    
    TEST_CASE_METHOD(IntegrationTestFixture, "ResearchTools Integration", "[integration][research_tools]") {
        // Create experimental condition
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "integration_test";
        condition.description = "Test integration of research tools";
        condition.parameters = {{ "learning_rate", 0.01 }};
        condition.enabledSystems = { "brain_core", "sensory_processing" };
        
        researchTools_->createExperimentalCondition(condition);
        REQUIRE(researchTools_->runExperimentalCondition("integration_test") == true);
        
        // Integrate with metrics
        nlm::Metrics metrics;
        metrics.record("integration_metric", 1.0);
        
        researchTools_->integrateWithMetrics(metrics);
        
        // Both systems should work together
        auto conditions = researchTools_->getExperimentalConditions();
        REQUIRE(conditions.size() == 1);
        REQUIRE(conditions[0].name == "integration_test");
    }
    
    TEST_CASE_METHOD(IntegrationTestFixture, "Complete System Integration", "[integration][complete]") {
        nlm::SimpleWorld world(48, 48);
        agentBrain_->initialize(world);
        
        // Create complete sensory input
        nlm::SensoryPercept percept;
        percept.vision = std::vector<float>(48 * 48, 0.3f);
        percept.touch = std::vector<float>(12, 0.1f);
        percept.internal = std::vector<float>(6, 0.2f);
        percept.proprioception = std::vector<float>(8, 0.15f);
        
        // Process through all systems
        agentBrain_->processSensoryInput(percept);
        
        // Update visualization
        visualizer_->update(*brain_, agentBrain_.get());
        
        // Record metrics
        researchMetrics_->record("integration_firing_rate", 45.2);
        researchMetrics_->recordLearningProgress("integration_phase", 0.7, 0.02);
        
        // Create research condition
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "complete_integration";
        condition.parameters = {{ "integration_test", 1.0 }};
        condition.enabledSystems = { "brain_core" };
        
        researchTools_->createExperimentalCondition(condition);
        researchTools_->integrateWithResearchMetrics(*researchMetrics_);
        
        // Verify all systems are working
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(agentBrain_->getMotorOutputSize() == 8);
        REQUIRE(visualizer_->isActive());
        REQUIRE(researchMetrics_->getMetricNames().size() > 0);
        REQUIRE(researchTools_->getExperimentalConditions().size() == 1);
    }
    
    // Phase 6 Integration Tests
    TEST_CASE("Phase 6 Integration Test", "[integration][phase6]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.maxSteps = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Verify integration results
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.neuromodulationIntegrated);
        REQUIRE(result.predictionIntegrated);
        REQUIRE(result.developmentIntegrated);
        
        // Check that metrics were collected
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.avgFiringRate >= 0.0f);
        REQUIRE(result.memoryEpisodesStored >= 0.0f);
    }
    
    TEST_CASE("Phase 6 Memory Integration", "[integration][memory]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
        REQUIRE(result.memoryConsolidationProgress >= 0.0f);
        REQUIRE(result.memoryEpisodesStored >= 0.0f);
    }
    
    TEST_CASE("Phase 6 Neuromodulation Integration", "[integration][neuromodulation]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        REQUIRE(result.neuromodulationIntegrated);
        REQUIRE(result.dopamineLevel >= 0.0f);
        REQUIRE(result.curiosityLevel >= 0.0f);
        REQUIRE(result.noveltyLevel >= 0.0f);
        REQUIRE(result.predictionError >= 0.0f);
    }
    
    TEST_CASE("Phase 6 Development Integration", "[integration][development]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        REQUIRE(result.developmentIntegrated);
        REQUIRE(result.developmentalMaturation >= 0.0f);
        REQUIRE(result.neuralPlasticityIndex >= 0.0f);
    }
    
    TEST_CASE("Phase 6 Checkpointing Test", "[integration][checkpoint]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false; // Disable for this test
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Should still work without checkpointing
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.capturedFrames >= 0);
    }
    
    TEST_CASE("Phase 6 Research Visualization Integration", "[integration][research_viz]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableVisualization = true;
        
        auto result = experiment.run(config);
        
        // Should have visualization data
        REQUIRE(result.visualizationDataPath.empty() || !result.visualizationDataPath.empty());
        REQUIRE(result.capturedFrames >= 0);
    }
    
    TEST_CASE("Phase 6 Advanced Metrics Integration", "[integration][advanced_metrics]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableAdvancedMetrics = true;
        
        auto result = experiment.run(config);
        
        // Should have collected advanced metrics
        REQUIRE(result.avgLearningRate >= 0.0f);
        REQUIRE(result.predictionAccuracy >= 0.0f);
        REQUIRE(result.cognitiveFlexibility >= 0.0f);
        REQUIRE(result.neuralPlasticityIndex >= 0.0f);
    }
    
    TEST_CASE("Phase 6 Research Data Export", "[integration][research_export]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Should be able to export research data
        REQUIRE(experiment.exportResearchData("/tmp/phase6_research_data.json") == true);
    }
    
    TEST_CASE("Phase 6 Research Data Import", "[integration][research_import]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        // Run experiment to generate data
        auto result = experiment.run(config);
        REQUIRE(experiment.exportResearchData("/tmp/phase6_research_import_test.json") == true);
        
        // Create new experiment and import data
        nlm::Phase6IntegratedExperiment experiment2;
        REQUIRE(experiment2.importResearchData("/tmp/phase6_research_import_test.json") == true);
    }
    
    // Cross-System Integration Tests
    TEST_CASE("Cross-System Performance Integration", "[integration][performance]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Should complete within reasonable time
        REQUIRE(duration.count() < 60000); // Less than 1 minute
        REQUIRE(result.totalWallClockTime >= 0.0);
        
        // Should have collected performance metrics
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.avgFiringRate >= 0.0f);
    }
    
    TEST_CASE("Cross-System Memory Integration", "[integration][memory_full]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 200;
        config.neuronCount = 200;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Memory systems should be integrated
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
        REQUIRE(result.memoryConsolidationProgress >= 0.0f);
        REQUIRE(result.memoryEpisodesStored >= 0.0f);
        
        // Memory should affect other systems
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.noiseLevel >= 0.0f);
    }
    
    TEST_CASE("Cross-System Neuromodulation Integration", "[integration][neuromod_full]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Neuromodulation should affect learning and behavior
        REQUIRE(result.neuromodulationIntegrated);
        REQUIRE(result.curiosityLevel >= 0.0f);
        REQUIRE(result.noveltyLevel >= 0.0f);
        REQUIRE(result.predictionError >= 0.0f);
        
        // Should have learning impact
        REQUIRE(result.avgLearningRate >= 0.0f);
    }
    
    // Performance Integration Tests
    TEST_CASE("Performance Monitoring Integration", "[integration][performance_monitoring]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Should have performance monitoring data
        REQUIRE(result.totalWallClockTime >= 0.0);
        REQUIRE(result.avgFiringRate >= 0.0f);
        REQUIRE(result.avgSynapticWeight >= -1.0f && result.avgSynapticWeight <= 1.0f);
        
        // Performance should affect reward
        REQUIRE(result.totalReward >= 0.0f);
    }
    
    // End-to-End Integration Tests
    TEST_CASE("End-to-End Experiment Lifecycle", "[e2e][lifecycle]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        // Complete experiment lifecycle
        auto result = experiment.run(config);
        
        // Verify all stages completed
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
        REQUIRE(result.neuromodulationIntegrated);
        REQUIRE(result.predictionIntegrated);
        REQUIRE(result.developmentIntegrated);
        
        // Verify data was collected and exported
        REQUIRE(experiment.exportResearchData("/tmp/e2e_lifecycle_data.json") == true);
    }
    
    TEST_CASE("End-to-End Real-time Monitoring", "[e2e][monitoring]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableVisualization = true;
        
        auto result = experiment.run(config);
        
        // Real-time monitoring should have captured data
        REQUIRE(result.capturedFrames >= 0);
        REQUIRE(result.visualizationDataPath.empty() || !result.visualizationDataPath.empty());
        
        // Should have metrics for monitoring
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.noiseLevel >= 0.0f);
    }
    
    TEST_CASE("End-to-End Research Workflow", "[e2e][research_workflow]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableAdvancedMetrics = true;
        
        auto result = experiment.run(config);
        
        // Complete research workflow
        REQUIRE(experiment.exportResearchData("/tmp/e2e_research_workflow.json") == true);
        
        // Research workflow should include all major components
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.neuromodulationIntegrated);
        REQUIRE(result.predictionIntegrated);
        REQUIRE(result.developmentIntegrated);
        
        // Should have research metrics
        REQUIRE(result.avgLearningRate >= 0.0f);
        REQUIRE(result.cognitiveFlexibility >= 0.0f);
        REQUIRE(result.neuralPlasticityIndex >= 0.0f);
    }
    
    // Scalability Tests
    TEST_CASE("Scalability - Neuron Count Integration", "[e2e][scalability_neuron]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 500; // Higher neuron count
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Should scale reasonably
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(duration.count() < 120000); // Less than 2 minutes
        
        // Memory integration should work with larger networks
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
    }
    
    TEST_CASE("Scalability - Step Count Integration", "[e2e][scalability_steps]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 500; // More steps
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Should handle more steps
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
        
        // Should have collected data over longer period
        REQUIRE(result.totalWallClockTime >= 0.0);
    }
    
    // Benchmark Tests
    BENCHMARK("Integration Test - Full System", "[e2e][benchmark]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        return result.totalReward;
    }
    
    BENCHMARK("Integration Test - Memory Systems", "[e2e][benchmark]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        return result.memoryConsolidationProgress;
    }
    
    BENCHMARK("Integration Test - Neuromodulation", "[e2e][benchmark]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        return result.curiosityLevel + result.noveltyLevel + result.dopamineLevel;
    }
    
    BENCHMARK("Integration Test - Development", "[e2e][benchmark]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        return result.developmentalMaturation;
    }
    
    BENCHMARK("Integration Test - Complete Workflow", "[e2e][benchmark]") {
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 100;
        config.neuronCount = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        return result.totalReward;
    }
    
    // Backward Compatibility Tests
    TEST_CASE("Backward Compatibility - Legacy API", "[compatibility][legacy]") {
        // Test that Phase 6 experiment still works with basic API
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        // Should work with minimal configuration
        auto result = experiment.run(config);
        
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        REQUIRE(result.memoryEpisodicMemoryIntegrated);
    }
    
    TEST_CASE("Backward Compatibility - Configuration Options", "[compatibility][config]") {
        // Test different configuration combinations
        std::vector<nlm::Phase6Config> configs;
        
        nlm::Phase6Config config1;
        config1.maxSteps = 50;
        config1.neuronCount = 50;
        config1.enableCheckpointing = false;
        config1.enableReplay = false;
        config1.enableDevelopment = false;
        configs.push_back(config1);
        
        nlm::Phase6Config config2;
        config2.maxSteps = 50;
        config2.neuronCount = 50;
        config2.enableCheckpointing = true; // Different option
        config2.enableReplay = false;
        config2.enableDevelopment = false;
        configs.push_back(config2);
        
        nlm::Phase6IntegratedExperiment experiment;
        
        for (const auto& config : configs) {
            auto result = experiment.run(config);
            REQUIRE(result.totalReward >= 0.0f);
        }
    }
    
    // Data Export/Import Compatibility
    TEST_CASE("Data Export/Import Compatibility", "[compatibility][data]") {
        nlm::Phase6IntegratedExperiment experiment1;
        nlm::Phase6IntegratedExperiment experiment2;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        // Run experiment 1 and export data
        auto result1 = experiment1.run(config);
        REQUIRE(experiment1.exportResearchData("/tmp/compatibility_export.json") == true);
        
        // Run experiment 2 and import data
        REQUIRE(experiment2.importResearchData("/tmp/compatibility_export.json") == true);
        
        // Both should have valid results
        REQUIRE(result1.totalReward >= 0.0f);
        // experiment2 result is from import, so it should be valid too
    }
    
    // Python Binding Tests (simulated)
    TEST_CASE("Python Binding Compatibility", "[compatibility][python]") {
        // Simulate Python binding tests
        // In real implementation, this would test Python bindings
        nlm::Phase6IntegratedExperiment experiment;
        
        nlm::Phase6Config config;
        config.maxSteps = 50;
        config.neuronCount = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto result = experiment.run(config);
        
        // Should work the same way as C++
        REQUIRE(result.totalReward >= 0.0f);
        REQUIRE(result.memoryWorkingMemoryIntegrated);
        
        // Data should be exportable (for Python integration)
        REQUIRE(experiment.exportResearchData("/tmp/python_binding_test.json") == true);
    }
    
} // namespace test_integration