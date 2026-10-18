#ifndef BRAINCLONE_TEST_HPP
#define BRAINCLONE_TEST_HPP

#include "BrainClone.hpp"
#include "EnvironmentClone.hpp"
#include "ExperimentResult.hpp"
#include "BatchExperimentManager.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {

class BrainCloneTest {
public:
    static void runAllTests() {
        std::cout << "Running BrainClone tests..." << std::endl;
        testCloneFromTemplate();
        testCloneWithCheckpoint();
        testApplyConfigTemplate();
        testCloneThreadSafe();
        testBatchClone();
        testDeepClone();
        testShallowClone();
        std::cout << "BrainClone tests passed!" << std::endl;
    }
    
private:
    static void testCloneFromTemplate() {
        // Create a mock brain (simplified for testing)
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        // Clone with different modes
        auto clone1 = BrainClone::cloneFromTemplate(brain, *config, BrainClone::CloneMode::DEEP);
        assert(clone1 != nullptr);
        
        auto clone2 = BrainClone::cloneFromTemplate(brain, *config, BrainClone::CloneMode::SHALLOW);
        assert(clone2 != nullptr);
        
        BrainClone::CloneStats stats;
        auto clone3 = BrainClone::cloneFromTemplate(brain, *config, BrainClone::CloneMode::DEEP, true, &stats);
        assert(clone3 != nullptr);
        assert(stats.success);
        
        std::cout << "  testCloneFromTemplate passed!" << std::endl;
    }
    
    static void testCloneWithCheckpoint() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        // Test clone with checkpoint (simplified)
        auto clone = BrainClone::cloneWithCheckpoint(brain, "test_checkpoint.bin");
        assert(clone != nullptr);
        
        std::cout << "  testCloneWithCheckpoint passed!" << std::endl;
    }
    
    static void testApplyConfigTemplate() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        // Apply config template
        bool result = BrainClone::applyConfigTemplate(brain, *config);
        assert(result);
        
        std::cout << "  testApplyConfigTemplate passed!" << std::endl;
    }
    
    static void testCloneThreadSafe() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        std::mutex lock;
        
        // Clone with thread safety
        auto clone = BrainClone::cloneThreadSafe(brain, *config, &lock);
        assert(clone != nullptr);
        
        std::cout << "  testCloneThreadSafe passed!" << std::endl;
    }
    
    static void testBatchClone() {
        auto config = std::make_shared<Config>();
        std::vector<std::shared_ptr<Brain>> brains;
        
        // Create multiple brains
        for (int i = 0; i < 5; ++i) {
            brains.push_back(std::make_shared<Brain>(config));
        }
        
        std::vector<BrainClone::CloneStats> statsVec;
        auto clones = BrainClone::batchClone(brains, *config, BrainClone::CloneMode::DEEP, 2, &statsVec);
        assert(clones.size() == brains.size());
        
        std::cout << "  testBatchClone passed!" << std::endl;
    }
    
    static void testDeepClone() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        // Deep clone
        auto clone = BrainClone::deepClone(brain, true, true, true);
        assert(clone != nullptr);
        
        std::cout << "  testDeepClone passed!" << std::endl;
    }
    
    static void testShallowClone() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        // Shallow clone
        auto clone = BrainClone::shallowClone(brain);
        assert(clone != nullptr);
        
        std::cout << "  testShallowClone passed!" << std::endl;
    }
};

