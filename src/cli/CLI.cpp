#include "CLI.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {
namespace cli {

// CLIError implementation
CLIError::CLIError(ErrorCode code, const std::string& message, 
                   const std::string& context, const std::string& suggestion)
    : std::runtime_error(message), code_(code), context_(context), suggestion_(suggestion) {}

// ArgumentParser implementation
ArgumentParser::ArgumentParser(const std::string& programName) 
    : programName_(programName) {}

void ArgumentParser::addOption(const std::string& name, char shortName, 
                              const std::string& description, bool required,
                              const std::string& defaultValue, bool hasValue) {
    Option option;
    option.name = name;
    option.shortName = shortName;
    option.description = description;
    option.required = required;
    option.defaultValue = defaultValue;
    option.hasValue = hasValue;
    options_.push_back(option);
}

void ArgumentParser::addSubcommand(const std::string& name, 
                                  const std::string& description) {
    subcommands_.emplace_back(name, description);
}

ArgumentParser::ParsedArgs ArgumentParser::parse(const std::vector<std::string>& args) {
    ParsedArgs parsed;
    parsed.help = false;
    parsed.version = false;
    parsed.verbose = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            parsed.help = true;
        } else if (arg == "--version" || arg == "-v") {
            parsed.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            parsed.verbose = true;
        } else if (parseOption(arg, parsed, i, args)) {
            continue;
        } else if (startsWith(arg, "--config=")) {
            parsed.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < args.size()) {
            parsed.configFile = args[++i];
        } else if (startsWith(arg, "--") && arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            parsed.options[key] = value;
        } else if (arg[0] == '-' && arg.size() > 1) {
            if (i + 1 < args.size() && args[i + 1][0] != '-') {
                parsed.shortOptions[arg[1]] = args[++i];
            } else {
                parsed.shortOptions[arg[1]] = "true";
            }
        } else {
            parsed.positionalArgs.push_back(arg);
        }
    }
    
    return parsed;
}

bool ArgumentParser::parseOption(const std::string& arg, ParsedArgs& parsed, 
                                size_t& i, const std::vector<std::string>& args) {
    if (startsWith(arg, "--") && arg.find('=') != std::string::npos) {
        size_t pos = arg.find('=');
        std::string key = arg.substr(2, pos - 2);
        std::string value = arg.substr(pos + 1);
        parsed.options[key] = value;
        return true;
    } else if (arg[0] == '-' && arg.size() > 1) {
        char shortName = arg[1];
        for (const auto& option : options_) {
            if (option.shortName == shortName) {
                if (option.hasValue) {
                    if (i + 1 < args.size() && args[i + 1][0] != '-') {
                        parsed.shortOptions[shortName] = args[++i];
                    } else {
                        parsed.shortOptions[shortName] = "true";
                    }
                } else {
                    parsed.shortOptions[shortName] = "true";
                }
                return true;
            }
        }
    }
    return false;
}

std::string ArgumentParser::getHelp() const {
    std::ostringstream oss;
    oss << "Usage: " << programName_ << " [OPTIONS] [SUBCOMMAND] [ARGS...]\n\n";
    oss << "Options:\n";
    for (const auto& option : options_) {
        oss << "  ";
        if (option.shortName) {
            oss << "-" << option.shortName << ", ";
        }
        oss << "--" << option.name;
        if (option.hasValue) {
            oss << "=" << (option.defaultValue.empty() ? "VALUE" : option.defaultValue);
        }
        oss << "\n    " << option.description;
        if (!option.defaultValue.empty()) {
            oss << " (default: " << option.defaultValue << ")";
        }
        oss << "\n";
    }
    oss << "\nSubcommands:\n";
    for (const auto& subcommand : subcommands_) {
        oss << "  " << subcommand.first << " - " << subcommand.second << "\n";
    }
    return oss.str();
}

std::string ArgumentParser::getVersion() const {
    return "NLM (熙然) v0.1.0 - Neural Learning Machine";
}

