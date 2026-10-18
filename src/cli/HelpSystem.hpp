// CLI Help System
//
// This file provides the help and documentation system for NLM command-line tools.
// It handles help text generation, examples, and command reference documentation.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

#include "CLICommand.hpp"
#include "CLIEngine.hpp"

namespace nlm {

class HelpSystem {
public:
    HelpSystem();
    ~HelpSystem() = default;
    
    // Help text generation
    std::string generateHelpText(const CLICommand& command) const;
    std::string generateCommandListText(const std::vector<std::string>& commands) const;
    std::string generateCategoryHelpText(CLICommand::CommandType type) const;
    
    // Documentation management
    bool loadHelpFile(const std::string& commandName, const std::string& filePath);
    bool saveHelpFile(const std::string& commandName, const std::string& filePath) const;
    
    // Tutorial system
    void displayTutorial(const std::string& topic) const;
    void displayCommandExamples(const std::string& commandName) const;
    
    // Command reference
    std::string generateCommandReference() const;
    std::string generateConfigurationReference() const;
    std::string generateExperimentReference() const;
    
    // Documentation file generation
    void generateHtmlDocumentation(const std::string& outputPath) const;
    void generateManPages(const std::string& outputPath) const;
    void generatePdfDocumentation(const std::string& outputPath) const;
    
    // Help formatting
    std::string formatCommandUsage(const std::string& commandName, 
                                   const std::vector<std::string>& examples) const;
    std::string formatOptionHelp(const std::string& optionName,
                                const std::string& description,
                                const std::string& defaultValue,
                                bool advanced) const;
    
    // Search and filtering
    std::vector<std::string> searchCommands(const std::string& query) const;
    std::vector<std::string> getCommandsByType(CLICommand::CommandType type) const;
    std::vector<std::string> getCommandsByKeyword(const std::string& keyword) const;
    
private:
    // Help text templates
    static const std::string HELP_TEMPLATE;
    static const std::string COMMAND_LIST_TEMPLATE;
    static const std::string OPTION_HELP_TEMPLATE;
    
    // Documentation data
    std::map<std::string, std::string> commandHelpFiles_;
    std::map<std::string, std::vector<std::string>> commandExamples_;
    std::map<std::string, std::string> commandReferences_;
    
    // Help system configuration
    bool autoGenerateHelp_;
    bool includeAdvancedCommands_;
    std::string helpOutputDirectory_;
    std::string documentationLanguage_;
    
    // Internal helper methods
    std::string escapeHtml(const std::string& text) const;
    std::string formatExamples(const std::vector<std::string>& examples) const;
    std::string generateExampleCommand(const std::string& commandName,
                                       const std::vector<std::string>& args) const;
};

// Example commands for different types
namespace CommandExamples {
    const std::vector<std::string> CONFIG_COMMANDS = {
        "nlm-cli config-info",
        "nlm-cli config-validate configs/default.json",
        "nlm-cli config-merge configs/*.json merged.json",
        "nlm-cli config-export --format yaml config.yaml"
    };
    
    const std::vector<std::string> EXPERIMENT_COMMANDS = {
        "nlm-cli exp-info learning_experiment",
        "nlm-cli exp-run --experiment my_exp --config configs/experiment.json",
        "nlm-cli exp-compare results1.json results2.json",
        "nlm-cli exp-export results.json --format csv"
    };
    
    const std::vector<std::string> BRAIN_COMMANDS = {
        "nlm-cli brain-info",
        "nlm-cli brain-save brain_state.json",
        "nlm-cli brain-load saved_state.json",
        "nlm-cli brain-compare brain1.json brain2.json"
    };
    
    const std::vector<std::string> WORLD_COMMANDS = {
        "nlm-cli world-info",
        "nlm-cli world-save world_state.json",
        "nlm-cli world-load saved_world.json",
        "nlm-cli world-test --config test_config.json"
    };
    
    const std::vector<std::string> ANALYSIS_COMMANDS = {
        "nlm-cli analyze-metrics --input results.csv --output analysis.png",
        "nlm-cli analyze-performance --benchmark benchmark.json",
        "nlm-cli analyze-results --input exp_results.json",
        "nlm-cli analyze-memory --threshold 1024"
    };
    
    const std::vector<std::string> UTILITY_COMMANDS = {
        "nlm-cli version",
        "nlm-cli health-check",
        "nlm-cli stats",
        "nlm-cli logs --tail 100",
        "nlm-cli benchmark"
    };
    
    const std::vector<std::string> VISUALIZATION_COMMANDS = {
        "nlm-cli viz-simulation --input simulation.json",
        "nlm-cli viz-results --input results.json",
        "nlm-cli viz-brain --input brain.json",
        "nlm-cli viz-metrics --input metrics.csv"
    };
    
    const std::vector<std::string> DEVELOPMENT_COMMANDS = {
        "nlm-cli debug --session current",
        "nlm-cli trace --enable",
        "nlm-cli profile --start",
        "nlm-cli monitor --watch"
    };
    
    const std::vector<std::string> ADVANCED_COMMANDS = {
        "nlm-cli batch-exec commands.txt",
        "nlm-cli parallel-exec exp-run --experiment exp1",
        "nlm-cli distributed --host cluster1",
        "nlm-cli scale-out --nodes 10"
    };
}

} // namespace nlm