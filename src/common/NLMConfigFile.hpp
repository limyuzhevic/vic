// Configuration file parser for NLM
// Implements INI-style configuration file parsing with type conversion

#ifndef NLM_CONFIG_FILE_H
#define NLM_CONFIG_FILE_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace {

// Configuration section structure
struct ConfigSection {
    std::string name;
    std::map<std::string, std::string> values;
};

// Configuration file parser
class ConfigFile {
private:
    std::vector<ConfigSection> sections;
    std::string currentSection;
    bool ignoreComments;
    bool trimWhitespace;

public:
    ConfigFile() : ignoreComments(true), trimWhitespace(true) {}

    // Load configuration from file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        sections.clear();
        currentSection = "";
        ConfigSection current;

        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Trim whitespace
            if (trimWhitespace) {
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);
            }

            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Skip comments
            if (ignoreComments && line[0] == '#' && line[1] == '#') {
                continue;
            }

            // Section header
            if (line[0] == '[' && line.back() == ']') {
                if (!currentSection.empty()) {
                    sections.push_back(current);
                }
                currentSection = line.substr(1, line.size() - 2);
                current = ConfigSection{currentSection, {}};
            }
            // Key-value pair
            else if (line.find('=') != std::string::npos) {
                size_t delimiterPos = line.find('=', 0);
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                
                if (trimWhitespace) {
                    key.erase(key.find_last_not_of(" \t") + 1);
                    key.erase(0, key.find_first_not_of(" \t"));
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                }

                current.values[key] = value;
            }
        }

        if (!currentSection.empty()) {
            sections.push_back(current);
        }

        file.close();
        return true;
    }

    // Get value from section
    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "") {
        for (const auto& sec : sections) {
            if (sec.name == section) {
                auto it = sec.values.find(key);
                if (it != sec.values.end()) {
                    return it->second;
                }
            }
        }
        return defaultValue;
    }

    // Get boolean value
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) {
        std::string value = getValue(section, key, defaultValue ? "true" : "false");
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "yes" || value == "1" || value == "on";
    }

    // Get integer value
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) {
        std::string value = getValue(section, key);
        try {
            return std::stoi(value);
        } catch (...) {
            return defaultValue;
        }
    }

    // Get float value
    float getFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) {
        std::string value = getValue(section, key);
        try {
            return std::stof(value);
        } catch (...) {
            return defaultValue;
        }
    }

    // Get all sections
    const std::vector<ConfigSection>& getSections() const {
        return sections;
    }

    // Check if section exists
    bool hasSection(const std::string& section) const {
        for (const auto& sec : sections) {
            if (sec.name == section) {
                return true;
            }
        }
        return false;
    }

    // Check if key exists in section
    bool hasKey(const std::string& section, const std::string& key) const {
        for (const auto& sec : sections) {
            if (sec.name == section) {
                return sec.values.find(key) != sec.values.end();
            }
        }
        return false;
    }

    // Load configuration into Core Config object
    bool loadToConfig(std::shared_ptr<Config> config) {
        if (!config) return false;

        for (const auto& section : sections) {
            for (const auto& pair : section.values) {
                const std::string& key = pair.first;
                const std::string& value = pair.second;

                // Try to parse as different types based on key name or content
                bool success = false;
                
                // Try integer (keys ending with _count, _steps, etc.)
                if (endsWith(key, "_count") || endsWith(key, "_steps") || endsWith(key, "_neurons") || 
                    endsWith(key, "_trials") || endsWith(key, "_episodes") || endsWith(key, "_trials") ||
                    endsWith(key, "_iterations") || endsWith(key, "_events") || endsWith(key, "_nodes")) {
                    try {
                        int64_t intValue = std::stoll(value);
                        config->set(key, intValue, ConfigSource::File);
                        success = true;
                    } catch (...) {}
                }

                // Try float (keys ending with _rate, _weight, _probability, etc.)
                if (!success && (endsWith(key, "_rate") || endsWith(key, "_weight") || 
                               endsWith(key, "_probability") || endsWith(key, "_sensitivity") ||
                               endsWith(key, "_factor") || endsWith(key, "_alpha") || endsWith(key, "_threshold") ||
                               endsWith(key, "_speed") || endsWith(key, "_time") || endsWith(key, "_temperature") ||
                               endsWith(key, "_size") || endsWith(key, "_width") || endsWith(key, "_height"))) {
                    try {
                        float floatValue = std::stof(value);
                        config->set(key, floatValue, ConfigSource::File);
                        success = true;
                    } catch (...) {}
                }

                // Try boolean (keys ending with _enable, _disable, _enabled, etc.)
                if (!success && (endsWith(key, "_enable") || endsWith(key, "_disable") ||
                               endsWith(key, "_enabled") || endsWith(key, "_disabled") ||
                               endsWith(key, "_enabled") || endsWith(key, "_check") ||
                               endsWith(key, "_profiling") || endsWith(key, "_verbose") ||
                               endsWith(key, "_debug") || endsWith(key, "_quiet"))) {
                    bool boolValue = getBool(section.name, key);
                    config->set(key, boolValue, ConfigSource::File);
                    success = true;
                }

                // Default to string if none of the above worked
                if (!success) {
                    config->set(key, value, ConfigSource::File);
                }
            }
        }

        return true;
    }

private:
    // Helper function to check if string ends with suffix
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
};

} // namespace

} // namespace nlm

#endif // NLM_CONFIG_FILE_H