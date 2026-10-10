// HelpSystem implementation

#include "HelpSystem.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace nlm {
namespace cli {

HelpSystem::HelpSystem() : interactiveMode(false) {
    // Set default program info
    setProgramInfo("nlm", "0.1.0", 
                   "Neural Learning Machine - Advanced CLI for neural simulations",
                   "Copyright (c) 2026 NLM Team",
                   "https://github.com/nlm/nlm");
}

void HelpSystem::setProgramInfo(const std::string& name, const std::string& version,
                               const std::string& description,
                               const std::string& copyright,
                               const std::string& website) {
    globalInfo.programName = name;
    globalInfo.version = version;
    globalInfo.description = description;
    globalInfo.copyright = copyright;
    globalInfo.website = website;
    
    // Set usage based on program name
    globalInfo.usage = "Usage: " + name + " <command> [OPTIONS]";
    
    // Set default options
    globalInfo.globalOptions = {
        "--help, -h          Show this help message",
        "--version           Show version information",
        "--verbose, -v       Enable verbose output",
        "--quiet, -q         Suppress non-essential output",
        "--config FILE        Configuration file path",
        "--output FILE        Output file path",
        "--output-format FMT  Output format (text, json, csv)",
        "--profile            Enable profiling",
        "--benchmark          Enable benchmarking",
        "--threads N          Number of threads to use"
    };
    
    // Set default tips
    globalInfo.tips = {
        "Use 'nlm <command> --help' for detailed command help",
        "Configuration files can be specified with --config",
        "Output formats supported: text, json, csv",
        "Enable profiling with --profile for performance analysis",
        "Use --verbose for detailed output during execution"
    };
    
    // Set default examples
    globalInfo.examples = {
        "nlm simulation --neuron-count 1000",
        "nlm config --config myconfig.cfg",
        "nlm benchmark --profile --output results.json"
    };
}

void HelpSystem::addGlobalOption(const std::string& option) {
    globalInfo.globalOptions.push_back(option);
}

void HelpSystem::addTip(const std::string& tip) {
    globalInfo.tips.push_back(tip);
}

void HelpSystem::addExample(const std::string& example) {
    globalInfo.examples.push_back(example);
}

void HelpSystem::registerCommand(const std::string& name, const std::string& description,
                                const std::string& usage, CommandCategory category) {
    CommandHelpInfo helpInfo(name, description, usage, category);
    commandHelp[name] = helpInfo;
    
    // Add to category info
    auto it = std::find_if(categoryInfo.begin(), categoryInfo.end(),
                          [category](const CategoryInfo& cat) { return cat.category == category; });
    if (it == categoryInfo.end()) {
        std::string catName;
        std::string catDesc;
        
        switch (category) {
            case CommandCategory::SIMULATION: catName = "simulation"; catDesc = "Neural simulation commands"; break;
            case CommandCategory::CONFIGURATION: catName = "configuration"; catDesc = "Configuration management commands"; break;
            case CommandCategory::DEBUGGING: catName = "debugging"; catDesc = "Debugging and profiling commands"; break;
            case CommandCategory::VALIDATION: catName = "validation"; catDesc = "System validation commands"; break;
            case CommandCategory::PLUGIN: catName = "plugin"; catDesc = "Plugin management commands"; break;
            case CommandCategory::PERFORMANCE: catName = "performance"; catDesc = "Performance analysis commands"; break;
            case CommandCategory::ANALYSIS: catName = "analysis"; catDesc = "System analysis commands"; break;
            case CommandCategory::UTILITIES: catName = "utilities"; catDesc = "Utility commands"; break;
        }
        
        categoryInfo.push_back(CategoryInfo(category, catName, catDesc));
        it = categoryInfo.end() - 1;
    }
    
    it->commands.push_back(name);
}

void HelpSystem::setCommandDetailedDescription(const std::string& name,
                                               const std::string& description) {
    auto it = commandHelp.find(name);
    if (it != commandHelp.end()) {
        it->second.detailedDescription = description;
    }
}

void HelpSystem::addCommandExample(const std::string& name, const std::string& example) {
    auto it = commandHelp.find(name);
    if (it != commandHelp.end()) {
        it->second.examples.push_back(example);
    }
}

void HelpSystem::addCommandOption(const std::string& name, const std::string& option) {
    auto it = commandHelp.find(name);
    if (it != commandHelp.end()) {
        it->second.options.push_back(option);
    }
}

void HelpSystem::addCommandNote(const std::string& name, const std::string& note) {
    auto it = commandHelp.find(name);
    if (it != commandHelp.end()) {
        it->second.notes.push_back(note);
    }
}

void HelpSystem::setCommandSeeAlso(const std::string& name, const std::string& seeAlso) {
    auto it = commandHelp.find(name);
    if (it != commandHelp.end()) {
        it->second.seeAlso = seeAlso;
    }
}

void HelpSystem::addCategory(CommandCategory category, const std::string& name,
                            const std::string& description) {
    auto it = std::find_if(categoryInfo.begin(), categoryInfo.end(),
                          [category](const CategoryInfo& cat) { return cat.category == category; });
    if (it == categoryInfo.end()) {
        categoryInfo.push_back(CategoryInfo(category, name, description));
    } else {
        it->name = name;
        it->description = description;
    }
}

void HelpSystem::showHelp(const std::string& commandName, bool interactive) {
    interactiveMode = interactive;
    currentCommand = commandName;
    addToHistory("help " + commandName);
    
    if (commandName.empty()) {
        showGlobalHelp();
    } else {
        showCommandHelp(commandName);
    }
}

void HelpSystem::showCommandHelp(const std::string& commandName) {
    auto it = commandHelp.find(commandName);
    if (it == commandHelp.end()) {
        std::cout << "Error: Unknown command '" << commandName << "'" << std::endl;
        return;
    }
    
    const CommandHelpInfo& helpInfo = it->second;
    printCommandDetails(helpInfo);
}

void HelpSystem::showGlobalHelp() const {
    printFormattedHelp("NLM - Neural Learning Machine", "", false);
    
    std::cout << "Commands:" << std::endl;
    std::cout << "========" << std::endl;
    std::cout << std::endl;
    
    // Group by category
    for (const auto& category : categoryInfo) {
        std::cout << category.name << ":" << std::endl;
        for (const auto& cmdName : category.commands) {
            auto cmdIt = commandHelp.find(cmdName);
            if (cmdIt != commandHelp.end()) {
                std::cout << "  " << cmdName << " - " << cmdIt->second.description << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    showTips();
}

void HelpSystem::showCategoryHelp(CommandCategory category) const {
    auto it = std::find_if(categoryInfo.begin(), categoryInfo.end(),
                          [category](const CategoryInfo& cat) { return cat.category == category; });
    if (it == categoryInfo.end()) {
        std::cout << "Unknown category" << std::endl;
        return;
    }
    
    std::cout << it->name << " commands:" << std::endl;
    std::cout << std::string(it->name.length() + 9, '=') << std::endl;
    std::cout << std::endl;
    
    for (const auto& cmdName : it->commands) {
        auto cmdIt = commandHelp.find(cmdName);
        if (cmdIt != commandHelp.end()) {
            std::cout << cmdName << ": " << cmdIt->second.description << std::endl;
            if (!cmdIt->second.usage.empty()) {
                std::cout << "  Usage: " << cmdIt->second.usage << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

void HelpSystem::startInteractiveHelp() {
    interactiveMode = true;
    printFormattedHelp("Interactive Help", 
                      "Type 'help <command>' for command details\n"
                      "Type 'list' to see all commands\n"
                      "Type 'search <query>' to search commands\n"
                      "Type 'category <name>' to see category commands\n"
                      "Type 'tips' to see helpful tips\n"
                      "Type 'exit' to return to command mode", true);
}

void HelpSystem::processInteractiveCommand(const std::string& command) {
    std::vector<std::string> parts = splitString(command);
    std::string cmd = toLower(parts[0]);
    
    if (cmd == "exit" || cmd == "quit") {
        interactiveMode = false;
        return;
    } else if (cmd == "help") {
        if (parts.size() > 1) {
            showCommandHelp(parts[1]);
        } else {
            showGlobalHelp();
        }
    } else if (cmd == "list") {
        showGlobalHelp();
    } else if (cmd == "search") {
        if (parts.size() > 1) {
            std::string query;
            for (size_t i = 1; i < parts.size(); ++i) {
                if (i > 1) query += " ";
                query += parts[i];
            }
            std::vector<std::string> results = searchCommands(query);
            std::cout << "Search results for '" << query << "':" << std::endl;
            for (const auto& result : results) {
                std::cout << "  " << result << std::endl;
            }
        } else {
            std::cout << "Please provide a search query" << std::endl;
        }
    } else if (cmd == "category") {
        if (parts.size() > 1) {
            std::string categoryName = toLower(parts[1]);
            auto it = std::find_if(categoryInfo.begin(), categoryInfo.end(),
                                  [categoryName](const CategoryInfo& cat) {
                                      std::string lowerName = toLower(cat.name);
                                      return lowerName == categoryName;
                                  });
            if (it != categoryInfo.end()) {
                showCategoryHelp(it->category);
            } else {
                std::cout << "Unknown category: " << parts[1] << std::endl;
            }
        } else {
            std::cout << "Please specify a category name" << std::endl;
        }
    } else if (cmd == "tips") {
        showTips();
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
        std::cout << "Available commands: help, list, search, category, tips, exit" << std::endl;
    }
    
    addToHistory(command);
}

void HelpSystem::showPreviousHelp() const {
    if (!helpHistory.empty()) {
        std::cout << "Previous help queries:" << std::endl;
        for (const auto& query : helpHistory) {
            std::cout << "  " << query << std::endl;
        }
    }
}

void HelpSystem::printFormattedHelp(const std::string& title, const std::string& content,
                                   bool interactive) const {
    std::cout << std::string(config.maxWidth, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(config.maxWidth, '=') << std::endl;
    std::cout << std::endl;
    
    if (!content.empty()) {
        std::cout << content << std::endl;
        std::cout << std::endl;
    }
    
    if (interactive) {
        std::cout << "Press Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

void HelpSystem::printCommandDetails(const CommandHelpInfo& helpInfo) const {
    std::cout << "Command: " << helpInfo.name << std::endl;
    std::cout << "Category: " << getCategoryName(helpInfo.category) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Description:" << std::endl;
    std::cout << "  " << helpInfo.description << std::endl;
    std::cout << std::endl;
    
    if (!helpInfo.detailedDescription.empty()) {
        std::cout << "Detailed Description:" << std::endl;
        std::cout << "  " << wrapText(helpInfo.detailedDescription, 70) << std::endl;
        std::cout << std::endl;
    }
    
    if (!helpInfo.usage.empty()) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << helpInfo.usage << std::endl;
        std::cout << std::endl;
    }
    
    if (!helpInfo.options.empty()) {
        std::cout << "Options:" << std::endl;
        for (const auto& option : helpInfo.options) {
            std::cout << "  " << option << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (!helpInfo.examples.empty()) {
        std::cout << "Examples:" << std::endl;
        for (const auto& example : helpInfo.examples) {
            std::cout << "  " << example << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (!helpInfo.notes.empty()) {
        std::cout << "Notes:" << std::endl;
        for (const auto& note : helpInfo.notes) {
            std::cout << "  " << wrapText(note, 70) << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (!helpInfo.seeAlso.empty()) {
        std::cout << "See Also:" << std::endl;
        std::cout << "  " << helpInfo.seeAlso << std::endl;
        std::cout << std::endl;
    }
}

void HelpSystem::printExamples(const std::vector<std::string>& examples) const {
    std::cout << "Examples:" << std::endl;
    for (const auto& example : examples) {
        std::cout << "  " << example << std::endl;
    }
    std::cout << std::endl;
}

void HelpSystem::showTips() const {
    if (config.showTips) {
        std::cout << "Tips:" << std::endl;
        for (const auto& tip : globalInfo.tips) {
            std::cout << "  " << wrapText(tip, 70) << std::endl;
        }
        std::cout << std::endl;
    }
}

void HelpSystem::printQuickReference() const {
    std::cout << "Quick Reference:" << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Global Options:" << std::endl;
    for (const auto& option : globalInfo.globalOptions) {
        std::cout << "  " << option << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Command Categories:" << std::endl;
    for (const auto& category : categoryInfo) {
        std::cout << "  " << category.name << ": " << category.description << std::endl;
    }
    std::cout << std::endl;
}

bool HelpSystem::saveHelpToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Help Documentation" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Global Help:" << std::endl;
    file << globalInfo.programName << " " << globalInfo.version << std::endl;
    file << globalInfo.description << std::endl;
    file << std::endl;
    
    file << "Commands:" << std::endl;
    for (const auto& [name, helpInfo] : commandHelp) {
        file << "  " << name << ": " << helpInfo.description << std::endl;
    }
    
    file.close();
    return true;
}

bool HelpSystem::loadHelpFromFile(const std::string& filePath) {
    return readFile(filePath, content);
}

std::vector<std::string> HelpSystem::searchCommands(const std::string& query) const {
    std::vector<std::string> results;
    std::string lowerQuery = toLower(query);
    
    for (const auto& [name, helpInfo] : commandHelp) {
        if (toLower(name).find(lowerQuery) != std::string::npos ||
            toLower(helpInfo.description).find(lowerQuery) != std::string::npos) {
            results.push_back(name);
        }
    }
    
    return results;
}

std::vector<CommandCategory> HelpSystem::getCategoriesWithCommands() const {
    std::vector<CommandCategory> categories;
    for (const auto& category : categoryInfo) {
        categories.push_back(category.category);
    }
    return categories;
}

std::vector<std::string> HelpSystem::getCommandsInCategory(CommandCategory category) const {
    auto it = std::find_if(categoryInfo.begin(), categoryInfo.end(),
                          [category](const CategoryInfo& cat) { return cat.category == category; });
    if (it == categoryInfo.end()) {
        return {};
    }
    
    return it->commands;
}

std::vector<std::string> HelpSystem::getCommandSuggestions(const std::string& partialCommand) const {
    std::vector<std::string> suggestions;
    std::string lowerPartial = toLower(partialCommand);
    
    for (const auto& name : getAllCommandNames()) {
        if (name.find(lowerPartial) != std::string::npos) {
            suggestions.push_back(name);
        }
    }
    
    return suggestions;
}

std::vector<std::string> HelpSystem::getOptionSuggestions(const std::string& commandName,
                                                         const std::string& partialOption) const {
    std::vector<std::string> suggestions;
    
    auto it = commandHelp.find(commandName);
    if (it == commandHelp.end()) {
        return suggestions;
    }
    
    std::string lowerPartial = toLower(partialOption);
    
    for (const auto& option : it->second.options) {
        if (toLower(option).find(lowerPartial) != std::string::npos) {
            suggestions.push_back(option);
        }
    }
    
    return suggestions;
}

std::string HelpSystem::wrapText(const std::string& text, size_t width) const {
    if (!config.wrapText || text.length() <= width) {
        return text;
    }
    
    std::stringstream ss;
    std::istringstream iss(text);
    std::string line;
    size_t currentLength = 0;
    
    while (std::getline(iss, line, ' ')) {
        if (currentLength + line.length() + 1 > width) {
            ss << std::endl << line << " ";
            currentLength = line.length() + 1;
        } else {
            if (currentLength > 0) ss << " ";
            ss << line;
            currentLength += line.length() + 1;
        }
    }
    
    return ss.str();
}

std::string HelpSystem::getCategoryColor(CommandCategory category) const {
    // ANSI color codes for different categories
    switch (category) {
        case CommandCategory::SIMULATION: return "\033[1;34m"; // Blue
        case CommandCategory::CONFIGURATION: return "\033[1;32m"; // Green
        case CommandCategory::DEBUGGING: return "\033[1;31m"; // Red
        case CommandCategory::VALIDATION: return "\033[1;33m"; // Yellow
        case CommandCategory::PLUGIN: return "\033[1;35m"; // Magenta
        case CommandCategory::PERFORMANCE: return "\033[1;36m"; // Cyan
        case CommandCategory::ANALYSIS: return "\033[1;37m"; // White
        case CommandCategory::UTILITIES: return "\033[1;30m"; // Black
        default: return "\033[0m"; // Default
    }
}

std::string HelpSystem::getHighlight(const std::string& text) const {
    return "\033[1;33m" + text + "\033[0m"; // Yellow bold
}

bool HelpSystem::readFile(const std::string& filePath, std::string& content) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    return true;
}

bool HelpSystem::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::vector<std::string> HelpSystem::splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string HelpSystem::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string HelpSystem::getCategoryName(CommandCategory category) const {
    switch (category) {
        case CommandCategory::SIMULATION: return "Simulation";
        case CommandCategory::CONFIGURATION: return "Configuration";
        case CommandCategory::DEBUGGING: return "Debugging";
        case CommandCategory::VALIDATION: return "Validation";
        case CommandCategory::PLUGIN: return "Plugin";
        case CommandCategory::PERFORMANCE: return "Performance";
        case CommandCategory::ANALYSIS: return "Analysis";
        case CommandCategory::UTILITIES: return "Utilities";
        default: return "Unknown";
    }
}

std::vector<std::string> HelpSystem::getAllCommandNames() const {
    std::vector<std::string> names;
    for (const auto& [name, helpInfo] : commandHelp) {
        names.push_back(name);
    }
    return names;
}

// CommandLineHelp implementation

bool CommandLineHelp::autoGenerateHelp() const {
    // Generate comprehensive help documentation
    generateHtmlHelp("docs/cli/api/html/help.html");
    generateManPage("docs/cli/api/man/nlm.1");
    generateTextHelp("docs/cli/api/text/help.txt");
    
    // Generate API documentation
    generateApiDocumentation("docs/cli/api/html/api.html");
    generateExamplesDocumentation("docs/cli/api/examples.html");
    generateConfigurationDocumentation("docs/cli/api/config.html");
    
    return true;
}

void CommandLineHelp::integrateWithCommandLine(CommandLine& cli) const {
    // Register all commands with the help system
    for (const auto& cmd : cli.getAllCommands()) {
        helpSystem->registerCommand(cmd->getName(), cmd->getDescription(),
                                   cmd->getUsage(), cmd->getCategory());
        
        // Add examples if available
        auto examples = cmd->getExamples();
        for (const auto& example : examples) {
            helpSystem->addCommandExample(cmd->getName(), example);
        }
    }
}

bool CommandLineHelp::generateHtmlHelp(const std::string& outputPath) const {
    // Generate HTML help documentation
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>NLM - Command Line Interface Help</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    file << "        h1 { color: #3366cc; }" << std::endl;
    file << "        h2 { color: #333; }" << std::endl;
    file << "        .command { background-color: #f5f5f5; padding: 10px; margin: 10px 0; }" << std::endl;
    file << "        .option { margin-left: 20px; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>NLM - Neural Learning Machine CLI Help</h1>" << std::endl;
    file << "    <p>" << globalInfo.description << "</p>" << std::endl;
    file << "    <h2>Commands</h2>" << std::endl;
    
    for (const auto& [name, helpInfo] : commandHelp) {
        file << "    <div class=\"command\">" << std::endl;
        file << "        <h3>" << name << "</h3>" << std::endl;
        file << "        <p><strong>Category:</strong> " << getCategoryName(helpInfo.category) << "</p>" << std::endl;
        file << "        <p><strong>Description:</strong> " << helpInfo.description << "</p>" << std::endl;
        
        if (!helpInfo.usage.empty()) {
            file << "        <p><strong>Usage:</strong> " << helpInfo.usage << "</p>" << std::endl;
        }
        
        if (!helpInfo.examples.empty()) {
            file << "        <p><strong>Examples:</strong></p>" << std::endl;
            file << "        <ul>" << std::endl;
            for (const auto& example : helpInfo.examples) {
                file << "            <li>" << example << "</li>" << std::endl;
            }
            file << "        </ul>" << std::endl;
        }
        
        file << "    </div>" << std::endl;
    }
    
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

bool CommandLineHelp::generateManPage(const std::string& outputPath) const {
    // Generate man page format
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << ".TH NLM 1 \"NLM Command Line Interface\"" << std::endl;
    file << ".SH NAME" << std::endl;
    file << globalInfo.programName << " - " << globalInfo.description << std::endl;
    file << ".SH SYNOPSIS" << std::endl;
    file << "\n";
    file << globalInfo.usage << std::endl;
    file << ".SH DESCRIPTION" << std::endl;
    file << globalInfo.description << std::endl;
    file << ".SH OPTIONS" << std::endl;
    
    for (const auto& option : globalInfo.globalOptions) {
        file << ".TP" << std::endl;
        file << option << std::endl;
    }
    
    file << ".SH COMMANDS" << std::endl;
    for (const auto& [name, helpInfo] : commandHelp) {
        file << ".SS " << name << std::endl;
        file << helpInfo.description << std::endl;
        if (!helpInfo.usage.empty()) {
            file << "Usage: " << helpInfo.usage << std::endl;
        }
    }
    
    file.close();
    return true;
}

bool CommandLineHelp::generateTextHelp(const std::string& outputPath) const {
    // Generate plain text help file
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM - Neural Learning Machine CLI Help" << std::endl;
    file << "=======================================" << std::endl;
    file << std::endl;
    file << globalInfo.description << std::endl;
    file << std::endl;
    
    file << "Global Options:" << std::endl;
    for (const auto& option : globalInfo.globalOptions) {
        file << "  " << option << std::endl;
    }
    file << std::endl;
    
    file << "Commands:" << std::endl;
    for (const auto& [name, helpInfo] : commandHelp) {
        file << "  " << name << " - " << helpInfo.description << std::endl;
    }
    file << std::endl;
    
    file << "Tips:" << std::endl;
    for (const auto& tip : globalInfo.tips) {
        file << "  " << tip << std::endl;
    }
    file << std::endl;
    
    file.close();
    return true;
}

void CommandLineHelp::generateApiDocumentation(const std::string& outputPath) const {
    // Generate API documentation
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM CLI API Documentation" << std::endl;
    file << "===========================" << std::endl;
    file << std::endl;
    
    for (const auto& [name, helpInfo] : commandHelp) {
        file << "Command: " << name << std::endl;
        file << "Category: " << getCategoryName(helpInfo.category) << std::endl;
        file << "Description: " << helpInfo.description << std::endl;
        file << "Usage: " << helpInfo.usage << std::endl;
        file << std::endl;
    }
    
    file.close();
}

void CommandLineHelp::generateExamplesDocumentation(const std::string& outputPath) const {
    // Generate examples documentation
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM CLI Examples" << std::endl;
    file << "=================" << std::endl;
    file << std::endl;
    
    for (const auto& example : globalInfo.examples) {
        file << example << std::endl;
    }
    file << std::endl;
    
    for (const auto& [name, helpInfo] : commandHelp) {
        if (!helpInfo.examples.empty()) {
            file << "Command: " << name << std::endl;
            for (const auto& example : helpInfo.examples) {
                file << "  " << example << std::endl;
            }
            file << std::endl;
        }
    }
    
    file.close();
}

void CommandLineHelp::generateConfigurationDocumentation(const std::string& outputPath) const {
    // Generate configuration documentation
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Configuration" << std::endl;
    file << "===================" << std::endl;
    file << std::endl;
    file << "Configuration files can be specified with the --config option." << std::endl;
    file << "Environment variables can also be used." << std::endl;
    file << std::endl;
    file << "Default configuration file: configs/default.cfg" << std::endl;
    file << std::endl;
    
    file.close();
}

} // namespace cli
} // namespace nlm
