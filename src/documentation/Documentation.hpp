#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Documentation system version
constexpr int DOCUMENTATION_SYSTEM_VERSION = 1;

// Documentation categories
enum class DocumentationCategory {
    API,           // API reference
    Tutorial,      // Step-by-step tutorials
    Concept,       // Conceptual guides
    Reference,      // Reference documentation
    Examples,      // Code examples
    BestPractices,   // Best practices
    Troubleshooting, // Troubleshooting
    Migration,     // Migration guides
    Changelog,     // Version history
    FAQ,           // Frequently asked questions
    Architecture   // Architecture overview
};

// Documentation section
struct DocumentationSection {
    std::string id;
    std::string title;
    DocumentationCategory category;
    std::string content;
    std::string filePath;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    std::vector<std::string> childSections;
    std::string parentSection;
    
    int order;
    bool visible;
    bool searchable;
    std::string lastModified;
    
    DocumentationSection() : order(0), visible(true), searchable(true) {}
};

// API documentation entry
struct APIDocumentation {
    std::string name;
    std::string signature;
    std::string returnType;
    std::string description;
    std::string file;
    int line;
    std::string category;
    std::vector<std::string> parameters;
    std::vector<std::string> exceptions;
    std::vector<std::string> seeAlso;
    std::string example;
    std::map<std::string, std::string> attributes;
    
    bool isFunction;
    bool isClass;
    bool isMethod;
    bool isStatic;
    bool isVirtual;
    
    APIDocumentation() : isFunction(false), isClass(false), isMethod(false),
                      isStatic(false), isVirtual(false) {}
};

// Tutorial entry
struct TutorialEntry {
    std::string title;
    std::string level;
    std::string category;
    std::string description;
    std::string difficulty;
    double estimatedTime;
    std::vector<std::string> prerequisites;
    std::vector<std::string> objectives;
    std::string content;
    std::vector<std::string> codeExamples;
    std::vector<std::string> outputExamples;
    std::map<std::string, std::string> metadata;
    
    TutorialEntry() : estimatedTime(30.0), level("intermediate") {}
};

// Examples entry
struct ExamplesEntry {
    std::string title;
    std::string category;
    std::string description;
    std::string code;
    std::string output;
    std::string explanation;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
    ExamplesEntry() {}
};

// Documentation configuration
struct DocumentationConfig {
    std::string name;
    std::string version;
    std::string title;
    std::string description;
    std::string outputDirectory;
    std::string theme;
    std::string language;
    
    bool enableSearch;
    bool enableToc;
    bool enableIndex;
    bool enableTags;
    bool enableCrossReferences;
    
    std::string cssFile;
    std::string jsFile;
    std::vector<std::string> customStyles;
    std::vector<std::string> customScripts;
    
    std::map<std::string, std::string> metadata;
    
    DocumentationConfig() : enableSearch(true), enableToc(true), enableIndex(true),
                              enableTags(true), enableCrossReferences(true) {}
};

// Documentation index entry
struct DocumentationIndexEntry {
    std::string id;
    std::string title;
    std::string summary;
    DocumentationCategory category;
    std::string file;
    int line;
    std::vector<std::string> tags;
    std::vector<std::string> keywords;
    std::map<std::string, std::string> metadata;
    
    double relevanceScore;
    int accessCount;
    std::chrono::system_clock::time_point lastAccessed;
    
    DocumentationIndexEntry() : relevanceScore(0.0), accessCount(0), lastAccessed(std::chrono::system_clock::now()) {}
};

// Documentation interface
class IDocumentation {
public:
    virtual ~IDocumentation() = default;
    
    // Documentation generation
    virtual bool generateDocumentation() = 0;
    virtual bool generateAPIDocumentation() = 0;
    virtual bool generateTutorials() = 0;
    virtual bool generateExamples() = 0;
    virtual bool generateConceptGuides() = 0;
    
