#ifndef NLM_HELP_DOCS_HPP
#define NLM_HELP_DOCS_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace nlm {
namespace help {

// Help documentation types
enum class HelpType {
    CommandLine,
    Configuration,
    Experiment,
    Plugin,
    Statistics,
    ErrorHandling,
    Complete
};

// Command line help documentation
class CLIHelp {
public:
    CLIHelp() = default;
    ~CLIHelp() = default;
    
    // Command help generation
    std::string generateCommandHelp() const;
    std::string generateSubcommandHelp(const std::string& commandName) const;
    std::string generateExperimentHelp(const std::string& experimentName) const;
    std::string generateUtilsHelp(const std::string& utilityName) const;
    
    // Configuration help
    std::string generateConfigHelp() const;
    std::string generateConfigFileHelp(const std::string& format = "all") const;
    
    // Experiment help
    std::string generateExperimentListHelp() const;
    std::string generateExperimentRunHelp(const std::string& experimentName) const;
    
    // Utility help
    std::string generateUtilsListHelp() const;
    std::string generateBenchmarkHelp() const;
    std::string generateValidateHelp() const;
    std::string generateStatsHelp() const;
    
    // Plugin help
    std::string generatePluginHelp() const;
    std::string generatePluginListHelp() const;
    
    // Error handling help
    std::string generateErrorHelp() const;
    std::string generateErrorCodeHelp(const std::string& errorCode) const;
    
    // Complete documentation
    std::string generateCompleteDocumentation() const;
    std::string generateQuickStartGuide() const;
    std::string generateTutorials() const;
    std::string generateReference() const;
    
private:
    struct CommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
        std::vector<std::string> options;
    };
    
    struct SubcommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
        std::vector<std::string> options;
    };
    
    struct ExperimentInfo {
        std::string name;
        std::string phase;
        std::string description;
        std::string purpose;
        std::vector<std::string> features;
        std::string typicalUsage;
        std::map<std::string, std::string> parameters;
    };
    
    struct UtilsInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
        std::vector<std::string> options;
    };
    
    struct PluginInfo {
        std::string name;
        std::string type;
        std::string description;
        std::string version;
        std::string author;
        std::vector<std::string> capabilities;
    };
    
    static std::vector<CommandInfo> getCommandInfos();
    static std::vector<SubcommandInfo> getSubcommandInfos();
    static std::vector<ExperimentInfo> getExperimentInfos();
    static std::vector<UtilsInfo> getUtilsInfos();
    static std::vector<PluginInfo> getPluginInfos();
    
    std::string formatHelpSection(const std::string& title, const std::string& content) const;
    std::string formatUsage(const std::string& command, const std::string& usage) const;
    std::string formatExample(const std::string& example) const;
    std::string formatOption(const std::string& option, const std::string& description) const;
};

// Configuration file help
class ConfigFileHelp {
public:
    ConfigFileHelp() = default;
    ~ConfigFileHelp() = default;
    
    std::string generateConfigFileHelp(const std::string& format = "all") const;
    std::string generateJSONConfigHelp() const;
    std::string generateYAMLConfigHelp() const;
    std::string generateCustomConfigHelp() const;
    
    // Schema help
    std::string generateSchemaHelp() const;
    std::string generateSchemaExamples() const;
    
    // Validation help
    std::string generateValidationHelp() const;
    std::string generateValidationExamples() const;
    
private:
    struct ConfigFieldInfo {
        std::string name;
        std::string type;
        std::string description;
        std::string example;
        std::vector<std::string> allowedValues;
        std::vector<std::string> validationRules;
    };
    
    static std::vector<ConfigFieldInfo> getJSONConfigFields();
    static std::vector<ConfigFieldInfo> getYAMLConfigFields();
    static std::vector<ConfigFieldInfo> getCustomConfigFields();
};

// Experiment help
class ExperimentHelp {
public:
    ExperimentHelp() = default;
    ~ExperimentHelp() = default;
    
    std::string generateExperimentHelp(const std::string& experimentName) const;
    std::string generateExperimentListHelp() const;
    std::string generatePhaseHelp(int phase) const;
    
    // Phase-specific help
    std::string generatePhase1Help() const;
    std::string generatePhase2Help() const;
    std::string generatePhase3Help() const;
    std::string generatePhase4Help() const;
    std::string generatePhase5Help() const;
    std::string generatePhase6Help() const;
    
private:
    struct PhaseInfo {
        int phaseNumber;
        std::string name;
        std::string description;
        std::vector<std::string> keyFeatures;
        std::string typicalUseCase;
        std::string complexity;
    };
    
    static std::vector<PhaseInfo> getPhaseInfos();
};

// Plugin help
class PluginHelp {
public:
    PluginHelp() = default;
    ~PluginHelp() = default;
    
    std::string generatePluginHelp() const;
    std::string generatePluginListHelp() const;
    std::string generatePluginTypeHelp(PluginType type) const;
    
    // Plugin creation help
    std::string generateCreatePluginHelp() const;
    std::string generatePluginDevelopmentHelp() const;
    
private:
    struct PluginTypeInfo {
        PluginType type;
        std::string name;
        std::string description;
        std::vector<std::string> examplePlugins;
        std::string typicalUseCase;
    };
    
    static std::vector<PluginTypeInfo> getPluginTypeInfos();
};

