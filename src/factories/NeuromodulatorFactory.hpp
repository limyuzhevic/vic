#pragma once

#include "abstraction/AbstractComponents.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class NeuromodulatorFactory {
public:
    // Factory type enum for configuration
    enum class NeuromodulatorType {
        Dopamine,
        Curiosity,
        Novelty,
        PredictionError,
        Custom
    };
    
    NeuromodulatorFactory();
    ~NeuromodulatorFactory();
    
    // Create neuromodulator based on type
    std::unique_ptr<AbstractNeuromodulator> createNeuromodulator(
        NeuromodulatorType type,
        std::shared_ptr<Config> config = nullptr
    );
    
    std::unique_ptr<AbstractNeuromodulator> createNeuromodulator(
        const std::string& neuromodulatorName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Create specific neuromodulator types
    std::unique_ptr<Dopamine> createDopamine(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto dopamine = std::make_unique<Dopamine>();
        if (config) {
            configureFromConfig(dopamine.get(), config);
        }
        return dopamine;
    }
    
    std::unique_ptr<Curiosity> createCuriosity(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto curiosity = std::make_unique<Curiosity>();
        if (config) {
            configureFromConfig(curiosity.get(), config);
        }
        return curiosity;
    }
    
    std::unique_ptr<Novelty> createNovelty(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto novelty = std::make_unique<Novelty>();
        if (config) {
            configureFromConfig(novelty.get(), config);
        }
        return novelty;
    }
    
    std::unique_ptr<PredictionError> createPredictionError(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto predictionError = std::make_unique<PredictionError>();
        if (config) {
            configureFromConfig(predictionError.get(), config);
        }
        return predictionError;
    }
    
    // Configuration-based factory creation
    std::unique_ptr<AbstractNeuromodulator> createFromConfig(
        const Config& config
    );
    
    // Plugin-based factory creation
    std::unique_ptr<AbstractNeuromodulator> createFromPlugin(
        const std::string& pluginName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // List available neuromodulator types
    static std::vector<NeuromodulatorType> getAvailableTypes() {
        return {
            NeuromodulatorType::Dopamine,
            NeuromodulatorType::Curiosity,
            NeuromodulatorType::Novelty,
            NeuromodulatorType::PredictionError
        };
    }
    
    // Get type name
    static const char* getTypeName(NeuromodulatorType type) {
        switch (type) {
            case NeuromodulatorType::Dopamine: return "Dopamine";
            case NeuromodulatorType::Curiosity: return "Curiosity";
            case NeuromodulatorType::Novelty: return "Novelty";
            case NeuromodulatorType::PredictionError: return "PredictionError";
            default: return "Unknown";
        }
    }
    
    // Configuration helpers
    void configureFromConfig(AbstractNeuromodulator* neuromodulator, const Config& config);
    void configureFromConfig(Dopamine* dopamine, const Config& config);
    void configureFromConfig(Curiosity* curiosity, const Config& config);
    void configureFromConfig(Novelty* novelty, const Config& config);
    void configureFromConfig(PredictionError* predictionError, const Config& config);
    
    // Plugin system
    bool registerPlugin(
        const std::string& name,
        std::function<std::unique_ptr<AbstractNeuromodulator>(std::shared_ptr<Config>)> factory
    );
    std::unique_ptr<AbstractNeuromodulator> createFromRegisteredPlugin(
        const std::string& name,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Get registered plugin names
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Statistics and monitoring
    size_t getCreationCount() const { return creationCount_; }
    size_t getPluginCount() const { return plugins_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    mutable size_t creationCount_;
    std::unordered_map<std::string, std::function<std::unique_ptr<AbstractNeuromodulator>(std::shared_ptr<Config>)>> plugins_;
};

} // namespace nlm
