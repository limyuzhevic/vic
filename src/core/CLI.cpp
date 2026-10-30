#include "CLI.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLIParser::CLIParser() : showHelp_(false), showVersion_(false), argIndex_(0) {
}

void CLIParser::parse(int argc, char** argv) {
    args_.clear();
    flags_.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            showHelp_ = true;
        } else if (arg == "--version") {
            showVersion_ = true;
        } else if (arg.find("--") == 0) {
            // Flag with optional value (--flag value or --flag=value)
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string flag = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                flags_[flag] = value;
            } else {
                flags_[arg] = "";
            }
        } else if (arg.find('-') == 0 && arg.length() == 2) {
            // Short flag (-f)
            flags_[arg] = "";
        } else {
            // Positional argument
            args_.push_back(arg);
        }
    }
    
    // First positional argument is mode
    if (!args_.empty()) {
        mode_ = args_[0];
        args_.erase(args_.begin());
    }
}

bool CLIParser::hasFlag(const std::string& flag) const {
    return flags_.find(flag) != flags_.end();
}

std::string CLIParser::getFlagValue(const std::string& flag) const {
    auto it = flags_.find(flag);
    if (it != flags_.end()) {
        return it->second;
    }
    return "";
}

std::string CLIParser::getValue(const std::string& key) const {
    for (const auto& arg : args_) {
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos && arg.substr(0, equalsPos) == key) {
            return arg.substr(equalsPos + 1);
        }
    }
    return "";
}

int CLIParser::getIntValue(const std::string& key, int defaultValue) const {
    std::string value = getValue(key);
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

float CLIParser::getFloatValue(const std::string& key, float defaultValue) const {
    std::string value = getValue(key);
    if (value.empty()) {
        return defaultValue;
    }
    try {
        return std::stof(value);
    } catch (...) {
        return defaultValue;
    }
}

std::string CLIParser::getMode() const {
    return mode_;
}

bool CLIParser::showHelp() const {
    return showHelp_;
}

bool CLIParser::showVersion() const {
    return showVersion_;
}