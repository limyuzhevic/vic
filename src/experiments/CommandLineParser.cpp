// CommandLineParser implementation

#include "experiments/CommandLineParser.hpp"

using namespace nlm;

// Constructor
CommandLineParser::CommandLineParser(const std::string& name, const std::string& description)
    : programName(name), programDescription(description), parseSuccessful(false), verbose(false) {}

// Parse command-line arguments
bool CommandLineParser::parse(int argc, char* argv[]) {
    // Convert argv to vector of strings
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    parseSuccessful = true;
    errorMessage.clear();
    
    // Process arguments
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        
        // Check for help flag
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        
        // Check for verbose flag
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
            continue;
        }
        
        // Check for version flag
        if (arg == "--version") {
            printVersion();
            exit(0);
        }
        
        // Check for argument with value
        if (arg.starts_with("--")) {
            // Find argument definition
            bool found = false;
            for (const auto& argInfo : arguments) {
                if (argInfo.name == arg) {
                    found = true;
                    // Check if next argument is a value
                    if (i + 1 < args.size() && args[i + 1][0] != '-') {
                        // Store the value
                        parsedValues[arg] = args[i + 1];
                        ++i;
                    } else if (!argInfo.defaultValue.empty()) {
                        // Use default value
                        parsedValues[arg] = argInfo.defaultValue;
                    } else if (argInfo.required) {
                        // This shouldn't happen, but just in case
                        printError("Missing required argument: " + arg);
                        parseSuccessful = false;
                        return false;
                    }
                    break;
                }
            }
            if (!found) {
                printError("Unknown argument: " + arg);
                parseSuccessful = false;
                return false;
            }
        } else if (arg.starts_with("-")) {
            // Single character flag
            // Handle single character flags
            for (char c : arg.substr(1)) {
                std::string flag("-"); flag += c;
                // Check if flag is defined
                bool found = false;
                for (const auto& argInfo : arguments) {
                    if (argInfo.name == flag) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    printError("Unknown flag: " + flag);
                    parseSuccessful = false;
                    return false;
                }
                // For now, just mark as parsed
                flags.insert(flag);
            }
        } else {
            // Positional argument
            // Store positional arguments
            positionalArgs.push_back(arg);
        }
    }
    
    // Validate required arguments
    if (!validateArguments()) {
        return false;
    }
    
    parseSuccessful = true;
    return parseSuccessful;
}

// Check if a flag is set
bool CommandLineParser::isFlagSet(const std::string& flag) const {
    return flags.find(flag) != flags.end();
}

// Get string value from parsed arguments
std::string CommandLineParser::getString(const std::string& name) const {
    auto it = parsedValues.find(name);
    if (it != parsedValues.end()) {
        return it->second;
    }
    return "";
}

// Get integer value from parsed arguments
int CommandLineParser::getInt(const std::string& name) const {
    auto it = parsedValues.find(name);
    if (it != parsedValues.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

// Get float value from parsed arguments
float CommandLineParser::getFloat(const std::string& name) const {
    auto it = parsedValues.find(name);
    if (it != parsedValues.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return 0.0f;
        }
    }
    return 0.0f;
}

// Check if an argument has a value
bool CommandLineParser::hasValue(const std::string& name) const {
    return parsedValues.find(name) != parsedValues.end();
}

// Get positional arguments
const std::vector<std::string>& CommandLineParser::getPositionalArgs() const {
    return positionalArgs;
}

// Print help message
void CommandLineParser::printHelp() const {
    std::cout << "Usage: " << programName;
    for (const auto& arg : arguments) {
        if (arg.type == ArgumentType::POSITIONAL) {
            std::cout << " [" << arg.name << "]";
        }
    }
    std::cout << "\n\nOptions:\n";
    
    // Print all arguments
    for (const auto& arg : arguments) {
        std::cout << "  ";
        if (arg.type == ArgumentType::FLAG) {
            std::cout << "--" << arg.name;
        } else {
            std::cout << "--" << arg.name;
            if (arg.hasValue) {
                std::cout << "=<value>";
            }
        }
        std::cout << std::setw(20) << std::left << " " + arg.description;
        if (!arg.helpText.empty()) {
            std::cout << "\n      " << arg.helpText;
        }
        std::cout << "\n";
    }
    
    if (!programDescription.empty()) {
        std::cout << "\n" << programDescription << "\n";
    }
}

// Print version information
void CommandLineParser::printVersion() const {
    std::cout << programName << " version 1.0.0\n";
}

// Print error message
void CommandLineParser::printError(const std::string& message) const {
    std::cerr << "Error: " << message << std::endl;
    errorMessage = message;
}

// Validate parsed arguments
bool CommandLineParser::validateArguments() const {
    // Check for required arguments
    for (const auto& arg : arguments) {
        if (arg.required && !hasValue(arg.name)) {
            printError("Required argument missing: " + arg.name);
            return false;
        }
    }
    return true;
}

// Set error message
void CommandLineParser::setError(const std::string& message) {
    errorMessage = message;
}

// Get error message
const std::string& CommandLineParser::getErrorMessage() const {
    return errorMessage;
}

// Check if parsing was successful
bool CommandLineParser::isValid() const {
    return parseSuccessful;
}