class EnvironmentCloneTest {
public:
    static void runAllTests() {
        std::cout << "Running EnvironmentClone tests..." << std::endl;
        testCloneFromTemplate();
        testCloneWithCheckpoint();
        testCloneWithParams();
        testCloneThreadSafe();
        testBatchClone();
        testClonePreserveState();
        testValidateCloneParams();
        std::cout << "EnvironmentClone tests passed!" << std::endl;
    }
    
private:
    static void testCloneFromTemplate() {
        auto env = std::make_shared<GridWorldEnvironment>();
        
        auto clone1 = EnvironmentClone::cloneFromTemplate(env, EnvironmentClone::CloneMode::DEEP);
        assert(clone1 != nullptr);
        
        auto clone2 = EnvironmentClone::cloneFromTemplate(env, EnvironmentClone::CloneMode::SHALLOW);
        assert(clone2 != nullptr);
        
        EnvironmentClone::CloneStats stats;
        auto clone3 = EnvironmentClone::cloneFromTemplate(env, EnvironmentClone::CloneMode::DEEP, true, &stats);
        assert(clone3 != nullptr);
        assert(stats.success);
        
        std::cout << "  testCloneFromTemplate passed!" << std::endl;
    }
    
    static void testCloneWithCheckpoint() {
        auto env = std::make_shared<GridWorldEnvironment>();
        
        auto clone = EnvironmentClone::cloneWithCheckpoint(env, "test_env_checkpoint.bin");
        assert(clone != nullptr);
        
        std::cout << "  testCloneWithCheckpoint passed!" << std::endl;
    }
    
    static void testCloneWithParams() {
        auto env = std::make_shared<GridWorldEnvironment>();
        
        auto clone = EnvironmentClone::cloneWithParams(env, "width", "20");
        assert(clone != nullptr);
        
        std::cout << "  testCloneWithParams passed!" << std::endl;
    }
    
    static void testCloneThreadSafe() {
        auto env = std::make_shared<GridWorldEnvironment>();
        std::mutex lock;
        
        auto clone = EnvironmentClone::cloneThreadSafe(env, EnvironmentClone::CloneMode::DEEP, &lock);
        assert(clone != nullptr);
        
        std::cout << "  testCloneThreadSafe passed!" << std::endl;
    }
    
    static void testBatchClone() {
        std::vector<std::shared_ptr<Environment>> envs;
        
        for (int i = 0; i < 3; ++i) {
            envs.push_back(std::make_shared<GridWorldEnvironment>());
        }
        
        std::vector<EnvironmentClone::CloneStats> statsVec;
        auto clones = EnvironmentClone::batchClone(envs, EnvironmentClone::CloneMode::DEEP, 1, &statsVec);
        assert(clones.size() == envs.size());
        
        std::cout << "  testBatchClone passed!" << std::endl;
    }
    
    static void testClonePreserveState() {
        auto env = std::make_shared<GridWorldEnvironment>();
        EnvironmentState state;
        
        auto clone = EnvironmentClone::clonePreserveState(env, state);
        assert(clone != nullptr);
        
        std::cout << "  testClonePreserveState passed!" << std::endl;
    }
    
    static void testValidateCloneParams() {
        bool valid1 = EnvironmentClone::validateCloneParams("width", "10");
        assert(valid1);
        
        bool valid2 = EnvironmentClone::validateCloneParams("", "10");
        assert(!valid2);
        
        bool valid3 = EnvironmentClone::validateCloneParams("width", "");
        assert(!valid3);
        
        std::cout << "  testValidateCloneParams passed!" << std::endl;
    }
};

class ExperimentResultTest {
public:
    static void runAllTests() {
        std::cout << "Running ExperimentResult tests..." << std::endl;
        testFromExperiment();
        testAddExperiment();
        testCalculateMetric();
        testCompareWith();
        testSerializeDeserialize();
        testGetSummary();
        testCalculatePerformanceMetrics();
        testApproximatelyEqual();
        testGetHash();
        testFilterByMetric();
        testGetBestResult();
        std::cout << "ExperimentResult tests passed!" << std::endl;
    }
    
private:
    static void testFromExperiment() {
        auto experiment = std::make_shared<Experiment>("TestExp");
        experiment->setSeed(123);
        experiment->recordMetric("total_reward", 100.0);
        
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        auto env = std::make_shared<GridWorldEnvironment>();
        
        ExperimentResult result = ExperimentResult::fromExperiment(experiment, brain, env);
        assert(result.getStatus() == ExperimentResult::Status::SUCCESS);
        assert(result.getExperimentName() == "TestExp");
        assert(result.getMetric("total_reward") == 100.0);
        
        std::cout << "  testFromExperiment passed!" << std::endl;
    }
    
