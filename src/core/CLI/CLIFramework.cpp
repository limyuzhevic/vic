#include "core/CLI/CLIFramework.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace nlm {

ProgressBar::ProgressBar(int total, const std::string& label, bool showPercentage)
    : total(total), current(0), label(label), showPercentage(showPercentage) {
    if (total > 0) {
        update(0, "Starting...");
    }
}

void ProgressBar::update(int current, const std::string& status) {
    this->current = std::max(0, std::min(current, total));
    
    double percentage = total > 0 ? (static_cast<double>(current) / total) * 100.0 : 0.0;
    
    if (showPercentage) {
        std::cout << "\r[" << std::string(current * 40 / total, '=') << ">";
        std::cout << std::string(40 - current * 40 / total, ' ') << "]";
        std::cout << std::fixed << std::setprecision(1) << percentage << "%";
    }
    
    if (!status.empty()) {
        std::cout << " - " << status;
    }
    
    std::cout.flush();
}

void ProgressBar::finish() {
    update(total, "Complete!");
    std::cout << std::endl;
}

void ProgressBar::setTotal(int total) {
    this->total = total;
}

OutputFormatter::OutputFormatter(bool useColors, bool verbose)
    : useColors(useColors), verbose(verbose), level(ExperienceLevel::INTERMEDIATE) {
}

void OutputFormatter::setLevel(ExperienceLevel level) {
    this->level = level;
    useColors = (level != ExperienceLevel::BEGINNER);
}

void OutputFormatter::printBanner() const {
    std::cout << colorize("", "cyan") << "╔═══════════════════════════════════════════════════════════════╗" << "\n";
    std::cout << colorize("", "cyan") << "║                                                               ║" << "\n";
    std::cout << colorize("", "cyan") << "║     " << bold("NLM — 熙然") << "                                     " << "\n";
    std::cout << colorize("", "cyan") << "║     " << bold("Neural Learning Machine") << "                                " << "\n";
    std::cout << colorize("", "cyan") << "║                                                               " << "\n";
    std::cout << colorize("", "cyan") << "║     " << bold("Phase 2: Real Neural Computation") << "                      " << "\n";
    std::cout << colorize("", "cyan") << "║                                                               " << "\n";
    std::cout << colorize("", "cyan") << "║     An experimental artificial developmental brain.          " << "\n";
    std::cout << colorize("", "cyan") << "║     This phase implements:                                  " << "\n";
    std::cout << colorize("", "cyan") << "║     - Real LIF neuron dynamics                              " << "\n";
    std::cout << colorize("", "cyan") << "║     - Event-driven spike propagation                        " << "\n";
    std::cout << colorize("", "cyan") << "║     - STDP and Hebbian plasticity                          " << "\n";
    std::cout << colorize("", "cyan") << "║     - Structural plasticity                                 " << "\n";
    std::cout << colorize("", "cyan") << "║                                                               " << "\n";
    std::cout << colorize("", "cyan") << "╚═══════════════════════════════════════════════════════════════╝" << "\n";
}

void OutputFormatter::printHelp(const CLIOptions& options, ExperienceLevel level) const {
    if (level == ExperienceLevel::BEGINNER) {
        showBeginnerHelp();
    } else if (level == ExperienceLevel::INTERMEDIATE) {
        showIntermediateHelp();
    } else {
        showAdvancedHelp();
    }
}

void OutputFormatter::printSection(const std::string& title, const std::string& content) const {
    std::cout << "\n" << colorize("=== " + title + " ===", "green") << "\n";
    std::cout << content << "\n";
}

void OutputFormatter::printError(const std::string& message) const {
    std::cout << colorize("ERROR: " + message, "red") << "\n";
}

void OutputFormatter::printWarning(const std::string& message) const {
    std::cout << colorize("WARNING: " + message, "yellow") << "\n";
}

void OutputFormatter::printSuccess(const std::string& message) const {
    std::cout << colorize("SUCCESS: " + message, "green") << "\n";
}

void OutputFormatter::printInfo(const std::string& message) const {
    std::cout << colorize("INFO: " + message, "blue") << "\n";
}

void OutputFormatter::printDebug(const std::string& message) const {
    if (verbose) {
        std::cout << colorize("DEBUG: " + message, "dim") << "\n";
    }
}

void OutputFormatter::printCommand(const std::string& command) const {
    std::cout << dim("> ") << command << "\n";
}