    // Documentation management
    virtual bool addSection(const DocumentationSection& section) = 0;
    virtual bool removeSection(const std::string& id) = 0;
    virtual bool updateSection(const DocumentationSection& section) = 0;
    virtual DocumentationSection getSection(const std::string& id) const = 0;
    virtual std::vector<DocumentationSection> getSectionsByCategory(DocumentationCategory category) const = 0;
    
    // API documentation
    virtual bool addAPIDocumentation(const APIDocumentation& api) = 0;
    virtual bool removeAPIDocumentation(const std::string& name) = 0;
    virtual APIDocumentation getAPIDocumentation(const std::string& name) const = 0;
    virtual std::vector<APIDocumentation> getAPIDocumentationsByCategory(const std::string& category) const = 0;
    
    // Tutorials
    virtual bool addTutorial(const TutorialEntry& tutorial) = 0;
    virtual bool removeTutorial(const std::string& title) = 0;
    virtual TutorialEntry getTutorial(const std::string& title) const = 0;
    virtual std::vector<TutorialEntry> getTutorialsByCategory(const std::string& category) const = 0;
    
    // Examples
    virtual bool addExample(const ExamplesEntry& example) = 0;
    virtual bool removeExample(const std::string& title) = 0;
    virtual ExamplesEntry getExample(const std::string& title) const = 0;
    virtual std::vector<ExamplesEntry> getExamplesByCategory(const std::string& category) const = 0;
    
    // Index management
    virtual bool rebuildIndex() = 0;
    virtual std::vector<DocumentationIndexEntry> searchIndex(const std::string& query) const = 0;
    virtual DocumentationIndexEntry getIndexEntry(const std::string& id) const = 0;
    virtual std::vector<DocumentationIndexEntry> getIndexByCategory(DocumentationCategory category) const = 0;
    
    // Configuration
    virtual bool configure(const DocumentationConfig& config) = 0;
    virtual DocumentationConfig getConfig() const = 0;
    
    // Output
    virtual bool exportDocumentation(const std::string& outputPath, const std::string& format = "html") = 0;
    virtual bool exportAPIDocumentation(const std::string& outputPath, const std::string& format = "json") = 0;
    virtual bool exportTutorials(const std::string& outputPath, const std::string& format = "html") = 0;
    virtual bool exportExamples(const std::string& outputPath, const std::string& format = "html") = 0;
    
    // Maintenance
    virtual bool cleanGeneratedFiles() = 0;
    virtual bool validateDocumentation() = 0;
    virtual std::string getDocumentationStatus() const = 0;
    
    // Statistics
    virtual size_t getSectionCount() const = 0;
    virtual size_t getAPICount() const = 0;
    virtual size_t getTutorialCount() const = 0;
    virtual size_t getExampleCount() const = 0;
    virtual size_t getIndexEntryCount() const = 0;
    
    // Callbacks
    virtual void setDocumentationGeneratedCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setGenerationErrorCallback(std::function<void(const std::string&, const std::string&)> callback) = 0;
    virtual void setIndexUpdateCallback(std::function<void()> callback) = 0;
    
    // Configuration
    virtual void setPluginManager(void* manager) = 0;
    virtual void* getPluginManager() const = 0;
};

// Default documentation generator
class DefaultDocumentationGenerator : public IDocumentation {
private:
    DocumentationConfig config_;
    std::map<std::string, DocumentationSection> sections_;
    std::map<std::string, APIDocumentation> apiDocs_;
    std::map<std::string, TutorialEntry> tutorials_;
    std::map<std::string, ExamplesEntry> examples_;
    std::vector<DocumentationIndexEntry> index_;
    
    std::mutex mutex_;
    IPluginManager* pluginManager_;
    
    std::map<std::string, std::function<std::string(const DocumentationSection&)>> sectionFormatters_;
    std::map<std::string, std::function<std::string(const APIDocumentation&)>> apiFormatters_;
    std::map<std::string, std::function<std::string(const TutorialEntry&)>> tutorialFormatters_;
    std::map<std::string, std::function<std::string(const ExamplesEntry&)>> exampleFormatters_;
    
