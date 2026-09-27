// ==============================================================================
// Configuration Wizard
// ==============================================================================
#ifndef CONFIGURATIONWIZARD_HPP
#define CONFIGURATIONWIZARD_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../config/OptimizedConfig.hpp"

namespace nlm {
namespace debug {

/**
 * @class ConfigurationWizard
 * @brief Guided configuration setup for NLM
 * 
 * Provides interactive configuration wizard for setting up neural network systems
 * with validation, optimization recommendations, and debugging configuration presets.
 */
class ConfigurationWizard {
public:
    ConfigurationWizard();
    ~ConfigurationWizard();

    /**
     * @brief Start configuration wizard
     * @param config Configuration to modify
     * @return true if configuration was successfully created
     */
    bool startWizard(Config& config);

    /**
     * @brief Get recommended configuration for given use case
     * @param useCase Use case type
     * @param config Reference to configuration to fill
     * @return true if recommendation was generated
     */
    bool getRecommendation(const std::string& useCase, Config& config);

    /**
     * @brief Validate configuration for errors
     * @param config Configuration to validate
     * @return Vector of validation errors
     */
    std::vector<std::string> validateConfiguration(const Config& config);

    /**
     * @brief Get optimization recommendations for configuration
     * @param config Configuration to analyze
     * @return Vector of optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations(const Config& config);

    /**
     * @brief Apply debugging configuration preset
     * @param presetName Preset name
     * @param config Configuration to modify
     * @return true if preset was applied
     */
    bool applyDebugPreset(const std::string& presetName, Config& config);

    /**
     * @brief Get available configuration presets
     * @return Vector of preset names
     */
    std::vector<std::string> getAvailablePresets() const;

    /**
     * @brief Export configuration to file
     * @param config Configuration to export
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportConfiguration(const Config& config, const std::string& filename);

    /**
     * @brief Import configuration from file
     * @param config Configuration to populate
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importConfiguration(Config& config, const std::string& filename);

    /**
     * @brief Get configuration history
     * @return Vector of configuration snapshots
     */
    std::vector<Config> getConfigurationHistory() const;

    /**
     * @brief Restore configuration from history
     * @param index Index in history
     * @param config Configuration to restore
     * @return true if restoration was successful
     */
    bool restoreConfiguration(size_t index, Config& config);

protected:
    /**
     * @brief Get default configuration for system
     * @return Default configuration
     */
    Config getDefaultConfiguration() const;

    /**
     * @brief Get high-performance configuration
     * @return High-performance configuration
     */
    Config getHighPerformanceConfiguration() const;

    /**
     * @brief Get memory-efficient configuration
     * @return Memory-efficient configuration
     */
    Config getMemoryEfficientConfiguration() const;

    /**
     * @brief Get debug-friendly configuration
     * @return Debug-friendly configuration
     */
    Config getDebugFriendlyConfiguration() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // CONFIGURATIONWIZARD_HPP