// Statistics and performance help
class StatsPerfHelp {
public:
    StatsPerfHelp() = default;
    ~StatsPerfHelp() = default;
    
    std::string generateStatsHelp() const;
    std::string generatePerformanceHelp() const;
    std::string generateMonitoringHelp() const;
    
    // Statistics collection help
    std::string generateCounterHelp() const;
    std::string generateGaugeHelp() const;
    std::string generateHistogramHelp() const;
    std::string generateTimerHelp() const;
    
    // Performance profiling help
    std::string generateProfilerHelp() const;
    std::string generateMemoryProfilerHelp() const;
    std::string generateRealTimeStatsHelp() const;
    
private:
    struct StatsMetricInfo {
        std::string name;
        std::string description;
        std::string unit;
        std::string exampleUsage;
        std::string dataType;
    };
    
    static std::vector<StatsMetricInfo> getStatsMetricInfos();
};

// Error handling help
class ErrorHelp {
public:
    ErrorHelp() = default;
    ~ErrorHelp() = default;
    
    std::string generateErrorHelp() const;
    std::string generateErrorCodeHelp(const std::string& errorCode) const;
    std::string generateErrorCategoryHelp() const;
    
    // Error recovery help
    std::string generateErrorRecoveryHelp() const;
    std::string generateErrorHandlingHelp() const;
    
private:
    struct ErrorInfo {
        std::string code;
        std::string category;
        std::string severity;
        std::string description;
        std::string commonCauses;
        std::string suggestedSolutions;
    };
    
    static std::vector<ErrorInfo> getErrorInfos();
};

// Quick start guide
class QuickStartGuide {
public:
    QuickStartGuide() = default;
    ~QuickStartGuide() = default;
    
    std::string generateQuickStart() const;
    std::string generateFirstExperiment() const;
    std::string generateConfigurationSetup() const;
    std::string generateRunningExperiments() const;
    
private:
    struct QuickStartStep {
        int stepNumber;
        std::string title;
        std::string description;
        std::string command;
        std::string expectedOutput;
    };
    
    static std::vector<QuickStartStep> getQuickStartSteps();
};

// Tutorials
class Tutorials {
public:
    Tutorials() = default;
    ~Tutorials() = default;
    
    std::string generateTutorialsList() const;
    std::string generateBeginnerTutorial() const;
    std::string generateIntermediateTutorial() const;
    std::string generateAdvancedTutorial() const;
    
    // Tutorial modules
    std::string generateGettingStartedTutorial() const;
    std::string generateConfigurationTutorial() const;
    std::string generateExperimentTutorial() const;
    std::string generatePluginTutorial() const;
    std::string generateStatsTutorial() const;
    
private:
    struct TutorialModule {
        std::string title;
        std::string difficulty;
        std::string duration;
        std::string learningObjectives;
        std::vector<std::string> topics;
    };
    
    static std::vector<TutorialModule> getTutorialModules();
};

// User guide
class UserGuide {
public:
    UserGuide() = default;
    ~UserGuide() = default;
    
    std::string generateUserGuide() const;
    std::string generateNavigationGuide() const;
    std::string generateCommandReference() const;
    std::string generateConfigurationGuide() const;
    std::string generateExperimentGuide() const;
    std::string generateTroubleshootingGuide() const;
    
private:
    struct NavigationItem {
        std::string title;
        std::string target;
        std::string description;
    };
    
    static std::vector<NavigationItem> getNavigationItems();
};

// Complete documentation generator
class CompleteDocumentation {
public:
    CompleteDocumentation() = default;
    ~CompleteDocumentation() = default;
    
    std::string generateCompleteDocumentation() const;
    std::string generateAPIReference() const;
    std::string generateExamplesReference() const;
    std::string generateChangelog() const;
    std::string generateReleaseNotes() const;
    
private:
    std::string generateDocumentationIndex() const;
    std::string generateCommandLineIndex() const;
    std::string generateConfigurationIndex() const;
    std::string generateExamplesIndex() const;
};

// Help system interface
class HelpSystem {
public:
    HelpSystem() = default;
    ~HelpSystem() = default;
    
    // Help generation
    std::string getHelp(HelpType type) const;
    std::string getHelp(const std::string& topic) const;
    std::string getQuickStart() const;
    std::string getTutorials() const;
    
    // Interactive help
    void showHelp(const std::string& topic) const;
    void showCommandHelp(const std::string& command) const;
    void showExperimentHelp(const std::string& experiment) const;
    void showErrorHelp(const std::string& errorCode) const;
    
    // Help search
    std::vector<std::string> searchHelp(const std::string& query) const;
    bool hasHelpForTopic(const std::string& topic) const;
    
    // Help export
    bool exportHelp(const std::string& filepath, HelpType type = HelpType::Complete) const;
    bool exportHelpToHTML(const std::string& filepath) const;
    bool exportHelpToPDF(const std::string& filepath) const;
    
    // Default help content
    static std::string getDefaultHelpContent(HelpType type);
    static std::string getDefaultQuickStart();
    static std::string getDefaultTutorials();
    
private:
    mutable std::map<std::string, std::string> cachedHelp_;
    mutable std::mutex cacheMutex_;
    
    std::string cacheHelp(HelpType type, const std::string& content) const;
    std::string getCachedHelp(HelpType type) const;
};

} // namespace help
} // namespace nlm

#endif // NLM_HELP_DOCS_HPP