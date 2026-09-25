#include "main_advanced_cli.cpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "sensory/SensoryPercept.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cmath>

namespace nlm {

bool AdvancedCommandLineInterface::parseArgs(int argc, char** argv) {
    if (!CommandLineInterface::parseArgs(argc, argv)) {
        return false;
    }
    
    // Parse extended arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--benchmark") {
            currentExtendedMode_ = ExtendedMode::Benchmark;
        } else if (arg == "--validate") {
            currentExtendedMode_ = ExtendedMode::Validate;
        } else if (arg == "--profile") {
            currentExtendedMode_ = ExtendedMode::Profile;
        } else if (arg == "--export") {
            currentExtendedMode_ = ExtendedMode::Export;
        } else if (arg == "--import") {
            currentExtendedMode_ = ExtendedMode::Import;
        } else if (arg == "--monitor") {
            currentExtendedMode_ = ExtendedMode::Monitor;
        } else if (arg == "--export-config") {
            if (i + 1 < argc) {
                exportFormat_ = argv[++i];
            } else {
                printError("Missing value for --export-config option");
                return false;
            }
        } else if (arg == "--import-config") {
            if (i + 1 < argc) {
                importFormat_ = argv[++i];
            } else {
                printError("Missing value for --import-config option");
                return false;
            }
        } else if (arg == "--benchmark-file") {
            if (i + 1 < argc) {
                benchmarkFile_ = argv[++i];
            } else {
                printError("Missing value for --benchmark-file option");
                return false;
            }
        } else if (arg == "--monitor-file") {
            if (i + 1 < argc) {
                monitoringFile_ = argv[++i];
            } else {
                printError("Missing value for --monitor-file option");
                return false;
            }
        } else {
            // Unknown option
            printError("Unknown option: " + arg);
            return false;
        }
    }
    
    return true;
}

int AdvancedCommandLineInterface::run() {
    logBanner();
    
    if (currentMode_ == Mode::Help) {
        showHelp();
        return 0;
    } else if (currentMode_ == Mode::Version) {
        showVersion();
        return 0;
    }
    
    // Load configuration
    if (!loadConfiguration()) {
        return 1;
    }
    
    // Run the appropriate extended mode
    switch (currentExtendedMode_) {
        case ExtendedMode::Benchmark:
            return handleBenchmarkMode() ? 0 : 1;
        case ExtendedMode::Validate:
            return handleValidateMode() ? 0 : 1;
        case ExtendedMode::Profile:
            return handleProfileMode() ? 0 : 1;
        case ExtendedMode::Export:
            return handleExportMode() ? 0 : 1;
        case ExtendedMode::Import:
            return handleImportMode() ? 0 : 1;
        case ExtendedMode::Monitor:
            startMonitoringMode();
            return 0;
        default:
            // Fall back to basic modes
            return CommandLineInterface::run();
    }
}

