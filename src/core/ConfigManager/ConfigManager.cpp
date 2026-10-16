#include "ConfigManager.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>

namespace nlm {

ConfigManager::ConfigManager() {
    config = std::make_shared<Config>();
    
    // Initialize parser
    parser = std::make_unique<CommandLineParser>("nlm");
    
    // Add all options
    
    // Experiment Selection
    parser->addOption("phase", "p", 
        "Select Phase (1-6)", false, true, "1", "int", "--phase 3 or -p 3");
    parser->addOption("experiment-type", "",
        "Specify experiment type (e.g., 'learning', 'plasticity', 'stdp', 'connectivity')", 
        false, true, "", "string", "--experiment-type plasticity or -e plasticity");
    parser->addOption("demo-mode", "d",
        "Enable demo mode for quick testing", false, false, "", "bool", "--demo-mode or -d");
    
    // Simulation Parameters
    parser->addOption("steps", "s",
        "Set number of simulation steps", false, true, "1000", "int", "--steps 5000 or -s 5000");
    parser->addOption("timestep", "t",
        "Set simulation timestep (seconds)", false, true, "0.001", "double", "--timestep 0.01 or -t 0.01");
    parser->addOption("neuron-count", "n",
        "Set neuron count", false, true, "500", "int64", "--neuron-count 1000 or -n 1000");
    parser->addOption("region-count", "r",
        "Set region count", false, true, "1", "int64", "--region-count 2 or -r 2");
    
    // Output Control
    parser->addOption("verbose", "v",
        "Enable verbose logging", false, false, "", "bool", "--verbose or -v");
    parser->addOption("quiet", "q",
        "Suppress most output", false, false, "", "bool", "--quiet or -q");
    parser->addOption("output-file", "",
        "Write output to file", false, true, "", "string", "--output-file results.txt");
    parser->addOption("no-banner", "",
        "Suppress banner at startup", false, false, "", "bool", "--no-banner");
    
    // Checkpoint Management
    parser->addOption("load-checkpoint", "",
        "Load from checkpoint", false, false, "", "bool", "--load-checkpoint");
    parser->addOption("save-checkpoint", "",
        "Save checkpoint", false, false, "", "bool", "--save-checkpoint");
    parser->addOption("checkpoint-path", "",
        "Specify checkpoint path", false, true, "checkpoint", "string", "--checkpoint-path my_checkpoint");
    
    // Debug and Analysis
    parser->addOption("debug", "",
        "Enable debug mode", false, false, "", "bool", "--debug");
    parser->addOption("benchmark", "",
        "Run benchmark", false, false, "", "bool", "--benchmark");
    parser->addOption("analyze", "",
        "Trigger analysis mode", false, false, "", "bool", "--analyze");
    
    // Performance monitoring and profiling
    parser->addOption("perf-metrics", "",
        "Output performance metrics", false, false, "", "bool", "--perf-metrics");
    parser->addOption("profile", "",
        "Enable detailed profiling", false, false, "", "bool", "--profile");
    parser->addOption("compare-configs", "",
        "Compare different configurations", false, false, "", "bool", "--compare-configs");
    parser->addOption("profile-format", "",
        "Profile output format (json, human, csv)", false, true, "json", "string", "--profile-format human");
    parser->addOption("benchmark-output", "",
        "Benchmark output file", false, true, "", "string", "--benchmark-output benchmark.json");
    parser->addOption("metrics-output", "",
        "Metrics output file", false, true, "", "string", "--metrics-output metrics.json");
    parser->addOption("profile-output", "",
        "Profile output file", false, true, "", "string", "--profile-output profile.json");
    parser->addOption("comparison-output", "",
        "Configuration comparison output file", false, true, "", "string", "--comparison-output comparison.json");
    parser->addOption("verbose-performance", "",
        "Enable verbose performance output", false, false, "", "bool", "--verbose-performance");
    
    // Configuration
    parser->addOption("set", "",
        "Set individual config option (key=value)", false, true, "", "string", "--set random_seed=42");
    
    // Legacy --config option
    parser->addOption("config", "",
        "Load configuration from file", false, true, "configs/default.cfg", "string", "--config myconfig.cfg");
}

bool ConfigManager::initialize(int argc, char** argv) {
    // Parse command line arguments
    if (!parser->parse(argc, argv)) {
        return false;
    }
    
    // Process and apply all options
    return processOptions();
}

void ConfigManager::printHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: Phase 2: Real Neural Computation" << std::endl << std::endl;
    
    std::cout << "Usage: nlm [OPTIONS]... [ARGS]" << std::endl << std::endl;
    
