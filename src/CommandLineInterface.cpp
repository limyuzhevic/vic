#include "CommandLineInterface.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace nlm {

struct CommandLineInterface::Impl {
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    bool helpRequested;
    bool versionRequested;
    bool verboseMode;
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    uint64_t seed;
    int64_t neuronCount;
    int64_t regionCount;
    float connectionProbability;
    
    Impl() : helpRequested(false), versionRequested(false), verboseMode(false),
             seed(42), neuronCount(1000), regionCount(1), connectionProbability(0.1f) {}
};

CommandLineInterface::CommandLineInterface() : pImpl(std::make_unique<Impl>()) {}

CommandLineInterface::~CommandLineInterface() = default;

void CommandLineInterface::printHelp() const {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Usage:" << std::endl;
    std::cout << "  nlm [options]" << std::endl;
    std::cout << "  nlm --config=config.json --help" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Core Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  --neurons=N             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions=N             Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --conn-prob=P            Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --seed=N                Random seed (default: 42)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "File I/O:" << std::endl;
    std::cout << "  --config=FILE            Configuration file (JSON/YAML/simple)" << std::endl;
    std::cout << "  --output=FILE            Output file for results" << std::endl;
    std::cout << "  --log-level=L            Logging level: debug, info, warning, error" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --steps=N                Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --checkpoint=PATH         Enable checkpointing with directory" << std::endl;
    std::cout << "  --develop                 Enable developmental stages" << std::endl;
    std::cout << "  --replay                 Enable memory replay" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation:" << std::endl;
    std::cout << "  --dopamine               Enable dopamine-based learning" << std::endl;
    std::cout << "  --curiosity              Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --novelty                Enable novelty detection" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Plasticity:" << std::endl;
    std::cout << "  --stdp                   Enable Spike-Timing-Dependent Plasticity" << std::endl;
    std::cout << "  --hebbian                Enable Hebbian learning" << std::endl;
    std::cout << "  --reward-modulated       Enable reward-modulated plasticity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --load=FILE              Load brain state from checkpoint" << std::endl;
    std::cout << "  --save=FILE              Save brain state after simulation" << std::endl;
    std::cout << "  --demo                   Run integration demo" << std::endl;
    std::cout << "  --experiment=TYPE        Run specific experiment (phase3, phase4, phase6)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example Commands:" << std::endl;
    std::cout << "  nlm --help                    # Show help" << std::endl;
    std::cout << "  nlm --neurons=500 --steps=2000  # Quick simulation" << std::endl;
    std::cout << "  nlm --config=sim.cfg --save=brain.bin  # Load config, run, save brain" << std::endl;
    std::cout << "  nlm --develop --curiosity --replay  # Advanced development simulation" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printVersion() const {
    std::cout << "NLM (熙然) v0.1.0 - Phase 6: Final Integration" << std::endl;
    std::cout << "An experimental artificial developmental brain." << std::endl;
    std::cout << "C++20 implementation with focus on biologically realistic neural dynamics." << std::endl;
    std::cout << std::endl;
}

bool CommandLineInterface::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            pImpl->helpRequested = true;
        } else if (arg == "--version") {
            pImpl->versionRequested = true;
        } else if (arg == "--verbose" || arg == "-v") {
            pImpl->verboseMode = true;
        } else if (arg.find("--neurons=") == 0) {
            try {
                pImpl->neuronCount = std::stoll(arg.substr(9));
            } catch (...) {
                std::cerr << "Warning: Invalid neuron count, using default" << std::endl;
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                pImpl->regionCount = std::stoll(arg.substr(9));
            } catch (...) {
                std::cerr << "Warning: Invalid region count, using default" << std::endl;
            }
        } else if (arg.find("--conn-prob=") == 0) {
            try {
                pImpl->connectionProbability = std::stof(arg.substr(12));
            } catch (...) {
                std::cerr << "Warning: Invalid connection probability, using default" << std::endl;
            }
        } else if (arg.find("--seed=") == 0) {
            try {
                pImpl->seed = std::stoull(arg.substr(7));
            } catch (...) {
                std::cerr << "Warning: Invalid seed, using default" << std::endl;
            }
        } else if (arg.find("--config=") == 0) {
            pImpl->configFile = arg.substr(9);
        } else if (arg.find("--output=") == 0) {
            pImpl->outputFile = arg.substr(10);
        } else if (arg.find("--log-level=") == 0) {
            pImpl->logLevel = arg.substr(12);
        } else {
            pImpl->args.push_back(arg);
        }
    }
    
    return true;
}

void CommandLineInterface::printStartupBanner() const {
    std::cout << "=== NLM (熙然) Neural Brain Simulator ===" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Artificial Developmental Brain with Full System Integration" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printConfigurationSummary() const {
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::cout << "Neuron count: " << pImpl->neuronCount << std::endl;
    std::cout << "Region count: " << pImpl->regionCount << std::endl;
    std::cout << "Connection probability: " << pImpl->connectionProbability << std::endl;
    std::cout << "Random seed: " << pImpl->seed << std::endl;
    
    if (!pImpl->configFile.empty()) {
        std::cout << "Config file: " << pImpl->configFile << std::endl;
    }
    
    if (pImpl->verboseMode) {
        std::cout << "Verbose mode: Enabled" << std::endl;
    }
    
    if (!pImpl->logLevel.empty()) {
        std::cout << "Log level: " << pImpl->logLevel << std::endl;
    }
    
    std::cout << std::endl;
}

const std::map<std::string, std::string>& CommandLineInterface::getOptions() const {
    return pImpl->options;
}

const std::vector<std::string>& CommandLineInterface::getArgs() const {
    return pImpl->args;
}

bool CommandLineInterface::isHelpRequested() const {
    return pImpl->helpRequested;
}

bool CommandLineInterface::isVersionRequested() const {
    return pImpl->versionRequested;
}

bool CommandLineInterface::isVerboseMode() const {
    return pImpl->verboseMode;
}

const std::string& CommandLineInterface::getConfigFile() const {
    return pImpl->configFile;
}

const std::string& CommandLineInterface::getOutputFile() const {
    return pImpl->outputFile;
}

const std::string& CommandLineInterface::getLogLevel() const {
    return pImpl->logLevel;
}

uint64_t CommandLineInterface::getSeed() const {
    return pImpl->seed;
}

int64_t CommandLineInterface::getNeuronCount() const {
    return pImpl->neuronCount;
}

int64_t CommandLineInterface::getRegionCount() const {
    return pImpl->regionCount;
}

float CommandLineInterface::getConnectionProbability() const {
    return pImpl->connectionProbability;
}

void CommandLineInterface::setOption(const std::string& key, const std::string& value) {
    pImpl->options[key] = value;
}

std::string CommandLineInterface::getOption(const std::string& key) const {
    auto it = pImpl->options.find(key);
    if (it != pImpl->options.end()) {
        return it->second;
    }
    return "";
}

bool CommandLineInterface::hasOption(const std::string& key) const {
    return pImpl->options.find(key) != pImpl->options.end();
}

CommandLineInterface::CommandLineInterface(CommandLineInterface&&) noexcept = default;
CommandLineInterface& CommandLineInterface::operator=(CommandLineInterface&&) noexcept = default;

} // namespace nlm
