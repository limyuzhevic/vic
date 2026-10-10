// Enhanced configuration help system
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

namespace nlm {

// Configuration help and documentation system
class ConfigHelpSystem {
public:
    static std::string getParameterHelp(const std::string& key, const ConfigSchema& schema) {
        const auto& param = schema.getParameter(key);
        if (param.key.empty()) return "Unknown parameter: '" + key + "'";
        
        std::stringstream ss;
        ss << "=== Parameter: " << key << " ===\n\n";
        
        // Basic info
        ss << "Type: ";
        switch (param.type) {
            case ConfigType::Integer: ss << "integer"; break;
            case ConfigType::Integer64: ss << "64-bit integer"; break;
            case ConfigType::Float: ss << "floating point"; break;
            case ConfigType::Boolean: ss << "boolean"; break;
            case ConfigType::String: ss << "string"; break;
            case ConfigType::StringList: ss << "string list"; break;
            case ConfigType::IntList: ss << "integer list"; break;
            case ConfigType::FloatList: ss << "float list"; break;
            default: ss << "unknown"; break;
        }
        ss << "\n";
        
        ss << "Description: " << param.description << "\n";
        ss << "Category: " << param.category << "\n";
        ss << "Default: " << getDefaultValueString(param) << "\n";
        
        // Validation hints
        ss << "\nValidation:" << "\n";
        if (param.positive) ss << "  ✓ Must be positive\n";
        if (param.nonNegative) ss << "  ✓ Must be non-negative\n";
        if (param.required) ss << "  ✓ Required parameter\n";
        if (param.inRange) {
            ss << "  ✓ Range: ";
            if (param.floatRange.has_value()) {
                ss << param.floatRange->first << " - " << param.floatRange->second;
            } else if (param.int64Range.has_value()) {
                ss << param.int64Range->first << " - " << param.int64Range->second;
            } else if (param.intRange.has_value()) {
                ss << param.intRange->first << " - " << param.intRange->second;
            }
            ss << "\n";
        }
        
        if (!param.validValues.empty()) {
            ss << "  ✓ Valid values: ";
            for (size_t i = 0; i < param.validValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "'" << param.validValues[i] << "'";
            }
            ss << "\n";
        }
        
        // Dependencies
        if (!param.dependencies.empty()) {
            ss << "Dependencies: ";
            for (size_t i = 0; i < param.dependencies.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "'" << param.dependencies[i] << "'";
            }
            ss << "\n";
        }
        
        // Exclusive keys
        if (!param.exclusiveKeys.empty()) {
            ss << "Mutually exclusive with: ";
            for (size_t i = 0; i < param.exclusiveKeys.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "'" << param.exclusiveKeys[i] << "'";
            }
            ss << "\n";
        }
        
        // Usage examples
        ss << "\nUsage examples:" << "\n";
        ss << "  " << getUsageExample(param) << "\n";
        
        return ss.str();
    }
    
    static std::string getUsageExample(const ConfigParameter& param) {
        std::stringstream ss;
        
        switch (param.type) {
            case ConfigType::Integer:
                ss << param.key << " = 42";
                break;
            case ConfigType::Float:
                ss << param.key << " = 3.14159";
                break;
            case ConfigType::Boolean:
                ss << param.key << " = true";
                break;
            case ConfigType::String:
                ss << param.key << " = \"example\"";
                break;
            case ConfigType::StringList:
                ss << param.key << " = [\"item1\", \"item2\", \"item3\"]";
                break;
            case ConfigType::IntList:
                ss << param.key << " = [1, 2, 3, 4, 5]";
                break;
            case ConfigType::FloatList:
                ss << param.key << " = [1.5, 2.5, 3.5]";
                break;
            default:
                ss << param.key << " = <value>";
        }
        
        return ss.str();
    }
    
