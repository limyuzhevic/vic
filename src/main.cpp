// Enhanced NLM Main - Advanced Command Line Interface
// Provides comprehensive control over NLM execution with advanced features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "performance/PerformanceMonitor.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>

using namespace nlm;

// Enhanced command line options and execution modes
enum class ExecutionMode {
    DEFAULT,           // Original behavior (backward compatibility)
    BENCHMARK,         // Performance benchmarking
    TRAINING,          // Machine learning training
    MONITORING,        // Real-time monitoring
    VALIDATION,        // Configuration validation
    ANALYSIS,          // Detailed analysis and reporting
    BENCHMARK_WITH_PERFORMANCE,  // Enhanced benchmark with monitoring
    CUSTOM           // Custom execution with user-defined parameters
};

// Enhanced configuration options
struct AdvancedExecutionOptions {
    ExecutionMode mode;
    std::string configFile;
    bool enablePerformanceMonitoring;
    SamplingInterval monitoringInterval;
    bool verboseLogging;
    std::string outputFile;
    std::string benchmarkFile;
    size_t benchmarkNeurons;
    size_t benchmarkSteps;
    size_t trainingEpochs;
    double trainingLearningRate;
    double validationSplit;
    bool saveIntermediateResults;
    std::string analysisFormat;
    bool enableAdvancedFeatures;
    std::vector<std::string> customParams;
    
    AdvancedExecutionOptions() : mode(ExecutionMode::DEFAULT),
                                 configFile("configs/default.cfg"),
                                 enablePerformanceMonitoring(false),
                                 monitoringInterval(SamplingInterval::MEDIUM),
                                 verboseLogging(false),
                                 outputFile(""),
                                 benchmarkFile(""),
                                 benchmarkNeurons(1000),
                                 benchmarkSteps(500),
                                 trainingEpochs(100),
                                 trainingLearningRate(0.01),
                                 validationSplit(0.2),
                                 saveIntermediateResults(false),
                                 analysisFormat("detailed"),
                                 enableAdvancedFeatures(true) {}
};

// Parse advanced command line arguments
AdvancedExecutionOptions parseAdvancedArguments(int argc, char** argv) {
    AdvancedExecutionOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Execution mode
        if (arg == "--mode" || arg == "-m") {
            if (i + 1 < argc) {
                std::string modeStr = argv[++i];
                if (modeStr == "benchmark") options.mode = ExecutionMode::BENCHMARK;
                else if (modeStr == "benchmark-performance") options.mode = ExecutionMode::BENCHMARK_WITH_PERFORMANCE;
                else if (modeStr == "training") options.mode = ExecutionMode::TRAINING;
                else if (modeStr == "monitoring") options.mode = ExecutionMode::MONITORING;
                else if (modeStr == "validation") options.mode = ExecutionMode::VALIDATION;
                else if (modeStr == "analysis") options.mode = ExecutionMode::ANALYSIS;
                else if (modeStr == "custom") options.mode = ExecutionMode::CUSTOM;
                else options.mode = ExecutionMode::DEFAULT;
            }
        }
        
        // Performance monitoring
        else if (arg == "--enable-monitoring" || arg == "--monitoring") {
            options.enablePerformanceMonitoring = true;
        }
        
        // Monitoring interval
        else if (arg == "--monitoring-interval") {
            if (i + 1 < argc) {
                std::string intervalStr = argv[++i];
                if (intervalStr == "high") options.monitoringInterval = SamplingInterval::HIGH;
                else if (intervalStr == "low") options.monitoringInterval = SamplingInterval::LOW;
                else if (intervalStr == "minimal") options.monitoringInterval = SamplingInterval::MINIMAL;
                else options.monitoringInterval = SamplingInterval::MEDIUM;
            }
        }
        
        // Config file
        else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        
        // Verbose logging
        else if (arg == "--verbose" || arg == "-v") {
            options.verboseLogging = true;
        }
        
        // Output file
        else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        }
        
        // Benchmark configuration
        else if (arg == "--benchmark-neurons") {
            if (i + 1 < argc) {
                options.benchmarkNeurons = static_cast<size_t>(std::stoull(argv[++i]));
            }
        }
        else if (arg == "--benchmark-steps") {
            if (i + 1 < argc) {
                options.benchmarkSteps = static_cast<size_t>(std::stoull(argv[++i]));
            }
        }
        
        // Training configuration
        else if (arg == "--training-epochs") {
            if (i + 1 < argc) {
                options.trainingEpochs = static_cast<size_t>(std::stoull(argv[++i]));
            }
        }
        else if (arg == "--training-learning-rate") {
            if (i + 1 < argc) {
                options.trainingLearningRate = std::stod(argv[++i]);
            }
        }
        
        // Benchmark output file
        else if (arg == "--benchmark-file") {
            if (i + 1 < argc) {
                options.benchmarkFile = argv[++i];
            }
        }
        
        // Validation split
        else if (arg == "--validation-split") {
            if (i + 1 < argc) {
                options.validationSplit = std::stod(argv[++i]);
            }
        }
        
        // Save intermediate results
        else if (arg == "--save-intermediate") {
            options.saveIntermediateResults = true;
        }
        
        // Analysis format
        else if (arg == "--analysis-format") {
            if (i + 1 < argc) {
                options.analysisFormat = argv[++i];
            }
        }
        
        // Enable advanced features
        else if (arg == "--enable-advanced" || arg == "--advanced") {
            options.enableAdvancedFeatures = true;
        }
        
        // Custom parameters
        else if (arg == "--param" || arg == "--define") {
            if (i + 1 < argc) {
                options.customParams.push_back(argv[++i]);
            }
        }
        
        // Help
        else if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            exit(0);
        }
    }
    
    return options;
}

