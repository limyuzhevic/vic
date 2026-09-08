// Implementation of CLIParser
// This file provides the complete implementation of the improved command-line interface

#include "CLIParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <memory>
#include <cassert>

namespace nlm {

CLIParser::CLIParser(int argc, char** argv) {
    if (!parse(argc, argv)) {
        valid_ = false;
    }
}

bool CLIParser::parse(int argc, char** argv) {
    if (argc < 1) {
        error_ = "Invalid argument count";
        return false;
    }
    
    return parseArguments(argc, argv);
}

bool CLIParser::parseArguments(int argc, char** argv) {
    // Skip program name
    for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
        std::string arg = argv[i];
        
        // Help and version
        if (arg == "--help" || arg == "-h") {
            printHelp("nlm");
            return false; // Exit after help
        }
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false; // Exit after version
        }
        
        // Subcommand handling
        if (arg == "neuron" || arg == "synapse" || arg == "plasticity" || 
            arg == "brain" || arg == "test") {
            if (!handleSubcommand(arg, i, argc, argv)) {
                error_ = "Failed to handle subcommand: " + arg;
                return false;
            }
            continue;
        }
        
        // Short options
        if (arg[0] == '-') {
            if (!parseShortOption(arg[1], arg, i, argc, argv)) {
                error_ = "Failed to parse short option: " + arg;
                return false;
            }
        }
        // Long options
        else if (arg.substr(0, 2) == "--") {
            if (!parseLongOption(arg, i, argc, argv)) {
                error_ = "Failed to parse long option: " + arg;
                return false;
            }
        }
        // Positional arguments (checkpoint files, etc.)
        else {
            if (options_.checkpointLoad.empty()) {
                options_.checkpointLoad = arg;
            } else if (options_.checkpointSave.empty()) {
                options_.checkpointSave = arg;
            } else {
                error_ = "Unexpected positional argument: " + arg;
                return false;
            }
        }
    }
    
    // Validate options
    if (!validateOptions()) {
        return false;
    }
    
    return true;
}

bool CLIParser::parseShortOption(char c, const std::string& arg, size_t& i, 
                                 int argc, char** argv) {
    switch (c) {
        case 'h':
            printHelp("nlm");
            return false;
            
        case 'v':
            printVersion();
            return false;
            
        case 'c': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.configFile = value;
            }
            break;
        }
        
        case 't': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                if (!handleTestSubcommand(value, i, argc, argv)) {
                    return false;
                }
            }
            break;
        }
        
        case 'r': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.simulationRunTime = std::stod(value);
            }
            break;
        }
        
        case 'd': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.timestep = std::stod(value);
            }
            break;
        }
        
        case 's': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.steps = std::stoi(value);
            }
            break;
        }
        
        case 'o': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.outputFile = value;
            }
            break;
        }
        
        case 'q':
            options_.quiet = true;
            options_.verbose = false;
            break;
            
        case 'V':
            options_.verbose = true;
            break;
            
        case 'j':
            options_.jsonOutput = true;
            break;
            
        case 'x': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.randomSeed = std::stoull(value);
                options_.showSeed = true;
            }
            break;
        }
        
        case 'p':
            options_.profile = true;
            break;
            
        case 'm':
            options_.measureMemory = true;
            break;
            
        case 'n':
            options_.countNeurons = true;
            break;
            
        case 'y':
            options_.countSynapses = true;
            break;
            
        case 'g': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.checkpointSave = value;
            }
            break;
        }
        
        case 'l': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.checkpointLoad = value;
            }
            break;
        }
        
        case 'z':
            options_.enableVisualization = true;
            break;
            
        case 'L': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                setLogLevelFromString(value);
            }
            break;
        }
        
        case 'C': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.enableCategories.push_back(value);
            }
            break;
        }
        
        case 'D': {
            std::string value;
            if (arg.find('=') != std::string::npos) {
                value = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < static_cast<size_t>(argc)) {
                value = argv[++i];
            }
            if (!value.empty()) {
                options_.disableCategories.push_back(value);
            }
            break;
        }
        
        case 'P':
            options_.progressIndicators = false;
            break;
            
        case 'f':
            options_.csvOutput = true;
            options_.humanReadable = false;
            break;
            
        case 'J':
            options_.jsonOutputFormat = true;
            options_.humanReadable = false;
            break;
            
        default:
            error_ = "Unknown short option: -" + std::string(1, c);
            return false;
    }
    
    return true;
}