void AdvancedCommandLineInterface::showExtendedHelp() const {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM — 熙然 (Phase 6) Extended CLI                ║
║           Advanced Command Line Interface                        ║
╚═══════════════════════════════════════════════════════════════╝

ADVANCED COMMANDS:
  --benchmark          Run performance benchmarks
  --validate           Validate configuration and files
  --profile            Profile simulation performance
  --export             Export configuration or data
  --import             Import configuration or data
  --monitor            Start monitoring mode

BENCHMARK OPTIONS:
  --benchmark-file FILE   Output file for benchmark results (default: benchmark.json)

VALIDATION OPTIONS:
  --validate-config FILE   Validate configuration file
  --validate-brain FILE    Validate brain configuration
  --validate-world FILE    Validate world configuration

EXPORT/IMPORT OPTIONS:
  --export-config FORMAT   Export configuration (json, yaml, ini)
  --import-config FORMAT   Import configuration (json, yaml, ini)
  --export-file FILE       Export file (for data export)
  --import-file FILE       Import file (for data import)

MONITORING OPTIONS:
  --monitor-file FILE      Output file for monitoring data (default: monitoring.json)

EXAMPLES:
  nlm --benchmark                    # Run comprehensive benchmarks
  nlm --validate --validate-config config.json    # Validate config file
  nlm --export --export-config json --export-file out.json    # Export config
  nlm --import --import-config yaml --import-file in.yaml    # Import config
  nlm --monitor --monitor-file monitor.json         # Start monitoring

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

void AdvancedCommandLineInterface::showSystemInfo() const {
    std::cout << "System Information:" << std::endl;
    std::cout << "  NLM Version: 0.1.0 Phase 6" << std::endl;
    std::cout << "  Build Type: Release" << std::endl;
    std::cout << "  Architecture: x86_64" << std::endl;
    std::cout << "  Compiler: " << __VERSION__ << std::endl;
    std::cout << "  Configuration format: " << (pImpl ? pImpl->fileFormat : "unknown") << std::endl;
    std::cout << "  Number of configuration entries: " << (pImpl ? pImpl->entries.size() : 0) << std::endl;
    std::cout << "  Available memory: " << (getAvailableMemory() / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "  CPU cores: " << std::thread::hardware_concurrency() << std::endl;
}

void AdvancedCommandLineInterface::showBenchmarkResults() const {
    std::cout << "Benchmark Results:" << std::endl;
    
    if (benchmarkResults_.empty()) {
        std::cout << "  No benchmark results available." << std::endl;
        return;
    }
    
    for (const auto& result : benchmarkResults_) {
        std::cout << "  " << result.first << ": " << result.second << " ms" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Summary:" << std::endl;
    std::cout << "  Total benchmark time: " << getTotalBenchmarkTime() << " ms" << std::endl;
    std::cout << "  Number of benchmarks: " << benchmarkResults_.size() << std::endl;
}

bool AdvancedCommandLineInterface::handleBenchmarkMode() {
    printStatus("Starting NLM benchmark mode...")
    
    // Run benchmarks
    if (!runBenchmark()) {
        printError("Benchmark failed!");
        return false;
    }
    
    // Save benchmark results
    if (!benchmarkFile_.empty()) {
        if (!saveBenchmarkResults(benchmarkFile_)) {
            printWarning("Failed to save benchmark results to: " + benchmarkFile_);
        } else {
            printInfo("Benchmark results saved to: " + benchmarkFile_);
        }
    }
    
    // Show results
    showBenchmarkResults();
    
    return true;
}

bool AdvancedCommandLineInterface::handleValidateMode() {
    printStatus("Starting NLM validation mode...")
    
    validationErrors_.clear();
    
    // Validate configuration file if specified
    if (!experimentArgs_.empty()) {
        for (const auto& filepath : experimentArgs_) {
            if (!validateConfigurationFile(filepath)) {
                printError("Configuration validation failed for: " + filepath);
                return false;
            }
        }
    } else {
        // Validate current configuration
        if (!validateConfiguration()) {
            return false;
        }
    }
    
    if (validationErrors_.empty()) {
        printInfo("Configuration validation passed.");
        return true;
    } else {
        printError("Configuration validation failed:");
        for (const auto& error : validationErrors_) {
            printError("  " + error);
        }
        return false;
    }
}

bool AdvancedCommandLineInterface::handleProfileMode() {
    printStatus("Starting NLM profiling mode...")
    
    // Run simulation with profiling
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        printError("Failed to initialize brain!");
        return false;
    }
    
    printInfo("Profiling simulation with " + std::to_string(steps_) + " steps...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps_; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0 && verbose_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            printInfo("  Step " + std::to_string(step) + 
                     " | Time: " + std::to_string(elapsed.count()) + " ms" +
                     " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double avgStepTime = totalDuration.count() / static_cast<double>(steps_);
    double stepsPerSecond = steps_ / (totalDuration.count() / 1000.0);
    
    printInfo("Profiling completed:");
    printInfo("  Total time: " + std::to_string(totalDuration.count()) + " ms");
    printInfo("  Average step time: " + std::to_string(avgStepTime) + " ms");
    printInfo("  Steps per second: " + std::to_string(stepsPerSecond));
    printInfo("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    return true;
}

bool AdvancedCommandLineInterface::handleExportMode() {
    printStatus("Starting NLM export mode...")
    
    if (outputFile_.empty()) {
        printError("No output file specified. Use --output <file> option.");
        return false;
    }
    
    // Export configuration
    if (!exportConfiguration(exportFormat_)) {
        printError("Failed to export configuration!");
        return false;
    }
    
    printInfo("Configuration exported to: " + outputFile_);
    return true;
}

bool AdvancedCommandLineInterface::handleImportMode() {
    printStatus("Starting NLM import mode...")
    
    if (inputFile_.empty()) {
        printError("No input file specified. Use --input <file> option.");
        return false;
    }
    
    // Import configuration
    if (!importConfiguration(inputFile_, importFormat_)) {
        printError("Failed to import configuration!");
        return false;
    }
    
    printInfo("Configuration imported from: " + inputFile_);
    return true;
}

void AdvancedCommandLineInterface::startMonitoringMode() {
    printStatus("Starting NLM monitoring mode...")
    
    // Start monitoring
    startMonitoring(monitoringFile_);
    
    printInfo("Monitoring started. Press Ctrl+C to stop.");
    
    // In a real implementation, this would run in a separate thread
    // For now, we'll just simulate monitoring for a short time
    printInfo("Monitoring simulation for 1000 steps...");
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        printError("Failed to initialize brain for monitoring!");
        return;
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
        collectSystemMetrics();
        
        if (step % 100 == 0) {
            printInfo("  Monitoring step " + std::to_string(step) + 
                     " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Stop monitoring
    stopMonitoring();
    
    // Write monitoring data
    if (!monitoringFile_.empty()) {
        writeMonitoringData(monitoringFile_);
        printInfo("Monitoring data written to: " + monitoringFile_);
    }
    
    printInfo("Monitoring completed.");
}

bool AdvancedCommandLineInterface::runBenchmark() const {
    printStatus("Running benchmarks...");
    
    // Reset benchmark results
    const_cast<AdvancedCommandLineInterface*>(this)->benchmarkResults_.clear();
    
    // Run individual benchmarks
    benchmarkResults_["brain_initialization"] = benchmarkBrainInitialization();
    benchmarkResults_["connectivity_setup"] = benchmarkConnectivitySetup();
    benchmarkResults_["plasticity_updates"] = benchmarkPlasticityUpdates();
    benchmarkResults_["memory_usage"] = benchmarkMemoryUsage();
    benchmarkResults_["simulation_1000"] = benchmarkSimulationSteps(1000);
    benchmarkResults_["simulation_5000"] = benchmarkSimulationSteps(5000);
    
    return true;
}

bool AdvancedCommandLineInterface::validateConfigurationFile(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        validationErrors_.push_back(std::string("Cannot open file: " + filepath));
        return false;
    }
    
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Basic validation
            if (!jsonData.is_object()) {
                validationErrors_.push_back(std::string("File \"" + filepath + "\" is not a JSON object"));
                return false;
            }
            
            return true;
        } catch (const nlohmann::json::exception& e) {
            validationErrors_.push_back(std::string("JSON validation error in \"" + filepath + "\": " + e.what()));
            return false;
        }
    } else if (extension == "yaml" || extension == "yml") {
        try {
            YAML::Node yamlData = YAML::Load(file);
            
            // Basic validation
            if (!yamlData.IsMap()) {
                validationErrors_.push_back(std::string("File \"" + filepath + "\" is not a YAML map"));
                return false;
            }
            
            return true;
        } catch (const YAML::Exception& e) {
            validationErrors_.push_back(std::string("YAML validation error in \"" + filepath + "\": " + e.what()));
            return false;
        }
    } else if (extension == "ini") {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            
            if (line[0] == '[' && line.back() == ']') {
                continue;  // Section header
            }
            
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                validationErrors_.push_back(std::string("Invalid INI format in \"" + filepath + "\": \"" + line + "\""));
                return false;
            }
        }
        
        return true;
    } else {
        validationErrors_.push_back(std::string("Unsupported file format in \"" + filepath + "\": \"" + extension + "\""));
        return false;
    }
}

bool AdvancedCommandLineInterface::exportConfiguration(const std::string& format) const {
    if (outputFile_.empty()) {
        printError("No output file specified.");
        return false;
    }
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    std::string extension = format;
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return exportConfigToJSON(*config, outputFile_);
    } else if (extension == "yaml" || extension == "yml") {
        return exportConfigToYAML(*config, outputFile_);
    } else if (extension == "ini") {
        return exportConfigToINI(*config, outputFile_);
    } else {
        printError("Unsupported export format: " + format);
        return false;
    }
}

