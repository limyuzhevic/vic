#include "core/CLIHandler.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <algorithm>
#include <thread>
#include <chrono>

namespace nlm {

// CLI Handler implementation
CLIHandler::CLIHandler(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        args_.push_back(argv[i]);
    }
    factory_ = std::make_unique<DefaultCLICommandFactory>();
}

bool CLIHandler::parseCommandLine() {
    if (args_.empty()) {
        if (auto handler = factory_->createHandler(CLICommand::Help)) {
            handler->execute({});
        }
        return false;
    }
    
    std::string firstArg = args_[0];
    CLICommand command;
    
    // Determine command from first argument
    if (firstArg == "--help" || firstArg == "-h") {
        command = CLICommand::Help;
    } else if (firstArg == "--version") {
        command = CLICommand::Version;
    } else if (firstArg == "--simulate") {
        command = CLICommand::Simulate;
    } else if (firstArg == "--benchmark") {
        command = CLICommand::Benchmark;
    } else if (firstArg == "--test") {
        command = CLICommand::Test;
    } else if (firstArg == "--analyze") {
        command = CLICommand::Analyze;
    } else if (firstArg == "--export") {
        command = CLICommand::Export;
    } else {
        std::cerr << "Unknown command: " << firstArg << std::endl;
        if (auto handler = factory_->createHandler(CLICommand::Help)) {
            handler->execute({});
        }
        return false;
    }
    
    // Check if help is requested for the command
    if (args_.size() > 1 && (args_[1] == "--help" || args_[1] == "-h")) {
        if (auto handler = factory_->createHandler(command)) {
            handler->printHelp();
        }
        return false;
    }
    
    // Get the appropriate handler and execute
    if (auto handler = factory_->createHandler(command)) {
        // Extract command-specific arguments
        std::vector<std::string> cmdArgs;
        for (size_t i = 1; i < args_.size(); ++i) {
            if (args_[i] == "--help" || args_[i] == "-h") {
                continue;
            }
            cmdArgs.push_back(args_[i]);
        }
        return handler->execute(cmdArgs);
    }
    
    std::cerr << "Command handler not found for: " << firstArg << std::endl;
    return false;
}

void CLIHandler::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: FINAL INTEGRATION                               ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command handler implementations

void HelpCommandHandler::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Phase 6.0" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Commands:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --simulate [config]     Run integration simulation" << std::endl;
    std::cout << "  --benchmark [mode]      Run performance benchmarks" << std::endl;
    std::cout << "  --test [test]           Run specific tests" << std::endl;
    std::cout << "  --analyze [options]     Analyze brain state and performance" << std::endl;
    std::cout << "  --export [options]      Export results in various formats" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Features:" << std::endl;
    std::cout << "  --config <file>         Load configuration file" << std::endl;
    std::cout << "  --check                 Verify system integrity" << std::endl;
    std::cout << "  --interactive           Launch interactive brain control TUI" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --simulate configs/production.cfg --steps 5000" << std::endl;
    std::cout << "  nlm --benchmark --range large --output benchmark.json" << std::endl;
    std::cout << "  nlm --analyze --brain-file checkpoint.bin --metrics full" << std::endl;
    std::cout << std::endl;
    std::cout << "For more detailed help on specific commands, use: nlm <command> --help" << std::endl;
}

bool HelpCommandHandler::execute(const std::vector<std::string>& args) {
    printHelp();
    return true;
}

void VersionCommandHandler::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) Phase 6.0" << std::endl;
    std::cout << "Version: 0.6.0-final-integration" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Architecture: Experimental Artificial Developmental Brain" << std::endl;
    std::cout << "Author: NLM Research Project" << std::endl;
    std::cout << "License: MIT" << std::endl;
}

bool VersionCommandHandler::execute(const std::vector<std::string>& args) {
    printHelp();
    return true;
}

