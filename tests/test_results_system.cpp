#include "../src/experiments/ResultsSystem.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>

namespace nlm {

class ExperimentResultsSystemTest {
public:
    ExperimentResultsSystemTest() {
        // Initialize logging
        NLM_LOG_INFO("=== Starting Experiment Results System Tests ===");
    }
    
    void runAllTests() {
        testCollectResults();
        testAnalysisCalculation();
        testJSONExportImport();
        testCSVExport();
        testTextExport();
        testBinaryExport();
        testResultsComparison();
        testBatchResultsAggregation();
        testVisualizationGeneration();
        testSummaryReportGeneration();
        testFileOperations();
        
        NLM_LOG_INFO("=== All tests passed successfully ===");
    }
    
private:
    void testCollectResults() {
        NLM_LOG_INFO("Testing collectResults...");
        
        // Create a test experiment
        auto experiment = std::make_shared<Experiment>("TestExperiment");
        experiment->setSeed(12345);
        experiment->setStartStep(0);
        experiment->setEndStep(100);
        
        // Add some step data
        Experiment::StepData step1;
        step1.step = 0;
        step1.neuronCount = 100;
        step1.synapseCount = 1000;
        step1.firingCount = 50;
        step1.avgFiringRate = 0.5f;
        
        Experiment::StepData step2;
        step2.step = 50;
        step2.neuronCount = 105;
        step2.synapseCount = 1050;
        step2.firingCount = 60;
        step2.avgFiringRate = 0.6f;
        
        experiment->recordStep(step1.step, step1.neuronCount, step1.synapseCount,
                              step1.firingCount, step1.avgFiringRate);
        experiment->recordStep(step2.step, step2.neuronCount, step2.synapseCount,
                              step2.firingCount, step2.avgFiringRate);
        
        // Add a metric
        experiment->recordMetric("total_reward", 42.5f);
        experiment->recordMetric("energy", 10.0f);
        
        // Test collectResults
        ComprehensiveExperimentResult result = ExperimentResultsSystem::collectResults(experiment);
        
        assert(result.experimentName == "TestExperiment");
        assert(result.seed == 12345);
        assert(result.startStep == 0);
        assert(result.endStep == 100);
        assert(result.stepData.size() == 2);
        assert(result.stepData[0].step == 0);
        assert(result.stepData[0].neuronCount == 100);
        assert(result.stepData[1].step == 50);
        assert(result.stepData[1].avgFiringRate == 0.6f);
        assert(result.metrics.size() == 2);
        assert(result.experimentType == "Generic");
        
        NLM_LOG_INFO("✓ collectResults test passed");
    }
    
    void testAnalysisCalculation() {
        NLM_LOG_INFO("Testing analysis calculation...");
        
        ComprehensiveExperimentResult result;
        
        // Add step data
        Experiment::StepData step1;
        step1.step = 0;
        step1.neuronCount = 100;
        step1.synapseCount = 1000;
        step1.firingCount = 50;
        step1.avgFiringRate = 0.5f;
        
        Experiment::StepData step2;
        step2.step = 1;
        step2.neuronCount = 105;
        step2.synapseCount = 1050;
        step2.firingCount = 60;
        step2.avgFiringRate = 0.6f;
        
        Experiment::StepData step3;
        step3.step = 2;
        step3.neuronCount = 110;
        step3.synapseCount = 1100;
        step3.firingCount = 70;
        step3.avgFiringRate = 0.7f;
        
        result.stepData = {step1, step2, step3};
        result.endStep = 2;
        result.startStep = 0;
        
        // Test analysis calculation
        ExperimentResultsSystem::analyzeResults(result);
        
        assert(result.analysis.meanFiringRate == (0.5f + 0.6f + 0.7f) / 3.0f);
        assert(result.analysis.minFiringRate == 0.5f);
        assert(result.analysis.maxFiringRate == 0.7f);
        assert(std::abs(result.analysis.stdDevFiringRate - 0.08164966f) < 0.01f);
        
        NLM_LOG_INFO("✓ analysis calculation test passed");
    }
    
