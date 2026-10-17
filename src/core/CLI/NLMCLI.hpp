#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

class NLMCLI {
public:
    struct CLIConfig {
        std::string configFile;
        std::string outputFile;
        std::string checkpointFile;
        std::string logLevel;
        bool verbose;
        bool quiet;
        int maxSteps;
        int numSteps;
        double timestep;
        std::string mode;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        bool enableCuriosity;
        bool enableRewardModulation;
        std::vector<std::string> customArgs;
        std::string helpText;
    };

    NLMCLI();
    ~NLMCLI() = default;

    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);

    // Parse configuration from file
    bool parseConfigFile(const std::string& filename);

    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char* argv[]);

    // Execute the CLI with the parsed configuration
    int execute();

    // Run demo mode
    int runDemoMode() const;

    // Run simulation mode
    int runSimulationMode() const;

    // Run training mode
    int runTrainingMode() const;

    // Run monitoring mode
    int runMonitoringMode() const;

    // Run benchmark mode
    int runBenchmarkMode() const;

    // Export brain state
    int exportBrainState() const;

    // Help and version information
    void printHelp() const;
    void printVersion() const;
    void printUsage() const;

    // Configuration access
    const CLIConfig& getConfig() const { return config_; }
    std::shared_ptr<Config> getBrainConfig() const { return brainConfig_; }

protected:
    CLIConfig config_;
    std::shared_ptr<Config> brainConfig_;
    std::shared_ptr<Brain> brain_;

private:
    // Parse argument value based on type
    template<typename T>
    bool parseArgValue(const std::string& arg, T& value) const;

    // Check if argument exists
    bool hasArg(const std::string& arg) const;

    // Get argument value
    std::string getArgValue(const std::string& arg) const;

    // Get boolean argument
    bool getBoolArg(const std::string& arg, bool defaultValue = false) const;

    // Get integer argument
    int getIntArg(const std::string& arg, int defaultValue = 0) const;

    // Get float argument
    float getFloatArg(const std::string& arg, float defaultValue = 0.0f) const;

    // Initialize brain with configuration
    bool initializeBrain() const;

    // Log message with appropriate level
    void logMessage(const std::string& message, const std::string& level = "INFO") const;

    // Validate configuration
    bool validateConfig() const;

    // Save configuration to file
    bool saveConfigToFile(const std::string& filename) const;

    // Load configuration from file
    bool loadConfigFromFile(const std::string& filename);
};

} // namespace nlm

#endif // NLM_CLI_HPP