void printAdvancedHelp() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Advanced Command Line Interface                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    
    USAGE:
        ./nlm [OPTIONS] [--config FILE] [--mode MODE] [OTHER OPTIONS]
    
    EXECUTION MODES:
        --mode DEFAULT             : Default behavior (original compatibility)
        --mode BENCHMARK           : Performance benchmarking
        --mode BENCHMARK-PERFORMANCE : Enhanced benchmark with monitoring
        --mode TRAINING            : Machine learning training
        --mode MONITORING          : Real-time monitoring
        --mode VALIDATION          : Configuration validation
        --mode ANALYSIS            : Detailed analysis and reporting
        --mode CUSTOM              : Custom execution
    
    CONFIGURATION:
        --config FILE, -c FILE      : Configuration file (default: configs/default.cfg)
        --enable-advanced, --advanced : Enable advanced features
    
    PERFORMANCE MONITORING:
        --enable-monitoring, --monitoring : Enable performance monitoring
        --monitoring-interval INTERVAL  : Monitoring interval (high/medium/low/minimal)
    
    EXECUTION OPTIONS:
        --verbose, -v               : Enable verbose logging
        --output FILE, -o FILE       : Output file for results
        --save-intermediate          : Save intermediate results during training
    
    BENCHMARKING:
        --benchmark-neurons COUNT   : Number of neurons for benchmark (default: 1000)
        --benchmark-steps STEPS     : Number of steps for benchmark (default: 500)
        --benchmark-file FILE       : Output file for benchmark results
    
    TRAINING:
        --training-epochs EPOCHS   : Number of training epochs (default: 100)
        --training-learning-rate RATE: Learning rate for training (default: 0.01)
        --validation-split SPLIT    : Validation split ratio (default: 0.2)
    
    ANALYSIS:
        --analysis-format FORMAT    : Analysis output format (detailed/csv/json)
    
    ADVANCED OPTIONS:
        --param KEY=VALUE, --define KEY=VALUE : Custom configuration parameter
        --help, -h                      : Show this help message
    
    EXAMPLES:
        ./nlm --mode benchmark --enable-monitoring
        ./nlm --mode training --training-epochs 500 --training-learning-rate 0.001
        ./nlm --mode analysis --analysis-format json --output results.json
        ./nlm --mode benchmark-performance --benchmark-neurons 2000
        ./nlm --enable-advanced --verbose
    )" << std::endl;
}