std::string OutputFormatter::formatProgress(const std::string& label, int current, int total) const {
    std::stringstream ss;
    ss << "[" << std::setw(4) << (total > 0 ? (current * 100 / total) : 0) << "%]";
    if (!label.empty()) {
        ss << " " << label;
    }
    ss << " (" << current << "/" << total << ")";
    return ss.str();
}

std::string OutputFormatter::formatParameter(const std::string& key, const std::string& value) const {
    std::stringstream ss;
    ss << colorize(key, "cyan") << ": " << value;
    return ss.str();
}

void OutputFormatter::setOutputFile(const std::string& filename) {
    outputFile.open(filename, std::ios::app);
    if (outputFile.is_open()) {
        fileOutputActive = true;
    } else {
        printWarning("Could not open log file: " + filename);
    }
}

void OutputFormatter::closeOutputFile() {
    if (fileOutputActive) {
        outputFile.close();
        fileOutputActive = false;
    }
}

void OutputFormatter::printJSON(const std::string& key, const std::string& value) const {
    std::cout << "\"" << key << "\": \"" << value << "\"";
}

void OutputFormatter::printJSON(const std::string& key, int value) const {
    std::cout << "\"" << key << "\": " << value;
}

void OutputFormatter::printJSON(const std::string& key, double value) const {
    std::cout << "\"" << key << "\": " << std::fixed << std::setprecision(3) << value;
}

void OutputFormatter::printJSON(const std::string& key, bool value) const {
    std::cout << "\"" << key << "\": " << (value ? "true" : "false");
}

HelpSystem::HelpSystem(const OutputFormatter& formatter) : formatter(formatter) {
}

void HelpSystem::showHelp(ExperienceLevel level, const CLIOptions& options) const {
    if (level == ExperienceLevel::BEGINNER) {
        showBeginnerHelp();
    } else if (level == ExperienceLevel::INTERMEDIATE) {
        showIntermediateHelp();
    } else {
        showAdvancedHelp();
    }
}

void HelpSystem::showExamples(ExperienceLevel level) const {
    std::cout << "\n" << colorize("EXAMPLES FOR " + std::string(level == ExperienceLevel::BEGINNER ? "BEGINNERS" : level == ExperienceLevel::INTERMEDIATE ? "INTERMEDIATE" : "ADVANCED", "green") << "\n");
    
    if (level == ExperienceLevel::BEGINNER) {
        std::cout << "\nBeginner examples:" << "\n";
        std::cout << "  ./nlm --help                          # Show help message" << "\n";
        std::cout << "  ./nlm --test-mode                     # Quick test (50 steps)" << "\n";
        std::cout << "  ./nlm all                             # Run all tests (default)" << "\n";
        std::cout << "  ./nlm basic                           # Run basic connectivity test" << "\n";
        std::cout << "\nTips for beginners:" << "\n";
        std::cout << "  - Use --help for basic usage information" << "\n";
        std::cout << "  - --test-mode is perfect for quick verification" << "\n";
        std::cout << "  - Start with 'basic' test to understand the system" << "\n";
    } else if (level == ExperienceLevel::INTERMEDIATE) {
        std::cout << "\nIntermediate examples:" << "\n";
        std::cout << "  ./nlm --neurons 1000 --timestep 0.0005 --plasticity" << "\n";
        std::cout << "  ./nlm basic --verbose --log-to-file" << "\n";
        std::cout << "  ./nlm plasticity --neurons 2000" << "\n";
        std::cout << "\nIntermediate tips:" << "\n";
        std::cout << "  - Adjust neuron count and timestep for experimentation" << "\n";
        std::cout << "  - Enable --plasticity to see learning in action" << "\n";
        std::cout << "  - Use --verbose for detailed progress output" << "\n";
    } else {
        std::cout << "\nAdvanced examples:" << "\n";
        std::cout << "  ./nlm --neurons 10000 --plasticity --timestep 0.0001 --timeout 300" << "\n";
        std::cout << "  ./nlm stdp --neurons 5000 --log-to-file --log-filename benchmark.json" << "\n";
        std::cout << "  ./nlm all --config custom.cfg --output results.json" << "\n";
        std::cout << "\nAdvanced tips:" << "\n";
        std::cout << "  - Use custom config files for specific experimental setups" << "\n";
        std::cout << "  - Monitor with --log-to-file for long-running experiments" << "\n";
        std::cout << "  - Set --timeout for automated batch processing" << "\n";
    }
}

