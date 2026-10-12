#include "AdvancedArgumentParser.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

bool AdvancedArgumentParser::parse(int argc, char** argv, AdvancedCLIOptions& options) {
    if (!argv) {
        NLM_LOG_ERROR("AdvancedArgumentParser: argv is null");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_LOG_ERROR("AdvancedArgumentParser: argv[" + std::to_string(i) + "] is null");
            continue;
        }
        
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options.legacy_overrides[key] = value;
            } else {
                NLM_LOG_ERROR("AdvancedArgumentParser: Invalid --key=value format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            if (!argv[i + 1]) {
                NLM_LOG_ERROR("AdvancedArgumentParser: argv[" + std::to_string(i + 1) + "] is null");
                continue;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            options.legacy_overrides[key] = value;
        } else {
            NLM_LOG_ERROR("AdvancedArgumentParser: Invalid argument format: " + arg);
        }
    }
    
    // Load legacy config file if specified
    if (options.legacy_overrides.find("config") != options.legacy_overrides.end()) {
        options.legacy_config_file = options.legacy_overrides["config"];
        parseConfigFile(options.legacy_config_file, options.legacy_overrides);
    }
    
    return true;
}

bool AdvancedArgumentParser::parse(int argc, char** argv, AdvancedCLIOptions& options, const std::string& configFile) {
    // Parse arguments first
    if (!parse(argc, argv, options)) {
        return false;
    }
    
    // Then load from specified config file if provided
    if (!configFile.empty()) {
        std::map<std::string, std::string> configMap;
        if (parseConfigFile(configFile, configMap)) {
            // Apply config file values (override defaults)
            for (const auto& pair : configMap) {
                options.legacy_overrides[pair.first] = pair.second;
            }
        }
    }
    
    return true;
}

void AdvancedArgumentParser::printHelp(const std::string& programName) {
    std::cout << "=== NLM Advanced CLI Options ===\n" << std::endl;
    std::cout << "Current CLI Features:" << std::endl;
    std::cout << "  --config=filepath    Load configuration from JSON file" << std::endl;
    std::cout << "  --<key>=<value>      Override configuration values" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Required Advanced CLI Options:" << std::endl;
    
    std::cout << "\n1. Logging Control:" << std::endl;
    std::cout << "   --log-level=<level>     Set logging level (debug, info, warning, error)" << std::endl;
    std::cout << "   --log-file=<path>       Log to file instead of console" << std::endl;
    std::cout << "   --log-format=<format>   Log output format (simple, json, verbose)" << std::endl;
    
    std::cout << "\n2. Simulation Control:" << std::endl;
    std::cout << "   --steps=<n>             Number of simulation steps to run" << std::endl;
    std::cout << "   --timestep=<dt>         Override simulation timestep" << std::endl;
    std::cout << "   --time=<t>              Run simulation for specific duration" << std::endl;
    std::cout << "   --stop-at=<condition>   Stop simulation when condition met" << std::endl;
    
    std::cout << "\n3. Experiment Control:" << std::endl;
    std::cout << "   --experiment=<name>     Select specific experiment (Phase3, Phase4, Phase5, Phase6, Phase3Demo, etc.)" << std::endl;
    std::cout << "   --trial=<n>             Number of trials for experiments" << std::endl;
    std::cout << "   --random-seed=<seed>    Set random seed for reproducibility" << std::endl;
    
    std::cout << "\n4. Output Control:" << std::endl;
    std::cout << "   --output-format=<format> Output format (text, csv, json, xml)" << std::endl;
    std::cout << "   --output-file=<path>    Save results to file" << std::endl;
    std::cout << "   --quiet                 Suppress non-error output" << std::endl;
    std::cout << "   --verbose               Enable verbose output" << std::endl;
    
    std::cout << "\n5. Debugging & Analysis:" << std::endl;
    std::cout << "   --debug                 Enable debugging mode" << std::endl;
    std::cout << "   --profile               Enable profiling" << std::endl;
    std::cout << "   --benchmark            Run performance benchmarks" << std::endl;
    std::cout << "   --validate             Validate configuration and exit" << std::endl;
    std::cout << "   --dry-run               Parse arguments and exit" << std::endl;
    
    std::cout << "\n6. Visualization & Monitoring:" << std::endl;
    std::cout << "   --monitor               Enable real-time monitoring" << std::endl;
    std::cout << "   --watch=<interval>     Monitoring interval in seconds" << std::endl;
    std::cout << "   --neural-visualization Enable neural activity visualization" << std::endl;
    std::cout << "   --memory-visualization  Enable memory system visualization" << std::endl;
    
    std::cout << "\n7. Batch Processing:" << std::endl;
    std::cout << "   --batch                 Batch processing mode" << std::endl;
    std::cout << "   --batch-config=<path>   Load batch configuration from file" << std::endl;
    std::cout << "   --parallel=<n>         Number of parallel simulations" << std::endl;
    
    std::cout << "\n=== Examples ===" << std::endl;
    std::cout << "# Run simulation with custom timestep and log to file:" << std::endl;
    std::cout << "nlm --timestep=0.0005 --log-level=debug --log-file=nlm.log" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Run specific experiment with multiple trials:" << std::endl;
    std::cout << "nlm --experiment=Phase4 --trial=10 --random-seed=12345" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Batch processing with parallel simulations:" << std::endl;
    std::cout << "nlm --batch --batch-config=batch_config.txt --parallel=4" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "# Validation mode:" << std::endl;
    std::cout << "nlm --validate --config=my_config.json" << std::endl;
}

