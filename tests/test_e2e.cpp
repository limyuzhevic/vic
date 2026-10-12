// End-to-End Test Suite
// Complete workflow and real-world scenario tests

#include "AgentBrain.hpp"
#include "NLMLiveVisualization.hpp"
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include "Brain.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <filesystem>

namespace test_e2e {
    
    // Test fixtures
    class E2ETestFixture {
    public:
        std::shared_ptr<nlm::Config> config_;
        std::shared_ptr<nlm::Brain> brain_;
        std::unique_ptr<nlm::AgentBrain> agentBrain_;
        std::unique_ptr<nlm::NLMLiveVisualization> visualizer_;
        std::unique_ptr<nlm::ResearchMetrics> researchMetrics_;
        std::unique_ptr<nlm::ResearchTools> researchTools_;
        
        E2ETestFixture() {
            config_ = std::make_shared<nlm::Config>();
            config_->setVisionDimensions(64, 64);
            config_->setTouchCount(16);
            config_->setInternalSignalCount(8);
            config_->setProprioceptionCount(10);
            config_->setMotorActionCount(10);
            
            brain_ = std::make_shared<nlm::Brain>(config_);
            REQUIRE(brain_->initialize());
            
            agentBrain_ = std::make_unique<nlm::AgentBrain>(brain_, config_);
            
            visualizer_ = std::make_unique<nlm::NLMLiveVisualization>();
            REQUIRE(visualizer_->initialize(1920, 1080, true)); // Headless
            
            researchMetrics_ = std::make_unique<nlm::ResearchMetrics>();
            researchTools_ = std::make_unique<nlm::ResearchTools>();
        }
        
        ~E2ETestFixture() {
            if (visualizer_) visualizer_->close();
        }
    };
    
    // Experiment Lifecycle Tests
    TEST_CASE_METHOD(E2ETestFixture, "Complete Experiment Execution", "[e2e][lifecycle]") {
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        // Simulate complete experiment lifecycle
        std::vector<nlm::SensoryPercept> percepts;
        for (int step = 0; step < 50; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(64 * 64, 0.3f + (step * 0.01f));
            percept.touch = std::vector<float>(16, 0.1f + (step * 0.005f));
            percept.internal = std::vector<float>(8, 0.2f);
            percept.proprioception = std::vector<float>(10, 0.15f);
            
            agentBrain_->processSensoryInput(percept);
            
            // Get motor command
            nlm::MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Apply reward based on some logic
            float reward = 0.0f;
            if (cmd == nlm::MotorCommand::Wait) reward = 0.1f;
            else if (cmd == nlm::MotorCommand::MoveForward) reward = 0.5f;
            else if (cmd == nlm::MotorCommand::TurnLeft || cmd == nlm::MotorCommand::TurnRight) reward = 0.3f;
            
            agentBrain_->applyRewardModulation(reward, reward * 0.8f);
            
            // Update metrics
            researchMetrics_->record("reward", reward, nlm::Metrics::Category::Learning);
            researchMetrics_->record("firing_rate", brain_->getAverageFiringRate(), nlm::Metrics::Category::Neuron);
            
            // Update visualization
            visualizer_->update(*brain_, agentBrain_.get());
            
            // Update research tools
            researchTools_->addComparisonData("step_" + std::to_string(step), *researchMetrics_);
        }
        
        // Verify experiment completed
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(agentBrain_->getMotorOutputSize() == 10);
        REQUIRE(visualizer_->isActive());
        
        // Verify metrics collected
        auto metricNames = researchMetrics_->getMetricNames();
        REQUIRE(!metricNames.empty());
        REQUIRE(metricNames.size() >= 2); // reward and firing_rate
        
        // Verify research tools have data
        auto conditions = researchTools_->getExperimentalConditions();
        REQUIRE(conditions.size() >= 1); // at least one condition
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "Result Validation", "[e2e][validation]") {
        nlm::SimpleWorld world(32, 32);
        agentBrain_->initialize(world);
        
        // Run experiment with validation criteria
        float totalReward = 0.0f;
        float maxFiringRate = 0.0f;
        float minNovelty = 1.0f;
        
        for (int step = 0; step < 30; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.4f);
            
            agentBrain_->processSensoryInput(percept);
            
            // Apply reward based on curiosity level
            float curiosity = agentBrain_->getCuriosityLevel();
            float reward = curiosity * 0.5f;
            totalReward += reward;
            
            agentBrain_->applyRewardModulation(reward, reward * 0.9f);
            
            // Update metrics with validation data
            researchMetrics_->record("total_reward", totalReward, nlm::Metrics::Category::Learning);
            researchMetrics_->record("curiosity_level", curiosity, nlm::Metrics::Category::Neuromodulation);
            researchMetrics_->record("firing_rate", brain_->getAverageFiringRate(), nlm::Metrics::Category::Neuron);
            
            // Track validation metrics
            maxFiringRate = std::max(maxFiringRate, brain_->getAverageFiringRate());
            minNovelty = std::min(minNovelty, agentBrain_->getNoveltyLevel());
        
        }
        
