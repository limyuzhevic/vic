#include "utils/EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <thread>

namespace nlm {
namespace utils {

bool EnhancedLogger::exportLogsToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Enhanced Log Export\n";
    file << "Generated: " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : logHistory) {
        file << "[" << levelToString(entry.level) << "] "
             << "[" << entry.file << ":" << entry.line << "] "
             << "[" << entry.threadId << "] "
             << entry.message << "\n";
    }
    
    return true;
}

bool ErrorHandler::validateConfigValue(const std::string& key, const std::string& value) {
    // Simple validation logic
    if (key == "random_seed") {
        try {
            int seed = std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return timestep > 0.0;
        } catch (...) {
            return false;
        }
    }
    return true; // Default to valid for unknown keys
}

bool ErrorHandler::validateSimulationParams(const std::string& params) {
    // Basic validation
    return !params.empty();
}

bool ErrorHandler::safeFileRead(const std::string& filepath, std::string& content) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        handleError(ErrorCode::FileIOError, "Failed to open file for reading: {}", filepath);
        return false;
    }
    
    content.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    return true;
}

bool ErrorHandler::safeFileWrite(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        handleError(ErrorCode::FileIOError, "Failed to open file for writing: {}", filepath);
        return false;
    }
    
    file.write(content.c_str(), content.size());
    return file.good();
}

} // namespace utils
} // namespace nlm