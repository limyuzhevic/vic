#include "CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unordered_map>

namespace nlm {

UnifiedCLI::UnifiedCLI() : programName_("nlm") {}

UnifiedCLI::~UnifiedCLI() = default;

bool UnifiedCLI::parseArgs(int argc, char* argv[]) {
    programName_ = (argc > 0) ? argv[0] : "nlm";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            config_.help = true;
        } else if (arg == "--version") {
            config_.version = true;
        } else if (arg == "--verbose") {
            config_.verbose = true;
        } else if (arg == "--quiet") {
            config_.quiet = true;
        } else if (arg == "--validate-config") {
            config_.validateConfig = true;
        } else if (arg == "--export-metrics") {
            config_.exportMetrics = true;
        } else if (arg == "--interactive") {
            config_.interactive = true;
        } else if (arg == "--no-pause") {
            config_.pauseOnError = false;
        } else if (arg == "--step-mode") {
            config_.enableStepMode = true;
        } else if (arg == "--enable-visualization") {
            config_.enableVisualization = true;
        } else if (arg.substr(0, 12) == "--output-format=") {
            std::string format = arg.substr(12);
            config_.outputFormat = parseOutputFormat(format);
        } else if (arg.substr(0, 15) == "--output-file=") {
            config_.outputFile = arg.substr(15);
        } else if (arg.substr(0, 12) == "--config=") {
            config_.configFile = arg.substr(12);
        } else if (arg.substr(0, 9) == "--config") {
            // Handle --config file without = sign
            if (i + 1 < argc) {
                config_.configFile = argv[++i];
            }
        } else if (arg.find('=') != std::string::npos) {
            // Parse key=value argument
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            config_.customArgs[key] = value;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

bool UnifiedCLI::processConfig(std::shared_ptr<Config> config) {
    // Load from config file if specified
    if (!config_.configFile.empty()) {
        if (!config->loadFromFile(config_.configFile)) {
            if (config_.verbose) {
                std::cerr << "Warning: Could not load config file: " << config_.configFile << std::endl;
            }
        }
    }
    
    // Apply custom arguments
    for (const auto& pair : config_.customArgs) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Try to parse based on type hints (simplified)
        if (key == "neuron_count") {
            try {
                size_t val = std::stoul(value);
                config->set(key, static_cast<int64_t>(val), ConfigSource::CommandLine);
            } catch (...) {
                return false;
            }
        } else if (key == "region_count") {
            try {
                size_t val = std::stoul(value);
                config->set(key, static_cast<int64_t>(val), ConfigSource::CommandLine);
            } catch (...) {
                return false;
            }
        } else if (key == "connection_probability") {
            try {
                float val = std::stof(value);
                config->set(key, val, ConfigSource::CommandLine);
            } catch (...) {
                return false;
            }
        } else if (key == "random_seed") {
            try {
                uint64_t val = std::stoull(value);
                config->set(key, static_cast<int64_t>(val), ConfigSource::CommandLine);
            } catch (...) {
                return false;
            }
        } else if (key == "simulation_timestep") {
            try {
                double val = std::stod(value);
                config->set(key, val, ConfigSource::CommandLine);
            } catch (...) {
                return false;
            }
        } else {
            // For unknown keys, treat as string or generic type
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Validate if requested
    if (config_.validateConfig && !validateConfig(config)) {
        return false;
    }
    
    return true;
}

void UnifiedCLI::printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] [key=value...]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --quiet                   Suppress non-essential output" << std::endl;
    std::cout << "  --config=file             Load configuration from file" << std::endl;
    std::cout << "  --config file            Alternative format for config file" << std::endl;
    std::cout << "  --output-format=FORMAT    Output format: human|json|csv|compact|minimal" << std::endl;
    std::cout << "  --output-file=FILE        Write output to file" << std::endl;
    std::cout << "  --validate-config         Validate configuration before running" << std::endl;
    std::cout << "  --export-metrics          Export metrics in machine-readable format" << std::endl;
    std::cout << "  --interactive             Enable interactive mode" << std::endl;
    std::cout << "  --no-pause               Don't pause on errors in interactive mode" << std::endl;
    std::cout << "  --step-mode               Enable step-by-step execution mode" << std::endl;
    std::cout << "  --enable-visualization    Enable visualization (requires external viewer)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Key=Value Arguments:" << std::endl;
    std::cout << "  Sets configuration parameters (e.g., neuron_count=1000, random_seed=42)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    std::cout << "  " << programName << " --config=myconfig.cfg --verbose" << std::endl;
    std::cout << "  " << programName << " neuron_count=2000 connection_probability=0.2" << std::endl;
    std::cout << "  " << programName << " --interactive --step-mode" << std::endl;
    std::cout << std::endl;
}

void UnifiedCLI::printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine" << std::endl;
    std::cout << "Version: 1.0.0 (Phase 6 - Integration Complete)" << std::endl;
    std::cout << "Copyright: 2024-2026 NLM Research Team" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Real spiking neural computation (LIF neurons)" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - Comprehensive neuromodulation (DA, ACH, NE, 5-HT)" << std::endl;
    std::cout << "  - Integrated memory systems (working, episodic, associative)" << std::endl;
    std::cout << "  - Emergent cognitive capabilities (planning, concept formation, attention)" << std::endl;
    std::cout << "  - Developmental plasticity stages" << std::endl;
    std::cout << "  - Checkpointing and replay mechanisms" << std::endl;
    std::cout << std::endl;
}

