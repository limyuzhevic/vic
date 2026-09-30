// Enhanced help system for NLM command-line interface
// Provides comprehensive help with formatting, examples, and contextual information

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <sstream>
#include "core/CommandLine/CommandLineOptions.hpp"
#include "core/CommandLine/CommandLineParser.hpp"

namespace nlm {

// Help section types
enum class HelpSection {
    Overview,
    Performance,
    Debugging,
    Experiment,
    Validation,
    Output,
    Neural,
    Memory,
    Checkpoint,
    Configuration,
    Examples,
    CommonPatterns,
    Troubleshooting
};

// Help formatter with multiple output styles
class CommandLineHelp {
public:
    CommandLineHelp() = default;
    
    // Set the options instance
    void setOptions(const CommandLineOptions* options) { this->options = options; }
    
    // Display full help
    void displayFullHelp() const;
    
    // Display help for a specific section
    void displaySectionHelp(HelpSection section) const;
    
    // Display brief help (usage only)
    void displayBriefHelp() const;
    
    // Display help for specific argument
    void displayArgumentHelp(const std::string& argumentName) const;
    
    // Display option examples
    void displayOptionExamples() const;
    
    // Display common patterns
    void displayCommonPatterns() const;
    
    // Display troubleshooting guide
    void displayTroubleshooting() const;
    
    // Set output stream
    void setOutputStream(std::ostream& stream) { outputStream = &stream; }
    
    // Generate help for argument (internal)
    std::string generateArgumentHelp(const std::string& argumentName) const;
    
    // Generate examples for argument (internal)
    std::string generateArgumentExamples(const std::string& argumentName) const;
    
private:
    const CommandLineOptions* options = nullptr;
    std::ostream* outputStream = &std::cout;
    
    // Display section header
    void displaySectionHeader(HelpSection section) const;
    
    // Display argument details
    void displayArgumentDetails(const OptionMetadata& option) const;
    
    // Format option name
    std::string formatOptionName(const std::string& longName, const std::string& shortName) const;
    
    // Format type
    std::string formatType(const std::string& type) const;
    
    // Get section title
    std::string getSectionTitle(HelpSection section) const;
    
    // Get section description
    std::string getSectionDescription(HelpSection section) const;
    
    // Check if section is available
    bool isSectionAvailable(HelpSection section) const;
    
    // Get example command for section
    std::string getExampleCommand(HelpSection section) const;
};

// Help text templates and utilities
class HelpText {
public:
    static std::string getOverviewText();
    static std::string getUsageText();
    static std::string getPerformanceDescription() { return "Performance tuning options to optimize computational efficiency and speed."; }
    static std::string getDebuggingDescription() { return "Debugging and profiling options for development and troubleshooting."; }
    static std::string getExperimentDescription() { return "Experiment configuration options for controlled testing environments."; }
    static std::string getValidationDescription() { return "Configuration validation and safety options."; }
    static std::string getOutputDescription() { return "Output formatting and logging options."; }
    static std::string getNeuralDescription() { return "Neural network architecture and connection options."; }
    static std::string getMemoryDescription() { return "Memory management and optimization options."; }
    static std::string getCheckpointDescription() { return "Checkpoint saving and restoration options."; }
    
    // Get option help text by long name
    static std::string getOptionHelp(const std::string& longName);
    
    // Get example commands
    static std::vector<std::string> getExampleCommands(HelpSection section);
    
    // Get common pattern
    static std::string getCommonPattern(const std::string& patternName);
    
    // Get troubleshooting tip
    static std::string getTroubleshootingTip(const std::string& issue);
    
private:
    static std::unordered_map<std::string, std::string> optionHelpTexts;
    static std::unordered_map<HelpSection, std::vector<std::string>> sectionExamples;
    static std::unordered_map<std::string, std::string> commonPatterns;
    static std::unordered_map<std::string, std::string> troubleshootingTips;
    
    static void initializeOptionHelpTexts();
    static void initializeSectionExamples();
    static void initializeCommonPatterns();
    static void initializeTroubleshootingTips();
};

// Global help instance
CommandLineHelp& getCommandLineHelp();

} // namespace nlm