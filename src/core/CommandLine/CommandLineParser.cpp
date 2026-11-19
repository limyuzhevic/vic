#include "CommandLineParser.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

struct CommandLineParser::Impl {
    std::unordered_map<std::string, Option> options;
    std::unordered_map<std::string, std::function<void(const std::string&)>> callbacks;
    std::unordered_map<std::string, std::string> values;
    std::vector<Option> optionList;
};

CommandLineParser::CommandLineParser() : pImpl_(std::make_unique<Impl>()), valid_(true) {}

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::addOption(const std::string& longName, char shortName,
                                 const std::string& description, bool hasArgument,
                                 const std::string& defaultValue, bool required) {
    Option option;
    option.longName = longName;
    option.shortName = shortName;
    option.description = description;
    option.hasArgument = hasArgument;
    option.defaultValue = defaultValue;
    option.required = required;
    
    pImpl_->options[longName] = option;
    pImpl_->optionList.push_back(option);
    
    // Set default value
    if (!defaultValue.empty()) {
        pImpl_->values[longName] = defaultValue;
    }
}

void CommandLineParser::setCallback(const std::string& longName,
                                   std::function<void(const std::string&)> callback) {
    pImpl_->callbacks[longName] = callback;
}

bool CommandLineParser::parse(int argc, char* argv[]) {
    unknownArgs_.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Long option
            std::string longName = arg.substr(2);
            auto it = pImpl_->options.find(longName);
            
            if (it == pImpl_->options.end()) {
                unknownArgs_.push_back(arg);
                continue;
            }
            
            Option& option = it->second;
            
            if (option.hasArgument) {
                if (i + 1 >= argc) {
                    std::cerr << "Error: Option --" << longName << " requires an argument" << std::endl;
                    valid_ = false;
                    return false;
                }
                
                std::string value = argv[++i];
                pImpl_->values[longName] = value;
                
                if (pImpl_->callbacks.find(longName) != pImpl_->callbacks.end()) {
                    pImpl_->callbacks[longName](value);
                }
            } else {
                // Boolean option, set to "true"
                pImpl_->values[longName] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option
            std::string shortName = arg.substr(1);
            auto it = pImpl_->options.begin();
            bool found = false;
            
            for (; it != pImpl_->options.end(); ++it) {
                if (it->second.shortName == shortName[0]) {
                    found = true;
                    std::string longName = it->first;
                    
                    if (it->second.hasArgument) {
                        if (shortName.size() > 1) {
                            // Combined value like -xvalue
                            std::string value = shortName.substr(1);
                            pImpl_->values[longName] = value;
                            
                            if (pImpl_->callbacks.find(longName) != pImpl_->callbacks.end()) {
                                pImpl_->callbacks[longName](value);
                            }
                        } else if (i + 1 >= argc) {
                            std::cerr << "Error: Option -" << shortName << " requires an argument" << std::endl;
                            valid_ = false;
                            return false;
                        } else {
                            std::string value = argv[++i];
                            pImpl_->values[longName] = value;
                            
                            if (pImpl_->callbacks.find(longName) != pImpl_->callbacks.end()) {
                                pImpl_->callbacks[longName](value);
                            }
                        }
                    } else {
                        pImpl_->values[longName] = "true";
                    }
                    break;
                }
            }
            
            if (!found) {
                unknownArgs_.push_back("-".append(shortName));
            }
        } else {
            unknownArgs_.push_back(arg);
        }
    }
    
    // Check for required options
    for (const auto& option : pImpl_->optionList) {
        if (option.required && pImpl_->values.find(option.longName) == pImpl_->values.end()) {
            std::cerr << "Error: Required option --" << option.longName << " is missing" << std::endl;
            valid_ = false;
            return false;
        }
    }
    
    return true;
}

std::string CommandLineParser::getOption(const std::string& longName) const {
    auto it = pImpl_->values.find(longName);
    if (it != pImpl_->values.end()) {
        return it->second;
    }
    
    // Check if option exists in options
    auto optionIt = pImpl_->options.find(longName);
    if (optionIt != pImpl_->options.end() && !optionIt->second.defaultValue.empty()) {
        return optionIt->second.defaultValue;
    }
    
    return "";
}

bool CommandLineParser::hasOption(const std::string& longName) const {
    return pImpl_->values.find(longName) != pImpl_->values.end();
}

void CommandLineParser::printHelp() const {
    std::cout << "Usage: program_name [options]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& option : pImpl_->optionList) {
        std::cout << "  ";
        if (option.shortName != '\0') {
            std::cout << "-" << option.shortName << ", ";
        }
        std::cout << "--" << option.longName;
        
        if (option.hasArgument) {
            std::cout << "=<value>";
        }
        
        std::cout << std::endl << "    " << option.description;
        
        if (!option.defaultValue.empty()) {
            std::cout << " (default: " << option.defaultValue << ")";
        }
        
        if (option.required) {
            std::cout << " (required)";
        }
        
        std::cout << std::endl << std::endl;
    }
}

} // namespace nlm
