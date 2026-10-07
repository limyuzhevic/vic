// Advanced Experiment Automation Application
// Automated experiment scheduling, parameter optimization, and batch processing
// Integrates all advanced experiment capabilities with the enhanced NLM system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/AdvancedExperimentSystem.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace nlm;

void printAdvancedExperimentBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM ADVANCED EXPERIMENTS — 熙然                               ║
    ║     Neural Learning Machine Experimental Automation               ║
    ║                                                               ║
    ║     Automated experiment scheduling, parameter optimization      ║
    ║     and batch processing for Phase 2                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printAdvancedExperimentHelp() {
    printAdvancedExperimentBanner();
    std::cout << "\n";
    std::cout << "Usage: nlm_experiments [OPTIONS] [EXPERIMENT_MODE]\n";
    std::cout << "\n";
    stdcout << "EXPERIMENT MODES:\n";
    stdcout << "  --scheduler                  Run experiment scheduler\n";
    stdcout << "  --optimizer                  Run parameter optimizer\n";
    stdcout << "  --auto                       Run automated experiment runner\n";
    stdcout << "  --batch                      Run batch processor\n";
    stdcout << "  --all                        Run all experiment automation modes\n";
    stdcout << "\n";
    stdcout << "CORE OPTIONS:\n";
    stdcout << "  --help                       Show this help message\n";
    stdcout << "  --version                    Show version information\n";
    stdcout << "  --quiet                      Suppress informational messages\n";
    stdcout << "  --verbose                    Enable verbose output\n";
    stdcout << "  --config <file>              Load configuration file\n";
    std::cout << "  --output <dir>               Set output directory for results\n";
    stdcout << "\n";
    stdcout << "SCHEDULER OPTIONS:\n";
    stdcout << "  --schedule-file <file>        Load experiment schedule from file\n";
    stdcout << "  --schedule-interval <mins>    Set scheduling interval in minutes\n";
    stdcout << "  --max-concurrent <num>        Set maximum concurrent experiments\n";
    stdcout << "  --enable-auto-schedule        Enable automatic scheduling\n";
    stdcout << "\n";
    stdcout << "OPTIMIZER OPTIONS:\n";
    stdcout << "  --objective <func>            Set objective function (grid, random, genetic, bayesian)\n";
    stdcout << "  --opt-iterations <num>        Set optimization iterations\n";
    stdcout << "  --opt-tolerance <num>         Set optimization tolerance\n";
    stdcout << "  --param-range <param,min,max>  Set parameter range for optimization\n";
    stdcout << "\n";
    stdcout << "AUTOMATION OPTIONS:\n";
    stdcout << "  --continuous-learning          Enable continuous learning\n";
    stdcout << "  --learning-interval <steps>    Set learning interval in steps\n";
    stdcout << "  --quality-threshold <num>     Set quality threshold (0-1)\n";
    stdcout << "  --max-runtime <sec>           Set maximum runtime in seconds\n";
    stdcout << "\n";
    stdcout << "BATCH PROCESSING OPTIONS:\n";
    stdcout << "  --batch-size <num>            Set batch size\n";
    stdcout << "  --batch-format <format>       Set batch output format\n";
    stdcout << "  --enable-resume               Enable batch resume capability\n";
    stdcout << "  --enable-validation           Enable batch validation\n";
    std::cout << "  --max-threads <num>           Set maximum threads for parallel processing\n";
    stdcout << "\n";
}

