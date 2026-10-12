// Research Tools Test Suite
// Tests for ResearchTools experimental control and analysis

#include "ResearchTools.hpp"
#include "Metrics.hpp"
#include "ResearchMetrics.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <setenv.h>
#include <cstdlib>

namespace test_research_tools {
    
    // Test fixtures
    class ResearchToolsTestFixture {
    public:
        std::unique_ptr<nlm::ResearchTools> researchTools_;
        
        ResearchToolsTestFixture() {
            researchTools_ = std::make_unique<nlm::ResearchTools>();
        }
    };
    
    // Experimental Condition Management Tests
    TEST_CASE("ResearchTools Experimental Condition Creation", "[research_tools][conditions]") {
        nlm::ResearchTools tools;
        
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "test_condition";
        condition.description = "A test experimental condition";
        condition.parameters = {{ "learning_rate", 0.01 }, { "neuron_count", 1000 }};
        condition.enabledSystems = { "brain_core", "sensory_processing" };
        condition.disabledSystems = { "development_system" };
        condition.isBaseline = false;
        
        tools.createExperimentalCondition(condition);
        
        auto conditions = tools.getExperimentalConditions();
        REQUIRE(conditions.size() == 1);
        REQUIRE(conditions[0].name == "test_condition");
        REQUIRE(conditions[0].parameters["learning_rate"] == 0.01);
        REQUIRE(conditions[0].parameters["neuron_count"] == 1000);
    }
    
    TEST_CASE("ResearchTools Experimental Condition Execution", "[research_tools][conditions]") {
        nlm::ResearchTools tools;
        
        // Create and run a condition
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "execution_test";
        condition.parameters = {{ "test_param", 42.0 }};
        condition.enabledSystems = { "brain_core" };
        
        tools.createExperimentalCondition(condition);
        REQUIRE(tools.runExperimentalCondition("execution_test") == true);
        
        // Verify current condition is set
        REQUIRE(tools.getCurrentCondition() == "execution_test");
    }
    
    TEST_CASE("ResearchTools Condition Restoration", "[research_tools][conditions]") {
        nlm::ResearchTools tools;
        
        // Create multiple conditions
        nlm::ResearchTools::ExperimentalCondition baseline;
        baseline.name = "baseline";
        baseline.parameters = {{ "learning_rate", 0.01 }};
        baseline.isBaseline = true;
        
        nlm::ResearchTools::ExperimentalCondition experimental;
        experimental.name = "experimental";
        experimental.parameters = {{ "learning_rate", 0.1 }};
        experimental.isBaseline = false;
        
        tools.createExperimentalCondition(baseline);
        tools.createExperimentalCondition(experimental);
        
        // Run experimental condition
        tools.runExperimentalCondition("experimental");
        REQUIRE(tools.getCurrentCondition() == "experimental");
        
        // Restore to baseline
        tools.restoreExperimentalCondition("baseline");
        REQUIRE(tools.getCurrentCondition() == "baseline");
    }
    
    TEST_CASE("ResearchTools Experimental Condition Deletion", "[research_tools][conditions]") {
        nlm::ResearchTools tools;
        
        // Create a condition
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "to_delete";
        condition.parameters = {{ "test_param", 1.0 }};
        
        tools.createExperimentalCondition(condition);
        REQUIRE(tools.getExperimentalConditions().size() == 1);
        
        // Delete it
        REQUIRE(tools.deleteExperimentalCondition("to_delete") == true);
        REQUIRE(tools.getExperimentalConditions().size() == 0);
        
        // Try to delete non-existent condition
        REQUIRE(tools.deleteExperimentalCondition("non_existent") == false);
    }
    
    // Parameter Tuning Tests
    TEST_CASE("ResearchTools Parameter Tuning Configuration", "[research_tools][tuning]") {
        nlm::ResearchTools tools;
        
        nlm::ResearchTools::ParameterTune tune;
        tune.parameterName = "learning_rate";
        tune.initialValue = 0.01;
        tune.minValue = 0.001;
        tune.maxValue = 0.1;
        tune.stepSize = 0.01;
        tune.optimizeForMax = true;
        
        tools.addParameterTune(tune);
        
        auto tuningResults = tools.getParameterTuningResults();
        REQUIRE(tuningResults.empty()); // No results yet
        
        // Run tuning (simplified - just tests the mechanism)
        // In a real implementation, this would actually perform tuning
        REQUIRE(tools.runParameterTuning("learning_rate") == true);
        
        tuningResults = tools.getParameterTuningResults();
        REQUIRE(tuningResults.size() == 1);
        REQUIRE(tuningResults[0].parameterName == "learning_rate");
        REQUIRE(tuningResults[0].tuningCompleted);
    }
    
