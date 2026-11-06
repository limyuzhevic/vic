// NLM Command Line Argument Parser Implementation
// Implementation of the ArgumentParser class

#include "core/ArgumentParser.hpp"
#include <algorithm>
#include <iomanip>

namespace nlm {

ArgumentParser::ArgumentParser(const std::string& programName, 
                             const std::string& versionStr)
    : programName_(programName), versionStr_(versionStr), helpAdded_(false), versionAdded_(false) {}

ArgumentParser& ArgumentParser::addOption(const std::string& name, 
                                        const std::string& shortName,
                                        const std::string& description,
                                        bool requiresValue,
                                        const std::optional<std::string>& defaultValue,
                                        std::function<bool(const std::string&)> validator) {
    Option option;
    option.name = name;
    option.shortName = shortName;
    option.description = description;
    option.requiresValue = requiresValue;
    option.defaultValue = defaultValue;
    option.validator = validator;
    
    options_.push_back(option);
    return *this;
}

ArgumentParser& ArgumentParser::addHelpOption(const std::string& helpFlag,
                                             const std::string& shortHelpFlag) {
    helpAdded_ = true;
    return *this;
}

ArgumentParser& ArgumentParser::addVersionOption(const std::string& versionFlag,
                                               const std::string& shortVersionFlag) {
    versionAdded_ = true;
    return *this;
}

namespace nlm {

ArgumentParser::ParsedArguments parse(int argc, char* argv[]);

} // namespace nlm

return *this;
}

ArgumentParser::ParsedArguments ArgumentParser::parse(int argc, char* argv[]) {
    ParsedArguments result;
    result.helpRequested = false;
    result.versionRequested = false;
    
    parseArguments(argc, argv, result);
    result.helpRequested = validateHelpFlag(result.options["--help"]);
    result.versionRequested = validateVersionFlag(result.options["--version"]);
    
    return result;
}

void ArgumentParser::printHelp(std::ostream& out) const {
    out << "Usage: " << programName_ << " [options]" << std::endl << std::endl;
    
    out << "Options:" << std::endl;
    for (const auto& option : options_) {
        out << "  ";
        if (!option.shortName.empty()) {
            out << option.shortName << ", ";
        }
        out << std::left << std::setw(20) << option.name;
        out << option.description;
        if (option.requiresValue) {
            out << " <value>";
        }
        if (option.defaultValue.has_value()) {
            out << " (default: " << option.defaultValue.value() << ")";
        }
        out << std::endl;
    }
    
    if (helpAdded_) {
        out << "  " << std::left << std::setw(20) << "--help, -h"
            << "Show this help message" << std::endl;
    }
    
    if (versionAdded_) {
        out << "  " << std::left << std::setw(20) << "--version, -v"
            << "Show version information" << std::endl;
    }
    
    out << std::endl;
    out << "Examples:" << std::endl;
    out << "  " << programName_ << " --config myconfig.cfg" << std::endl;
    out << "  " << programName_ << " --neurons 1000 --steps 500" << std::endl;
    out << "  " << programName_ << " --verbose --log-file simulation.log" << std::endl;
}

bool ArgumentParser::validate(const ParsedArguments& args) const {
    // Validate required options
    for (const auto& option : options_) {
        if (option.requiresValue && option.name != "--help" && option.name != "--version") {
            if (args.options.find(option.name) == args.options.end()) {
                // Option not provided
                if (!option.defaultValue.has_value()) {
                    return false; // No default, validation fails
                }
                // Option has default value, so it's valid
                continue;
            }
        }
    }
    return true;
}

void ArgumentParser::parseArguments(int argc, char* argv[], ParsedArguments& result) const {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    processPositionalArgs(args, result);
}

bool ArgumentParser::findOption(const std::string& arg, size_t& argIndex) const {
    for (size_t i = 0; i < options_.size(); ++i) {
        if (arg == options_[i].name || 
            (!options_[i].shortName.empty() && arg == options_[i].shortName)) {
            argIndex = i;
            return true;
        }
    }
    return false;
}

void ArgumentParser::processPositionalArgs(const std::vector<std::string>& args,
                                          ParsedArguments& result) const {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            result.options["--help"] = "";
            continue;
        }
        
        if (arg == "--version" || arg == "-v") {
            result.options["--version"] = "";
            continue;
        }
        
        // Check if this is an option
        size_t optionIndex;
        if (findOption(arg, optionIndex)) {
            const Option& option = options_[optionIndex];
            
            if (option.requiresValue) {
                // Next argument should be the value
                if (i + 1 < args.size()) {
                    result.options[option.name] = args[i + 1];
                    ++i; // Skip the value argument
                } else {
                    // No value provided, error will be caught by validation
                }
            } else {
                // Option doesn't require a value, treat as flag
                result.options[option.name] = "true";
            }
        } else {
            // Positional argument
            result.positionalArgs.push_back(arg);
        }
    }
}

std::string ArgumentParser::extractOptionName(const std::string& arg) const {
    if (arg.substr(0, 2) == "--") {
        return arg.substr(2);
    } else if (arg.substr(0, 1) == "-") {
        return arg.substr(1);
    }
    return "";
}

std::string ArgumentParser::extractOptionValue(const std::string& arg) const {
    // For arguments like --option=value
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

} // namespace nlm