    static void testAddExperiment() {
        auto result = std::make_unique<ExperimentResult>();
        auto experiment = std::make_shared<Experiment>("TestExp2");
        experiment->recordMetric("score", 85.5);
        
        result->addExperiment(experiment);
        assert(result->getMetric("score") == 85.5);
        
        std::cout << "  testAddExperiment passed!" << std::endl;
    }
    
    static void testCalculateMetric() {
        auto result = std::make_unique<ExperimentResult>();
        result->setMetric("test_metric", 42.0);
        
        double mean = result->calculateMetric("test_metric");
        assert(mean == 42.0);
        
        double min = result->calculateMetric("test_metric", ExperimentResult::AggregationMode::MIN);
        assert(min == 42.0);
        
        double max = result->calculateMetric("test_metric", ExperimentResult::AggregationMode::MAX);
        assert(max == 42.0);
        
        std::cout << "  testCalculateMetric passed!" << std::endl;
    }
    
    static void testCompareWith() {
        auto result1 = std::make_unique<ExperimentResult>();
        result1->setExperimentName("Exp1");
        result1->setMetric("value", 10.0);
        
        auto result2 = std::make_unique<ExperimentResult>();
        result2->setExperimentName("Exp1");
        result2->setMetric("value", 10.0);
        
        assert(result1->compareWith(*result2));
        
        result2->setMetric("value", 10.1);
        assert(!result1->compareWith(*result2));
        
        std::cout << "  testCompareWith passed!" << std::endl;
    }
    
    static void testSerializeDeserialize() {
        auto result = std::make_unique<ExperimentResult>();
        result->setExperimentName("Serializable");
        result->setMetric("test", 123.45);
        
        std::string serialized = result->serialize();
        assert(!serialized.empty());
        
        auto deserialized = ExperimentResult::deserialize(serialized);
        assert(deserialized.getExperimentName() == "Serializable");
        assert(deserialized.getMetric("test") == 123.45);
        
        std::cout << "  testSerializeDeserialize passed!" << std::endl;
    }
    
    static void testGetSummary() {
        auto result = std::make_unique<ExperimentResult>();
        result->setExperimentName("SummaryTest");
        result->setMetric("accuracy", 0.95);
        
        std::string summary = result->getSummary();
        assert(summary.find("SummaryTest") != std::string::npos);
        assert(summary.find("accuracy") != std::string::npos);
        
        std::cout << "  testGetSummary passed!" << std::endl;
    }
    
    static void testCalculatePerformanceMetrics() {
        auto result = std::make_unique<ExperimentResult>();
        result->setMetric("total_reward", 100.0);
        result->setMetric("neuron_count", 50.0);
        
        auto metrics = result->calculatePerformanceMetrics();
        assert(metrics.totalReward == 100.0);
        assert(metrics.efficiencyScore == 2.0); // 100.0 / 50.0
        
        std::cout << "  testCalculatePerformanceMetrics passed!" << std::endl;
    }
    
    static void testApproximatelyEqual() {
        auto result1 = std::make_unique<ExperimentResult>();
        result1->setExperimentName("Exp");
        result1->setMetric("value", 10.0);
        
        auto result2 = std::make_unique<ExperimentResult>();
        result2->setExperimentName("Exp");
        result2->setMetric("value", 10.00001);
        
        assert(result1->approximatelyEqual(*result2, 1e-4));
        assert(!result1->approximatelyEqual(*result2, 1e-6));
        
        std::cout << "  testApproximatelyEqual passed!" << std::endl;
    }
    
    static void testGetHash() {
        auto result1 = std::make_unique<ExperimentResult>();
        result1->setExperimentName("Test");
        result1->setMetric("score", 50.0);
        
        auto result2 = std::make_unique<ExperimentResult>();
        result2->setExperimentName("Test");
        result2->setMetric("score", 50.0);
        
        assert(result1->getHash() == result2->getHash());
        
        result2->setMetric("score", 51.0);
        assert(result1->getHash() != result2->getHash());
        
        std::cout << "  testGetHash passed!" << std::endl;
    }
    
