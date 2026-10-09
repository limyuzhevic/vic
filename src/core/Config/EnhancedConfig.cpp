#include "EnhancedConfig.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <unordered_set>

namespace nlm {

namespace json_utils {
    bool JSONParser::parseJSON(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        // Simple JSON parser - looks for objects { "key": "value", "key2": 42 }
        entries.clear();
        size_t pos = 0;
        
        if (jsonStr.empty() || jsonStr[0] != '{') {
            return false;
        }
        
        pos = 1; // Skip opening brace
        
        while (pos < jsonStr.size()) {
            // Skip whitespace
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] == '}') {
                break; // End of object
            }
            
            // Parse key
            std::string key = extractString(jsonStr, pos);
            if (key.empty()) {
                return false;
            }
            
            // Skip whitespace and colon
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false;
            }
            pos++;
            
            // Skip whitespace after colon
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            // Parse value
            ConfigValue value;
            bool success = false;
            
            if (pos < jsonStr.size() && jsonStr[pos] == '"') {
                // String value
                value = extractString(jsonStr, pos);
                success = true;
            } else if (pos < jsonStr.size() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-')) {
                // Number value
                size_t endPos = pos;
                while (endPos < jsonStr.size() && (std::isdigit(jsonStr[endPos]) || jsonStr[endPos] == '.' || jsonStr[endPos] == '-' || jsonStr[endPos] == 'e' || jsonStr[endPos] == 'E')) {
                    endPos++;
                }
                std::string numStr = jsonStr.substr(pos, endPos - pos);
                value = std::stod(numStr);
                success = true;
                pos = endPos;
            } else if (pos < jsonStr.size() && jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                // Boolean value
                if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "true") {
                    value = true;
                    pos += 4;
                    success = true;
                } else if (pos + 5 <= jsonStr.size() && jsonStr.substr(pos, 5) == "false") {
                    value = false;
                    pos += 5;
                    success = true;
                }
            }
            
            if (!success) {
                return false;
            }
            
            // Add entry
            entries.emplace_back(key, value, ConfigSource::File);
            
            // Skip whitespace and comma
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++;
            }
        }
        
        return true;
    }
    
    std::string JSONParser::toJSON(const std::vector<ConfigEntry>& entries) {
        std::ostringstream oss;
        oss << "{\n";
        
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            oss << "  \"" << entry.key << "\": ";
            
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << arg[j];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << arg[j];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << "\"" << arg[j] << "\"";
                    }
                    oss << "]";
                }
            }, entry.value);
            
            if (i < entries.size() - 1) {
                oss << ",\n";
            }
        }
        
        oss << "\n}\n";
        return oss.str();
    }
    
    bool JSONParser::parseYAML(const std::string& yamlStr, std::vector<ConfigEntry>& entries) {
        // Simple YAML parser for key-value pairs
        entries.clear();
        std::istringstream stream(yamlStr);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Trim whitespace
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key: value
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on type
            ConfigValue value;
            bool success = false;
            
            // Try to parse as boolean
            if (valueStr == "true") {
                value = true;
                success = true;
            } else if (valueStr == "false") {
                value = false;
                success = true;
            }
            // Try to parse as integer
            else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit)) {
                try {
                    size_t pos;
                    int64_t intVal = std::stoll(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        value = intVal;
                        success = true;
                    }
                } catch (...) {}
            }
            // Try to parse as double
            else {
                try {
                    size_t pos;
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        value = doubleVal;
                        success = true;
                    }
                } catch (...) {}
            }
            
            // If parsing failed, treat as string
            if (!success) {
                value = valueStr;
                success = true;
            }
            
            if (success) {
                entries.emplace_back(key, value, ConfigSource::File);
            }
        }
        
        return !entries.empty();
    }
    
    std::string JSONParser::toYAML(const std::vector<ConfigEntry>& entries) {
        std::ostringstream oss;
        
        for (const auto& entry : entries) {
            oss << entry.key << ": ";
            
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << arg[j];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << arg[j];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << "\"" << arg[j] << "\"";
                    }
                    oss << "]";
                }
            }, entry.value);
            
            oss << "\n";
        }
        
        return oss.str();
    }
    
    std::string JSONParser::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool JSONParser::isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
    
    std::string JSONParser::extractString(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '"') {
            return "";
        }
        
        pos++; // Skip opening quote
        std::string result;
        
        while (pos < str.size()) {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                // Escape sequence
                pos++; // Skip backslash
                if (pos >= str.size()) break;
                
                switch (str[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[pos]; break; // Unknown escape, keep as-is
                }
                pos++;
            } else if (str[pos] == '"') {
                pos++; // Skip closing quote
                return result;
            } else {
                result += str[pos];
                pos++;
            }
        }
        
        return result; // Unclosed string
    }
    
    std::string JSONParser::unescapeString(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case '"': result += '"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    case '/': result += '/'; i++; break;
                    case 'b': result += '\b'; i++; break;
                    case 'f': result += '\f'; i++; break;
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    default: result += str[i + 1]; i++; break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
}

