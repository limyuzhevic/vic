#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "ExperimentProtocol.hpp"
#include "ExperimentResults.hpp"
#include "ExperimentMetrics.hpp"

namespace nlm {

// Experiment report configuration
struct ExperimentReportConfig {
    std::string name;
    std::string title;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point creationTime;
    std::vector<std::string> recipients;
    std::map<std::string, std::string> formattingOptions;
    std::map<std::string, double> layoutParameters;
    bool includeExecutiveSummary;
    bool includeDetailedResults;
    bool includeStatisticalAnalysis;
    bool includeVisualization;
    bool includeRecommendations;
    bool includeAppendices;
    std::string defaultOutputFormat;
    std::map<std::string, std::string> customSections;
    
    ExperimentReportConfig() 
        : creationTime(std::chrono::system_clock::now()), includeExecutiveSummary(true),
          includeDetailedResults(true), includeStatisticalAnalysis(true), includeVisualization(true),
          includeRecommendations(true), includeAppendices(true), defaultOutputFormat("html") {}
};

// Report section
struct ReportSection {
    std::string sectionId;
    std::string title;
    std::string content;
    std::vector<std::string> subsections;
    std::map<std::string, std::string> sectionMetadata;
    std::chrono::system_clock::time_point creationTime;
    
    ReportSection() : creationTime(std::chrono::system_clock::now()) {}
    ReportSection(const std::string& id, const std::string& title) 
        : sectionId(id), title(title), creationTime(std::chrono::system_clock::now()) {}
};

// Report visualization
struct ReportVisualization {
    std::string visualizationId;
    std::string title;
    std::string type; // "chart", "graph", "table", "heatmap", "histogram"
    std::string dataSource;
    std::map<std::string, std::string> visualizationParams;
    std::string visualizationCode;
    std::string visualizationOutput;
    
    ReportVisualization() {}
    ReportVisualization(const std::string& id, const std::string& title, const std::string& type)
        : visualizationId(id), title(title), type(type) {}
};

// Report recommendation
struct ReportRecommendation {
    std::string recommendationId;
    std::string title;
    std::string description;
    std::string priority; // "high", "medium", "low"
    std::string actionRequired;
    std::chrono::system_clock::time_point deadline;
    std::string responsiblePerson;
    std::map<std::string, std::string> recommendationMetadata;
    
    ReportRecommendation() : deadline(std::chrono::system_clock::now() + std::chrono::days(30)) {}
    ReportRecommendation(const std::string& id, const std::string& title, const std::string& priority)
        : recommendationId(id), title(title), priority(priority), 
          deadline(std::chrono::system_clock::now() + std::chrono::days(30)) {}
};

class ExperimentReport {
public:
    ExperimentReport();
    ~ExperimentReport();
    
    // Disable copying, enable moving
    ExperimentReport(const ExperimentReport&) = delete;
    ExperimentReport& operator=(const ExperimentReport&) = delete;
    ExperimentReport(ExperimentReport&&) noexcept;
    ExperimentReport& operator=(ExperimentReport&&) noexcept;
    
    // Report generation
    bool generate(const std::string& experimentName, const std::string& outputPath);
    bool generateSummary(const std::string& experimentName);
    bool generateDetailedReport(const std::string& experimentName);
    bool generateStatisticalAnalysis(const std::string& experimentName);
    bool generateVisualization(const std::string& experimentName);
    
    // Report sections management
    bool addSection(const std::string& experimentName, const ReportSection& section);
    bool removeSection(const std::string& experimentName, const std::string& sectionId);
    bool updateSection(const std::string& experimentName, const ReportSection& section);
    ReportSection getSection(const std::string& experimentName, const std::string& sectionId) const;
    std::vector<ReportSection> getAllSections(const std::string& experimentName) const;
    
    // Report visualization management
    bool addVisualization(const std::string& experimentName, const ReportVisualization& visualization);
    bool removeVisualization(const std::string& experimentName, const std::string& visualizationId);
    bool updateVisualization(const std::string& experimentName, const ReportVisualization& visualization);
    ReportVisualization getVisualization(const std::string& experimentName, const std::string& visualizationId) const;
    std::vector<ReportVisualization> getAllVisualizations(const std::string& experimentName) const;
    
    // Report recommendation management
    bool addRecommendation(const std::string& experimentName, const ReportRecommendation& recommendation);
    bool removeRecommendation(const std::string& experimentName, const std::string& recommendationId);
    bool updateRecommendation(const std::string& experimentName, const ReportRecommendation& recommendation);
    ReportRecommendation getRecommendation(const std::string& experimentName, const std::string& recommendationId) const;
    std::vector<ReportRecommendation> getAllRecommendations(const std::string& experimentName) const;
    
    // Report content generation
    bool generateExecutiveSummary(const std::string& experimentName);
    bool generateMethodologySection(const std::string& experimentName);
    bool generateResultsSection(const std::string& experimentName);
    bool generateAnalysisSection(const std::string& experimentName);
    bool generateDiscussionSection(const std::string& experimentName);
    bool generateConclusionSection(const std::string& experimentName);
    bool generateAppendixSection(const std::string& experimentName);
    
    // Report formatting
    bool formatReport(const std::string& experimentName, const std::string& format);
    bool setFormatOptions(const std::string& experimentName, 
                         const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getFormatOptions(const std::string& experimentName) const;
    
    // Report export
    bool exportToPDF(const std::string& experimentName, const std::string& filepath) const;
    bool exportToHTML(const std::string& experimentName, const std::string& filepath) const;
    bool exportToDOCX(const std::string& experimentName, const std::string& filepath) const;
    bool exportToTXT(const std::string& experimentName, const std::string& filepath) const;
    bool exportToJSON(const std::string& experimentName, const std::string& filepath) const;
    bool exportToXML(const std::string& experimentName, const std::string& filepath) const;
    
    // Report import
    bool importFromPDF(const std::string& experimentName, const std::string& filepath);
    bool importFromHTML(const std::string& experimentName, const std::string& filepath);
    bool importFromDOCX(const std::string& experimentName, const std::string& filepath);
    bool importFromTXT(const std::string& experimentName, const std::string& filepath);
    bool importFromJSON(const std::string& experimentName, const std::string& filepath);
    bool importFromXML(const std::string& experimentName, const std::string& filepath);
    
    // Report querying
    std::string getReportContent(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getReportCreationTime(const std::string& experimentName) const;
    std::string getReportTitle(const std::string& experimentName) const;
    std::string getReportAuthor(const std::string& experimentName) const;
    std::string getReportVersion(const std::string& experimentName) const;
    std::vector<std::string> getReportRecipients(const std::string& experimentName) const;
    
    // Report configuration
    void setReportConfig(const std::string& experimentName, const ExperimentReportConfig& config);
    ExperimentReportConfig getReportConfig(const std::string& experimentName) const;
    void setReportTitle(const std::string& experimentName, const std::string& title);
    void setReportAuthor(const std::string& experimentName, const std::string& author);
    void setReportVersion(const std::string& experimentName, const std::string& version);
    void addReportRecipient(const std::string& experimentName, const std::string& recipient);
    void removeReportRecipient(const std::string& experimentName, const std::string& recipient);
    
    // Report statistics
    size_t getReportSectionCount(const std::string& experimentName) const;
    size_t getReportVisualizationCount(const std::string& experimentName) const;
    size_t getReportRecommendationCount(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getReportLastModified(const std::string& experimentName) const;
    std::string getReportFilePath(const std::string& experimentName) const;
    
    // Report analysis
    std::string analyzeReportContent(const std::string& experimentName) const;
    bool validateReportFormat(const std::string& experimentName) const;
    std::string getReportQualityScore(const std::string& experimentName) const;
    
    // Report optimization
    bool optimizeReportGeneration(const std::string& experimentName);
    bool enableReportCaching(const std::string& experimentName, bool enable);
    bool setReportCompressionLevel(const std::string& experimentName, int level);
    bool enableReportValidation(const std::string& experimentName, bool enable);
    
    // Report cleanup
    bool cleanupReport(const std::string& experimentName);
    bool cleanupOldReports(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetReport(const std::string& experimentName);
    bool clearAllReports(const std::string& experimentName);
    std::vector<std::string> getGeneratedReports() const;
    size_t getReportCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm