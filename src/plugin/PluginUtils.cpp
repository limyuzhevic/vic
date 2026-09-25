// Plugin utilities implementation
// Helper functions for plugin operations

#include "PluginInterface.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <regex>

namespace nlm {
namespace plugin {
namespace utils {

bool parseVersion(const std::string& versionStr, std::vector<int>& versionParts) {
    versionParts.clear();
    
    // Split by dots
    std::istringstream iss(versionStr);
    std::string token;
    while (std::getline(iss, token, '.')) {
        // Parse integer part
        try {
            size_t pos;
            int value = std::stoi(token, &pos);
            if (pos == token.size()) {
                versionParts.push_back(value);
            } else {
                return false;  // Invalid version format
            }
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return !versionParts.empty();
}

bool isVersionCompatible(const std::string& required, const std::string& available) {
    std::vector<int> requiredParts, availableParts;
    
    if (!parseVersion(required, requiredParts) || !parseVersion(available, availableParts)) {
        return false;
    }
    
    // Compare each part
    size_t minSize = std::min(requiredParts.size(), availableParts.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (availableParts[i] < requiredParts[i]) {
            return false;
        } else if (availableParts[i] > requiredParts[i]) {
            return true;  // Available version is newer
        }
    }
    
    // If all compared parts are equal, check lengths
    return availableParts.size() >= requiredParts.size();
}

template<typename T>
bool validateConfigValue(const std::string& key, const std::any& value, 
                       const std::string& typeName, T& outValue) {
    if (typeName == "int") {
        try {
            outValue = std::any_cast<int>(value);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        }
    } else if (typeName == "double") {
        try {
            outValue = std::any_cast<double>(value);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        }
    } else if (typeName == "string") {
        try {
            outValue = std::any_cast<std::string>(value);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        }
    } else if (typeName == "bool") {
        try {
            outValue = std::any_cast<bool>(value);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        }
    }
    
    return false;
}

std::string extractPluginName(const std::string& pluginPath) {
    std::filesystem::path path(pluginPath);
    std::string filename = path.filename().string();
    
    // Remove extension
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        filename = filename.substr(0, dotPos);
    }
    
    return filename;
}

std::string extractPluginVersion(const std::string& pluginPath) {
    // Try to extract version from filename or manifest
    std::string filename = extractPluginName(pluginPath);
    
    // Look for version pattern in filename
    std::regex versionRegex(R"(_v(\d+\.\d+\.\d+)$|_v(\d+\.\d+)$|_v(\d+)$)");
    std::smatch match;
    if (std::regex_search(filename, match, versionRegex)) {
        if (match[1].matched) return match[1].str();
        if (match[2].matched) return match[2].str();
        if (match[3].matched) return match[3].str();
    }
    
    return "1.0.0";  // Default version
}

std::string getPluginDescription(const std::string& pluginPath) {
    // Try to read description from plugin file
    // For now, return a default description
    return "Plugin loaded from " + pluginPath;
}

} // namespace utils
} // namespace plugin
} // namespace nlm