namespace type_utils {
    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            }
        }, value);
        return oss.str();
    }
    
    std::optional<ConfigValue> stringToValue(const std::string& str, const std::string& typeHint) {
        // Try to parse based on type hint or content
        if (typeHint == "bool") {
            if (str == "true" || str == "false") {
                return str == "true";
            }
        } else if (typeHint == "int") {
            try {
                size_t pos;
                int64_t val = std::stoll(str, &pos);
                if (pos == str.size()) return val;
            } catch (...) {}
        } else if (typeHint == "double") {
            try {
                size_t pos;
                double val = std::stod(str, &pos);
                if (pos == str.size()) return val;
            } catch (...) {}
        } else if (typeHint == "string") {
            return str;
        }
        
        // Auto-detection
        if (str == "true" || str == "false") {
            return str == "true";
        }
        
        // Try integer
        try {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) return val;
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos == str.size()) return val;
        } catch (...) {}
        
        // Default to string
        return str;
    }
    
    bool validateValue(const ConfigValue& value, const std::string& validator) {
        if (validator.empty()) return true;
        
        if (validator.find("positive") != std::string::npos) {
            return validation_rules::isPositive(value);
        } else if (validator.find("nonnegative") != std::string::npos) {
            return validation_rules::isNonNegative(value);
        }
        
        return true; // No specific validation
    }
    
    std::string typeToString(const ConfigValue& value) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) return "string";
            else if constexpr (std::is_same_v<T, bool>) return "bool";
            else if constexpr (std::is_same_v<T, int64_t>) return "int64";
            else if constexpr (std::is_same_t<T, double>) return "double";
            else if constexpr (std::is_same_v<T, std::vector<int>>) return "int[]";
            else if constexpr (std::is_same_v<T, std::vector<double>>) return "double[]";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "string[]";
            else return "unknown";
        }, value);
        return "unknown";
    }
}

namespace env_utils {
    std::string prefixEnvVars(const std::vector<ConfigEntry>& entries) {
        std::ostringstream oss;
        oss << "Environment variable mapping:\n";
        
        for (const auto& entry : entries) {
            std::string envVar = "NLM_" + entry.key;
            std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
            oss << "  " << envVar << " -> " << entry.key << "\n";
        }
        
        return oss.str();
    }
    
    std::string envVarToKey(const std::string& envVar) {
        std::string key = envVar;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        if (key.rfind("nlm_", 0) == 0) {
            key = key.substr(4);
        }
        return key;
    }
}