bool AdvancedArgumentParser::validateOptions(const AdvancedCLIOptions& options) {
    bool valid = true;
    
    // Validate log level
    if (!options.log_level.empty()) {
        std::string level = toLower(options.log_level);
        if (level != "debug" && level != "info" && level != "warning" && level != "error") {
            NLM_LOG_ERROR("AdvancedArgumentParser: Invalid log level: " + options.log_level);
            valid = false;
        }
    }
    
    // Validate log format
    if (!options.log_format.empty()) {
        std::string format = toLower(options.log_format);
        if (format != "simple" && format != "json" && format != "verbose") {
            NLM_LOG_ERROR("AdvancedArgumentParser: Invalid log format: " + options.log_format);
            valid = false;
        }
    }
    
    // Validate output format
    if (!options.output_format.empty()) {
        std::string format = toLower(options.output_format);
        if (format != "text" && format != "csv" && format != "json" && format != "xml") {
            NLM_LOG_ERROR("AdvancedArgumentParser: Invalid output format: " + options.output_format);
            valid = false;
        }
    }
    
    // Validate simulation parameters
    if (options.simulation_steps <= 0 && options.simulation_steps != -1) {
        NLM_LOG_ERROR("AdvancedArgumentParser: Invalid simulation steps: " + std::to_string(options.simulation_steps));
        valid = false;
    }
    
    if (options.simulation_timestep <= 0.0) {
        NLM_LOG_ERROR("AdvancedArgumentParser: Invalid simulation timestep: " + std::to_string(options.simulation_timestep));
        valid = false;
    }
    
    if (options.simulation_time < 0.0) {
        NLM_LOG_ERROR("AdvancedArgumentParser: Invalid simulation time: " + std::to_string(options.simulation_time));
        valid = false;
    }
    
    if (options.monitor_interval <= 0) {
        NLM_LOG_ERROR("AdvancedArgumentParser: Invalid monitor interval: " + std::to_string(options.monitor_interval));
        valid = false;
    }
    
    if (options.parallel_simulations <= 0) {
        NLM_LOG_ERROR("AdvancedArgumentParser: Invalid parallel simulations: " + std::to_string(options.parallel_simulations));
        valid = false;
    }
    
    return valid;
}

std::string AdvancedArgumentParser::valueToString(const std::string& value) {
    return value;
}

std::string AdvancedArgumentParser::valueToString(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string AdvancedArgumentParser::valueToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string AdvancedArgumentParser::valueToString(bool value) {
    return value ? "true" : "false";
}

bool AdvancedArgumentParser::parseConfigFile(const std::string& filepath, std::map<std::string, std::string>& configMap) {
    // TODO: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format with validation
    
    // Check if filepath is valid
    if (filepath.empty()) {
        NLM_LOG_ERROR("AdvancedArgumentParser: parseConfigFile: Empty filepath provided");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("AdvancedArgumentParser: parseConfigFile: Failed to open file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("AdvancedArgumentParser: parseConfigFile: Line " + std::to_string(lineNumber) + " has no '=' separator: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_ERROR("AdvancedArgumentParser: parseConfigFile: Line " + std::to_string(lineNumber) + ": Empty key");
            return false;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        configMap[key] = value;
    }
    
    return true;
}

std::string AdvancedArgumentParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string AdvancedArgumentParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
