// Command line options implementation

#include "main.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace nlm;

CommandLineOptions::Options CommandLineOptions::parse(int argc, char** argv) {
    Options opts;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            opts.mode = Mode::HELP;
        } else if (arg == "--simulate" || arg == "-s") {
            opts.mode = Mode::SIMULATION;
        } else if (arg == "--connectivity-test") {
            opts.mode = Mode::CONNECTIVITY_TEST;
        } else if (arg == "--plasticity-test") {
            opts.mode = Mode::PLASTICITY_TEST;
        } else if (arg == "--stdp-test") {
            opts.mode = Mode::STDP_TEST;
        } else if (arg == "--benchmark" || arg == "-b") {
            opts.mode = Mode::BENCHMARK;
        } else if (arg == "--config-export") {
            opts.mode = Mode::CONFIG_EXPORT;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                opts.customArgs["config_file"] = arg.substr(arg.find('=') + 1);
                opts.configFile = opts.customArgs["config_file"];
            } else if (i + 1 < argc) {
                opts.customArgs["config_file"] = argv[++i];
                opts.configFile = opts.customArgs["config_file"];
            }
        } else if (arg.substr(0, 7) == "--output") {
            if (arg.find('=') != std::string::npos) {
                opts.outputFile = arg.substr(arg.find('=') + 1);
            }
        } else if (arg.substr(0, 8) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                opts.neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            }
        } else if (arg.substr(0, 9) == "--duration") {
            if (arg.find('=') != std::string::npos) {
                opts.duration = std::stoul(arg.substr(arg.find('=') + 1));
            }
        } else if (arg.substr(0, 8) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                opts.timestep = std::stod(arg.substr(arg.find('=') + 1));
            }
        } else if (arg == "--verbose" || arg == "-v") {
            opts.verbose = true;
        } else if (arg == "--headless") {
            opts.headless = true;
        } else if (arg == "--save-on-exit") {
            opts.saveOnExit = true;
        } else if (arg.substr(0, 12) == "--test-mode=") {
            opts.testMode = arg.substr(12);
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return opts;
}

void CommandLineOptions::printHelp(Mode context) {
    printBanner();
    
    std::cout << "\n=== NLM Command Line Interface ===" << std::endl;
    std::cout << "Usage: nlm [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Modes:" << std::endl;
    std::cout << "  --simulate, -s          Run standard simulation (default)" << std::endl;
    std::cout << "  --connectivity-test     Run basic connectivity test" << std::endl;
    std::cout << "  --plasticity-test       Run plasticity learning experiment" << std::endl;
    std::cout << "  --stdp-test             Run STDP verification test" << std::endl;
    std::cout << "  --benchmark, -b         Run benchmark and performance tests" << std::endl;
    std::cout << "  --config-export         Export current configuration to file" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --config=FILE           Load configuration from file" << std::endl;
    std::cout << "  --output=FILE           Output file (checkpoint, results, etc.)" << std::endl;
    std::cout << "  --neurons=N             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --duration=N            Duration in steps (default: 1000)" << std::endl;
    std::cout << "  --timestep=T            Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --test-mode=MODE        Which tests to run (all, connectivity, plasticity, stdp)" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
    std::cout << "  --headless              Run without GUI/visualization" << std::endl;
    std::cout << "  --save-on-exit          Save brain state on normal exit" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --simulate --neurons=500 --duration=5000" << std::endl;
    std::cout << "  nlm --plasticity-test --config=my_config.cfg" << std::endl;
    std::cout << "  nlm --benchmark -v --output=benchmark_results.txt" << std::endl << std::endl;
    
    if (context == Mode::CONFIG_EXPORT) {
        std::cout << "Configuration export mode: Exports current config to specified file." << std::endl;
    } else if (context == Mode::BENCHMARK) {
        std::cout << "Benchmark mode: Measures performance and outputs statistics." << std::endl;
    }
}

bool CommandLineOptions::validateOptions(const Options& opts) {
    if (opts.mode == Mode::HELP) {
        printHelp();
        return false;
    }
    
    if (opts.neuronCount == 0) {
        std::cerr << "Error: neuron count must be greater than 0" << std::endl;
        return false;
    }
    
    if (opts.duration == 0) {
        std::cerr << "Error: duration must be greater than 0" << std::endl;
        return false;
    }
    
    if (opts.timestep <= 0.0) {
        std::cerr << "Error: timestep must be positive" << std::endl;
        return false;
    }
    
    if (opts.testMode != "all" && opts.testMode != "connectivity" && 
        opts.testMode != "plasticity" && opts.testMode != "stdp") {
        std::cerr << "Error: invalid test mode. Must be: all, connectivity, plasticity, or stdp" << std::endl;
        return false;
    }
    
    return true;
}