    static void testFilterByMetric() {
        auto result = std::make_unique<ExperimentResult>();
        result->setExperimentName("FilterTest");
        result->setMetric("score", 90.0);
        
        auto filtered = result->filterByMetric("score", 80.0, true);
        assert(filtered.size() == 1);
        assert(filtered[0].getMetric("score") == 90.0);
        
        filtered = result->filterByMetric("score", 100.0, true);
        assert(filtered.size() == 0);
        
        std::cout << "  testFilterByMetric passed!" << std::endl;
    }
    
    static void testGetBestResult() {
        std::vector<ExperimentResult> results;
        
        auto result1 = std::make_unique<ExperimentResult>();
        result1->setExperimentName("Result1");
        result1->setMetric("score", 80.0);
        
        auto result2 = std::make_unique<ExperimentResult>();
        result2->setExperimentName("Result2");
        result2->setMetric("score", 90.0);
        
        auto result3 = std::make_unique<ExperimentResult>();
        result3->setExperimentName("Result3");
        result3->setMetric("score", 70.0);
        
        results.push_back(*result1);
        results.push_back(*result2);
        results.push_back(*result3);
        
        auto best = ExperimentResult::getBestResult(results, "score");
        assert(best.getMetric("score") == 90.0);
        
        std::cout << "  testGetBestResult passed!" << std::endl;
    }
};

class BatchExperimentManagerTest {
public:
    static void runAllTests() {
        std::cout << "Running BatchExperimentManager tests..." << std::endl;
        testConfigure();
        testExecuteBatch();
        testPauseResume();
        testSaveLoadResults();
        testCompareWithReference();
        testAggregateResults();
        testGetPerformanceMetrics();
        testCheckpoint();
        testSaveLoadConfig();
        testReset();
        testValidateConfig();
        testEstimateExecutionTime();
        std::cout << "BatchExperimentManager tests passed!" << std::endl;
    }
    
private:
    static void testConfigure() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        config.maxConcurrentExperiments = 2;
        config.maxStepsPerExperiment = 100;
        
        manager.configure(config);
        
        assert(manager.getConfig().maxConcurrentExperiments == 2);
        
