// Command line interface header for backward compatibility
// Integrates new CLI system with existing test infrastructure

#ifndef NLM_COMMAND_LINE_HPP
#define NLM_COMMAND_LINE_HPP

#include <memory>
#include <string>
#include "core/Config/Config.hpp"

namespace nlm {

// Legacy test options for backward compatibility
struct LegacyTestOptions {
    bool runAll = false;
    bool runBasic = false;
    bool runPlasticity = false;
    bool runStdp = false;
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "INFO";
    bool verbose = false;
    int maxNeurons = 500;
    double maxTime = 1.0;
    
    LegacyTestOptions() {
        runAll = true;
        runBasic = true;
        runPlasticity = true;
        runStdp = true;
    }
};

// Legacy test execution interface for backward compatibility
class LegacyCommandLineInterface {
public:
    LegacyCommandLineInterface();
    ~LegacyCommandLineInterface();
    
    // Parse legacy command line options
    bool parseLegacyCommandLine(int argc, char** argv, LegacyTestOptions& options);
    
    // Display legacy help information
    void displayLegacyHelp() const;
    
    // Legacy test execution functions
    bool executeLegacyTests(const LegacyTestOptions& options, std::shared_ptr<Brain> brain);
    bool runBasicTest(std::shared_ptr<Brain> brain);
    bool runPlasticityTest(std::shared_ptr<Brain> brain);
    bool runStdpTest(std::shared_ptr<Brain> brain);
    
    // Configuration and brain initialization
    bool initializeApplication(const LegacyTestOptions& options, std::shared_ptr<Config>& config);
    bool initializeBrain(std::shared_ptr<Brain>& brain, std::shared_ptr<Config> config);
    
    // Display functions
    void displayFinalSummary(std::shared_ptr<Brain>& brain);
    void printUsage() const;
    
    // String to log level conversion
    LogLevel stringToLogLevel(const std::string& level);
    
private:
    std::shared_ptr<ConsoleLogger> consoleLogger_;
};

} // namespace nlm

#endif // NLM_COMMAND_LINE_HPP