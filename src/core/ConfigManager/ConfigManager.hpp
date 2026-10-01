// Configuration Manager - Centralized config setup and command line handling

#ifndef NLM_CONFIG_MANAGER_HPP
#define NLM_CONFIG_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class ConfigManager {
private:
    std::shared_ptr<Config> config;
    std::map<std::string, std::string> cliOverrides;
    
public:
    ConfigManager() : config(std::make_shared<Config>()) {}
    
    void parseCommandLine(int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        for (size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            
            if (arg.find("--config") == 0) {
                std::string configFile;
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < args.size()) {
                    configFile = args[++i];
                }
                if (!configFile.empty()) {
                    loadFromFile(configFile);
                }
            } else if (arg.find("--") != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=', 2) - 2);
                std::string value = "";
                size_t pos = arg.find('=', 2);
                if (pos != std::string::npos) {
                    value = arg.substr(pos + 1);
                }
                if (!key.empty() && !value.empty()) {
                    cliOverrides[key] = value;
                    applyConfigOverride(key, value);
                }
            }
        }
    }
    
    void loadFromFile(const std::string& filename) {
        if (config->loadFromFile(filename)) {
            NLM_LOG_INFO("Loaded configuration from: " + filename);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + filename + ")");
        }
    }
    
    void applyPhase2Defaults() {
        // Apply default values for Phase 2 neural computation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    void logConfigurationSummary() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("");
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
};

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_HPP