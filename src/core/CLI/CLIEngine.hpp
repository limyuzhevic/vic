// CLIEngine.hpp - Main CLI engine using CLI11
// Main CLI logic for NLM command-line interface

#pragma once

#include <CLI/CLI.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include "core/Config/Config.hpp"

namespace nlm {

class CLIEngine {
public:
    CLIEngine();
    virtual ~CLIEngine() = default;
    
    // Add CLI commands and options
    void setupCLI(CLI::App& app);
    
    // Execute CLI commands
    int execute(int argc, char** argv);
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Set output format
    void setOutputFormat(const std::string& format) { outputFormat_ = format; }
    std::string getOutputFormat() const { return outputFormat_; }
    
    // Set logging level
    void setLogLevel(const std::string& level) { logLevel_ = level; }
    std::string getLogLevel() const { return logLevel_; }
    
    // Set verbose mode
    void setVerbose(bool verbose) { verbose_ = verbose; }
    bool isVerbose() const { return verbose_; }
    
    // Set quiet mode
    void setQuiet(bool quiet) { quiet_ = quiet; }
    bool isQuiet() const { return quiet_; }
    
    // Set progress reporting
    void setProgress(bool progress) { progress_ = progress; }
    bool isProgress() const { return progress_; }
    
    // Set dry run mode
    void setDryRun(bool dryRun) { dryRun_ = dryRun; }
    bool isDryRun() const { return dryRun_; }
    
    // Set config file
    void setConfigFile(const std::string& configFile) { configFile_ = configFile; }
    std::string getConfigFile() const { return configFile_; }
    
    // Set preset
    void setPreset(const std::string& preset) { preset_ = preset; }
    std::string getPreset() const { return preset_; }
    
    // Set validation mode
    void setValidateOnly(bool validateOnly) { validateOnly_ = validateOnly; }
    bool isValidateOnly() const { return validateOnly_; }
    
    // Set benchmark mode
    void setBenchmark(bool benchmark) { benchmark_ = benchmark; }
    bool isBenchmark() const { return benchmark_; }
    
    // Set help flag
    void setHelp(bool help) { help_ = help; }
    bool isHelp() const { return help_; }
    
private:
    // CLI11 app instance
    CLI::App app_;
    
    // Core configuration
    std::shared_ptr<Config> config_;
    
    // Output and logging
    std::string outputFormat_ = "text";
    std::string logLevel_ = "info";
    bool verbose_ = false;
    bool quiet_ = false;
    bool progress_ = false;
    
    // Operation modes
    bool dryRun_ = false;
    bool validateOnly_ = false;
    bool benchmark_ = false;
    bool help_ = false;
    
    // Configuration
    std::string configFile_ = "configs/default.cfg";
    std::string preset_ = "";
    
    // Environment variables
    std::unordered_map<std::string, std::string> envVars_;
    
    // Command subcommands
    CLI::Subcommand* configSubcommand_ = nullptr;
    CLI::Subcommand* runSubcommand_ = nullptr;
    CLI::Subcommand* validateSubcommand_ = nullptr;
    CLI::Subcommand* exportSubcommand_ = nullptr;
    CLI::Subcommand* benchmarkSubcommand_ = nullptr;
};

} // namespace nlm