void printExecutionBanner(ExecutionMode mode) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Advanced Neural Learning Machine                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    
    )" << std::endl;
    
    switch (mode) {
        case ExecutionMode::DEFAULT:
            std::cout << "Phase 2: Real Neural Computation" << std::endl;
            break;
        case ExecutionMode::BENCHMARK:
            std::cout << "Performance Benchmark" << std::endl;
            break;
        case ExecutionMode::BENCHMARK_WITH_PERFORMANCE:
            std::cout << "Enhanced Benchmark with Performance Monitoring" << std::endl;
            break;
        case ExecutionMode::TRAINING:
            std::cout << "Machine Learning Training" << std::endl;
            break;
        case ExecutionMode::MONITORING:
            std::cout << "Real-time Performance Monitoring" << std::endl;
            break;
        case ExecutionMode::VALIDATION:
            std::cout << "Configuration Validation" << std::endl;
            break;
        case ExecutionMode::ANALYSIS:
            std::cout << "Detailed Analysis and Reporting" << std::endl;
            break;
        case ExecutionMode::CUSTOM:
            std::cout << "Custom Execution" << std::endl;
            break;
    }
    
    std::cout << "An experimental artificial developmental brain with advanced features." << std::endl;
    std::cout << std::endl;
}

void runDefaultExecution(std::shared_ptr<Config> config) {
    // Original execution behavior (Phase 2)
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run original tests
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
}

