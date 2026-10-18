// Command line argument parsing utilities implementation
#include "CommandLineUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Convert string to lowercase
static std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Trim whitespace from both ends
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

CommandLineOptions CommandLineParser::parse(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        }
        else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        }
        else if (arg == "--debug" || arg == "-d") {
            options.debug = true;
        }
        else if (arg == "--benchmark") {
            options.benchmark = true;
        }
        else if (arg == "--save-config") {
            options.saveConfig = true;
        }
        else if (arg == "--no-learning") {
            options.enableLearning = false;
        }
        else if (arg == "--no-development") {
            options.enableDevelopment = false;
        }
        else if (arg == "--no-neuromodulation") {
            options.enableNeuromodulation = false;
        }
        else if (arg == "--visualization") {
            options.enableVisualization = true;
        }
        else if (arg == "--steps") {
            if (i + 1 < argc) {
                options.maxSteps = std::stol(argv[++i]);
            }
        }
        else if (arg == "--timestep") {
            if (i + 1 < argc) {
                options.timestep = std::stod(argv[++i]);
            }
        }
        else if (arg == "--seed") {
            if (i + 1 < argc) {
                options.randomSeed = std::stoull(argv[++i]);
            }
        }
        else if (arg == "--learning-rate") {
            if (i + 1 < argc) {
                options.learningRate = std::stof(argv[++i]);
            }
        }
        else if (arg == "--curiosity") {
            if (i + 1 < argc) {
                options.curiosity = std::stof(argv[++i]);
            }
        }
        else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        else if (arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        }
        else if (arg == "--log") {
            if (i + 1 < argc) {
                options.logFile = argv[++i];
            }
        }
        else if (arg == "--experiment") {
            if (i + 1 < argc) {
                options.experimentType = argv[++i];
            }
        }
        else if (arg.substr(0, 5) == "--set=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(5, pos - 5);
                std::string value = arg.substr(pos + 1);
                options.customSettings[key] = value;
            }
        }
        else if (arg.find("--set=") != std::string::npos) {
            // Handle --set key value pattern
            if (i + 2 < argc && std::string(argv[i + 1]).find('=') == std::string::npos) {
                std::string key = argv[i + 1];
                std::string value = argv[i + 2];
                options.customSettings[key] = value;
                i += 2;
            }
        }
        else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
        }
        else {
            options.experimentArgs.push_back(arg);
        }
    }
    
    return options;
}

bool CommandLineParser::validateOptions(const CommandLineOptions& options) {
    if (options.maxSteps < 0) {
        std::cerr << "Error: maxSteps must be non-negative" << std::endl;
        return false;
    }
    
    if (options.timestep <= 0.0) {
        std::cerr << "Error: timestep must be positive" << std::endl;
        return false;
    }
    
    if (options.learningRate < 0.0f || options.learningRate > 1.0f) {
        std::cerr << "Error: learning rate must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (options.curiosity < 0.0f || options.curiosity > 10.0f) {
        std::cerr << "Error: curiosity level must be between 0.0 and 10.0" << std::endl;
        return false;
    }
    
    return true;
}

void CommandLineParser::printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --steps <n>             Set maximum number of simulation steps" << std::endl;
    std::cout << "  --timestep <t>          Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --seed <n>              Set random seed (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment options:" << std::endl;
    std::cout << "  --experiment <type>     Experiment type (basic, learning, plasticity, stdp)" << std::endl;
    std::cout << "  --no-learning           Disable learning subsystems" << std::endl;
    std::cout << "  --no-development        Disable developmental processes" << std::endl;
    std::cout << "  --no-neuromodulation    Disable neuromodulation" << std::endl;
    std::cout << "  --visualization         Enable visualization" << std::endl;
    std::cout << std::endl;
    std::cout << "Output options:" << std::endl;
    std::cout << "  --output <file>         Save output to file" << std::endl;
    std::cout << "  --log <file>            Log output to file" << std::endl;
    std::cout << "  --save-config           Save final configuration to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced options:" << std::endl;
    std::cout << "  --learning-rate <r>     Learning rate (0.0-1.0, default: 0.001)" << std::endl;
    std::cout << "  --curiosity <c>         Curiosity level (0.0-10.0, default: 1.0)" << std::endl;
    std::cout << "  --verbose, -v           Verbose output" << std::endl;
    std::cout << "  --debug, -d             Debug output with extra information" << std::endl;
    std::cout << "  --benchmark             Run performance benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "Custom settings:" << std::endl;
    std::cout << "  --set <key> <value>     Set custom configuration parameter" << std::endl;
    std::cout << "  --set=<key>=<value>     Set custom configuration parameter (short form)" << std::endl;
}

void CommandLineParser::printHelp(const char* programName) {
    printUsage(programName);
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --experiment learning --steps 5000" << std::endl;
    std::cout << "  " << programName << " --config myconfig.json --visualization" << std::endl;
    std::cout << "  " << programName << " --experiment plasticity --learning-rate 0.01 --curiosity 2.0" << std::endl;
}

bool CommandLineParser::saveOptionsToFile(const CommandLineOptions& options, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "# NLM Configuration Generated from Command Line" << std::endl;
    file << "# Generated at: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;
    
    file << "# Basic settings" << std::endl;
    file << "simulation.timestep = " << options.timestep << std::endl;
    file << "random.seed = " << options.randomSeed << std::endl;
    file << "experiment.max_steps = " << options.maxSteps << std::endl;
    file << std::endl;
    
    file << "# Learning settings" << std::endl;
    file << "plasticity.learning_rate = " << options.learningRate << std::endl;
    file << "neuromodulation.curiosity = " << options.curiosity << std::endl;
    file << std::endl;
    
    file << "# Enabled subsystems" << std::endl;
    file << "system.learning = " << (options.enableLearning ? "true" : "false") << std::endl;
    file << "system.development = " << (options.enableDevelopment ? "true" : "false") << std::endl;
    file << "system.neuromodulation = " << (options.enableNeuromodulation ? "true" : "false") << std::endl;
    file << "system.visualization = " << (options.enableVisualization ? "true" : "false") << std::endl;
    file << std::endl;
    
    if (!options.experimentType.empty() && options.experimentType != "basic") {
        file << "experiment.type = " << options.experimentType << std::endl;
    }
    
    if (!options.configFile.empty() && options.configFile != "configs/default.cfg") {
        file << "experiment.config_file = " << options.configFile << std::endl;
    }
    
    file << "# End of configuration" << std::endl;
    
    return true;
}

} // namespace nlm
