// Configuration Helper Functions Header
// Improved configuration loading and validation

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool loadConfiguration(std::shared_ptr<Config> config, const std::string& configFile);
bool validateBrainConfiguration(const std::shared_ptr<Config>& config);
void parseCommandLineArguments(int argc, char** argv, std::string& configFile);
void setupDefaultConfiguration(std::shared_ptr<Config> config);

// Advanced configuration methods
void enableAdvancedFeatures(std::shared_ptr<Config> config);
void setPhase2Defaults(std::shared_ptr<Config> config);
void setPhase3Defaults(std::shared_ptr<Config> config);
void setPhase4Defaults(std::shared_ptr<Config> config);
void setPhase5Defaults(std::shared_ptr<Config> config);
void setPhase6Defaults(std::shared_ptr<Config> config);

// Command-line argument parsing helpers
struct CommandLineOptions {
    std::string configFile;
    std::string logLevel;
    bool verbose;
    bool quiet;
    int maxSteps;
    double simulationTimestep;
    std::string experimentType;
    bool enableCuriosity;
    bool enableRewardModulation;
    bool enableDevelopment;
    bool enableStructuralPlasticity;
    int randomSeed;
    std::string outputFile;
};

CommandLineOptions parseCommandLineOptions(int argc, char** argv);
void applyCommandLineOptions(std::shared_ptr<Config> config, const CommandLineOptions& options);

// Configuration validation helpers
bool validateConfigurationRange(const std::shared_ptr<Config>& config, const std::string& key, 
                               double min, double max, const std::string& description = "");
bool validateConfigurationInteger(const std::shared_ptr<Config>& config, const std::string& key,
                                 int64_t min, int64_t max, const std::string& description = "");
bool validateAllConfiguration(const std::shared_ptr<Config>& config);

// Configuration backup and restore
bool saveConfigurationBackup(const std::string& filepath);
bool restoreConfigurationBackup(const std::string& filepath);

// Configuration comparison
bool compareConfigurations(const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2);
std::string configurationDiff(const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2);

} // namespace nlm

