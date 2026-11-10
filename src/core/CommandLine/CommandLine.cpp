#include "CommandLine.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineArgs CommandLineProcessor::parse(int argc, char** argv) {
    CommandLineArgs parsedArgs;
    parsedArgs.programName = argv[0];
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle help and version flags
        if (arg == "--help" || arg == "-help" || arg == "--h") {
            parsedArgs.helpRequested = true;
            continue;
        }
        
        if (arg == "--version" || arg == "-version" || arg == "--v") {
            parsedArgs.versionRequested = true;
            continue;
        }
        
        if (arg == "--list-config" || arg == "--list-keys") {
            parsedArgs.listConfigKeys = true;
            continue;
        }
        
        if (arg == "--validate-only") {
            parsedArgs.validateOnly = true;
            continue;
        }
        
        // Handle key=value pairs
        if (arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Remove leading dashes
            if (key.find("-") == 0) {
                key = key.substr(1);
            }
            
            parsedArgs.options[key] = value;
            continue;
        }
        
        // Handle flag-only arguments
        if (arg.find("-") == 0 && arg.size() > 1) {
            std::string key = arg.substr(1);
            // Check if it's a known key that should have an argument
            if (key == "config" || key == "output" || key == "log-level" || 
                key == "profile" || key == "input" || key == "prompt") {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    parsedArgs.options[key] = argv[++i];
                } else {
                    logCommandLineError("Missing argument for option: " + key);
                    parsedArgs.helpRequested = true;
                }
                continue;
            }
            
            // It's a flag
            if (key == "enable-profiling") {
                parsedArgs.enableProfiling = true;
            } else if (key == "verbose") {
                parsedArgs.logLevel = "debug";
            } else {
                parsedArgs.options[key] = "true";
            }
            continue;
        }
        
        // Positional arguments
        parsedArgs.positionalArgs.push_back(arg);
    }
    
    return parsedArgs;
}

std::string CommandLineProcessor::generateHelp(const std::string& programName) const {
    std::ostringstream help;
    help << "NLM (Neural Learning Machine) - Advanced Artificial Brain Framework\n";
    help << "===================================================================\n\n";
    help << "USAGE:\n";
    help << "  " << programName << " [OPTIONS] [ARGS]...\n\n";
    help << "CORE OPTIONS:\n";
    help << "  --help, -help            Show this help message and exit\n";
    help << "  --version, -v            Show version information and exit\n";
    help << "  --config FILE            Configuration file to load\n";
    help << "  --output FILE, -o FILE   Output file for results\n";
    help << "  --log-level LEVEL        Set logging level (off, error, warning, info, debug)\n";
    help << "  --enable-profiling       Enable performance profiling\n";
    help << "  --list-config, -l        List all available configuration keys\n";
    help << "  --validate-only          Validate configuration only, don't run\n";
    help << "\nNEURAL NETWORK OPTIONS:\n";
    help << "  --neuron-count NUM       Number of neurons in the network (default: 1000)\n";
    help << "  --region-count NUM       Number of neural regions (default: 1)\n";
    help << "  --connection-prob FLOAT  Connection probability (0.0-1.0, default: 0.1)\n";
    help << "  --stdp-weights LTP,LTD    STDP learning rates (default: 0.01,0.012)\n";
    help << "  --plasticity             Enable synaptic plasticity\n";
    help << "\nMEMORY OPTIONS:\n";
    help << "  --working-memory         Enable working memory system\n";
    help << "  --episodic-memory        Enable episodic memory system\n";
    help << "  --memory-capacity NUM    Memory capacity (default: 100)\n";
    help << "\nBEHAVIORAL OPTIONS:\n";
    help << "  --curiosity              Enable curiosity-driven learning\n";
    help << "  --novelty                Enable novelty detection\n";
    help << "  --reward-modulation      Enable reward modulation\n";
    help << "\nDEVELOPMENT OPTIONS:\n";
    help << "  --enable-development     Enable developmental stages\n";
    help << "  --stage STAGE            Development stage (initial, critical, maturation, adult)\n";
    help << "\nCHECKPOINTING OPTIONS:\n";
    help << "  --checkpoint-dir DIR     Directory for checkpoints\n";
    help << "  --checkpoint-interval NUM Checkpoint save interval (default: 100)\n";
    help << "  --load-checkpoint FILE   Load from checkpoint file\n";
    help << "  --save-checkpoint FILE   Save to checkpoint file\n";
    help << "\nADVANCED MODES:\n";
    help << "  --batch INPUT FILE        Batch processing mode\n";
    help << "  --interactive            Interactive mode\n";
    help << "  --debug                  Enable debug mode\n";
    help << "  --verbose, -v            Enable verbose logging\n";
    help << "\nEXPERIMENTAL OPTIONS:\n";
    help << "  --enable-memory-tracking Enable detailed memory tracking\n";
    help << "  --step-by-step           Step-by-step execution\n";
    help << "  --generate-profile       Generate performance profile\n";
    help << "\nCONFIGURATION FILE FORMAT:\n";
    help << "  KEY = VALUE              Configuration key-value pairs\n";
    help << "  # comment                Comments start with #\n";
    help << "  [SECTION]               Section headers in brackets\n";
    help << "\nEXAMPLES:\n";
    help << "  " << programName << " --neuron-count 5000 --connection-prob 0.15 --enable-profiling\n";
    help << "  " << programName << " --config myconfig.cfg --output results.json --enable-development\n";
    help << "  " << programName << " --batch inputs/ --output outputs/ --enable-profiling\n\n";
    help << "For more information, visit: https://github.com/nlm-project/nlm\n";
    
    return help.str();
}