    static std::string getDefaultValueString(const ConfigParameter& param) {
        std::stringstream ss;
        
        auto getValueString = [](const auto& val) -> std::string {
            std::stringstream s;
            s << val;
            return s.str();
        };
        
        switch (param.type) {
            case ConfigType::Integer:
                if (param.intDefault.has_value()) ss << getValueString(param.intDefault.value());
                else ss << "(none)";
                break;
            case ConfigType::Integer64:
                if (param.int64Default.has_value()) ss << getValueString(param.int64Default.value());
                else ss << "(none)";
                break;
            case ConfigType::Float:
                if (param.floatDefault.has_value()) ss << getValueString(param.floatDefault.value());
                else ss << "(none)";
                break;
            case ConfigType::Boolean:
                if (param.boolDefault.has_value()) ss << (param.boolDefault.value() ? "true" : "false");
                else ss << "(none)";
                break;
            case ConfigType::String:
                if (param.stringDefault.has_value()) ss << "\"" << getValueString(param.stringDefault.value()) << "\"";
                else ss << "(none)";
                break;
            case ConfigType::StringList:
                if (param.stringListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.stringListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << getValueString(param.stringListDefault.value()[i]) << "\"";
                    }
                    ss << "]";
                } else ss << "(none)";
                break;
            case ConfigType::IntList:
                if (param.intListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.intListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << getValueString(param.intListDefault.value()[i]);
                    }
                    ss << "]";
                } else ss << "(none)";
                break;
            case ConfigType::FloatList:
                if (param.floatListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.floatListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << getValueString(param.floatListDefault.value()[i]);
                    }
                    ss << "]";
                } else ss << "(none)";
                break;
            default:
                ss << "(none)";
        }
        
        return ss.str();
    }
    
    static std::string getCategoryHelp(const std::string& category, const ConfigSchema& schema) {
        std::stringstream ss;
        ss << "=== " << category << " Parameters ===\n\n";
        
        auto keys = schema.getKeysByCategory(category);
        for (const auto& key : keys) {
            const auto& param = schema.getParameter(key);
            ss << "  " << key << ": " << param.description << "\n";
            ss << "    Type: ";
            switch (param.type) {
                case ConfigType::Integer: ss << "integer"; break;
                case ConfigType::Float: ss << "float"; break;
                case ConfigType::Boolean: ss << "boolean"; break;
                case ConfigType::String: ss << "string"; break;
                default: ss << "unknown"; break;
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    static std::string getAllHelp(const ConfigSchema& schema) {
        std::stringstream ss;
        ss << "NLM Configuration Help\n";
        ss << "=======================\n\n";
        
        auto keys = schema.getKeys();
        std::map<std::string, std::vector<std::string>> categories;
        
        for (const auto& key : keys) {
            const auto& param = schema.getParameter(key);
            categories[param.category].push_back(key);
        }
        
        ss << "Available Categories:\n";
        for (const auto& pair : categories) {
            ss << "  " << pair.first << " (" << pair.second.size() << " parameters)\n";
        }
        ss << "\n";
        
        ss << "Use 'help <parameter>' for detailed information about a specific parameter.\n";
        ss << "Use 'category <name>' for all parameters in a category.\n";
        ss << "Use 'profiles' to see available configuration profiles.\n";
        
        return ss.str();
    }
    
    static std::string getProfileHelp() {
        std::stringstream ss;
        ss << "=== Configuration Profiles ===\n\n";
        ss << "The NLM configuration system supports several predefined profiles:\n\n";
        
        ss << "1. Research Profile\n";
        ss << "   - Optimized for research and experimentation\n";
        ss << "   - Higher precision settings\n";
        ss << "   - Detailed logging\n";
        ss << "   - Conservative plasticity rates\n\n";
        
        ss << "2. Simulation Profile\n";
        ss << "   - Balanced settings for general simulation\n";
        ss << "   - Moderate logging\n";
        ss << "   - Standard plasticity settings\n";
        ss << "   - Good performance vs accuracy tradeoff\n\n";
        
        ss << "3. Production Profile\n";
        ss << "   - Optimized for production environments\n";
        ss << "   - Minimal logging\n";
        ss << "   - Aggressive performance settings\n";
        ss << "   - Fast plasticity and development\n\n";
        
        ss << "4. Development Profile\n";
        ss << "   - Maximum debugging and logging\n";
        ss << "   - Slow simulation for debugging\n";
        ss << "   - Detailed error reporting\n";
        ss << "   - Safe defaults for experimentation\n\n";
        
        ss << "5. Benchmark Profile\n";
        ss << "   - Optimized for performance testing\n";
        ss << "   - Minimal configuration overhead\n";
        ss << "   - Fast simulation settings\n";
        ss << "   - Reduced logging overhead\n";
        
        return ss.str();
    }
};