    std::function<void(const std::string&)> documentationGeneratedCallback_;
    std::function<void(const std::string&, const std::string&)> generationErrorCallback_;
    std::function<void()> indexUpdateCallback_;
    
public:
    DefaultDocumentationGenerator() : pluginManager_(nullptr) {
        // Initialize formatters
        initializeFormatters();
    }
    
    ~DefaultDocumentationGenerator() override {
        // Cleanup
    }
    
    // IDocumentation interface implementation
    bool configure(const DocumentationConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        return true;
    }
    
    DocumentationConfig getConfig() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }
    
    bool generateDocumentation() override {
        try {
            // Generate all documentation
            if (!generateAPIDocumentation()) {
                return false;
            }
            
            if (!generateTutorials()) {
                return false;
            }
            
            if (!generateExamples()) {
                return false;
            }
            
            if (!generateConceptGuides()) {
                return false;
            }
            
            if (documentationGeneratedCallback_) {
                documentationGeneratedCallback_("Documentation generation completed");
            }
            
            return true;
            
        } catch (const std::exception& e) {
            if (generationErrorCallback_) {
                generationErrorCallback_("Documentation generation failed", std::string(e.what()));
            }
            return false;
        }
    }
    
    bool generateAPIDocumentation() override {
        // Generate API documentation
        // In a real implementation, this would parse source code and generate documentation
        return true;
    }
    
    bool generateTutorials() override {
        // Generate tutorials
        // In a real implementation, this would render tutorial content
        return true;
    }
    
    bool generateExamples() override {
        // Generate examples
        // In a real implementation, this would render example code
        return true;
    }
    
    bool generateConceptGuides() override {
        // Generate concept guides
        // In a real implementation, this would render concept explanations
        return true;
    }
    
    bool addSection(const DocumentationSection& section) override {
        std::lock_guard<std::mutex> lock(mutex_);
        sections_[section.id] = section;
        return true;
    }
    
    bool removeSection(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = sections_.find(id);
        if (it == sections_.end()) {
            return false;
        }
        
        sections_.erase(it);
        
        // Remove from index
        index_.erase(std::remove_if(index_.begin(), index_.end(),
                                  [&id](const DocumentationIndexEntry& entry) { return entry.id == id; }),
                     index_.end());
        
        return true;
    }
    
    bool updateSection(const DocumentationSection& section) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        sections_[section.id] = section;
        
        // Update index
        rebuildIndex();
        
        return true;
    }
    
    DocumentationSection getSection(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = sections_.find(id);
        if (it == sections_.end()) {
            throw std::runtime_error("Section not found: " + id);
        }
        
        return it->second;
    }
    
    std::vector<DocumentationSection> getSectionsByCategory(DocumentationCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<DocumentationSection> results;
        for (const auto& pair : sections_) {
            if (pair.second.category == category) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }
    
    bool addAPIDocumentation(const APIDocumentation& api) override {
        std::lock_guard<std::mutex> lock(mutex_);
        apiDocs_[api.name] = api;
        return true;
    }
    
    bool removeAPIDocumentation(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = apiDocs_.find(name);
        if (it == apiDocs_.end()) {
            return false;
        }
        
        apiDocs_.erase(it);
        return true;
    }
    
    APIDocumentation getAPIDocumentation(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = apiDocs_.find(name);
        if (it == apiDocs_.end()) {
            throw std::runtime_error("API documentation not found: " + name);
        }
        
        return it->second;
    }
    
    std::vector<APIDocumentation> getAPIDocumentationsByCategory(const std::string& category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<APIDocumentation> results;
        for (const auto& pair : apiDocs_) {
            if (pair.second.category == category) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }
    
    bool addTutorial(const TutorialEntry& tutorial) override {
        std::lock_guard<std::mutex> lock(mutex_);
        tutorials_[tutorial.title] = tutorial;
        return true;
    }
    
    bool removeTutorial(const std::string& title) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tutorials_.find(title);
        if (it == tutorials_.end()) {
            return false;
        }
        
        tutorials_.erase(it);
        return true;
    }
    
    TutorialEntry getTutorial(const std::string& title) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = tutorials_.find(title);
        if (it == tutorials_.end()) {
            throw std::runtime_error("Tutorial not found: " + title);
        }
        
        return it->second;
    }
    
    std::vector<TutorialEntry> getTutorialsByCategory(const std::string& category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<TutorialEntry> results;
        for (const auto& pair : tutorials_) {
            if (pair.second.category == category) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }
    
    bool addExample(const ExamplesEntry& example) override {
        std::lock_guard<std::mutex> lock(mutex_);
        examples_[example.title] = example;
        return true;
    }
    
    bool removeExample(const std::string& title) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = examples_.find(title);
        if (it == examples_.end()) {
            return false;
        }
        
        examples_.erase(it);
        return true;
    }
    
    ExamplesEntry getExample(const std::string& title) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = examples_.find(title);
        if (it == examples_.end()) {
            throw std::runtime_error("Example not found: " + title);
        }
        
        return it->second;
    }
    
    std::vector<ExamplesEntry> getExamplesByCategory(const std::string& category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<ExamplesEntry> results;
        for (const auto& pair : examples_) {
            if (pair.second.category == category) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }
    
    bool rebuildIndex() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        index_.clear();
        
        // Rebuild index from sections
        for (const auto& pair : sections_) {
            DocumentationIndexEntry entry;
            entry.id = pair.second.id;
            entry.title = pair.second.title;
            entry.summary = pair.second.description;
            entry.category = pair.second.category;
            entry.file = pair.second.filePath;
            entry.line = 0; // Placeholder
            entry.tags = pair.second.tags;
            entry.keywords = extractKeywords(pair.second.content);
            entry.metadata = pair.second.metadata;
            
            index_.push_back(entry);
        }
        
        // Rebuild index from API documentation
        for (const auto& pair : apiDocs_) {
            DocumentationIndexEntry entry;
            entry.id = pair.second.name;
            entry.title = pair.second.name;
            entry.summary = pair.second.description;
            entry.category = DocumentationCategory::API;
            entry.file = pair.second.file;
            entry.line = pair.second.line;
            entry.tags = {"api", "function"};
            entry.keywords = extractKeywords(pair.second.description);
            entry.metadata = pair.second.metadata;
            
            index_.push_back(entry);
        }
        
        // Rebuild index from tutorials
        for (const auto& pair : tutorials_) {
            DocumentationIndexEntry entry;
            entry.id = pair.second.title;
            entry.title = pair.second.title;
            entry.summary = pair.second.description;
            entry.category = DocumentationCategory::Tutorial;
            entry.file = ""; // Placeholder
            entry.line = 0;
            entry.tags = {"tutorial"};
            entry.keywords = extractKeywords(pair.second.description);
            entry.metadata = pair.second.metadata;
            
            index_.push_back(entry);
        }
        
        // Rebuild index from examples
        for (const auto& pair : examples_) {
            DocumentationIndexEntry entry;
            entry.id = pair.second.title;
            entry.title = pair.second.title;
            entry.summary = pair.second.description;
            entry.category = DocumentationCategory::Examples;
            entry.file = ""; // Placeholder
            entry.line = 0;
            entry.tags = {"example"};
            entry.keywords = extractKeywords(pair.second.description);
            entry.metadata = pair.second.metadata;
            
            index_.push_back(entry);
        }
        
        if (indexUpdateCallback_) {
            indexUpdateCallback_();
        }
        
        return true;
    }
    
    std::vector<DocumentationIndexEntry> searchIndex(const std::string& query) const override {
        std::vector<DocumentationIndexEntry> results;
        
        std::string lowerQuery = toLower(query);
        
        for (const auto& entry : index_) {
            // Simple relevance calculation
            double relevance = calculateRelevance(entry, lowerQuery);
            if (relevance > 0.1) {
                entry.relevanceScore = relevance;
                results.push_back(entry);
            }
        }
        
        // Sort by relevance
        std::sort(results.begin(), results.end(),
                 [](const DocumentationIndexEntry& a, const DocumentationIndexEntry& b) {
                     return a.relevanceScore > b.relevanceScore;
                 });
        
        return results;
    }
    
    DocumentationIndexEntry getIndexEntry(const std::string& id) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::find_if(index_.begin(), index_.end(),
                              [&id](const DocumentationIndexEntry& entry) { return entry.id == id; });
        if (it == index_.end()) {
            throw std::runtime_error("Index entry not found: " + id);
        }
        
        return *it;
    }
    
    std::vector<DocumentationIndexEntry> getIndexByCategory(DocumentationCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<DocumentationIndexEntry> results;
        for (const auto& entry : index_) {
            if (entry.category == category) {
                results.push_back(entry);
            }
        }
        
        return results;
    }
    
    bool exportDocumentation(const std::string& outputPath, const std::string& format) override {
        if (format == "html") {
            return exportToHTML(outputPath);
        } else if (format == "json") {
            return exportToJSON(outputPath);
        } else {
            return false;
        }
    }
    
    bool exportAPIDocumentation(const std::string& outputPath, const std::string& format) override {
        if (format == "json") {
            return exportAPIToJSON(outputPath);
        } else {
            return false;
        }
    }
    
    bool exportTutorials(const std::string& outputPath, const std::string& format) override {
        if (format == "html") {
            return exportTutorialsToHTML(outputPath);
        } else {
            return false;
        }
    }
    
    bool exportExamples(const std::string& outputPath, const std::string& format) override {
        if (format == "html") {
            return exportExamplesToHTML(outputPath);
        } else {
            return false;
        }
    }
    
    bool cleanGeneratedFiles() override {
        // Clean up generated files
        // This is a placeholder
        return true;
    }
    
    bool validateDocumentation() override {
        // Validate documentation consistency
        // This is a placeholder
        return true;
    }
    
    std::string getDocumentationStatus() const override {
        std::ostringstream oss;
        oss << "=== Documentation Status ===" << std::endl;
        oss << "Sections: " << sections_.size() << std::endl;
        oss << "API documentation entries: " << apiDocs_.size() << std::endl;
        oss << "Tutorials: " << tutorials_.size() << std::endl;
        oss << "Examples: " << examples_.size() << std::endl;
        oss << "Index entries: " << index_.size() << std::endl;
        oss << "Format: " << config_.theme << " theme" << std::endl;
        oss << "Language: " << config_.language << std::endl;
        
        return oss.str();
    }
    
    size_t getSectionCount() const override {
        return sections_.size();
    }
    
    size_t getAPICount() const override {
        return apiDocs_.size();
    }
    
    size_t getTutorialCount() const override {
        return tutorials_.size();
    }
    
    size_t getExampleCount() const override {
        return examples_.size();
    }
    
    size_t getIndexEntryCount() const override {
        return index_.size();
    }
    
    void setDocumentationGeneratedCallback(std::function<void(const std::string&)> callback) override {
        documentationGeneratedCallback_ = callback;
    }
    
    void setGenerationErrorCallback(std::function<void(const std::string&, const std::string&)> callback) override {
        generationErrorCallback_ = callback;
    }
    
    void setIndexUpdateCallback(std::function<void()> callback) override {
        indexUpdateCallback_ = callback;
    }
    
    void setPluginManager(void* manager) override {
        pluginManager_ = static_cast<IPluginManager*>(manager);
    }
    
    void* getPluginManager() const override {
        return pluginManager_;
    }
    