    TEST_CASE("ResearchTools Parameter Tuning Results", "[research_tools][tuning]") {
        nlm::ResearchTools tools;
        
        // Create a parameter tuning setup
        nlm::ResearchTools::ParameterTune tune;
        tune.parameterName = "plasticity_rate";
        tune.initialValue = 0.1;
        tune.minValue = 0.05;
        tune.maxValue = 0.2;
        tune.stepSize = 0.05;
        tune.optimizeForMax = true;
        
        tools.addParameterTune(tune);
        tools.runParameterTuning("plasticity_rate");
        
        auto results = tools.getParameterTuningResults();
        REQUIRE(results.size() == 1);
        
        auto& result = results[0];
        REQUIRE(result.parameterName == "plasticity_rate");
        REQUIRE(result.testedValues.size() >= 2);
        REQUIRE(result.performanceValues.size() == result.testedValues.size());
        REQUIRE(result.optimalPerformance >= 0.0);
        REQUIRE(result.optimalValue >= result.minValue);
        REQUIRE(result.optimalValue <= result.maxValue);
    }
    
    // Data Comparison Tests
    TEST_CASE("ResearchTools Comparison Data Addition", "[research_tools][comparison]") {
        nlm::ResearchTools tools;
        
        // Add comparison data
        nlm::Metrics metrics1;
        metrics1.record("metric1", 1.0);
        metrics1.record("metric2", 2.0);
        
        nlm::Metrics metrics2;
        metrics2.record("metric1", 1.5);
        metrics2.record("metric2", 2.5);
        
        tools.addComparisonData("condition1", metrics1);
        tools.addComparisonData("condition2", metrics2);
        
        // Generate comparisons
        auto comparisons = tools.compareConditions("condition1", "condition2");
        REQUIRE(comparisons.size() > 0);
        
        // Find metric1 comparison
        bool foundMetric1 = false;
        for (const auto& comp : comparisons) {
            if (comp.metricName == "metric1") {
                foundMetric1 = true;
                REQUIRE(comp.value1 == 1.0);
                REQUIRE(comp.value2 == 1.5);
                REQUIRE(comp.difference == 0.5);
                REQUIRE(comp.relativeDifference == 0.5); // (1.5 - 1.0) / 1.0
                break;
            }
        }
        REQUIRE(foundMetric1);
    }
    
    TEST_CASE("ResearchTools ResearchMetrics Comparison", "[research_tools][comparison]") {
        nlm::ResearchTools tools;
        
        // Add ResearchMetrics comparison data
        nlm::ResearchMetrics metrics1;
        metrics1.record("accuracy", 0.8);
        metrics1.record("f1_score", 0.75);
        
        nlm::ResearchMetrics metrics2;
        metrics2.record("accuracy", 0.85);
        metrics2.record("f1_score", 0.80);
        
        tools.addComparisonData("baseline", metrics1);
        tools.addComparisonData("experimental", metrics2);
        
        auto comparisons = tools.compareConditions("baseline", "experimental");
        REQUIRE(comparisons.size() > 0);
        
        // Verify comparisons
        bool foundAccuracy = false;
        for (const auto& comp : comparisons) {
            if (comp.metricName == "accuracy") {
                foundAccuracy = true;
                REQUIRE(comp.value1 == 0.8);
                REQUIRE(comp.value2 == 0.85);
                REQUIRE(comp.isBetter == true); // Experimental is better
                break;
            }
        }
        REQUIRE(foundAccuracy);
    }
    
    TEST_CASE("ResearchTools Compare All Conditions", "[research_tools][comparison]") {
        nlm::ResearchTools tools;
        
        // Create multiple conditions with metrics
        for (int i = 0; i < 5; ++i) {
            nlm::Metrics metrics;
            metrics.record("metric", i * 0.1);
            tools.addComparisonData("condition_" + std::to_string(i), metrics);
        }
        
        // Compare all conditions
        auto comparisons = tools.compareAllConditions();
        REQUIRE(comparisons.size() > 0);
        
        // With 5 conditions, we should have 10 pairwise comparisons (5 choose 2)
        REQUIRE(comparisons.size() == 10);
    }
    