    std::cout << "EXPERIMENT SELECTION:" << std::endl;
    std::cout << "  -p, --phase PHASE              Select Phase (1-6)" << std::endl;
    std::cout << "  -e, --experiment-type TYPE    Specify experiment type" << std::endl;
    std::cout << "  -d, --demo-mode               Enable demo mode" << std::endl;
    std::cout << "  -b, --benchmark               Run benchmark" << std::endl;
    std::cout << "  -a, --analyze                 Trigger analysis mode" << std::endl << std::endl;
    
    std::cout << "SIMULATION PARAMETERS:" << std::endl;
    std::cout << "  -s, --steps STEPS            Set number of simulation steps" << std::endl;
    std::cout << "  -t, --timestep TIMESTEP      Set simulation timestep" << std::endl;
    std::cout << "  -n, --neuron-count COUNT     Set neuron count" << std::endl;
    std::cout << "  -r, --region-count COUNT     Set region count" << std::endl << std::endl;
    
    std::cout << "OUTPUT CONTROL:" << std::endl;
    std::cout << "  -v, --verbose                Enable verbose logging" << std::endl;
    std::cout << "  -q, --quiet                  Suppress most output" << std::endl;
    std::cout << "      --output-file FILE       Write output to file" << std::endl;
    std::cout << "      --no-banner             Suppress banner" << std::endl << std::endl;
    
    std::cout << "CHECKPOINT MANAGEMENT:" << std::endl;
    std::cout << "      --load-checkpoint        Load from checkpoint" << std::endl;
    std::cout << "      --save-checkpoint        Save checkpoint" << std::endl;
    std::cout << "      --checkpoint-path PATH   Specify checkpoint path" << std::endl << std::endl;
    
    std::cout << "DEBUG AND ANALYSIS:" << std::endl;
    std::cout << "      --debug                  Enable debug mode" << std::endl;
    std::cout << "      --benchmark             Run benchmark" << std::endl;
    std::cout << "      --analyze               Trigger analysis mode" << std::endl << std::endl;
    
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "      --config FILE            Load configuration from file" << std::endl;
    std::cout << "      --set KEY=VALUE          Set individual config option" << std::endl << std::endl;
    
    std::cout << "PERFORMANCE MONITORING:" << std::endl;
    std::cout << "      --benchmark             Run performance benchmarking" << std::endl;
    std::cout << "      --perf-metrics          Output performance metrics" << std::endl;
    std::cout << "      --profile               Enable detailed profiling" << std::endl;
    std::cout << "      --compare-configs       Compare different configurations" << std::endl;
    std::cout << "      --profile-format FORMAT  Profile output format (json, human, csv)" << std::endl;
    std::cout << "      --benchmark-output FILE  Benchmark output file" << std::endl;
    std::cout << "      --metrics-output FILE    Metrics output file" << std::endl;
    std::cout << "      --profile-output FILE    Profile output file" << std::endl;
    std::cout << "      --comparison-output FILE Configuration comparison output file" << std::endl;
    std::cout << "      --verbose-performance     Enable verbose performance output" << std::endl << std::endl;
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --phase 3 --steps 5000 --neuron-count 1000" << std::endl;
    std::cout << "  nlm -d --demo-mode" << std::endl;
    std::cout << "  nlm --benchmark --output-file benchmark_results.txt" << std::endl;
    std::cout << "  nlm --set random_seed=42 --set simulation_timestep=0.01" << std::endl;
}

void ConfigManager::printVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build: 2026-10-16" << std::endl;
}

bool ConfigManager::processOptions() {
    // Process options in order of dependencies
    processExperimentOptions();
    processSimulationOptions();
    processOutputOptions();
    processCheckpointOptions();
    processDebugOptions();
    processPerformanceOptions();
    processConfigOverrides();
    
    // Validate all options
    if (!validateOptions()) {
        return false;
    }
    
    return true;
}