void runAdvancedScheduler(std::shared_ptr<Config> config, UserFeedback& feedback) {
    std::cout << "\n=== Advanced Experiment Scheduler ===\n" << std::endl;
    
    // Initialize experiment scheduler
    ExperimentScheduler scheduler;
    
    // Load schedule from file if specified
    std::string scheduleFile = config->getOr<std::string>("schedule_file", "");
    if (!scheduleFile.empty()) {
        feedback.reportOperationStart("Schedule Loading");
        scheduler.loadSchedule(scheduleFile);
        feedback.reportOperationComplete("Schedule Loading", true);
    }
    
    // Set scheduler parameters
    int scheduleInterval = config->getOr<int>("schedule_interval", 60);
    int maxConcurrent = config->getOr<int>("max_concurrent", 3);
    bool autoSchedule = config->getOr<bool>("enable_auto_schedule", false);
    
    scheduler.setScheduleInterval(scheduleInterval);
    scheduler.setMaxConcurrentExperiments(maxConcurrent);
    scheduler.enableAutoScheduling(autoSchedule);
    
    // Set up parameter ranges for experiments
    std::cout << "Configuring parameter ranges for experiments..." << std::endl;
    
    // Example experiment configurations
    std::vector<std::string> experimentTypes = {"basic", "plasticity", "stdp"};
    std::vector<std::string> neuronCounts = {"100", "500", "1000", "2000"};
    std::vector<std::string> timestepValues = {"0.001", "0.005", "0.01"};
    
    for (const auto& expType : experimentTypes) {
        std::string expName = "experiment_" + expType + "_test";
        std::map<std::string, std::string> params;
        
        if (expType == "basic") {
            params["neuron_count"] = "500";
            params["timestep"] = "0.001";
        } else if (expType == "plasticity") {
            params["neuron_count"] = "1000";
            params["timestep"] = "0.005";
        } else if (expType == "stdp") {
            params["neuron_count"] = "1000";
            params["timestep"] = "0.001";
        }
        
        scheduler.addExperiment(expName, params);
    }
    
    feedback.reportOperationStart("Experiment Scheduling");
    
    // Run scheduled experiments
    std::vector<std::string> experimentNames = {"experiment_basic_test", 
                                               "experiment_plasticity_test",
                                               "experiment_stdp_test"};
    
    scheduler.runBatch(experimentNames);
    
    feedback.reportOperationComplete("Experiment Scheduling", true);
    
    // Analyze results
    std::cout << "\n=== Experiment Analysis ===\n" << std::endl;
    
    for (const auto& expName : experimentNames) {
        std::string analysis = scheduler.analyzeExperimentResults(expName);
        std::cout << "Experiment: " << expName << "\n";
        std::cout << "Analysis: " << analysis << "\n";
    }
    
    // Save schedule
    std::string outputFile = "experiments/scheduler_output.json";
    scheduler.saveSchedule(outputFile);
    std::cout << "\nSchedule saved to: " << outputFile << std::endl;
}

void runAdvancedOptimizer(std::shared_ptr<Config> config, UserFeedback& feedback) {
    std::cout << "\n=== Advanced Parameter Optimizer ===\n" << std::endl;
    
    // Initialize brain and optimizer
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for optimization!" << std::endl;
        return;
    }
    
    ParameterOptimizer optimizer;
    optimizer.attach(brain);
    
    // Set up objective function
    std::string objectiveFunc = config->getOr<std::string>("objective_function", "grid");
    auto objective = [brain](const std::map<std::string, double>& params) -> double {
        // Simple objective function: maximize firing rate while minimizing energy usage
        double firingRate = brain->getAverageFiringRate();
        double energyUsage = params["neuron_count"] * params["timestep"] * 100.0; // Simplified energy model
        return firingRate / (1.0 + energyUsage);
    };
    
    optimizer.setObjectiveFunction(objective);
    
    // Configure parameter ranges
    std::map<std::string, std::pair<double, double>> paramRanges;
    paramRanges["neuron_count"] = {100.0, 5000.0};
    paramRanges["timestep"] = {0.001, 0.01};
    paramRanges["plasticity_rate"] = {0.001, 0.1};
    
    // Set optimization parameters
    int iterations = config->getOr<int>("optimization_iterations", 100);
    double tolerance = config->getOr<double>("optimization_tolerance", 1e-6);
    
    // Set optimization bounds
    optimizer.setOptimizationBounds(paramRanges);
    
    feedback.reportOperationStart("Parameter Optimization");
    
    // Run optimization based on algorithm
    std::map<std::string, double> optimalParams;
    
    if (objectiveFunc == "grid") {
        std::cout << "Running grid search optimization..." << std::endl;
        // Create parameter grid
        std::map<std::string, std::vector<double>> paramGrid;
        for (int neurons : {100, 1000, 2000}) {
            paramGrid["neuron_count"].push_back(neurons);
        }
        for (double dt : {0.001, 0.005, 0.01}) {
            paramGrid["timestep"].push_back(dt);
        }
        
        optimalParams = optimizer.optimizeGrid(paramGrid);
    } else if (objectiveFunc == "random") {
        std::cout << "Running random search optimization..." << std::endl;
        optimalParams = optimizer.optimizeRandom(paramRanges, iterations);
    } else if (objectiveFunc == "genetic") {
        std::cout << "Running genetic algorithm optimization..." << std::endl;
        optimalParams = optimizer.optimizeGenetic(paramRanges, 50, iterations / 5);
    } else if (objectiveFunc == "bayesian") {
        std::cout << "Running Bayesian optimization..." << std::endl;
        optimalParams = optimizer.optimizeBayesian(paramRanges, iterations);
    }
    
    feedback.reportOperationComplete("Parameter Optimization", true);
    
    // Display optimization results
    std::cout << "\n=== Optimization Results ===\n" << std::endl;
    std::cout << "Optimal Parameters:" << std::endl;
    for (const auto& param : optimalParams) {
        std::cout << "  " << param.first << ": " << param.second << std::endl;
    }
    
    std::string result = optimizer.getOptimizationResult(optimalParams);
    std::cout << "\nOptimization Quality: " << result << std::endl;
}

void runAutomatedExperimentRunner(std::shared_ptr<Config> config, UserFeedback& feedback) {
    std::cout << "\n=== Automated Experiment Runner ===\n" << std::endl;
    
    // Initialize components
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for automated experiments!" << std::endl;
        return;
    }
    
    AutomatedExperimentRunner runner;
    runner.attach(brain);
    
    // Configure automation parameters
    bool continuousLearning = config->getOr<bool>("continuous_learning", false);
    int learningInterval = config->getOr<int>("learning_interval", 100);
    double qualityThreshold = config->getOr<double>("quality_threshold", 0.8);
    int maxRuntime = config->getOr<int>("max_runtime", 3600);
    
    runner.enableContinuousLearning(continuousLearning);
    runner.setLearningInterval(learningInterval);
    
    // Define experiment configurations
    std::vector<AdvancedExperimentConfig> experiments;
    
    // Basic experiment
    AdvancedExperimentConfig basicExp;
    basicExp.experimentName = "automated_basic_experiment";
    basicExp.experimentType = "basic";
    basicExp.parameters = {{"neuron_count", 500}, {"timestep", 0.001}};
    basicExp.qualityThreshold = qualityThreshold;
    basicExp.maxRuntimeSeconds = maxRuntime;
    basicExp.outputFormats = {"JSON", "CSV"};
    experiments.push_back(basicExp);
    
    // Plasticity experiment
    AdvancedExperimentConfig plasticityExp;
    plasticityExp.experimentName = "automated_plasticity_experiment";
    plasticityExp.experimentType = "plasticity";
    plasticityExp.parameters = {{"neuron_count", 1000}, {"timestep", 0.005}, {"plasticity_rate", 0.01}};
    plasticityExp.qualityThreshold = qualityThreshold;
    plasticityExp.maxRuntimeSeconds = maxRuntime;
    plasticityExp.outputFormats = {"JSON", "HTML"};
    experiments.push_back(plasticityExp);
    
    // STDP experiment
    AdvancedExperimentConfig stdpExp;
    stdpExp.experimentName = "automated_stdp_experiment";
    stdpExp.experimentType = "stdp";
    stdpExp.parameters = {{"neuron_count", 1000}, {"timestep", 0.001}, {"stdp_rate", 0.02}};
    stdpExp.qualityThreshold = qualityThreshold;
    stdpExp.maxRuntimeSeconds = maxRuntime;
    stdpExp.outputFormats = {"JSON", "PLAIN"};
    experiments.push_back(stdpExp);
    
    feedback.reportOperationStart("Automated Experiment Runner");
    
    // Run automated experiments
    for (const auto& exp : experiments) {
        feedback.reportOperationStart("Running " + exp.experimentName);
        
        // Validate parameters
        bool paramsValid = runner.validateParameters(exp.parameters);
        if (!paramsValid) {
            std::vector<std::string> errors = runner.getParameterValidationErrors(exp.parameters);
            std::cout << "Parameter validation failed: " << errors[0] << std::endl;
            feedback.reportOperationComplete("Running " + exp.experimentName, false);
            continue;
        }
        
        // Run experiment
        runAutomatedExperimentRunner(&runner, exp);
        
        // Analyze results
        std::string analysis = runner.analyzeExperimentalResults(exp.experimentName);
        std::cout << "Experiment " << exp.experimentName << " completed: " << analysis << std::endl;
        
        feedback.reportOperationComplete("Running " + exp.experimentName, true);
    }
    
    feedback.reportOperationComplete("Automated Experiment Runner", true);
    
    std::cout << "\n=== Automated Experiment Runner Complete ===\n" << std::endl;
}