    // Statistical Tests
    TEST_CASE("ResearchTools Statistical Test Management", "[research_tools][stats]") {
        nlm::ResearchTools tools;
        
        // Add statistical test results
        nlm::ResearchTools::StatisticalTestResult tTest;
        tTest.testName = "t_test_example";
        tTest.testType = "independent_samples";
        tTest.statistic = 2.5;
        tTest.pValue = 0.015;
        tTest.isSignificant = true;
        tTest.interpretation = "Statistically significant difference between groups";
        tTest.recommendation = "Report mean differences";
        
        tools.addStatisticalTest(tTest);
        
        auto statsResults = tools.getStatisticalTestResults();
        REQUIRE(statsResults.size() == 1);
        REQUIRE(statsResults[0].testName == "t_test_example");
        REQUIRE(statsResults[0].isSignificant);
        REQUIRE(statsResults[0].pValue == 0.015);
    }
    
    TEST_CASE("ResearchTools Statistical Tests Execution", "[research_tools][stats]") {
        nlm::ResearchTools tools;
        
        // Add statistical tests
        nlm::ResearchTools::StatisticalTestResult test1;
        test1.testName = "test1";
        test1.testType = "correlation";
        test1.statistic = 0.8;
        test1.pValue = 0.05;
        test1.isSignificant = true;
        
        nlm::ResearchTools::StatisticalTestResult test2;
        test2.testName = "test2";
        test2.testType = "anova";
        test2.statistic = 3.2;
        test2.pValue = 0.01;
        test2.isSignificant = true;
        
        tools.addStatisticalTest(test1);
        tools.addStatisticalTest(test2);
        
        // Run statistical tests (returns existing results)
        auto results = tools.runStatisticalTests();
        REQUIRE(results.size() == 2);
    }
    
    TEST_CASE("ResearchTools Statistical Test Clearing", "[research_tools][stats]") {
        nlm::ResearchTools tools;
        
        // Add tests
        nlm::ResearchTools::StatisticalTestResult test;
        test.testName = "test_to_clear";
        test.testType = "correlation";
        test.statistic = 1.0;
        test.pValue = 0.05;
        test.isSignificant = true;
        
        tools.addStatisticalTest(test);
        REQUIRE(tools.getStatisticalTestResults().size() == 1);
        
        // Clear results
        REQUIRE(tools.clearStatisticalTestResults() == true);
        REQUIRE(tools.getStatisticalTestResults().size() == 0);
    }
    
    // Ablation Studies Tests
    TEST_CASE("ResearchTools Ablation Configuration", "[research_tools][ablation]") {
        nlm::ResearchTools tools;
        
        // Configure ablation
        nlm::ResearchTools::AblationConfig ablation;
        ablation.systemName = "development_system";
        ablation.isAblated = true;
        ablation.ablationMethod = "complete_removal";
        
        tools.configureAblation(ablation);
        
        auto ablations = tools.getAblationConfigs();
        REQUIRE(ablations.size() == 1);
        REQUIRE(ablations[0].systemName == "development_system");
        REQUIRE(ablations[0].isAblated);
        REQUIRE(ablations[0].ablationMethod == "complete_removal");
    }
    
    TEST_CASE("ResearchTools Ablation Control", "[research_tools][ablation]") {
        nlm::ResearchTools tools;
        
        // Configure and enable ablation
        nlm::ResearchTools::AblationConfig ablation;
        ablation.systemName = "memory_system";
        ablation.isAblated = false;
        ablation.ablationMethod = "deactivation";
        
        tools.configureAblation(ablation);
        
        // Enable ablation
        REQUIRE(tools.enableAblation("memory_system") == true);
        
        auto ablations = tools.getAblationConfigs();
        REQUIRE(ablations.size() == 1);
        REQUIRE(ablations[0].isAblated);
        
        // Disable ablation
        REQUIRE(tools.disableAblation("memory_system") == true);
        REQUIRE(ablations[0].isRestored);
    }
    
