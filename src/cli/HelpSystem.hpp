// Comprehensive help system for NLM CLI
// Provides interactive help, examples, and documentation

#ifndef NLM_HELP_SYSTEM_HPP
#define NLM_HELP_SYSTEM_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace cli {

// Forward declarations
class Command;
class CommandLine;

// Help system configuration
struct HelpConfig {
    bool showExamples = true;
    bool showAdvancedOptions = true;
    bool showTips = true;
    bool interactiveMode = false;
    std::string format = "text";
    std::string theme = "default";
    size_t maxWidth = 80;
    bool wrapText = true;
};

// Command help information
struct CommandHelpInfo {
    std::string name;
    std::string description;
    std::string usage;
    std::string detailedDescription;
    CommandCategory category;
    std::vector<std::string> examples;
    std::vector<std::string> options;
    std::vector<std::string> notes;
    std::string seeAlso;
    
    CommandHelpInfo(const std::string& name, const std::string& desc,
                   const std::string& usage, CommandCategory cat)
        : name(name), description(desc), usage(usage), 
          detailedDescription(""), category(cat) {}
};

// Category information
struct CategoryInfo {
    CommandCategory category;
    std::string name;
    std::string description;
    std::vector<std::string> commands;
    
    CategoryInfo(CommandCategory cat, const std::string& name, const std::string& desc)
        : category(cat), name(name), description(desc) {}
};

// Global help information
struct GlobalHelpInfo {
    std::string programName;
    std::string version;
    std::string description;
    std::string usage;
    std::string copyright;
    std::string website;
    std::string documentationPath;
    std::vector<std::string> globalOptions;
    std::vector<std::string> tips;
    std::vector<std::string> examples;
};

// Comprehensive help system
class HelpSystem {
private:
    GlobalHelpInfo globalInfo;
    std::unordered_map<std::string, CommandHelpInfo> commandHelp;
    std::vector<CategoryInfo> categoryInfo;
    HelpConfig config;
    
    // Interactive help state
    bool interactiveMode;
    std::string currentCommand;
    std::vector<std::string> helpHistory;
    
public:
    HelpSystem();
    ~HelpSystem() = default;
    
    // Configuration
    void setConfig(const HelpConfig& cfg) { config = cfg; }
    const HelpConfig& getConfig() const { return config; }
    
    // Global help setup
    void setProgramInfo(const std::string& name, const std::string& version,
                       const std::string& description = "",
                       const std::string& copyright = "",
                       const std::string& website = "");
    void addGlobalOption(const std::string& option);
    void addTip(const std::string& tip);
    void addExample(const std::string& example);
    
    // Command help setup
    void registerCommand(const std::string& name, const std::string& description,
                        const std::string& usage, CommandCategory category);
    void setCommandDetailedDescription(const std::string& name,
                                       const std::string& description);
    void addCommandExample(const std::string& name, const std::string& example);
    void addCommandOption(const std::string& name, const std::string& option);
    void addCommandNote(const std::string& name, const std::string& note);
    void setCommandSeeAlso(const std::string& name, const std::string& seeAlso);
    
    // Category management
    void addCategory(CommandCategory category, const std::string& name,
                    const std::string& description);
    
    // Help display
    void showHelp(const std::string& commandName = "", bool interactive = false);
    void showCommandHelp(const std::string& commandName);
    void showGlobalHelp() const;
    void showCategoryHelp(CommandCategory category) const;
    
    // Interactive help
    void startInteractiveHelp();
    void processInteractiveCommand(const std::string& command);
    void showPreviousHelp() const;
    void clearHistory() { helpHistory.clear(); }
    
    // Help formatting and output
    void printFormattedHelp(const std::string& title, const std::string& content,
                           bool interactive = false) const;
    void printCommandDetails(const CommandHelpInfo& helpInfo) const;
    void printExamples(const std::vector<std::string>& examples) const;
    void printTips() const;
    void printQuickReference() const;
    