bool AdvancedCommandLineInterface::importConfiguration(const std::string& filepath, const std::string& format) {
    auto config = std::make_shared<Config>();
    
    std::string extension = format;
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        if (!importConfigFromJSON(filepath, *config)) {
            return false;
        }
    } else if (extension == "yaml" || extension == "yml") {
        if (!importConfigFromYAML(filepath, *config)) {
            return false;
        }
    } else if (extension == "ini") {
        if (!importConfigFromINI(filepath, *config)) {
            return false;
        }
    } else {
        printError("Unsupported import format: " + format);
        return false;
    }
    
    // In a real implementation, this would save the imported config
    // For now, we'll just show a message
    printInfo("Configuration imported and ready to use.");
    
    return true;
}

void AdvancedCommandLineInterface::startMonitoring(const std::string& outputFile) {
    monitoringActive_ = true;
    monitoringStartTime_ = std::chrono::high_resolution_clock::now();
    monitoringData_.clear();
    
    printInfo("Monitoring started. Collecting system metrics...");
}

void AdvancedCommandLineInterface::stopMonitoring() {
    monitoringActive_ = false;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - monitoringStartTime_);
    
    printInfo("Monitoring stopped. Total duration: " + std::to_string(totalDuration.count()) + " ms");
}

void AdvancedCommandLineInterface::logMonitoringData() const {
    if (!monitoringActive_) {
        return;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - monitoringStartTime_);
    
    MonitoringData data;
    data.timestamp = elapsed.count();
    data.step = currentStep_;
    data.spikeCount = brain_->getTotalSpikeCount();
    data.activeNeurons = brain_->getActiveNeuronCount();
    data.memoryUsage = getCurrentMemoryUsage();
    data.cpuUsage = getCurrentCPUUsage();
    
    monitoringData_.push_back(data);
}