    TEST_CASE("ResearchTools Restore All Ablations", "[research_tools][ablation]") {
        nlm::ResearchTools tools;
        
        // Configure multiple ablations
        for (int i = 0; i < 3; ++i) {
            nlm::ResearchTools::AblationConfig ablation;
            ablation.systemName = "system_" + std::to_string(i);
            ablation.isAblated = true;
            ablation.ablationMethod = "test_method";
            
            tools.configureAblation(ablation);
        }
        
        auto ablations = tools.getAblationConfigs();
        REQUIRE(ablations.size() == 3);
        REQUIRE(ablations[0].isAblated);
        
        // Restore all ablations
        REQUIRE(tools.restoreAllAblations() == true);
        
        for (const auto& ablation : ablations) {
            REQUIRE(!ablation.isAblated);
            REQUIRE(ablation.isRestored);
        }
    }
    
    // Export/Import Tests
    TEST_CASE("ResearchTools Results Export", "[research_tools][export]") {
        nlm::ResearchTools tools;
        
        // Add some test data
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "export_test";
        condition.parameters = {{ "param1", 1.0 }, { "param2", 2.0 }};
        
        tools.createExperimentalCondition(condition);
        
        nlm::ResearchTools::AblationConfig ablation;
        ablation.systemName = "test_system";
        ablation.isAblated = true;
        ablation.ablationMethod = "test_method";
        
        tools.configureAblation(ablation);
        
        // Export results
        REQUIRE(tools.exportResults("/tmp/research_tools_export.json") == true);
    }
    
    TEST_CASE("ResearchTools Results Import", "[research_tools][import]") {
        nlm::ResearchTools tools;
        
        // Create and export test data
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "import_test";
        condition.parameters = {{ "learning_rate", 0.01 }};
        condition.isBaseline = true;
        
        tools.createExperimentalCondition(condition);
        tools.exportResults("/tmp/research_tools_import_test.json");
        
        // Clear and import
        tools.reset();
        REQUIRE(tools.getExperimentalConditions().size() == 0);
        
        REQUIRE(tools.importResults("/tmp/research_tools_import_test.json") == true);
        REQUIRE(tools.getExperimentalConditions().size() == 1);
        REQUIRE(tools.getExperimentalConditions()[0].name == "import_test");
    }
    
    TEST_CASE("ResearchTools Conditions Export/Import", "[research_tools][conditions_export]") {
        nlm::ResearchTools tools;
        
        // Create and export conditions
        nlm::ResearchTools::ExperimentalCondition condition1;
        condition1.name = "condition1";
        condition1.description = "First test condition";
        condition1.parameters = {{ "param1", 1.0 }};
        condition1.isBaseline = true;
        
        nlm::ResearchTools::ExperimentalCondition condition2;
        condition2.name = "condition2";
        condition2.description = "Second test condition";
        condition2.parameters = {{ "param2", 2.0 }};
        condition2.isBaseline = false;
        
        tools.createExperimentalCondition(condition1);
        tools.createExperimentalCondition(condition2);
        
        // Export conditions
        REQUIRE(tools.exportConditions("/tmp/research_tools_conditions.txt") == true);
        
        // Clear and import
        tools.reset();
        REQUIRE(tools.getExperimentalConditions().size() == 0);
        
        REQUIRE(tools.importConditions("/tmp/research_tools_conditions.txt") == true);
        REQUIRE(tools.getExperimentalConditions().size() == 2);
    }
    
    // Configuration Tests
    TEST_CASE("ResearchTools Configuration Management", "[research_tools][config]") {
        nlm::ResearchTools tools;
        
        // Test default parameters
        auto params = tools.getCurrentParameters();
        REQUIRE(params.find("neuron_count") != params.end());
        REQUIRE(params.find("max_steps") != params.end());
        
        // Set custom parameters
        tools.setParameter("custom_param", 42.0);
        tools.setParameter("another_param", 3.14);
        
        params = tools.getCurrentParameters();
        REQUIRE(params["custom_param"] == 42.0);
        REQUIRE(params["another_param"] == 3.14);
    }
    
    TEST_CASE("ResearchTools Parameter File Operations", "[research_tools][param_file]") {
        nlm::ResearchTools tools;
        
        // Set some parameters
        tools.setParameter("learning_rate", 0.05);
        tools.setParameter("plasticity_rate", 0.2);
        
        // Save to file
        REQUIRE(tools.saveParametersToFile("/tmp/research_tools_params.json") == true);
        
        // Reset parameters
        tools.reset();
        
        // Load from file
        REQUIRE(tools.loadParametersFromFile("/tmp/research_tools_params.json") == true);
        
        auto params = tools.getCurrentParameters();
        REQUIRE(params["learning_rate"] == 0.05);
        REQUIRE(params["plasticity_rate"] == 0.2);
    }
    