void ConfigManager::processExperimentOptions() {
    experimentOptions.phase = parser->getOptionInt("phase", 1);
    experimentOptions.experimentType = parser->getOptionString("experiment-type", "");
    experimentOptions.demoMode = parser->getOptionBool("demo-mode", false);
    experimentOptions.benchmarkMode = parser->getOptionBool("benchmark", false);
    experimentOptions.analyzeMode = parser->getOptionBool("analyze", false);
    
    // Set experiment type based on phase if not explicitly set
    if (experimentOptions.experimentType.empty()) {
        switch (experimentOptions.phase) {
            case 1: experimentOptions.experimentType = "learning"; break;
            case 2: experimentOptions.experimentType = "connectivity"; break;
            case 3: experimentOptions.experimentType = "plasticity"; break;
            case 4: experimentOptions.experimentType = "stdp"; break;
            case 5: experimentOptions.experimentType = "structural"; break;
            case 6: experimentOptions.experimentType = "integrated"; break;
            default: experimentOptions.experimentType = "demo"; break;
        }
    }
    
    // Set default config values based on experiment
    config->set("experiment_phase", static_cast<int64_t>(experimentOptions.phase), ConfigSource::CommandLine);
    config->set("experiment_type", experimentOptions.experimentType, ConfigSource::CommandLine);
    config->set("demo_mode", experimentOptions.demoMode, ConfigSource::CommandLine);
    config->set("benchmark_mode", experimentOptions.benchmarkMode, ConfigSource::CommandLine);
    config->set("analyze_mode", experimentOptions.analyzeMode, ConfigSource::CommandLine);
}

