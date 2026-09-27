// Advanced CLI implementation

#include "debug/AdvancedCLI.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>

namespace nlm {

AdvancedCLI::AdvancedCLI() {
    initializeOptionParsers();
}

void AdvancedCLI::initializeOptionParsers() {
    // Initialize parsers for all option types
    // This allows for flexible parsing and validation
}

bool AdvancedCLI::parse(int argc, char** argv) {
    // Parse all arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) != "--" && arg.substr(0, 1) != "-") {
            // Positional argument, skip for now
            continue;
        }
        
        bool parsed = false;
        
        // Try parsing simulation options
        if (parseSimulationOptions(arg)) {
            parsed = true;
        }
        // Try parsing memory options
        else if (parseMemoryOptions(arg)) {
            parsed = true;
        }
        // Try parsing neuromodulation options
        else if (parseNeuromodulationOptions(arg)) {
            parsed = true;
        }
        // Try parsing debug options
        else if (parseDebugOptions(arg)) {
            parsed = true;
        }
        // Try parsing logging options
        else if (parseLoggingOptions(arg)) {
            parsed = true;
        }
        // Try parsing export options
        else if (parseExportOptions(arg)) {
            parsed = true;
        }
        // Try parsing performance options
        else if (parsePerformanceOptions(arg)) {
            parsed = true;
        }
        // Try parsing system options
        else if (parseSystemOptions(arg)) {
            parsed = true;
        }
        // Try parsing interactive options
        else if (parseInteractiveOptions(arg)) {
            parsed = true;
        }
        // Try parsing config options
        else if (parseConfigOptions(arg)) {
            parsed = true;
        }
        // Try parsing environment options
        else if (parseEnvironmentOptions(arg)) {
            parsed = true;
        }
        else {
            // Legacy argument format
            options.legacyArgs.push_back(arg);
        }
        
        if (!parsed && (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-")) {
            std::cerr << "Warning: Unknown option: " << arg << std::endl;
        }
    }
    
    // Parse legacy arguments (for backward compatibility)
    parseLegacyArguments(argc, argv);
    
    return true;
}

