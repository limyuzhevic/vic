#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {

// Documentation section types
enum class DocSectionType {
    OVERVIEW,
    USAGE,
    API_REFERENCE,
    CONFIGURATION,
    EXPERIMENTS,
    EXTENSIONS,
    EXAMPLES,
    TROUBLESHOOTING
};

// Documentation entry
struct DocEntry {
    DocSectionType section;
    std::string title;
    std::string content;
    std::vector<std::string> examples;
    std::map<std::string, std::string> parameters;
    
    DocEntry(DocSectionType sec, const std::string& title, const std::string& content)
        : section(sec), title(title), content(content) {}
};

// Documentation generator
class DocumentationGenerator {
public:
    DocumentationGenerator();
    ~DocumentationGenerator();
    
    // Generate complete documentation
    std::string generateFullDocumentation() const;
    
    // Generate specific section
    std::string generateSection(DocSectionType section) const;
    
    // Add content to documentation
    void addSection(DocSectionType section, const std::string& title, const std::string& content);
    void addExample(DocSectionType section, const std::string& example);
    
    // Get available sections
    std::vector<DocSectionType> getAvailableSections() const;
    std::string getSectionTitle(DocSectionType section) const;
    
    // Format and styling
    void setFormattingStyle(const std::string& style);
    void setIncludeExamples(bool include);
    void setIncludeParameters(bool include);
    
    // Export
    bool exportToFile(const std::string& filepath, const std::string& format = "markdown");
    std::string exportToString(const std::string& format = "markdown") const;
    
    // Get current timestamp for documentation
    static std::string getCurrentTimestamp();
    static std::string formatTimestamp(const std::string& timestamp, const std::string& format);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Online documentation system
class OnlineDocumentation {
public:
    OnlineDocumentation();
    ~OnlineDocumentation();
    
    // Get documentation for experiment type
    std::string getExperimentDocumentation(const std::string& experimentType) const;
    
    // Get documentation for configuration parameters
    std::string getConfigurationDocumentation(const std::string& experimentType) const;
    
    // Get documentation for experiment APIs
    std::string getAPIDocumentation(const std::string& className) const;
    
    // Search documentation
    std::vector<std::string> search(const std::string& query) const;
    
    // Get related documentation
    std::vector<std::string> getRelatedDocs(const std::string& topic) const;
    
    // Version and compatibility
    std::string getVersion() const;
    std::vector<std::string> getCompatibleVersions() const;
    
    // Updates and changelogs
    std::string getChangelog() const;
    std::string getLastUpdated() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Documentation content generators
namespace DocumentationContent {
    // Overview content
    std::string getOverviewContent();
    std::string getOverviewUsage();
    std::string getOverviewFeatures();
    
    // API reference content
    std::string getExperimentAPIContent();
    std::string getConfigurationAPIContent();
    std::string getRunnerAPIContent();
    
    // Configuration documentation
    std::string getPhase6ConfigContent();
    std::string getPhase5ConfigContent();
    std::string getPhase4ConfigContent();
    
    // Experiment documentation
    std::string getPhase6ExperimentContent();
    std::string getScalingBenchmarkContent();
    std::string getAblationStudyContent();
    
    // Extension documentation
    std::string getFactoryPatternContent();
    std::string getPluginArchitectureContent();
    
    // Examples
    std::string getBasicExperimentExample();
    std::string getConfigurationExample();
    std::string getExportExample();
    
    // Troubleshooting
    std::string getTroubleshootingContent();
    std::string getCommonErrors();
    std::string getPerformanceTips();
}

// Tutorial system
class TutorialSystem {
public:
    TutorialSystem();
    ~TutorialSystem();
    
    // Get tutorial for specific topic
    std::string getTutorial(const std::string& topic) const;
    
    // Get tutorial steps
    std::vector<std::string> getTutorialSteps(const std::string& topic) const;
    
    // Get tutorial difficulty
    std::string getTutorialDifficulty(const std::string& topic) const;
    
    // Check if tutorial exists
    bool hasTutorial(const std::string& topic) const;
    
    // Available tutorials
    std::vector<std::string> getAvailableTutorials() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Code examples system
class CodeExamples {
public:
    CodeExamples();
    ~CodeExamples();
    
    // Get code example
    std::string getExample(const std::string& exampleName) const;
    
    // Get code example with language specification
    std::string getExample(const std::string& exampleName, const std::string& language) const;
    
    // Get categorized examples
    std::vector<std::string> getExamplesByCategory(const std::string& category) const;
    
    // Search examples
    std::vector<std::string> searchExamples(const std::string& query) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global documentation instances
DocumentationGenerator& getDocumentationGenerator();
OnlineDocumentation& getOnlineDocumentation();
TutorialSystem& getTutorialSystem();
CodeExamples& getCodeExamples();

// Documentation helper functions
namespace DocHelpers {
    // Format code blocks
    std::string formatCodeBlock(const std::string& code, const std::string& language = "cpp");
    
    // Format lists
    std::string formatList(const std::vector<std::string>& items, const std::string& style = "bullet");
    
    // Format tables
    std::string formatTable(const std::vector<std::vector<std::string>>& data, 
                           const std::vector<std::string>& headers);
    
    // Format links
    std::string formatLink(const std::string& text, const std::string& url);
    
    // Format warnings and notes
    std::string formatWarning(const std::string& message);
    std::string formatNote(const std::string& message);
    std::string formatImportant(const std::string& message);
    
    // Generate table of contents
    std::string generateTOC(const std::vector<DocEntry>& entries);
}

} // namespace nlm
