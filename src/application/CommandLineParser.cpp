#include "application/CommandLineParser.hpp"
#include <cstdlib>
#include <algorithm>
#include <cctype>

namespace nlm {
    // CommandLineParser implementation
    void CommandLineParser::parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                help = true;
                printHelp();
                exit(0);
            } else if (arg == "--version") {
                version = true;
                std::cout << "NLM v0.1.0 Phase 2" << std::endl;
                exit(0);
            } else if (arg.find("--config") == 0) {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg.find("--timestep") == 0) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--neuron-count") == 0) {
                neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--region-count") == 0) {
                regionCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--connection-prob") == 0) {
                connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--max-steps") == 0) {
                maxSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--experiment-type") == 0) {
                experimentType = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--log-level") == 0) {
                logLevel = arg.substr(arg.find('=') + 1);
            } else if (arg.find("--output-file") == 0) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (arg == "--visualization") {
                visualization = true;
            } else if (arg == "--disable-plasticity") {
                disablePlasticity = true;
            } else if (arg == "--enable-structural-plasticity") {
                enableStructuralPlasticity = true;
            } else if (arg.find("--seed") == 0) {
                randomSeed = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (arg.find("--option") == 0 && arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                customOptions[key] = value;
            }
        }
    }
    
    void CommandLineParser::printHelp() {
        std::cout << "Usage: nlm [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Core Options:" << std::endl;
        std::cout << "  --config FILE        Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --help               Print this help message" << std::endl;
        std::cout << "  --version            Print version information" << std::endl;
        std::cout << std::endl;
        std::cout << "Experiment Options:" << std::endl;
        std::cout << "  --timestep VALUE     Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --neuron-count VALUE Number of neurons (default: 500)" << std::endl;
        std::cout << "  --region-count VALUE Number of regions (default: 1)" << std::endl;
        std::cout << "  --connection-prob VALUE Connection probability (default: 0.15)" << std::endl;
        std::cout << "  --max-steps VALUE    Maximum simulation steps (default: 10000)" << std::endl;
        std::cout << "  --experiment-type VALUE Experiment type: 0=all, 1=connectivity, 2=plasticity, 3=stdp (default: 0)" << std::endl;
        std::cout << std::endl;
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --log-level LEVEL    Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL (default: INFO)" << std::endl;
        std::cout << "  --verbose            Enable verbose output" << std::endl;
        std::cout << "  --output-file FILE   Write output to file" << std::endl;
        std::cout << "  --visualization      Enable visualization" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --disable-plasticity           Disable plasticity" << std::endl;
        std::cout << "  --enable-structural-plasticity Enable structural plasticity" << std::endl;
        std::cout << "  --seed VALUE            Random seed (default: 42)" << std::endl;
        std::cout << "  --option KEY=VALUE      Add custom configuration option" << std::endl;
    }
}