namespace cli_utils {
    ParsedArgs parseCommandLine(int argc, char** argv) {
        ParsedArgs args;
        args.verbose = false;
        args.debug = false;
        args.help = false;
        args.version = false;
        args.verbose = false;
        args.debug = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version") {
                args.version = true;
            } else if (arg == "--verbose") {
                args.verbose = true;
            } else if (arg == "--debug") {
                args.debug = true;
            } else if (arg == "--dry-run") {
                args.dryRun = true;
            } else if (arg == "--validate") {
                args.validate = true;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else if (arg.substr(0, 3) == "--log") {
                if (arg.find('=') != std::string::npos) {
                    args.logLevel = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 10) == "--experiment") {
                if (arg.find('=') != std::string::npos) {
                    args.experiment = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=') - 2);
                std::string value = arg.substr(arg.find('=') + 1);
                args.overrides[key] = value;
            } else if (arg.substr(0, 2) == "-" && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                args.overrides[key] = value;
            }
        }
        
        return args;
    }
    
    void printHelp(const std::string& programName) {
        std::cout << "Usage: " << programName << " [OPTIONS]\n";
        std::cout << "\nNLM (Neural Learning Machine) - Brain Simulation Framework\n";
        std::cout << "\nCore Options:\n";
        std::cout << "  --help, -h                    Show this help message and exit\n";
        std::cout << "  --version                     Show version information and exit\n";
        std::cout << "  --verbose                     Enable verbose output\n";
        std::cout << "  --debug                       Enable debug mode with detailed logging\n";
        std::cout << "\nConfiguration Options:\n";
        std::cout << "  --config <file> or --config=<file>   Load configuration from file\n";
        std::cout << "  --log-level=<level>                 Set log level (debug, info, warning, error, critical)\n";
        std::cout << "  --dry-run                          Validate configuration without running\n";
        std::cout << "  --validate                         Validate configuration and exit\n";
        std::cout << "\nExperiment Options:\n";
        std::cout << "  --experiment=<name>                Run specific experiment (phase3, phase4, phase5, phase6)\n";
        std::cout << "\nConfiguration Overrides:\n";
        std::cout << "  --key=value                       Set configuration value\n";
        std::cout << "  -k value                          Short form for --key\n";
        std::cout << "\nPreset Configurations:\n";
        std::cout << "  --preset=research                  Research-focused configuration\n";
        std::cout << "  --preset=production                Production configuration\n";
        std::cout << "  --preset=development               Development configuration\n";
        std::cout << "  --preset=minimal                   Minimal configuration\n";
        std::cout << "\nExamples:\n";
        std::cout << "  " << programName << " --help\n";
        std::cout << "  " << programName << " --experiment=phase3\n";
        std::cout << "  " << programName << " --config=research_config.json --verbose\n";
        std::cout << "  " << programName << " --log-level=debug --dry-run\n";
    }
}

namespace config_helpers {
    Config createDefaultConfig() {
        Config config;
        config.loadPreset("default");
        return config;
    }
    
    Config createPhase2Config() {
        Config config;
        config.loadPreset("phase2");
        return config;
    }
    
    Config createPhase3Config() {
        Config config;
        config.loadPreset("phase3");
        return config;
    }
    
    Config createPhase4Config() {
        Config config;
        config.loadPreset("phase4");
        return config;
    }
    
    Config createMinimalConfig() {
        Config config;
        config.loadPreset("minimal");
        return config;
    }
    
    Config createResearchConfig() {
        Config config;
        config.loadPreset("research");
        return config;
    }
}

namespace validation_rules {
    bool isPositive(const ConfigValue& value) {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return arg > 0;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return std::all_of(arg.begin(), arg.end(), [](int v) { return v > 0; });
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return std::all_of(arg.begin(), arg.end(), [](double v) { return v > 0; });
            }
            return false;
        }, value);
    }
    
    bool isNonNegative(const ConfigValue& value) {
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return arg >= 0;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return std::all_of(arg.begin(), arg.end(), [](int v) { return v >= 0; });
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return std::all_of(arg.begin(), arg.end(), [](double v) { return v >= 0; });
            }
            return false;
        }, value);
    }
    
    bool isInRange(const ConfigValue& value, double min, double max) {
        return std::visit([min, max](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return arg >= min && arg <= max;
            }
            return false;
        }, value);
    }
    
    bool isStringLength(const ConfigValue& value, size_t minLen, size_t maxLen) {
        return std::visit([minLen, maxLen](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg.size() >= minLen && arg.size() <= maxLen;
            }
            return false;
        }, value);
    }
    
    bool matchesPattern(const ConfigValue& value, const std::string& pattern) {
        return std::visit([pattern](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::regex regex(pattern);
                return std::regex_match(arg, regex);
            }
            return false;
        }, value);
    }
    
    bool isValidEnum(const ConfigValue& value, const std::vector<std::string>& validValues) {
        return std::visit([validValues](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return std::find(validValues.begin(), validValues.end(), arg) != validValues.end();
            }
            return false;
        }, value);
    }
}