protected:
    void initializeFormatters() {
        // Initialize formatters for different output formats
        sectionFormatters_[["html"]] = [this](const DocumentationSection& section) {
            return formatSectionAsHTML(section);
        };
        sectionFormatters_["json"] = [this](const DocumentationSection& section) {
            return formatSectionAsJSON(section);
        };
        
        apiFormatters_[["html"]] = [this](const APIDocumentation& api) {
            return formatAPIDocumentationAsHTML(api);
        };
        apiFormatters_["json"] = [this](const APIDocumentation& api) {
            return formatAPIDocumentationAsJSON(api);
        };
        
        tutorialFormatters_[["html"]] = [this](const TutorialEntry& tutorial) {
            return formatTutorialEntryAsHTML(tutorial);
        };
        
        exampleFormatters_[["html"]] = [this](const ExamplesEntry& example) {
            return formatExamplesEntryAsHTML(example);
        };
    }
    
    std::vector<std::string> extractKeywords(const std::string& text) const {
        std::vector<std::string> keywords;
        std::istringstream iss(text);
        std::string word;
        
        while (iss >> word) {
            // Simple keyword extraction - in real implementation, use more sophisticated method
            if (word.length() > 3) {
                keywords.push_back(word);
            }
        }
        
        return keywords;
    }
    
    double calculateRelevance(const DocumentationIndexEntry& entry, const std::string& query) const {
        double score = 0.0;
        
        std::string lowerTitle = toLower(entry.title);
        std::string lowerSummary = toLower(entry.summary);
        
        if (lowerTitle.find(query) != std::string::npos) {
            score += 2.0;
        }
        
        if (lowerSummary.find(query) != std::string::npos) {
            score += 1.0;
        }
        
        for (const auto& tag : entry.tags) {
            std::string lowerTag = toLower(tag);
            if (lowerTag.find(query) != std::string::npos) {
                score += 1.5;
            }
        }
        
        return score;
    }
    
    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Export formatters (simplified implementations)
    bool exportToHTML(const std::string& outputPath) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<!DOCTYPE html>" << std::endl;
        file << "<html>" << std::endl;
        file << "  <head>" << std::endl;
        file << "    <title>" << config_.title << "</title>" << std::endl;
        file << "  </head>" << std::endl;
        file << "  <body>" << std::endl;
        file << "    <h1>" << config_.title << "</h1>" << std::endl;
        file << "    <p>" << config_.description << "</p>" << std::endl;
        file << "  </body>" << std::endl;
        file << "</html>" << std::endl;
        
        return true;
    }
    
    bool exportToJSON(const std::string& outputPath) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple JSON export (placeholder)
        file << "{\"title\": \"" << config_.title << "\",\"description\": \"" << config_.description << "\"}" << std::endl;
        
        return true;
    }
    
    bool exportAPIToJSON(const std::string& outputPath) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "[\n";
        
        for (size_t i = 0; i < apiDocs_.size(); ++i) {
            const auto& api = apiDocs_.at(i);
            // In a real implementation, you would serialize the API documentation
            if (i < apiDocs_.size() - 1) {
                file << "," << std::endl;
            }
        }
        
        file << "\n]" << std::endl;
        
        return true;
    }
    
    bool exportTutorialsToHTML(const std::string& outputPath) {
        // Placeholder implementation
        return true;
    }
    
    bool exportExamplesToHTML(const std::string& outputPath) {
        // Placeholder implementation
        return true;
    }
    
    // Formatting methods (simplified implementations)
    std::string formatSectionAsHTML(const DocumentationSection& section) const {
        std::ostringstream oss;
        oss << "<div class=\"section\">" << std::endl;
        oss << "  <h2>" << section.title << "</h2>" << std::endl;
        oss << "  <p>" << section.description << "</p>" << std::endl;
        oss << "</div>" << std::endl;
        return oss.str();
    }
    
    std::string formatSectionAsJSON(const DocumentationSection& section) const {
        std::ostringstream oss;
        oss << "{\"id\": \"" << section.id << "\",\"title\": \"" << section.title << "\",\"category\": \"" << categoryToString(section.category) << "\",\"content\": \"" << escapeJSON(section.content) << "\"}";
        return oss.str();
    }
    
    std::string formatAPIDocumentationAsHTML(const APIDocumentation& api) const {
        std::ostringstream oss;
        oss << "<div class=\"api\">" << std::endl;
        oss << "  <h3>" << api.name << "</h3>" << std::endl;
        oss << "  <code>" << api.signature << "</code>" << std::endl;
        oss << "  <p>" << api.description << "</p>" << std::endl;
        oss << "</div>" << std::endl;
        return oss.str();
    }
    
    std::string formatAPIDocumentationAsJSON(const APIDocumentation& api) const {
        std::ostringstream oss;
        oss << "{\"name\": \"" << api.name << "\",\"signature\": \"" << escapeJSON(api.signature) << "\",\"returnType\": \"" << api.returnType << "\",\"description\": \"" << escapeJSON(api.description) << "\"}";
        return oss.str();
    }
    
    std::string formatTutorialEntryAsHTML(const TutorialEntry& tutorial) const {
        std::ostringstream oss;
        oss << "<div class=\"tutorial\">" << std::endl;
        oss << "  <h3>" << tutorial.title << "</h3>" << std::endl;
        oss << "  <p>" << tutorial.description << "</p>" << std::endl;
        oss << "</div>" << std::endl;
        return oss.str();
    }
    
    std::string formatExamplesEntryAsHTML(const ExamplesEntry& example) const {
        std::ostringstream oss;
        oss << "<div class=\"example\">" << std::endl;
        oss << "  <h3>" << example.title << "</h3>" << std::endl;
        oss << "  <pre>" << escapeHTML(example.code) << "</pre>" << std::endl;
        oss << "  <p>Output: " << escapeHTML(example.output) << "</p>" << std::endl;
        oss << "</div>" << std::endl;
        return oss.str();
    }
    
    std::string categoryToString(DocumentationCategory category) const {
        switch (category) {
            case DocumentationCategory::API: return "api";
            case DocumentationCategory::Tutorial: return "tutorial";
            case DocumentationCategory::Concept: return "concept";
            case DocumentationCategory::Reference: return "reference";
            case DocumentationCategory::Examples: return "examples";
            case DocumentationCategory::BestPractices: return "best-practices";
            case DocumentationCategory::Troubleshooting: return "troubleshooting";
            case DocumentationCategory::Migration: return "migration";
            case DocumentationCategory::Changelog: return "changelog";
            case DocumentationCategory::FAQ: return "faq";
            case DocumentationCategory::Architecture: return "architecture";
            default: return "unknown";
        }
    }
    
    std::string escapeJSON(const std::string& input) const {
        std::string escaped;
        for (char c : input) {
            switch (c) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        std::stringstream ss;
                        ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                        escaped += ss.str();
                    } else {
                        escaped += c;
                    }
                    break;
            }
        }
        return escaped;
    }
    
    std::string escapeHTML(const std::string& input) const {
        std::string escaped;
        for (char c : input) {
            switch (c) {
                case '&': escaped += "&amp;"; break;
                case '<': escaped += "&lt;"; break;
                case '>': escaped += "&gt;"; break;
                case '"': escaped += "&quot;"; break;
                case '\'': escaped += "&#39;"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
}; // namespace nlm

// Global documentation generator instance
IDocumentation& getGlobalDocumentationGenerator() {
    static DefaultDocumentationGenerator generator;
    return generator;
}

// Convenience functions for documentation
inline bool generateDocumentation() {
    return getGlobalDocumentationGenerator().generateDocumentation();
}

inline bool exportDocumentation(const std::string& outputPath, const std::string& format = "html") {
    return getGlobalDocumentationGenerator().exportDocumentation(outputPath, format);
}

inline bool addDocumentationSection(const DocumentationSection& section) {
    return getGlobalDocumentationGenerator().addSection(section);
}

inline bool addAPIDocumentation(const APIDocumentation& api) {
    return getGlobalDocumentationGenerator().addAPIDocumentation(api);
}

inline bool addTutorial(const TutorialEntry& tutorial) {
    return getGlobalDocumentationGenerator().addTutorial(tutorial);
}

inline bool addExample(const ExamplesEntry& example) {
    return getGlobalDocumentationGenerator().addExample(example);
}

inline std::vector<DocumentationIndexEntry> searchDocumentation(const std::string& query) {
    return getGlobalDocumentationGenerator().searchIndex(query);
}