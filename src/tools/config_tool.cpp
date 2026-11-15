#!/usr/bin/env g++ -std=c++17 -O2 -Wall -Wextra

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"

using namespace nlm;

class ConfigTool {
public:
    enum class Action {
        NONE,
        LIST,
        GET,
        SET,
        DELETE,
        EXPORT,
        IMPORT,
        VALIDATE,
        MERGE,
        SHOW_SUMMARY,
        CREATE_DEFAULT
    };

    struct Options {
        Action action = Action::NONE;
        std::string input_file;
        std::string output_file;
        std::string key;
        std::string value;
        std::string key_pattern;
        std::vector<std::string> args;
        bool verbose = false;
        bool backup = false;
        bool force = false;
        std::string output_format;
        std::string merge_mode;
    };

    ConfigTool() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    int run(int argc, char* argv[]) {
        Options options;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--list" || arg == "-l") {
                options.action = Action::LIST;
            } else if (arg == "--get" || arg == "-g") {
                options.action = Action::GET;
                if (i + 1 < argc) {
                    options.key = argv[++i];
                }
            } else if (arg == "--set" || arg == "-s") {
                options.action = Action::SET;
                if (i + 1 < argc) {
                    options.key = argv[++i];
                }
                if (i + 1 < argc) {
                    options.value = argv[++i];
                }
            } else if (arg == "--delete" || arg == "-d") {
                options.action = Action::DELETE;
                if (i + 1 < argc) {
                    options.key = argv[++i];
                }
            } else if (arg == "--export" || arg == "-e") {
                options.action = Action::EXPORT;
                if (i + 1 < argc) {
                    options.output_file = argv[++i];
                }
            } else if (arg == "--import" || arg == "-i") {
                options.action = Action::IMPORT;
                if (i + 1 < argc) {
                    options.input_file = argv[++i];
                }
            } else if (arg == "--validate") {
                options.action = Action::VALIDATE;
            } else if (arg == "--merge") {
                options.action = Action::MERGE;
                if (i + 1 < argc) {
                    options.input_file = argv[++i];
                }
                if (i + 1 < argc) {
                    options.merge_mode = argv[++i];
                }
            } else if (arg == "--summary") {
                options.action = Action::SHOW_SUMMARY;
            } else if (arg == "--create-default") {
                options.action = Action::CREATE_DEFAULT;
                if (i + 1 < argc) {
                    options.output_file = argv[++i];
                }
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--backup" && options.action != Action::NONE) {
                options.backup = true;
            } else if (arg == "--force") {
                options.force = true;
            } else if (arg == "--format") {
                if (i + 1 < argc) {
                    options.output_format = argv[++i];
                }
            } else if (arg.substr(0, 2) == "--") {
                options.args.push_back(arg.substr(2));
            } else {
                options.input_file = arg;
            }
        }
        
        return execute(options);
    }

