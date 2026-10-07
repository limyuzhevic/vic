// CommandLineOptions.cpp
// Implementation of CLI argument parsing and validation for NLM

#include "core/Utilities/CommandLineOptions.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nlm {

CommandLineOptions CommandLineOptions::parse(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            options.helpRequested = true;
        } else if (arg == "--version") {
            options.versionRequested = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--debug") {
            options.debug = true;
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 13) == "--test-mode=") {
            options.testMode = arg.substr(13);
        } else if (arg == "--test-mode" && i + 1 < argc) {
            options.testMode = argv[++i];
        } else if (arg.substr(0, 10) == "--test-mode=") {
            options.testMode = arg.substr(10);
        } else if (arg.substr(0, 12) == "--neuron-count") {
            if (arg.find('=') != std::string::npos) {
                options.neuronCount = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                options.maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 10) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                options.simulationTimestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.simulationTimestep = std::stod(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                options.randomSeed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg == "--connectivity") {
            options.runConnectivityTest = true;
        } else if (arg == "--plasticity") {
            options.runPlasticityTest = true;
        } else if (arg == "--stdp") {
            options.runStdpTest = true;
        } else if (arg == "--learning") {
            options.runLearningTest = true;
        } else if (arg == "--hebbian") {
            options.enableHebbian = true;
        } else if (arg == "--no-hebbian") {
            options.enableHebbian = false;
        } else if (arg == "--stdp") {
            options.enableStdp = true;
        } else if (arg == "--no-stdp") {
            options.enableStdp = false;
        } else if (arg == "--structural") {
            options.enableStructuralPlasticity = true;
        } else if (arg == "--no-structural") {
            options.enableStructuralPlasticity = false;
        } else if (arg == "--development") {
            options.enableDevelopment = true;
        } else if (arg == "--json") {
            options.jsonOutput = true;
        } else if (arg == "--csv") {
            options.csvOutput = true;
        } else if (arg == "--validate-config") {
            options.validateConfig = true;
        } else if (arg == "--list-configs") {
            options.listConfigs = true;
        } else if (arg.substr(0, 9) == "--output=") {
            options.outputFile = arg.substr(9);
        } else if (arg.substr(0, 16) == "--benchmark-output=") {
            options.benchmarkFile = arg.substr(16);
        }
    }
    
    // Parse test mode
    options.parseTestMode(options.testMode);
    
    // Parse configuration file
    if (options.configFile != "configs/default.cfg") {
        options.parseConfigFile(options.configFile);
    }
    
    // Parse environment variables
    options.parseEnvironmentVariables();
    
    return options;
}

bool CommandLineOptions::validate() const {
    // Check test mode validity
    std::vector<std::string> validModes = {"all", "connectivity", "plasticity", "stdp", "learning"};
    if (std::find(validModes.begin(), validModes.end(), testMode) == validModes.end()) {
        std::cerr << "Error: Invalid test mode '" << testMode << "'" << std::endl;
        std::cerr << "Valid modes: all, connectivity, plasticity, stdp, learning" << std::endl;
        return false;
    }
    
    // Check parameter ranges
    if (neuronCount == 0) {
        std::cerr << "Error: Neuron count must be greater than 0" << std::endl;
        return false;
    }
    
    if (maxSteps == 0) {
        std::cerr << "Error: Max steps must be greater than 0" << std::endl;
        return false;
    }
    
    if (simulationTimestep <= 0.0) {
        std::cerr << "Error: Simulation timestep must be positive" << std::endl;
        return false;
    }
    
    // Check for conflicting options
    if (quiet && verbose) {
        std::cerr << "Error: Cannot specify both --quiet and --verbose" << std::endl;
        return false;
    }
    
    if (jsonOutput && csvOutput) {
        std::cerr << "Error: Cannot specify both --json and --csv output formats" << std::endl;
        return false;
    }
    
    return true;
}

void CommandLineOptions::printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --quiet                   Suppress non-essential output" << std::endl;
    std::cout << "  --debug                   Enable debug output" << std::endl;
    std::cout << "  --benchmark               Run performance benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  --config=<file>           Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Execution:" << std::endl;
    std::cout << "  --test-mode=<mode>        Test mode: all, connectivity, plasticity, stdp, learning" << std::endl;
    std::cout << "  --connectivity            Run connectivity test" << std::endl;
    std::cout << "  --plasticity              Run plasticity test" << std::endl;
    std::cout << "  --stdp                    Run STDP test" << std::endl;
    std::cout << "  --learning                Run learning test" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Parameters:" << std::endl;
    std::cout << "  --neuron-count=<n>        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<n>               Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --timestep=<t>            Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --seed=<n>                Random seed (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Plasticity:" << std::endl;
    std::cout << "  --hebbian                 Enable Hebbian plasticity (default)" << std::endl;
    std::cout << "  --no-hebbian              Disable Hebbian plasticity" << std::endl;
    std::cout << "  --stdp                    Enable STDP (default)" << std::endl;
    std::cout << "  --no-stdp                 Disable STDP" << std::endl;
    std::cout << "  --structural              Enable structural plasticity (default)" << std::endl;
    std::cout << "  --no-structural           Disable structural plasticity" << std::endl;
    std::cout << std::endl;
    std::cout << "Development:" << std::endl;
    std::cout << "  --development             Enable development stages" << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  --output=<file>           Output file for results" << std::endl;
    std::cout << "  --json                    Output results in JSON format" << std::endl;
    std::cout << "  --csv                     Output results in CSV format" << std::endl;
    std::cout << "  --benchmark-output=<file> Benchmark output file" << std::endl;
    std::cout << std::endl;
    std::cout << "Validation:" << std::endl;
    std::cout << "  --validate-config         Validate configuration" << std::endl;
    std::cout << "  --list-configs            List available configurations" << std::endl;
}