void HelpSystem::showParameterInfo(const std::string& param) const {
    // Parameter information would be stored in a configuration file or database
    // For now, provide basic information
    if (param == "--neurons") {
        std::cout << "Number of neurons (10-10000). More neurons = more computation but realistic brain simulation." << "\n";
    } else if (param == "--plasticity") {
        std::cout << "Enable synaptic plasticity (STDP and Hebbian). Allows learning and adaptation." << "\n";
    } else if (param == "--timestep") {
        std::cout << "Simulation timestep in seconds (0.0001-0.001). Smaller = more accurate but slower." << "\n";
    } else if (param == "--test-mode") {
        std::cout << "Quick test mode with 50 steps. Perfect for verification and automated testing." << "\n";
    }
}

void HelpSystem::showBeginnerHelp() const {
    std::cout << "\n" << colorize("=" + std::string(60, '='), "cyan") << "\n";
    std::cout << colorize("BEGINNER HELP", "green", true) << "\n";
    std::cout << colorize("=" + std::string(60, '='), "cyan") << "\n";
    
    std::cout << "\nBASIC COMMANDS:" << "\n";
    std::cout << "  ./nlm                    # Run all tests (recommended start)" << "\n";
    std::cout << "  ./nlm --help             # Show help message" << "\n";
    std::cout << "  ./nlm --test-mode        # Quick test with 50 steps" << "\n";
    std::cout << "  ./nlm basic              # Run basic connectivity test" << "\n";
    
    std::cout << "\nCOMMON OPTIONS (with defaults):" << "\n";
    std::cout << "  --neurons 500           # Number of neurons (default)" << "\n";
    std::cout << "  --timestep 0.001         # Simulation timestep (default)" << "\n";
    
    std::cout << "\nTIPS:" << "\n";
    std::cout << "  • Start with './nlm --test-mode' to verify installation" << "\n";
    std::cout << "  • Use './nlm basic' to understand neural connectivity" << "\n";
    std::cout << "  • Enable --plasticity to see learning in action" << "\n";
    
    showExamples(ExperienceLevel::BEGINNER);
}

void HelpSystem::showIntermediateHelp() const {
    std::cout << "\n" << colorize("=" + std::string(60, '='), "cyan") << "\n";
    std::cout << colorize("INTERMEDIATE HELP", "yellow", true) << "\n";
    std::cout << colorize("=" + std::string(60, '='), "cyan") << "\n";
    
    std::cout << "\nCOMMANDS:" << "\n";
    std::cout << "  ./nlm all                 # Run all three test suites" << "\n";
    std::cout << "  ./nlm basic              # Basic connectivity test" << "\n";
    std::cout << "  ./nlm plasticity         # Plasticity learning experiment" << "\n";
    std::cout << "  ./nlm stdp               # STDP verification test" << "\n";
    std::cout << "  ./nlm benchmark          # Performance benchmark" << "\n";
    
    std::cout << "\nADVANCED OPTIONS:" << "\n";
    std::cout << "  --neurons N              # Neuron count (10-10000)" << "\n";
    std::cout << "  --plasticity             # Enable synaptic plasticity" << "\n";
    std::cout << "  --timestep T             # Simulation timestep" << "\n";
    std::cout << "  --verbose                # Detailed output with progress" << "\n";
    std::cout << "  --quiet                  # Minimal output" << "\n";
    std::cout << "  --log-to-file            # Save results to log file" << "\n";
    
    std::cout << "\nINTERMEDIATE TIPS:" << "\n";
    std::cout << "  • Adjust neuron count for computational balance" << "\n";
    std::cout << "  • Use --verbose to see progress bars and timing" << "\n";
    std::cout << "  • Combine --plasticity with tests for learning demonstrations" << "\n";
    std::cout << "  • Use --log-to-file for experiment reproducibility" << "\n";
    
    showExamples(ExperienceLevel::INTERMEDIATE);
}

