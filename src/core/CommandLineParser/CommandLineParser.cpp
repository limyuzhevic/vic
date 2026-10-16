#include "CommandLineParser.hpp"

namespace nlm {

CommandLineParser::CommandLineParser(const std::string& programName) 
    : programName(programName) {}

void CommandLineParser::addOption(const std::string& name, const std::string& description, bool required, bool hasValue, const std::string& defaultValue, const std::string& type, const std::string& example) {
    options.push_back(Option(name, description, required, hasValue, defaultValue, type, example));
}

void CommandLineParser::addOption(const std::string& name, const std::string& shortName, const std::string& description, bool required, bool hasValue, const std::string& defaultValue, const std::string& type, const std::string& example) {
    options.push_back(Option(name, shortName, description, required, hasValue, defaultValue, type, example));
}

bool CommandLineParser::parse(int argc, char** argv) {
    remainingArgs.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                key = trim(key);
                
                bool found = false;
                for (const auto& option : options) {
                    if (option.name == key && option.hasValue) {
                        if (parseValue(value, option)) {
                            found = true;
                        } else {
                            std::cerr << "Error: Invalid value '" << value << "' for option --" << option.name << "\n";
                            return false;
                        }
                        break;
                    }
                }
                
                if (!found) {
                    std::cerr << "Error: Unknown option --" << key << "\n";
                    return false;
                }
            } else {
                std::string key = arg.substr(2);
                key = trim(key);
                
                bool found = false;
                for (const auto& option : options) {
                    if (option.name == key) {
                        if (option.hasValue) {
                            if (i + 1 >= argc) {
                                std::cerr << "Error: Option --" << option.name << " requires a value\n";
                                return false;
                            }
                            
                            std::string value = argv[++i];
                            if (parseValue(value, option)) {
                                found = true;
                            } else {
                                std::cerr << "Error: Invalid value '" << value << "' for option --" << option.name << "\n";
                                return false;
                            }
                        } else {
                            found = true;
                            boolOptions[key] = true;
                        }
                        break;
                    }
                }
                
                if (!found) {
                    std::cerr << "Error: Unknown option --" << key << "\n";
                    return false;
                }
            }
        } else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            key = trim(key);
            
            bool found = false;
            for (const auto& option : options) {
                if (option.shortName == key) {
                    found = true;
                    if (option.hasValue) {
                        if (i + 1 >= argc) {
                            std::cerr << "Error: Option -" << option.shortName << " requires a value\n";
                            return false;
                        }
                        
                        std::string value = argv[++i];
                        if (parseValue(value, option)) {
                            found = true;
                        } else {
                            std::cerr << "Error: Invalid value '" << value << "' for option -" << option.shortName << "\n";
                            return false;
                        }
                    } else {
                        boolOptions[key] = true;
                    }
                    break;
                }
            }
            
            if (!found) {
                std::cerr << "Error: Unknown option -" << key << "\n";
                return false;
            }
        } else {
            remainingArgs.push_back(arg);
        }
    }
    
    return true;
}

bool CommandLineParser::hasOption(const std::string& name) const {
    std::string normalized = trim(name);
    return stringOptions.find(normalized) != stringOptions.end() ||
           boolOptions.find(normalized) != boolOptions.end();
}

template<typename T>
T CommandLineParser::getOption(const std::string& name, const T& defaultValue) const {
    std::string normalized = trim(name);
    
    if (stringOptions.find(normalized) != stringOptions.end()) {
        try {
            return parseValueToType<T>(stringOptions.at(normalized));
        } catch (...) {
            return defaultValue;
        }
    }
    
    if (boolOptions.find(normalized) != boolOptions.end()) {
        return static_cast<T>(boolOptions.at(normalized));
    }
    
    return defaultValue;
}

std::string CommandLineParser::getOptionString(const std::string& name, const std::string& defaultValue) const {
    std::string normalized = trim(name);
    
    if (stringOptions.find(normalized) != stringOptions.end()) {
        return stringOptions.at(normalized);
    }
    
    return defaultValue;
}

int CommandLineParser::getOptionInt(const std::string& name, int defaultValue) const {
    return getOption<int>(name, defaultValue);
}

double CommandLineParser::getOptionDouble(const std::string& name, double defaultValue) const {
    return getOption<double>(name, defaultValue);
}

bool CommandLineParser::getOptionBool(const std::string& name, bool defaultValue) const {
    return getOption<bool>(name, defaultValue);
}

void CommandLineParser::printHelp() const {
    std::cout << "Usage: " << programName << " [OPTIONS]... [ARGS]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    for (const auto& option : options) {
        std::cout << "  ";
        
        if (!option.shortName.empty()) {
            std::cout << "-" << option.shortName << ", ";
        }
        
        std::cout << "--" << option.name;
        
        if (option.hasValue) {
            std::cout << "=" << option.type << "";
            if (!option.defaultValue.empty()) {
                std::cout << " (default: " << option.defaultValue << ")";
            }
        }
        
        std::cout << std::endl;
        
        if (!option.example.empty()) {
            std::cout << "    Example: " << option.example << std::endl;
        }
        
        std::cout << "    " << option.description << std::endl << std::endl;
    }
}

bool CommandLineParser::parseValue(const std::string& valueStr, const Option& option) {
    std::string normalizedValue = trim(valueStr);
    
    if (option.type == "int") {
        try {
            int value = std::stoi(normalizedValue);
            if (option.hasValue) {
                stringOptions[option.name] = normalizedValue;
            }
            return true;
        } catch (...) {
            return false;
        }
    } else if (option.type == "double") {
        try {
            double value = std::stod(normalizedValue);
            if (option.hasValue) {
                stringOptions[option.name] = normalizedValue;
            }
            return true;
        } catch (...) {
            return false;
        }
    } else if (option.type == "bool") {
        bool value = strToBool(normalizedValue);
        if (option.hasValue) {
            stringOptions[option.name] = normalizedValue;
        }
        return true;
    } else {
        if (option.hasValue) {
            stringOptions[option.name] = normalizedValue;
        }
        return true;
    }
}

bool CommandLineParser::strToBool(const std::string& value) const {
    std::string lower = toLower(value);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

std::string CommandLineParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string CommandLineParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

template<typename T>
T CommandLineParser::parseValueToType(const std::string& value) const {
    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return strToBool(value);
    } else {
        return T();
    }
}

} // namespace nlm