std::string ArgumentParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ArgumentParser::startsWith(const std::string& str, const std::string& prefix) const {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

// CLI implementation
CLI::CLI() : parser_("nlm") {
    programName_ = "nlm";
}

void CLI::registerCommand(std::unique_ptr<Command> command) {
    commands_[command->getName()] = std::move(command);
}

void CLI::registerSubcommand(std::unique_ptr<Subcommand> subcommand) {
    subcommands_[subcommand->getName()] = std::move(subcommand);
}

Result CLI::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return Result::failure(1, "No command provided. Use --help for usage information.");
    }
    
    ParsedArgs parsed = parser_.parse(args);
    
    if (parsed.help) {
        std::cout << getHelp() << std::endl;
        return Result::success();
    }
    
    if (parsed.version) {
        std::cout << getVersion() << std::endl;
        return Result::success();
    }
    
    if (parsed.positionalArgs.empty()) {
        return Result::failure(1, "No command provided. Use --help for usage information.");
    }
    
    std::string commandName = parsed.positionalArgs[0];
    
    // Try to find in commands first
    auto commandIt = commands_.find(commandName);
    if (commandIt != commands_.end()) {
        std::vector<std::string> commandArgs(parsed.positionalArgs.begin() + 1, 
                                             parsed.positionalArgs.end());
        return commandIt->second->executeWithConfig(commandArgs, parsed.configFile);
    }
    
    // Try subcommands
    auto subcommandIt = subcommands_.find(commandName);
    if (subcommandIt != subcommands_.end()) {
        std::vector<std::string> subcommandArgs(parsed.positionalArgs.begin() + 1,
                                                parsed.positionalArgs.end());
        return subcommandIt->second->execute(subcommandArgs);
    }
    
    return Result::failure(1, "Unknown command: " + commandName + ". Use --help for usage information.");
}

std::string CLI::getHelp() const {
    std::ostringstream oss;
    oss << "NLM (熙然) - Neural Learning Machine" << std::endl;
    oss << "=====================================" << std::endl;
    oss << std::endl;
    oss << "Usage: nlm [OPTIONS] <COMMAND> [ARGS...]" << std::endl;
    oss << std::endl;
    
    if (!commands_.empty()) {
        oss << "Available commands:" << std::endl;
        for (const auto& pair : commands_) {
            if (!pair.second->isHidden()) {
                oss << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
            }
        }
        oss << std::endl;
    }
    
    if (!subcommands_.empty()) {
        oss << "Subcommands:" << std::endl;
        for (const auto& pair : subcommands_) {
            oss << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
        }
        oss << std::endl;
    }
    
    oss << "Options:" << std::endl;
    oss << "  -h, --help              Show this help message" << std::endl;
    oss << "  -v, --version           Show version information" << std::endl;
    oss << "  -V, --verbose           Enable verbose output" << std::endl;
    oss << "  --config=FILE           Load configuration from file" << std::endl;
    oss << "  --config FILE           Alias for --config=FILE" << std::endl;
    oss << std::endl;
    oss << "For more information about a specific command, use:" << std::endl;
    oss << "  nlm <COMMAND> --help" << std::endl;
    
    return oss.str();
}

std::string CLI::getVersion() const {
    return "NLM (熙然) v0.1.0 - Neural Learning Machine (Phase 6: Final Integration)";
}

// DefaultCLI implementation
DefaultCLI::DefaultCLI() {
    // Default commands will be registered by the main function
}

// CLI utils implementation
namespace utils {
    std::string formatHelp(const std::string& command, const std::string& description,
                          const std::string& usage, const std::string& examples) {
        std::ostringstream oss;
        oss << "Command: " << command << std::endl;
        oss << "Description: " << description << std::endl;
        if (!usage.empty()) {
            oss << "Usage: " << usage << std::endl;
        }
        if (!examples.empty()) {
            oss << "Examples:" << std::endl;
            oss << examples << std::endl;
        }
        return oss.str();
    }
    
    std::string formatError(const CLIError& error) {
        std::ostringstream oss;
        oss << "Error: " << error.what();
        if (!error.getContext().empty()) {
            oss << " (Context: " << error.getContext() << ")";
        }
        if (!error.getSuggestion().empty()) {
            oss << "\nSuggestion: " << error.getSuggestion();
        }
        return oss.str();
    }
    
    std::string loadFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            throw CLIError(CLIError::ErrorCode::FileNotFound, 
                          "Failed to open file: " + path);
        }
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
    
    bool saveFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file) {
            return false;
        }
        file << content;
        return file.good();
    }
    
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    bool directoryExists(const std::string& path) {
        return std::filesystem::is_directory(path);
    }
    
    std::string getCurrentDirectory() {
        return std::filesystem::current_path().string();
    }
    
    std::vector<std::string> listFiles(const std::string& directory, 
                                      const std::string& pattern) {
        std::vector<std::string> files;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().filename().string());
                }
            }
        } catch (const std::exception& e) {
            // Ignore directory listing errors
        }
        return files;
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    std::string join(const std::vector<std::string>& strings, 
                    const std::string& delimiter) {
        std::ostringstream oss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) oss << delimiter;
            oss << strings[i];
        }
        return oss.str();
    }
    
    std::string generateId() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return "nlm_" + std::to_string(timestamp) + "_" + generateTimestamp();
    }
    
    std::string generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        ss << "_" << std::setfill('0') << std::setw(3) << ms;
        return ss.str();
    }
}

} // namespace cli
} // namespace nlm