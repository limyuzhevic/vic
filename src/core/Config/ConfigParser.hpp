#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <stdexcept>

// Include nlohmann JSON
#include "nlohmann/json.hpp"
// Include yaml-cpp
#include "yaml-cpp/yaml.h"

namespace nlm {

class ConfigParser {
public:
    enum class Format {
        Unknown,
        Simple,
        JSON,
        YAML
    };

    struct ParseResult {
        bool success;
        std::string errorMessage;
        Format detectedFormat;
        nlohmann::json parsedData;
    };

    static ParseResult parseFile(const std::string& filepath);
    static ParseResult parseString(const std::string& content, Format format = Format::Unknown);
    static Format detectFormat(const std::string& content);
    static std::string formatToString(Format format);

private:
    static ParseResult trySimpleFormat(const std::string& content);
    static ParseResult tryJSONFormat(const std::string& content);
    static ParseResult tryYAMLFormat(const std::string& content);
};

} // namespace nlm