void HelpSystem::showAdvancedHelp() const {
    std::cout << "\n" << colorize("=" + std::string(60, '='), "cyan") << "\n";
    std::cout << colorize("ADVANCED HELP", "red", true) << "\n";
    std::cout << colorize("=" + std::string(60, '='), "cyan") << "\n";
    
    std::cout << "\nFULL COMMAND LINE INTERFACE:" << "\n";
    std::cout << "  ./nlm [OPTIONS] [ARGS...]" << "\n";
    
    std::cout << "\nOPTIONS:" << "\n";
    std::cout << "  --help, -h              Show help message" << "\n";
    std::cout << "  --version               Show version info" << "\n";
    std::cout << "  --test-mode             Quick test (50 steps)" << "\n";
    std::cout << "  --neurons N             Number of neurons (10-10000)" << "\n";
    std::cout << "  --plasticity            Enable plasticity" << "\n";
    std::cout << "  --timestep T            Simulation timestep" << "\n";
    std::cout << "  --verbose               Detailed output" << "\n";
    std::cout << "  --quiet                 Minimal output" << "\n";
    std::cout << "  --config FILE           Load config file" << "\n";
    std::cout << "  --log-to-file           Save results to file" << "\n";
    std::cout << "  --log-filename FILENAME Save log to specific file" << "\n";
    std::cout << "  --timeout SECONDS       Maximum runtime" << "\n";
    
    std::cout << "\nCOMMANDS:" << "\n";
    std::cout << "  basic                   Run basic connectivity test" << "\n";
    std::cout << "  plasticity              Run plasticity learning experiment" << "\n";
    std::cout << "  stdp                    Run STDP verification test" << "\n";
    std::cout << "  all                     Run all tests (default)" << "\n";
    std::cout << "  benchmark               Run performance benchmark" << "\n";
    
    std::cout << "\nADVANCED FEATURES:" << "\n";
    std::cout << "  • Custom configuration files for experimental setups" << "\n";
    std::cout << "  • Real-time progress monitoring with estimated completion" << "\n";
    std::cout << "  • JSON output for programmatic data processing" << "\n";
    std::cout << "  • Timeout protection for automated batch processing" << "\n";
    std::cout << "  • Custom logging to file for reproducibility" << "\n";
    
    std::cout << "\nPERFORMANCE OPTIMIZATION:" << "\n";
    std::cout << "  • Neuron count: 1000-5000 for research, 10000+ for production" << "\n";
    std::cout << "  • Timestep: 0.0001 for accuracy, 0.001 for speed" << "\n";
    std::cout << "  • Use --quiet for headless operation" << "\n";
    
    showExamples(ExperienceLevel::ADVANCED);
}

CLIParser::CLIParser(HelpSystem& helpSystem) : helpSystem(helpSystem) {
}

CLIOptions CLIParser::parse(int argc, char** argv) {
    CLIOptions options;
    
    try {
        parseArguments(argc, argv, options);
        handleSpecialArguments(options, argc, argv);
        validateOptions(options);
    } catch (const std::exception& e) {
        std::cerr << colorize("CLI Error: " + std::string(e.what()), "red") << std::endl;
        std::exit(1);
    }
    
    return options;
}

ExperienceLevel CLIParser::detectExperienceLevel(const CLIOptions& options) const {
    // Auto-detection based on command line complexity
    int optionCount = 0;
    if (options.help || options.version) optionCount++;
    if (options.testMode) optionCount++;
    if (options.neurons != 500) optionCount++;
    if (options.timestep != 0.001) optionCount++;
    if (options.plasticity) optionCount++;
    if (options.verbose || options.quiet) optionCount++;
    if (!options.configFile.empty()) optionCount++;
    if (options.logToFile || !options.logFilename.empty()) optionCount++;
    if (options.timeout > 0) optionCount++;
    
    if (optionCount < 2) {
        return ExperienceLevel::BEGINNER;
    } else if (optionCount < 6) {
        return ExperienceLevel::INTERMEDIATE;
    } else {
        return ExperienceLevel::ADVANCED;
    }
}

void CLIParser::parseArguments(int argc, char** argv, CLIOptions& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--test-mode") {
            options.testMode = true;
        } else if (arg == "--plasticity") {
            options.plasticity = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--log-to-file") {
            options.logToFile = true;
        } else if (arg.find("--log-filename=") == 0) {
            options.logFilename = arg.substr(16);
        } else if (arg.find("--neurons=") == 0) {
            std::string value = arg.substr(10);
            try {
                options.neurons = std::stoi(value);
            } catch (...) {
                throw std::runtime_error("Invalid value for --neurons: " + value);
            }
        } else if (arg.find("--timestep=") == 0) {
            std::string value = arg.substr(11);
            try {
                options.timestep = std::stod(value);
            } catch (...) {
                throw std::runtime_error("Invalid value for --timestep: " + value);
            }
        } else if (arg.find("--timeout=") == 0) {
            std::string value = arg.substr(10);
            try {
                options.timeout = std::stoi(value);
            } catch (...) {
                throw std::runtime_error("Invalid value for --timeout: " + value);
            }
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg.find("--output=") == 0) {
            options.outputFile = arg.substr(9);
        } else if (arg.find("--output-format=") == 0) {
            options.outputFormat = arg.substr(16);
        } else if (arg == "basic" || arg == "plasticity" || arg == "stdp" || arg == "all" || arg == "benchmark") {
            options.commands.push_back(arg);
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }
}

