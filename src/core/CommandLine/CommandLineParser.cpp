#include "CommandLineParser.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iterator>

namespace nlm {

CommandLineParser::CommandLineParser() {
    // Initialize parser with default values from options
    for (const auto& pair : options_) {
        const auto& info = pair.second;
        if (!info.defaultValue.empty()) {
            ArgumentValue& val = values_[pair.first];
            if (parseValue(info.defaultValue, val)) {
                val.isSet = true;
            }
        }
    }
}

bool CommandLineParser::parse(int argc, char** argv) {
    if (!argv || argc < 1) {
        return false;
    }
    
    // Parse each argument
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue; // Skip null pointers
        }
        
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            if (!parseArg(arg)) {
                return false;
            }
        }
        // Handle -key value format  
        else if (arg.size() > 1 && arg[0] == '-' && i + 1 < argc && argv[i + 1]) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Remove leading dash from key if it's a single letter
            if (key.size() == 1 && std::isalpha(key[0])) {
                key = "-" + key;
            }
            
            // Set option value
            set(key, value);
        }
        // Positional argument
        else {
            positionalArgs_.push_back(arg);
        }
    }
    
    return true;
}

void CommandLineParser::addOption(const std::string& name, const std::string& description,
                                 const std::string& usage,
                                 const std::string& defaultValue,
                                 bool hasValue, bool isHidden) {
    std::string normalizedName = normalizeName(name);
    options_[normalizedName] = CommandInfo(name, description, usage, defaultValue, hasValue, isHidden);
}

void CommandLineParser::set(const std::string& name, const std::string& value) {
    std::string normalizedName = normalizeName(name);
    
    // Check if option exists
    auto it = options_.find(normalizedName);
    if (it == options_.end()) {
        return; // Unknown option, ignore
    }
    
    // Parse value and store
    ArgumentValue& target = values_[normalizedName];
    if (parseValue(value, target)) {
        target.isSet = true;
    }
}

std::string CommandLineParser::getString(const std::string& name, const std::string& defaultValue) const {
    std::string normalizedName = normalizeName(name);
    
    auto it = values_.find(normalizedName);
    if (it == values_.end()) {
        return defaultValue;
    }
    
    return it->second.asString;
}

int CommandLineParser::getInt(const std::string& name, int defaultValue) const {
    std::string normalizedName = normalizeName(name);
    
    auto it = values_.find(normalizedName);
    if (it == values_.end()) {
        return defaultValue;
    }
    
    const auto& value = it->second;
    
    // Try to convert string to int
    try {
        return std::stoi(value.asString);
    } catch (...) {
        return defaultValue;
    }
}

bool CommandLineParser::getBool(const std::string& name, bool defaultValue) const {
    std::string normalizedName = normalizeName(name);
    
    auto it = values_.find(normalizedName);
    if (it == values_.end()) {
        return defaultValue;
    }
    
    const auto& value = it->second.asString;
    
    // Convert string to bool
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
}

bool CommandLineParser::has(const std::string& name) const {
    std::string normalizedName = normalizeName(name);
    return values_.find(normalizedName) != values_.end() && values_.at(normalizedName).isSet;
}

std::vector<std::string> CommandLineParser::getOptionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : options_) {
        if (!pair.second.isHidden) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::string CommandLineParser::getHelp() const {
    std::ostringstream oss;
    
    oss << "NLM Command Line Options:" << std::endl << std::endl;
    
    for (const auto& pair : options_) {
        const auto& info = pair.second;
        if (info.isHidden) continue;
        
        oss << "  ";
        if (info.name.substr(0, 2) == "--") {
            oss << info.name;
        } else {
            oss << "-" << info.name;
        }
        
        if (info.hasValue) {
            oss << " <value>";
        }
        oss << std::endl;
        
        oss << "    " << info.description << std::endl;
        
        if (!info.usage.empty()) {
            oss << "    Usage: " << info.usage << std::endl;
        }
        if (!info.defaultValue.empty()) {
            oss << "    Default: " << info.defaultValue << std::endl;
        }
        oss << std::endl;
    }
    
    if (!positionalArgs_.empty()) {
        oss << "Positional Arguments:" << std::endl;
        for (size_t i = 0; i < positionalArgNames_.size(); ++i) {
            oss << "  " << positionalArgNames_[i];
            if (i < positionalArgs_.size()) {
                oss << " = " << positionalArgs_[i];
            }
            oss << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> CommandLineParser::getPositionalArgs() const {
    return positionalArgs_;
}

void CommandLineParser::addPositionalArg(const std::string& name, const std::string& description) {
    positionalArgNames_.push_back(name);
}

void CommandLineParser::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName;
    if (!usageLine_.empty()) {
        std::cout << " " << usageLine_;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& pair : options_) {
        const auto& info = pair.second;
        if (info.isHidden) continue;
        
        std::cout << "  ";
        if (info.name.substr(0, 2) == "--") {
            std::cout << info.name;
        } else {
            std::cout << "-" << info.name;
        }
        
        if (info.hasValue) {
            std::cout << " <value>";
        }
        std::cout << std::endl;
        
        std::cout << "    " << info.description << std::endl;
        if (!info.usage.empty()) {
            std::cout << "    " << info.usage << std::endl;
        }
    }
}

void CommandLineParser::printVersion(const std::string& version) {
    std::cout << "NLM " << version << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "An Experimental Artificial Developmental Brain" << std::endl;
}

bool CommandLineParser::parseArg(const std::string& arg) {
    // Find equals sign
    size_t pos = arg.find('=');
    if (pos == std::string::npos) {
        // Just a flag (no value)
        std::string key = arg.substr(2);
        if (!key.empty()) {
            set(key, "true");
        }
        return true;
    }
    
    // Has value
    std::string key = arg.substr(2, pos - 2);
    std::string value = arg.substr(pos + 1);
    
    if (key.empty() || value.empty()) {
        return false;
    }
    
    set(key, value);
    return true;
}

bool CommandLineParser::parseValue(const std::string& value, ArgumentValue& target) const {
    if (value.empty()) {
        return false;
    }
    
    // Treat as string for now (simpler implementation)
    target.asString = value;
    return true;
}

std::string CommandLineParser::normalizeName(const std::string& name) {
    std::string result = name;
    // Convert dashes to underscores
    std::replace(result.begin(), result.end(), '-', '_');
    std::replace(result.begin(), result.end(), '.', '_');
    // Convert to lowercase
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm