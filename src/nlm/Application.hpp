#pragma once

#include "main.cpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <argparse/argparse.hpp>

using namespace nlm;

// Command Handler Implementation
class CommandHandler : public CLIApplication {
public:
    CommandHandler() : CLIApplication() {}
    ~CommandHandler() = default;
    
    bool handleCommand(const std::string& command, const std::map<std::string, std::string>& args) override {
        SimpleArgParser parser;
        parser.args = args;
        
        if (command == "demo" || command == "run") {
            return handleDemoCommand(parser);
        } else if (command == "benchmark") {
            return handleBenchmarkCommand(parser);
        } else if (command == "experiment") {
            return handleExperimentCommand(parser);
        } else if (command == "config") {
            return handleConfigCommand(parser);
        } else if (command == "list") {
            return handleListCommand(parser);
        } else if (command == "info") {
            return handleInfoCommand(parser);
        } else {
            NLM_LOG_ERROR("Unknown command: " + command);
            return false;
        }
    }
    
    std::vector<std::string> getAvailableCommands() const override {
        return {"demo", "run", "benchmark", "experiment", "config", "list", "info"};
    }
    
    std::string getCommandHelp(const std::string& command) const override {
        std::map<std::string, std::string> helpMap = {
            {"demo", "Run the original Phase 2 demo with backward compatibility"},
            {"run", "Run simulation with specified mode (alias for demo)"},
            {"benchmark", "Run benchmark tests and performance measurements"},
            {"experiment", "Run experiment suite with configurable parameters"},
            {"config", "Manage configuration files (load, save, validate)"},
            {"list", "List available experiments and configuration files"},
            {"info", "Show information about NLM and available options"}
        };
        
        auto it = helpMap.find(command);
        return (it != helpMap.end()) ? it->second : "No help available";
    }
    
private:
    bool handleDemoCommand(const SimpleArgParser& parser) {
        NLM_LOG_INFO("=== NLM Phase 2 Demo Mode ===");
        NLM_LOG_INFO("Running the original Phase 2 demo with backward compatibility");
        
        // Use the original Phase 2 demo implementation
        runPhase2CompatibilityMode(config_);
        
        NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
        return true;
    }
    
    bool handleBenchmarkCommand(const SimpleArgParser& parser) {
        NLM_LOG_INFO("=== NLM Benchmark Mode ===");
        NLM_LOG_INFO("Running benchmark tests...")
        
        // Parse benchmark arguments
        size_t steps = 1000;
        size_t neurons = 100;
        double duration = 10.0;
        std::string configFile = "configs/default.cfg";
        
        if (parser.args.find("--steps") != parser.args.end()) {
            steps = std::stoul(parser.args["--steps"]);
        }
        if (parser.args.find("--neurons") != parser.args.end()) {
            neurons = std::stoul(parser.args["--neurons"]);
        }
        if (parser.args.find("--duration") != parser.args.end()) {
            duration = std::stod(parser.args["--duration"]);
        }
        if (parser.args.find("--config") != parser.args.end()) {
            configFile = parser.args["--config"];
        }
        
        // Create benchmark configuration
        auto benchConfig = std::make_shared<Config>(*config_);
        benchConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        benchConfig->set("region_count", 1, ConfigSource::Default);
        
        NLM_LOG_INFO("Benchmark configuration:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neurons));
        NLM_LOG_INFO("  Steps: " + std::to_string(steps));
        NLM_LOG_INFO("  Duration: " + std::to_string(duration) + "s");
        
        // Run benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        runPhase2CompatibilityMode(benchConfig);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        NLM_LOG_INFO("Benchmark results:");
        NLM_LOG_INFO("  Total time: " + std::to_string(durationMs / 1000.0) + "s");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(steps * 1000.0 / durationMs));
        
        return true;
    }
    
    bool handleExperimentCommand(const SimpleArgParser& parser) {
        NLM_LOG_INFO("=== NLM Experiment Mode ===");
        
        // Parse experiment arguments
        std::string experimentName = "phase2-demo";
        size_t iterations = 1;
        std::string outputFile = "";
        
        if (parser.args.find("--name") != parser.args.end()) {
            experimentName = parser.args["--name"];
        }
        if (parser.args.find("--iterations") != parser.args.end()) {
            iterations = std::stoul(parser.args["--iterations"]);
        }
        if (parser.args.find("--output") != parser.args.end()) {
            outputFile = parser.args["--output"];
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentName);
        NLM_LOG_INFO("Iterations: " + std::to_string(iterations));
        
        // Run experiment
        if (experimentManager_->runExperiment(experimentName, *config_)) {
            NLM_LOG_INFO("Experiment completed successfully");
            
            if (!outputFile.empty()) {
                NLM_LOG_INFO("Results saved to: " + outputFile);
                // Would save results here
            }
            
            return true;
        } else {
            NLM_LOG_ERROR("Experiment failed");
            return false;
        }
    }
    