private:
    int execute(const Options& options) {
        try {
            switch (options.action) {
                case Action::NONE:
                    printUsage();
                    return 1;
                case Action::LIST:
                    return handleList(options);
                case Action::GET:
                    return handleGet(options);
                case Action::SET:
                    return handleSet(options);
                case Action::DELETE:
                    return handleDelete(options);
                case Action::EXPORT:
                    return handleExport(options);
                case Action::IMPORT:
                    return handleImport(options);
                case Action::VALIDATE:
                    return handleValidate(options);
                case Action::MERGE:
                    return handleMerge(options);
                case Action::SHOW_SUMMARY:
                    return handleShowSummary(options);
                case Action::CREATE_DEFAULT:
                    return handleCreateDefault(options);
                default:
                    NLM_LOG_ERROR("Unknown action");
                    return 1;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }

    int handleList(const Options& options) {
        auto config = std::make_shared<Config>();
        
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        if (!config->loadFromFile(input_file)) {
            if (options.verbose) {
                NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            }
            return 1;
        }
        
        auto keys = config->getKeys();
        
        std::cout << "Configuration keys:" << std::endl;
        for (const auto& key : keys) {
            if (config->has(key)) {
                std::cout << "  " << key << std::endl;
            }
        }
        
        return 0;
    }

    int handleGet(const Options& options) {
        if (options.key.empty()) {
            NLM_LOG_ERROR("Missing key for get operation");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        if (!config->has(options.key)) {
            std::cout << "Key not found: " << options.key << std::endl;
            return 1;
        }
        
        // Try to get value as different types
        if (config->has(options.key + ".int")) {
            auto val = config->get<int64_t>(options.key + ".int");
            if (val.has_value()) {
                std::cout << val.value() << std::endl;
                return 0;
            }
        }
        
        if (config->has(options.key + ".float")) {
            auto val = config->get<float>(options.key + ".float");
            if (val.has_value()) {
                std::cout << std::fixed << std::setprecision(6) << val.value() << std::endl;
                return 0;
            }
        }
        
        if (config->has(options.key + ".double")) {
            auto val = config->get<double>(options.key + ".double");
            if (val.has_value()) {
                std::cout << std::fixed << std::setprecision(6) << val.value() << std::endl;
                return 0;
            }
        }
        
        if (config->has(options.key + ".string")) {
            auto val = config->get<std::string>(options.key + ".string");
            if (val.has_value()) {
                std::cout << "\"" << val.value() << "\"" << std::endl;
                return 0;
            }
        }
        
        if (config->has(options.key)) {
            // Try to get as generic value
            auto val = config->get<std::string>(options.key);
            if (val.has_value()) {
                std::cout << val.value() << std::endl;
                return 0;
            }
        }
        
        std::cout << "Key not found: " << options.key << std::endl;
        return 1;
    }

    int handleSet(const Options& options) {
        if (options.key.empty() || options.value.empty()) {
            NLM_LOG_ERROR("Missing key or value for set operation");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        // Load if exists
        config->loadFromFile(input_file);
        
        // Determine type and set value
        if (isNumber(options.value)) {
            if (isInteger(options.value)) {
                int64_t int_val = std::stoll(options.value);
                config->set(options.key, int_val, ConfigSource::CommandLine);
            } else {
                float float_val = std::stof(options.value);
                config->set(options.key, float_val, ConfigSource::CommandLine);
            }
        } else if (options.value == "true" || options.value == "false") {
            bool bool_val = (options.value == "true");
            config->set(options.key, bool_val, ConfigSource::CommandLine);
        } else if ((options.value.front() == '"' && options.value.back() == '"') ||
                   (options.value.front() == '\'' && options.value.back() == '\'')) {
            std::string str_val = options.value.substr(1, options.value.size() - 2);
            config->set(options.key, str_val, ConfigSource::CommandLine);
        } else {
            config->set(options.key, options.value, ConfigSource::CommandLine);
        }
        
        // Save
        std::string output_file = input_file;
        if (options.backup) {
            // Create backup
            std::string backup_file = input_file + ".backup_" + 
                                     std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            std::rename(input_file.c_str(), backup_file.c_str());
        }
        
        if (!config->saveToFile(input_file)) {
            NLM_LOG_ERROR("Failed to save configuration to " + input_file);
            return 1;
        }
        
        if (options.verbose) {
            NLM_LOG_INFO("Set " + options.key + " = " + options.value + " in " + input_file);
        }
        
        return 0;
    }

    int handleDelete(const Options& options) {
        if (options.key.empty()) {
            NLM_LOG_ERROR("Missing key for delete operation");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        config->remove(options.key);
        
        if (!config->saveToFile(input_file)) {
            NLM_LOG_ERROR("Failed to save configuration after delete");
            return 1;
        }
        
        if (options.verbose) {
            NLM_LOG_INFO("Deleted key: " + options.key);
        }
        
        return 0;
    }

    int handleExport(const Options& options) {
        if (options.output_file.empty()) {
            NLM_LOG_ERROR("Missing output file for export");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        // Convert to desired format if specified
        if (options.output_format == "json" || options.output_format == "yaml") {
            // TODO: Implement JSON/YAML export
            NLM_LOG_WARNING("JSON/YAML export not yet implemented, using default format");
        }
        
        if (!config->saveToFile(options.output_file)) {
            NLM_LOG_ERROR("Failed to export configuration to " + options.output_file);
            return 1;
        }
        
        if (options.verbose) {
            NLM_LOG_INFO("Exported configuration to " + options.output_file);
        }
        
        return 0;
    }

    int handleImport(const Options& options) {
        if (options.input_file.empty()) {
            NLM_LOG_ERROR("Missing input file for import");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(options.input_file)) {
            NLM_LOG_ERROR("Failed to import configuration from " + options.input_file);
            return 1;
        }
        
        std::string output_file = options.input_file;
        if (!config->saveToFile(output_file)) {
            NLM_LOG_ERROR("Failed to save imported configuration");
            return 1;
        }
        
        if (options.verbose) {
            NLM_LOG_INFO("Imported configuration from " + options.input_file);
        }
        
        return 0;
    }

    int handleValidate(const Options& options) {
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        std::cout << "Configuration validation:" << std::endl;
        std::cout << "  File: " << input_file << std::endl;
        std::cout << "  Keys: " << config->getKeys().size() << std::endl;
        
        // Validate key names
        bool has_errors = false;
        for (const auto& key : config->getKeys()) {
            if (key.empty()) {
                std::cout << "  ERROR: Empty key name" << std::endl;
                has_errors = true;
            } else if (key.find(' ') != std::string::npos) {
                std::cout << "  WARNING: Key name contains spaces: " << key << std::endl;
            }
        }
        
        // Try to get values for validation
        for (const auto& key : config->getKeys()) {
            if (config->has(key)) {
                // Try to parse as different types
                auto val = config->get<std::string>(key);
                if (!val.has_value()) {
                    std::cout << "  ERROR: Cannot parse value for key: " << key << std::endl;
                    has_errors = true;
                }
            }
        }
        
        if (has_errors) {
            return 1;
        } else {
            std::cout << "  All validations passed!" << std::endl;
            return error
        }
    }

    int handleMerge(const Options& options) {
        if (options.input_file.empty()) {
            NLM_LOG_ERROR("Missing input file for merge");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        // TODO: Implement merge logic
        NLM_LOG_WARNING("Merge functionality not yet implemented");
        
        return 0;
    }

    int handleShowSummary(const Options& options) {
        auto config = std::make_shared<Config>();
        std::string input_file = options.input_file.empty() ? "configs/default.cfg" : options.input_file;
        
        if (!config->loadFromFile(input_file)) {
            NLM_LOG_ERROR("Failed to load configuration from " + input_file);
            return 1;
        }
        
        std::cout << config->summary() << std::endl;
        return 0;
    }

    int handleCreateDefault(const Options& options) {
        if (options.output_file.empty()) {
            NLM_LOG_ERROR("Missing output file for create-default");
            return 1;
        }
        
        auto config = std::make_shared<Config>();
        
        // Set some default values
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        if (!config->saveToFile(options.output_file)) {
            NLM_LOG_ERROR("Failed to create default configuration");
            return 1;
        }
        
        if (options.verbose) {
            NLM_LOG_INFO("Created default configuration in " + options.output_file);
        }
        
        return 0;
    }

    bool isNumber(const std::string& str) {
        if (str.empty()) return false;
        
        size_t start = 0;
        if (str[0] == '+' || str[0] == '-') {
            start = 1;
        }
        
        bool hasDecimal = false;
        for (size_t i = start; i < str.size(); ++i) {
            if (!std::isdigit(str[i])) {
                if (str[i] == '.' && !hasDecimal) {
                    hasDecimal = true;
                } else {
                    return false;
                }
            }
        }
        
        return true;
    }

    bool isInteger(const std::string& str) {
        if (str.empty()) return false;
        
        size_t start = 0;
        if (str[0] == '+' || str[0] == '-') {
            start = 1;
        }
        
        for (size_t i = start; i < str.size(); ++i) {
            if (!std::isdigit(str[i])) {
                return false;
            }
        }
        
        return true;
    }

    void printUsage() {
        std::cout << "NLM Configuration Tool" << std::endl;
        std::cout << "Usage: config_tool [options] [input_file]" << std::endl;
        std::cout << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  --list, -l           List all configuration keys" << std::endl;
        std::cout << "  --get <key>, -g <k>  Get value for key" << std::endl;
        std::cout << "  --set <key> <value>, -s <k> <v>  Set value for key" << std::endl;
        std::cout << "  --delete <key>, -d <k>  Delete key" << std::endl;
        std::cout << "  --export <file>, -e <f>  Export configuration to file" << std::endl;
        std::cout << "  --import <file>, -i <f>  Import configuration from file" << std::endl;
        std::cout << "  --validate           Validate configuration" << std::endl;
        std::cout << "  --merge <file>       Merge with another configuration" << std::endl;
        std::cout << "  --summary            Show configuration summary" << std::endl;
        std::cout << "  --create-default <file>  Create default configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --verbose, -v         Verbose output" << std::endl;
        std::cout << "  --backup             Create backup before saving" << std::endl;
        std::cout << "  --force              Force operation without confirmation" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  config_tool --list" << std::endl;
        std::cout << "  config_tool --get neuron_count" << std::endl;
        std::cout << "  config_tool --set neuron_count 2000 configs/test.cfg" << std::endl;
        std::cout << "  config_tool --export config.json --format json" << std::endl;
        std::cout << "  config_tool --create-default my_config.cfg" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    ConfigTool tool;
    return tool.run(argc, argv);
}
