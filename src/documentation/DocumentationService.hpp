// NLM Enhanced Documentation System
//
// Comprehensive documentation for NLM enhanced features
// Includes API documentation, examples, tutorials, and configuration guides
// Supports both beginners and advanced users

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Documentation sections
enum class DocumentationSection {
    Overview,
    Installation,
    QuickStart,
    BasicUsage,
    AdvancedUsage,
    Configuration,
    CommandLine,
    APIReference,
    Examples,
    Tutorials,
    Troubleshooting,
    FAQ,
    Migration
};

// Simple documentation entry
struct DocumentationEntry {
    std::string title;
    std::string content;
    DocumentationSection section;
    std::vector<std::string> tags;
    std::string fileName;
    
    DocumentationEntry(const std::string& t, const std::string& c, DocumentationSection s,
                      const std::string& f = "")
        : title(t), content(c), section(s), fileName(f) {}
};

// Simple documentation generator
class DocumentationGenerator {
public:
    DocumentationGenerator();
    ~DocumentationGenerator();
    
    // Documentation generation
    void generateAllDocumentation();
    void generateDocumentation(DocumentationSection section);
    std::string getDocumentation(const std::string& topic, DocumentationSection section = DocumentationSection::Overview) const;
    
    // Documentation content templates
    std::string generateOverviewContent() const;
    std::string generateInstallationContent() const;
    std::string generateQuickStartContent() const;
    std::string generateBasicUsageContent() const;
    std::string generateAdvancedUsageContent() const;
    std::string generateConfigurationContent() const;
    std::string generateCommandLineContent() const;
    std::string generateExamplesContent() const;
    std::string generateTutorialsContent() const;
    std::string generateTroubleshootingContent() const;
    std::string generateFAQContent() const;
    
    // Documentation formatting
    std::string formatMarkdown(const std::string& content) const;
    std::string formatText(const std::string& content) const;
    
    // Documentation export
    bool exportDocumentation(const std::string& outputDir, const std::string& format = "markdown");
    bool exportSection(DocumentationSection section, const std::string& outputFile, const std::string& format = "markdown");
    
    // Documentation search and navigation
    std::vector<std::string> searchDocumentation(const std::string& query) const;
    std::vector<DocumentationEntry> getDocumentationBySection(DocumentationSection section) const;
    
    // Documentation validation
    std::vector<std::string> validateDocumentation() const;
    bool isDocumentationComplete() const;
    
    // Documentation statistics
    std::map<DocumentationSection, size_t> getSectionStatistics() const;
    size_t getTotalDocumentationSize() const;
    double getDocumentationCompletenessScore() const;
    
private:
    std::vector<DocumentationEntry> documentation;
    
    // Internal helpers
    static std::string formatCodeBlock(const std::string& code, const std::string& language = "cpp") const;
    static std::string escapeHTML(const std::string& text) const;
};

// Documentation service
class DocumentationService {
public:
    static DocumentationService& getInstance();
    static void initialize();
    static void shutdown();
    
    // Documentation access
    static std::string getDocumentation(const std::string& topic, DocumentationSection section = DocumentationSection::Overview);
    static std::vector<std::string> searchDocumentation(const std::string& query);
    static bool hasDocumentation(const std::string& topic, DocumentationSection section = DocumentationSection::Overview);
    
    // Documentation generation
    static void generateAllDocumentation();
    static void generateDocumentation(DocumentationSection section);
    
    // Documentation export
    static bool exportDocumentation(const std::string& outputDir, const std::string& format = "markdown");
    static bool exportDocumentation(const std::string& topic, const std::string& outputFile, const std::string& format = "markdown");
    
    // Documentation statistics
    static std::map<DocumentationSection, size_t> getSectionStatistics();
    static size_t getTotalDocumentationSize();
    static double getDocumentationCompletenessScore();
    
private:
    DocumentationService();
    ~DocumentationService();
    DocumentationService(const DocumentationService&) = delete;
    DocumentationService& operator=(const DocumentationService&) = delete;
    
    static DocumentationService* instance;
    std::vector<DocumentationEntry> documentation;
};

} // namespace nlm