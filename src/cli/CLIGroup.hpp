#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <any>

namespace nlm {

// Command group for organizing related commands
class CLIGroup {
public:
    CLIGroup(const std::string& name, const std::string& description = "",
             const std::string& version = "1.0", const std::string& author = "",
             const std::string& contact = "");
    ~CLIGroup();
    
    // Group metadata
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    const std::string& getVersion() const { return version; }
    const std::string& getAuthor() const { return author; }
    const std::string& getContact() const { return contact; }
    
    void setName(const std::string& name) { this->name = name; }
    void setDescription(const std::string& description) { this->description = description; }
    void setVersion(const std::string& version) { this->version = version; }
    void setAuthor(const std::string& author) { this->author = author; }
    void setContact(const std::string& contact) { this->contact = contact; }
    
    // Command management
    void addCommand(std::shared_ptr<CLICommand> command);
    bool removeCommand(const std::string& commandName);
    std::shared_ptr<CLICommand> getCommand(const std::string& commandName) const;
    bool hasCommand(const std::string& commandName) const;
    
    // Get all commands
    const std::vector<std::shared_ptr<CLICommand>>& getCommands() const { return commands; }
    
    // Group filtering
    void filterCommands(const std::function<bool(const std::shared_ptr<CLICommand>&)>& predicate);
    
    // Command execution
    int executeCommand(const std::string& commandName, const std::vector<std::string>& args);
    bool validateCommand(const std::string& commandName, const std::vector<std::string>& args) const;
    
    // Help and documentation
    void showHelp(std::ostream& out = std::cout) const;
    void showCommandHelp(const std::string& commandName, std::ostream& out = std::cout) const;
    void showGroupInfo(std::ostream& out = std::cout) const;
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    std::vector<std::string> getOptionCompletions(const std::string& commandName, 
                                                  const std::string& partial) const;
    
    // Command history
    void addCommandToHistory(const std::string& commandName, const std::vector<std::string>& args);
    const std::vector<std::pair<std::string, std::vector<std::string>>>& getCommandHistory() const { return commandHistory; }
    
    // Configuration
    void setDefaultOutputFormat(const std::string& format) { defaultOutputFormat = format; }
    const std::string& getDefaultOutputFormat() const { return defaultOutputFormat; }
    
    void setVerbose(bool verbose) { this->verbose = verbose; }
    bool isVerbose() const { return verbose; }
    
    void setQuiet(bool quiet) { this->quiet = quiet; }
    bool isQuiet() const { return quiet; }
    
    void setAutoComplete(bool autoComplete) { this->autoComplete = autoComplete; }
    bool isAutoComplete() const { return autoComplete; }
    
    // Statistics
    size_t getCommandCount() const { return commands.size(); }
    size_t getHistorySize() const { return commandHistory.size(); }
    
    // Import/Export
    bool importFromFile(const std::string& filename);
    bool exportToFile(const std::string& filename) const;
    bool loadFromString(const std::string& data);
    std::string exportToString() const;
    
private:
    std::string name;
    std::string description;
    std::string version;
    std::string author;
    std::string contact;
    std::vector<std::shared_ptr<CLICommand>> commands;
    std::vector<std::pair<std::string, std::vector<std::string>>> commandHistory;
    std::string defaultOutputFormat;
    bool verbose;
    bool quiet;
    bool autoComplete;
    mutable std::mutex mutex;
    
    // Internal help generation
    void generateCommandHelp(const std::shared_ptr<CLICommand>& command, std::ostream& out) const;
    void generateOptionHelp(const CLIOption& option, std::ostream& out) const;
    void generateParameterHelp(const CLIParameter& parameter, std::ostream& out) const;
    
    // Command execution helpers
    bool validateCommandArguments(const std::shared_ptr<CLICommand>& command,
                                  const std::vector<std::string>& args) const;
    int executeCommandWithContext(const std::string& commandName,
                                  const std::vector<std::string>& args);
    
    // File I/O
    bool writeToStream(std::ostream& out) const;
    bool readFromStream(std::istream& in);
    
    // Statistics and logging
    void logCommandExecution(const std::string& commandName, int result, double executionTime) const;
    void logCommandValidation(const std::string& commandName, bool valid) const;
    
    // Command filtering
    static bool filterByName(const std::shared_ptr<CLICommand>& command, const std::string& pattern);
    static bool filterByOption(const std::shared_ptr<CLICommand>& command, const std::string& optionName);
    static bool filterByParameter(const std::shared_ptr<CLICommand>& command, const std::string& parameterName);
};

// Command group manager for managing multiple groups
class CLIGroupManager {
public:
    CLIGroupManager();
    ~CLIGroupManager();
    
    // Group management
    void addGroup(std::shared_ptr<CLIGroup> group);
    bool removeGroup(const std::string& groupName);
    std::shared_ptr<CLIGroup> getGroup(const std::string& groupName) const;
    bool hasGroup(const std::string& groupName) const;
    
    // Get all groups
    const std::vector<std::shared_ptr<CLIGroup>>& getGroups() const { return groups; }
    
    // Command execution across all groups
    int executeCommand(const std::string& commandName, const std::vector<std::string>& args);
    bool validateCommand(const std::string& commandName, const std::vector<std::string>& args) const;
    
    // Help and documentation
    void showHelp(const std::string& command = "", const std::string& group = "", 
                  std::ostream& out = std::cout) const;
    void showGroupHelp(const std::string& groupName, std::ostream& out = std::cout) const;
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    std::vector<std::string> getOptionCompletions(const std::string& commandName, 
                                                  const std::string& partial) const;
    