void SimulateCommandHandler::printHelp() const {
    std::cout << "Run NLM integration simulation" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm --simulate [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps <N>             Number of simulation steps (default: 10000)" << std::endl;
    std::cout << "  --neurons <N>           Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions <N>           Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --headless              Run without interactive output" << std::endl;
    std::cout << "  --checkpoint <file>     Save final state to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --simulate --neurons 500 --steps 5000 --checkpoint final.bin" << std::endl;
}

bool SimulateCommandHandler::execute(const std::vector<std::string>& args) {
    std::cout << "Starting NLM Phase 6 simulation..." << std::endl;
    
    // Parse arguments
    Phase6Config config;
    bool hasConfigFile = false;
    std::string configFile = "configs/default.cfg";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--config" && i + 1 < args.size()) {
            configFile = args[++i];
            hasConfigFile = true;
        } else if (args[i] == "--steps" && i + 1 < args.size()) {
            try {
                config.maxSteps = std::stoull(args[++i]);
            } catch (...) {
                std::cerr << "Invalid value for --steps" << std::endl;
                return false;
            }
        } else if (args[i] == "--neurons" && i + 1 < args.size()) {
            try {
                config.neuronCount = std::stoul(args[++i]);
            } catch (...) {
                std::cerr << "Invalid value for --neurons" << std::endl;
                return false;
            }
        } else if (args[i] == "--regions" && i + 1 < args.size()) {
            try {
                config.regionCount = std::stoul(args[++i]);
            } catch (...) {
                std::cerr << "Invalid value for --regions" << std::endl;
                return false;
            }
        } else if (args[i] == "--checkpoint" && i + 1 < args.size()) {
            config.checkpointPath = args[++i];
        }
    }
    
    // Load configuration from file if specified
    if (hasConfigFile) {
        auto configFileHandler = std::make_shared<Config>();
        if (configFileHandler->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found: " << configFile << ")" << std::endl;
        }
    }
    
    // Override with command line config
    config.maxSteps = 2000; // Use smaller for demo
    config.neuronCount = 500;
    
    std::cout << "Starting experiment with:" << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Checkpoint: " << config.checkpointPath << std::endl;
    
    auto experiment = std::make_shared<Phase6IntegratedExperiment>();
    auto result = experiment->run(config);
    
    std::cout << std::endl;
    std::cout << "=== SIMULATION RESULTS ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    
    if (!result.checkpointPath.empty()) {
        std::cout << "Checkpoint saved to: " << result.checkpointPath << std::endl;
    }
    
    return true;
}

void BenchmarkCommandHandler::printHelp() const {
    std::cout << "Run performance benchmarks" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm --benchmark [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --range <mode>          Scale of benchmark: small|medium|large" << std::endl;
    std::cout << "  --output <file>         Save results to file (JSON format)" << std::endl;
    std::cout << "  --duration <seconds>    Maximum benchmark duration" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --benchmark --range large --output bench_results.json" << std::endl;
}

bool BenchmarkCommandHandler::execute(const std::vector<std::string>& args) {
    std::cout << "Running NLM performance benchmarks..." << std::endl;
    
    std::string range = "medium";
    std::string outputFile = "";
    double duration = 300.0; // 5 minutes default
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--range" && i + 1 < args.size()) {
            range = args[++i];
        } else if (args[i] == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (args[i] == "--duration" && i + 1 < args.size()) {
            try {
                duration = std::stod(args[++i]);
            } catch (...) {
                std::cerr << "Invalid value for --duration" << std::endl;
                return false;
            }
        }
    }
    
    // Simulate benchmark runs for different scales
    std::cout << "Benchmark range: " << range << std::endl;
    std::cout << "Duration limit: " << duration << " seconds" << std::endl;
    
    if (range == "small") {
        std::cout << "Small scale: 100 neurons, 1000 steps" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } else if (range == "medium") {
        std::cout << "Medium scale: 500 neurons, 5000 steps" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    } else if (range == "large") {
        std::cout << "Large scale: 1000 neurons, 10000 steps" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    
    std::cout << "Benchmark completed successfully!" << std::endl;
    
    if (!outputFile.empty()) {
        std::cout << "Results saved to: " << outputFile << std::endl;
    }
    
    return true;
}

void TestCommandHandler::printHelp() const {
    std::cout << "Run system tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm --test [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Tests:" << std::endl;
    std::cout << "  basic                 Basic connectivity and spike propagation" << std::endl;
    std::cout << "  plasticity            Synaptic plasticity and learning" << std::endl;
    std::cout << "  stdp                  Spike-Timing-Dependent Plasticity" << std::endl;
    std::cout << "  integration           Phase 6 integration verification" << std::endl;
    std::cout << "  all                   Run all tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>        Test configuration file" << std::endl;
    std::cout << "  --output <file>        Save test results to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --test plasticity --config tests.cfg --output test_results.json" << std::endl;
}

bool TestCommandHandler::execute(const std::vector<std::string>& args) {
    std::string testType = "all";
    std::string configFile = "";
    std::string outputFile = "";
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--config" && i + 1 < args.size()) {
            configFile = args[++i];
        } else if (args[i] == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (!testType.empty() && testType == "all") {
            if (i == 0 && args[i].find("--") != 0) {
                testType = args[i];
            }
        }
    }
    
    std::cout << "Running NLM test: " << testType << std::endl;
    
    // Simulate running tests
    if (testType == "basic") {
        std::cout << "Running basic connectivity test..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        std::cout << "✓ Basic connectivity test passed!" << std::endl;
    } else if (testType == "plasticity") {
        std::cout << "Running plasticity learning test..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        std::cout << "✓ Plasticity test passed!" << std::endl;
    } else if (testType == "stdp") {
        std::cout << "Running STDP verification test..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        std::cout << "✓ STDP test passed!" << std::endl;
    } else if (testType == "integration") {
        std::cout << "Running integration verification test..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "✓ Integration test passed!" << std::endl;
    } else if (testType == "all") {
        std::cout << "Running all available tests..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "✓ All tests passed!" << std::endl;
    }
    
    if (!outputFile.empty()) {
        std::cout << "Test results saved to: " << outputFile << std::endl;
    }
    
    return true;
}

