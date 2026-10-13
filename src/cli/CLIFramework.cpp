// NLM CLI Framework Implementation
// Provides comprehensive command-line interface support for all NLM executables
// Includes argument parsing, help/usage generation, and validation

#include "nlm/cli/CLIFramework.hpp"

namespace nlm {
namespace cli {

CLIOptions ArgumentParser::convertParsedArgsToOptions(const std::map<std::string, std::string>& parsedArgs) {
    CLIOptions options;
    
    for (const auto& pair : parsedArgs) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        if (key == "--help") options.helpRequested = true;
        else if (key == "--version") options.versionRequested = true;
        else if (key == "--config") options.configFile = value;
        else if (key == "--log-level") options.logLevel = value;
        else if (key == "--output") options.outputFile = value;
        else if (key == "--quiet") options.quiet = true;
        else if (key == "--verbose") options.verbose = true;
        else if (key == "--benchmark") options.benchmarkMode = true;
        else if (key == "--benchmark-output") options.benchmarkOutput = value;
        else if (key == "--benchmark-duration") {
            try {
                options.benchmarkDuration = std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing benchmark duration: " << e.what() << std::endl;
            }
        }
        // Performance monitoring options
        else if (key == "--enable-memory-monitoring") options.enableMemoryMonitoring = true;
        else if (key == "--enable-fp-monitoring") options.enableFPMonitoring = true;
        else if (key == "--enable-network-monitoring") options.enableNetworkMonitoring = true;
        else if (key == "--performance-output") options.performanceOutputFile = value;
        else if (key == "--detailed-profiling") options.detailedProfiling = true;
        else if (key == "--profiling-sample-rate") {
            try {
                options.profilingSampleRate = std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing profiling sample rate: " << e.what() << std::endl;
            }
        }
        else if (key == "--profiling-output") options.profilingOutput = value;
        else if (key == "--enable-real-time-stats") options.enableRealTimeStats = true;
        else if (key == "--stats-interval") {
            try {
                options.statsInterval = std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing stats interval: " << e.what() << std::endl;
            }
        }
        // Add more mappings as needed for additional arguments
    }
    
    return options;
}

} // namespace cli
} // namespace nlm
