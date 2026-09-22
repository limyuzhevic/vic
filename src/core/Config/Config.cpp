#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyIndex;  // Fast lookup for key existence
    
    // Statistics for performance monitoring
    size_t totalLookups = 0;
    size_t successfulLookups = 0;
    double avgLookupTime = 0.0;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Clear existing entries before loading new ones
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
    
    // Check file size to warn about potentially large files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize > 10 * 1024 * 1024) {  // 10MB
        NLM_LOG_WARN("Config file is large (" + std::to_string(fileSize / 1024) + "KB). Parsing may take time.");
    }
    
    // Parse the file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseContent(content, filepath, ConfigSource::File);
}

}

bool Config::parseContent(const std::string& content, const std::string& sourceDesc, ConfigSource source) {
    // Implementation will be added in the next step
    // For now, use the simple parser for backward compatibility
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs (backward compatibility)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, source);
            
            // Log config entries for debugging
            NLM_LOG_DEBUG("Config entry: " + key + " = " + value + " (source: " + std::to_string(static_cast<int>(source)) + ")");
        } else {
            NLM_LOG_WARN("Invalid config line in " + sourceDesc + ": line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc == 0 || argv == nullptr) {
        return false;
    }
    
    bool hasChanges = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                hasChanges = true;
                NLM_LOG_INFO("Config from command line: " + key + " = " + value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            hasChanges = true;
            NLM_LOG_INFO("Config from command line: " + key + " = " + value);
        } else {
            NLM_LOG_WARN("Unhandled command line argument: " + arg);
        }
    }
    
    return hasChanges;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    // Write configuration entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = \"" << entry.value << "\"\n\n";
    }
    
    // Write statistics if enabled
    file << "# --- Configuration Statistics ---\n";
    file << "# Total entries: " << pImpl->entries.size() << "\n";
    file << "# Average lookup time: " << pImpl->avgLookupTime << " microseconds\n";
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Update existing entry
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = pImpl->entries.size() - 1;
    }
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    pImpl->totalLookups++;
    if (pImpl->totalLookups > 0) {
        pImpl->avgLookupTime = (pImpl->avgLookupTime * (pImpl->totalLookups - 1) + duration) / pImpl->totalLookups;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[it->second];
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    pImpl->totalLookups++;
    pImpl->successfulLookups++;
    if (pImpl->totalLookups > 0) {
        pImpl->avgLookupTime = (pImpl->avgLookupTime * (pImpl->totalLookups - 1) + duration) / pImpl->totalLookups;
    }
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARN("Config type mismatch for key: " + key);
        return std::nullopt;
    }
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