bool CLIParser::parseLongOption(const std::string& arg, size_t& i, 
                                int argc, char** argv) {
    std::string key = arg.substr(2);
    std::string value;
    
    // Extract value from --key=value
    size_t pos = key.find('=');
    if (pos != std::string::npos) {
        value = key.substr(pos + 1);
        key = key.substr(0, pos);
    } else if (i + 1 < static_cast<size_t>(argc) && argv[i + 1][0] != '-') {
        // Next argument is the value
        value = argv[++i];
    }
    
    if (key == "config") {
        if (!value.empty()) {
            options_.configFile = value;
        }
    } else if (key == "test") {
        if (!value.empty()) {
            if (!handleTestSubcommand(value, i, argc, argv)) {
                return false;
            }
        }
    } else if (key == "runtime") {
        if (!value.empty()) {
            options_.simulationRunTime = std::stod(value);
        }
    } else if (key == "timestep") {
        if (!value.empty()) {
            options_.timestep = std::stod(value);
        }
    } else if (key == "steps") {
        if (!value.empty()) {
            options_.steps = std::stoi(value);
        }
    } else if (key == "output") {
        if (!value.empty()) {
            options_.outputFile = value;
        }
    } else if (key == "verbose") {
        options_.verbose = true;
        options_.quiet = false;
    } else if (key == "quiet") {
        options_.quiet = true;
        options_.verbose = false;
    } else if (key == "json") {
        options_.jsonOutput = true;
    } else if (key == "seed") {
        if (!value.empty()) {
            options_.randomSeed = std::stoull(value);
            options_.showSeed = true;
        }
    } else if (key == "show-seed") {
        options_.showSeed = true;
    } else if (key == "profile") {
        options_.profile = true;
    } else if (key == "memory") {
        options_.measureMemory = true;
    } else if (key == "spikes") {
        options_.countSpikes = true;
    } else if (key == "neurons") {
        options_.countNeurons = true;
    } else if (key == "synapses") {
        options_.countSynapses = true;
    } else if (key == "checkpoint-save") {
        if (!value.empty()) {
            options_.checkpointSave = value;
        }
    } else if (key == "checkpoint-load") {
        if (!value.empty()) {
            options_.checkpointLoad = value;
        }
    } else if (key == "visualization") {
        options_.enableVisualization = true;
    } else if (key == "log-level") {
        if (!value.empty()) {
            setLogLevelFromString(value);
        }
    } else if (key == "enable-category") {
        if (!value.empty()) {
            options_.enableCategories.push_back(value);
        }
    } else if (key == "disable-category") {
        if (!value.empty()) {
            options_.disableCategories.push_back(value);
        }
    } else if (key == "no-progress") {
        options_.progressIndicators = false;
    } else if (key == "csv") {
        options_.csvOutput = true;
        options_.humanReadable = false;
    } else if (key == "json-format") {
        options_.jsonOutputFormat = true;
        options_.humanReadable = false;
    } else {
        error_ = "Unknown long option: --" + key;
        return false;
    }
    
    return true;
}

bool CLIParser::handleSubcommand(const std::string& cmd, size_t& i, 
                                 int argc, char** argv) {
    if (cmd == "neuron" || cmd == "synapse" || cmd == "plasticity" || 
        cmd == "brain") {
        options_.runAllTests = false;
        if (cmd == "neuron") options_.runNeuronTests = true;
        else if (cmd == "synapse") options_.runSynapseTests = true;
        else if (cmd == "plasticity") options_.runPlasticityTests = true;
        else if (cmd == "brain") options_.runBrainTests = true;
    } else if (cmd == "test") {
        options_.runAllTests = true;
    }
    return true;
}