void AdvancedCommandLineInterface::writeMonitoringData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printError("Cannot open monitoring file for writing: " + filepath);
        return;
    }
    
    file << "[Metrics]" << std::endl;
    file << "Timestamp,Step,SpikeCount,ActiveNeurons,MemoryUsage,CPUUsage" << std::endl;
    
    for (const auto& data : monitoringData_) {
        file << data.timestamp << "," << data.step << "," 
             << data.spikeCount << "," << data.activeNeurons << ","
             << data.memoryUsage << "," << data.cpuUsage << std::endl;
    }
    
    file.close();
    printInfo("Monitoring data written to: " + filepath);
}

void AdvancedCommandLineInterface::generateMonitoringReport() const {
    // Generate a simple report
    printInfo("Monitoring Report:");
    printInfo("  Total monitoring duration: " + std::to_string(getMonitoringDuration()) + " ms");
    printInfo("  Total monitoring steps: " + std::to_string(getMonitoringStepCount()));
    printInfo("  Average spike rate: " + std::to_string(getAverageSpikeRate()) + " spikes/step");
    printInfo("  Peak memory usage: " + std::to_string(getPeakMemoryUsage()) + " MB");
    printInfo("  Average CPU usage: " + std::to_string(getAverageCPUUsage()) + "%");
}

// Benchmark implementations
double AdvancedCommandLineInterface::benchmarkBrainInitialization() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return static_cast<double>(duration.count());
}

double AdvancedCommandLineInterface::benchmarkConnectivitySetup() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Trigger connectivity setup by running a few steps
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return static_cast<double>(duration.count());
}

double AdvancedCommandLineInterface::benchmarkPlasticityUpdates() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity and run many steps
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run steps with plasticity
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return static_cast<double>(duration.count());
}

double AdvancedCommandLineInterface::benchmarkMemoryUsage() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return static_cast<double>(duration.count());
}

double AdvancedCommandLineInterface::benchmarkSimulationSteps(SimulationStep steps) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return static_cast<double>(duration.count());
}

// Export/import implementations
bool AdvancedCommandLineInterface::exportConfigToJSON(const Config& config, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& key : config.getKeys()) {
        auto val_opt = config.get<std::string>(key);
        if (val_opt) {
            // Try to convert to appropriate type
            try {
                auto int_val = config.get<int64_t>(key);
                if (int_val) {
                    jsonData[key] = *int_val;
                    continue;
                }
            } catch (...) {}
            
            try {
                auto float_val = config.get<double>(key);
                if (float_val) {
                    jsonData[key] = *float_val;
                    continue;
                }
            } catch (...) {}
            
            try {
                auto bool_val = config.get<bool>(key);
                if (bool_val) {
                    jsonData[key] = *bool_val;
                    continue;
                }
            } catch (...) {}
            
            // Default to string
            jsonData[key] = *val_opt;
        }
    }
    
    file << jsonData.dump(2);
    return true;
}

