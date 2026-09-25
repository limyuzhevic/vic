#include "ConfigParser.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace nlm {

ConfigParser::ParseResult ConfigParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return {false, "Cannot open file: " + filepath, Format::Unknown, nullptr};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str());
}

ConfigParser::ParseResult ConfigParser::parseString(const std::string& content, Format format) {
    if (format == Format::Unknown) {
        format = detectFormat(content);
    }

    switch (format) {
        case Format::Simple:
            return trySimpleFormat(content);
        case Format::JSON:
            return tryJSONFormat(content);
        case Format::YAML:
            return tryYAMLFormat(content);
        default:
            return {false, "Unsupported or unknown format", format, nullptr};
    }
}

ConfigParser::Format ConfigParser::detectFormat(const std::string& content) {
    if (content.empty()) {
        return Format::Unknown;
    }

    // Trim whitespace for detection
    std::string trimmed = content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

    if (trimmed.empty()) {
        return Format::Unknown;
    }

    // Check for JSON
    if ((trimmed.front() == '{' && trimmed.back() == '}') ||
        (trimmed.front() == '[' && trimmed.back() == ']') ||
        trimmed.find("{") != std::string::npos && trimmed.find("}") != std::string::npos) {
        return Format::JSON;
    }

    // Check for YAML
    if (trimmed.find(':') != std::string::npos && 
        (trimmed.find('-') != std::string::npos || trimmed.find('\n') != std::string::npos)) {
        return Format::YAML;
    }

    // Check for simple key=value format
    if (trimmed.find('=') != std::string::npos && 
        (trimmed.find(' ') != std::string::npos || trimmed.find('\n') != std::string::npos)) {
        return Format::Simple;
    }

    return Format::Unknown;
}

std::string ConfigParser::formatToString(Format format) {
    switch (format) {
        case Format::Unknown: return "Unknown";
        case Format::Simple: return "Simple";
        case Format::JSON: return "JSON";
        case Format::YAML: return "YAML";
        default: return "Unknown";
    }
}

ConfigParser::ParseResult ConfigParser::trySimpleFormat(const std::string& content) {
    nlohmann::json result = nlohmann::json::object();
    std::string line;
    std::istringstream stream(content);

    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty() || line.front() == '#' || line.front() == '/') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Trim whitespace
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            key.erase(0, key.find_first_not_of(" \t\r\n"));

            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);

            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            // Try to parse value as number or boolean
            try {
                if (value.find('.') != std::string::npos) {
                    float floatVal = std::stof(value);
                    result[key] = floatVal;
                } else {
                    try {
                        int intVal = std::stoi(value);
                        result[key] = intVal;
                    } catch (...) {
                        if (value == "true" || value == "false") {
                            bool boolVal = (value == "true");
                            result[key] = boolVal;
                        } else {
                            result[key] = value;
                        }
                    }
                }
            } catch (...) {
                result[key] = value;
            }
        }
    }

    return {true, "", Format::Simple, result};
}

ConfigParser::ParseResult ConfigParser::tryJSONFormat(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        return {true, "", Format::JSON, json};
    } catch (const std::exception& e) {
        return {false, "JSON parsing error: " + std::string(e.what()), Format::JSON, nullptr};
    }
}

ConfigParser::ParseResult ConfigParser::tryYAMLFormat(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        // Convert YAML to nlohmann::json
        nlohmann::json json;

        if (yaml.IsMap()) {
            for (const auto& pair : yaml) {
                const std::string& key = pair.first.as<std::string>();
                const YAML::Node& value = pair.second;
                json[key] = value;
            }
        }

        return {true, "", Format::YAML, json};
    } catch (const std::exception& e) {
        return {false, "YAML parsing error: " + std::string(e.what()), Format::YAML, nullptr};
    }
}

} // namespace nlm