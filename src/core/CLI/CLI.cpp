#include "CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

namespace {
    const std::string VERSION = "NLM Phase 6.1 (Integrated Artificial Brain)";
    const std::string HELP_TEXT = R"(
NLM - Neural Learning Machine Command Line Interface

USAGE:
    nlm [OPTIONS] [EXPERIMENT_MODE] [EXPERIMENT_ARGS]

EXPERIMENT_MODES:
    --phase 3        Run Phase 3: World Interaction Demo
    --phase 4        Run Phase 4: Emerging Cognition (8 experiments)
    --phase 5        Run Phase 5: Personality/Brain-Sandbox Integration
    --phase 6        Run Phase 6: Integration Test (default)

OPTIONS:
    --config <file>          Load configuration from file
    --preset <name>          Use preset configuration (quick-test, full-experiment, research)
    --verbose, -v            Enable verbose output
    --benchmark              Run performance benchmark
    --benchmark-runs <n>      Number of benchmark runs (default: 3)
    --benchmark-timeout <s>   Benchmark timeout in seconds (default: 300)
    --save-results <file>     Save experiment results to file
    --progress, -p           Show progress reporting
    --log-level <level>      Set logging level (DEBUG, INFO, WARNING, ERROR)
    --help, -h               Show this help message
    --version, --ver         Show version information

CONFIGURATION PRESETS:
    quick-test              Small configuration for fast testing (50 neurons, 100 steps)
    full-experiment         Full experimental setup (1000 neurons, 5000 steps)
    research                Research-focused configuration (500 neurons, 2000 steps, high plasticity)

EXAMPLES:
    nlm --phase 6                    Run Phase 6 integration test
    nlm --phase 4 --preset quick-test  Run Phase 4 with quick test preset
    nlm --phase 3 -v --progress       Run Phase 3 with verbose output and progress
    nlm --phase 6 --benchmark         Run performance benchmark
    nlm --phase 6 --save-results results.txt  Save results to file

For more detailed information about specific experiments, use the experiment's
own help flags (--help within individual experiments).
    );
}

CLIParser::ParsedArgs CLIParser::parseArgs(int argc, char** argv) {
    ParsedArgs args;
    std::vector<std::string> positionalArgs;

    // Set defaults
    args.verbose = false;
    args.benchmark = false;
    args.benchmarkRuns = 3;
    args.benchmarkTimeout = 300.0;
    args.saveResults = false;
    args.progress = false;
    args.showHelp = false;
    args.showVersion = false;
    args.logLevel = "INFO";
    args.experimentMode = "6";  // Default to Phase 6

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--version" || arg == "--ver") {
            args.showVersion = true;
        } else if (arg.substr(0, 7) == "--phase ") {
            args.experimentMode = arg.substr(7);
        } else if (arg.substr(0, 8) == "--preset ") {
            args.configPreset = arg.substr(8);
        } else if (arg.substr(0, 9) == "--config ") {
            args.configFile = arg.substr(9);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else if (arg.substr(0, 17) == "--benchmark-runs ") {
            args.benchmarkRuns = std::stoi(arg.substr(17));
        } else if (arg.substr(0, 19) == "--benchmark-timeout ") {
            args.benchmarkTimeout = std::stod(arg.substr(19));
        } else if (arg == "--progress" || arg == "-p") {
            args.progress = true;
        } else if (arg.substr(0, 14) == "--log-level ") {
            args.logLevel = arg.substr(14);
            std::transform(args.logLevel.begin(), args.logLevel.end(), args.logLevel.begin(), ::toupper);
        } else if (arg.substr(0, 16) == "--save-results ") {
            args.saveResults = true;
            args.resultsFile = arg.substr(16);
        } else {
            // Check if it's a flag argument (starts with --)
            if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
                // Handle --key=value format
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.extraArgs.push_back("--" + key + "=" + value);
            } else if (arg.substr(0, 1) == "-" && arg.size() > 1) {
                // Handle -key format
                std::string key = arg.substr(1);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.extraArgs.push_back("-" + key + " " + std::string(argv[++i]));
                } else {
                    args.extraArgs.push_back("-" + key);
                }
            } else {
                // Positional argument
                positionalArgs.push_back(arg);
            }
        }
    }

    // Override experiment mode with positional args if provided
    if (!positionalArgs.empty()) {
        args.experimentMode = positionalArgs[0];
    }

    return args;
}

void CLIParser::printHelp() {
    std::cout << HELP_TEXT;
}

void CLIParser::printVersion() {
    std::cout << "NLM " << VERSION << std::endl;
}

std::string CLIParser::getConfigPreset(const std::string& preset) {
    // Convert to lowercase for case-insensitive matching
    std::string presetLower = preset;
    std::transform(presetLower.begin(), presetLower.end(), presetLower.begin(), ::tolower);

    if (presetLower == "quick-test" || presetLower == "quicktest") {
        return "quick-test";
    } else if (presetLower == "full-experiment" || presetLower == "full") {
        return "full-experiment";
    } else if (presetLower == "research" || presetLower == "research-mode") {
        return "research";
    }

    return "";
}

void ConfigPresets::applyPreset(const std::string& preset, std::shared_ptr<Config> config) {
    std::string presetName = CLIParser::getConfigPreset(preset);
    if (presetName.empty()) {
        return;
    }

    // Clear existing config
    config->clear();

    if (presetName == "quick-test") {
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("max_steps", static_cast<int64_t>(100));
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.001f);
        config->set("pruning_rate", 0.0001f);
    } else if (presetName == "full-experiment") {
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("max_steps", static_cast<int64_t>(5000));
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    } else if (presetName == "research") {
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.02f);  // Higher plasticity
        config->set("stdp_ltd_weight", 0.018f);
        config->set("max_steps", static_cast<int64_t>(2000));
        config->set("stdp_tau", 30.0f);  // Longer STDP window
        config->set("synaptogenesis_rate", 0.0005f);  // Higher structural plasticity
        config->set("pruning_rate", 0.00005f);
    }
}

bool ConfigPresets::hasPreset(const std::string& preset) {
    std::string presetName = CLIParser::getConfigPreset(preset);
    return !presetName.empty();
}

bool ResultSerializer::saveResults(const std::string& filepath, const std::string& experimentName, const std::string& results) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        // Write header
        file << "NLM Experiment Results" << std::endl;
        file << "=======================" << std::endl;
        file << "Experiment: " << experimentName << std::endl;
        file << "Timestamp: " << std::time(nullptr) << std::endl;
        file << std::endl;

        // Write results
        file << results;

        // Write footer
        file << std::endl;
        file << "=== End of Results ===" << std::endl;

        return true;
    } catch (const std::exception& e) {
        // Log error
        NLM_LOG_ERROR("Failed to save results to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool ResultSerializer::loadResults(const std::string& filepath, std::string& experimentName, std::string& results) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        bool inResults = false;

        while (std::getline(file, line)) {
            if (line.find("Experiment: ") == 0) {
                experimentName = line.substr(13);
            } else if (line == "========================" && experimentName.empty()) {
                inResults = true;
            } else if (line == "=== End of Results ===") {
                break;
            } else if (inResults) {
                results += line + "\n";
            }
        }

        return true;
    } catch (const std::exception& e) {
        // Log error
        NLM_LOG_ERROR("Failed to load results from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm