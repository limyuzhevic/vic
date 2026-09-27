#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/ResultsSystem.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Experiment.hpp"
#include <iostream>
#include <vector>
#include <memory>

namespace nlm {

class DemoExperimentRunner {
public:
    DemoExperimentRunner() {
        NLM_LOG_INFO("=== NLM Phase 2 Experiment Runner Demo ===");
    }
    
    void runDemonstration() {
        // Create experiment runner
        auto runner = std::make_shared<ExperimentRunner>();
        
        // Set global seed for reproducibility
        runner->setGlobalSeed(42);
        
        // Create multiple experiments
        std::vector<std::shared_ptr<Experiment>> experiments;
        
        for (int i = 0; i < 3; ++i) {
            std::string expName = "Phase3_Experiment_" + std::to_string(i);
            auto experiment = runner->createExperiment(expName);
            
            // Add some metadata
            experiment->setConfigValue("neurons", "100");
            experiment->setConfigValue("synapses", "1000");
            experiment->addNote("Demo experiment for Phase 2 testing");
            
            // Add some step data (simulated)
            for (int step = 0; step < 10; ++step) {
                Experiment::StepData stepData;
                stepData.step = step;
                stepData.neuronCount = 100 + i * 50;
                stepData.synapseCount = 1000 + i * 500;
                stepData.firingCount = 50 + i * 25;
                stepData.avgFiringRate = 0.5f + i * 0.1f + step * 0.01f;
                experiment->recordStep(stepData.step, stepData.neuronCount, stepData.synapseCount,
                                      stepData.firingCount, stepData.avgFiringRate);
            }
            
            // Add some metrics
            experiment->recordMetric("total_reward", 10.0f + i * 5.0f);
            experiment->recordMetric("energy", 5.0f + i * 2.0f);
            
            experiments.push_back(experiment);
            
            NLM_LOG_INFO("Created experiment: " + expName);
        }
        
        // Demonstrate saveResults with different formats
        std::vector<std::string> formats = {"json", "csv", "txt"};
        
        for (const auto& format : formats) {
            std::string filename = "/tmp/experiment_results." + format;
            
            NLM_LOG_INFO("Saving results to: " + filename);
            bool success = runner->saveResults(filename);
            
            if (success) {
                NLM_LOG_INFO("[SUCCESS] Results saved in " + format + " format");
            } else {
                NLM_LOG_ERROR("[FAILED] Failed to save results in " + format + " format");
            }
        }
        
        // Demonstrate batch processing
        NLM_LOG_INFO("=== Demonstrating Batch Processing ===");
        
        // Create more experiments for batch processing
        std::vector<std::shared_ptr<Experiment>> batchExperiments;
        
        for (int i = 0; i < 5; ++i) {
            std::string expName = "Batch_Experiment_" + std::to_string(i);
            auto experiment = runner->createExperiment(expName);
            
            // Add batch-specific metadata
            experiment->setConfigValue("batch_id", std::to_string(i));
            experiment->setConfigValue("batch_size", "1000");
            
            // Simulate batch data collection
            float baseReward = 20.0f + i * 10.0f;
            for (int step = 0; step < 5; ++step) {
                Experiment::StepData stepData;
                stepData.step = step;
                stepData.neuronCount = 150 + i * 30;
                stepData.synapseCount = 1500 + i * 300;
                stepData.firingCount = 75 + i * 15;
                stepData.avgFiringRate = 0.75f + i * 0.05f;
                experiment->recordStep(stepData.step, stepData.neuronCount, stepData.synapseCount,
                                      stepData.firingCount, stepData.avgFiringRate);
            }
            
            experiment->recordMetric("total_reward", baseReward);
            batchExperiments.push_back(experiment);
        }
        
        // Save batch results
        std::string batchFile = "/tmp/batch_results.json";
        bool batchSuccess = runner->saveResults(batchFile);
        
        if (batchSuccess) {
            NLM_LOG_INFO("[SUCCESS] Batch results saved successfully");
            
            // Get all experiments from runner
            auto allExperiments = runner->getExperiments();
            NLM_LOG_INFO("Total experiments in runner: " + std::to_string(allExperiments.size()));
        } else {
            NLM_LOG_ERROR("[FAILED] Batch results save failed");
        }
        
        // Demonstrate results system analysis
        NLM_LOG_INFO("=== Demonstrating Results Analysis ===");
        
        ExperimentResultsSystem resultsSystem;
        
        // Use the first experiment for detailed analysis
        if (!allExperiments.empty()) {
            ComprehensiveExperimentResult comprehensiveResult = 
                resultsSystem.collectResults(allExperiments[0]);
            
            resultsSystem.analyzeResults(comprehensiveResult);
            
            NLM_LOG_INFO("Analysis Results:");
            NLM_LOG_INFO("  Experiment: " + comprehensiveResult.experimentName);
            NLM_LOG_INFO("  Type: " + comprehensiveResult.experimentType);
            NLM_LOG_INFO("  Mean Firing Rate: " + std::to_string(comprehensiveResult.analysis.meanFiringRate) + " Hz");
            NLM_LOG_INFO("  Total Reward: " + std::to_string(comprehensiveResult.analysis.totalReward));
            NLM_LOG_INFO("  Convergence Rate: " + std::to_string(comprehensiveResult.analysis.convergenceRate));
            NLM_LOG_INFO("  Temporal Stability: " + std::to_string(comprehensiveResult.analysis.temporalStability));
            NLM_LOG_INFO("  Learning Curve Slope: " + std::to_string(comprehensiveResult.analysis.learningCurveSlope));
            
            // Generate visualization and summary
            std::string vizFile = "/tmp/experiment_viz.txt";
            std::string summaryFile = "/tmp/experiment_summary.txt";
            
            resultsSystem.generateVisualization(comprehensiveResult, vizFile);
            resultsSystem.generateSummaryReport(comprehensiveResult, summaryFile);
            
            NLM_LOG_INFO("[SUCCESS] Visualization and summary reports generated");
        }
        
        // Clear experiments for clean demonstration
        runner->clearExperiments();
        NLM_LOG_INFO("All experiments cleared. Demo completed successfully.");
    }
};

} // namespace nlm

int main() {
    // Initialize logging
    nlm::Logger::initialize();
    
    // Run demonstration
    nlm::DemoExperimentRunner demo;
    demo.runDemonstration();
    
    // Cleanup
    nlm::Logger::shutdown();
    
    return 0;
}