void ConfigManager::processSimulationOptions() {
    simulationOptions.steps = parser->getOptionInt("steps", 1000);
    simulationOptions.timestep = parser->getOptionDouble("timestep", 0.001);
    simulationOptions.neuronCount = parser->getOptionInt("neuron-count", 500);
    simulationOptions.regionCount = parser->getOptionInt("region-count", 1);
    
    // Apply to config
    config->set("simulation_steps", static_cast<int64_t>(simulationOptions.steps), ConfigSource::CommandLine);
    config->set("simulation_timestep", simulationOptions.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(simulationOptions.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(simulationOptions.regionCount), ConfigSource::CommandLine);
}

void ConfigManager::processOutputOptions() {
    outputOptions.verbose = parser->getOptionBool("verbose", false);
    outputOptions.quiet = parser->getOptionBool("quiet", false);
    outputOptions.outputFile = parser->getOptionString("output-file", "");
    outputOptions.noBanner = parser->getOptionBool("no-banner", false);
    
    // Set log level based on options
    if (outputOptions.quiet) {
        config->set("log_level", "error", ConfigSource::CommandLine);
    } else if (outputOptions.verbose) {
        config->set("log_level", "debug", ConfigSource::CommandLine);
    }
    
    // Set output file
    if (!outputOptions.outputFile.empty()) {
        config->set("output_file", outputOptions.outputFile, ConfigSource::CommandLine);
    }
}

void ConfigManager::processCheckpointOptions() {
    checkpointOptions.loadCheckpoint = parser->getOptionBool("load-checkpoint", false);
    checkpointOptions.saveCheckpoint = parser->getOptionBool("save-checkpoint", false);
    checkpointOptions.checkpointPath = parser->getOptionString("checkpoint-path", "checkpoint");
    
    // Apply to config
    config->set("load_checkpoint", checkpointOptions.loadCheckpoint, ConfigSource::CommandLine);
    config->set("save_checkpoint", checkpointOptions.saveCheckpoint, ConfigSource::CommandLine);
    config->set("checkpoint_path", checkpointOptions.checkpointPath, ConfigSource::CommandLine);
}

void ConfigManager::processDebugOptions() {
    debugOptions.debug = parser->getOptionBool("debug", false);
    
    // Set debug mode
    config->set("debug_mode", debugOptions.debug, ConfigSource::CommandLine);
}

void ConfigManager::processPerformanceOptions() {
    performanceOptions.benchmark = parser->getOptionBool("benchmark", false);
    performanceOptions.perfMetrics = parser->getOptionBool("perf-metrics", false);
    performanceOptions.profile = parser->getOptionBool("profile", false);
    performanceOptions.compareConfigs = parser->getOptionBool("compare-configs", false);
    performanceOptions.profileOutputFormat = parser->getOptionString("profile-format", "json");
    performanceOptions.benchmarkOutputFile = parser->getOptionString("benchmark-output", "");
    performanceOptions.metricsOutputFile = parser->getOptionString("metrics-output", "");
    performanceOptions.profileOutputFile = parser->getOptionString("profile-output", "");
    performanceOptions.comparisonOutputFile = parser->getOptionString("comparison-output", "");
    performanceOptions.verbosePerformance = parser->getOptionBool("verbose-performance", false);
    
    // Apply performance options to config
    config->set("benchmark_mode", performanceOptions.benchmark, ConfigSource::CommandLine);
    config->set("perf_metrics", performanceOptions.perfMetrics, ConfigSource::CommandLine);
    config->set("profile", performanceOptions.profile, ConfigSource::CommandLine);
    config->set("compare_configs", performanceOptions.compareConfigs, ConfigSource::CommandLine);
    config->set("profile_output_format", performanceOptions.profileOutputFormat, ConfigSource::CommandLine);
    config->set("benchmark_output_file", performanceOptions.benchmarkOutputFile, ConfigSource::CommandLine);
    config->set("metrics_output_file", performanceOptions.metricsOutputFile, ConfigSource::CommandLine);
    config->set("profile_output_file", performanceOptions.profileOutputFile, ConfigSource::CommandLine);
    config->set("comparison_output_file", performanceOptions.comparisonOutputFile, ConfigSource::CommandLine);
    config->set("verbose_performance", performanceOptions.verbosePerformance, ConfigSource::CommandLine);
}

void ConfigManager::processConfigOverrides() {
    std::vector<std::string> remainingArgs = parser->getRemainingArgs();
    
    // Check for --config option (legacy)
    std::string configFile = parser->getOptionString("config", "configs/default.cfg");
    if (!configFile.empty() && configFile != "configs/default.cfg") {
        // Load config file (this would override file-based config)
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
    }
    
    // Process --set options
    for (const auto& arg : remainingArgs) {
        if (arg.find('=') != std::string::npos) {
            std::string key = trim(arg.substr(0, arg.find('=')));
            std::string value = trim(arg.substr(arg.find('=') + 1));
            
            // Try to parse as different types
            bool success = false;
            
            // Try bool
            if (!success && (value == "true" || value == "false")) {
                bool boolVal = (value == "true");
                config->set(key, boolVal, ConfigSource::CommandLine);
                success = true;
            }
            
            // Try int
            if (!success) {
                try {
                    size_t pos;
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        config->set(key, intVal, ConfigSource::CommandLine);
                        success = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!success) {
                try {
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        config->set(key, doubleVal, ConfigSource::CommandLine);
                        success = true;
                    }
                } catch (...) {}
            }
            
            // If not a simple type, treat as string
            if (!success) {
                config->set(key, value, ConfigSource::CommandLine);
            }
            
            NLM_LOG_INFO("Set config: " + key + " = " + value);
        }
    }
}

std::string ConfigManager::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigManager::parseBool(const std::string& value) const {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

int ConfigManager::parseInt(const std::string& value, int defaultValue) const {
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

double ConfigManager::parseDouble(const std::string& value, double defaultValue) const {
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}

bool ConfigManager::validateOptions() const {
    // Validate phase
    if (experimentOptions.phase < MIN_PHASE || experimentOptions.phase > MAX_PHASE) {
        std::cerr << "Error: Phase must be between " << MIN_PHASE << " and " << MAX_PHASE << "\n";
        return false;
    }
    
    // Validate steps
    if (simulationOptions.steps < MIN_STEPS || simulationOptions.steps > MAX_STEPS) {
        std::cerr << "Error: Steps must be between " << MIN_STEPS << " and " << MAX_STEPS << "\n";
        return false;
    }
    
    // Validate timestep
    if (simulationOptions.timestep < MIN_TIMESTEP || simulationOptions.timestep > MAX_TIMESTEP) {
        std::cerr << "Error: Timestep must be between " << MIN_TIMESTEP << " and " << MAX_TIMESTEP << "\n";
        return false;
    }
    
    // Validate neuron count
    if (simulationOptions.neuronCount < MIN_NEURON_COUNT || simulationOptions.neuronCount > MAX_NEURON_COUNT) {
        std::cerr << "Error: Neuron count must be between " << MIN_NEURON_COUNT << " and " << MAX_NEURON_COUNT << "\n";
        return false;
    }
    
    // Validate region count
    if (simulationOptions.regionCount < MIN_REGION_COUNT || simulationOptions.regionCount > MAX_REGION_COUNT) {
        std::cerr << "Error: Region count must be between " << MIN_REGION_COUNT << " and " << MAX_REGION_COUNT << "\n";
        return false;
    }
    
    // Check for incompatible options
    if (outputOptions.quiet && outputOptions.verbose) {
        std::cerr << "Error: Cannot specify both --quiet and --verbose\n";
        return false;
    }
    
    if (checkpointOptions.loadCheckpoint && checkpointOptions.saveCheckpoint) {
        std::cerr << "Error: Cannot specify both --load-checkpoint and --save-checkpoint\n";
        return false;
    }
    
    if (checkpointOptions.loadCheckpoint && !checkpointOptions.checkpointPath.empty()) {
        std::cerr << "Warning: --checkpoint-path ignored with --load-checkpoint\n";
    }
    
    return true;
}

} // namespace nlm
