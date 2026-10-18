#include "CommandLineParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineParser::CommandLineParser(int argc, char** argv) {
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options_["help"] = "true";
        } else if (arg == "--version") {
            options_["version"] = "true";
        } else if (arg == "--experiments") {
            options_["list_experiments"] = "true";
        } else if (arg.substr(0, 2) == "--") {
            // Option with potential value
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(0, eqPos);
                std::string value = arg.substr(eqPos + 1);
                options_[key] = value;
            } else {
                // Check if next argument is a value
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    options_[arg] = argv[++i];
                } else {
                    options_[arg] = "true";
                }
            }
        } else {
            // Positional argument
            positionalArgs_.push_back({arg, "", ""});
        }
    }
}

std::string CommandLineParser::getOption(const std::string& flag, const std::string& defaultValue) const {
    auto it = options_.find(flag);
    if (it != options_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool CommandLineParser::hasOption(const std::string& flag) const {
    return options_.find(flag) != options_.end();
}

void CommandLineParser::printHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine\n" << std::endl;
    std::cout << "Usage: nlm [options] [experiment]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --config=<file>         Load configuration from file" << std::endl;
    std::cout << "  --neuron_count=<num>    Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region_count=<num>    Number of regions (default: 1)" << std::endl;
    std::cout << "  --simulation_timestep=<t> Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random_seed=<seed>    Random seed (default: 42)" << std::endl;
    std::cout << "  --experiment=<name>     Run specific experiment (auto, basic, plasticity, stdp, all)" << std::endl;
    std::cout << "  --output=<file>         Save brain state to file" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << "  --quiet                 Suppress non-essential output" << std::endl;
    std::cout << "  --experiments           List available experiments" << std::endl;
    std::cout << "  --no-plasticity         Disable plasticity" << std::endl;
    std::cout << "  --no-development        Disable development" << std::endl;
    std::cout << "  --learning-only         Only run learning experiments" << std::endl;
    
    std::cout << std::endl << "Available Experiments:" << std::endl;
    printAvailableExperiments();
    
    std::cout << std::endl << "Examples:" << std::endl;
    std::cout << "  nlm --experiment=basic                    # Run basic connectivity test" << std::endl;
    std::cout << "  nlm --experiment=plasticity              # Run plasticity learning experiment" << std::endl;
    std::cout << "  nlm --experiment=stdp                    # Run STDP verification" << std::endl;
    std::cout << "  nlm --experiment=all --neuron_count=2000  # Run all experiments with more neurons" << std::endl;
    std::cout << "  nlm --config=myconfig.cfg --output=result.bin  # Load config and save output" << std::endl;
}

void CommandLineParser::printAvailableExperiments() {
    std::cout << "  basic        Run basic neural connectivity test" << std::endl;
    std::cout << "  plasticity   Run plasticity learning experiment" << std::endl;
    std::cout << "  stdp         Run STDP verification test" << std::endl;
    std::cout << "  all          Run all available experiments in sequence" << std::endl;
    std::cout << "  agent        Run agent-based experiment (if available)" << std::endl;
}

void CommandLineParser::applyToConfig(std::shared_ptr<Config> config) const {
    if (hasOption("--neuron_count")) {
        int64_t neurons = std::stoll(getOption("--neuron_count", "500"));
        config->set("neuron_count", neurons, ConfigSource::CommandLine);
    }
    
    if (hasOption("--region_count")) {
        int64_t regions = std::stoll(getOption("--region_count", "1"));
        config->set("region_count", regions, ConfigSource::CommandLine);
    }
    
    if (hasOption("--simulation_timestep")) {
        double timestep = std::stod(getOption("--simulation_timestep", "0.001"));
        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    }
    
    if (hasOption("--random_seed")) {
        int64_t seed = std::stoll(getOption("--random_seed", "42"));
        config->set("random_seed", seed, ConfigSource::CommandLine);
    }
    
    if (hasOption("--stdp_ltp_weight")) {
        float ltp = std::stof(getOption("--stdp_ltp_weight"));
        config->set("stdp_ltp_weight", ltp, ConfigSource::CommandLine);
    }
    
    if (hasOption("--stdp_ltd_weight")) {
        float ltd = std::stof(getOption("--stdp_ltd_weight"));
        config->set("stdp_ltd_weight", ltd, ConfigSource::CommandLine);
    }
    
    if (hasOption("--synaptogenesis_rate")) {
        float synRate = std::stof(getOption("--synaptogenesis_rate"));
        config->set("synaptogenesis_rate", synRate, ConfigSource::CommandLine);
    }
    
    if (hasOption("--pruning_rate")) {
        float pruneRate = std::stof(getOption("--pruning_rate"));
        config->set("pruning_rate", pruneRate, ConfigSource::CommandLine);
    }
}

} // namespace nlm