    // Global configuration
    void setGlobalOutputFormat(const std::string& format) { globalOutputFormat = format; }
    const std::string& getGlobalOutputFormat() const { return globalOutputFormat; }
    
    void setGlobalVerbose(bool verbose) { globalVerbose = verbose; }
    bool isGlobalVerbose() const { return globalVerbose; }
    
    void setGlobalQuiet(bool quiet) { globalQuiet = quiet; }
    bool isGlobalQuiet() const { return globalQuiet; }
    
    // Statistics
    size_t getGroupCount() const { return groups.size(); }
    size_t getTotalCommandCount() const;
    
    // Import/Export
    bool importFromFile(const std::string& filename);
    bool exportToFile(const std::string& filename) const;
    bool loadFromString(const std::string& data);
    std::string exportToString() const;
    
    // Global command history
    const std::vector<std::pair<std::string, std::vector<std::string>>>& getGlobalCommandHistory() const;
    void clearGlobalCommandHistory() { globalCommandHistory.clear(); }
    
private:
    std::vector<std::shared_ptr<CLIGroup>> groups;
    std::string globalOutputFormat;
    bool globalVerbose;
    bool globalQuiet;
    mutable std::vector<std::pair<std::string, std::vector<std::string>>> globalCommandHistory;
    mutable std::mutex mutex;
    
    // Command execution across groups
    std::shared_ptr<CLICommand> findCommand(const std::string& commandName) const;
    std::shared_ptr<CLIGroup> findGroupForCommand(const std::string& commandName) const;
    
    // File I/O
    bool writeGroupsToStream(std::ostream& out) const;
    bool readGroupsFromStream(std::istream& in);
    
    // Statistics and logging
    void logCommandExecution(const std::string& commandName, int result, double executionTime) const;
    void logCommandValidation(const std::string& commandName, bool valid) const;
};

// Command group factory for creating common group types
class CLIGroupFactory {
public:
    static std::shared_ptr<CLIGroup> createCoreGroup();
    static std::shared_ptr<CLIGroup> createExperimentGroup();
    static std::shared_ptr<CLIGroup> createDebugGroup();
    static std::shared_ptr<CLIGroup> createProfileGroup();
    static std::shared_ptr<CLIGroup> createConfigGroup();
    static std::shared_ptr<CLIGroup> createStatusGroup();
    static std::shared_ptr<CLIGroup> createHelpGroup();
    static std::shared_ptr<CLIGroup> createUtilityGroup();
    
    static std::shared_ptr<CLIGroup> createGroup(const std::string& name,
                                                const std::string& description = "",
                                                const std::string& version = "1.0",
                                                const std::string& author = "",
                                                const std::string& contact = "");
    
    static std::shared_ptr<CLIGroup> createGroupFromConfig(const std::map<std::string, std::any>& config);
};

// Command group utilities
namespace CLIGroupUtils {
    // Group comparison
    bool compareGroups(const CLIGroup& group1, const CLIGroup& group2);
    bool compareGroupsByName(const CLIGroup& group1, const CLIGroup& group2);
    bool compareGroupsByCommandCount(const CLIGroup& group1, const CLIGroup& group2);
    
    // Group filtering
    std::vector<std::shared_ptr<CLIGroup>> filterGroupsByName(const std::vector<std::shared_ptr<CLIGroup>>& groups,
                                                             const std::string& pattern);
    std::vector<std::shared_ptr<CLIGroup>> filterGroupsByCommandCount(const std::vector<std::shared_ptr<CLIGroup>>& groups,
                                                                     size_t minCount, size_t maxCount = SIZE_MAX);
    std::vector<std::shared_ptr<CLIGroup>> filterGroupsByDescription(const std::vector<std::shared_ptr<CLIGroup>>& groups,
                                                                    const std::string& pattern);
    
    // Group sorting
    void sortGroupsByName(std::vector<std::shared_ptr<CLIGroup>>& groups);
    void sortGroupsByCommandCount(std::vector<std::shared_ptr<CLIGroup>>& groups);
    void sortGroupsByDescription(std::vector<std::shared_ptr<CLIGroup>>& groups);
    
    // Group merging
    std::shared_ptr<CLIGroup> mergeGroups(const std::vector<std::shared_ptr<CLIGroup>>& groups);
    void mergeGroupsInto(std::vector<std::shared_ptr<CLIGroup>>& target,
                         const std::vector<std::shared_ptr<CLIGroup>>& source);
    
    // Group statistics
    struct GroupStats {
        std::string groupName;
        size_t commandCount;
        size_t historySize;
        std::string description;
        std::string author;
        std::string version;
    };
    
    std::vector<GroupStats> calculateGroupStats(const std::vector<std::shared_ptr<CLIGroup>>& groups);
    
    // Group validation
    bool validateGroup(const CLIGroup& group);
    std::vector<std::string> validateGroups(const std::vector<std::shared_ptr<CLIGroup>>& groups);
    
    // Group documentation
    std::string generateGroupDocumentation(const CLIGroup& group);
    std::string generateGroupMarkdown(const CLIGroup& group);
    std::string generateGroupHTML(const CLIGroup& group);
    
    // Group comparison
    std::string compareGroups(const CLIGroup& group1, const CLIGroup& group2);
    std::string compareGroups(const std::vector<std::shared_ptr<CLIGroup>>& groups);
};

} // namespace nlm
