// NLM CLI Experiment - Experiment Management Commands
// Handles experiment running, listing, and management

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {
namespace cli {

class ExperimentCLI {
public:
    ExperimentCLI() {
        initializeLogger();
    }
    
    int run(int argc, char** argv) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string command = argv[1];
        
        try {
            if (command == "run") {
                return handleRunCommand(argc, argv);
            } else if (command == "list") {
                return handleListCommand(argc, argv);
            } else if (command == "info") {
                return handleInfoCommand(argc, argv);
            } else if (command == "compare") {
                return handleCompareCommand(argc, argv);
            } else if (command == "track") {
                return handleTrackCommand(argc, argv);
            } else if (command == "results") {
                return handleResultsCommand(argc, argv);
            } else {
                std::cerr << "Unknown experiment command: " << command << std::endl;
                printUsage();
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
private:
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    int handleRunCommand(int argc, char** argv) {
        std::string experimentType = "phase3";
        SimulationStep steps = 10000;
        std::string outputFile = "experiment_results.json";
        double samplingInterval = 0.1;
        bool continuous = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 15) == "--experiment") {
                if (arg.find('=') != std::string::npos) {
                    experimentType = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    experimentType = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg.substr(0, 20) == "--sampling-interval") {
                if (arg.find('=') != std::string::npos) {
                    samplingInterval = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    samplingInterval = std::stod(argv[++i]);
                }
            } else if (arg == "--continuous") {
                continuous = true;
            }
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentType);
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        NLM_LOG_INFO("Output file: " + outputFile);
        NLM_LOG_INFO("Sampling interval: " + std::to_string(samplingInterval) + "s");
        NLM_LOG_INFO("Continuous mode: " + std::string(continuous ? "enabled" : "disabled"));
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (continuous) {
            return runContinuousExperiment(brain, experimentType, steps, outputFile, samplingInterval);
        } else {
            if (experimentType == "phase3") {
                return runPhase3Experiment(brain, steps, outputFile);
            } else if (experimentType == "phase4") {
                return runPhase4Experiment(brain, steps, outputFile);
            } else if (experimentType == "phase5") {
                return runPhase5Experiment(brain, steps, outputFile);
            } else if (experimentType == "phase6") {
                return runPhase6Experiment(brain, steps, outputFile);
            } else {
                NLM_LOG_ERROR("Unknown experiment type: " + experimentType);
                return 1;
            }
        }
    }
    
    int runContinuousExperiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                                SimulationStep steps, const std::string& outputFile,
                                double samplingInterval) {
        NLM_LOG_INFO("Running continuous experiment...");
        
        std::vector<std::string> experimentResults;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % static_cast<SimulationStep>(samplingInterval * 100.0) == 0) {
                std::stringstream result;
                result << "Step: " << step
                       << " | Spikes: " << brain->getTotalSpikeCount()
                       << " | Firing: " << brain->getFiringNeuronCount()
                       << " | Active: " << brain->getActiveNeuronCount()
                       << " | E/I ratio: " << brain->getExcitationInhibitionRatio();
                
                NLM_LOG_INFO(result.str());
                experimentResults.push_back(result.str());
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Experiment completed in " + std::to_string(duration.count()) + "ms");
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                for (const auto& result : experimentResults) {
                    file << result << std::endl;
                }
                NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                file.close();
            } else {
                NLM_LOG_WARNING("Failed to save experiment results to: " + outputFile);
            }
        }
        
        return 0;
    }
    
    int runPhase3Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase3Experiment>("Phase3_" + 
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 3 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 3 experiment failed");
            return 1;
        }
    }
    
    int runPhase4Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase4Experiment>("Phase4_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 4 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 4 experiment failed");
            return 1;
        }
    }
    
    int runPhase5Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase5Experiment>("Phase5_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 5 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 5 experiment failed");
            return 1;
        }
    }
    
    int runPhase6Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase6IntegratedExperiment>("Phase6_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 6 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 6 experiment failed");
            return 1;
        }
    }
    
    int handleListCommand(int argc, char** argv) {
        auto runner = std::make_shared<ExperimentRunner>();
        
        NLM_LOG_INFO("Registered experiments:");
        for (const auto& exp : runner->getExperiments()) {
            NLM_LOG_INFO("  - " + exp->getName());
            NLM_LOG_INFO("    Start step: " + std::to_string(exp->getStartStep()));
            NLM_LOG_INFO("    End step: " + std::to_string(exp->getEndStep()));
            NLM_LOG_INFO("    Metrics count: " + std::to_string(exp->getMetrics().size()));
            NLM_LOG_INFO("    Success: " + std::string(exp->getSuccess() ? "true" : "false"));
        }
        
        return 0;
    }
    
    int handleInfoCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing experiment name for experiment info command" << std::endl;
            return 1;
        }
        
        std::string experimentName = argv[2];
        
        auto runner = std::make_shared<ExperimentRunner>();
        auto experiment = runner->getExperiment(experimentName);
        
        if (experiment) {
            NLM_LOG_INFO("Experiment: " + experiment->getName());
            NLM_LOG_INFO("Description: " + experiment->getDescription());
            NLM_LOG_INFO("Start step: " + std::to_string(experiment->getStartStep()));
            NLM_LOG_INFO("End step: " + std::to_string(experiment->getEndStep()));
            NLM_LOG_INFO("Success: " + std::string(experiment->getSuccess() ? "true" : "false"));
            
            if (!experiment->getMetrics().empty()) {
                NLM_LOG_INFO("Metrics:");
                for (const auto& [key, value] : experiment->getMetrics()) {
                    NLM_LOG_INFO("  " + key + ": " + std::to_string(value));
                }
            }
        } else {
            NLM_LOG_ERROR("Experiment not found: " + experimentName);
            return 1;
        }
        
        return 0;
    }
    
    int handleCompareCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing experiment names for compare command" << std::endl;
            return 1;
        }
        
        std::string exp1Name = argv[2];
        std::string exp2Name = "";
        std::string metricsFormat = "detailed"; // simple, detailed, statistical
        double confidenceLevel = 0.95;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 4) == "--exp2") {
                if (arg.find('=') != std::string::npos) {
                    exp2Name = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    exp2Name = argv[++i];
                }
            } else if (arg.substr(0, 9) == "--metrics") {
                if (arg.find('=') != std::string::npos) {
                    metricsFormat = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    metricsFormat = argv[++i];
                }
            } else if (arg.substr(0, 15) == "--confidence") {
                if (arg.find('=') != std::string::npos) {
                    confidenceLevel = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    confidenceLevel = std::stod(argv[++i]);
                }
            }
        }
        
        if (exp2Name.empty()) {
            std::cerr << "Error: Missing second experiment name for compare command" << std::endl;
            return 1;
        }
        
        auto runner = std::make_shared<ExperimentRunner>();
        auto exp1 = runner->getExperiment(exp1Name);
        auto exp2 = runner->getExperiment(exp2Name);
        
        if (!exp1) {
            NLM_LOG_ERROR("Experiment not found: " + exp1Name);
            return 1;
        }
        
        if (!exp2) {
            NLM_LOG_ERROR("Experiment not found: " + exp2Name);
            return 1;
        }
        
        NLM_LOG_INFO("Comparing experiments: " + exp1Name + " vs " + exp2Name);
        
        // Perform statistical comparison
        compareExperimentsStatistical(exp1, exp2, metricsFormat, confidenceLevel);
        
        return 0;
    }
    
    int handleTrackCommand(int argc, char** argv) {
        std::string experimentName = "";
        bool followMode = false;
        double updateInterval = 1.0; // seconds
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 15) == "--experiment") {
                if (arg.find('=') != std::string::npos) {
                    experimentName = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    experimentName = argv[++i];
                }
            } else if (arg == "--follow") {
                followMode = true;
            } else if (arg.substr(0, 16) == "--interval") {
                if (arg.find('=') != std::string::npos) {
                    updateInterval = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    updateInterval = std::stod(argv[++i]);
                }
            }
        }
        
        if (experimentName.empty()) {
            std::cerr << "Error: Missing experiment name for experiment track command" << std::endl;
            return 1;
        }
        
        auto runner = std::make_shared<ExperimentRunner>();
        auto experiment = runner->getExperiment(experimentName);
        
        if (!experiment) {
            NLM_LOG_ERROR("Experiment not found: " + experimentName);
            return 1;
        }
        
        NLM_LOG_INFO("Tracking experiment: " + experiment->getName());
        NLM_LOG_INFO("Follow mode: " + std::string(followMode ? "enabled" : "disabled"));
        NLM_LOG_INFO("Update interval: " + std::to_string(updateInterval) + "s");
        
        if (followMode) {
            trackExperimentContinuously(experiment, updateInterval);
        } else {
            trackExperimentSnapshot(experiment);
        }
        
        return 0;
    }
    
    int handleResultsCommand(int argc, char** argv) {
        std::string outputFile = "experiment_results.csv";
        bool includeMetrics = true;
        bool includeRawData = false;
        std::string format = "csv"; // csv, json, excel
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg == "--no-metrics") {
                includeMetrics = false;
            } else if (arg == "--raw-data") {
                includeRawData = true;
            } else if (arg == "--format") {
                if (i + 1 < argc) {
                    format = argv[++i];
                }
            }
        }
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        NLM_LOG_INFO("Exporting experiment results to: " + outputFile);
        NLM_LOG_INFO("Include metrics: " + std::string(includeMetrics ? "yes" : "no"));
        NLM_LOG_INFO("Include raw data: " + std::string(includeRawData ? "yes" : "no"));
        NLM_LOG_INFO("Format: " + format);
        
        // Get all registered experiments
        auto experiments = runner->getExperiments();
        
        if (experiments.empty()) {
            NLM_LOG_WARNING("No experiments registered for export.");
            return 0;
        }
        
        // Export to specified format
        if (format == "csv") {
            exportResultsToCSV(experiments, outputFile, includeMetrics, includeRawData);
        } else if (format == "json") {
            exportResultsToJSON(experiments, outputFile, includeMetrics, includeRawData);
        } else if (format == "excel") {
            exportResultsToExcel(experiments, outputFile, includeMetrics, includeRawData);
        } else {
            std::cerr << "Unknown format: " << format << std::endl;
            return 1;
        }
        
        NLM_LOG_INFO("Experiment results exported successfully to: " + outputFile);
        return 0;
    }
    
    void printUsage() {
        std::cout << "Usage: nlm-experiment <command> [options]" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << "  run           Run an experiment" << std::endl;
        std::cout << "  list          List registered experiments" << std::endl;
        std::cout << "  info <name>   Show experiment information" << std::endl;
        std::cout << "  compare <name1> <name2>  Compare two experiments" << std::endl;
        std::cout << "  track         Track experiment progress" << std::endl;
        std::cout << "  results       Export experiment results" << std::endl;
    }
};

int main(int argc, char** argv) {
    ExperimentCLI cli;
    return cli.run(argc, argv);
}

} // namespace cli
} // namespace nlm
