// CLI Utility Implementation
// Provides enhanced command-line argument parsing, help system, and configuration management

#include "core/CLIUtility.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>

namespace nlm {

struct CLIParser::Impl {
    int argc;
    char** argv;
    std::map<std::string, CLIArgument> argumentDefinitions;
    std::map<std::string, std::string> parsedValues;
    std::vector<std::string> unknownArgs;
    std::map<std::string, std::string> choiceOptions;
    bool parsed;
    
    Impl(int argc, char** argv) : argc(argc), argv(argv), parsed(false) {
        // Register standard options
        addArgument(DemoOptions::Help);
        addArgument(DemoOptions::Version);
    }
};

CLIParser::CLIParser(int argc, char** argv) : pImpl(std::make_unique<Impl>(argc, argv)) {}

void CLIParser::addArgument(const CLIArgument& arg) {
    pImpl->argumentDefinitions[arg.name] = arg;
    if (!arg.choices.empty()) {
        pImpl->choiceOptions[arg.name] = arg.choices[0];  // Store first choice as default
    }
}

bool CLIParser::parse() {
    if (pImpl->parsed) return true;
    
    parseArguments();
    pImpl->parsed = true;
    return validate();
}

template<typename T>
T CLIParser::getArgument(const std::string& name, const T& defaultValue) const {
    // Simplified implementation - in a real version, this would handle different types
    auto it = pImpl->parsedValues.find(name);
    if (it != pImpl->parsedValues.end()) {
        // Convert string to T (simplified)
        if constexpr (std::is_same_v<T, std::string>) {
            return it->second;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(it->second);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(it->second);
        } else if constexpr (std::is_same_v<T, bool>) {
            return it->second == "true" || it->second == "1" || it->second == "yes";
        }
    }
    return defaultValue;
}

bool CLIParser::hasArgument(const std::string& name) const {
    return pImpl->parsedValues.find(name) != pImpl->parsedValues.end();
}

std::vector<std::string> CLIParser::getArgumentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->argumentDefinitions) {
        names.push_back(pair.first);
    }
    return names;
}

void CLIParser::printHelp() const {
    std::cout << "=== NLM Demo CLI Options ===" << std::endl;
    std::cout << std::endl;
    
    for (const auto& pair : pImpl->argumentDefinitions) {
        const auto& arg = pair.second;
        std::cout << "  ";
        
        // Short option
        if (arg.shortOption != 0) {
            std::cout << "-" << arg.shortOption << ", ";
        }
        
        // Long option
        if (!arg.longOption.empty()) {
            std::cout << "--" << arg.longOption;
        }
        
        std::cout << "\t" << arg.description;
        
        if (!arg.defaultValue.empty()) {
            std::cout << " (default: " << arg.defaultValue << ")";
        }
        
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    printUnknownArgsWarning();
}

void CLIParser::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Common options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "For more options, use --help" << std::endl;
}