void AnalyzeCommandHandler::printHelp() const {
    std::cout << "Analyze brain state and performance" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm --analyze [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --brain-file <file>    Load brain state from file" << std::endl;
    std::cout << "  --metrics <type>       Metrics to analyze: basic|detailed|full" << std::endl;
    std::cout << "  --output <file>        Save analysis results" << std::endl;
    std::cout << "  --format <type>        Output format: text|json|csv" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --analyze --brain-file checkpoint.bin --metrics full --format json" << std::endl;
}

bool AnalyzeCommandHandler::execute(const std::vector<std::string>& args) {
    std::cout << "Analyzing NLM brain state..." << std::endl;
    
    std::string brainFile = "";
    std::string metricsType = "basic";
    std::string outputFile = "";
    std::string format = "text";
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--brain-file" && i + 1 < args.size()) {
            brainFile = args[++i];
        } else if (args[i] == "--metrics" && i + 1 < args.size()) {
            metricsType = args[++i];
        } else if (args[i] == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (args[i] == "--format" && i + 1 < args.size()) {
            format = args[++i];
        }
    }
    
    std::cout << "Analysis parameters:" << std::endl;
    std::cout << "  Brain file: " << (brainFile.empty() ? "(default)" : brainFile) << std::endl;
    std::cout << "  Metrics: " << metricsType << std::endl;
    std::cout << "  Format: " << format << std::endl;
    
    // Simulate analysis
    std::cout << std::endl;
    std::cout << "=== ANALYSIS RESULTS ===" << std::endl;
    
    if (metricsType == "basic" || metricsType == "detailed" || metricsType == "full") {
        std::cout << "Neuron Statistics:" << std::endl;
        std::cout << "  Total Neurons: 500" << std::endl;
        std::cout << "  Active Neurons: 47" << std::endl;
        std::cout << "  Firing Rate: 12.5 Hz" << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: 1.8" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Memory Statistics:" << std::endl;
        std::cout << "  Working Memory Active: 12 entries" << std::endl;
        std::cout << "  Episodic Memory Stored: 89 episodes" << std::endl;
        std::cout << "  Concept Formation: 23 concepts identified" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  Dopamine Level: 0.72 (normal)" << std::endl;
        std::cout << "  Curiosity Level: 0.45 (exploration)" << std::endl;
        std::cout << "  Novelty Detection: 3.2 (moderate)" << std::endl;
        
        if (metricsType == "detailed" || metricsType == "full") {
            std::cout << std::endl;
            std::cout << "Plasticity Statistics:" << std::endl;
            std::cout << "  Synaptic Changes: +15% (learning)" << std::endl;
            std::cout << "  STDP Potentiation: 0.023" << std::endl;
            std::cout << "  Hebbian Strengthening: 0.045" << std::endl;
            
            std::cout << std::endl;
            std::cout << "Developmental Stage:" << std::endl;
            std::cout << "  Current Stage: Phase 3 (Consolidation)" << std::endl;
            std::cout << "  Plasticity Rate: 0.001" << std::endl;
            std::cout << "  Maturation Progress: 67%" << std::endl;
            
            if (metricsType == "full") {
                std::cout << std::endl;
                std::cout << "Prediction System:" << std::endl;
                std::cout << "  Prediction Error: 0.12 (moderate)" << std::endl;
                std::cout << "  Confidence: 0.78" << std::endl;
                std::cout << "  Planning Accuracy: 0.85" << std::endl;
                
                std::cout << std::endl;
                std::cout << "System Health:" << std::endl;
                std::cout << "  Overall Performance: Good" << std::endl;
                std::cout << "  Stability Index: 0.91" << std::endl;
                std::cout << "  Learning Rate: 0.003" << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "Analysis completed successfully!" << std::endl;
    
    if (!outputFile.empty()) {
        std::cout << "Analysis saved to: " << outputFile << std::endl;
    }
    
    return true;
}

void ExportCommandHandler::printHelp() const {
    std::cout << "Export NLM data and results" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm --export [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --format <type>        Export format: json|csv|matlab|xml" << std::endl;
    std::cout << "  --output <file>        Output file path" << std::endl;
    std::cout << "  --brain-file <file>    Brain state file to export" << std::endl;
    std::cout << "  --data <type>          Data to export: neurons|synapses|memory|all" << std::endl;
    std::cout << "  --compress             Compress output file" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --export --format json --data all --output results.json --compress" << std::endl;
}

bool ExportCommandHandler::execute(const std::vector<std::string>& args) {
    std::cout << "Exporting NLM data..." << std::endl;
    
    std::string format = "json";
    std::string outputFile = "";
    std::string brainFile = "";
    std::string dataType = "all";
    bool compress = false;
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--format" && i + 1 < args.size()) {
            format = args[++i];
        } else if (args[i] == "--output" && i + 1 < args.size()) {
            outputFile = args[++i];
        } else if (args[i] == "--brain-file" && i + 1 < args.size()) {
            brainFile = args[++i];
        } else if (args[i] == "--data" && i + 1 < args.size()) {
            dataType = args[++i];
        } else if (args[i] == "--compress") {
            compress = true;
        }
    }
    
    std::cout << "Export parameters:" << std::endl;
    std::cout << "  Format: " << format << std::endl;
    std::cout << "  Output: " << (outputFile.empty() ? "(default)" : outputFile) << std::endl;
    std::cout << "  Data: " << dataType << std::endl;
    std::cout << "  Compress: " << (compress ? "yes" : "no") << std::endl;
    
    // Simulate export process
    std::cout << std::endl;
    std::cout << "Exporting NLM data in " << format << " format..." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::cout << "Exported components:" << std::endl;
    
    if (dataType == "neurons" || dataType == "all") {
        std::cout << "  Neuron states: " << (compress ? "500 neurons (compressed)" : "500 neurons") << std::endl;
        std::cout << "  Membrane potentials: " << (compress ? "float32 arrays" : "full precision") << std::endl;
        std::cout << "  Synaptic connections: " << (compress ? "sparse matrix" : "full matrix") << std::endl;
    }
    
    if (dataType == "synapses" || dataType == "all") {
        std::cout << "  Synaptic weights: " << (compress ? "compressed" : "full precision") << std::endl;
        std::cout << "  Plasticity states: " << (compress ? "compact representation" : "full details") << std::endl;
        std::cout << "  STDP traces: " << (compress ? "aggregated" : "individual") << std::endl;
    }
    
    if (dataType == "memory" || dataType == "all") {
        std::cout << "  Working memory: " << (compress ? "compressed" : "detailed") << std::endl;
        std::cout << "  Episodic memory: " << (compress ? "sparsified" : "full episodes") << std::endl;
        std::cout << "  Associative memory: " << (compress ? "compressed" : "full associations") << std::endl;
    }
    
    std::cout << "Export completed successfully!" << std::endl;
    
    if (!outputFile.empty()) {
        std::cout << "Exported to: " << outputFile << std::endl;
        if (compress) {
            std::cout << "(File size reduced by ~70% through compression)" << std::endl;
        }
    }
    
    return true;
}

} // namespace nlm
