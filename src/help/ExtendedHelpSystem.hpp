// Extended Help System
// This class provides advanced help and documentation capabilities for NLM power users

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace help {

class ExtendedHelpSystem {
public:
    ExtendedHelpSystem();
    ~ExtendedHelpSystem();
    
    // Help content management
    void loadHelpContent(const std::string& directory);
    bool loadHelpTopic(const std::string& topic);
    std::string getHelpContent(const std::string& topic) const;
    
    // Help system navigation
    std::vector<std::string> getAvailableTopics() const;
    std::vector<std::string> getSubtopics(const std::string& topic) const;
    std::vector<std::string> getRelatedTopics(const std::string& topic) const;
    
    // Help display
    void displayHelp(const std::string& topic, const std::map<std::string, std::string>& options = {});
    void displayHelp(const std::string& topic, const std::vector<std::string>& arguments = {});
    void displayAdvancedHelp(const std::string& topic);
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partialCommand, 
                                                   const std::map<std::string, std::string>& currentArgs = {});
    bool resolveCommandAliases(const std::string& command, std::string& resolvedCommand);
    
    // Context-sensitive help
    std::string getContextHelp(const std::string& command, const std::string& context);
    void setContext(const std::string& command, const std::string& context);
    void clearContext();
    
    // Help examples and tutorials
    std::string getExample(const std::string& exampleName) const;
    std::vector<std::string> getAvailableExamples() const;
    std::string getTutorial(const std::string& tutorialName) const;
    std::vector<std::string> getAvailableTutorials() const;
    
    // Help system configuration
    bool enableAdvancedHelp() { advancedHelpEnabled_ = true; return true; }
    bool disableAdvancedHelp() { advancedHelpEnabled_ = false; return true; }
    bool isAdvancedHelpEnabled() const { return advancedHelpEnabled_; }
    
    bool enableCommandCompletion() { commandCompletionEnabled_ = true; return true; }
    bool disableCommandCompletion() { commandCompletionEnabled_ = false; return true; }
    bool isCommandCompletionEnabled() const { return commandCompletionEnabled_; }
    
    bool enableContextSensitiveHelp() { contextSensitiveHelpEnabled_ = true; return true; }
    bool disableContextSensitiveHelp() { contextSensitiveHelpEnabled_ = false; return true; }
    bool isContextSensitiveHelpEnabled() const { return contextSensitiveHelpEnabled_; }
    
    // Help system statistics
    size_t getTotalHelpTopics() const;
    size_t getTotalExamples() const;
    size_t getTotalTutorials() const;
    double getHelpSystemUsage() const;
    
    // Help system maintenance
    bool rebuildHelpIndex();
    bool updateHelpContent(const std::string& topic, const std::string& content);
    bool removeHelpTopic(const std::string& topic);
    bool exportHelpToFormat(const std::string& format, const std::string& outputFile);
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    bool hasError() const { return !lastError_.empty(); }
    
private:
    // Help content storage
    std::map<std::string, std::string> helpContent_;
    std::map<std::string, std::vector<std::string>> helpSubtopics_;
    std::map<std::string, std::vector<std::string>> helpRelatedTopics_;
    std::map<std::string, std::string> helpExamples_;
    std::map<std::string, std::string> helpTutorials_;
    
    bool advancedHelpEnabled_;
    bool commandCompletionEnabled_;
    bool contextSensitiveHelpEnabled_;
    
    mutable std::mutex mutex_;
    
    std::string currentCommand_;
    std::string currentContext_;
    std::chrono::nanoseconds lastHelpAccess_;
    
    std::string lastError_;
    
    // Help content management
    void tokenizeHelpContent(const std::string& content, std::vector<std::string>& tokens);
    bool validateHelpContent(const std::string& content);
    std::string formatHelpContent(const std::string& content) const;
    std::string extractExamples(const std::string& content) const;
    std::string extractTutorials(const std::string& content) const;
    
    // Help system navigation
    bool buildHelpIndex();
    bool discoverHelpTopics();
    bool discoverExamples();
    bool discoverTutorials();
    
    // Command completion
    std::vector<std::string> completeCommand(const std::string& partialCommand,
                                            const std::map<std::string, std::string>& currentArgs);
    std::vector<std::string> getSubcommandCompletions(const std::string& mainCommand,
                                                    const std::map<std::string, std::string>& currentArgs);
    std::vector<std::string> getOptionCompletions(const std::string& command,
                                                 const std::string& partialOption);
    
    // Context-sensitive help
    std::string generateContextHelp(const std::string& command, const std::string& context);
    bool loadContextInformation(const std::string& command, const std::string& context);
    
    // Help display formatting
    std::string formatHelpTopic(const std::string& topic, const std::map<std::string, std::string>& options) const;
    std::string formatAdvancedHelp(const std::string& topic) const;
    std::string formatExample(const std::string& exampleName) const;
    std::string formatTutorial(const std::string& tutorialName) const;
    
    // Statistics calculation
    double calculateHelpUsage(const std::string& topic) const;
    bool updateHelpAccessStats(const std::string& topic);
    
    // Help system maintenance
    bool cleanHelpContent();
    bool optimizeHelpStorage();
    bool validateHelpSystemIntegrity();
    
    // Error handling
    void setError(const std::string& error) { lastError_ = error; }
    void clearError() { lastError_.clear(); }
};

} // namespace help
} // namespace nlm