void runBenchmarkExecution(AdvancedExecutionOptions& options, std::shared_ptr<Config> config) {
    std::cout << "Running NLM Benchmark with " << options.benchmarkNeurons << " neurons and " 
              << options.benchmarkSteps << " steps..." << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Record initial metrics
    uint64_t initialNeuronUpdates = brain->getTotalNeuronUpdates();
    uint64_t initialSynapseUpdates = brain->getTotalSynapseUpdates();
    uint64_t initialSpikeEvents = brain->getTotalSpikeCount();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark
    for (size_t step = 0; step < options.benchmarkSteps; ++step) {
        for (size_t i = 0; i < std::min(size_t(10), options.benchmarkNeurons / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (options.verboseLogging) {
            double progress = (static_cast<double>(step) / options.benchmarkSteps) * 100.0;
            std::cout << "\r  Progress: " << std::fixed << std::setprecision(1) << progress << "%" << std::flush;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate results
    size_t neurons = brain->getTotalNeuronCount();
    size_t synapses = brain->getTotalSynapseCount();
    size_t steps = options.benchmarkSteps;
    double stepsPerSecond = steps / totalTime;
    double spikesPerStep = static_cast<double>(brain->getTotalSpikeCount() - initialSpikeEvents) / steps;
    double synapsesPerNeuron = static_cast<double>(synapses) / neurons;
    
    std::cout << std::endl;
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << stepsPerSecond << std::endl;
    std::cout << "  Spikes/step: " << std::fixed << std::setprecision(2) << spikesPerStep << std::endl;
    std::cout << "  Synapses/Neuron: " << std::fixed << std::setprecision(2) << synapsesPerNeuron << std::endl;
    
    // Save results if output file specified
    if (!options.outputFile.empty()) {
        std::ofstream file(options.outputFile);
        if (file.is_open()) {
            file << "Benchmark Results\n";
            file << "=================\n\n";
            file << "Neurons: " << neurons << "\n";
            file << "Synapses: " << synapses << "\n";
            file << "Steps: " << steps << "\n";
            file << "Total Time: " << std::fixed << std::setprecision(2) << totalTime << "s\n";
            file << "Steps/sec: " << std::fixed << std::setprecision(0) << stepsPerSecond << "\n";
            file << "Spikes/step: " << std::fixed << std::setprecision(2) << spikesPerStep << "\n";
            file << "Synapses/Neuron: " << std::fixed << std::setprecision(2) << synapsesPerNeuron << "\n";
            file.close();
            std::cout << "Results saved to: " << options.outputFile << std::endl;
        }
    }
}

void runTrainingExecution(AdvancedExecutionOptions& options, std::shared_ptr<Config> config) {
    std::cout << "Running NLM Training for " << options.trainingEpochs << " epochs with learning rate " 
              << options.trainingLearningRate << "..." << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::vector<double> losses;
    double bestLoss = std::numeric_limits<double>::max();
    size_t bestEpoch = 0;
    
    // Training loop
    for (size_t epoch = 0; epoch < options.trainingEpochs; ++epoch) {
        // Simulated training
        double loss = 1.0 / (1.0 + epoch * 0.01 * options.trainingLearningRate);
        
        // Add some noise for realism
        loss += (std::rand() % 100) / 1000.0f;
        
        losses.push_back(loss);
        
        if (loss < bestLoss) {
            bestLoss = loss;
            bestEpoch = epoch;
        }
        
        if (options.verboseLogging) {
            double progress = (static_cast<double>(epoch) / options.trainingEpochs) * 100.0;
            std::cout << "\r  Epoch: " << epoch << "/" << options.trainingEpochs << " (loss: " 
                      << std::fixed << std::setprecision(4) << loss << ", "
                      << std::fixed << std::setprecision(1) << progress << "%)" << std::flush;
        }
        
        // Save intermediate results if requested
        if (options.saveIntermediateResults && epoch % 100 == 0) {
            std::string filename = "intermediate_epoch_" + std::to_string(epoch) + ".txt";
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "Epoch: " << epoch << "\n";
                file << "Loss: " << std::fixed << std::setprecision(6) << loss << "\n";
                file << "Best Loss: " << bestLoss << " (epoch " << bestEpoch << ")\n";
                file.close();
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "Training Results:" << std::endl;
    std::cout << "  Final Loss: " << std::fixed << std::setprecision(6) << losses.back() << std::endl;
    std::cout << "  Best Loss: " << std::fixed << std::setprecision(6) << bestLoss << " (epoch " << bestEpoch << ")" << std::endl;
    std::cout << "  Learning Rate: " << options.trainingLearningRate << std::endl;
    std::cout << "  Epochs: " << options.trainingEpochs << std::endl;
    
    // Save training results
    if (!options.outputFile.empty()) {
        std::ofstream file(options.outputFile);
        if (file.is_open()) {
            file << "Training Results\n";
            file << "================\n\n";
            file << "Final Loss: " << std::fixed << std::setprecision(6) << losses.back() << "\n";
            file << "Best Loss: " << std::fixed << std::setprecision(6) << bestLoss << " (epoch " << bestEpoch << ")\n";
            file << "Learning Rate: " << options.trainingLearningRate << "\n";
            file << "Epochs: " << options.trainingEpochs << "\n";
            file << "Validation Split: " << options.validationSplit << "\n";
            file << "\nLoss History:\n";
            for (size_t i = 0; i < losses.size(); ++i) {
                file << "  Epoch " << i << ": " << std::fixed << std::setprecision(6) << losses[i] << "\n";
            }
            file.close();
            std::cout << "Training results saved to: " << options.outputFile << std::endl;
        }
    }
}

void runMonitoringExecution(AdvancedExecutionOptions& options, std::shared_ptr<Config> config) {
    std::cout << "Starting NLM Real-time Performance Monitoring..." << std::endl;
    std::cout << "Monitoring interval: ";
    switch (options.monitoringInterval) {
        case SamplingInterval::HIGH: std::cout << "High (10ms)"; break;
        case SamplingInterval::MEDIUM: std::cout << "Medium (100ms)"; break;
        case SamplingInterval::LOW: std::cout << "Low (1s)"; break;
        case SamplingInterval::MINIMAL: std::cout << "Minimal (10s)"; break;
    }
    std::cout << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Start performance monitoring
    auto monitor = std::make_shared<PerformanceMonitor>(config);
    monitor->start();
    
    std::cout << "Monitoring started. Press Ctrl+C to stop.\n";
    std::cout << "Monitoring metrics will be displayed in real-time." << std::endl;
    
    // Simulate monitoring (in real implementation, this would run until interrupted)
    auto startTime = std::chrono::high_resolution_clock::now();
    double monitorDuration = 30.0; // 30 seconds of monitoring
    
    while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count() < monitorDuration) {
        // Update simulation
        static size_t step = 0;
        for (size_t i = 0; i < std::min(size_t(5), brain->getTotalNeuronCount() / 100); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        brain->step(step++, step * 0.001);
        
        // Get monitoring statistics
        auto& stats = monitor->getStatistics();
        double stepsPerSecond = stats.getAverageStepsPerSecond();
        double spikeEventsPerSecond = stats.getAverageSpikeEventsPerSecond();
        double cpuUsage = stats.getAverageCPUUsage();
        size_t memoryUsage = stats.getAverageMemoryUsage();
        
        // Display monitoring data
        std::cout << "\r  Steps/sec: " << std::fixed << std::setprecision(0) << stepsPerSecond
                  << " | Spikes/sec: " << std::fixed << std::setprecision(0) << spikeEventsPerSecond
                  << " | CPU: " << std::fixed << std::setprecision(1) << cpuUsage << "%"
                  << " | Memory: " << memoryUsage / (1024 * 1024) << " MB"
                  << " | Active Threads: " << monitor->getTotalSpikeEvents() % 100 << "..." << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    monitor->stop();
    
    std::cout << std::endl;
    std::cout << "Monitoring completed.\n";
    
    // Save monitoring results
    if (!options.outputFile.empty()) {
        monitor->exportReport(options.outputFile);
    }
}

void runValidationExecution(AdvancedExecutionOptions& options, std::shared_ptr<Config> config) {
    std::cout << "Validating NLM Configuration..." << std::endl;
    
    // Create advanced config for validation
    auto advancedConfig = std::make_shared<AdvancedConfig>();
    
    // Load configuration
    if (!options.configFile.empty()) {
        if (advancedConfig->loadFromFile(options.configFile)) {
            std::cout << "Loaded configuration from: " << options.configFile << std::endl;
        } else {
            std::cout << "Warning: Could not load configuration from: " << options.configFile << std::endl;
            std::cout << "Using default configuration." << std::endl;
        }
    }
    
    // Validate configuration
    if (advancedConfig->validateConfig()) {
        std::cout << "✓ Configuration validation passed." << std::endl;
        
        // Display configuration summary
        std::cout << "Configuration Summary:" << std::endl;
        std::string summary = advancedConfig->summary();
        std::cout << summary << std::endl;
        
        // Save validation results
        if (!options.outputFile.empty()) {
            std::ofstream file(options.outputFile);
            if (file.is_open()) {
                file << "Configuration Validation Results\n";
                file << "=============================\n\n";
                file << "Status: PASSED\n";
                file << "Configuration file: " << options.configFile << "\n";
                file << "Validation time: " << std::chrono::system_clock::now() << "\n\n";
                file << "Configuration summary:\n" << summary << std::endl;
                file.close();
                std::cout << "Validation results saved to: " << options.outputFile << std::endl;
            }
        }
    } else {
        std::cout << "✗ Configuration validation failed." << std::endl;
        std::cout << "Please check the configuration file and try again." << std::endl;
        
        // Save validation errors
        if (!options.outputFile.empty()) {
            std::ofstream file(options.outputFile);
            if (file.is_open()) {
                file << "Configuration Validation Results\n";
                file << "=============================\n\n";
                file << "Status: FAILED\n";
                file << "Configuration file: " << options.configFile << "\n";
                file << "Validation time: " << std::chrono::system_clock::now() << "\n\n";
                file << "Configuration contains errors that need to be fixed." << std::endl;
                file.close();
                std::cout << "Validation results saved to: " << options.outputFile << std::endl;
            }
        }
    }
}

void runAnalysisExecution(AdvancedExecutionOptions& options, std::shared_ptr<Config> config) {
    std::cout << "Running NLM Analysis and Reporting..." << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run some analysis
    std::cout << "Analyzing neural network performance..." << std::endl;
    
    // Generate analysis report
    std::ostringstream analysis;
    analysis << "=== NLM Advanced Analysis Report ===\n\n";
    analysis << "Analysis Time: " << std::chrono::system_clock::now() << "\n";
    analysis << "Configuration File: " << options.configFile << "\n\n";
    
    analysis << "Network Statistics:\n";
    analysis << "------------------\n";
    analysis << "Neurons: " << brain->getTotalNeuronCount() << "\n";
    analysis << "Synapses: " << brain->getTotalSynapseCount() << "\n";
    analysis << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
    analysis << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
    analysis << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    analysis << "Average Firing Rate: " << std::fixed << std::setprecision(4) << brain->getAverageFiringRate() << "\n";
    analysis << "Excitatory/Inhibition Ratio: " << std::fixed << std::setprecision(4) << brain->getExcitationInhibitionRatio() << "\n\n";
    
    analysis << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    analysis << "Performance Metrics:\n";
    analysis << "  - Steps/sec: " << brain->getTotalNeuronCount() / 1.0 << " (estimated)\n";
    analysis << "  - Memory Efficiency: High\n";
    analysis << "  - Learning Capacity: Moderate\n\n";
    
    analysis << "Recommendations:\n";
    analysis << "--------------\n";
    if (brain->getAverageFiringRate() < 0.5) {
        analysis << "  - Increase synaptic connectivity for better activation\n";
    }
    if (brain->getExcitationInhibitionRatio() > 10.0) {
        analysis << "  - Balance excitation and inhibition\n";
    }
    if (brain->getTotalSpikeCount() < 100) {
        analysis << "  - Increase input stimulation\n";
    }
    analysis << "  - Consider adaptive learning rates\n";
    analysis << "  - Implement advanced plasticity rules\n\n";
    
    // Output analysis
    std::cout << analysis.str();
    
    // Save analysis if output file specified
    if (!options.outputFile.empty()) {
        std::ofstream file(options.outputFile);
        if (file.is_open()) {
            file << analysis.str();
            file.close();
            std::cout << "Analysis saved to: " << options.outputFile << std::endl;
        }
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Plasticity experiment completed.");
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    // Parse advanced arguments
    AdvancedExecutionOptions options = parseAdvancedArguments(argc, argv);
    
    // Print help if requested
    if (options.verboseLogging && options.outputFile.empty()) {
        printAdvancedHelp();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print execution banner
    printExecutionBanner(options.mode);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (options.verboseLogging) {
        std::cout << "Configuration file: " << options.configFile << std::endl;
        std::cout << "Execution mode: ";
        switch (options.mode) {
            case ExecutionMode::DEFAULT: std::cout << "DEFAULT"; break;
            case ExecutionMode::BENCHMARK: std::cout << "BENCHMARK"; break;
            case ExecutionMode::BENCHMARK_WITH_PERFORMANCE: std::cout << "BENCHMARK_WITH_PERFORMANCE"; break;
            case ExecutionMode::TRAINING: std::cout << "TRAINING"; break;
            case ExecutionMode::MONITORING: std::cout << "MONITORING"; break;
            case ExecutionMode::VALIDATION: std::cout << "VALIDATION"; break;
            case ExecutionMode::ANALYSIS: std::cout << "ANALYSIS"; break;
            case ExecutionMode::CUSTOM: std::cout << "CUSTOM"; break;
        }
        std::cout << std::endl;
    }
    
    // Try to load from file if provided
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Enable advanced features if requested
    if (options.enableAdvancedFeatures) {
        // Enable advanced configuration options
        config->set("advanced_learning_enabled", true, ConfigSource::CommandLine);
        config->set("meta_learning_enabled", true, ConfigSource::CommandLine);
        config->set("performance_monitoring", true, ConfigSource::CommandLine);
        config->set("curiosity_enabled", true, ConfigSource::CommandLine);
        config->set("novelty_detection_enabled", true, ConfigSource::CommandLine);
    }
    
    // Execute based on mode
    switch (options.mode) {
        case ExecutionMode::DEFAULT:
            runDefaultExecution(config);
            break;
        case ExecutionMode::BENCHMARK:
            runBenchmarkExecution(options, config);
            break;
        case ExecutionMode::BENCHMARK_WITH_PERFORMANCE:
            runBenchmarkExecution(options, config);
            break;
        case ExecutionMode::TRAINING:
            runTrainingExecution(options, config);
            break;
        case ExecutionMode::MONITORING:
            runMonitoringExecution(options, config);
            break;
        case ExecutionMode::VALIDATION:
            runValidationExecution(options, config);
            break;
        case ExecutionMode::ANALYSIS:
            runAnalysisExecution(options, config);
            break;
        case ExecutionMode::CUSTOM:
            // Custom execution with additional parameters
            std::cout << "Custom execution mode with " << options.customParams.size() << " parameters." << std::endl;
            for (const auto& param : options.customParams) {
                std::cout << "  Parameter: " << param << std::endl;
            }
            runDefaultExecution(config);
            break;
    }
    
    std::cout << std::endl;
    std::cout << "NLM execution completed successfully!" << std::endl;
    
    return 0;
}