bool CLIParser::applyToConfiguration(ConfigurationManager& configManager) const {
    bool success = true;
    
    for (const auto& pair : pImpl->parsedValues) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Try to parse as different types
        try {
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                configManager.setWithValidation(key, boolVal, ConfigSource::CommandLine, "Command line");
            } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
                int intVal = std::stoi(value);
                configManager.setWithValidation(key, intVal, ConfigSource::CommandLine, "Command line");
            } else if (isNumber(value)) {
                double doubleVal = std::stod(value);
                configManager.setWithValidation(key, doubleVal, ConfigSource::CommandLine, "Command line");
            } else {
                configManager.setWithValidation(key, value, ConfigSource::CommandLine, "Command line");
            }
        } catch (const std::exception& e) {
            std::cerr << "Error applying configuration option \"" << key << "\": " << e.what() << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool CLIParser::validate() const {
    std::string errors = getValidationErrors();
    if (!errors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        std::cerr << errors << std::endl;
        return false;
    }
    return true;
}

std::string CLIParser::getValidationErrors() const {
    std::ostringstream oss;
    
    for (const auto& pair : pImpl->argumentDefinitions) {
        const auto& arg = pair.second;
        
        // Check if required
        if (arg.required && !hasArgument(arg.name)) {
            oss << "  Error: Required option --" << arg.name << " is missing" << std::endl;
        }
        
        // Check choices if provided
        if (!arg.choices.empty() && hasArgument(arg.name)) {
            const std::string& value = pImpl->parsedValues.at(arg.name);
            bool validChoice = false;
            for (const auto& choice : arg.choices) {
                if (value == choice) {
                    validChoice = true;
                    break;
                }
            }
            if (!validChoice) {
                oss << "  Error: Invalid value \"" << value << "\" for option --" << arg.name << std::endl;
                oss << "         Valid choices: ";
                for (size_t i = 0; i < arg.choices.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg.choices[i];
                }
                oss << std::endl;
            }
        }
    }
    
    return oss.str();
}

size_t CLIParser::getArgumentCount() const {
    return pImpl->parsedValues.size();
}

size_t CLIParser::getUnknownArgsCount() const {
    return pImpl->unknownArgs.size();
}

void CLIParser::parseArguments() {
    for (int i = 1; i < pImpl->argc; ++i) {
        std::string arg = pImpl->argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                pImpl->parsedValues[key] = value;
            } else {
                // Check if this is a known flag
                std::string key = arg.substr(2);
                auto it = pImpl->argumentDefinitions.find(key);
                if (it != pImpl->argumentDefinitions.end() && it->second.type == "flag") {
                    pImpl->parsedValues[key] = "true";
                } else {
                    pImpl->unknownArgs.push_back(arg);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            auto it = pImpl->argumentDefinitions.find(key);
            
            if (it != pImpl->argumentDefinitions.end()) {
                if (i + 1 < pImpl->argc && pImpl->argv[i + 1][0] != '-') {
                    // Next argument is the value
                    pImpl->parsedValues[key] = pImpl->argv[++i];
                } else {
                    // Flag option
                    pImpl->parsedValues[key] = "true";
                }
            } else {
                pImpl->unknownArgs.push_back(arg);
            }
        } else {
            pImpl->unknownArgs.push_back(arg);
        }
    }
}

void CLIParser::printUnknownArgsWarning() const {
    if (!pImpl->unknownArgs.empty()) {
        std::cout << "Warnings:" << std::endl;
        for (const auto& arg : pImpl->unknownArgs) {
            std::cout << "  Unknown argument: " << arg << std::endl;
        }
        std::cout << std::endl;
    }
}

// Initialize static options
namespace DemoOptions {
    const CLIArgument Help("help", 'h', "help", "Show help message", "", "flag");
    const CLIArgument Version("version", 0, "version", "Show version information", "", "flag");
    const CLIArgument LoadConfig("load", 0, "load", "Load configuration from file", "", "string");
    const CLIArgument SaveConfig("save", 0, "save", "Save configuration to file", "", "string");
    
    const CLIArgument Preset("preset", 0, "preset", "Configuration preset (quick, full, performance, development)", 
                             "", "choice", {"quick", "full", "performance", "development"}, false);
    const CLIArgument NeuronCount("neurons", 0, "neurons", "Number of neurons", "1000", "int");
    const CLIArgument RegionCount("regions", 0, "regions", "Number of regions", "1", "int");
    const CLIArgument ConnectionProbability("connection_prob", 0, "connection_prob", "Connection probability", "0.1", "float");
    const CLIArgument MaxSteps("steps", 0, "steps", "Maximum simulation steps", "1000", "int");
    const CLIArgument SimulationTimestep("timestep", 0, "timestep", "Simulation timestep", "0.001", "float");
    
    const CLIArgument EnableCheckpointing("checkpoint", 0, "checkpoint", "Enable checkpointing", "false", "flag");
    const CLIArgument EnableReplay("replay", 0, "replay", "Enable replay functionality", "false", "flag");
    const CLIArgument EnableDevelopment("development", 0, "development", "Enable development system", "false", "flag");
    const CLIArgument EnableNeuromodulation("neuromodulation", 0, "neuromodulation", "Enable neuromodulation", "true", "flag");
    
    const CLIArgument Verbose("verbose", 0, "verbose", "Verbose output", "false", "flag");
    const CLIArgument Quiet("quiet", 0, "quiet", "Quiet output", "false", "flag");
    const CLIArgument OutputFile("output", 0, "output", "Output file", "", "string");
    const CLIArgument OutputFormat("format", 0, "format", "Output format (text, json, csv)", "text", "choice", {"text", "json", "csv"});
    
    const CLIArgument BenchmarkType("bench_type", 0, "bench_type", "Benchmark type", "standard", "choice", {"standard", "stress", "performance"});
    const CLIArgument BenchmarkDuration("bench_duration", 0, "bench_duration", "Benchmark duration (seconds)", "60", "int");
    const CLIArgument BenchmarkIterations("bench_iterations", 0, "bench_iterations", "Number of benchmark iterations", "10", "int");
    
    const CLIArgument RandomSeed("seed", 0, "seed", "Random seed", "42", "int");
    const CLIArgument EnvironmentWidth("width", 0, "width", "Environment width", "20", "int");
    const CLIArgument EnvironmentHeight("height", 0, "height", "Environment height", "20", "int");
}

std::vector<CLIArgument> DemoOptionsBuilder::buildCommonOptions() {
    std::vector<CLIArgument> options;
    
    options.push_back(DemoOptions::Help);
    options.push_back(DemoOptions::Version);
    options.push_back(DemoOptions::LoadConfig);
    options.push_back(DemoOptions::SaveConfig);
    options.push_back(DemoOptions::Preset);
    options.push_back(DemoOptions::NeuronCount);
    options.push_back(DemoOptions::RegionCount);
    options.push_back(DemoOptions::ConnectionProbability);
    options.push_back(DemoOptions::MaxSteps);
    options.push_back(DemoOptions::SimulationTimestep);
    options.push_back(DemoOptions::EnableCheckpointing);
    options.push_back(DemoOptions::EnableReplay);
    options.push_back(DemoOptions::EnableDevelopment);
    options.push_back(DemoOptions::EnableNeuromodulation);
    options.push_back(DemoOptions::Verbose);
    options.push_back(DemoOptions::Quiet);
    options.push_back(DemoOptions::OutputFile);
    options.push_back(DemoOptions::OutputFormat);
    
    return options;
}

std::vector<CLIArgument> DemoOptionsBuilder::buildPhase3Options() {
    auto options = buildCommonOptions();
    
    // Add Phase 3 specific options
    options.push_back(DemoOptions::BenchmarkType);
    options.push_back(DemoOptions::BenchmarkDuration);
    options.push_back(DemoOptions::BenchmarkIterations);
    
    return options;
}

std::vector<CLIArgument> DemoOptionsBuilder::buildPhase4Options() {
    auto options = buildCommonOptions();
    
    // Add Phase 4 specific options
    options.push_back(CLIArgument("trial_count", 0, "trials", "Number of trials", "50", "int"));
    options.push_back(CLIArgument("simulation_steps", 0, "steps", "Steps per simulation", "200", "int"));
    
    return options;
}

std::vector<CLIArgument> DemoOptionsBuilder::buildPhase5Options() {
    auto options = buildCommonOptions();
    
    // Add Phase 5 specific options
    options.push_back(CLIArgument("pattern_count", 0, "patterns", "Number of patterns", "3", "int"));
    options.push_back(CLIArgument("sandbox_steps", 0, "sandbox_steps", "Sandbox steps", "100", "int"));
    
    return options;
}

std::vector<CLIArgument> DemoOptionsBuilder::buildPhase6Options() {
    auto options = buildCommonOptions();
    
    // Add Phase 6 specific options
    options.push_back(CLIArgument("integration_test", 0, "integration_test", "Run integration test only", "false", "flag"));
    options.push_back(CLIArgument("checkpoint_path", 0, "checkpoint_path", "Checkpoint file path", "", "string"));
    options.push_back(CLIArgument("replay_episodes", 0, "replay_episodes", "Number of replay episodes", "3", "int"));
    
    return options;
}

std::vector<CLIArgument> DemoOptionsBuilder::buildBenchmarkOptions() {
    auto options = buildCommonOptions();
    
    // Add benchmark specific options
    options.push_back(DemoOptions::BenchmarkType);
    options.push_back(DemoOptions::BenchmarkDuration);
    options.push_back(DemoOptions::BenchmarkIterations);
    options.push_back(CLIArgument("report_file", 0, "report", "Benchmark report file", "benchmark_report.txt", "string"));
    options.push_back(CLIArgument("csv_output", 0, "csv", "Enable CSV output", "false", "flag"));
    options.push_back(CLIArgument("json_output", 0, "json", "Enable JSON output", "false", "flag"));
    
    return options;
}

int enhancedMain(int argc, char** argv,
                 std::function<int(CLIParser&)> demoMainFunction,
                 const std::string& programDescription) {
    CLIParser parser(argc, argv);
    
    // Build common options
    auto commonOptions = DemoOptionsBuilder::buildCommonOptions();
    for (const auto& option : commonOptions) {
        parser.addArgument(option);
    }
    
    // Parse arguments
    if (!parser.parse()) {
        return 1;
    }
    
    // Execute demo main function
    return demoMainFunction(parser);
}

int demoEntryPoint(int argc, char** argv,
                   std::function<int(CLIParser&)> runDemo,
                   const std::string& demoName,
                   const std::string& version) {
    std::cout << "=== " << demoName << " ===" << std::endl;
    if (!version.empty()) {
        std::cout << "Version: " << version << std::endl;
    }
    std::cout << std::endl;
    
    return enhancedMain(argc, argv, runDemo, demoName + " demo");
}

void Presets::quickTest(CLIParser& parser) {
    parser.setArgumentValue("preset", "quick");
    parser.setArgumentValue("neurons", "500");
    parser.setArgumentValue("steps", "1000");
}

void Presets::fullTest(CLIParser& parser) {
    parser.setArgumentValue("preset", "full");
    parser.setArgumentValue("neurons", "1000");
    parser.setArgumentValue("steps", "10000");
}

void Presets::performance(CLIParser& parser) {
    parser.setArgumentValue("preset", "performance");
    parser.setArgumentValue("neurons", "2000");
    parser.setArgumentValue("steps", "500");
}

void Presets::development(CLIParser& parser) {
    parser.setArgumentValue("preset", "development");
    parser.setArgumentValue("neurons", "500");
    parser.setArgumentValue("development", "true");
}

void Presets::production(CLIParser& parser) {
    parser.setArgumentValue("preset", "production");
    parser.setArgumentValue("neurons", "1000");
    parser.setArgumentValue("checkpoint", "true");
}

void OutputFormatter::printBanner(const std::string& title) {
    std::cout << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;
}

void OutputFormatter::printSection(const std::string& title) {
    std::cout << std::endl;
    std::cout << "--- " << title << " ---" << std::endl;
}

void OutputFormatter::printResult(const std::string& name, float value, float max) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

void OutputFormatter::printMetrics(const std::map<std::string, float>& metrics) {
    std::cout << "Metrics:" << std::endl;
    for (const auto& pair : metrics) {
        printResult(pair.first, pair.second);
    }
}

void OutputFormatter::printProgress(const std::string& message, double progress) {
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r" << std::string(30, ' ') << "\r";
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "% " << message;
    std::cout.flush();
}

ProgressReporter::ProgressReporter(const std::string& title, int totalSteps) {
    // Simplified implementation
}

void ProgressReporter::update(int currentStep, const std::string& status) {
    // Simplified implementation
}

void ProgressReporter::updateProgress(double progress, const std::string& status) {
    OutputFormatter::printProgress(status, progress);
}

void ProgressReporter::setMessage(const std::string& message) {
    // Simplified implementation
}

void ProgressReporter::finish() {
    std::cout << std::endl;
}

bool ProgressReporter::isActive() const {
    return false; // Simplified implementation
}

ExperimentRunner::ExperimentRunner(const std::string& name) : name(name) {}

void ExperimentRunner::runExperiment(const std::function<void()>& experimentFunc) {
    // Simplified implementation
}

void ExperimentRunner::addResult(const ExperimentResult& result) {
    results.push_back(result);
}

void ExperimentRunner::printResults(bool verbose) const {
    std::cout << "=== " << name << " Results ===" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.name << ": " << result.status << std::endl;
    }
}

void ExperimentRunner::exportResults(const std::string& filename) const {
    // Simplified implementation
}

const std::vector<ExperimentResult>& ExperimentRunner::getResults() const {
    return results;
}

CLIParser& getGlobalCLI() {
    static CLIParser instance(0, nullptr);
    return instance;
}

} // namespace nlm
