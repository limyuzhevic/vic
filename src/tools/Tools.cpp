#include "Tools.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace nlm {

/**
 * Common tool utilities implementation
 */

std::vector<std::pair<std::string, std::string>> ToolUtils::parseArgs(int argc, char** argv) {
    std::vector<std::pair<std::string, std::string>> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.emplace_back(key, value);
            }
        }
    }
    
    return args;
}

void ToolUtils::printHelp(const std::string& toolName, const std::string& description) {
    std::cout << toolName << std::endl;
    std::cout << std::string(toolName.size(), '=') << std::endl;
    std::cout << description << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << toolName << " [options]" << std::endl;
}

bool ToolUtils::validateFile(const std::string& filepath, bool requireRead) {
    std::filesystem::path path(filepath);
    
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    if (!std::filesystem::is_regular_file(path)) {
        return false;
    }
    
    if (requireRead && !(std::filesystem::status(path).permissions() & std::filesystem::perms::owner_read)) {
        return false;
    }
    
    return true;
}

bool ToolUtils::ensureDirectory(const std::string& dirpath) {
    std::filesystem::path path(dirpath);
    
    if (std::filesystem::exists(path)) {
        return std::filesystem::is_directory(path);
    }
    
    return std::filesystem::create_directories(path);
}

std::string ToolUtils::toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string ToolUtils::trim(const std::string& str) {
    std::stringstream ss(str);
    std::string word;
    std::string result;
    
    while (ss >> word) {
        if (!result.empty()) {
            result += " ";
        }
        result += word;
    }
    
    return result;
}

/**
 * Tool initialization implementation
 */

bool initializeTool(const ToolConfig& config) {
    try {
        auto& logger = nlm::Logger::getGlobal();
        logger.setLevel(config.logLevel == "debug" ? nlm::LogLevel::Debug :
                       config.logLevel == "info" ? nlm::LogLevel::Info :
                       config.logLevel == "warning" ? nlm::LogLevel::Warning :
                       config.logLevel == "error" ? nlm::LogLevel::Error :
                       nlm::LogLevel::Info);
        
        if (!config.logFile.empty()) {
            logger.setOutputFile(config.logFile);
        }
        
        if (!config.quiet) {
            logger.info("Tool initialized");
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize tool: " << e.what() << std::endl;
        return false;
    }
}

void cleanupTool() {
    auto& logger = nlm::Logger::getGlobal();
    logger.info("Tool cleaned up");
}

} // namespace nlm

