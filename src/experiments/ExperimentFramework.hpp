#pragma once

#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <vector>
#include <map>
#include <any>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Experiment result base class
class ExperimentResult {
public:
    virtual ~ExperimentResult() = default;
    virtual std::string getSummary() const = 0;
    virtual std::string getDetails() const = 0;
};

// Configuration for experiments
class ExperimentConfig {
public:
    std::string name;
    double duration = 0.0;
    std::map<std::string, std::any> parameters;
    std::map<std::string, std::string> options;
    
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    std::string getOption(const std::string& key, const std::string& defaultValue = "") const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
};

// Base experiment class
class Experiment {
public:
    virtual ~Experiment() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::shared_ptr<ExperimentResult> run(const ExperimentConfig& config) = 0;
    virtual bool validateConfig(const ExperimentConfig& config) const = 0;
    virtual std::vector<std::string> getAvailableParameters() const { return {}; }
    virtual std::vector<std::string> getAvailableOptions() const { return {}; }
};

// Simple experiment result for basic experiments
class SimpleExperimentResult : public ExperimentResult {
public:
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> info;
    double executionTime = 0.0;
    bool success = true;
    std::string errorMessage;
    
    std::string getSummary() const override {
        std::ostringstream oss;
        oss << "Experiment: Success" << (success ? " ✓" : " ✗");
        oss << "\nExecution time: " << executionTime << "s\n";
        for (const auto& [key, value] : metrics) {
            oss << "  " << key << ": " << value << "\n";
        }
        return oss.str();
    }
    
    std::string getDetails() const override {
        std::ostringstream oss;
        oss << getSummary();
        for (const auto& [key, value] : info) {
            oss << "  " << key << ": " << value << "\n";
        }
        if (!errorMessage.empty()) {
            oss << "Error: " << errorMessage << "\n";
        }
        return oss.str();
    }
};

// Experiment factory for creating experiments from configuration
class ExperimentFactory {
public:
    using ExperimentCreator = std::function<std::unique_ptr<Experiment>()>;
    
    static ExperimentFactory& getInstance() {
        static ExperimentFactory instance;
        return instance;
    }
    
    void registerExperiment(const std::string& name, const std::string& description,
                           ExperimentCreator creator) {
        experimentRegistry_[name] = {description, std::move(creator)};
    }
    
    std::unique_ptr<Experiment> createExperiment(const std::string& name) {
        auto it = experimentRegistry_.find(name);
        if (it != experimentRegistry_.end()) {
            return it->second.creator();
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailableExperiments() const {
        std::vector<std::string> names;
        for (const auto& pair : experimentRegistry_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::map<std::string, std::string> getExperimentInfo() const {
        return experimentRegistry_;
    }
    
private:
    ExperimentFactory() = default;
    
    struct ExperimentInfo {
        std::string description;
        ExperimentCreator creator;
    };
    
    std::map<std::string, ExperimentInfo> experimentRegistry_;
};

// Experiment runner that manages experiment execution
class ExperimentRunner {
public:
    static ExperimentRunner& getInstance() {
        static ExperimentRunner instance;
        return instance;
    }
    
    std::shared_ptr<ExperimentResult> runExperiment(const std::string& experimentName,
                                                   const ExperimentConfig& config) {
        auto experiment = ExperimentFactory::getInstance().createExperiment(experimentName);
        if (!experiment) {
            auto result = std::make_shared<SimpleExperimentResult>();
            result->success = false;
            result->errorMessage = "Experiment not found: " + experimentName;
            return result;
        }
        
        if (!experiment->validateConfig(config)) {
            auto result = std::make_shared<SimpleExperimentResult>();
            result->success = false;
            result->errorMessage = "Invalid configuration for experiment: " + experimentName;
            return result;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto result = experiment->run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        if (result) {
            result->executionTime = std::chrono::duration<double>(endTime - startTime).count();
        }
        
        return result;
    }
    
    std::vector<std::string> getAvailableExperiments() const {
        return ExperimentFactory::getInstance().getAvailableExperiments();
    }
};

// Configuration manager for experiments
class ExperimentConfigManager {
public:
    static ExperimentConfigManager& getInstance() {
        static ExperimentConfigManager instance;
        return instance;
    }
    
    bool loadConfig(const std::string& configFile, ExperimentConfig& config) {
        auto nlmConfig = std::make_shared<Config>();
        if (!nlmConfig->loadFromFile(configFile)) {
            return false;
        }
        
        // Convert NLM config to experiment config
        for (const auto& key : nlmConfig->getKeys()) {
            if (auto value = nlmConfig->get<std::string>(key)) {
                config.parameters[key] = *value;
            } else if (auto value = nlmConfig->get<int>(key)) {
                config.parameters[key] = *value;
            } else if (auto value = nlmConfig->get<double>(key)) {
                config.parameters[key] = *value;
            } else if (auto value = nlmConfig->get<bool>(key)) {
                config.parameters[key] = *value;
            }
        }
        
        return true;
    }
    
    bool saveConfig(const std::string& configFile, const ExperimentConfig& config) {
        auto nlmConfig = std::make_shared<Config>();
        
        for (const auto& [key, value] : config.parameters) {
            std::visit([&nlmConfig, &key](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    nlmConfig->set(key, arg, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, int>) {
                    nlmConfig->set(key, arg, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, double>) {
                    nlmConfig->set(key, arg, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, bool>) {
                    nlmConfig->set(key, arg, ConfigSource::File);
                }
            }, value);
        }
        
        return nlmConfig->saveToFile(configFile);
    }
};

} // namespace nlm