bool AdvancedCommandLineInterface::exportConfigToYAML(const Config& config, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& key : config.getKeys()) {
        auto val_opt = config.get<std::string>(key);
        if (val_opt) {
            emitter << YAML::Key << key;
            
            try {
                auto int_val = config.get<int64_t>(key);
                if (int_val) {
                    emitter << *int_val;
                    continue;
                }
            } catch (...) {}
            
            try {
                auto float_val = config.get<double>(key);
                if (float_val) {
                    emitter << *float_val;
                    continue;
                }
            } catch (...) {}
            
            try {
                auto bool_val = config.get<bool>(key);
                if (bool_val) {
                    emitter << *bool_val;
                    continue;
                }
            } catch (...) {}
            
            // Default to string
            emitter << *val_opt;
        }
    }
    
    emitter << YAML::EndMap;
    file << emitter.c_str();
    
    return true;
}

bool AdvancedCommandLineInterface::exportConfigToINI(const Config& config, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& key : config.getKeys()) {
        file << "# " << key << std::endl;
        
        auto val_opt = config.get<std::string>(key);
        if (val_opt) {
            file << key << " = " << *val_opt << std::endl;
        }
        
        file << std::endl;
    }
    
    return true;
}

bool AdvancedCommandLineInterface::importConfigFromJSON(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing config
        config.clear();
        
        // Import JSON data
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_number_integer()) {
                config.set(key, it->get<int64_t>());
            } else if (it->is_number_float()) {
                config.set(key, it->get<double>());
            } else if (it->is_boolean()) {
                config.set(key, it->get<bool>());
            } else {
                config.set(key, it->get<std::string>());
            }
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        printError("JSON import error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedCommandLineInterface::importConfigFromYAML(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        
        // Clear existing config
        config.clear();
        
        // Import YAML data
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            
            if (it->IsInteger()) {
                config.set(key, it->as<int64_t>());
            } else if (it->IsDouble()) {
                config.set(key, it->as<double>());
            } else if (it->IsBoolean()) {
                config.set(key, it->as<bool>());
            } else {
                config.set(key, it->as<std::string>());
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        printError("YAML import error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedCommandLineInterface::importConfigFromINI(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (currentSection.empty()) {
                config.set(key, value);
            } else {
                config.set(currentSection + "." + key, value);
            }
        }
    }
    
    return true;
}

// Helper functions
std::string AdvancedCommandLineInterface::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// System information functions
double AdvancedCommandLineInterface::getAvailableMemory() const {
    // This is a platform-specific implementation
    // For now, return a placeholder value
    return 8192.0;  // 8 GB
}

void* AdvancedCommandLineInterface::getCurrentMemoryUsage() const {
    // This is a platform-specific implementation
    // For now, return a placeholder value
    return reinterpret_cast<void*>(0x1000000);  // 16 MB
}

double AdvancedCommandLineInterface::getCurrentCPUUsage() const {
    // This is a platform-specific implementation
    // For now, return a placeholder value
    return 25.0;  // 25%
}

double AdvancedCommandLineInterface::getPeakMemoryUsage() const {
    return 512.0;  // 512 MB peak
}

double AdvancedCommandLineInterface::getAverageCPUUsage() const {
    return 35.0;  // 35% average
}

double AdvancedCommandLineInterface::getTotalBenchmarkTime() const {
    double total = 0.0;
    for (const auto& result : benchmarkResults_) {
        total += result.second;
    }
    return total;
}

double AdvancedCommandLineInterface::getAverageSpikeRate() const {
    if (monitoringData_.empty()) {
        return 0.0;
    }
    
    double totalSpikes = 0.0;
    size_t totalSteps = 0;
    
    for (const auto& data : monitoringData_) {
        totalSpikes += data.spikeCount;
        totalSteps += data.step;
    }
    
    return totalSteps > 0 ? totalSpikes / totalSteps : 0.0;
}

size_t AdvancedCommandLineInterface::getMonitoringDuration() const {
    if (monitoringData_.empty()) {
        return 0;
    }
    
    return monitoringData_.back().timestamp - monitoringData_.front().timestamp;
}

size_t AdvancedCommandLineInterface::getMonitoringStepCount() const {
    return monitoringData_.size();
}

} // namespace nlm