// Implementation of Config class
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool historyEnabled;
    std::vector<std::pair<std::chrono::system_clock::time_point, std::vector<ConfigEntry>>> history;
    std::map<std::string, std::string> presets;
    
    Impl() : historyEnabled(false) {
        // Initialize built-in presets
        presets["default"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.001,
            "development_pruning_rate": 0.0001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "log_level": "INFO",
            "log_to_file": false,
            "log_filename": "nlm.log",
            "max_simulation_steps": 10000,
            "simulation_time_limit": 0.0,
            "visualization_enabled": false,
            "visualization_update_rate": 30.0
        })";
        
        presets["phase2"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        })";
        
        presets["phase3"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.01,
            "neuron_count": 2000,
            "region_count": 3,
            "connection_probability": 0.05,
            "stdp_ltp_weight": 0.015,
            "stdp_ltd_weight": 0.013,
            "synaptogenesis_rate": 0.00005,
            "pruning_rate": 0.000005,
            "environment_width": 50,
            "environment_height": 50,
            "vision_width": 16,
            "vision_height": 16
        })";
        
        presets["production"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 10000,
            "region_count": 5,
            "connection_probability": 0.08,
            "plasticity_learning_rate": 0.005,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "log_level": "WARNING",
            "log_to_file": true,
            "log_filename": "nlm_production.log",
            "max_simulation_steps": 100000,
            "simulation_time_limit": 3600.0,
            "visualization_enabled": false
        })";
        
        presets["minimal"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.1,
            "log_level": "ERROR"
        })";
        
        presets["research"] = R"({
            "random_seed": 42,
            "simulation_timestep": 0.0005,
            "neuron_count": 5000,
            "region_count": 4,
            "connection_probability": 0.12,
            "plasticity_learning_rate": 0.02,
            "stdp_ltp_weight": 0.025,
            "stdp_ltd_weight": 0.02,
            "stdp_time_constant": 25.0,
            "development_synaptogenesis_rate": 0.002,
            "development_pruning_rate": 0.0002,
            "dopamine_baseline": 0.2,
            "reward_discount_factor": 0.95,
            "environment_width": 100,
            "environment_height": 100,
            "vision_width": 32,
            "vision_height": 32,
            "log_level": "DEBUG",
            "log_to_file": true,
            "log_filename": "nlm_research.log",
            "max_simulation_steps": 500000,
            "simulation_time_limit": 7200.0,
            "visualization_enabled": true,
            "visualization_update_rate": 60.0
        })";
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Add basic configuration entries
    const std::vector<std::pair<std::string, std::string>> defaultEntries = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"dopamine_baseline", "Dopamine baseline level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)"},
        {"log_to_file", "Log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit in seconds"},
        {"visualization_enabled", "Enable visualization"},
        {"visualization_update_rate", "Visualization update rate"},
        {"vision_width", "Vision width"},
        {"vision_height", "Vision height"},
        {"synaptogenesis_rate", "Synaptogenesis rate"},
        {"pruning_rate", "Pruning rate"},
        {"stdp_tau", "STDP tau parameter"}
    };
    
    for (const auto& entry : defaultEntries) {
        // Add basic defaults based on type hint
        std::string key = entry.first;
        std::string desc = entry.second;
        
        // Set default values based on key name
        if (key == "random_seed") set(key, 42, ConfigSource::Default, desc, "int64", false, "nonnegative");
        else if (key == "simulation_timestep") set(key, 0.001, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "neuron_count") set(key, 1000, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "region_count") set(key, 2, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "connection_probability") set(key, 0.1, ConfigSource::Default, desc, "double", false, "range=0-1");
        else if (key == "plasticity_learning_rate") set(key, 0.01, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "stdp_ltp_weight") set(key, 0.01, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "stdp_ltd_weight") set(key, 0.012, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "stdp_time_constant") set(key, 20.0, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "development_synaptogenesis_rate") set(key, 0.001, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "development_pruning_rate") set(key, 0.0001, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "dopamine_baseline") set(key, 0.1, ConfigSource::Default, desc, "double", false, "range=0-1");
        else if (key == "reward_discount_factor") set(key, 0.99, ConfigSource::Default, desc, "double", false, "range=0-1");
        else if (key == "environment_name") set(key, "GridWorld", ConfigSource::Default, desc, "string", false, "");
        else if (key == "environment_width") set(key, 10, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "environment_height") set(key, 10, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "log_level") set(key, std::string("INFO"), ConfigSource::Default, desc, "string", false, "enum=DEBUG,INFO,WARNING,ERROR,CRITICAL");
        else if (key == "log_to_file") set(key, false, ConfigSource::Default, desc, "bool", false, "");
        else if (key == "log_filename") set(key, std::string("nlm.log"), ConfigSource::Default, desc, "string", false, "");
        else if (key == "max_simulation_steps") set(key, 10000, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "simulation_time_limit") set(key, 0.0, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "visualization_enabled") set(key, false, ConfigSource::Default, desc, "bool", false, "");
        else if (key == "visualization_update_rate") set(key, 30.0, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "vision_width") set(key, 16, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "vision_height") set(key, 16, ConfigSource::Default, desc, "int64", false, "positive");
        else if (key == "synaptogenesis_rate") set(key, 0.0001, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "pruning_rate") set(key, 0.00001, ConfigSource::Default, desc, "double", false, "positive");
        else if (key == "stdp_tau") set(key, 20.0, ConfigSource::Default, desc, "double", false, "positive");
    }
}

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse based on file extension
        std::string ext = "";
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            ext = filepath.substr(dotPos + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        }
        
        if (ext == "json") {
            // For single JSON file parsing, we'll parse the whole file at once
            file.clear();
            file.seekg(0, std::ios::beg);
            std::stringstream buffer;
            buffer << file.rdbuf();
            return loadFromJSON(buffer.str(), source);
        } else if (ext == "yaml" || ext == "yml") {
            file.clear();
            file.seekg(0, std::ios::beg);
            std::stringstream buffer;
            buffer << file.rdbuf();
            return loadFromYAML(buffer.str(), source);
        } else {
            // Original key=value format for backward compatibility
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to parse value with type conversion
                ConfigValue configValue;
                bool success = false;
                
                // Try boolean
                if (value == "true") {
                    configValue = true;
                    success = true;
                } else if (value == "false") {
                    configValue = false;
                    success = true;
                }
                // Try integer
                else {
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(value, &pos);
                        if (pos == value.size()) {
                            configValue = intVal;
                            success = true;
                        }
                    } catch (...) {}
                    
                    if (!success) {
                        // Try double
                        try {
                            size_t pos;
                            double doubleVal = std::stod(value, &pos);
                            if (pos == value.size()) {
                                configValue = doubleVal;
                                success = true;
                            }
                        } catch (...) {}
                    }
                    
                    if (!success) {
                        // Default to string
                        configValue = value;
                        success = true;
                    }
                }
                
                if (success) {
                    // Get type hint from existing entry or infer from value
                    std::string typeHint = "";
                    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                          [&key](const ConfigEntry& e) { return e.key == key; });
                    if (it != pImpl->entries.end()) {
                        typeHint = it->typeHint;
                    }
                    
                    set(key, configValue, source, "", typeHint);
                }
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr, ConfigSource source) {
    return json_utils::JSONParser::parseJSON(jsonStr, pImpl->entries);
}