bool CLIParser::handleTestSubcommand(const std::string& testName, 
                                     size_t& i, int argc, char** argv) {
    if (testName == "neuron") {
        options_.runNeuronTests = true;
    } else if (testName == "synapse") {
        options_.runSynapseTests = true;
    } else if (testName == "plasticity") {
        options_.runPlasticityTests = true;
    } else if (testName == "brain") {
        options_.runBrainTests = true;
    } else {
        error_ = "Unknown test: " + testName;
        return false;
    }
    return true;
}

bool CLIParser::validateOptions() const {
    // Check for conflicting options
    if (options_.quiet && options_.verbose) {
        error_ = "Cannot specify both --quiet and --verbose";
        return false;
    }
    
    if (options_.jsonOutput && options_.csvOutput) {
        error_ = "Cannot specify both --json and --csv output formats";
        return false;
    }
    
    if (options_.jsonOutput && options_.humanReadable) {
        error_ = "Cannot specify both JSON output and human-readable format";
        return false;
    }
    
    if (options_.simulationRunTime <= 0 && options_.steps <= 0) {
        error_ = "Must specify either --runtime or --steps for simulation";
        return false;
    }
    
    if (options_.checkpointSave.empty() && options_.checkpointLoad.empty()) {
        // Valid, just no checkpoint operations
    }
    
    return true;
}

void CLIParser::printHelp(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [OPTIONS] [SUBCOMMANDS]" << std::endl;
    std::cout << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -c, --config FILE       Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Modes:" << std::endl;
    std::cout << "  --test TYPE             Run specific test (neuron, synapse, plasticity, brain)" << std::endl;
    std::cout << "  --test neuron           Run neuron dynamics test" << std::endl;
    std::cout << "  --test synapse          Run synapse plasticity test" << std::endl;
    std::cout << "  --test plasticity       Run Hebbian/STDP plasticity test" << std::endl;
    std::cout << "  --test brain            Run brain integration test" << std::endl;
    std::cout << "  -t, --test MODE         Alias for test mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Control:" << std::endl;
    std::cout << "  -r, --runtime SECONDS   Simulation run time in seconds" << std::endl;
    std::cout << "  -d, --timestep MS       Simulation timestep in milliseconds" << std::endl;
    std::cout << "  -s, --steps N           Number of simulation steps to run" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  -V, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet             Suppress output (quiet mode)" << std::endl;
    std::cout << "  -j, --json              Enable JSON output for tests" << std::endl;
    std::cout << "  -o, --output FILE       Write output to file" << std::endl;
    std::cout << "  -f, --csv               Enable CSV output format" << std::endl;
    std::cout << "  -J, --json-format       Enable JSON format output" << std::endl;
    std::cout << std::endl;
    std::cout << "Random Seed:" << std::endl;
    std::cout << "  -x, --seed NUM          Set random seed" << std::endl;
    std::cout << "  --show-seed             Display random seed" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance Profiling:" << std::endl;
    std::cout << "  -p, --profile           Enable performance profiling" << std::endl;
    std::cout << "  -m, --memory            Measure memory usage" << std::endl;
    std::cout << "  -n, --neurons           Count neurons" << std::endl;
    std::cout << "  -y, --synapses          Count synapses" << std::endl;
    std::cout << "  -g, --checkpoint-save FILE Save checkpoint to file" << std::endl;
    std::cout << "  -l, --checkpoint-load FILE Load checkpoint from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Visualization:" << std::endl;
    std::cout << "  -z, --visualization      Enable visualization" << std::endl;
    std::cout << std::endl;
    std::cout << "Logging Control:" << std::endl;
    std::cout << "  -L, --log-level LEVEL   Set log level (debug, info, warning, error)" << std::endl;
    std::cout << "  -C, --enable-category CAT Enable log category" << std::endl;
    std::cout << "  -D, --disable-category CAT Disable log category" << std::endl;
    std::cout << "  -P, --no-progress       Disable progress indicators" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --test neuron --runtime 10 --verbose" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg --runtime 5 --json --output results.json" << std::endl;
    std::cout << "  " << programName << " --profile --memory --neurons --synapses --steps 1000" << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << "NLM (熙然) Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
}