void runBatchProcessor(std::shared_ptr<Config> config, UserFeedback& feedback) {
    std::cout << "\n=== Batch Experiment Processor ===\n" << std::endl;
    
    // Initialize brain and batch processor
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for batch processing!" << std::endl;
        return;
    }
    
    ExperimentBatchProcessor batchProcessor;
    batchProcessor.attach(brain);
    
    // Configure batch processing parameters
    int batchSize = config->getOr<int>("batch_size", 10);
    std::string batchFormat = config->getOr<std::string>("batch_format", "JSON");
    bool resumeBatch = config->getOr<bool>("enable_batch_resume", false);
    bool validateBatch = config->getOr<bool>("enable_batch_validation", true);
    int maxThreads = config->getOr<int>("max_threads", std::thread::hardware_concurrency());
    
    batchProcessor.setBatchSize(batchSize);
    batchProcessor.setBatchOutputFormat(batchFormat);
    batchProcessor.enableBatchResume(resumeBatch);
    batchProcessor.enableBatchValidation(validateBatch);
    
    // Generate batch parameter sets
    std::vector<std::map<std::string, double>> parameterSets;
    
    for (int i = 0; i < batchSize; ++i) {
        std::map<std::string, double> params;
        
        // Neuron count: vary between 100 and 2000
        params["neuron_count"] = 100 + (i % 19) * 100;
        
        // Timestep: vary between 0.001 and 0.01
        params["timestep"] = 0.001 + (i % 10) * 0.0009;
        
        // Plasticity rate: vary between 0.001 and 0.1
        params["plasticity_rate"] = 0.001 + (i % 100) * 0.001;
        
        parameterSets.push_back(params);
    }
    
    feedback.reportOperationStart("Batch Processing");
    
    // Process batch with or without parallelism
    if (maxThreads > 1) {
        std::cout << "Processing batch with parallel processing (" << maxThreads << " threads)..." << std::endl;
        batchProcessor.processBatchWithParallelism(parameterSets, maxThreads);
    } else {
        std::cout << "Processing batch sequentially..." << std::endl;
        batchProcessor.processBatch(parameterSets);
    }
    
    feedback.reportOperationComplete("Batch Processing", true);
    
    // Display batch status
    std::cout << "\n=== Batch Processing Status ===\n" << std::endl;
    std::vector<std::string> status = batchProcessor.getBatchStatus();
    for (const auto& task : status) {
        std::cout << "  " << task << std::endl;
    }
    
    double progress = batchProcessor.getProgress();
    std::cout << "\nBatch Progress: " << progress * 100 << "%" << std::endl;
    
    // Generate batch report
    std::string outputDir = "experiments/batch_results";
    std::string report = batchProcessor.generateBatchReport(outputDir);
    std::cout << "Batch report generated: " << outputDir << std::endl;
}

int main(int argc, char** argv) {
    // Initialize feedback system
    UserFeedback feedback;
    ErrorHandler errorHandler;
    
    try {
        // Parse command line arguments
        std::string configFile = "configs/default.cfg";
        std::string outputDir = "experiments/output";
        bool quiet = false;
        bool verbose = false;
        std::string experimentMode = "all";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help") {
                printAdvancedExperimentHelp();
                return 0;
            } else if (arg == "--version") {
                printAdvancedExperimentBanner();
                stdcout << "\nVersion: NLM Advanced Experiment Automation\n";
                stdcout << "Build: Automated experiment scheduling, parameter optimization, and batch processing\n";
                stdcout << "Features: Smart scheduling, intelligent optimization, continuous learning\n";
                return 0;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--output") {
                if (arg.find('=') != std::string::npos) {
                    outputDir = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputDir = argv[++i];
                }
            } else if (arg == "--quiet") {
                quiet = true;
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg == "--scheduler") {
                experimentMode = "scheduler";
            } else if (arg == "--optimizer") {
                experimentMode = "optimizer";
            } else if (arg == "--auto") {
                experimentMode = "auto";
            } else if (arg == "--batch") {
                experimentMode = "batch";
            }
        }
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize feedback system
        feedback.initialize(verbose, !quiet);
        feedback.enableFileOutput(outputDir);
        
        printAdvancedExperimentBanner();
        
        std::cout << "\nInitializing NLM Advanced Experiment Automation...\n" << std::endl;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found: " << configFile << ")" << std::endl;
        }
        
        // Run experiment automation based on mode
        if (experimentMode == "scheduler" || experimentMode == "all") {
            runAdvancedScheduler(config, feedback);
        }
        
        if (experimentMode == "optimizer" || experimentMode == "all") {
            runAdvancedOptimizer(config, feedback);
        }
        
        if (experimentMode == "auto" || experimentMode == "all") {
            runAutomatedExperimentRunner(config, feedback);
        }
        
        if (experimentMode == "batch" || experimentMode == "all") {
            runBatchProcessor(config, feedback);
        }
        
        // Save feedback session report
        feedback.saveSessionReport(outputDir + "/advanced_experiment_session_report.txt");
        
        std::cout << "\n=== Advanced Experiment Automation Complete ===\n" << std::endl;
        std::cout << "Advanced experiment results saved to: " << outputDir << std::endl;
        std::cout << "Key files:" << std::endl;
        std::cout << "  - " << outputDir << "/scheduler_output.json" << std::endl;
        std::cout << "  - " << outputDir << "/batch_results/" << std::endl;
        std::cout << "  - " << outputDir << "/advanced_experiment_session_report.txt" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError during advanced experiment automation: " << e.what() << std::endl;
        std::cerr << "Run with --verbose for detailed error information." << std::endl;
        
        return 1;
    }
    
    return 0;
}
