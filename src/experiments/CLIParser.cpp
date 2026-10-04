#include "CLIParser.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstring>

namespace nlm {

CLIParser::CommandLineOptions CLIParser::parse(int argc, char* argv[]) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Skip if argument is empty
        if (arg.empty()) continue;
        
        // Check for help/version flags
        if (matches(arg, "help", true) || matches(arg, "h")) {
            options.showHelp = true;
        } else if (matches(arg, "version", true) || matches(arg, "v")) {
            options.showVersion = true;
        } else if (matches(arg, "verbose", true) || matches(arg, "verb")) {
            options.verbose = true;
        } else if (matches(arg, "quiet", true) || matches(arg, "q")) {
            options.quiet = true;
        } else if (matches(arg, "interactive", true) || matches(arg, "i")) {
            options.interactive = true;
        } else if (matches(arg, "export", true) || matches(arg, "export-results")) {
            options.exportResults = true;
        } else if (matches(arg, "import", true) || matches(arg, "import-config")) {
            options.importResults = true;
        } else if (matches(arg, "enable-colors", true) || matches(arg, "color")) {
            options.enableColors = true;
        } else if (matches(arg, "no-colors", true) || matches(arg, "nocolor")) {
            options.enableColors = false;
        } else if (matches(arg, "enable-progress", true) || matches(arg, "progress")) {
            options.enableProgress = true;
        } else if (matches(arg, "no-progress", true) || matches(arg, "noprogress")) {
            options.enableProgress = false;
        } else if (matches(arg, "enable-logging", true) || matches(arg, "logging")) {
            options.enableLogging = true;
        } else if (matches(arg, "no-logging", true) || matches(arg, "nologging")) {
            options.enableLogging = false;
        } else if (matches(arg, "max-steps", true)) {
            // Handle --max-steps=2000 or --max-steps 2000
            options.maxSteps = std::stoi(extractValue(arg, true));
        } else if (matches(arg, "max-episodes", true)) {
            // Handle --max-episodes=5 or --max-episodes 5
            options.maxEpisodes = std::stoi(extractValue(arg, true));
        } else if (matches(arg, "timeout", true)) {
            // Handle --timeout=1.5 or --timeout 1.5
            options.timeout = std::stof(extractValue(arg, true));
        } else if (matches(arg, "config", true) || matches(arg, "c")) {
            options.configFile = extractValue(arg, true);
        } else if (matches(arg, "output", true) || matches(arg, "o")) {
            options.outputFile = extractValue(arg, true);
        } else if (matches(arg, "log", true) || matches(arg, "l")) {
            options.logFile = extractValue(arg, true);
        } else if (matches(arg, "education", true) || matches(arg, "e")) {
            options.educationMode = extractValue(arg, true);
        } else if (matches(arg, "power", true) || matches(arg, "power-user")) {
            options.powerUserMode = extractValue(arg, true);
        } else if (matches(arg, "export-format", true) || matches(arg, "export-fmt")) {
            options.exportFormat = extractValue(arg, true);
        } else if (matches(arg, "import-format", true) || matches(arg, "import-fmt")) {
            options.importFormat = extractValue(arg, true);
        } else {
            // Unknown argument - treat as custom argument
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

void CLIParser::showHelp() const {
    std::cout << Colors::BOLD << Colors::CYAN << "NLM Enhanced Phase 6 Demo" << Colors::RESET << std::endl;
    std::cout << Colors::DIM << "=" << std::string(60, '=') << Colors::RESET << std::endl << std::endl;
    
    std::cout << Colors::BOLD << "Basic Usage:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --help" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --version" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Basic Options:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--help, -h" << Colors::RESET << "           " << Colors::WHITE << "Show this help message" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--version, -v" << Colors::RESET << "        " << Colors::WHITE << "Show version information" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--verbose, -V" << Colors::RESET << "       " << Colors::WHITE << "Enable verbose output" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--quiet, -q" << Colors::RESET << "         " << Colors::WHITE << "Suppress informational messages" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--interactive, -i" << Colors::RESET << "  " << Colors::WHITE << "Enable interactive mode" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Configuration:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--config, -c <file>" << Colors::RESET << "   " << Colors::WHITE << "Load configuration from file (.cfg or .json)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--output, -o <file>" << Colors::RESET << "   " << Colors::WHITE << "Save results to file" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--log <file>" << Colors::RESET << "       " << Colors::WHITE << "Log output to file" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Display Options:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--enable-colors, --color" << Colors::RESET << "   " << Colors::WHITE << "Enable colored output (default)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--no-colors, --nocolor" << Colors::RESET << "  " << Colors::WHITE << "Disable colored output" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--enable-progress, --progress" << Colors::RESET << " " << Colors::WHITE << "Enable progress indicators (default)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--no-progress, --noprogress" << Colors::RESET << Colors::WHITE << "  Disable progress indicators" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--enable-logging, --logging" << Colors::RESET << " " << Colors::WHITE << "Enable logging to file (default)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--no-logging, --nologging" << Colors::RESET << "  " << Colors::WHITE << "Disable logging" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Simulation Control:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--max-steps <n>" << Colors::RESET << "       " << Colors::WHITE << "Maximum number of simulation steps (default: 2000)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--max-episodes <n>" << Colors::RESET << "    " << Colors::WHITE << "Maximum number of episodes (default: 5)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--timeout <s>" << Colors::RESET << "       " << Colors::WHITE << "Timeout in seconds (default: 0 = unlimited)" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Educational Mode:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--education <topic>" << Colors::RESET << "  " << Colors::WHITE << "Enable educational mode with step-by-step guidance" << Colors::RESET << std::endl;
    std::cout << "    Topics: basic, integration, memory, neuromodulation, prediction, development, replay, checkpointing" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Power User Mode:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--power <mode>" << Colors::RESET << "      " << Colors::WHITE << "Enable power user mode with advanced features" << Colors::RESET << std::endl;
    std::cout << "    Modes: batch, tuning, comparison, plugin, api" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Export/Import:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--export-results" << Colors::RESET << "     " << Colors::WHITE << "Export results to file (default: JSON)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--export-format <fmt>" << Colors::RESET << " " << Colors::WHITE << "Export format (json, csv, xml)" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--import-results" << Colors::RESET << "     " << Colors::WHITE << "Import results from file" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--import-format <fmt>" << Colors::RESET << " " << Colors::WHITE << "Import format (json, csv, xml)" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Examples:" << Colors::RESET << std::endl;
    std::cout << Colors::DIM << "# Basic demo with custom config" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --config demo.cfg" << Colors::RESET << std::endl;
    std::cout << std::endl;
    std::cout << Colors::DIM << "# Educational mode focusing on memory" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --education memory" << Colors::RESET << std::endl;
    std::cout << std::endl;
    std::cout << Colors::DIM << "# Power user mode for batch processing" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --power batch --max-episodes 20" << Colors::RESET << std::endl;
    std::cout << std::endl;
    std::cout << Colors::DIM << "# Export results with custom format" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::GREEN << "nlm_phase6_demo_enhanced --output results.csv --export-format csv" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << "Backward Compatibility:" << Colors::RESET << std::endl;
    std::cout << Colors::WHITE << "All existing CLI arguments continue to work:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "[num_episodes] [steps_per_episode]" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::YELLOW << "--neuron_count=500" << Colors::RESET << "     " << Colors::YELLOW << "--connection_probability=0.1" << Colors::RESET << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::DIM << "=" << std::string(60, '=') << Colors::RESET << std::endl;
}

void CLIParser::showVersion() const {
    std::cout << Colors::BOLD << "NLM Phase 6 Demo Enhanced" << Colors::RESET << std::endl;
    std::cout << Colors::DIM << "Version: 2.0.0 (Enhanced)" << Colors::RESET << std::endl;
    std::cout << Colors::DIM << "Build: " << __DATE__ << " " << __TIME__ << Colors::RESET << std::endl;
    std::cout << Colors::DIM << "Features: Advanced CLI, Config support, Visualization, Education, Power User" << Colors::RESET << std::endl;
}

bool CLIParser::matches(const std::string& arg, const std::string& pattern, bool isLongOption) const {
    if (arg.empty()) return false;
    
    std::string argCopy = arg;
    
    // Remove dashes and check for prefix
    if (isLongOption && argCopy.rfind("--", 0) == 0) {
        argCopy = argCopy.substr(2);
    } else if (!isLongOption && argCopy.rfind("-", 0) == 0 && argCopy.size() > 1) {
        argCopy = argCopy.substr(1);
    }
    
    // Case-insensitive comparison
    std::string patternLower = pattern;
    std::string argCopyLower = argCopy;
    std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);
    std::transform(argCopyLower.begin(), argCopyLower.end(), argCopyLower.begin(), ::tolower);
    
    return argCopyLower == patternLower;
}

std::string CLIParser::extractValue(const std::string& arg, bool hasPrefix) const {
    if (!hasPrefix) return "";
    
    // Handle --key=value format
    size_t equalsPos = arg.find('=');
    if (equalsPos != std::string::npos) {
        std::string value = arg.substr(equalsPos + 1);
        // Trim whitespace
        size_t first = value.find_first_not_of(" \t\n\r");
        size_t last = value.find_last_not_of(" \t\n\r");
        if (first != std::string::npos) {
            return value.substr(first, (last - first + 1));
        }
        return "";
    }
    
    // For --key value format, would need next argument - not supported in this simple implementation
    return "";
}

} // namespace nlm