bool Config::loadFromYAML(const std::string& yamlStr, ConfigSource source) {
    return json_utils::JSONParser::parseYAML(yamlStr, pImpl->entries);
}

bool Config::loadFromEnvironment(ConfigSource source) {
    bool success = false;
    
    for (auto& entry : pImpl->entries) {
        std::string envVar = "NLM_" + entry.key;
        std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
        
        const char* envValue = std::getenv(envVar.c_str());
        if (envValue) {
            std::string valueStr(envValue);
            
            // Convert to appropriate type
            std::optional<ConfigValue> configValue = type_utils::stringToValue(valueStr, entry.typeHint);
            if (configValue) {
                entry.value = *configValue;
                entry.source = source;
                success = true;
            }
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv, ConfigSource source) {
    cli_utils::ParsedArgs args = cli_utils::parseCommandLine(argc, argv);
    
    // Load from config file if specified
    if (!args.configFile.empty()) {
        loadFromFile(args.configFile, source);
    }
    
    // Apply command line overrides
    for (const auto& override : args.overrides) {
        std::optional<ConfigValue> value = type_utils::stringToValue(override.second);
        if (value) {
            // Find existing entry or create new one
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&override](const ConfigEntry& e) { return e.key == override.first; });
            if (it != pImpl->entries.end()) {
                it->value = *value;
                it->source = source;
            } else {
                pImpl->entries.emplace_back(override.first, *value, source);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use YAML format for better readability
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        if (!entry.typeHint.empty()) {
            file << "# Type: " << entry.typeHint << "\n";
        }
        file << entry.key << ": ";
        
        // Write value with proper formatting
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

bool Config::saveToJSON(std::ostream& output) const {
    output << json_utils::JSONParser::toJSON(pImpl->entries);
    return true;
}

bool Config::saveToYAML(std::ostream& output) const {
    output << json_utils::JSONParser::toYAML(pImpl->entries);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw std::runtime_error("Required configuration key not found: " + key);
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& typeHint,
                 bool required, const std::string& validator) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) it->description = description;
        if (!typeHint.empty()) it->typeHint = typeHint;
        it->required = required;
        if (!validator.empty()) it->validator = validator;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, typeHint, required, validator);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, const std::string& typeHint,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, typeHint, required, validator);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, const std::string& typeHint,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, typeHint, required, validator);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, const std::string& typeHint,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, typeHint, required, validator);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, const std::string& typeHint,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, typeHint, required, validator);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::clear() {
    pImpl->entries.clear();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->source : ConfigSource::Default;
}

