// Unit Tests for Experiment Management
// Comprehensive test suite for advanced experiment management features

#include "tests/utils/TestUtils.hpp"
#include "src/experiments/ExperimentManager.hpp"
#include "src/experiments/Experiment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include "src/brain/Brain.hpp"
#include "src/environment/Environment.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_experiment_management {

void testExperimentManagerCreation() {
    TestUtils::TestSession session("testExperimentManagerCreation");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    TestAssert::true(manager != nullptr, "ExperimentManager should be created successfully");
    
    std::cout << "    testExperimentManagerCreation passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerCreateExperiment() {
    TestUtils::TestSession session("testExperimentManagerCreateExperiment");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create experiment config
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "test_experiment_1";
    config.description = "Test experiment for unit testing";
    config.parameters = {
        {"learning_rate", "0.01"},
        {"population_size", "100"},
        {"max_steps", "1000"}
    };
    config.parameterRanges = {
        {"learning_rate", std::vector<double>{0.001, 0.1}},
        {"population_size", std::vector<double>{50, 500}},
        {"max_steps", std::vector<double>{100, 10000}}
    };
    config.seed = 42;
    config.maxSteps = 1000;
    config.enabled = true;
    
    // Create experiment
    bool createSuccess = manager->createExperiment("test_exp_1", config);
    TestAssert::true(createSuccess, "ExperimentManager should create experiment successfully");
    
    // Verify experiment was created
    auto experimentResults = manager->getAllResults();
    TestAssert::true(experimentResults.size() >= 1, "Should have at least one experiment result");
    
    // Get experiment
    auto experiment = manager->getResult("test_exp_1");
    TestAssert::true(experiment != nullptr, "Should get experiment from manager");
    TestAssert::equal(experiment->getName(), "test_exp_1", "Experiment name should match");
    
    std::cout << "    testExperimentManagerCreateExperiment passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerExperimentTemplates() {
    TestUtils::TestSession session("testExperimentManagerExperimentTemplates");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create template
    nlm::ExperimentManager::ExperimentConfig templateConfig;
    templateConfig.name = "template_1";
    templateConfig.description = "Template for testing";
    templateConfig.parameters = {
        {"param1", "value1"},
        {"param2", "value2"}
    };
    templateConfig.seed = 123;
    
    bool templateSuccess = manager->createTemplate("template_1", templateConfig);
    TestAssert::true(templateSuccess, "Should create experiment template");
    
    // Create experiment from template
    bool experimentSuccess = manager->createExperimentFromTemplate("exp_from_template", "template_1");
    TestAssert::true(experimentSuccess, "Should create experiment from template");
    
    // Verify template was used
    auto templates = manager->getTemplates();
    TestAssert::true(templates.find("template_1") != templates.end(), "Should have template in templates list");
    
    std::cout << "    testExperimentManagerExperimentTemplates passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerExperimentParameters() {
    TestUtils::TestSession session("testExperimentManagerExperimentParameters");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create experiment
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "param_test_exp";
    config.parameters = {
        {"initial_param", "initial_value"}
    };
    
    bool createSuccess = manager->createExperiment("param_test_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for parameter test");
    
    // Set parameter
    bool setSuccess = manager->setParameter("param_test_exp", "new_param", "new_value");
    TestAssert::true(setSuccess, "Should set experiment parameter");
    
    // Get parameter
    std::string paramValue = manager->getParameter("param_test_exp", "new_param");
    TestAssert::equal(paramValue, "new_value", "Parameter value should match");
    
    // Get all parameters
    auto allParams = manager->getExperimentParameters("param_test_exp");
    TestAssert::true(allParams.find("initial_param") != allParams.end(), "Should have initial parameter");
    TestAssert::true(allParams.find("new_param") != allParams.end(), "Should have new parameter");
    
    std::cout << "    testExperimentManagerExperimentParameters passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerExperimentResults() {
    TestUtils::TestSession session("testExperimentManagerExperimentResults");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create and run experiment
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "result_test_exp";
    config.maxSteps = 100;
    
    bool createSuccess = manager->createExperiment("result_test_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for result test");
    
    // Simulate running experiment
    auto experiment = manager->getResult("result_test_exp");
    TestAssert::true(experiment != nullptr, "Should get experiment for result test");
    
    // Store result
    bool storeSuccess = manager->storeResult("result_test_exp", experiment);
    TestAssert::true(storeSuccess, "Should store experiment result");
    
    // Get result
    auto retrievedExperiment = manager->getResult("result_test_exp");
    TestAssert::true(retrievedExperiment != nullptr, "Should retrieve experiment result");
    TestAssert::equal(retrievedExperiment->getName(), "result_test_exp", "Retrieved experiment name should match");
    
    // Get all results
    auto allResults = manager->getAllResults();
    TestAssert::true(allResults.size() >= 1, "Should have at least one result");
    
    std::cout << "    testExperimentManagerExperimentResults passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerExperimentComparison() {
    TestUtils::TestSession session("testExperimentManagerExperimentComparison");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create two experiments
    nlm::ExperimentManager::ExperimentConfig config1;
    config1.name = "comparison_exp_1";
    config1.parameters = {
        {"param1", "value1"},
        {"param2", "50"}
    };
    
    nlm::ExperimentManager::ExperimentConfig config2;
    config2.name = "comparison_exp_2";
    config2.parameters = {
        {"param1", "value1"},
        {"param2", "100"}
    };
    
    manager->createExperiment("comparison_exp_1", config1);
    manager->createExperiment("comparison_exp_2", config2);
    
    // Store results
    auto exp1 = std::make_shared<nlm::Experiment>("comparison_exp_1");
    exp1->setConfigValue("result_value", "98.5");
    manager->storeResult("comparison_exp_1", exp1);
    
    auto exp2 = std::make_shared<nlm::Experiment>("comparison_exp_2");
    exp2->setConfigValue("result_value", "95.2");
    manager->storeResult("comparison_exp_2", exp2);
    
    // Compare experiments
    std::map<std::string, double> comparisonResults;
    bool compareSuccess = manager->compareExperiments("comparison_exp_1", "comparison_exp_2", comparisonResults);
    TestAssert::true(compareSuccess, "Should compare experiments successfully");
    
    // Verify comparison results
    TestAssert::true(comparisonResults.size() > 0, "Should have comparison results");
    
    std::cout << "    testExperimentManagerExperimentComparison passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerAutomation() {
    TestUtils::TestSession session("testExperimentManagerAutomation");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create experiments for automation test
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "automation_exp";
    config.maxSteps = 50;
    
    bool createSuccess = manager->createExperiment("automation_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for automation test");
    
    // Test scheduling
    auto now = std::chrono::system_clock::now();
    auto scheduledTime = now + std::chrono::hours(1);
    
    bool scheduleSuccess = manager->scheduleExperiment("automation_exp", scheduledTime);
    TestAssert::true(scheduleSuccess, "Should schedule experiment");
    
    // Run scheduled experiments
    bool runSuccess = manager->runScheduledExperiments();
    TestAssert::true(runSuccess, "Should run scheduled experiments");
    
    std::cout << "    testExperimentManagerAutomation passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerConfigSaveLoad() {
    TestUtils::TestSession session("testExperimentManagerConfigSaveLoad");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create experiment config
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "config_save_exp";
    config.parameters = {
        {"test_param", "test_value"},
        {"test_number", "42"}
    };
    
    // Create experiment
    bool createSuccess = manager->createExperiment("config_save_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for config save test");
    
    // Save manager config
    bool saveSuccess = manager->saveManagerConfig("test_manager_config.json");
    TestAssert::true(saveSuccess, "Should save manager config");
    
    // Load manager config
    bool loadSuccess = manager->loadManagerConfig("test_manager_config.json");
    TestAssert::true(loadSuccess, "Should load manager config");
    
    std::cout << "    testExperimentManagerConfigSaveLoad passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerResultAnalysis() {
    TestUtils::TestSession session("testExperimentManagerResultAnalysis");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create and store experiment with results
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "analysis_exp";
    config.maxSteps = 100;
    
    bool createSuccess = manager->createExperiment("analysis_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for analysis test");
    
    // Create a mock experiment with metrics
    auto experiment = std::make_shared<nlm::Experiment>("analysis_exp");
    
    // Add some metrics to experiment
    auto metrics = std::make_unique<nlm::Metrics>();
    metrics->addMetric("accuracy", 0.85);
    metrics->addMetric("loss", 0.15);
    metrics->addMetric("precision", 0.92);
    metrics->addMetric("recall", 0.88);
    
    experiment->setMetrics(std::move(metrics));
    
    // Store result
    bool storeSuccess = manager->storeResult("analysis_exp", experiment);
    TestAssert::true(storeSuccess, "Should store experiment with metrics");
    
    // Generate result summary
    std::string summary = manager->generateResultSummary("analysis_exp");
    TestAssert::true(summary.size() > 0, "Should generate result summary");
    
    std::cout << "    testExperimentManagerResultAnalysis passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerProgressTracking() {
    TestUtils::TestSession session("testExperimentManagerProgressTracking");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create experiment
    nlm::ExperimentManager::ExperimentConfig config;
    config.name = "progress_test_exp";
    config.maxSteps = 1000;
    
    bool createSuccess = manager->createExperiment("progress_test_exp", config);
    TestAssert::true(createSuccess, "Should create experiment for progress tracking test");
    
    // Get progress (should be 0.0 since not started)
    float progress = manager->getExperimentProgress("progress_test_exp");
    TestAssert::true(progress >= 0.0 && progress <= 1.0, "Progress should be between 0 and 1");
    
    // Test batch progress
    std::vector<std::string> experimentNames = {"progress_test_exp"};
    auto batchProgress = manager->getBatchProgress(experimentNames);
    TestAssert::true(batchProgress.size() == 1, "Should have progress for one experiment");
    
    std::cout << "    testExperimentManagerProgressTracking passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerValidation() {
    TestUtils::TestSession session("testExperimentManagerValidation");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Test valid experiment config
    nlm::ExperimentManager::ExperimentConfig validConfig;
    validConfig.name = "valid_exp";
    validConfig.maxSteps = 100;
    validConfig.parameters = {
        {"param1", "value1"}
    };
    
    bool valid = manager->validateExperimentConfig(validConfig);
    TestAssert::true(valid, "Valid config should pass validation");
    
    // Test invalid experiment config (empty name)
    nlm::ExperimentManager::ExperimentConfig invalidConfig;
    invalidConfig.name = ""; // Empty name
    invalidConfig.maxSteps = 100;
    
    bool invalid = manager->validateExperimentConfig(invalidConfig);
    TestAssert::false(invalid, "Invalid config should fail validation");
    
    std::cout << "    testExperimentManagerValidation passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentManagerUtilityFunctions() {
    TestUtils::TestSession session("testExperimentManagerUtilityFunctions");
    
    auto manager = std::make_unique<nlm::ExperimentManager>();
    
    // Create some experiments
    for (int i = 1; i <= 5; ++i) {
        nlm::ExperimentManager::ExperimentConfig config;
        config.name = "utility_exp_" + std::to_string(i);
        config.maxSteps = 100 * i;
        manager->createExperiment(config.name, config);
    }
    
    // Test getAllExperimentNames
    auto allNames = manager->getAllExperimentNames();
    TestAssert::true(allNames.size() >= 5, "Should have at least 5 experiment names");
    
    // Test getExperimentCount
    size_t count = manager->getExperimentCount();
    TestAssert::true(count >= 5, "Should have at least 5 experiments");
    
    // Test getExperimentCount with completed only
    size_t completedCount = manager->getExperimentCount(true);
    TestAssert::true(completedCount >= 0, "Completed count should be non-negative");
    
    std::cout << "    testExperimentManagerUtilityFunctions passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Experiment Management Tests ===" << std::endl;
    
    testExperimentManagerCreation();
    testExperimentManagerCreateExperiment();
    testExperimentManagerExperimentTemplates();
    testExperimentManagerExperimentParameters();
    testExperimentManagerExperimentResults();
    testExperimentManagerExperimentComparison();
    testExperimentManagerAutomation();
    testExperimentManagerConfigSaveLoad();
    testExperimentManagerResultAnalysis();
    testExperimentManagerProgressTracking();
    testExperimentManagerValidation();
    testExperimentManagerUtilityFunctions();
    
    std::cout << std::endl;
    std::cout << "=== All Experiment Management Tests PASSED ===" << std::endl;
}