void UnifiedCLI::printConfigSummary(const std::shared_ptr<Config>& config) const {
    if (config_.quiet || !config_.verbose) {
        return;
    }
    
    std::cout << "=== Configuration Summary ===" << std::endl;
    
    // Print key configuration values
    std::vector<std::string> keys = {
        "neuron_count", "region_count", "connection_probability",
        "random_seed", "simulation_timestep", "stdp_ltp_weight",
        "stdp_ltd_weight", "synaptogenesis_rate", "pruning_rate"
    };
    
    for (const auto& key : keys) {
        auto value = config->get<void>(key);
        if (value.has_value()) {
            // Simple conversion for display
            if (std::holds_alternative<size_t>(value)) {
                std::cout << "  " << key << ": " << std::to_string(std::get<size_t>(value)) << std::endl;
            } else if (std::holds_alternative<double>(value)) {
                std::cout << "  " << key << ": " << std::to_string(std::get<double>(value)) << std::endl;
            } else if (std::holds_alternative<float>(value)) {
                std::cout << "  " << key << ": " << std::to_string(std::get<float>(value)) << std::endl;
            } else if (std::holds_alternative<std::string>(value)) {
                std::cout << "  " << key << ": " << std::get<std::string>(value) << std::endl;
            }
        }
    }
    
    std::cout << "================================" << std::endl;
}

std::string UnifiedCLI::outputFormatToString(OutputFormat format) const {
    static const std::unordered_map<OutputFormat, std::string> formatStrings = {
        {OutputFormat::HUMAN_READABLE, "human"},
        {OutputFormat::JSON, "json"},
        {OutputFormat::CSV, "csv"},
        {OutputFormat::COMPACT, "compact"},
        {OutputFormat::MINIMAL, "minimal"}
    };
    
    auto it = formatStrings.find(format);
    return (it != formatStrings.end()) ? it->second : "human";
}

void UnifiedCLI::convertOutput(const std::string& humanOutput, OutputFormat targetFormat, std::ostream& output) {
    if (targetFormat == OutputFormat::HUMAN_READABLE || targetFormat == OutputFormat::COMPACT) {
        output << humanOutput;
        return;
    }
    
    // For machine-readable formats, we'd need to parse the human output
    // and convert it. This is a simplified implementation.
    output << "{\"format\": \"" << outputFormatToString(targetFormat) << \"\", \"data\": \"" << humanOutput << "\"}" << std::endl;
}

bool UnifiedCLI::validateConfig(const std::shared_ptr<Config>& config) const {
    return validateConfigValues(config);
}

bool UnifiedCLI::validateConfigValues(const std::shared_ptr<Config>& config) const {
    // Validate essential configuration values
    auto neuronCountOpt = config->get<size_t>("neuron_count");
    if (!neuronCountOpt || *neuronCountOpt == 0) {
        std::cerr << "Error: neuron_count must be positive" << std::endl;
        return false;
    }
    
    auto regionCountOpt = config->get<size_t>("region_count");
    if (!regionCountOpt || *regionCountOpt == 0) {
        std::cerr << "Error: region_count must be positive" << std::endl;
        return false;
    }
    
    auto connectionProbOpt = config->get<float>("connection_probability");
    if (!connectionProbOpt || *connectionProbOpt < 0.0f || *connectionProbOpt > 1.0f) {
        std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    return true;
}

bool UnifiedCLI::validateCLIConfig() const {
    if (config_.outputFormat == OutputFormat::JSON && config_.outputFile.empty() && !config_.exportMetrics) {
        // For JSON output, we typically want to write to a file
        std::cerr << "Warning: JSON output will be written to console. Consider using --output-file for better results." << std::endl;
    }
    
    return true;
}

void UnifiedCLI::addCommonConfigOptions(std::shared_ptr<Config> config) {
    // Add common configuration options that are useful across demos
    config->set("verbose_logging", false, ConfigSource::Default);
    config->set("checkpoint_interval", 1000, ConfigSource::Default);
    config->set("max_simulation_steps", 10000, ConfigSource::Default);
    config->set("enable_synaptic_history", true, ConfigSource::Default);
}

bool UnifiedCLI::parseDemoArgs(int argc, char* argv[], int& numEpisodes, int& stepsPerEpisode,
                                int& numTrials, int& maxSteps) {
    // Parse standard demo arguments with backward compatibility
    numEpisodes = 5;
    stepsPerEpisode = 200;
    numTrials = 50;
    maxSteps = 2000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (i + 1 < argc) {
            if (arg == "--episodes" || arg == "-e") {
                numEpisodes = std::atoi(argv[++i]);
            } else if (arg == "--steps" || arg == "-s") {
                stepsPerEpisode = std::atoi(argv[++i]);
            } else if (arg == "--trials" || arg == "-t") {
                numTrials = std::atoi(argv[++i]);
            } else if (arg == "--max-steps") {
                maxSteps = std::atoi(argv[++i]);
            }
        } else if (arg == "--episodes" || arg == "-e") {
            // Handle case where argument is last one (error)
            std::cerr << "Error: --episodes requires a value" << std::endl;
            return false;
        }
    }
    
    return true;
}

OutputFormat UnifiedCLI::parseOutputFormat(const std::string& format) const {
    static const std::unordered_map<std::string, OutputFormat> formatMap = {
        {"human", OutputFormat::HUMAN_READABLE},
        {"json", OutputFormat::JSON},
        {"csv", OutputFormat::CSV},
        {"compact", OutputFormat::COMPACT},
        {"minimal", OutputFormat::MINIMAL}
    };
    
    auto it = formatMap.find(format);
    return (it != formatMap.end()) ? it->second : OutputFormat::HUMAN_READABLE;
}

} // namespace nlm