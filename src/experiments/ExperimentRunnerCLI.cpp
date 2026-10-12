// NLM (熙然) - Neural Learning Machine
// Experiment Runner CLI - Command-line interface for running experiments

#include "core/CLI/NLMCLI.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace nlm;

class ExperimentRunnerCLI {
public:
    ExperimentRunnerCLI(const CLIConfigManager& config) : configManager(config) {}
    
    int run(CLIProgressReporter& reporter) {
        reporter.log("=== NLM Experiment Runner CLI ===");
        reporter.log("Running experiments via command line...");
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Get experiment configuration
        std::string mode = configManager.getConfig("mode", "single");
        std::string experimentName = configManager.getConfig("experiment", "default");
        int maxSteps = configManager.getInt("steps", 1000);
        int numExperiments = configManager.getInt("experiments", 1);
        std::string outputFile = configManager.getConfig("output", "experiment_results.json");
        bool saveResults = configManager.hasOption("--save") || configManager.hasOption("--output");
        bool analyzeResults = configManager.hasOption("--analyze") || configManager.getBool("analyze", false);
        bool batchMode = configManager.getBool("batch", false);
        
        reporter.log("Experiment configuration:");
        reporter.log("  Mode: " + mode);
        reporter.log("  Experiment: " + experimentName);
        reporter.log("  Steps per experiment: " + std::to_string(maxSteps));
        reporter.log("  Number of experiments: " + std::to_string(numExperiments));
        reporter.log("  Save results: " + std::string(saveResults ? "yes" : "no"));
        reporter.log("  Analyze results: " + std::string(analyzeResults ? "yes" : "no"));
        reporter.log("  Batch mode: " + std::string(batchMode ? "yes" : "no"));
        
        ExperimentRunner runner;
        
        if (batchMode) {
            return runBatchExperiments(runner, reporter, numExperiments, maxSteps, saveResults, outputFile, analyzeResults);
        } else {
            return runSingleExperiment(runner, reporter, experimentName, maxSteps, saveResults, outputFile, analyzeResults);
        }
    }
    
private:
    int runSingleExperiment(ExperimentRunner& runner, CLIProgressReporter& reporter, 
                           const std::string& experimentName, int maxSteps,
                           bool saveResults, const std::string& outputFile, bool analyzeResults) {
        reporter.log("");
        reporter.log("=== Running Single Experiment ===");
        
        // Create experiment
        auto experiment = runner.createExperiment(experimentName);
        experiment->setSeed(configManager.getInt("seed", 42));
        
        // Create brain with configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", experiment->getSeed());
        config->set("neuron_count", configManager.getInt("neuron_count", 100));
        config->set("region_count", configManager.getInt("region_count", 1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            reporter.error("Failed to initialize brain!");
            return 1;
        }
        
        // Create environment (simplified)
        auto environment = std::make_shared<Environment>();
        if (!environment->reset()) {
            reporter.error("Failed to reset environment!");
            return 1;
        }
        
        reporter.log("Experiment: " + experimentName);
        reporter.log("Seed: " + std::to_string(experiment->getSeed()));
        reporter.log("Running simulation for " + std::to_string(maxSteps) + " steps...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run experiment
        bool success = runner.runExperiment(experiment, brain, environment, maxSteps);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
        
        if (!success) {
            reporter.error("Experiment failed!");
            return 1;
        }
        
        reporter.log("");
        reporter.log("=== Experiment Results ===");
        displayExperimentResults(experiment, reporter);
        
        if (saveResults) {
            saveExperimentResults(experiment, outputFile, reporter);
        }
        
        if (analyzeResults) {
            analyzeExperimentResults(experiment, reporter);
        }
        
        reporter.log("");
        reporter.log("Experiment completed in " + std::to_string(duration) + "s");
        
        return 0;
    }
    
    int runBatchExperiments(ExperimentRunner& runner, CLIProgressReporter& reporter,
                           int numExperiments, int maxSteps,
                           bool saveResults, const std::string& outputFile, bool analyzeResults) {
        reporter.log("");
        reporter.log("=== Running Batch Experiments ===");
        reporter.log("This will run " + std::to_string(numExperiments) + " experiments...");
        
        // Create multiple experiments
        std::vector<std::shared_ptr<Experiment>> experiments;
        for (int i = 0; i < numExperiments; ++i) {
            std::string expName = "experiment_" + std::to_string(i + 1);
            auto experiment = runner.createExperiment(expName);
            experiment->setSeed(configManager.getInt("base_seed", 42) + i);
            experiments.push_back(experiment);
        }
        
        reporter.log("Created " + std::to_string(experiments.size()) + " experiments");
        
        // Create brain template
        auto config = std::make_shared<Config>();
        config->set("random_seed", configManager.getInt("base_seed", 42));
        config->set("neuron_count", configManager.getInt("neuron_count", 100));
        config->set("region_count", configManager.getInt("region_count", 1));
        
        auto brainTemplate = std::make_shared<Brain>(config);
        if (!brainTemplate->initialize()) {
            reporter.error("Failed to initialize brain template!");
            return 1;
        }
        
        // Create environment template
        auto environmentTemplate = std::make_shared<Environment>();
        if (!environmentTemplate->reset()) {
            reporter.error("Failed to reset environment template!");
            return 1;
        }
        
        reporter.log("Starting batch execution...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run batch
        std::vector<std::shared_ptr<Experiment>> results = 
            runner.runBatch(experiments, brainTemplate, environmentTemplate, maxSteps);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
        
        reporter.log("");
        reporter.log("=== Batch Results ===");
        reporter.log("Total experiments: " + std::to_string(experiments.size()));
        reporter.log("Successful experiments: " + std::to_string(results.size());
        reporter.log("Failed experiments: " + std::to_string(experiments.size() - results.size()));
        reporter.log("Batch time: " + std::to_string(duration) + "s");
        
        // Save batch results if requested
        if (saveResults) {
            saveBatchResults(results, outputFile, reporter);
        }
        
        // Analyze batch results if requested
        if (analyzeResults) {
            analyzeBatchResults(results, reporter);
        }
        
        reporter.log("");
        reporter.log("Batch experiment completed successfully");
        
        return 0;
    }
    
    void displayExperimentResults(std::shared_ptr<Experiment> experiment, CLIProgressReporter& reporter) {
        reporter.log("Experiment: " + experiment->getName());
        reporter.log("Seed: " + std::to_string(experiment->getSeed()));
        reporter.log("Steps: " + std::to_string(experiment->getStartStep()) + " - " + std::to_string(experiment->getEndStep()));
        
        // Display metrics
        reporter.log("Metrics:");
        const auto& metrics = experiment->getMetrics();
        for (const auto& pair : metrics) {
            reporter.log("  " + pair.first + ": " + std::to_string(pair.second));
        }
        
        // Display step data summary
        const auto& stepData = experiment->getStepData();
        if (!stepData.empty()) {
            size_t totalSteps = stepData.size();
            size_t totalNeurons = 0;
            size_t totalSynapses = 0;
            float totalFiring = 0.0f;
            
            for (const auto& step : stepData) {
                totalNeurons += step.neuronCount;
                totalSynapses += step.synapseCount;
                totalFiring += step.avgFiringRate;
            }
            
            reporter.log("Step data summary:");
            reporter.log("  Average neurons per step: " + std::to_string(totalNeurons / totalSteps));
            reporter.log("  Average synapses per step: " + std::to_string(totalSynapses / totalSteps));
            reporter.log("  Average firing rate: " + std::to_string(totalFiring / totalSteps));
        }
        
        // Display notes
        const auto& notes = experiment->getNotes();
        if (!notes.empty()) {
            reporter.log("Notes:");
            for (const auto& note : notes) {
                reporter.log("  - " + note);
            }
        }
    }
    
    void saveExperimentResults(std::shared_ptr<Experiment> experiment, const std::string& filename, CLIProgressReporter& reporter) {
        reporter.log("Saving experiment results to: " + filename);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            reporter.error("Failed to create output file: " + filename);
            return;
        }
        
        // Write experiment data in JSON format
        file << "{" << std::endl;
        file << "  \"name\": \"" << experiment->getName() << "\"," << std::endl;
        file << "  \"seed\": " << experiment->getSeed() << "," << std::endl;
        file << "  \"start_step\": " << experiment->getStartStep() << "," << std::endl;
        file << "  \"end_step\": " << experiment->getEndStep() << "," << std::endl;
        
        // Write metrics
        file << "  \"metrics\": {" << std::endl;
        const auto& metrics = experiment->getMetrics();
        for (size_t i = 0; i < metrics.size(); ++i) {
            const auto& pair = metrics[i];
            file << "    \"" << pair.first << "\": " << pair.second;
            if (i < metrics.size() - 1) file << ",";
            file << std::endl;
        }
        file << "  }," << std::endl;
        
        // Write step data summary
        file << "  \"step_data_count\": " << experiment->getStepData().size() << std::endl;
        
        file << "}" << std::endl;
        
        file.close();
        
        reporter.log("Experiment results saved successfully");
    }
    
    void saveBatchResults(const std::vector<std::shared_ptr<Experiment>>& results, const std::string& filename, CLIProgressReporter& reporter) {
        reporter.log("Saving batch results to: " + filename);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            reporter.error("Failed to create output file: " + filename);
            return;
        }
        
        // Write batch results in JSON format
        file << "{" << std::endl;
        file << "  \"batch_results\": {" << std::endl;
        file << "    \"total_experiments\": " << results.size() << "," << std::endl;
        file << "    \"successful_experiments\": " << results.size() << std::endl;
        file << "  }," << std::endl;
        
        // Write individual experiment results
        file << "  \"experiments\": [" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            auto& exp = results[i];
            file << "    {" << std::endl;
            file << "      \"name\": \"" << exp->getName() << "\"," << std::endl;
            file << "      \"seed\": " << exp->getSeed() << "," << std::endl;
            file << "      \"start_step\": " << exp->getStartStep() << "," << std::endl;
            file << "      \"end_step\": " << exp->getEndStep() << std::endl;
            file << "    }";
            if (i < results.size() - 1) file << ",";
            file << std::endl;
        }
        file << "  ]" << std::endl;
        
        file << "}" << std::endl;
        
        file.close();
        
        reporter.log("Batch results saved successfully");
    }
    
    void analyzeExperimentResults(std::shared_ptr<Experiment> experiment, CLIProgressReporter& reporter) {
        reporter.log("Analyzing experiment results...");
        
        const auto& metrics = experiment->getMetrics();
        if (metrics.empty()) {
            reporter.log("No metrics to analyze");
            return;
        }
        
        // Calculate statistics for numeric metrics
        reporter.log("Analysis results:");
        for (const auto& pair : metrics) {
            double value = pair.second;
            
            // Determine metric type and provide appropriate analysis
            if (pair.first.find("reward") != std::string::npos || 
                pair.first.find("score") != std::string::npos) {
                reporter.log("  " + pair.first + ": " + std::to_string(value) + " (performance metric)");
            } else if (pair.first.find("firing") != std::string::npos || 
                       pair.first.find("rate") != std::string::npos) {
                reporter.log("  " + pair.first + ": " + std::to_string(value) + " (neural activity metric)");
            } else if (pair.first.find("synapse") != std::string::npos || 
                       pair.first.find("weight") != std::string::npos) {
                reporter.log("  " + pair.first + ": " + std::to_string(value) + " (plasticity metric)");
            } else {
                reporter.log("  " + pair.first + ": " + std::to_string(value));
            }
        }
        
        const auto& stepData = experiment->getStepData();
        if (!stepData.empty()) {
            reporter.log("Step data analysis:");
            
            size_t minNeurons = stepData[0].neuronCount;
            size_t maxNeurons = stepData[0].neuronCount;
            size_t minSynapses = stepData[0].synapseCount;
            size_t maxSynapses = stepData[0].synapseCount;
            float minFiring = stepData[0].avgFiringRate;
            float maxFiring = stepData[0].avgFiringRate;
            
            for (const auto& step : stepData) {
                minNeurons = std::min(minNeurons, step.neuronCount);
                maxNeurons = std::max(maxNeurons, step.neuronCount);
                minSynapses = std::min(minSynapses, step.synapseCount);
                maxSynapses = std::max(maxSynapses, step.synapseCount);
                minFiring = std::min(minFiring, step.avgFiringRate);
                maxFiring = std::max(maxFiring, step.avgFiringRate);
            }
            
            reporter.log("  Neurons per step: " + std::to_string(minNeurons) + " - " + std::to_string(maxNeurons));
            reporter.log("  Synapses per step: " + std::to_string(minSynapses) + " - " + std::to_string(maxSynapses));
            reporter.log("  Firing rate per step: " + std::to_string(minFiring) + " - " + std::to_string(maxFiring));
        }
        
        reporter.log("Analysis completed");
    }
    
    void analyzeBatchResults(const std::vector<std::shared_ptr<Experiment>>& results, CLIProgressReporter& reporter) {
        reporter.log("Analyzing batch results...");
        
        if (results.empty()) {
            reporter.log("No results to analyze");
            return;
        }
        
        reporter.log("Batch analysis summary:");
        reporter.log("  Total experiments: " + std::to_string(results.size()));
        
        // Calculate aggregate metrics
        double totalReward = 0.0;
        double avgFiringRate = 0.0;
        size_t totalSteps = 0;
        
        for (const auto& exp : results) {
            const auto& metrics = exp->getMetrics();
            for (const auto& pair : metrics) {
                if (pair.first.find("reward") != std::string::npos || 
                    pair.first.find("score") != std::string::npos) {
                    totalReward += pair.second;
                }
                if (pair.first.find("firing") != std::string::npos || 
                    pair.first.find("rate") != std::string::npos) {
                    avgFiringRate += pair.second;
                }
            }
            
            totalSteps += exp->getEndStep() - exp->getStartStep();
        }
        
        if (!results.empty()) {
            reporter.log("  Average reward: " + std::to_string(totalReward / results.size()));
            reporter.log("  Average firing rate: " + std::to_string(avgFiringRate / results.size()));
            reporter.log("  Average steps per experiment: " + std::to_string(totalSteps / results.size()));
        }
        
        reporter.log("Batch analysis completed");
    }
    
private:
    CLIConfigManager& configManager;
};

int main(int argc, char** argv) {
    // Create and run experiment runner CLI
    NLMCLI cli(argc, argv, "nlm_experiment");
    
    if (cli.getConfigManager().hasOption("--help") || 
        cli.getConfigManager().getConfig("subcommand") == "help") {
        cli.printUsage();
        return 0;
    }
    
    if (cli.getConfigManager().hasOption("--version")) {
        std::cout << "NLM Experiment Runner CLI" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Batch experiment execution and analysis" << std::endl;
        return 0;
    }
    
    // Run experiment command
    CLIProgressReporter reporter(std::cout, cli.getConfigManager().getBool("--quiet", false));
    ExperimentRunnerCLI experimentRunner(cli.getConfigManager());
    
    return experimentRunner.run(reporter);
}