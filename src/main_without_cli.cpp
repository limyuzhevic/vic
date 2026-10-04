// Enhanced NLM (熙然) - Neural Learning Machine
// Phase 2 & 6: Real Neural Computation with Integration Tests
//
// This enhanced version includes:
// - Comprehensive command-line interface with 12+ flags
// - Phase 6 integration test runner with 6 comprehensive tests
// - Improved configuration system with JSON/YAML support
// - Better user experience with progress bars, colors, and timing
// - Robust error handling and validation
// - Full backward compatibility with Phase 2 tests
//
// ENHANCED CLI & HELP SYSTEM:
// - Comprehensive help documentation with command categorization
// - Enhanced argument validation and error recovery
// - Configuration precedence: CLI > Config file > Defaults
// - Progress indicators and context-sensitive help
// - Interactive help mode with searchable commands
// - Detailed error messages with recovery suggestions

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

// For YAML support (if available)
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <unordered_set>
#include <set>
#include <regex>

using namespace nlm;

// Forward declarations for enhanced CLI
class EnhancedCLI;

// Global error handler flag
volatile bool g_errorExit = false;

// ANSI color codes for terminal output
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
}

// Progress bar implementation
class ProgressBar {
private:
    size_t total;
    size_t current;
    std::string label;
    bool enabled;
    
public:
    ProgressBar(size_t total, const std::string& label = "Progress", bool enabled = true)
        : total(total), current(0), label(label), enabled(enabled) {}
    
    void update(size_t value) {
        if (!enabled) return;
        current = value;
        float progress = (float)current / total;
        int barWidth = 50;
        
        std::cout << "\r" << Color::CYAN << label << ": [" << Color::RESET;
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << Color::GREEN << "=";
            else if (i == pos) std::cout << Color::YELLOW << ">";
            else std::cout << Color::DIM << " ";
        }
        std::cout << Color::CYAN << "] " << Color::RESET;
        std::cout << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        std::cout.flush();
        
        if (current >= total) {
            std::cout << "\r" << Color::GREEN << label << ": [" << Color::RESET;
            for (int i = 0; i < barWidth; ++i) std::cout << Color::GREEN << "=";
            std::cout << Color::CYAN << "] 100.0%" << Color::RESET << std::endl;
        }
    }
    
    void finish() {
        if (!enabled) return;
        update(total);
    }
};

// Signal handler for graceful error handling
void signalHandler(int signal) {
    std::cout << "\n" << Color::YELLOW << "[!] Received signal " << signal << ". Shutting down gracefully..." << Color::RESET << std::endl;
    g_errorExit = true;
}

// Enhanced CLI argument parser
class EnhancedCLI {
private:
    std::map<std::string, std::string> args;
    std::map<std::string, std::string> flags;
    
public:
    EnhancedCLI(int argc, char** argv) {
        parseArgs(argc, argv);
    }
    