void CLIParser::setLogLevelFromString(const std::string& level) {
    std::string levelStr = level;
    std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
    
    if (levelStr == "debug") {
        options_.logLevel = LogLevel::Debug;
    } else if (levelStr == "info") {
        options_.logLevel = LogLevel::Info;
    } else if (levelStr == "warning") {
        options_.logLevel = LogLevel::Warning;
    } else if (levelStr == "error") {
        options_.logLevel = LogLevel::Error;
    } else {
        error_ = "Invalid log level: " + level;
    }
}

std::string CLIParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

PerformanceStats CLIParser::runSimulation(std::shared_ptr<Brain> brain) {
    PerformanceStats stats;
    startProfiling();
    
    // Set random seed if specified
    if (options_.showSeed) {
        std::cout << "Random seed: " << options_.randomSeed << std::endl;
    }
    
    // Determine simulation duration
    double runTime = options_.simulationRunTime;
    int steps = options_.steps;
    
    if (runTime > 0) {
        steps = static_cast<int>(runTime / options_.timestep);
    }
    
    std::cout << "Running simulation for " << steps << " steps (" 
              << (runTime > 0 ? std::to_string(runTime) + "s" : "custom") << ")" << std::endl;
    
    // Run simulation
    for (int i = 0; i < steps; ++i) {
        brain->step(i, i * options_.timestep);
        
        if (options_.progressIndicators && i % 100 == 0) {
            std::cout << "\rStep: " << i << "/" << steps 
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() << std::flush;
        }
        
        if (options_.countSpikes) {
            incrementSpikeCount();
        }
        
        if (options_.countNeurons) {
            incrementNeuronCount();
        }
        
        if (options_.countSynapses) {
            incrementSynapseCount();
        }
    }
    
    std::cout << std::endl;
    stopProfiling();
    
    stats.spikesGenerated = brain->getTotalSpikeCount();
    stats.neuronsCreated = brain->getTotalNeuronCount();
    stats.synapsesCreated = brain->getTotalSynapseCount();
    
    return stats;
}

bool CLIParser::runTests(std::shared_ptr<Brain> brain) {
    bool allPassed = true;
    
    if (options_.runNeuronTests) {
        std::cout << "Running neuron dynamics test..." << std::endl;
        runBasicConnectivityTest(brain);
    }
    
    if (options_.runSynapseTests) {
        std::cout << "Running synapse plasticity test..." << std::endl;
        runStdpVerification(brain);
    }
    
    if (options_.runPlasticityTests) {
        std::cout << "Running plasticity learning test..." << std::endl;
        runPlasticityExperiment(brain);
    }
    
    if (options_.runBrainTests) {
        std::cout << "Running brain integration test..." << std::endl;
        // Run comprehensive brain test
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    return allPassed;
}

void CLIParser::saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot save checkpoint to " << filepath << std::endl;
        return;
    }
    
    // Write brain state to checkpoint
    // For now, just write a placeholder
    file << "NLM_CHECKPOINT_v1" << std::endl;
    file << "simulation_run_time: " << options_.simulationRunTime << std::endl;
    file << "timestep: " << options_.timestep << std::endl;
    file << "steps: " << options_.steps << std::endl;
    file << "spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    file.close();
    std::cout << "Checkpoint saved to: " << filepath << std::endl;
}

