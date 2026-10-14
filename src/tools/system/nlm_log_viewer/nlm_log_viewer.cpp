// nlm_log_viewer tool implementation
// Advanced log analysis

#include "tools/system/nlm_log_viewer/nlm_log_viewer.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ArgumentParser.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <vector>

namespace nlm {

NLMLogViewerTool::NLMLogViewerTool() : parser_(std::make_shared<ArgumentParser>("nlm_log_viewer", "Advanced log analysis")) {
    setupArgumentParser();
}

bool NLMLogViewerTool::initialize(const std::unordered_map<std::string, std::string>& config) {
    logger_ = std::make_unique<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    config_ = std::make_shared<nlm::Config>();
    return true;
}

bool NLMLogViewerTool::execute(const std::vector<std::string>& args) {
    if (parser_.getPositionalArgs().empty()) {
        parser_.printHelp();
        return false;
    }
    
    std::string command = parser_.getPositionalArgs()[0];
    
    if (command == "view") {
        return executeViewCommand(args);
    } else if (command == "analyze") {
        return executeAnalyzeCommand(args);
    } else {
        log("Unknown command: " + command, true);
        return false;
    }
}

bool NLMLogViewerTool::executeViewCommand(const std::vector<std::string>& args) {
    std::string logFile = parser_.wasProvided("--file") ? parser_.getValue("--file") : "";
    int lines = parser_.getValueInt("--lines", 100);
    std::string level = parser_.wasProvided("--level") ? parser_.getValue("--level") : "all";
    
    if (logFile.empty()) {
        log("Error: --file argument is required for 'view' command", true);
        return false;
    }
    
    log("Viewing log file: " << logFile);
    
    // Check if log file exists
    if (!std::filesystem::exists(logFile)) {
        log("Error: Log file does not exist: " << logFile, true);
        return false;
    }
    
    // Read and display log file
    std::ifstream inFile(logFile);
    if (!inFile.is_open()) {
        log("Error: Cannot read log file: " << logFile, true);
        return false;
    }
    
    std::string line;
    int lineCount = 0;
    
    std::cout << std::endl << "Log File: " << logFile << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    while (std::getline(inFile, line) && lineCount < lines) {
        // Filter by level if specified
        if (level != "all") {
            size_t bracketPos = line.find('[');
            if (bracketPos != std::string::npos) {
                size_t bracketEnd = line.find(']', bracketPos);
                if (bracketEnd != std::string::npos) {
                    std::string logLevel = line.substr(bracketPos + 1, bracketEnd - bracketPos - 1);
                    std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::tolower);
                    if (logLevel != level) {
                        continue;
                    }
                }
            }
        }
        
        std::cout << line << std::endl;
        lineCount++;
    }
    
    inFile.close();
    
    std::cout << std::endl << std::string(80, '=') << std::endl;
    std::cout << "Displayed " << lineCount << " lines (requested: " << lines << ")" << std::endl;
    
    return true;
}

