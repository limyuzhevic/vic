#include "cli/ConfigDiff.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unordered_map>

namespace nlm {

ConfigDiff::ConfigDiff() {
    // Initialize diff options
}

std::string ConfigDiff::compare(const std::string& file1, const std::string& file2,
                                const std::string& format) {
    std::stringstream result;
    
    result << "=== Configuration Comparison ===" << std::endl;
    result << "File 1: " << file1 << std::endl;
    result << "File 2: " << file2 << std::endl;
    result << std::endl;
    
    // Parse configurations (simplified - would use actual parser in production)
    auto config1 = parseConfigFile(file1, format);
    auto config2 = parseConfigFile(file2, format);
    
    // Find differences
    auto diff = findDifferences(config1, config2);
    
    // Generate diff report based on format
    if (format == "json" || format == "yaml") {
        result << generateDiffReport(diff);
    } else {
        result << "=== Key Differences ===" << std::endl;
        if (diff.added.empty() && diff.removed.empty() && diff.changed.empty()) {
            result << "No differences found." << std::endl;
        } else {
            if (!diff.added.empty()) {
                result << "Added keys:" << std::endl;
                for (const auto& key : diff.added) {
                    result << "  + " << key << std::endl;
                }
            }
            
            if (!diff.removed.empty()) {
                result << "Removed keys:" << std::endl;
                for (const auto& key : diff.removed) {
                    result << "  - " << key << std::endl;
                }
            }
            
            if (!diff.changed.empty()) {
                result << "Changed keys:" << std::endl;
                for (const auto& pair : diff.changed) {
                    result << "  * " << pair.first.first << " -> " << pair.first.second
                           << std::endl;
                }
            }
        }
    }
    
    return result.str();
}

std::string ConfigDiff::generateDiffReport(const std::map<std::string, std::string>& changes) {
    std::stringstream result;
    
    // Simple JSON-like diff output
    result << "{\n";
    bool first = true;
    for (const auto& change : changes) {
        if (!first) result << ",\n";
        first = false;
        
        result << "  \"" << change.first << "\": {\n";
        result << "    \"before\": \"\" (placeholder)\n";
        result << "    \"after\": \"\" (placeholder)\n";
        result << "  }";
    }
    result << "\n}";
    
    return result.str();
}

std::string ConfigDiff::highlightChanges(const std::string& content,
                                         const std::vector<std::pair<size_t, size_t>>& diffs) {
    std::stringstream result;
    
    // Simple diff highlighting
    size_t lineStart = 0;
    for (size_t i = 0; i < diffs.size(); ++i) {
        size_t lineEnd = diffs[i].first;
        
        // Add lines before diff
        std::string line = content.substr(lineStart, lineEnd - lineStart);
        result << line << std::endl;
        
        // Add diff marker
        if (i < diffs.size()) {
            result << "  >> " << content.substr(lineEnd, diffs[i].second) << " <<" << std::endl;
        }
        
        lineStart = lineEnd + diffs[i].second;
    }
    
    // Add remaining content
    if (lineStart < content.length()) {
        result << content.substr(lineStart) << std::endl;
    }
    
    return result.str();
}

bool ConfigDiff::exportConfig(const std::string& filepath, const CLIOptions& options) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration in requested format
        if (options.jsonOutput || (options.outputFormat == "json" && !options.yamlOutput)) {
            file << "// Exported configuration (JSON format)" << std::endl;
            // Would actually write JSON in production
            file << "{\n  // Configuration data\n}";
        } else if (options.yamlOutput || (options.outputFormat == "yaml" && !options.jsonOutput)) {
            file << "# Exported configuration (YAML format)" << std::endl;
            // Would actually write YAML in production
            file << "# Configuration data\nkey: value";
        } else {
            file << "# Exported configuration (simple format)" << std::endl;
            // Would actually write simple format in production
            file << "# Configuration data\n# key = value";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error exporting configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigDiff::importConfig(const std::string& filepath, CLIOptions& options) {
    try {
        // Clear existing custom args
        options.customArgs.clear();
        
        // Parse file based on format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                          (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                options.customArgs[key] = value;
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error importing configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigDiff::validateConfig(const std::string& filepath, const std::string& format) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filepath << std::endl;
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        file.close();
        
        // Validate based on format
        if (format == "json" || (format == "auto" && detectFormatFromContent(content) == "json")) {
            // Validate JSON (simplified)
            if (content.empty() || content.front() != '{' || content.back() != '}') {
                std::cerr << "Invalid JSON format in: " << filepath << std::endl;
                return false;
            }
        } else if (format == "yaml" || (format == "auto" && detectFormatFromContent(content) == "yaml")) {
            // Validate YAML (simplified)
            // Would need proper YAML parser in production
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error validating configuration: " << e.what() << std::endl;
        return false;
    }
}

std::map<std::string, std::string> ConfigDiff::parseConfigFile(const std::string& filepath, const std::string& format) {
    std::map<std::string, std::string> config;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return config;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        file.close();
        
        // Simple parsing based on format
        if (format == "json") {
            // Simplified JSON parsing
            size_t pos = content.find('"');
            while (pos != std::string::npos && pos + 1 < content.length()) {
                size_t keyStart = pos;
                pos = content.find('"', keyStart + 1);
                if (pos == std::string::npos) break;
                
                std::string key = content.substr(keyStart + 1, pos - keyStart - 1);
                
                size_t colonPos = content.find(':', pos);
                if (colonPos == std::string::npos) break;
                
                size_t valueStart = content.find('"', colonPos);
                if (valueStart == std::string::npos) break;
                
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                config[key] = value;
                
                pos = valueEnd;
            }
        } else {
            // Parse simple key=value format
            std::istringstream stream(content);
            std::string line;
            while (std::getline(stream, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    config[key] = value;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config file " << filepath << ": " << e.what() << std::endl;
    }
    
    return config;
}

DiffResult ConfigDiff::findDifferences(const std::map<std::string, std::string>& config1,
                                       const std::map<std::string, std::string>& config2) {
    DiffResult diff;
    
    // Find added and changed keys
    for (const auto& pair : config2) {
        auto it = config1.find(pair.first);
        if (it == config1.end()) {
            diff.added.insert(pair.first);
        } else if (it->second != pair.second) {
            diff.changed[pair.first] = std::make_pair(it->second, pair.second);
        }
    }
    
    // Find removed keys
    for (const auto& pair : config1) {
        if (config2.find(pair.first) == config2.end()) {
            diff.removed.insert(pair.first);
        }
    }
    
    return diff;
}

std::string ConfigDiff::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigDiff::detectFormatFromContent(const std::string& content) {
    if (content.empty()) return "simple";
    
    std::string trimmed = trim(content);
    
    // Check for JSON (starts with { or [)
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        return "json";
    }
    
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        return "json";
    }
    
    // Check for YAML (contains colons at start of lines)
    bool hasColonAfterTab = false;
    size_t pos = 0;
    while ((pos = trimmed.find('\n', pos)) != std::string::npos) {
        pos++;
        size_t end = trimmed.find('\n', pos);
        if (end == std::string::npos) end = trimmed.size();
        
        std::string line = trimmed.substr(pos, end - pos);
        line = trim(line);
        
        // Check for YAML-like pattern
        if (!line.empty() && line.front() == ':' && line.size() > 1 && line[1] != ':') {
            hasColonAfterTab = true;
            break;
        }
    }
    
    if (hasColonAfterTab) {
        return "yaml";
    }
    
    // Default to simple format
    return "simple";
}

} // namespace nlm
