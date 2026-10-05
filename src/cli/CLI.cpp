#include "cli/CLI.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

namespace nlm {

CLI::Options CLI::parseArgs(int argc, char** argv) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.mode = Mode::HELP;
        } else if (arg == "--version") {
            options.mode = Mode::VERSION;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
            options.verbose = false;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
            options.quiet = false;
        } else if (arg == "--timing" || arg == "--time") {
            options.timing = true;
        } else if (arg == "--save-results" || arg == "--output") {
            options.saveResults = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.outputDir = argv[++i];
            }
        } else if (arg == "--duration") {
            if (i + 1 < argc && std::isdigit(argv[i + 1][0])) {
                options.duration = std::stoull(argv[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                options.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc && std::isdigit(argv[i + 1][0])) {
                options.neuronCount = std::stoll(argv[++i]);
            }
        } else if (arg == "--regions") {
            if (i + 1 < argc && std::isdigit(argv[i + 1][0])) {
                options.regionCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--conn-prob") {
            if (i + 1 < argc) {
                options.connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg == "--advanced") {
            options.enableAdvancedFeatures = true;
        } else if (arg == "--experiment" || arg == "-e") {
            if (i + 1 < argc) {
                options.customExperiment = argv[++i];
            }
        } else if (arg == "--param") {
            if (i + 1 < argc) {
                // Parse key=value format
                std::string param = argv[++i];
                size_t eqPos = param.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = param.substr(0, eqPos);
                    std::string value = param.substr(eqPos + 1);
                    options.advancedParams[key] = value;
                } else {
                    options.advancedParams[param] = "true";
                }
            }
        } else if (arg == "--connectivity" || arg == "-c") {
            options.mode = Mode::CONNECTIVITY;
        } else if (arg == "--plasticity" || arg == "-p") {
            options.mode = Mode::PLASTICITY;
        } else if (arg == "--stdp" || arg == "-s") {
            options.mode = Mode::STDP;
        } else if (arg == "--all" || arg == "-a") {
            options.mode = Mode::ALL;
        } else if (arg == "--config-mode") {
            options.mode = Mode::CONFIG;
        } else if (arg.substr(0, 2) == "--") {
            // Unknown option
            std::cerr << "Warning: Unknown option " << arg << std::endl;
        }
    }
    
    return options;
}

void CLI::printHelp() {
    std::cout << "\nUsage: nlm [options]\n\n";
    std::cout << "Modes:\n";
    std::cout << "  --connectivity, -c    Run basic connectivity test\n";
    std::cout << "  --plasticity, -p      Run plasticity learning experiment\n";
    std::cout << "  --stdp, -s            Run STDP verification\n";
    std::cout << "  --all, -a             Run all tests\n";
    std::cout << "  (default)             Run complete test suite\n\n";
    std::cout << "Options:\n";
    std::cout << "  --config <file>       Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "  --quiet, -q           Quiet mode (errors only)\n";
    std::cout << "  --verbose, -v         Verbose output\n";
    std::cout << "  --timing, --time      Show execution timing\n";
    std::cout << "  --save-results        Save detailed results\n";
    std::cout << "  --output <dir>        Output directory for saved results\n\n";
    std::cout << "Advanced Options:\n";
    std::cout << "  --neurons <n>         Number of neurons (default: 500)\n";
    std::cout << "  --regions <n>         Number of regions (default: 1)\n";
    std::cout << "  --conn-prob <p>       Connection probability (default: 0.15)\n";
    std::cout << "  --timestep <t>        Simulation timestep (default: 0.001)\n";
    std::cout << "  --advanced            Enable advanced features\n";
    std::cout << "  --experiment <name>   Run custom experiment\n";
    std::cout << "  --param <key>=<val>   Set advanced parameter\n\n";
    std::cout << "Examples:\n";
    std::cout << "  nlm --connectivity --verbose\n";
    std::cout << "  nlm --plasticity --timing\n";
    std::cout << "  nlm --all --save-results --output=./results\n";
}

void CLI::printVersion() {
    std::cout << "\nNLM (熙然) - Neural Learning Machine\n";
    std::cout << "Version 2.1.0 (Enhanced)\n";
    std::cout << "Phase 6: Final Integration Complete\n";
    std::cout << "Build with improved modularity, CLI, and advanced features.\n";
}

CLI::Mode CLI::detectMode(const std::string& arg) {
    if (arg == "--connectivity" || arg == "-c") return Mode::CONNECTIVITY;
    if (arg == "--plasticity" || arg == "-p") return Mode::PLASTICITY;
    if (arg == "--stdp" || arg == "-s") return Mode::STDP;
    if (arg == "--all" || arg == "-a") return Mode::ALL;
    if (arg == "--config") return Mode::CONFIG;
    if (arg == "--help" || arg == "-h") return Mode::HELP;
    if (arg == "--version") return Mode::VERSION;
    return Mode::DEFAULT;
}

} // namespace nlm