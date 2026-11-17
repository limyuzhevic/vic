#include "NLMCLI.hpp"
#include "core/Config/Config.hpp"
#include <getopt.h>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

namespace nlm {

CommandLineInterface::CLIOptions CommandLineInterface::parseArgs(int argc, char* argv[]) {
    CLIOptions options;
    options.verbose = false;
    options.quiet = false;
    options.help = false;
    options.version = false;
    options.numTrials = 0; // Default will be set by individual demos
    
    static struct option longOptions[] = {
        {"config", required_argument, 0, 'c'},
        {"log-level", required_argument, 0, 'l'},
        {"verbose", no_argument, 0, 'v'},
        {"quiet", no_argument, 0, 'q'},
        {"trials", required_argument, 0, 't'},
        {"output", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int longIndex = 0;
    while ((opt = getopt_long(argc, argv, "c:l:vqt:o:hV", longOptions, &longIndex)) != -1) {
        switch (opt) {
            case 'c':
                options.configFile = optarg;
                break;
            case 'l':
                options.logLevel = optarg;
                break;
            case 'v':
                options.verbose = true;
                break;
            case 'q':
                options.quiet = true;
                break;
            case 't':
                options.numTrials = std::stoull(optarg);
                break;
            case 'o':
                options.outputFile = optarg;
                break;
            case 'h':
                options.help = true;
                break;
            case 'V':
                options.version = true;
                break;
            default:
                break;
        }
    }
    
    // Collect remaining arguments
    for (int i = optind; i < argc; ++i) {
        options.extraArgs.push_back(argv[i]);
    }
    
    return options;
}

void CommandLineInterface::printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM (Neural Learning Machine) - Phase " << programName << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c, --config FILE        Load configuration from file" << std::endl;
    std::cout << "  -l, --log-level LEVEL    Set logging level (ERROR, WARN, INFO, DEBUG)" << std::endl;
    std::cout << "  -v, --verbose            Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet              Suppress non-essential output" << std::endl;
    std::cout << "  -t, --trials N           Number of trials (demo-specific)" << std::endl;
    std::cout << "  -o, --output FILE        Save results to file" << std::endl;
    std::cout << "  -h, --help               Show this help message" << std::endl;
    std::cout << "  -V, --version            Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --trials 100" << std::endl;
    std::cout << "  " << programName << " --config demo.cfg --log-level INFO" << std::endl;
    std::cout << "  " << programName << " --verbose --output results.txt" << std::endl;
}

void CommandLineInterface::printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Experimental Artificial Developmental Brain" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  Compiler: " << __VERSION__ << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Build Date: " << __DATE__ << std::endl;
    std::cout << "  Build Time: " << __TIME__ << std::endl;
}

std::shared_ptr<Config> CommandLineInterface::createConfigFromOptions(const CLIOptions& options) {
    auto config = std::make_shared<Config>();
    
    if (!options.configFile.empty()) {
        if (!config->loadFromFile(options.configFile)) {
            printWarning("Failed to load config file: " + options.configFile);
        }
    }
    
    // Apply command-line specific settings
    if (options.numTrials > 0) {
        config->set("num_trials", static_cast<int64_t>(options.numTrials), ConfigSource::CommandLine);
    }
    
    return config;
}

void CommandLineInterface::setupLogging(const CLIOptions& options) {
    LogLevel level = LogLevel::Info;
    
    if (!options.logLevel.empty()) {
        std::string levelStr = options.logLevel;
        std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
        
        if (levelStr == "ERROR") level = LogLevel::Error;
        else if (levelStr == "WARN") level = LogLevel::Warn;
        else if (levelStr == "DEBUG") level = LogLevel::Debug;
        else level = LogLevel::Info; // Default
    }
    
    Logger::getGlobal().setLevel(level);
}

void CommandLineInterface::saveResultsToFile(const std::string& filename, const std::string& results) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << results;
        file.close();
        printSuccess("Results saved to: " + filename);
    } else {
        printError("Failed to save results to: " + filename);
    }
}

void printBanner(const std::string& title) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     " << title << "                                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << std::endl << std::string(70, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void printSubHeader(const std::string& title) {
    std::cout << std::endl << "  " << title << std::endl;
    std::cout << "  " << std::string(title.length(), '-') << std::endl;
}

void printSection(const std::string& title) {
    std::cout << std::endl << "--- " << title << " ---" << std::endl;
}

void printResult(const std::string& name, float value, float max) {
    std::cout << "    " << std::left << std::setw(35) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

void printResult(const std::string& name, double value, double max) {
    std::cout << "    " << std::left << std::setw(35) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

void printResult(const std::string& name, size_t value, size_t max) {
    std::cout << "    " << std::left << std::setw(35) << name << ": ";
    std::cout << value;
    if (max > 0) {
        std::cout << " (" << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

void printResult(const std::string& name, bool value) {
    std::cout << "    " << std::left << std::setw(35) << name << ": ";
    std::cout << (value ? "YES" : "NO") << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void printWarning(const std::string& message) {
    std::cout << "[WARN] " << message << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

std::string formatFloat(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string formatPercentage(float value, float max) {
    if (max <= 0) return formatFloat(value, 1) + "%";
    float percentage = (value / max) * 100.0f;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << percentage << "%";
    return ss.str();
}

std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    return std::string(buffer);
}

}  // namespace nlm