bool AdvancedCLI::parseSimulationOptions(const std::string& arg) {
    if (arg.find("--max-steps=") == 0) {
        try {
            options.simulation.maxSteps = std::stoi(arg.substr(11));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--timestep=") == 0) {
        try {
            options.simulation.timestep = std::stod(arg.substr(10));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--neuron-count=") == 0) {
        try {
            options.simulation.neuronCount = std::stoll(arg.substr(15));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--region-count=") == 0) {
        try {
            options.simulation.regionCount = std::stoll(arg.substr(15));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--temperature=") == 0) {
        try {
            options.simulation.temperature = std::stod(arg.substr(14));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--learning-rate=") == 0) {
        try {
            options.simulation.learningRate = std::stod(arg.substr(16));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--plasticity-enabled=") == 0) {
        options.simulation.plasticityEnabled = parseBoolValue(arg.substr(23));
        return true;
    }
    else if (arg.find("--neuromodulation-enabled=") == 0) {
        options.simulation.neuromodulationEnabled = parseBoolValue(arg.substr(26));
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseMemoryOptions(const std::string& arg) {
    if (arg.find("--working-memory-size=") == 0) {
        try {
            options.memory.workingMemorySize = std::stoll(arg.substr(23));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--episodic-memory-size=") == 0) {
        try {
            options.memory.episodicMemorySize = std::stoll(arg.substr(25));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--memory-replay=") == 0) {
        options.memory.memoryReplay = parseBoolValue(arg.substr(18));
        return true;
    }
    else if (arg.find("--replay-frequency=") == 0) {
        try {
            options.memory.replayFrequency = std::stoi(arg.substr(20));
            return true;
        } catch (...) { return false; }
    }
    
    return false;
}

bool AdvancedCLI::parseNeuromodulationOptions(const std::string& arg) {
    if (arg.find("--dopamine-level=") == 0) {
        try {
            options.neuromodulation.dopamineLevel = std::stod(arg.substr(18));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--curiosity-level=") == 0) {
        try {
            options.neuromodulation.curiosityLevel = std::stod(arg.substr(19));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--novelty-threshold=") == 0) {
        try {
            options.neuromodulation.noveltyThreshold = std::stod(arg.substr(21));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--reward-scale=") == 0) {
        try {
            options.neuromodulation.rewardScale = std::stod(arg.substr(16));
            return true;
        } catch (...) { return false; }
    }
    
    return false;
}

bool AdvancedCLI::parseDebugOptions(const std::string& arg) {
    if (arg.find("--debug-level=") == 0) {
        try {
            options.debug.debugLevel = std::stoi(arg.substr(14));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--trace=") == 0) {
        std::string traceValue = arg.substr(8);
        if (traceValue.empty()) {
            options.debug.traceTargets = {"neuron", "synapse", "memory", "neuromodulator"};
        } else {
            options.debug.traceTargets.push_back(traceValue);
        }
        return true;
    }
    else if (arg.find("--profile=") == 0) {
        std::string profileValue = arg.substr(9);
        if (profileValue.empty()) {
            options.debug.profileTargets = {"simulation", "memory", "neuromodulation"};
        } else {
            options.debug.profileTargets.push_back(profileValue);
        }
        return true;
    }
    else if (arg.find("--checkpoint=") == 0) {
        options.debug.checkpointFile = arg.substr(12);
        return true;
    }
    else if (arg.find("--visualize=") == 0) {
        std::string vizValue = arg.substr(12);
        if (vizValue.empty()) {
            options.debug.visualizationTargets = {"network", "spikes", "memory"};
        } else {
            options.debug.visualizationTargets.push_back(vizValue);
        }
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseLoggingOptions(const std::string& arg) {
    if (arg.find("--log-level=") == 0) {
        options.logging.logLevel = arg.substr(12);
        return true;
    }
    else if (arg.find("--log-file=") == 0) {
        options.logging.logFile = arg.substr(11);
        return true;
    }
    else if (arg.find("--log-format=") == 0) {
        options.logging.logFormat = arg.substr(13);
        return true;
    }
    else if (arg == "--event-log") {
        options.logging.eventLog = true;
        return true;
    }
    else if (arg == "--trace-sim") {
        options.logging.traceSim = true;
        return true;
    }
    else if (arg == "--structured-log") {
        options.logging.structuredLog = true;
        return true;
    }
    else if (arg == "--log-metrics") {
        options.logging.logMetrics = true;
        return true;
    }
    else if (arg == "--log-neural-activity") {
        options.logging.logNeuralActivity = true;
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseExportOptions(const std::string& arg) {
    if (arg.find("--export-network=") == 0) {
        std::string format = arg.substr(17);
        if (format.find('=') != std::string::npos) {
            options.export.exportNetworkFormat = format.substr(0, format.find('='));
            options.export.exportNetworkPath = format.substr(format.find('=') + 1);
        } else {
            options.export.exportNetworkFormat = format;
        }
        return true;
    }
    else if (arg.find("--export-spikes=") == 0) {
        std::string format = arg.substr(17);
        if (format.find('=') != std::string::npos) {
            options.export.exportSpikesFormat = format.substr(0, format.find('='));
            options.export.exportSpikesPath = format.substr(format.find('=') + 1);
        } else {
            options.export.exportSpikesFormat = format;
        }
        return true;
    }
    else if (arg.find("--export-memory=") == 0) {
        std::string format = arg.substr(17);
        if (format.find('=') != std::string::npos) {
            options.export.exportMemoryFormat = format.substr(0, format.find('='));
            options.export.exportMemoryPath = format.substr(format.find('=') + 1);
        } else {
            options.export.exportMemoryFormat = format;
        }
        return true;
    }
    else if (arg.find("--export-behavior=") == 0) {
        std::string format = arg.substr(19);
        if (format.find('=') != std::string::npos) {
            options.export.exportBehaviorFormat = format.substr(0, format.find('='));
            options.export.exportBehaviorPath = format.substr(format.find('=') + 1);
        } else {
            options.export.exportBehaviorFormat = format;
        }
        return true;
    }
    else if (arg == "--live-viz") {
        options.export.liveViz = true;
        return true;
    }
    else if (arg.find("--viz-port=") == 0) {
        try {
            options.export.vizPort = std::stoi(arg.substr(11));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--viz-update-interval=") == 0) {
        try {
            options.export.vizUpdateInterval = std::stoi(arg.substr(23));
            return true;
        } catch (...) { return false; }
    }
    
    return false;
}

bool AdvancedCLI::parsePerformanceOptions(const std::string& arg) {
    if (arg.find("--measure=") == 0) {
        std::string measureValue = arg.substr(9);
        if (measureValue.empty()) {
            options.performance.measurementTargets = {"cpu", "memory", "gpu", "network"};
        } else {
            options.performance.measurementTargets.push_back(measureValue);
        }
        return true;
    }
    else if (arg.find("--benchmark=") == 0) {
        options.performance.benchmarkMode = arg.substr(12);
        return true;
    }
    else if (arg.find("--profile-interval=") == 0) {
        try {
            options.performance.profileInterval = std::stoi(arg.substr(19));
            return true;
        } catch (...) { return false; }
    }
    else if (arg == "--performance-metrics") {
        options.performance.performanceMetrics = true;
        return true;
    }
    else if (arg == "--memory-usage") {
        options.performance.memoryUsage = true;
        return true;
    }
    else if (arg == "--neuron-firing-rate") {
        options.performance.neuronFiringRate = true;
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseSystemOptions(const std::string& arg) {
    if (arg.find("--threads=") == 0) {
        try {
            options.system.threads = std::stoi(arg.substr(10));
            return true;
        } catch (...) { return false; }
    }
    else if (arg.find("--simd=") == 0) {
        options.system.simd = parseBoolValue(arg.substr(7));
        return true;
    }
    else if (arg.find("--memory-pools=") == 0) {
        options.system.memoryPools = parseBoolValue(arg.substr(16));
        return true;
    }
    else if (arg.find("--checkpoint-compression=") == 0) {
        options.system.checkpointCompression = arg.substr(24);
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseInteractiveOptions(const std::string& arg) {
    if (arg == "--interactive") {
        options.interactive.interactive = true;
        return true;
    }
    else if (arg.find("--script=") == 0) {
        options.interactive.scriptFile = arg.substr(9);
        return true;
    }
    else if (arg == "--monitor") {
        options.interactive.monitor = true;
        return true;
    }
    else if (arg.find("--inspect-memory=") == 0) {
        std::string memValue = arg.substr(18);
        if (memValue.empty()) {
            options.interactive.memoryInspectionTargets = {"working", "episodic", "semantic", "procedural", "associative"};
        } else {
            options.interactive.memoryInspectionTargets.push_back(memValue);
        }
        return true;
    }
    else if (arg == "--memory-stats") {
        options.interactive.memoryInspectionTargets.push_back("stats");
        return true;
    }
    else if (arg.find("--memory-trace=") == 0) {
        options.interactive.memoryInspectionTargets.push_back("trace:" + arg.substr(18));
        return true;
    }
    else if (arg == "--neuromodulator-stats") {
        options.interactive.neuromodulatorInspectionTargets.push_back("stats");
        return true;
    }
    else if (arg.find("--neuromodulator-trace=") == 0) {
        std::string modValue = arg.substr(24);
        if (!modValue.empty()) {
            options.interactive.neuromodulatorInspectionTargets.push_back(modValue);
        }
        return true;
    }
    else if (arg == "--prediction-error-stats") {
        options.interactive.neuromodulatorInspectionTargets.push_back("prediction_error_stats");
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseConfigOptions(const std::string& arg) {
    if (arg.find("--config") == 0) {
        parseConfigFileOption(arg);
        return true;
    }
    else if (parseSetOption(arg)) {
        return true;
    }
    else if (parseEnableOption(arg)) {
        return true;
    }
    else if (parseOverrideOption(arg)) {
        return true;
    }
    else if (arg.find("--var-ref=") == 0) {
        std::string refValue = arg.substr(11);
        size_t commaPos = refValue.find(',');
        if (commaPos != std::string::npos) {
            std::string key = refValue.substr(0, commaPos);
            std::string value = refValue.substr(commaPos + 1);
            options.configFormat.varRefs[key] = value;
        }
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseEnvironmentOptions(const std::string& arg) {
    if (arg.find("--config-path=") == 0) {
        options.environment.nlmConfigPath = arg.substr(15);
        return true;
    }
    else if (arg.find("--data-path=") == 0) {
        options.environment.nlmDataPath = arg.substr(13);
        return true;
    }
    else if (arg.find("--python-bindings") == 0 || arg == "--enable-python-bindings") {
        options.environment.nlmPythonBindings = true;
        return true;
    }
    
    return false;
}

bool AdvancedCLI::parseSetOption(const std::string& arg) {
    if (arg.find("--set=") == 0) {
        std::string setValue = arg.substr(6);
        if (setValue.find('=') != std::string::npos) {
            size_t eqPos = setValue.find('=');
            std::string key = setValue.substr(0, eqPos);
            std::string value = setValue.substr(eqPos + 1);
            options.configFormat.setOptions[key] = value;
        }
        return true;
    }
    return false;
}

bool AdvancedCLI::parseEnableOption(const std::string& arg) {
    if (arg.find("--enable=") == 0) {
        std::string enableValue = arg.substr(9);
        options.configFormat.enableOptions.push_back(enableValue);
        return true;
    }
    return false;
}

bool AdvancedCLI::parseOverrideOption(const std::string& arg) {
    if (arg.find("--override=") == 0) {
        std::string overrideValue = arg.substr(12);
        if (overrideValue.find(',') != std::string::npos) {
            size_t commaPos = overrideValue.find(',');
            std::string key = overrideValue.substr(0, commaPos);
            std::string value = overrideValue.substr(commaPos + 1);
            options.configFormat.overrideOptions[key] = value;
        }
        return true;
    }
    return false;
}

void AdvancedCLI::parseConfigFileOption(const std::string& arg) {
    std::string configValue = arg.substr(8);
    if (configValue.find('=') != std::string::npos) {
        // --config=file
        options.configFiles.configFiles.push_back(configValue.substr(configValue.find('=') + 1));
    } else if (arg.length() > 8) {
        // --config file (space separated)
        // This would be handled by the legacy parser
        options.configFiles.configFiles.push_back(arg.substr(8));
    }
}

void AdvancedCLI::parseLegacyArguments(int argc, char** argv) {
    // Parse legacy --config=file option
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.legacyConfigFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.legacyConfigFile = argv[++i];
            }
        }
    }
}

bool AdvancedCLI::parseBoolValue(const std::string& value) const {
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes";
}

double AdvancedCLI::parseDoubleValue(const std::string& value) const {
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

int64_t AdvancedCLI::parseIntValue(const std::string& value) const {
    try {
        return std::stoll(value);
    } catch (...) {
        return 0;
    }
}

std::vector<std::string> AdvancedCLI::parseStringList(const std::string& value) const {
    std::vector<std::string> result;
    std::stringstream ss(value);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }
    return result;
}

std::vector<double> AdvancedCLI::parseDoubleList(const std::string& value) const {
    std::vector<double> result;
    std::stringstream ss(value);
    std::string item;
    while (std::getline(ss, item, ',')) {
        try {
            result.push_back(std::stod(item));
        } catch (...) {}
    }
    return result;
}

std::string AdvancedCLI::resolveVariables(const std::string& value) const {
    std::string result = value;
    size_t pos = 0;
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = result.find(' ', pos + 1);
        if (endPos == std::string::npos) endPos = result.size();
        
        std::string varName = result.substr(pos + 1, endPos - pos - 1);
        
        // Check if it's an environment variable
        const char* envValue = std::getenv(varName.c_str());
        if (envValue) {
            result.replace(pos, endPos - pos, envValue);
        } else {
            // Check if it's a config variable reference
            auto it = options.configFormat.varRefs.find(varName);
            if (it != options.configFormat.varRefs.end()) {
                result.replace(pos, endPos - pos, it->second);
            } else {
                // Replace with empty string if not found
                result.replace(pos, endPos - pos, "");
            }
        }
        pos += strlen(envValue ? envValue : "");
    }
    return result;
}

void AdvancedCLI::applyToConfig(std::shared_ptr<Config> config) const {
    if (!config) return;
    
    // Apply simulation control options
    config->set("max_steps", static_cast<int64_t>(options.simulation.maxSteps), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.simulation.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", options.simulation.neuronCount, ConfigSource::CommandLine);
    config->set("region_count", options.simulation.regionCount, ConfigSource::CommandLine);
    config->set("temperature", options.simulation.temperature, ConfigSource::CommandLine);
    config->set("learning_rate", options.simulation.learningRate, ConfigSource::CommandLine);
    config->set("plasticity_enabled", options.simulation.plasticityEnabled, ConfigSource::CommandLine);
    config->set("neuromodulation_enabled", options.simulation.neuromodulationEnabled, ConfigSource::CommandLine);
    config->set("development_enabled", options.simulation.developmentEnabled, ConfigSource::CommandLine);
    config->set("connection_probability", options.simulation.connectionProbability, ConfigSource::CommandLine);
    config->set("stdp_ltp_weight", options.simulation.stdpLtpWeight, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", options.simulation.stdpLtdWeight, ConfigSource::CommandLine);
    config->set("stdp_tau", options.simulation.stdpTau, ConfigSource::CommandLine);
    config->set("synaptogenesis_rate", options.simulation.synaptogenesisRate, ConfigSource::CommandLine);
    config->set("pruning_rate", options.simulation.pruningRate, ConfigSource::CommandLine);
    
    // Apply memory system options
    config->set("working_memory_size", options.memory.workingMemorySize, ConfigSource::CommandLine);
    config->set("episodic_memory_size", options.memory.episodicMemorySize, ConfigSource::CommandLine);
    config->set("memory_replay", options.memory.memoryReplay, ConfigSource::CommandLine);
    config->set("replay_frequency", static_cast<int64_t>(options.memory.replayFrequency), ConfigSource::CommandLine);
    
    // Apply neuromodulation options
    config->set("dopamine_level", options.neuromodulation.dopamineLevel, ConfigSource::CommandLine);
    config->set("curiosity_level", options.neuromodulation.curiosityLevel, ConfigSource::CommandLine);
    config->set("novelty_threshold", options.neuromodulation.noveltyThreshold, ConfigSource::CommandLine);
    config->set("reward_scale", options.neuromodulation.rewardScale, ConfigSource::CommandLine);
    config->set("acetylcholine_level", options.neuromodulation.achLevel, ConfigSource::CommandLine);
    config->set("norepinephrine_level", options.neuromodulation.neLevel, ConfigSource::CommandLine);
    config->set("serotonin_level", options.neuromodulation.fiveHTLevel, ConfigSource::CommandLine);
    config->set("nicotine_level", options.neuromodulation.nicotineLevel, ConfigSource::CommandLine);
    config->set("orexin_level", options.neuromodulation.orexinLevel, ConfigSource::CommandLine);
    
    // Apply advanced configuration format options
    for (const auto& [key, value] : options.configFormat.setOptions) {
        config->set(key, value, ConfigSource::CommandLine);
    }
    
    for (const auto& option : options.configFormat.enableOptions) {
        if (option == "plasticity") {
            config->set("plasticity_enabled", true, ConfigSource::CommandLine);
        } else if (option == "neuromodulation") {
            config->set("neuromodulation_enabled", true, ConfigSource::CommandLine);
        } else if (option == "development") {
            config->set("development_enabled", true, ConfigSource::CommandLine);
        } else if (option == "prediction") {
            config->set("prediction_enabled", true, ConfigSource::CommandLine);
        }
    }
    
    for (const auto& [key, value] : options.configFormat.overrideOptions) {
        // Parse numeric values
        try {
            if (key.find("count") != std::string::npos || key.find("steps") != std::string::npos) {
                config->set(key, std::stoll(value), ConfigSource::CommandLine);
            } else if (key.find("rate") != std::string::npos || key.find("time") != std::string::npos) {
                config->set(key, std::stod(value), ConfigSource::CommandLine);
            } else if (key.find("enabled") != std::string::npos || key.find("enable") != std::string::npos) {
                config->set(key, parseBoolValue(value), ConfigSource::CommandLine);
            } else {
                config->set(key, value, ConfigSource::CommandLine);
            }
        } catch (...) {
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Apply logging options
    config->set("log_level", options.logging.logLevel, ConfigSource::CommandLine);
    if (options.logging.logFile) {
        config->set("log_file", *options.logging.logFile, ConfigSource::CommandLine);
    }
    config->set("log_format", options.logging.logFormat, ConfigSource::CommandLine);
    config->set("event_log", options.logging.eventLog, ConfigSource::CommandLine);
    config->set("trace_sim", options.logging.traceSim, ConfigSource::CommandLine);
    config->set("structured_log", options.logging.structuredLog, ConfigSource::CommandLine);
    config->set("log_metrics", options.logging.logMetrics, ConfigSource::CommandLine);
    config->set("log_neural_activity", options.logging.logNeuralActivity, ConfigSource::CommandLine);
    
    // Apply debug options
    config->set("debug_level", static_cast<int64_t>(options.debug.debugLevel), ConfigSource::CommandLine);
    if (!options.debug.traceTargets.empty()) {
        std::string traceStr = "";
        for (size_t i = 0; i < options.debug.traceTargets.size(); ++i) {
            if (i > 0) traceStr += ",";
            traceStr += options.debug.traceTargets[i];
        }
        config->set("trace_targets", traceStr, ConfigSource::CommandLine);
    }
    if (!options.debug.profileTargets.empty()) {
        std::string profileStr = "";
        for (size_t i = 0; i < options.debug.profileTargets.size(); ++i) {
            if (i > 0) profileStr += ",";
            profileStr += options.debug.profileTargets[i];
        }
        config->set("profile_targets", profileStr, ConfigSource::CommandLine);
    }
    if (options.debug.checkpointFile) {
        config->set("checkpoint_file", *options.debug.checkpointFile, ConfigSource::CommandLine);
    }
    if (!options.debug.visualizationTargets.empty()) {
        std::string vizStr = "";
        for (size_t i = 0; i < options.debug.visualizationTargets.size(); ++i) {
            if (i > 0) vizStr += ",";
            vizStr += options.debug.visualizationTargets[i];
        }
        config->set("visualization_targets", vizStr, ConfigSource::CommandLine);
    }
    
    // Apply export options
    config->set("export_network_format", options.export.exportNetworkFormat, ConfigSource::CommandLine);
    if (options.export.exportNetworkPath) {
        config->set("export_network_path", *options.export.exportNetworkPath, ConfigSource::CommandLine);
    }
    config->set("export_spikes_format", options.export.exportSpikesFormat, ConfigSource::CommandLine);
    if (options.export.exportSpikesPath) {
        config->set("export_spikes_path", *options.export.exportSpikesPath, ConfigSource::CommandLine);
    }
    config->set("export_memory_format", options.export.exportMemoryFormat, ConfigSource::CommandLine);
    if (options.export.exportMemoryPath) {
        config->set("export_memory_path", *options.export.exportMemoryPath, ConfigSource::CommandLine);
    }
    config->set("export_behavior_format", options.export.exportBehaviorFormat, ConfigSource::CommandLine);
    if (options.export.exportBehaviorPath) {
        config->set("export_behavior_path", *options.export.exportBehaviorPath, ConfigSource::CommandLine);
    }
    config->set("live_viz", options.export.liveViz, ConfigSource::CommandLine);
    config->set("viz_port", static_cast<int64_t>(options.export.vizPort), ConfigSource::CommandLine);
    config->set("viz_update_interval", static_cast<int64_t>(options.export.vizUpdateInterval), ConfigSource::CommandLine);
    
    // Apply performance options
    if (!options.performance.measurementTargets.empty()) {
        std::string measureStr = "";
        for (size_t i = 0; i < options.performance.measurementTargets.size(); ++i) {
            if (i > 0) measureStr += ",";
            measureStr += options.performance.measurementTargets[i];
        }
        config->set("measurement_targets", measureStr, ConfigSource::CommandLine);
    }
    config->set("benchmark_mode", options.performance.benchmarkMode, ConfigSource::CommandLine);
    config->set("profile_interval", static_cast<int64_t>(options.performance.profileInterval), ConfigSource::CommandLine);
    config->set("performance_metrics", options.performance.performanceMetrics, ConfigSource::CommandLine);
    config->set("memory_usage", options.performance.memoryUsage, ConfigSource::CommandLine);
    config->set("neuron_firing_rate", options.performance.neuronFiringRate, ConfigSource::CommandLine);
    
    // Apply system options
    config->set("threads", static_cast<int64_t>(options.system.threads), ConfigSource::CommandLine);
    config->set("simd", options.system.simd, ConfigSource::CommandLine);
    config->set("memory_pools", options.system.memoryPools, ConfigSource::CommandLine);
    config->set("checkpoint_compression", options.system.checkpointCompression, ConfigSource::CommandLine);
    
    // Apply environment variable overrides
    if (options.environment.nlmLogLevel) {
        config->set("env_log_level_override", *options.environment.nlmLogLevel, ConfigSource::CommandLine);
    }
    if (options.environment.nlmConfigPath) {
        config->set("env_config_path_override", *options.environment.nlmConfigPath, ConfigSource::CommandLine);
    }
    if (options.environment.nlmDataPath) {
        config->set("env_data_path_override", *options.environment.nlmDataPath, ConfigSource::CommandLine);
    }
    config->set("python_bindings", options.environment.nlmPythonBindings, ConfigSource::CommandLine);
    
    // Apply interactive options
    config->set("interactive", options.interactive.interactive, ConfigSource::CommandLine);
    if (options.interactive.scriptFile) {
        config->set("script_file", *options.interactive.scriptFile, ConfigSource::CommandLine);
    }
    config->set("monitor", options.interactive.monitor, ConfigSource::CommandLine);
    if (!options.interactive.memoryInspectionTargets.empty()) {
        std::string memStr = "";
        for (size_t i = 0; i < options.interactive.memoryInspectionTargets.size(); ++i) {
            if (i > 0) memStr += ",";
            memStr += options.interactive.memoryInspectionTargets[i];
        }
        config->set("memory_inspection_targets", memStr, ConfigSource::CommandLine);
    }
    if (!options.interactive.neuromodulatorInspectionTargets.empty()) {
        std::string modStr = "";
        for (size_t i = 0; i < options.interactive.neuromodulatorInspectionTargets.size(); ++i) {
            if (i > 0) modStr += ",";
            modStr += options.interactive.neuromodulatorInspectionTargets[i];
        }
        config->set("neuromodulator_inspection_targets", modStr, ConfigSource::CommandLine);
    }
}

std::vector<std::string> AdvancedCLI::validate() const {
    std::vector<std::string> errors;
    
    // Validate simulation options
    if (options.simulation.maxSteps <= 0) {
        errors.push_back("max_steps must be positive");
    }
    if (options.simulation.timestep <= 0.0) {
        errors.push_back("timestep must be positive");
    }
    if (options.simulation.neuronCount <= 0) {
        errors.push_back("neuron_count must be positive");
    }
    if (options.simulation.regionCount <= 0) {
        errors.push_back("region_count must be positive");
    }
    if (options.simulation.temperature < 0.0) {
        errors.push_back("temperature must be non-negative");
    }
    if (options.simulation.learningRate < 0.0) {
        errors.push_back("learning_rate must be non-negative");
    }
    
    // Validate memory options
    if (options.memory.workingMemorySize <= 0) {
        errors.push_back("working_memory_size must be positive");
    }
    if (options.memory.episodicMemorySize <= 0) {
        errors.push_back("episodic_memory_size must be positive");
    }
    if (options.memory.memoryReplay && options.memory.replayFrequency <= 0) {
        errors.push_back("replay_frequency must be positive when memory_replay is enabled");
    }
    
    // Validate debug options
    if (options.debug.debugLevel < 0 || options.debug.debugLevel > 3) {
        errors.push_back("debug_level must be 0, 1, 2, or 3");
    }
    
    // Validate logging options
    std::vector<std::string> validLogLevels = {"debug", "info", "warning", "error"};
    if (std::find(validLogLevels.begin(), validLogLevels.end(), options.logging.logLevel) == validLogLevels.end()) {
        errors.push_back("Invalid log_level: " + options.logging.logLevel);
    }
    
    std::vector<std::string> validLogFormats = {"text", "json", "csv"};
    if (std::find(validLogFormats.begin(), validLogFormats.end(), options.logging.logFormat) == validLogFormats.end()) {
        errors.push_back("Invalid log_format: " + options.logging.logFormat);
    }
    
    // Validate export options
    std::vector<std::string> validExportFormats = {"none", "dot", "gexf", "graphml", "txt", "csv", "json"};
    if (std::find(validExportFormats.begin(), validExportFormats.end(), options.export.exportNetworkFormat) == validExportFormats.end()) {
        errors.push_back("Invalid export_network_format: " + options.export.exportNetworkFormat);
    }
    if (std::find(validExportFormats.begin(), validExportFormats.end(), options.export.exportSpikesFormat) == validExportFormats.end()) {
        errors.push_back("Invalid export_spikes_format: " + options.export.exportSpikesFormat);
    }
    if (std::find(validExportFormats.begin(), validExportFormats.end(), options.export.exportMemoryFormat) == validExportFormats.end()) {
        errors.push_back("Invalid export_memory_format: " + options.export.exportMemoryFormat);
    }
    if (std::find(validExportFormats.begin(), validExportFormats.end(), options.export.exportBehaviorFormat) == validExportFormats.end()) {
        errors.push_back("Invalid export_behavior_format: " + options.export.exportBehaviorFormat);
    }
    
    // Validate performance options
    if (options.performance.profileInterval <= 0) {
        errors.push_back("profile_interval must be positive");
    }
    
    // Validate system options
    if (options.system.threads < 0) {
        errors.push_back("threads must be non-negative");
    }
    
    std::vector<std::string> validCompressionTypes = {"none", "gzip", "zlib"};
    if (std::find(validCompressionTypes.begin(), validCompressionTypes.end(), options.system.checkpointCompression) == validCompressionTypes.end()) {
        errors.push_back("Invalid checkpoint_compression: " + options.system.checkpointCompression);
    }
    
    return errors;
}

std::string AdvancedCLI::getHelpText() {
    std::stringstream help;
    
    help << "=== NLM Advanced CLI Options ===\n\n";
    
    help << "Simulation Control Options:\n";
    help << "  --max-steps=N              Maximum simulation steps (default: 1000)\n";
    help << "  --timestep=N               Override simulation timestep (default: 0.001)\n";
    help << "  --neuron-count=N           Override neuron count (default: 500)\n";
    help << "  --region-count=N           Override region count (default: 1)\n";
    help << "  --temperature=N            Temperature parameter (default: 1.0)\n";
    help << "  --learning-rate=N           Global learning rate multiplier (default: 1.0)\n";
    help << "  --plasticity-enabled=[true/false] Enable/disable plasticity (default: true)\n";
    help << "  --neuromodulation-enabled=[true/false] Enable/disable neuromodulation (default: true)\n\n";
    
    help << "Memory System Options:\n";
    help << "  --working-memory-size=N     Working memory capacity (default: 1024)\n";
    help << "  --episodic-memory-size=N    Episodic memory capacity (default: 5000)\n";
    help << "  --memory-replay=[true/false] Enable/disable memory replay (default: false)\n";
    help << "  --replay-frequency=N       Replay frequency during rest (default: 10)\n\n";
    
    help << "Neuromodulation Options:\n";
    help << "  --dopamine-level=N         Set initial dopamine level (default: 0.5)\n";
    help << "  --curiosity-level=N        Set initial curiosity level (default: 0.3)\n";
    help << "  --novelty-threshold=N      Novelty detection threshold (default: 0.5)\n";
    help << "  --reward-scale=N           Reward scaling factor (default: 1.0)\n\n";
    
    help << "Debug Options:\n";
    help << "  --debug-level=0|1|2|3      Debug verbosity (default: 0)\n";
    help << "  --trace=[neuron|synapse|memory|neuromodulator] Enable specific tracing\n";
    help << "  --profile=[simulation|memory|neuromodulation] Enable profiling\n";
    help << "  --checkpoint=[file]         Enable checkpointing with file path\n";
    help << "  --visualize=[network|spikes|memory] Enable visualization\n\n";
    
    help << "Logging Options:\n";
    help << "  --log-level=debug|info|warning|error Global log level (default: info)\n";
    help << "  --log-file=file            Write logs to file\n";
    help << "  --log-format=json|csv|text Log format (default: text)\n";
    help << "  --event-log                Log all events for debugging\n";
    help << "  --trace-sim                Enable step-by-step trace\n";
    help << "  --structured-log           JSON/CSV structured logging\n";
    help << "  --log-metrics              Log performance metrics\n";
    help << "  --log-neural-activity      Log neural activity patterns\n\n";
    
    help << "Export Options:\n";
    help << "  --export-network=dot|gexf|graphml Export neural network\n";
    help << "  --export-spikes=txt|csv    Export spike train data\n";
    help << "  --export-memory=txt|json   Export memory contents\n";
    help << "  --export-behavior=txt|csv  Export behavioral data\n";
    help << "  --live-viz                 Enable real-time visualization\n";
    help << "  --viz-port=N               Visualization server port (default: 8080)\n";
    help << "  --viz-update-interval=N    Visualization update rate (default: 1000)\n\n";
    
    help << "Performance Options:\n";
    help << "  --measure=[cpu|memory|gpu|network] Performance measurement\n";
    help << "  --benchmark=[mode]         Run benchmark suites\n";
    help << "  --profile-interval=N       Profiling interval in steps (default: 100)\n";
    help << "  --performance-metrics      Enable performance metric collection\n";
    help << "  --memory-usage             Track memory consumption\n";
    help << "  --neuron-firing-rate       Monitor firing patterns\n\n";
    
    help << "System Options:\n";
    help << "  --threads=N                Number of threads for parallel processing\n";
    help << "  --simd=[true/false]        Enable SIMD optimization (default: false)\n";
    help << "  --memory-pools=[true/false] Enable memory pools (default: false)\n";
    help << "  --checkpoint-compression=[gzip|zlib|none] Checkpoint compression (default: gzip)\n\n";
    
    help << "Interactive Options:\n";
    help << "  --interactive              Enable interactive debugging mode\n";
    help << "  --script=file             Execute debugging script\n";
    help << "  --monitor                 Start monitoring thread\n";
    help << "  --inspect-memory=[working|episodic|semantic|procedural|associative] Memory inspection\n";
    help << "  --memory-stats             Print memory system statistics\n";
    help << "  --memory-trace=[neuron_id|region_id] Trace memory access\n";
    help << "  --neuromodulator-stats     Print neuromodulator levels\n";
    help << "  --neuromodulator-trace=[dopamine|curiosity|novelty|ACh|NE|5-HT] Neuromodulator tracing\n";
    help << "  --prediction-error-stats   Print prediction system stats\n\n";
    
    help << "Configuration Options:\n";
    help << "  --set=key=value            Set configuration key/value\n";
    help << "  --enable=plasticity        Enable plasticity\n";
    help << "  --enable=neuromodulation    Enable neuromodulation\n";
    help << "  --enable=development       Enable development\n";
    help << "  --override=simulation_timestep,0.0001 Override parameter\n";
    help << "  --override=learning_rate,2.0 Override parameter\n";
    help << "  --var-ref=config_key       Variable referencing\n";
    help << "  --config=file              Load configuration from file (can be repeated)\n\n";
    
    help << "Environment Options:\n";
    help << "  --config-path=path         Override config path via environment variable\n";
    help << "  --data-path=path           Set data directory for outputs\n";
    help << "  --python-bindings          Enable Python bindings\n\n";
    
    help << "Examples:\n";
    help << "  nlm --max-steps=5000 --neuron-count=1000 --plasticity-enabled=true --trace=neuron --log-level=debug --benchmark=stress\n";
    help << "  nlm --set=neuron_count,500 --enable=plasticity --enable=neuromodulation --memory-replay=true\n";
    help << "  nlm --interactive --script=debug_script.nlua --monitor\n";
    help << "  nlm --export-network=graphml --export-spikes=csv --live-viz\n";
    
    return help.str();
}

std::string AdvancedCLI::getVersionInfo() {
    return "NLM v2.0+ Advanced CLI"; // Would include actual version info
}

bool AdvancedCLI::hasOption(const std::string& option) const {
    std::string normalized = normalizeOptionName(option);
    
    // Check all option types
    if (normalized == "maxsteps" || normalized == "simulation_timestep" || 
        normalized == "neuron_count" || normalized == "region_count" ||
        normalized == "temperature" || normalized == "learning_rate" ||
        normalized == "plasticity_enabled" || normalized == "neuromodulation_enabled") {
        return true;
    }
    
    // Add other option checks...
    
    return false;
}

std::string AdvancedCLI::getOptionHelp(const std::string& option) const {
    // Return help for specific option
    return "Help for " + option + " (to be implemented)";
}

std::string AdvancedCLI::normalizeOptionName(const std::string& option) const {
    std::string normalized = option;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    // Replace hyphens with underscores
    std::replace(normalized.begin(), normalized.end(), '-', '_');
    
    return normalized;
}

} // namespace nlm