bool CLIParser::loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot load checkpoint from " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("simulation_run_time: ") == 0) {
            options_.simulationRunTime = std::stod(line.substr(23));
        } else if (line.find("timestep: ") == 0) {
            options_.timestep = std::stod(line.substr(10));
        } else if (line.find("steps: ") == 0) {
            options_.steps = std::stoi(line.substr(8));
        }
    }
    
    file.close();
    std::cout << "Checkpoint loaded from: " << filepath << std::endl;
    return true;
}

void CLIParser::outputJSON(const PerformanceStats& stats, std::ostream& out) const {
    out << "{\n";
    out << "  \"simulation_stats\": {\n";
    out << "    \"elapsed_seconds\": " << stats.getElapsedSeconds() << ",\n";
    out << "    \"spikes_generated\": " << stats.spikesGenerated << ",\n";
    out << "    \"neurons_created\": " << stats.neuronsCreated << ",\n";
    out << "    \"synapses_created\": " << stats.synapsesCreated << ",\n";
    out << "    \"peak_memory_bytes\": " << stats.peakMemoryUsage << "\n";
    out << "  },\n";
    out << "  \"config\": {\n";
    out << "    \"timestep\": " << options_.timestep << ",\n";
    out << "    \"steps\": " << options_.steps << "\n";
    out << "  }\n";
    out << "}\n";
}

void CLIParser::outputCSV(const PerformanceStats& stats, std::ostream& out) const {
    out << "metric,value\n";
    out << "elapsed_seconds," << stats.getElapsedSeconds() << "\n";
    out << "spikes_generated," << stats.spikesGenerated << "\n";
    out << "neurons_created," << stats.neuronsCreated << "\n";
    out << "synapses_created," << stats.synapsesCreated << "\n";
    out << "peak_memory_bytes," << stats.peakMemoryUsage << "\n";
}

void CLIParser::outputHumanReadable(const PerformanceStats& stats, std::ostream& out) const {
    out << "=== NLM Simulation Results ===\n";
    out << "\nPerformance Statistics:" << std::endl;
    out << "  Simulation time: " << stats.getElapsedSeconds() << " seconds\n";
    out << "  Spikes generated: " << stats.spikesGenerated << "\n";
    out << "  Neurons created: " << stats.neuronsCreated << "\n";
    out << "  Synapses created: " << stats.synapsesCreated << "\n";
    out << "  Peak memory usage: " << stats.peakMemoryUsage << " bytes\n";
}

void CLIParser::startProfiling() {
    perfStats_.startTime = std::chrono::high_resolution_clock::now();
}

void CLIParser::stopProfiling() {
    perfStats_.endTime = std::chrono::high_resolution_clock::now();
}

void CLIParser::incrementSpikeCount() {
    perfStats_.spikesGenerated++;
}

void CLIParser::incrementNeuronCount() {
    perfStats_.neuronsCreated++;
}

void CLIParser::incrementSynapseCount() {
    perfStats_.synapsesCreated++;
}

void CLIParser::updateMemoryUsage(size_t memory) {
    perfStats_.peakMemoryUsage = std::max(perfStats_.peakMemoryUsage, memory);
}

void CLIParser::initializeOutputStream(std::ofstream& outFile) const {
    if (!options_.outputFile.empty()) {
        outFile.open(options_.outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Warning: Cannot open output file " << options_.outputFile << std::endl;
        }
    }
}

void CLIParser::writeOutput(const PerformanceStats& stats) const {
    std::ofstream outFile;
    initializeOutputStream(outFile);
    std::ostream& out = outFile.is_open() ? outFile : std::cout;
    
    if (options_.csvOutput) {
        outputCSV(stats, out);
    } else if (options_.jsonOutput || options_.jsonOutputFormat) {
        outputJSON(stats, out);
    } else {
        outputHumanReadable(stats, out);
    }
    
    outFile.close();
}

} // namespace nlm