std::string Config::getTypeHint(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->typeHint : "";
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->description : "";
}

std::string Config::getValidator(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? it->validator : "";
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.required && !has(entry.key)) {
            errors.push_back("Required configuration key missing: " + entry.key);
        }
        
        if (!validateValue(entry)) {
            errors.push_back("Validation failed for configuration key: " + entry.key);
        }
    }
    
    return errors;
}

bool Config::validateValue(const ConfigEntry& entry) const {
    if (!type_utils::validateValue(entry.value, entry.validator)) {
        return false;
    }
    
    // Additional validation based on type hint
    if (entry.typeHint == "string") {
        if (!std::holds_alternative<std::string>(entry.value)) {
            return false;
        }
    } else if (entry.typeHint == "bool") {
        if (!std::holds_alternative<bool>(entry.value)) {
            return false;
        }
    } else if (entry.typeHint == "int64" || entry.typeHint == "int") {
        if (!std::holds_alternative<int64_t>(entry.value)) {
            return false;
        }
    } else if (entry.typeHint == "double") {
        if (!std::holds_alternative<double>(entry.value)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << type_utils::valueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::getSourceSummary() const {
    std::ostringstream oss;
    oss << "Configuration by source:\n";
    
    for (int source = 0; source < static_cast<int>(ConfigSource::Preset); ++source) {
        auto keys = getKeysBySource(static_cast<ConfigSource>(source));
        if (!keys.empty()) {
            oss << "  " << static_cast<int>(source) << ": " << keys.size() << " entries\n";
        }
    }
    
    return oss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
                       [&source](const ConfigEntry& e) { return e.source == source; });
}

bool Config::loadPreset(const std::string& presetName, ConfigSource source) {
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        return false;
    }
    
    std::string jsonStr = it->second;
    std::vector<ConfigEntry> presetEntries;
    
    if (!json_utils::JSONParser::parseJSON(jsonStr, presetEntries)) {
        return false;
    }
    
    // Clear existing entries and add preset entries
    pImpl->entries.clear();
    for (auto& entry : presetEntries) {
        entry.source = source;
        pImpl->entries.push_back(entry);
    }
    
    return true;
}

std::vector<std::string> Config::getAvailablePresets() {
    std::vector<std::string> presets;
    for (const auto& preset : Config::Impl().presets) {
        presets.push_back(preset.first);
    }
    return presets;
}

void Config::enableHistory(bool enable) {
    pImpl->historyEnabled = enable;
}

bool Config::hasHistory() const {
    return pImpl->historyEnabled && !pImpl->history.empty();
}

std::vector<std::chrono::system_clock::time_point> Config::getHistoryTimestamps() const {
    std::vector<std::chrono::system_clock::time_point> timestamps;
    for (const auto& entry : pImpl->history) {
        timestamps.push_back(entry.first);
    }
    return timestamps;
}

bool Config::restoreFromHistory(size_t index) {
    if (index >= pImpl->history.size()) {
        return false;
    }
    
    pImpl->entries = pImpl->history[index].second;
    return true;
}

bool Config::equals(const Config& other) const {
    return pImpl->entries.size() == other.pImpl->entries.size() &&
           std::equal(pImpl->entries.begin(), pImpl->entries.end(), other.pImpl->entries.begin(),
                     [](const ConfigEntry& a, const ConfigEntry& b) {
                         return a.key == b.key && a.value == b.value && a.source == b.source;
                     });
}

double Config::similarity(const Config& other) const {
    if (pImpl->entries.empty() && other.pImpl->entries.empty()) {
        return 1.0;
    }
    
    if (pImpl->entries.empty() || other.pImpl->entries.empty()) {
        return 0.0;
    }
    
    double matches = 0;
    double total = std::max(pImpl->entries.size(), other.pImpl->entries.size());
    
    for (const auto& entry1 : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                              [&entry1](const ConfigEntry& e) { return e.key == entry1.key; });
        if (it != other.pImpl->entries.end()) {
            if (entry1.value == it->value) {
                matches++;
            }
        }
    }
    
    return matches / total;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            // Override existing entry
            it->value = entry.value;
            it->source = source;
        } else {
            // Add new entry
            pImpl->entries.push_back(entry);
        }
    }
}

void Config::applyOverrides(const std::map<std::string, std::string>& overrides) {
    for (const auto& override : overrides) {
        std::optional<ConfigValue> value = type_utils::stringToValue(override.second);
        if (value) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&override](const ConfigEntry& e) { return e.key == override.first; });
            if (it != pImpl->entries.end()) {
                it->value = *value;
            } else {
                pImpl->entries.emplace_back(override.first, *value, ConfigSource::Runtime);
            }
        }
    }
}

std::vector<std::string> Config::suggestKeys(const std::string& partial) const {
    std::vector<std::string> suggestions;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(partial) != std::string::npos) {
            suggestions.push_back(entry.key);
        }
    }
    return suggestions;
}

std::vector<std::string> Config::getKeysByType(const std::string& typeHint) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.typeHint == typeHint) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getRequiredKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.required) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::string Config::getValueAsString(const ConfigEntry& entry) const {
    return type_utils::valueToString(entry.value);
}

ConfigValue Config::parseValueFromString(const std::string& str, const std::string& typeHint) const {
    auto value = type_utils::stringToValue(str, typeHint);
    return value.has_value() ? value.value() : ConfigValue(str);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace nlm