void CommandLineOptions::printVersion() {
    std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0 (Improved Edition)" << std::endl;
    std::cout << "Enhanced with better CLI, configuration management, and error handling." << std::endl;
}

std::string CommandLineOptions::getTestModeName() const {
    return testMode;
}

bool CommandLineOptions::shouldRunTest(const std::string& testName) const {
    if (testMode == "all") {
        return true;
    }
    
    if (testMode == "connectivity" && testName == "connectivity") return true;
    if (testMode == "plasticity" && testName == "plasticity") return true;
    if (testMode == "stdp" && testName == "stdp") return true;
    if (testMode == "learning" && testName == "learning") return true;
    
    return false;
}

std::vector<std::string> CommandLineOptions::getTestsToRun() const {
    std::vector<std::string> tests;
    
    if (shouldRunTest("connectivity")) tests.push_back("connectivity");
    if (shouldRunTest("plasticity")) tests.push_back("plasticity");
    if (shouldRunTest("stdp")) tests.push_back("stdp");
    if (shouldRunTest("learning")) tests.push_back("learning");
    
    return tests;
}

void CommandLineOptions::parseTestMode(const std::string& mode) {
    std::string lowerMode = toLower(mode);
    
    if (lowerMode == "connectivity" || lowerMode == "connect") {
        testMode = "connectivity";
        runConnectivityTest = true;
    } else if (lowerMode == "plasticity" || lowerMode == "plastic") {
        testMode = "plasticity";
        runPlasticityTest = true;
    } else if (lowerMode == "stdp" || lowerMode == "std") {
        testMode = "stdp";
        runStdpTest = true;
    } else if (lowerMode == "learning" || lowerMode == "learn") {
        testMode = "learning";
        runLearningTest = true;
    } else {
        // Default to all tests
        testMode = "all";
        runConnectivityTest = true;
        runPlasticityTest = true;
        runStdpTest = true;
        runLearningTest = true;
    }
}

void CommandLineOptions::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid config line " << lineNum << ": " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse based on key
        try {
            if (key == "neuron_count" || key == "neurons") {
                neuronCount = std::stoull(value);
            } else if (key == "max_steps" || key == "steps") {
                maxSteps = std::stoull(value);
            } else if (key == "timestep" || key == "dt") {
                simulationTimestep = std::stod(value);
            } else if (key == "random_seed" || key == "seed") {
                randomSeed = std::stoull(value);
            } else if (key == "connection_probability" || key == "connectivity") {
                connectionProbability = std::stof(value);
            } else if (key == "stdp_ltp_weight") {
                stdpLtpWeight = std::stof(value);
            } else if (key == "stdp_ltd_weight") {
                stdpLtdWeight = std::stof(value);
            } else if (key == "stdp_tau") {
                stdpTau = std::stof(value);
            } else if (key == "synaptogenesis_rate") {
                synaptogenesisRate = std::stof(value);
            } else if (key == "pruning_rate") {
                pruningRate = std::stof(value);
            } else if (key == "enable_hebbian") {
                enableHebbian = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_stdp") {
                enableStdp = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_structural") {
                enableStructuralPlasticity = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_development") {
                enableDevelopment = (value == "true" || value == "1" || value == "yes");
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error parsing config line " << lineNum << ": " << e.what() << std::endl;
        }
    }
    
    file.close();
    std::cout << "Config file parsed: " << filename << std::endl;
}

void CommandLineOptions::parseEnvironmentVariables() {
    const char* envVar = std::getenv("NLM_TEST_MODE");
    if (envVar) {
        testMode = envVar;
        parseTestMode(testMode);
    }
    
    envVar = std::getenv("NLM_NEURON_COUNT");
    if (envVar) {
        neuronCount = std::stoull(envVar);
    }
    
    envVar = std::getenv("NLM_STEPS");
    if (envVar) {
        maxSteps = std::stoull(envVar);
    }
    
    envVar = std::getenv("NLM_SEED");
    if (envVar) {
        randomSeed = std::stoull(envVar);
    }
}

std::string CommandLineOptions::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool CommandLineOptions::startsWith(const std::string& str, const std::string& prefix) const {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

} // namespace nlm