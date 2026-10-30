#pragma once

/**
 * Base class for demo systems
 * 
 * This provides common functionality for all demo systems:
 * - Command-line argument parsing
 * - Configuration validation
 * - Logging setup
 * - Progress reporting
 * - Results output
 * - Help system
 */

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>

namespace nlm {

class DemoBase {
public:
    struct DemoConfig {
        // Common configuration options
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        std::string logLevel = "info";
        std::string outputFile;
        bool exportResults = false;
        std::string benchmarkMode;  // "quick", "thorough", "custom"
        double benchmarkTimeLimit = 60.0;  // seconds
    };
    
    struct DemoResults {
        double totalTime = 0.0;
        std::vector<std::string> messages;
        std::vector<std::pair<std::string, double>> metrics;
        bool success = true;
        std::string errorMessage;
    };
    
    DemoBase() {}
    virtual ~DemoBase() {}
    
    /**
     * Parse command line arguments and setup configuration
     */
    DemoConfig parseArgs(int argc, char* argv[]) {
        DemoConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            } else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
                config.logLevel = "debug";
            } else if (arg == "--quiet" || arg == "-q") {
                config.quiet = true;
                config.logLevel = "error";
            } else if (arg == "--benchmark") {
                if (i + 1 < argc) {
                    config.benchmarkMode = argv[++i];
                }
            } else if (arg == "--benchmark-time") {
                if (i + 1 < argc) {
                    config.benchmarkTimeLimit = std::stod(argv[++i]);
                }
            } else if (arg == "--output") {
                if (i + 1 < argc) {
                    config.outputFile = argv[++i];
                }
            } else if (arg == "--export-results") {
                config.exportResults = true;
            } else if (arg == "--version") {
                printVersion();
                exit(0);
            } else {
                std::cerr << "Unknown option: " << arg << "\n";
                printHelp();
                exit(1);
            }
        }
        
        return config;
    }
    
    /**
     * Print help message
     */
    virtual void printHelp() {
        std::cout << "=== NLM Demo System ===\n\n";
        std::cout << "Common options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --verbose, -v           Enable verbose output\n";
        std::cout << "  --quiet, -q             Enable quiet output (errors only)\n";
        std::cout << "  --output FILE           Write output to file\n";
        std::cout << "  --export-results        Export results to files\n";
        std::cout << "  --version               Show version information\n";
        std::cout << "\n";
    }
    
    /**
     * Print version information
     */
    virtual void printVersion() {
        std::cout << "NLM Demo System v0.1.0\n";
        std::cout << "Built with C++20\n";
    }
    
    /**
     * Run the demo with configuration
     */
    virtual DemoResults run(const DemoConfig& config) = 0;
    
    /**
     * Setup logging based on configuration
     */
    void setupLogging(const DemoConfig& config) {
        if (config.quiet) {
            // Setup quiet logging
        } else if (config.verbose) {
            // Setup verbose logging
        } else {
            // Setup default logging
        }
    }
    
    /**
     * Run with timing
     */
    DemoResults runWithTiming(const DemoConfig& config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        DemoResults results;
        
        try {
            results = run(config);
        } catch (const std::exception& e) {
            results.success = false;
            results.errorMessage = e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        results.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return results;
    }
    
    /**
     * Export results to file
     */
    void exportResults(const DemoResults& results, const std::string& filename) {
        if (!results.success) {
            std::cerr << "Cannot export failed results to " << filename << "\n";
            return;
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open output file: " << filename << "\n";
            return;
        }
        
        file << "=== NLM Demo Results ===\n";
        file << "Total Time: " << results.totalTime << "s\n";
        file << "Success: " << (results.success ? "Yes" : "No") << "\n";
        
        if (!results.errorMessage.empty()) {
            file << "Error: " << results.errorMessage << "\n";
        }
        
        file << "\nMessages:\n";
        for (const auto& msg : results.messages) {
            file << "  " << msg << "\n";
        }
        
        file << "\nMetrics:\n";
        for (const auto& metric : results.metrics) {
            file << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        file.close();
    }
    
    /**
     * Print progress indicator
     */
    void printProgress(double current, double total, const std::string& label) {
        int progress = static_cast<int>((current / total) * 100);
        std::cout << "\r[" << std::string(progress / 10, '=') << std::string(10 - progress / 10, ' ') << "] " << progress << "% " << label << " " << current << "/" << total;
        std::cout.flush();
    }
};

} // namespace nlm