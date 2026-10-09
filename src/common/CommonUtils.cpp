// NLM Common Utilities Implementation
// Implementation of the common utility functions

#include "common/CommonUtils.hpp"
#include "core/Config/Config.hpp"
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <cctype>

namespace nlm {

// Helper function to print help
void CLIUtils::printHelp(const std::string& programName, const std::string& description) {
    std::cout << "Usage: " << programName << " [options] [args]\n\n";
    std::cout << description << "\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h           Show this help message\n";
    std::cout << "  --version, -v        Show version information\n";
    std::cout << "  --config <file>      Load configuration from file\n";
    std::cout << "  --output <file>      Write output to file\n";
    std::cout << "  --log-level <level>  Set log level (DEBUG, INFO, WARNING, ERROR)\n";
    std::cout << "  --mode <mode>        Execution mode (test, run, benchmark)\n";
    std::cout << "  --verbose, -V        Enable verbose output\n";
    std::cout << "  --quiet, -q          Suppress non-essential output\n\n";
    std::cout << "Executable-specific arguments can be added by individual programs.\n";
}

void CLIUtils::printVersion() {
    std::cout << "NLM Phase 2 - Real Neural Computation\n";
    std::cout << "Version: 2.0.0\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
}

CLIArgs CLIUtils::parseArgs(int argc, char* argv[], 
                           const std::vector<std::string>& allowedFlags,
                           const std::map<std::string, std::string>& flagDescriptions) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for flags
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version" || arg == "-v") {
            args.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (startsWith(arg, "--config")) {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (startsWith(arg, "--output")) {
            if (arg.find('=') != std::string::npos) {
                args.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (startsWith(arg, "--log-level")) {
            if (arg.find('=') != std::string::npos) {
                args.logLevel = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.logLevel = argv[++i];
            }
        } else if (startsWith(arg, "--mode")) {
            if (arg.find('=') != std::string::npos) {
                args.mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.mode = argv[++i];
            }
        } else if (arg.find('--') == 0 && arg.length() > 2) {
            // Custom flag
            std::string flag = arg.substr(2);
            size_t equalsPos = flag.find('=');
            if (equalsPos != std::string::npos) {
                args.customArgs[flag.substr(0, equalsPos)] = flag.substr(equalsPos + 1);
            } else {
                args.customArgs[flag] = "true";
            }
        } else {
            // Positional argument - pass through
            args.customArgs["positional"] = arg;
        }
    }
    
    return args;
}

bool CLIUtils::parseFlag(int& i, int argc, char* argv[], 
                         const std::string& flag, std::string& value) {
    std::string arg = argv[i];
    if (arg == "--" + flag || arg == "-" + flag.substr(0, 1)) {
        if (arg.find('=') != std::string::npos) {
            value = arg.substr(arg.find('=') + 1);
            return true;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            value = argv[++i];
            return true;
        }
    }
    return false;
}

std::string CLIUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CLIUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool CLIUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

OutputFormat CLIUtils::parseOutputFormat(const std::string& formatStr) {
    std::string lower = toLower(formatStr);
    if (lower == "json") return OutputFormat::JSON;
    if (lower == "csv") return OutputFormat::CSV;
    if (lower == "text") return OutputFormat::TEXT;
    return OutputFormat::CONSOLE; // Default
}

std::string CLIUtils::formatTime(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string CLIUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

// OutputManager implementation
OutputFormat OutputManager::currentFormat = OutputFormat::CONSOLE;
std::ofstream OutputManager::outputFileStream;

void OutputManager::setOutputFormat(OutputFormat format) {
    currentFormat = format;
}

OutputFormat OutputManager::getOutputFormat() {
    return currentFormat;
}

void OutputManager::setOutputFile(const std::string& filename) {
    outputFileStream.open(filename, std::ios::out | std::ios::trunc);
    if (!outputFileStream.is_open()) {
        std::cerr << "Warning: Could not open output file: " << filename << std::endl;
    }
}

std::ostream& OutputManager::getOutputStreamInternal() {
    if (!outputFileStream.is_open() && !outputFileStream.bad()) {
        return std::cout;
    }
    return outputFileStream.is_open() ? outputFileStream : std::cout;
}

std::ostream& OutputManager::getOutputStream() {
    return getOutputStreamInternal();
}

void OutputManager::printHeader(const std::string& title) {
    std::ostream& out = getOutputStreamInternal();
    out << "\n" << std::string(60, '=') << "\n";
    out << "  " << title << "\n";
    out << std::string(60, '=') << "\n";
}

void OutputManager::printSection(const std::string& title) {
    std::ostream& out = getOutputStreamInternal();
    out << "\n--- " << title << " ---\n";
}

void OutputManager::printResult(const std::string& name, float value, float max) {
    std::ostream& out = getOutputStreamInternal();
    out << "  " << std::left << std::setw(30) << name << ": ";
    out << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        out << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    out << "\n";
}

void OutputManager::printResult(const std::string& name, double value, double max) {
    std::ostream& out = getOutputStreamInternal();
    out << "  " << std::left << std::setw(30) << name << ": ";
    out << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        out << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    out << "\n";
}

void OutputManager::printResult(const std::string& name, int64_t value) {
    std::ostream& out = getOutputStreamInternal();
    out << "  " << std::left << std::setw(30) << name << ": " << value << "\n";
}

void OutputManager::printResult(const std::string& name, const std::string& value) {
    std::ostream& out = getOutputStreamInternal();
    out << "  " << std::left << std::setw(30) << name << ": " << value << "\n";
}

void OutputManager::printProgress(const std::string& message, int current, int total) {
    std::ostream& out = getOutputStreamInternal();
    float progress = (total > 0) ? (static_cast<float>(current) / total * 100.0f) : 0.0f;
    out << "  [" << std::fixed << std::setprecision(1) << progress << "%] " << message
        << " (" << current << "/" << total << ")\n";
}

void OutputManager::printSuccess(const std::string& message) {
    std::ostream& out = getOutputStreamInternal();
    out << "✓ " << message << "\n";
}

void OutputManager::printError(const std::string& message) {
    std::ostream& out = getOutputStreamInternal();
    out << "✗ " << message << "\n";
}

void OutputManager::printWarning(const std::string& message) {
    std::ostream& out = getOutputStreamInternal();
    out << "! " << message << "\n";
}

void OutputManager::printInfo(const std::string& message) {
    std::ostream& out = getOutputStreamInternal();
    out << "  " << message << "\n";
}

void OutputManager::printDebug(const std::string& message) {
    std::ostream& out = getOutputStreamInternal();
    out << "    [DEBUG] " << message << "\n";
}

// StringUtils implementation
std::string StringUtils::format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Determine required buffer size
    int len = std::vsnprintf(nullptr, 0, format.c_str(), args);
    va_end(args);
    
    if (len < 0) return "";
    
    // Format the string
    std::vector<char> buffer(len + 1);
    va_start(args, format);
    std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
    va_end(args);
    
    return std::string(buffer.data());
}

std::string StringUtils::leftAlign(const std::string& str, size_t width) {
    if (str.length() >= width) return str;
    return str + std::string(width - str.length(), ' ');
}

std::string StringUtils::rightAlign(const std::string& str, size_t width) {
    if (str.length() >= width) return str;
    return std::string(width - str.length(), ' ') + str;
}

std::string StringUtils::centerAlign(const std::string& str, size_t width) {
    if (str.length() >= width) return str;
    size_t left = (width - str.length()) / 2;
    size_t right = width - str.length() - left;
    return std::string(left, ' ') + str + std::string(right, ' ');
}

std::string StringUtils::toString(float value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string StringUtils::toString(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string StringUtils::toString(int64_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string StringUtils::toString(bool value) {
    return value ? "true" : "false";
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

bool StringUtils::contains(const std::string& str, const std::string& substring) {
    return str.find(substring) != std::string::npos;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::string StringUtils::escapeJSON(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "\\", "\\\\");
    result = replaceAll(result, "\"", "\\\"");
    result = replaceAll(result, "\b", "\\b");
    result = replaceAll(result, "\f", "\\f");
    result = replaceAll(result, "\n", "\\n");
    result = replaceAll(result, "\r", "\\r");
    result = replaceAll(result, "\t", "\\t");
    return result;
}

// ExperimentResult implementation
std::string ExperimentResult::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << name << "\",\n";
    oss << "  \"startTime\": " << startTime << ",\n";
    oss << "  \"endTime\": " << endTime << ",\n";
    oss << "  \"duration\": " << duration << ",\n";
    oss << "  \"totalReward\": " << totalReward << ",\n";
    oss << "  \"averageReward\": " << averageReward << ",\n";
    oss << "  \"successRate\": " << successRate << ",\n";
    oss << "  \"metadata\": {\n";
    for (auto it = metadata.begin(); it != metadata.end(); ++it) {
        if (it != metadata.begin()) oss << ",\n";
        oss << "    \"" << it->first << "\": \"" << it->second << "\"";
    }
    oss << "\n  }\n";
    oss << "}\n";
    return oss.str();
}

std::string ExperimentResult::summary() const {
    std::ostringstream oss;
    oss << "=== " << name << " ===\n";
    oss << "Duration: " << duration << "s\n";
    oss << "Total Reward: " << totalReward << "\n";
    oss << "Average Reward: " << averageReward << "\n";
    oss << "Success Rate: " << successRate << "%\n";
    return oss.str();
}

// ConfigUtils implementation
bool ConfigUtils::loadFromFile(std::shared_ptr<Config> config, const std::string& filename) {
    if (!config) return false;
    return config->loadFromFile(filename);
}

bool ConfigUtils::loadFromCLI(std::shared_ptr<Config> config, const std::map<std::string, std::string>& cliArgs) {
    if (!config) return false;
    // This is a placeholder - actual implementation would parse cliArgs
    // and set config values accordingly
    return true;
}

std::string ConfigUtils::getValueAsString(const std::shared_ptr<Config>& config, 
                                          const std::string& key,
                                          const std::string& defaultValue) {
    if (!config) return defaultValue;
    try {
        return config->getOr<std::string>(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

int64_t ConfigUtils::getValueAsInt(const std::shared_ptr<Config>& config,
                                  const std::string& key,
                                  int64_t defaultValue) {
    if (!config) return defaultValue;
    try {
        return config->getOr<int64_t>(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

double ConfigUtils::getValueAsDouble(const std::shared_ptr<Config>& config,
                                    const std::string& key,
                                    double defaultValue) {
    if (!config) return defaultValue;
    try {
        return config->getOr<double>(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

float ConfigUtils::getValueAsFloat(const std::shared_ptr<Config>& config,
                                  const std::string& key,
                                  float defaultValue) {
    if (!config) return defaultValue;
    try {
        return config->getOr<float>(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

bool ConfigUtils::getValueAsBool(const std::shared_ptr<Config>& config,
                                const std::string& key,
                                bool defaultValue) {
    if (!config) return defaultValue;
    try {
        return config->getOr<bool>(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

} // namespace nlm