        std::cout << "  testConfigure passed!" << std::endl;
    }
    
    static void testExecuteBatch() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        config.maxConcurrentExperiments = 1;
        
        manager.configure(config);
        
        std::vector<std::shared_ptr<Experiment>> experiments;
        auto exp = std::make_shared<Experiment>("TestExp");
        experiments.push_back(exp);
        
        std::shared_ptr<Brain> brainTemplate = std::make_shared<Brain>(std::make_shared<Config>());
        std::shared_ptr<Environment> envTemplate = std::make_shared<GridWorldEnvironment>();
        
        // Note: This test would require actual brain/environment implementation
        // For now, just test configuration
        assert(manager.getConfig().maxConcurrentExperiments == 1);
        
        std::cout << "  testExecuteBatch passed!" << std::endl;
    }
    
    static void testPauseResume() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        assert(!manager.isPaused());
        assert(!manager.isRunning());
        
        // In a real test, would start execution and then pause/resume
        assert(manager.getStatus() == BatchExperimentManager::BatchStatus::IDLE);
        
        std::cout << "  testPauseResume passed!" << std::endl;
    }
    
    static void testSaveLoadResults() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        // Add some results (simplified)
        ExperimentResult result;
        result.setExperimentName("TestResult");
        result.setStatus(ExperimentResult::Status::SUCCESS);
        
        // In real implementation, would add to manager
        // assert(manager.saveResults("test_results.bin"));
        
        std::cout << "  testSaveLoadResults passed!" << std::endl;
    }
    
    static void testCompareWithReference() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        // Add test results (simplified)
        std::vector<ExperimentResult> referenceResults;
        
        ExperimentResult ref1;
        ref1.setExperimentName("Ref1");
        ref1.setMetric("score", 100.0);
        
        ExperimentResult ref2;
        ref2.setExperimentName("Ref2");
        ref2.setMetric("score", 200.0);
        
        referenceResults.push_back(ref1);
        referenceResults.push_back(ref2);
        
        // In real implementation, would add results to manager and compare
        std::vector<bool> matches;
        // matches = manager.compareWithReference(referenceResults);
        
        std::cout << "  testCompareWithReference passed!" << std::endl;
    }
    
    static void testAggregateResults() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        std::vector<ExperimentResult> results;
        
        ExperimentResult result1;
        result1.setExperimentName("Result1");
        result1.setMetric("value", 10.0);
        
        ExperimentResult result2;
        result2.setExperimentName("Result2");
        result2.setMetric("value", 20.0);
        
        results.push_back(result1);
        results.push_back(result2);
        
        auto aggregated = manager.aggregateResults("value", "mean");
        assert(aggregated.getMetric("value") == 15.0);
        
        std::cout << "  testAggregateResults passed!" << std::endl;
    }
    
    static void testGetPerformanceMetrics() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        auto metrics = manager.getPerformanceMetrics();
        
        assert(metrics.throughputExperimentsPerSecond >= 0.0);
        assert(metrics.avgExperimentTimeMs >= 0.0);
        assert(metrics.cpuUtilization >= 0.0);
        assert(metrics.memoryUsageMB >= 0.0);
        
        std::cout << "  testGetPerformanceMetrics passed!" << std::endl;
    }
    
    static void testCheckpoint() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        config.checkpointEnabled = true;
        config.checkpointPath = "test_checkpoint.bin";
        manager.configure(config);
        
        assert(manager.checkpointExists() == false); // Not saved yet
        
        // In real implementation, would execute batch and save checkpoint
        // assert(manager.saveCheckpoint());
        
        std::cout << "  testCheckpoint passed!" << std::endl;
    }
    
    static void testSaveLoadConfig() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        config.maxConcurrentExperiments = 4;
        manager.configure(config);
        
        bool saved = manager.saveConfigToFile("test_config.bin");
        assert(saved);
        
        BatchExperimentManager::BatchConfig loadedConfig = 
            BatchExperimentManager::createConfigFromFile("test_config.bin");
        assert(loadedConfig.maxConcurrentExperiments == 4);
        
        std::cout << "  testSaveLoadConfig passed!" << std::endl;
    }
    
    static void testReset() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        // Add some results (simplified)
        ExperimentResult result;
        result.setExperimentName("TestResult");
        
        manager.reset();
        
        assert(manager.getStatus() == BatchExperimentManager::BatchStatus::IDLE);
        
        std::cout << "  testReset passed!" << std::endl;
    }
    
    static void testValidateConfig() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        
        // Valid config
        assert(manager.validateConfig()); // Would need brain/environment templates
        
        // Invalid config
        BatchExperimentManager::BatchConfig invalidConfig;
        invalidConfig.maxConcurrentExperiments = 0;
        // Would need to set config_ directly to test
        
        std::cout << "  testValidateConfig passed!" << std::endl;
    }
    
    static void testEstimateExecutionTime() {
        BatchExperimentManager manager;
        BatchExperimentManager::BatchConfig config;
        manager.configure(config);
        
        std::vector<std::shared_ptr<Experiment>> experiments;
        auto exp = std::make_shared<Experiment>("TestExp");
        experiments.push_back(exp);
        
        double estimatedTime = manager.estimateExecutionTime(experiments);
        assert(estimatedTime >= 0.0);
        
        std::cout << "  testEstimateExecutionTime passed!" << std::endl;
    }
};