bool NLMLogViewerTool::executeAnalyzeCommand(const std::vector<std::string>& args) {
    std::string logFile = parser_.wasProvided("--file") ? parser_.getValue("--file") : "";
    std::string output = parser_.wasProvided("--output") ? parser_.getValue("--output") : "log_analysis.json";
    
    if (logFile.empty()) {
        log("Error: --file argument is required for 'analyze' command", true);
        return false;
    }
    
    log("Analyzing log file: " << logFile);
    
    // Check if log file exists
    if (!std::filesystem::exists(logFile)) {
        log("Error: Log file does not exist: " << logFile, true);
        return false;
    }
    
    // Simulate log analysis
    std::map<std::string, double> analysis;
    
    // Read file and count different levels
    std::ifstream inFile(logFile);
    if (!inFile.is_open()) {
        log("Error: Cannot read log file: " << logFile, true);
        return false;
    }
    
    std::string line;
    int totalLines = 0;
    int errorLines = 0;
    int warningLines = 0;
    int infoLines = 0;
    int debugLines = 0;
    
    while (std::getline(inFile, line)) {
        totalLines++;
        
        size_t bracketPos = line.find('[');
        if (bracketPos != std::string::npos) {
            size_t bracketEnd = line.find(']', bracketPos);
            if (bracketEnd != std::string::npos) {
                std::string logLevel = line.substr(bracketPos + 1, bracketEnd - bracketPos - 1);
                std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::tolower);
                
                if (logLevel == "error") {
                    errorLines++;
                } else if (logLevel == "warning") {
                    warningLines++;
                } else if (logLevel == "info") {
                    infoLines++;
                } else if (logLevel == "debug") {
                    debugLines++;
                }
            }
        }
    }
    
    inFile.close();
    
    // Calculate analysis
    analysis["total_lines"] = totalLines;
    analysis["error_lines"] = errorLines;
    analysis["warning_lines"] = warningLines;
    analysis["info_lines"] = infoLines;
    analysis["debug_lines"] = debugLines;
    
    if (totalLines > 0) {
        analysis["error_percentage"] = (errorLines / static_cast<double>(totalLines)) * 100.0;
        analysis["warning_percentage"] = (warningLines / static_cast<double>(totalLines)) * 100.0;
        analysis["info_percentage"] = (infoLines / static_cast<double>(totalLines)) * 100.0;
        analysis["debug_percentage"] = (debugLines / static_cast<double>(totalLines)) * 100.0;
    }
    
    // Create output file
    std::ofstream outFile(output);
    if (!outFile.is_open()) {
        log("Error: Cannot create analysis file: " << output, true);
        return false;
    }
    
    outFile << "{\"log_analysis\": {\"file\": \"" << logFile << "\", \"timestamp\": \"2026-10-14T20:30:36Z\", \"analysis\": {";
    
    bool first = true;
    for (const auto& pair : analysis) {
        if (!first) outFile << ", ";
        outFile << "\"" << pair.first << "\": " << pair.second;
        first = false;
    }
    outFile << "}}}";
    outFile.close();
    
    log("Log analysis saved to: " << output");
    
    // Print console summary
    std::cout << std::endl << "Log Analysis Summary:" << std::endl;
    std::cout << std::left << std::setw(20) << "Level" << std::setw(10) << "Count" << std::setw(12) << "Percentage" << std::endl;
    std::cout << std::string(42, '=') << std::endl;
    
    std::vector<std::pair<std::string, int>> levels = {
        {"ERROR", errorLines},
        {"WARNING", warningLines},
        {"INFO", infoLines},
        {"DEBUG", debugLines}
    };
    
    for (const auto& level : levels) {
        double percentage = (totalLines > 0) ? (level.second / static_cast<double>(totalLines)) * 100.0 : 0.0;
        std::cout << std::left << std::setw(20) << level.first 
                  << std::setw(10) << level.second 
                  << std::setw(12) << percentage << "%" << std::endl;
    }
    
    return true;
}

void NLMLogViewerTool::setupArgumentParser() {
    parser_->addArgument("command", 0, "command", "Log viewer command", "", false);
    parser_->addArgument("--file", 'f', "file", "Log file path", "");
    parser_->addArgument("--output", 'o', "output", "Output file path", "log_analysis.json");
    parser_->addArgument("--lines", 0, "lines", "Number of lines to display", "100");
    parser_->addArgument("--level", 0, "level", "Log level filter (error, warning, info, debug, all)", "all");
    parser_->addArgument("--help", 0, "help", "Show help information");
    parser_->addArgument("--version", 0, "version", "Show version information");
}

std::string NLMLogViewerTool::getDescription() const {
    return "Advanced log analysis for NLM. Supports viewing and analyzing log files with filtering and statistics.";
}

std::string NLMLogViewerTool::getVersion() const {
    return "1.0.0";
}

std::string NLMLogViewerTool::getUsage() const {
    return "nlm_log_viewer <command> [options]"; 
}

void NLMLogViewerTool::shutdown() {
    if (logger_) {
        Logger::setGlobal(nullptr);
        logger_.reset();
    }
}

void NLMLogViewerTool::log(const std::string& message, bool error) const {
    std::cout << (error ? "[ERROR] " : "[INFO] ") << message << std::endl;
}

} // namespace nlm