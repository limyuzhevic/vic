#include "CLInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <cctype>

namespace nlm {

bool CLIParser::parse(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Help and version flags
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        } else if (arg == "--config-template") {
            std::cout << generateConfigTemplate();
            return false;
        } else if (arg == "--save-config") {
            if (i + 1 >= argc) {
                printError("--save-config requires a filename");
                return false;
            }
            std::string filename = argv[++i];
            if (!saveConfig(filename)) {
                return false;
            }
            return false;
        } else if (arg == "--load-config") {
            if (i + 1 >= argc) {
                printError("--load-config requires a filename");
                return false;
            }
            std::string filename = argv[++i];
            if (!loadConfig(filename)) {
                return false;
            }
            return false;
        }
        
        // Check for option flags (starting with --)
        if (arg.substr(0, 2) == "--") {
            std::string optName = arg.substr(2);
            size_t eqPos = optName.find('=');
            std::string value = "";
            
            if (eqPos != std::string::npos) {
                value = optName.substr(eqPos + 1);
                optName = optName.substr(0, eqPos);
            }
            
            bool found = false;
            for (const auto& option : options) {
                if (option.name == optName) {
                    found = true;
                    if (value.empty()) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            value = argv[++i];
                        }
                    }
                    
                    if (option.type == "string") {
                        stringArgs[optName] = value;
                    } else if (option.type == "int") {
                        try {
                            intArgs[optName] = std::stoi(value);
                        } catch (...) {
                            printError("Invalid integer value for option --" + optName);
                            return false;
                        }
                    } else if (option.type == "float") {
                        try {
                            doubleArgs[optName] = std::stod(value);
                        } catch (...) {
                            printError("Invalid float value for option --" + optName);
                            return false;
                        }
                    } else if (option.type == "bool") {
                        // Handle boolean flags (true/false)
                        if (value.empty()) {
                            boolArgs[optName] = true;
                        } else {
                            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                            boolArgs[optName] = (value == "true" || value == "1" || value == "yes");
                        }
                    }
                    break;
                }
            }
            
            if (!found) {
                printError("Unknown option: --" + optName);
                return false;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Handle short options (-f, -v, etc.)
            std::string shortOpt = arg.substr(1);
            
            for (size_t j = 0; j < shortOpt.size(); ++j) {
                char optChar = shortOpt[j];
                bool found = false;
                
                for (const auto& option : options) {
                    if (option.shortOpt == std::string(1, optChar)) {
                        found = true;
                        std::string optName = option.name;
                        
                        // Check if next argument is not an option
                        if (j + 1 < shortOpt.size() || i + 1 >= argc || argv[i + 1][0] == '-') {
                            boolArgs[optName] = true;
                        } else {
                            i++; // Skip next argument
                        }
                        break;
                    }
                }
                
                if (!found && shortOpt[j] != ' ' && shortOpt[j] != '=') {
                    printError("Unknown option: -" + std::string(1, optChar));
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::optional<std::string> CLIParser::getString(const std::string& name) const {
    auto it = stringArgs.find(name);
    if (it != stringArgs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<int> CLIParser::getInt(const std::string& name) const {
    auto it = intArgs.find(name);
    if (it != intArgs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<double> CLIParser::getDouble(const std::string& name) const {
    auto it = doubleArgs.find(name);
    if (it != doubleArgs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<bool> CLIParser::getBool(const std::string& name) const {
    auto it = boolArgs.find(name);
    if (it != boolArgs.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool CLIParser::getFlag(const std::string& name) const {
    auto it = flags.find(name);
    if (it != flags.end()) {
        auto argIt = boolArgs.find(name);
        if (argIt != boolArgs.end()) {
            return argIt->second;
        }
        return it->second.defaultValue;
    }
    return false;
}

void CLIParser::printHelp() const {
    printBanner(programName, description);
    
    std::cout << "\nUsage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << getUsage() << std::endl;
    
    printSection("OPTIONS");
    
    // Print options
    for (const auto& option : options) {
        std::cout << "  " << formatOption(option) << std::endl;
        std::cout << "    " << option.description << std::endl;
        if (!option.defaultValue.empty()) {
            std::cout << "    Default: " << option.defaultValue << std::endl;
        }
    }
    
    // Print flags
    if (!flags.empty()) {
        std::cout << "\nFlags:" << std::endl;
        for (const auto& flag : flags) {
            std::cout << "  --" << flag.first << std::endl;
            std::cout << "    " << flag.second.description << std::endl;
            std::cout << "    Default: " << (flag.second.defaultValue ? "enabled" : "disabled") << std::endl;
        }
    }
    
    printSection("EXAMPLES");
    std::cout << "  " << programName << " --config=my_config.cfg" << std::endl;
    std::cout << "  " << programName << " --neuron-count=1000 --simulation-timestep=0.0001" << std::endl;
    std::cout << "  " << programName << " --verbose --save-config=my_settings.cfg" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    
    printSection("CONFIGURATION");
    std::cout << "  " << programName << " --config-template" << std::endl;
    std::cout << "  " << programName << " --load-config=config.cfg --save-config=custom.cfg" << std::endl;
}

void CLIParser::printVersion() const {
    printBanner("NLM", "Neural Learning Machine - Version 0.1.0");
    std::cout << "\nBuild Information:" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Configuration: " << (NLM_PERFORMANCE_ENABLED ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  SIMD Support: " << NLM_SIMD_WIDTH << "x" << std::endl;
    std::cout << "  Threads: " << NLM_NUM_THREADS << std::endl;
    std::cout << "  Memory Pool: " << (NLM_USE_MEMORY_POOL ? "Enabled" : "Disabled") << std::endl;
}

void CLIParser::printBanner(const std::string& title, const std::string& subtitle) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    if (!title.empty()) {
        std::cout << "\n=== " << title << " ===" << std::endl;
        if (!subtitle.empty()) {
            std::cout << subtitle << std::endl;
        }
    }
}

void CLIParser::printSection(const std::string& title) {
    std::cout << "\n" << title << ":" << std::endl;
}

void CLIParser::printStatus(const std::string& label, const std::string& value, bool success) {
    std::string status = success ? "✓" : "✗";
    std::cout << "  " << status << " " << label << ": " << value << std::endl;
}

void CLIParser::printProgress(const std::string& message, int current, int total) {
    if (total > 0) {
        float progress = static_cast<float>(current) / total * 100.0f;
        std::cout << "  " << message << " " << std::fixed << std::setprecision(1) << progress << "% (" 
                  << current << "/" << total << ")" << std::endl;
    } else {
        std::cout << "  " << message << std::endl;
    }
}

bool CLIParser::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        printError("Cannot save config to file: " + filename);
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM CLI" << std::endl;
    file << std::endl;
    
    for (const auto& option : options) {
        std::optional<std::string> value;
        if (option.type == "string") {
            value = getString(option.name);
        } else if (option.type == "int") {
            auto intVal = getInt(option.name);
            if (intVal) {
                value = std::to_string(*intVal);
            }
        } else if (option.type == "float") {
            auto floatVal = getDouble(option.name);
            if (floatVal) {
                value = std::to_string(*floatVal);
            }
        }
        
        if (value) {
            file << "--" << option.name << "=" << *value << std::endl;
        }
    }
    
    for (const auto& flag : flags) {
        bool value = getFlag(flag.first);
        if (value) {
            file << "--" << flag.first << std::endl;
        }
    }
    
    std::cout << "Configuration saved to: " << filename << std::endl;
    return true;
}

bool CLIParser::loadConfig(const std::string& filename) {
    // For now, just parse command line args
    // In a full implementation, this would read from the config file
    std::cout << "Loading configuration from: " << filename << std::endl;
    std::cout << "(Config file loading implemented in full version)" << std::endl;
    return true;
}

std::string CLIParser::generateConfigTemplate() const {
    std::stringstream ss;
    ss << "# NLM Configuration Template" << std::endl;
    ss << "# Copy this to config files and modify values" << std::endl;
    ss << std::endl;
    
    for (const auto& option : options) {
        ss << "# " << option.description << std::endl;
        ss << "--" << option.name << "=" << option.defaultValue << std::endl;
        ss << std::endl;
    }
    
    ss << "# Flags (boolean options)" << std::endl;
    for (const auto& flag : flags) {
        ss << "# --" << flag.first << " (default: " << (flag.second.defaultValue ? "true" : "false") << ")" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

void CLIParser::printError(const std::string& message) const {
    std::cerr << "ERROR: " << message << std::endl;
}

std::string CLIParser::getUsage() const {
    return "\n";
}

std::string CLIParser::formatOption(const Option& option) const {
    std::stringstream ss;
    
    if (!option.shortOpt.empty()) {
        ss << "-" << option.shortOpt << ", ";
    }
    
    ss << "--" << option.name;
    
    if (!option.defaultValue.empty()) {
        ss << "=" << option.defaultValue;
    }
    
    return ss.str();
}

} // namespace nlm