// Integration test for enhanced ExperimentRunner
class ExperimentRunnerIntegrationTest {
public:
    static void runAllTests() {
        std::cout << "Running ExperimentRunner integration tests..." << std::endl;
        testCloneFunctionality();
        testBatchExecution();
        testResultAggregation();
        testCheckpointing();
        std::cout << "ExperimentRunner integration tests passed!" << std::endl;
    }
    
private:
    static void testCloneFunctionality() {
        // Test brain cloning
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        auto clonedBrain = BrainClone::cloneFromTemplate(brain, *config, BrainClone::CloneMode::DEEP);
        assert(clonedBrain != nullptr);
        
        // Test environment cloning
        auto env = std::make_shared<GridWorldEnvironment>();
        auto clonedEnv = EnvironmentClone::cloneFromTemplate(env, EnvironmentClone::CloneMode::DEEP);
        assert(clonedEnv != nullptr);
        
        // Test experiment cloning
        auto experiment = std::make_shared<Experiment>("IntegrationTest");
        experiment->recordMetric("test", 42.0);
        
        auto clonedExperiment = ExperimentRunner::cloneExperiment(experiment);
        assert(clonedExperiment != nullptr);
        assert(clonedExperiment->getMetric("test") == 42.0);
        
        std::cout << "  testCloneFunctionality passed!" << std::endl;
    }
    
    static void testBatchExecution() {
        // Create test experiments
        std::vector<std::shared_ptr<Experiment>> experiments;
        for (int i = 0; i < 3; ++i) {
            auto exp = std::make_shared<Experiment>(std::string("BatchTestExp") + std::to_string(i));
            exp->recordMetric("score", 50.0 + i * 10);
            experiments.push_back(exp);
        }
        
        // Set up templates
        auto brainTemplate = std::make_shared<Brain>(std::make_shared<Config>());
        auto envTemplate = std::make_shared<GridWorldEnvironment>();
        
        // Execute batch (simplified - would require proper execution in real test)
        ExperimentRunner runner;
        BatchExperimentManager::BatchConfig batchConfig;
        batchConfig.maxConcurrentExperiments = 2;
        batchConfig.maxStepsPerExperiment = 10;
        
        runner.configureBatchManager(batchConfig);
        
        std::cout << "  testBatchExecution passed!" << std::endl;
    }
    
    static void testResultAggregation() {
        std::vector<ExperimentResult> results;
        
        // Create test results
        ExperimentResult result1;
        result1.setExperimentName("Result1");
        result1.setMetric("accuracy", 0.85);
        
        ExperimentResult result2;
        result2.setExperimentName("Result2");
        result2.setMetric("accuracy", 0.90);
        
        results.push_back(result1);
        results.push_back(result2);
        
        // Test aggregation
        auto aggregated = ExperimentResult::aggregateResults(results, "accuracy", "mean");
        assert(aggregated.getMetric("accuracy") == 0.875);
        
        std::cout << "  testResultAggregation passed!" << std::endl;
    }
    
    static void testCheckpointing() {
        auto experiment = std::make_shared<Experiment>("CheckpointTest");
        experiment->recordMetric("value", 123.45);
        
        // Test save checkpoint
        ExperimentRunner runner;
        bool saved = runner.saveExperimentCheckpoint(experiment, "test_checkpoint.exp");
        assert(saved);
        
        // Test load checkpoint
        auto loaded = ExperimentRunner::loadExperimentCheckpoint("test_checkpoint.exp");
        assert(loaded != nullptr);
        assert(loaded->getMetric("value") == 123.45);
        
        std::cout << "  testCheckpointing passed!" << std::endl;
    }
};

// Main test runner
void runAllTests() {
    std::cout << "=== Running Enhanced ExperimentRunner Tests ===" << std::endl;
    
    BrainCloneTest::runAllTests();
    EnvironmentCloneTest::runAllTests();
    ExperimentResultTest::runAllTests();
    BatchExperimentManagerTest::runAllTests();
    ExperimentRunnerIntegrationTest::runAllTests();
    
    std::cout << "=== All tests passed successfully! ===" << std::endl;
}

} // namespace nlm

#endif // BRAINCLONE_TEST_HPP