    // Help file operations
    bool saveHelpToFile(const std::string& filePath) const;
    bool loadHelpFromFile(const std::string& filePath);
    
    // Search and discovery
    std::vector<std::string> searchCommands(const std::string& query) const;
    std::vector<CommandCategory> getCategoriesWithCommands() const;
    std::vector<std::string> getCommandsInCategory(CommandCategory category) const;
    
    // History management
    void addToHistory(const std::string& command) {
        helpHistory.push_back(command);
        if (helpHistory.size() > 100) {
            helpHistory.erase(helpHistory.begin());
        }
    }
    const std::vector<std::string>& getHelpHistory() const { return helpHistory; }
    
    // Completion and suggestions
    std::vector<std::string> getCommandSuggestions(const std::string& partialCommand) const;
    std::vector<std::string> getOptionSuggestions(const std::string& commandName,
                                                 const std::string& partialOption) const;
    
    // Theme and formatting
    void setTheme(const std::string& theme) { config.theme = theme; }
    void setFormat(const std::string& format) { config.format = format; }
    void setMaxWidth(size_t width) { config.maxWidth = width; }
    
private:
    // Formatting helpers
    std::string wrapText(const std::string& text, size_t width) const;
    std::string getCategoryColor(CommandCategory category) const;
    std::string getHighlight(const std::string& text) const;
    
    // File I/O helpers
    static bool readFile(const std::string& filePath, std::string& content);
    static bool writeFile(const std::string& filePath, const std::string& content);
    
    // Interactive help processing
    void processHelpCommand(const std::vector<std::string>& parts);
    void processListCommand();
    void processSearchCommand(const std::string& query);
    void processCategoryCommand(const std::string& categoryName);
    void processExampleCommand(const std::string& commandName);
    
    // Command completion
    static std::vector<std::string> completeCommands(const std::string& partial);
    static std::vector<std::string> completeOptions(const std::string& commandName,
                                                   const std::string& partial);
};

// Command line help integration
class CommandLineHelp {
private:
    std::unique_ptr<HelpSystem> helpSystem;
    std::unique_ptr<CommandLine> commandLine;
    
public:
    CommandLineHelp(std::unique_ptr<CommandLine> cli, std::unique_ptr<HelpSystem> help)
        : commandLine(std::move(cli)), helpSystem(std::move(help)) {}
    
    // Automatic help integration
    bool autoGenerateHelp() const;
    void integrateWithCommandLine(CommandLine& cli) const;
    
    // Help file generation
    bool generateHtmlHelp(const std::string& outputPath) const;
    bool generateManPage(const std::string& outputPath) const;
    bool generateTextHelp(const std::string& outputPath) const;
    
    // Documentation generation
    void generateApiDocumentation(const std::string& outputPath) const;
    void generateExamplesDocumentation(const std::string& outputPath) const;
    void generateConfigurationDocumentation(const std::string& outputPath) const;
};

// Help content generators
class HelpContentGenerator {
public:
    // Generate help content from commands
    static std::string generateCommandHelp(const CommandHelpInfo& helpInfo);
    static std::string generateCategoryHelp(const CategoryInfo& categoryInfo);
    static std::string generateGlobalHelp(const GlobalHelpInfo& globalInfo);
    
    // Generate documentation
    static std::string generateApiDocumentation(const std::vector<CommandHelpInfo>& commands);
    static std::string generateExamplesDocumentation(const std::vector<std::string>& examples);
    static std::string generateConfigurationDocumentation(const HelpSystem& helpSystem);
    
    // Generate specific help types
    static std::string generateQuickStartGuide(const HelpSystem& helpSystem);
    static std::string generateAdvancedUsage(const HelpSystem& helpSystem);
    static std::string generateTroubleshootingGuide(const HelpSystem& helpSystem);
    
private:
    static std::string formatOption(const std::string& option);
    static std::string formatCommand(const std::string& command);
    static std::string escapeHtml(const std::string& text);
};

} // namespace cli
} // namespace nlm

#endif // NLM_HELP_SYSTEM_HPP
