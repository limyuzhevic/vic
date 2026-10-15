#pragma once

#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class ParameterParser {
public:
    ParameterParser() {
    }

    // Parse parameters from string (key=value format)
    std::map<std::string, std::string> parseString(const std::string& paramString) const;

    // Parse parameters from JSON
    std::map<std::string, std::string> parseJSON(const std::string& jsonString) const;

    // Parse parameters from command line
    std::map<std::string, std::string> parseCommandLine(int argc, char** argv) const;

    // Parse parameters from file
    std::map<std::string, std::string> parseFile(const std::string& filepath) const;

    // Validate parameter values against expected types
    bool validateParameter(const std::string& key, const std::string& value, const std::string& typeHint = "auto") const;

    // Get parameter type hint
    std::string getParameterType(const std::string& key, const std::map<std::string, std::string>& parameters) const;

private:
    std::map<std::string, std::string> parseKeyValuePairs(const std::string& input) const;
};

} // namespace nlm