    void testJSONExport() {
        NLM_LOG_INFO("Testing JSON export...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "JSONTestExperiment";
        result.seed = 12345;
        result.experimentType = "Phase3";
        result.startStep = 0;
        result.endStep = 10;
        
        // Add step data
        Experiment::StepData step;
        step.step = 0;
        step.neuronCount = 100;
        step.synapseCount = 1000;
        step.firingCount = 50;
        step.avgFiringRate = 0.5f;
        result.stepData.push_back(step);
        
        // Add metrics
        result.metrics.push_back({"total_reward", 42.5f});
        result.metrics.push_back({"energy", 10.0f});
        
        // Test JSON export
        std::string jsonPath = "/tmp/test_experiment.json";
        bool success = ExperimentResultsSystem::exportToJSON(result, jsonPath);
        
        assert(success);
        
        // Verify file exists and can be read
        std::ifstream file(jsonPath);
        assert(file.good());
        file.close();
        
        // Test file cleanup
        std::filesystem::remove(jsonPath);
        
        NLM_LOG_INFO("✓ JSON export test passed");
    }
    
    void testCSVExport() {
        NLM_LOG_INFO("Testing CSV export...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "CSVTestExperiment";
        result.seed = 54321;
        result.experimentType = "Phase4";
        result.startStep = 0;
        result.endStep = 5;
        
        // Add step data
        for (int i = 0; i < 5; ++i) {
            Experiment::StepData step;
            step.step = i;
            step.neuronCount = 100 + i * 10;
            step.synapseCount = 1000 + i * 100;
            step.firingCount = 50 + i * 5;
            step.avgFiringRate = 0.5f + i * 0.05f;
            result.stepData.push_back(step);
        }
        
        result.metrics.push_back({"total_reward", 100.0f});
        result.metrics.push_back({"energy", 20.0f});
        
        // Test CSV export
        std::string csvPath = "/tmp/test_experiment.csv";
        bool success = ExperimentResultsSystem::exportToCSV(result, csvPath);
        
        assert(success);
        
        // Verify file exists and contains data
        std::ifstream file(csvPath);
        assert(file.good());
        
        std::string line;
        std::getline(file, line); // header
        assert(line.find("Metric") != std::string::npos);
        
        file.close();
        std::filesystem::remove(csvPath);
        
        NLM_LOG_INFO("✓ CSV export test passed");
    }
    
    void testTextExport() {
        NLM_LOG_INFO("Testing text export...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "TextTestExperiment";
        result.seed = 99999;
        result.experimentType = "Phase6";
        result.startStep = 0;
        result.endStep = 3;
        
        // Add step data
        for (int i = 0; i < 3; ++i) {
            Experiment::StepData step;
            step.step = i;
            step.neuronCount = 200 + i * 20;
            step.synapseCount = 2000 + i * 200;
            step.firingCount = 100 + i * 10;
            step.avgFiringRate = 1.0f + i * 0.1f;
            result.stepData.push_back(step);
        }
        
        result.metrics.push_back({"total_reward", 75.0f});
        
        // Test text export
        std::string textPath = "/tmp/test_experiment.txt";
        bool success = ExperimentResultsSystem::exportToText(result, textPath);
        
        assert(success);
        
        // Verify file exists and contains expected content
        std::ifstream file(textPath);
        assert(file.good());
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        assert(content.find("NLM EXPERIMENT RESULTS REPORT") != std::string::npos);
        assert(content.find("TextTestExperiment") != std::string::npos);
        assert(content.find("Phase6") != std::string::npos);
        
        file.close();
        std::filesystem::remove(textPath);
        
        NLM_LOG_INFO("✓ Text export test passed");
    }
    
    void testBinaryExport() {
        NLM_LOG_INFO("Testing binary export...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "BinaryTestExperiment";
        result.seed = 77777;
        result.experimentType = "Generic";
        result.startStep = 0;
        result.endStep = 2;
        
        // Add step data
        for (int i = 0; i < 2; ++i) {
            Experiment::StepData step;
            step.step = i;
            step.neuronCount = 300 + i * 30;
            step.synapseCount = 3000 + i * 300;
            step.firingCount = 150 + i * 15;
            step.avgFiringRate = 1.5f + i * 0.15f;
            result.stepData.push_back(step);
        }
        
        result.metrics.push_back({"total_reward", 200.0f});
        
        // Test binary export
        std::string binaryPath = "/tmp/test_experiment.bin";
        bool success = ExperimentResultsSystem::exportToBinary(result, binaryPath);
        
        assert(success);
        
        // Verify file exists and has reasonable size
        std::ifstream file(binaryPath, std::ios::binary);
        assert(file.good());
        
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        assert(size > 0); // Should have some data
        
        file.close();
        std::filesystem::remove(binaryPath);
        
        NLM_LOG_INFO("✓ Binary export test passed");
    }
    
    void testResultsComparison() {
        NLM_LOG_INFO("Testing results comparison...");
        
        // Create two test results
        ComprehensiveExperimentResult result1;
        result1.experimentName = "Experiment1";
        result1.analysis.meanFiringRate = 0.5f;
        result1.analysis.meanReward = 10.0f;
        result1.analysis.temporalStability = 0.8f;
        result1.analysis.timeEfficiency = 5.0f;
        
        ComprehensiveExperimentResult result2;
        result2.experimentName = "Experiment2";
        result2.analysis.meanFiringRate = 0.7f;
        result2.analysis.meanReward = 15.0f;
        result2.analysis.temporalStability = 0.9f;
        result2.analysis.timeEfficiency = 8.0f;
        
        // Test comparison
        ComprehensiveExperimentResult::ComparisonResults comparison;
        bool success = ExperimentResultsSystem::compareResults(result1, result2, comparison);
        
        assert(success);
        assert(comparison.performanceScore > 0.0f);
        assert(comparison.learningRate != 0.0f);
        assert(comparison.efficiencyScore > 0.0f);
        assert(comparison.normalizedMetrics.size() > 0);
        
        NLM_LOG_INFO("✓ Results comparison test passed");
    }
    
    void testBatchResultsAggregation() {
        NLM_LOG_INFO("Testing batch results aggregation...");
        
        // Create multiple test experiments
        std::vector<std::shared_ptr<Experiment>> experiments;
        
        for (int i = 0; i < 3; ++i) {
            auto experiment = std::make_shared<Experiment>("BatchExperiment" + std::to_string(i));
            experiment->setSeed(1000 + i);
            
            // Add some step data
            for (int j = 0; j < 2; ++j) {
                Experiment::StepData step;
                step.step = j;
                step.neuronCount = 100 + i * 50 + j * 10;
                step.synapseCount = 1000 + i * 500 + j * 100;
                step.firingCount = 50 + i * 25 + j * 5;
                step.avgFiringRate = 0.5f + i * 0.1f + j * 0.05f;
                experiment->recordStep(step.step, step.neuronCount, step.synapseCount,
                                      step.firingCount, step.avgFiringRate);
            }
            
            experiment->recordMetric("total_reward", static_cast<double>(i * 10 + 5));
            experiments.push_back(experiment);
        }
        
        // Test batch aggregation
        std::vector<ComprehensiveExperimentResult> aggregatedResults = 
            ExperimentResultsSystem::aggregateBatchResults(experiments);
        
        assert(aggregatedResults.size() == 3);
        
        // Verify aggregation
        for (size_t i = 0; i < aggregatedResults.size(); ++i) {
            assert(aggregatedResults[i].experimentName == "BatchExperiment" + std::to_string(i));
            assert(aggregatedResults[i].seed == 1000 + i);
        }
        
        NLM_LOG_INFO("✓ Batch results aggregation test passed");
    }
    
    void testVisualizationGeneration() {
        NLM_LOG_INFO("Testing visualization generation...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "VisTestExperiment";
        result.seed = 88888;
        result.experimentType = "Phase3";
        result.startStep = 0;
        result.endStep = 4;
        
        // Add step data
        for (int i = 0; i < 4; ++i) {
            Experiment::StepData step;
            step.step = i;
            step.neuronCount = 150 + i * 15;
            step.synapseCount = 1500 + i * 150;
            step.firingCount = 75 + i * 7;
            step.avgFiringRate = 0.75f + i * 0.05f;
            result.stepData.push_back(step);
        }
        
        result.metrics.push_back({"total_reward", 50.0f});
        result.analysis.meanFiringRate = 0.8f;
        result.analysis.meanReward = 60.0f;
        
        // Test visualization generation
        std::string vizPath = "/tmp/test_experiment_viz.txt";
        bool success = ExperimentResultsSystem::generateVisualization(result, vizPath);
        
        assert(success);
        
        // Verify file exists and contains visualization content
        std::ifstream file(vizPath);
        assert(file.good());
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        assert(content.find("NLM Experiment Results Visualization") != std::string::npos);
        assert(content.find("Firing Rate Time Series") != std::string::npos);
        
        file.close();
        std::filesystem::remove(vizPath);
        
        NLM_LOG_INFO("✓ Visualization generation test passed");
    }
    
    void testSummaryReportGeneration() {
        NLM_LOG_INFO("Testing summary report generation...");
        
        // Create a test result
        ComprehensiveExperimentResult result;
        result.experimentName = "SummaryTestExperiment";
        result.seed = 55555;
        result.experimentType = "Phase4";
        result.startStep = 0;
        result.endStep = 3;
        
        // Add step data
        for (int i = 0; i < 3; ++i) {
            Experiment::StepData step;
            step.step = i;
            step.neuronCount = 120 + i * 12;
            step.synapseCount = 1200 + i * 120;
            step.firingCount = 60 + i * 6;
            step.avgFiringRate = 0.6f + i * 0.03f;
            result.stepData.push_back(step);
        }
        
        result.metrics.push_back({"total_reward", 30.0f});
        result.analysis.meanFiringRate = 0.63f;
        result.analysis.meanReward = 35.0f;
        result.analysis.temporalStability = 0.7f;
        result.analysis.learningCurveSlope = 0.05f;
        
        // Test summary report generation
        std::string reportPath = "/tmp/test_experiment_summary.txt";
        bool success = ExperimentResultsSystem::generateSummaryReport(result, reportPath);
        
        assert(success);
        
        // Verify file exists and contains summary content
        std::ifstream file(reportPath);
        assert(file.good());
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        assert(content.find("NLM EXPERIMENT EXECUTIVE SUMMARY") != std::string::npos);
        assert(content.find("SummaryTestExperiment") != std::string::npos);
        assert(content.find("Phase4") != std::string::npos);
        
        file.close();
        std::filesystem::remove(reportPath);
        
        NLM_LOG_INFO("✓ Summary report generation test passed");
    }
    
    void testFileOperations() {
        NLM_LOG_INFO("Testing file operations...");
        
        // Create test result
        ComprehensiveExperimentResult result;
        result.experimentName = "FileTestExperiment";
        result.seed = 44444;
        result.experimentType = "Generic";
        
        // Test file finding (empty directory)
        std::string tempDir = "/tmp/test_results";
        std::vector<std::string> foundFiles = ExperimentResultsSystem::findResultFiles(tempDir);
        // Should return empty vector for non-existent directory
        
        // Test loading (should fail for non-existent file)
        bool loadJsonResult = ExperimentResultsSystem::loadFromJSON("/tmp/nonexistent.json", result);
        assert(!loadJsonResult);
        
        bool loadBinaryResult = ExperimentResultsSystem::loadFromBinary("/tmp/nonexistent.bin", result);
        assert(!loadBinaryResult);
        
        // Test metadata operations
        std::string key = "author";
        std::string value = "NLM Research Team";
        ExperimentResultsSystem::addMetadata(result, key, value);
        
        assert(result.metadata.find(key) != result.metadata.end());
        assert(result.metadata[key] == value);
        
        NLM_LOG_INFO("✓ File operations test passed");
    }
};

int main() {
    ExperimentResultsSystemTest test;
    test.runAllTests();
    return 0;
}

} // namespace nlm