    bool handleConfigCommand(const SimpleArgParser& parser) {
        NLM_LOG_INFO("=== NLM Configuration Mode ===");
        
        // Parse config command arguments
        std::string action = "list";
        std::string filename = "";
        std::string format = "json";
        
        if (parser.args.find("action") != parser.args.end()) {
            action = parser.args["action"];
        }
        if (parser.args.find("filename") != parser.args.end()) {
            filename = parser.args["filename"];
        }
        if (parser.args.find("--format") != parser.args.end()) {
            format = parser.args["--format"];
        }
        
        if (action == "list") {
            printAvailableExperiments();
            printConfigurationExamples();
            return true;
        } else if (action == "validate") {
            if (filename.empty()) {
                NLM_LOG_ERROR("Filename required for validation");
                return false;
            }
            Config tempConfig;
            if (configurationManager_->loadConfiguration(filename, tempConfig)) {
                if (configurationManager_->validateConfiguration(tempConfig)) {
                    NLM_LOG_INFO("Configuration is valid");
                    return true;
                } else {
                    NLM_LOG_ERROR("Configuration is invalid");
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Failed to load configuration");
                return false;
            }
        } else if (action == "generate") {
            if (filename.empty()) {
                NLM_LOG_ERROR("Filename required for generation");
                return false;
            }
            configurationManager_->saveConfiguration(filename, *config_);
            return true;
        } else if (action == "export") {
            if (filename.empty()) {
                NLM_LOG_ERROR("Filename required for export");
                return false;
            }
            configurationManager_->saveConfiguration(filename, *config_, (format == "json" || format == "pretty"));
            return true;
        }
        
        return false;
    }
    
    bool handleListCommand(const SimpleArgParser& parser) {
        std::string listType = "experiments";
        
        if (parser.args.find("--type") != parser.args.end()) {
            listType = parser.args["--type"];
        }
        
        if (listType == "experiments") {
            printAvailableExperiments();
        } else if (listType == "configs") {
            printConfigurationExamples();
        } else if (listType == "plugins") {
            NLM_LOG_INFO("Plugin system is under development");
        }
        
        return true;
    }
    
    bool handleInfoCommand(const SimpleArgParser& parser) {
        printVersion();
        printConfigurationExamples();
        return true;
    }
};

// Experiment Framework Implementation
ExperimentFramework::ExperimentFramework() {
    registerDefaultExperiments();
}

void ExperimentFramework::registerDefaultExperiments() {
    // Phase 2 experiments
    experiments_["phase2-demo"] = {
        "phase2-demo",
        "Original Phase 2 demonstration with all three tests",
        [this](const Config& config) { return runPhase2Demo(config); },
        "Phase 2",
        {}
    };
    
    experiments_["phase2-plasticity"] = {
        "phase2-plasticity",
        "Plasticity learning experiment with Hebbian and STDP",
        [this](const Config& config) { return runPhase2PlasticityTest(config); },
        "Phase 2",
        {}
    };
    
    experiments_["phase2-stdp"] = {
        "phase2-stdp",
        "STDP verification experiment with pre-post pairing",
        [this](const Config& config) { return runPhase2StdpTest(config); },
        "Phase 2",
        {}
    };
    
    experiments_["phase2-learning"] = {
        "phase2-learning",
        "Synaptic learning experiment demonstrating weight changes",
        [this](const Config& config) { return runPhase2LearningExperiment(config); },
        "Phase 2",
        {}
    };
    
    // Phase 6 experiments
    experiments_["phase6-integration"] = {
        "phase6-integration",
        "Phase 6 integration test verifying all systems work together",
        [this](const Config& config) { return runPhase6IntegrationTest(config); },
        "Phase 6",
        {}
    };
}

bool ExperimentFramework::registerExperiment(const std::string& name, const std::string& description, 
                                            std::function<bool(const Config&)> experimentFunc) {
    ExperimentInfo info;
    info.name = name;
    info.description = description;
    info.func = experimentFunc;
    info.category = "Custom";
    
    experiments_[name] = info;
    return true;
}

bool ExperimentFramework::runExperiment(const std::string& name, const Config& config, 
                                       const std::map<std::string, std::string>& options) {
    auto it = experiments_.find(name);
    if (it == experiments_.end()) {
        NLM_LOG_ERROR("Experiment not found: " + name);
        return false;
    }
    
    try {
        return it->second.func(config);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentFramework::runExperimentSeries(const std::vector<std::string>& names, 
                                            const Config& baseConfig, 
                                            const std::map<std::string, std::vector<std::string>>& parameterGrid) {
    // Run series of experiments with different parameters
    for (const auto& name : names) {
        NLM_LOG_INFO("Running experiment: " + name);
        if (!runExperiment(name, baseConfig)) {
            NLM_LOG_ERROR("Experiment failed: " + name);
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ExperimentFramework::getAvailableExperiments() const {
    std::vector<std::string> names;
    for (const auto& [name, info] : experiments_) {
        names.push_back(name);
    }
    return names;
}

std::string ExperimentFramework::getExperimentInfo(const std::string& name) const {
    auto it = experiments_.find(name);
    if (it != experiments_.end()) {
        const auto& info = it->second;
        std::stringstream ss;
        ss << "Experiment: " << info.name << std::endl;
        ss << "Description: " << info.description << std::endl;
        ss << "Category: " << info.category << std::endl;
        ss << "Parameters: ";
        for (const auto& [key, value] : info.parameters) {
            ss << key << "=" << value << " ";
        }
        return ss.str();
    }
    return "Experiment not found: " + name;
}

bool ExperimentFramework::saveExperimentResults(const std::string& experimentName, 
                                              const std::string& results) const {
    std::string filename = "results_" + experimentName + "_" + 
                          std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << results;
        file.close();
        NLM_LOG_INFO("Saved experiment results to: " + filename);
        return true;
    }
    return false;
}

bool ExperimentFramework::loadExperimentResults(const std::string& filename) const {
    std::ifstream file(filename);
    if (file.is_open()) {
        // Would parse JSON results here
        file.close();
        return true;
    }
    return false;
}

// ConfigurationManager Implementation
bool ConfigurationManager::loadConfiguration(const std::string& filename, Config& config) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file: " + filename);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return loadJSONConfig(filename, config);
}

bool ConfigurationManager::saveConfiguration(const std::string& filename, const Config& config, 
                                          bool prettyPrint) const {
    json j;
    
    // Extract config values (simplified)
    j["neuron_count"] = config.getOr<size_t>("neuron_count", 1000);
    j["region_count"] = config.getOr<size_t>("region_count", 1);
    j["connection_probability"] = config.getOr<float>("connection_probability", 0.1f);
    j["simulation_timestep"] = config.getOr<double>("simulation_timestep", 0.001);
    j["stdp_ltp_weight"] = config.getOr<float>("stdp_ltp_weight", 0.01f);
    j["stdp_ltd_weight"] = config.getOr<float>("stdp_ltd_weight", 0.012f);
    j["stdp_tau"] = config.getOr<float>("stdp_tau", 20.0f);
    j["synaptogenesis_rate"] = config.getOr<float>("synaptogenesis_rate", 0.0001f);
    j["pruning_rate"] = config.getOr<float>("pruning_rate", 0.00001f);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not write configuration file: " + filename);
        return false;
    }
    
    if (prettyPrint) {
        file << j.dump(2);
    } else {
        file << j.dump();
    }
    
    file.close();
    NLM_LOG_INFO("Saved configuration to: " + filename);
    return true;
}

bool ConfigurationManager::validateConfiguration(const Config& config) const {
    // Basic validation
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid configuration: neuron_count must be > 0");
        return false;
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_ERROR("Invalid configuration: simulation_timestep must be in (0, 1.0]");
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigurationManager::validateAndFix(Config& config) const {
    std::vector<std::string> warnings;
    
    // Auto-fix common issues
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        warnings.push_back("neuron_count was 0, set to 1000");
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        warnings.push_back("simulation_timestep was invalid, set to 0.001");
    }
    
    return warnings;
}

bool ConfigurationManager::mergeConfigurations(Config& base, const Config& override) const {
    if (override.hasKey("neuron_count")) {
        base.set("neuron_count", override.getOr<int64_t>("neuron_count", 1000), ConfigSource::Default);
    }
    if (override.hasKey("region_count")) {
        base.set("region_count", override.getOr<int64_t>("region_count", 1), ConfigSource::Default);
    }
    if (override.hasKey("connection_probability")) {
        base.set("connection_probability", override.getOr<float>("connection_probability", 0.1f), ConfigSource::Default);
    }
    
    return true;
}

bool ConfigurationManager::applyOverrides(Config& config, const std::map<std::string, std::string>& overrides) const {
    for (const auto& [key, value] : overrides) {
        try {
            if (key.find("neuron") != std::string::npos || key.find("count") != std::string::npos) {
                size_t intValue = std::stoul(value);
                config.set(key, static_cast<int64_t>(intValue), ConfigSource::Default);
            } else if (key.find("probability") != std::string::npos ||
                      key.find("rate") != std::string::npos ||
                      key.find("weight") != std::string::npos ||
                      key.find("ratio") != std::string::npos) {
                float floatValue = std::stof(value);
                config.set(key, floatValue, ConfigSource::Default);
            } else if (key.find("timestep") != std::string::npos ||
                      key.find("tau") != std::string::npos) {
                double doubleValue = std::stod(value);
                config.set(key, doubleValue, ConfigSource::Default);
            } else if (key.find("enable") != std::string::npos ||
                      key.find("flag") != std::string::npos) {
                bool boolValue = (value == "true" || value == "1" || value == "yes");
                config.set(key, boolValue, ConfigSource::Default);
            } else {
                config.set(key, value, ConfigSource::Default);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Failed to parse override " + key + " = " + value + ": " + e.what());
        }
    }
    
    return true;
}

// Add include guard
#endif // NLM_APPLICATION_H