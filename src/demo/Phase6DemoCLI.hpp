// Phase6DemoCLI.hpp - Complete CLI interface header for Phase 6 demo
#pragma once

/**
 * Phase6DemoCLI - Complete CLI interface for Phase 6 demo executable
 * 
 * This class provides a comprehensive command-line interface for the
 * Phase 6 integration demo, including configuration management,
 * output formatting, and error handling.
 */

#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Output format enumeration for Phase 6 demo
 */
enum class OutputFormat {
    TEXT = 0,
    JSON = 1,
    CSV = 2,
    XML = 3
};

/**
 * CLI options structure for Phase 6 demo
 */
struct Phase6DemoOptions {
    std::string configFile;
    std::string outputFile;
    OutputFormat outputFormat;
    bool verbose;
    bool quiet;
    uint64_t neuronCount;
    uint64_t maxSteps;
    uint64_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
};

/**
 * Phase6DemoCLI - Complete CLI interface for Phase 6 demo
 * 
 * This class provides a comprehensive command-line interface for the
 * Phase 6 integration demo, including configuration management,
 * output formatting, and error handling.
 */
class Phase6DemoCLI {
public:
    Phase6DemoOptions parseArguments(int argc, char* argv[]);
    bool runPhase6Demo(const Phase6DemoOptions& options);
    static void printHelp();
    
private:
    OutputFormat outputFormat_;
    
    bool loadConfigFromFile(Phase6DemoOptions& options);
    std::string formatResults(const Phase6IntegrationResult& result);
    std::string formatResultsAsText(const Phase6IntegrationResult& result);
    std::string formatResultsAsJson(const Phase6IntegrationResult& result);
    std::string formatResultsAsCsv(const Phase6IntegrationResult& result);
    std::string formatResultsAsXml(const Phase6IntegrationResult& result);
    void printSummary(const Phase6IntegrationResult& result);
};

} // namespace nlm