    void parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                flags["help"] = "true";
            } else if (arg == "--demo-mode") {
                flags["demo-mode"] = "true";
            } else if (arg == "--phase2") {
                flags["phase2"] = "true";
            } else if (arg == "--phase6") {
                flags["phase6"] = "true";
            } else if (arg == "--verbose") {
                flags["verbose"] = "true";
            } else if (arg == "--quiet") {
                flags["quiet"] = "true";
            } else if (arg == "--config") {
                flags["config"] = "true";
            } else if (arg == "--checkpoint") {
                flags["checkpoint"] = "true";
            } else if (arg == "--save") {
                flags["save"] = "true";
            } else if (arg == "--load") {
                flags["load"] = "true";
            } else if (arg == "--iterations") {
                flags["iterations"] = "true";
            } else if (arg == "--neuron-count") {
                flags["neuron-count"] = "true";
            } else if (arg == "--region-count") {
                flags["region-count"] = "true";
            } else {
                // Check for key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos && pos > 0) {
                    std::string key = arg.substr(0, pos);
                    std::string value = arg.substr(pos + 1);
                    args[key] = value;
                } else if (arg.substr(0, 2) == "--" && i + 1 < argc) {
                    // Check for --key value format
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    args[key] = value;
                }
            }
        }
    }
    
    bool hasFlag(const std::string& flag) const {
        return flags.find(flag) != flags.end();
    }
    
    bool hasArg(const std::string& arg) const {
        return args.find(arg) != args.end();
    }
    
    std::string getArg(const std::string& arg, const std::string& defaultValue = "") const {
        auto it = args.find(arg);
        return (it != args.end()) ? it->second : defaultValue;
    }
    
    int getArgInt(const std::string& arg, int defaultValue = 0) const {
        std::string val = getArg(arg);
        if (val.empty()) return defaultValue;
        try {
            return std::stoi(val);
        } catch (...) {
            return defaultValue;
        }
    }
    
    double getArgDouble(const std::string& arg, double defaultValue = 0.0) const {
        std::string val = getArg(arg);
        if (val.empty()) return defaultValue;
        try {
            return std::stod(val);
        } catch (...) {
            return defaultValue;
        }
    }
    
    float getArgFloat(const std::string& arg, float defaultValue = 0.0f) const {
        std::string val = getArg(arg);
        if (val.empty()) return defaultValue;
        try {
            return std::stof(val);
        } catch (...) {
            return defaultValue;
        }
    }
    
    bool getArgBool(const std::string& arg, bool defaultValue = false) const {
        std::string val = getArg(arg);
        if (val.empty()) return defaultValue;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return (val == "true" || val == "1" || val == "yes" || val == "on");
    }
    
    void printHelp() {
        std::cout << Color::BOLD << Color::CYAN << "\n=== NLM Enhanced Command-Line Interface ===\n" << Color::RESET;
        std::cout << Color::BOLD << "Basic Flags:" << Color::RESET << std::endl;
        std::cout << "  " << Color::GREEN << "-h, --help" << Color::RESET << "             Display this help message" << std::endl;
        std::cout << "  " << Color::GREEN << "--demo-mode" << Color::RESET << "           Run Phase 6 integration demo" << std::endl;
        std::cout << "  " << Color::GREEN << "--phase2" << Color::RESET << "             Run original Phase 2 tests (backward compatibility)" << std::endl;
        std::cout << "  " << Color::GREEN << "--phase6" << Color::RESET << "             Run Phase 6 integration tests" << std::endl;
        std::cout << "  " << Color::GREEN << "--verbose" << Color::RESET << "            Enable verbose logging" << std::endl;
        std::cout << "  " << Color::GREEN << "--quiet" << Color::RESET << "             Suppress most output" << std::endl;
        std::cout << "  " << Color::GREEN << "--iterations N" << Color::RESET << "        Number of simulation steps (default: 1000)" << std::endl;
        std::cout << Color::BOLD << "\nConfiguration Flags:" << Color::RESET << std::endl;
        std::cout << "  " << Color::GREEN << "--config file" << Color::RESET << "         Configuration file path (JSON/YAML)" << std::endl;
        std::cout << "  " << Color::GREEN << "--neuron-count N" << Color::RESET << "      Override neuron count" << std::endl;
        std::cout << "  " << Color::GREEN << "--region-count N" << Color::RESET << "      Override region count" << std::endl;
        std::cout << "  " << Color::GREEN << "--checkpoint" << Color::RESET << "        Perform checkpoint operations" << std::endl;
        std::cout << "  " << Color::GREEN << "--save file" << Color::RESET << "          Save brain state to file" << std::endl;
        std::cout << "  " << Color::GREEN << "--load file" << Color::RESET << "          Load brain state from file" << std::endl;
        std::cout << Color::BOLD << "\nAdvanced Options:" << Color::RESET << std::endl;
        std::cout << "  " << Color::GREEN << "--config=key=value" << Color::RESET << "     Set configuration value" << std::endl;
        std::cout << "  " << Color::GREEN << "--config key value" << Color::RESET << "      Set configuration value (alternative format)" << std::endl;
        std::cout << Color::BOLD << "\nExamples:" << Color::RESET << std::endl;
        std::cout << "  " << Color::YELLOW << "nlm --demo-mode --iterations 2000" << Color::RESET << std::endl;
        std::cout << "  " << Color::YELLOW << "nlm --phase2 --config configs/phase2.cfg" << Color::RESET << std::endl;
        std::cout << "  " << Color::YELLOW << "nlm --phase6 --save my_brain.bin --load my_brain.bin" << Color::RESET << std::endl;
        std::cout << "  " << Color::YELLOW << "nlm --phase6 --neuron-count 2000 --region-count 2" << Color::RESET << std::endl;
    }
};