    TEST_CASE("ResearchTools System Control", "[research_tools][system_control]") {
        nlm::ResearchTools tools;
        
        // Test system mode controls
        REQUIRE(!tools.isResearchMode());
        REQUIRE(!tools.isDebugMode());
        REQUIRE(!tools.areSystemsPaused());
        
        // Enable research mode
        tools.enableDebugMode(true);
        tools.setResearchMode(true);
        tools.pauseAllSystems(true);
        
        REQUIRE(tools.isDebugMode());
        REQUIRE(tools.isResearchMode());
        REQUIRE(tools.areSystemsPaused());
        
        // Resume systems
        tools.pauseAllSystems(false);
        REQUIRE(!tools.areSystemsPaused());
    }
    
    TEST_CASE("ResearchTools Session Tracking", "[research_tools][session]") {
        nlm::ResearchTools tools;
        
        // Get session information
        auto startTime = tools.getSessionStartTime();
        REQUIRE(startTime.time_since_epoch().count() > 0);
        
        auto duration = tools.getSessionDuration();
        REQUIRE(duration >= 0.0);
        
        auto activeSystems = tools.getActiveSystems();
        REQUIRE(!activeSystems.empty());
        
        // Should have default systems
        bool hasBrainCore = false;
        for (const auto& system : activeSystems) {
            if (system.find("brain") != std::string::npos) {
                hasBrainCore = true;
                break;
            }
        }
        REQUIRE(hasBrainCore);
    }
    
    TEST_CASE("ResearchTools Reset Functionality", "[research_tools][reset]") {
        nlm::ResearchTools tools;
        
        // Add some data
        nlm::ResearchTools::ExperimentalCondition condition;
        condition.name = "test_condition";
        condition.parameters = {{ "param1", 1.0 }};
        
        tools.createExperimentalCondition(condition);
        
        nlm::ResearchTools::AblationConfig ablation;
        ablation.systemName = "test_system";
        ablation.isAblated = true;
        
        tools.configureAblation(ablation);
        
        // Reset should clear everything
        tools.reset();
        
        REQUIRE(tools.getExperimentalConditions().empty());
        REQUIRE(tools.getAblationConfigs().empty());
        REQUIRE(tools.getCurrentCondition().empty());
        REQUIRE(tools.getBaselineCondition().empty());
    }
    
    TEST_CASE("ResearchTools Integration with Other Systems", "[research_tools][integration]") {
        nlm::ResearchTools tools;
        
        // Test integration methods exist and don't crash
        nlm::Metrics metrics;
        metrics.record("test", 1.0);
        
        tools.addComparisonData("baseline", metrics);
        tools.integrateWithMetrics(metrics);
        
        nlm::ResearchMetrics researchMetrics;
        researchMetrics.record("research", 2.0);
        
        tools.addComparisonData("research", researchMetrics);
        tools.integrateWithResearchMetrics(researchMetrics);
        
        // Generate performance report
        auto performanceReport = tools.generatePerformanceReport();
        // Should be empty since we have no comparison data yet
        // but should not crash
        
        // Generate learning report
        auto learningReport = tools.generateLearningReport();
        
        REQUIRE(true); // Placeholder
    }
    
    // Performance benchmarks
    BENCHMARK("ResearchTools Experimental Condition Management", "[research_tools][benchmark]") {
        nlm::ResearchTools tools;
        
        for (int i = 0; i < 100; ++i) {
            nlm::ResearchTools::ExperimentalCondition condition;
            condition.name = "condition_" + std::to_string(i);
            condition.parameters = {{ "param" + std::to_string(i), i * 0.1 }};
            condition.enabledSystems = { "brain_core", "sensory_processing" };
            
            tools.createExperimentalCondition(condition);
        }
        
        return tools.getExperimentalConditions().size();
    }
    
    BENCHMARK("ResearchTools Parameter Tuning Performance", "[research_tools][benchmark]") {
        nlm::ResearchTools tools;
        
        nlm::ResearchTools::ParameterTune tune;
        tune.parameterName = "learning_rate";
        tune.initialValue = 0.01;
        tune.minValue = 0.001;
        tune.maxValue = 0.1;
        tune.stepSize = 0.01;
        
        tools.addParameterTune(tune);
        tools.runParameterTuning("learning_rate");
        
        auto results = tools.getParameterTuningResults();
        return results.size();
    }
    
} // namespace test_research_tools