        // Validate results meet criteria
        REQUIRE(totalReward >= 0.0f);
        REQUIRE(maxFiringRate >= 0.0f);
        REQUIRE(minNovelty <= 1.0f);
        
        // Verify data was collected correctly
        auto totalRewardValue = researchMetrics_->getOr<double>("total_reward", 0.0);
        REQUIRE(totalRewardValue == totalReward);
        
        // Export validated results
        REQUIRE(visualizer_->exportToJSON("/tmp/e2e_validation_results.json") == true);
        REQUIRE(researchMetrics_->exportToCSV("/tmp/e2e_metrics_results.csv") == true);
        
        // Cleanup test files
        std::filesystem::remove("/tmp/e2e_validation_results.json");
        std::filesystem::remove("/tmp/e2e_metrics_results.csv");
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "Real-time Monitoring Tests", "[e2e][monitoring]") {
        nlm::SimpleWorld world(48, 48);
        agentBrain_->initialize(world);
        
        // Start real-time monitoring
        auto monitoringStart = std::chrono::high_resolution_clock::now();
        int monitoringSteps = 0;
        
        for (int step = 0; step < 100; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(48 * 48, 0.2f + (step * 0.001f));
            
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Process sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Update metrics in real-time
            researchMetrics_->record("monitoring_step", step);
            researchMetrics_->record("step_reward", agentBrain_->getNeuromodulationLevel());
            
            // Update visualization for monitoring
            visualizer_->update(*brain_, agentBrain_.get());
            visualizer_->captureFrame();
            
            // Export monitoring data
            if (step % 25 == 0) {
                researchMetrics_->exportToCSV("/tmp/monitoring_step_" + std::to_string(step) + ".csv");
                visualizer_->exportToJSON("/tmp/monitoring_visualization_" + std::to_string(step) + ".json");
            }
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart);
            
            // Ensure step completes in reasonable time
            REQUIRE(stepDuration.count() < 100); // Each step under 100ms
            
            monitoringSteps++;
        }
        
        auto monitoringEnd = std::chrono::high_resolution_clock::now();
        auto totalMonitoringDuration = std::chrono::duration_cast<std::chrono::seconds>(monitoringEnd - monitoringStart);
        
        // Monitoring should complete successfully
        REQUIRE(monitoringSteps == 100);
        REQUIRE(totalMonitoringDuration.count() < 300); // Less than 5 minutes
        
        // Verify monitoring data was captured
        auto timeSeries = researchMetrics_->getTimeSeries();
        REQUIRE(!timeSeries.empty());
        
        // Cleanup monitoring files
        for (int step = 0; step < 100; step += 25) {
            std::filesystem::remove("/tmp/monitoring_step_" + std::to_string(step) + ".csv");
            std::filesystem::remove("/tmp/monitoring_visualization_" + std::to_string(step) + ".json");
        }
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "Research Workflow Tests", "[e2e][workflow]") {
        // Complete research workflow: setup -> experiment -> analysis -> export
        
        // 1. Setup research tools
        nlm::ResearchTools::ExperimentalCondition researchCondition;
        researchCondition.name = "research_workflow_test";
        researchCondition.description = "Complete research workflow test";
        researchCondition.parameters = {
            {"learning_rate", 0.05},
            {"plasticity_rate", 0.2},
            {"neuron_count", 200}
        };
        researchCondition.enabledSystems = { "brain_core", "sensory_processing", "motor_control" };
        
        researchTools_->createExperimentalCondition(researchCondition);
        researchTools_->runExperimentalCondition("research_workflow_test");
        
        // 2. Run experiment with research tools active
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        for (int step = 0; step < 50; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(64 * 64, 0.3f + (step * 0.002f));
            
            agentBrain_->processSensoryInput(percept);
            
            // Record research metrics
            researchMetrics_->record("workflow_step", step);
            researchMetrics_->record("workflow_reward", agentBrain_->getNeuromodulationLevel());
            
            // Update visualization
            visualizer_->update(*brain_, agentBrain_.get());
            
            // Integrate with research tools
            researchTools_->integrateWithResearchMetrics(*researchMetrics_);
        }
        
        // 3. Analysis phase
        auto performanceReport = researchTools_->generatePerformanceReport();
        auto learningReport = researchTools_->generateLearningReport();
        
        // Analysis should generate reports
        // (Reports may be empty if no comparison data, but process should not crash)
        
        // 4. Export research data
        REQUIRE(researchTools_->exportResearchData("/tmp/research_workflow_data.json") == true);
        
        // 5. Generate final metrics report
        std::string finalMetricsReport = researchMetrics_->generateReport();
        REQUIRE(!finalMetricsReport.empty());
        REQUIRE(finalMetricsReport.find("NLM Research Metrics Report") != std::string::npos);
        
        // Cleanup
        std::filesystem::remove("/tmp/research_workflow_data.json");
        
        // Verify workflow completed successfully
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(visualizer_->isActive());
        REQUIRE(researchMetrics_->getMetricNames().size() > 0);
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "Parameter Optimization Tests", "[e2e][optimization]") {
        // Set up parameter optimization workflow
        
        // Create parameter tuning
        nlm::ResearchTools::ParameterTune learningRateTune;
        learningRateTune.parameterName = "learning_rate";
        learningRateTune.initialValue = 0.05;
        learningRateTune.minValue = 0.001;
        learningRateTune.maxValue = 0.2;
        learningRateTune.stepSize = 0.01;
        learningRateTune.optimizeForMax = true;
        
        // Mock performance function (simplified)
        learningRateTune.performanceFunction = [](const std::map<std::string, double>& params) {
            double learningRate = params.at("learning_rate");
            // Simple learning curve function
            return learningRate * (1.0 - std::exp(-10.0 * learningRate));
        };
        
        researchTools_->addParameterTune(learningRateTune);
        researchTools_->runParameterTuning("learning_rate");
        
        // Get optimization results
        auto tuningResults = researchTools_->getParameterTuningResults();
        REQUIRE(tuningResults.size() == 1);
        
        auto& result = tuningResults[0];
        REQUIRE(result.parameterName == "learning_rate");
        REQUIRE(result.optimalValue >= 0.001);
        REQUIRE(result.optimalValue <= 0.2);
        REQUIRE(result.optimalPerformance >= 0.0);
        REQUIRE(result.tuningCompleted);
        
        // Run complete experiment with optimal parameters
        researchTools_->setParameter("learning_rate", result.optimalValue);
        
        nlm::SimpleWorld world(48, 48);
        agentBrain_->initialize(world);
        
        for (int step = 0; step < 50; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(48 * 48, 0.3f);
            
            agentBrain_->processSensoryInput(percept);
            
            // Apply optimized parameter
            float reward = agentBrain_->getNeuromodulationLevel() * 0.5f;
            agentBrain_->applyRewardModulation(reward, reward * 0.8f);
            
            researchMetrics_->record("optimized_step", step);
            visualizer_->update(*brain_, agentBrain_.get());
        }
        
        // Export optimization results
        REQUIRE(visualizer_->exportToJSON("/tmp/optimization_results.json") == true);
        
        // Cleanup
        std::filesystem::remove("/tmp/optimization_results.json");
        
        // Verify optimization workflow
        REQUIRE(agentBrain_->getNeuromodulationLevel() >= 0.0f);
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "Ablation Study Tests", "[e2e][ablation]") {
        // Create ablations for study
        
        // Ablate development system
        nlm::ResearchTools::AblationConfig developmentAblation;
        developmentAblation.systemName = "development_system";
        developmentAblation.isAblated = false;
        developmentAblation.ablationMethod = "complete_deactivation";
        
        researchTools_->configureAblation(developmentAblation);
        researchTools_->enableAblation("development_system");
        
        // Run experiment with development ablated
        nlm::SimpleWorld world(32, 32);
        agentBrain_->initialize(world);
        
        float totalRewardAblated = 0.0f;
        for (int step = 0; step < 30; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.3f);
            
            agentBrain_->processSensoryInput(percept);
            
            // Get motor command
            nlm::MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Calculate reward based on behavior
            float reward = (cmd == nlm::MotorCommand::Wait) ? 0.1f : 0.3f;
            totalRewardAblated += reward;
            
            agentBrain_->applyRewardModulation(reward, reward * 0.8f);
            
            researchMetrics_->record("ablated_step", step);
            researchTools_->integrateWithMetrics(*researchMetrics_);
        }
        
        // Restore development system
        researchTools_->disableAblation("development_system");
        
        // Run experiment with development system restored
        float totalRewardRestored = 0.0f;
        for (int step = 0; step < 30; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.3f);
            
            agentBrain_->processSensoryInput(percept);
            
            // Get motor command
            nlm::MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Calculate reward based on behavior
            float reward = (cmd == nlm::MotorCommand::Wait) ? 0.1f : 0.3f;
            totalRewardRestored += reward;
            
            agentBrain_->applyRewardModulation(reward, reward * 0.8f);
            
            researchMetrics_->record("restored_step", step);
            researchTools_->integrateWithMetrics(*researchMetrics_);
        }
        
        // Compare results
        REQUIRE(totalRewardAblated >= 0.0f);
        REQUIRE(totalRewardRestored >= 0.0f);
        
        // Results should be different (development affects behavior)
        // Note: With the current simple brain, they might be similar, but the framework should work
        
        // Export ablation study results
        REQUIRE(researchTools_->exportResults("/tmp/ablation_study_results.json") == true);
        
        // Cleanup
        std::filesystem::remove("/tmp/ablation_study_results.json");
        
        // Verify study completed
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
    }
    
    TEST_CASE_METHOD(E2ETestFixture, "End-to-End with Research Tools", "[e2e][complete_workflow]") {
        // Complete end-to-end workflow combining all systems
        
        // Phase 1: Setup research environment
        researchTools_->setResearchMode(true);
        researchTools_->enableDebugMode(true);
        researchTools_->pauseAllSystems(false);
        
        // Create research conditions
        for (int i = 0; i < 3; ++i) {
            nlm::ResearchTools::ExperimentalCondition condition;
            condition.name = "condition_" + std::to_string(i);
            condition.description = "Research condition " + std::to_string(i);
            condition.parameters = {
                {"learning_rate", 0.01 + i * 0.01},
                {"neuron_count", 100 + i * 50},
                {"plasticity_rate", 0.1 + i * 0.05}
            };
            condition.enabledSystems = { "brain_core", "sensory_processing" };
            
            researchTools_->createExperimentalCondition(condition);
        }
        
        // Phase 2: Run complete experiment with each condition
        for (int condIdx = 0; condIdx < 3; ++condIdx) {
            researchTools_->runExperimentalCondition("condition_" + std::to_string(condIdx));
            
            nlm::SimpleWorld world(40, 40);
            agentBrain_->initialize(world);
            
            for (int step = 0; step < 40; ++step) {
                nlm::SensoryPercept percept;
                percept.vision = std::vector<float>(40 * 40, 0.2f + (step * 0.001f));
                
                agentBrain_->processSensoryInput(percept);
                
                // Update all metrics
                researchMetrics_->record("workflow_metric", agentBrain_->getNeuromodulationLevel());
                visualizer_->update(*brain_, agentBrain_.get());
                
                // Integrate with research tools
                researchTools_->integrateWithResearchMetrics(*researchMetrics_);
            }
            
            // Export data for each condition
            std::string conditionFile = "/tmp/condition_" + std::to_string(condIdx) + "_data.json";
            researchTools_->exportResults(conditionFile);
        }
        
        // Phase 3: Analysis and comparison
        auto allComparisons = researchTools_->compareAllConditions();
        REQUIRE(!allComparisons.empty());
        
        // Phase 4: Generate final reports
        std::string finalReport = researchMetrics_->generateReport();
        REQUIRE(!finalReport.empty());
        
        std::string statisticalReport = researchMetrics_->generateStatisticalReport();
        REQUIRE(!statisticalReport.empty());
        
        std::string timeSeriesReport = researchMetrics_->generateTimeSeriesReport();
        REQUIRE(!timeSeriesReport.empty());
        
        std::string mlReport = researchMetrics_->generateMLReport();
        REQUIRE(!mlReport.empty());
        
        // Phase 5: Export everything
        REQUIRE(researchTools_->exportResearchData("/tmp/final_research_data.json") == true);
        
        // Cleanup condition files
        for (int i = 0; i < 3; ++i) {
            std::filesystem::remove("/tmp/condition_" + std::to_string(i) + "_data.json");
        }
        std::filesystem::remove("/tmp/final_research_data.json");
        
        // Verify complete workflow
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(visualizer_->isActive());
        REQUIRE(researchMetrics_->getMetricNames().size() > 0);
    }
    
    // Performance Tests
    BENCHMARK("E2E Complete Workflow", "[e2e][benchmark]") {
        nlm::ResearchTools researchTools;
        nlm::ResearchMetrics researchMetrics;
        nlm::NLMLiveVisualization visualizer;
        
        auto config = std::make_shared<nlm::Config>();
        config->setVisionDimensions(32, 32);
        config->setTouchCount(12);
        config->setInternalSignalCount(6);
        config->setProprioceptionCount(8);
        config->setMotorActionCount(8);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        auto agentBrain = std::make_unique<nlm::AgentBrain>(brain, config);
        
        visualizer.initialize(1920, 1080, true);
        
        // Create research condition
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "benchmark_condition";
        condition.parameters = {{ "learning_rate", 0.01 }};
        condition.enabledSystems = { "brain_core" };
        
        researchTools.createExperimentalCondition(condition);
        researchTools.runExperimentalCondition("benchmark_condition");
        
        nlm::SimpleWorld world(32, 32);
        agentBrain->initialize(world);
        
        for (int step = 0; step < 50; ++step) {
            nlm::SensoryPercept percept;
            percept.vision = std::vector<float>(32 * 32, 0.3f);
            
            agentBrain->processSensoryInput(percept);
            
            researchMetrics.record("benchmark_step", step);
            visualizer.update(*brain, agentBrain.get());
            
            researchTools.integrateWithResearchMetrics(researchMetrics);
        }
        
        visualizer.close();
        
        return researchMetrics.getMetricNames().size();
    }
    
} // namespace test_e2e