void CLIParser::handleSpecialArguments(CLIOptions& options, int argc, char** argv) {
    // Handle --help before processing other arguments
    if (options.help) {
        // Help will be shown by main
        return;
    }
    
    // Handle --version before processing other arguments
    if (options.version) {
        // Version will be shown by main
        return;
    }
    
    // Process config file for additional options
    if (!options.configFile.empty() && options.configFile != "configs/default.cfg") {
        // Config loading is handled by the main Config class
    }
}

bool CLIParser::isValidParameter(const std::string& param) const {
    return param == "--neurons" || param == "--timestep" || param == "--timeout" ||
           param == "--config" || param == "--output" || param == "--output-format" ||
           param == "--log-filename";
}

std::string CLIParser::getParameterValue(int argc, char** argv, int& index) const {
    if (index + 1 < argc) {
        index++;
        return argv[index];
    }
    throw std::runtime_error("Missing value for parameter at position " + std::to_string(index));
}

void CLIParser::validateOptions(CLIOptions& options) const {
    if (options.neurons < 10) {
        options.neurons = 10;
    } else if (options.neurons > 10000) {
        options.neurons = 10000;
    }
    
    if (options.timestep < 0.0001) {
        options.timestep = 0.0001;
    } else if (options.timestep > 0.001) {
        options.timestep = 0.001;
    }
    
    if (options.timeout < 0) {
        options.timeout = 0;
    }
}

namespace CLIUtils {
    std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        while (start < end && std::isspace(str[start])) {
            start++;
        }
        
        while (end > start && std::isspace(str[end - 1])) {
            end--;
        }
        
        return str.substr(start, end - start);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void sleep(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
    
    std::string formatTime(double seconds) {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        
        std::stringstream ss;
        if (hours > 0) {
            ss << hours << "h ";
        }
        ss << minutes << "m " << secs << "s";
        return ss.str();
    }
    
    double getElapsedTime(const auto& startTime) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    std::string colorize(const std::string& text, const std::string& color, bool bold) {
        if (!ANSI_COLOR_SUPPORTED) return text;
        
        std::string colorCode;
        if (color == "red") colorCode = "\033[1;31m";
        else if (color == "green") colorCode = "\033[1;32m";
        else if (color == "yellow") colorCode = "\033[1;33m";
        else if (color == "blue") colorCode = "\033[1;34m";
        else if (color == "cyan") colorCode = "\033[1;36m";
        else if (color == "dim") colorCode = "\033[2m";
        else colorCode = "\033[0m";
        
        if (bold && color != "dim") {
            colorCode = "\033[1" + colorCode.substr(1);
        }
        
        return colorCode + text + "\033[0m";
    }
    
    std::string bold(const std::string& text) {
        return colorize(text, "", true);
    }
    
    std::string dim(const std::string& text) {
        return colorize(text, "dim", false);
    }
    
    ExperienceLevel detectExperienceLevel(const CLIOptions& options) {
        // Simple heuristic based on number of options used
        int optionCount = 0;
        if (options.help || options.version) optionCount++;
        if (options.testMode) optionCount++;
        if (options.neurons != 500) optionCount++;
        if (options.timestep != 0.001) optionCount++;
        if (options.plasticity) optionCount++;
        if (options.verbose || options.quiet) optionCount++;
        if (!options.configFile.empty()) optionCount++;
        if (options.logToFile || !options.logFilename.empty()) optionCount++;
        if (options.timeout > 0) optionCount++;
        
        if (optionCount < 2) {
            return ExperienceLevel::BEGINNER;
        } else if (optionCount < 6) {
            return ExperienceLevel::INTERMEDIATE;
        } else {
            return ExperienceLevel::ADVANCED;
        }
    }
    
    bool isValidNeuronCount(int neurons) {
        return neurons >= 10 && neurons <= 10000;
    }
    
    bool isValidTimestep(double timestep) {
        return timestep >= 0.0001 && timestep <= 0.001;
    }
    
    bool isValidTimeout(int timeout) {
        return timeout >= 0;
    }
}

// ANSI color support detection
bool CLIUtils::ANSI_COLOR_SUPPORTED = []() {
    #ifdef __unix__
        return isatty(fileno(stdout)) && getenv("TERM") != nullptr;
    #else
        return false;
    #endif
}();
}
