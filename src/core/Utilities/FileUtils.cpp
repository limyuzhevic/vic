#include "MathUtils.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

// FileUtils implementation
bool FileUtils::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool FileUtils::directoryExists(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::string FileUtils::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}

std::string FileUtils::getFileNameWithoutExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return filename;
    return filename.substr(0, dotPos);
}

std::string FileUtils::joinPaths(const std::string& base, const std::string& filename) {
    return base + "/" + filename;
}

std::string FileUtils::getDirectoryPath(const std::string& path) {
    size_t slashPos = path.find_last_of('/');
    if (slashPos == std::string::npos) return ".";
    return path.substr(0, slashPos);
}

std::string FileUtils::getAbsolutePath(const std::string& path) {
    return std::filesystem::absolute(path).string();
}

bool FileUtils::isConfigurationFile(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    return ext == "cfg" || ext == "ini" || ext == "conf";
}

std::vector<std::string> FileUtils::parseConfigurationFile(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return lines;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove comments and trim whitespace
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    return lines;
}

bool FileUtils::saveConfigurationFile(const std::string& filename, const std::vector<std::string>& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& line : content) {
        file << line << std::endl;
    }
    
    return file.good();
}

} // namespace nlm