std::string CommandLineProcessor::generateUsage() const {
    std::ostringstream usage;
    usage << "NLM Neural Learning Machine\n";
    usage << "Usage: program [options] [args...]\n";
    usage << "Options can be specified in configuration files or as command line arguments.\n";
    usage << "Run with --help for detailed information.\n";
    
    return usage.str();
}

bool CommandLineProcessor::validateArguments(const CommandLineArgs& args) const {
    bool isValid = true;
    
    // Validate numeric options
    if (args.options.find("neuron-count") !== args.options.end()) {
        try {
            size_t neuronCount = std::stoul(args.options.at("neuron-count"));
            if (neuronCount < 1 || neuronCount > 10000) {
                logCommandLineError("Invalid neuron-count: " + args.options.at("neuron-count") + ". Must be between 1 and 10000");
                isValid = false;
            }
        } catch (const std::exception& e) {
            logCommandLineError("Invalid neuron-count format: " + args.options.at("neuron-count"));
            isValid = false;
        }
    }
    
    if (args.options.find("region-count") !== args.options.end()) {
        try {
            size_t regionCount = std::stoul(args.options.at("region-count"));
            if (regionCount < 1 || regionCount > 100) {
                logCommandLineError("Invalid region-count: " + args.options.at("region-count") + ". Must be between 1 and 100");
                isValid = false;
            }
        } catch (const std::exception& e) {
            logCommandLineError("Invalid region-count format: " + args.options.at("region-count"));
            isValid = false;
        }
    }
    
    if (args.options.find("connection-prob") !== args.options.end()) {
        try {
            double connProb = std::stod(args.options.at("connection-prob"));
            if (connProb < 0.0 || connProb > 1.0) {
                logCommandLineError("Invalid connection-prob: " + args.options.at("connection-prob") + ". Must be between 0.0 and 1.0");
                isValid = false;
            }
        } catch (const std::exception& e) {
            logCommandLineError("Invalid connection-prob format: " + args.options.at("connection-prob"));
            isValid = false;
        }
    }
    
    // Validate file options
    if (args.options.find("config") !== args.options.end()) {
        std::string configFile = args.options.at("config");
        std::ifstream file(configFile);
        if (!file.is_open()) {
            logCommandLineError("Cannot open config file: " + configFile);
            isValid = false;
        }
    }
    
    return isValid;
}

void CommandLineProcessor::processCommonArgs(CommandLineArgs& args) {
    // Process common development mode arguments
    if (args.options.find("dev") !== args.options.end() || 
        args.options.find("debug") !== args.options.end()) {
        args.logLevel = "debug";
        args.enableProfiling = true;
    }
    
    // Process profiling arguments
    if (args.enableProfiling) {
        if (args.options.find("profile-interval") !== args.options.end()) {
            try {
                args.profilingInterval = std::stoul(args.options.at("profile-interval"));
            } catch (const std::exception& e) {
                logCommandLineWarning("Invalid profile-interval: " + args.options.at("profile-interval"));
            }
        }
    }
}

void CommandLineProcessor::processNLMArgs(CommandLineArgs& args) {
    // Process NLM-specific configuration arguments
    // Convert dash-separated options to underscore-separated for internal use
    std::map<std::string, std::string> convertedArgs;
    
    for (const auto& [key, value] : args.options) {
        // Convert --neuron-count to neuron_count
        std::string internalKey = key;
        for (size_t i = 0; i < internalKey.size(); ++i) {
            if (internalKey[i] == '-') {
                internalKey[i] = '_';
            }
        }
        convertedArgs[internalKey] = value;
    }
    
    // Update options with converted keys
    args.options = convertedArgs;
}

std::string CommandLineProcessor::getHelpForKey(const std::string& key) const {
    // Return help information for specific configuration key
    std::map<std::string, std::string> keyHelp;
    
    keyHelp["neuron-count"] = "Number of neurons in the artificial brain network";
    keyHelp["region-count"] = "Numberikea";
    keyHelp["connection-prob"] = "Probability of random synaptic connections";
    keyHelp["stdp-ltp-weight"] = "STDP Long-Term Potentiation weight";
    keyHelp["stdp-ltd-weight"] = "STDP Long-Term Depression weight";
    keyHelp["working-memory"] = "Enable working memory system";
    keyHelp["episodic-memory"] = "Enable episodic memory system";
    keyHelp["curiosity"] = "Enable curiosity-driven exploration";
    keyHelp["novelty"] = "Enable novelty detection";
    keyHelp["reward-modulation"] = "Enable reward-based learning";
    
    if (keyHelp.find(key) !== keyHelp.end()) {
        return keyHelp.at(key);
    }
    
    return "No help available for key: " + key;
}

std::vector<std::string> CommandLineProcessor::listAvailableKeys() const {
    std::vector<std::string> keys;
    
    // Common NLM configuration keys
    keys = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "synaptogenesis_rate",
        "pruning_rate",
        "working_memory_capacity",
        "episodic_memory_max_episodes",
        "enable_curiosity",
        "enable_novelty",
        "enable_reward_modulation",
        "developmental_stage",
        "checkpoint_interval",
        "replay_interval",
        "consolidation_interval"
    };
    
    return keys;
}

} // namespace nlm
