#pragma once

#include "../experiments/ExperimentRunner.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>

namespace nlm {

// Forward declarations
class VisualizationInterface;

// Visualization report types
enum class ReportFormat {
    Text,
    HTML,
    LaTeX,
    Markdown,
    JSON,
    CSV,
    PDF,
    PNG,
    SVG
};

// Chart types
enum class ChartType {
    LineChart,
    BarChart,
    ScatterPlot,
    Histogram,
    BoxPlot,
    Heatmap,
    TimeSeries,
    Comparison,
    Progress
};

// Experiment analysis results
struct ExperimentAnalysis {
    std::string experimentName;
    double averagePerformance;
    double performanceStdDev;
    double performanceVariance;
    double coefficientOfVariation;
    double minPerformance;
    double maxPerformance;
    double medianPerformance;
    double percentile25;
    double percentile75;
    double percentile95;
    size_t successCount;
    size_t failureCount;
    double successRate;
    std::map<std::string, double> metricStatistics;
    std::vector<double> performanceHistory;
    
    ExperimentAnalysis() : averagePerformance(0), performanceStdDev(0), 
                          performanceVariance(0), coefficientOfVariation(0),
                          minPerformance(0), maxPerformance(0), medianPerformance(0),
                          percentile25(0), percentile75(0), percentile95(0),
                          successCount(0), failureCount(0), successRate(0) {}
};

// Visualization data structure
struct VisualizationData {
    std::string title;
    std::string subtitle;
    std::string xLabel;
    std::string yLabel;
    std::vector<std::string> categories;
    std::vector<double> values;
    std::vector<std::pair<double, double>> errorBars;
    std::map<std::string, std::vector<double>> series;
    std::map<std::string, std::string> metadata;
    
    VisualizationData() : title("Untitled"), subtitle(""), xLabel("X"), yLabel("Y") {}
};

// Advanced visualization and reporting system
class AdvancedVisualizationReporter {
public:
    AdvancedVisualizationReporter() = default;
    ~AdvancedVisualizationReporter() = default;
    
    // === Report Generation ===
    
    // Generate comprehensive experiment report
    static std::string generateExperimentReport(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "NLM Experiment Report",
        ReportFormat format = ReportFormat::Text);
    
    // Generate statistical analysis report
    static std::string generateStatisticalReport(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Statistical Analysis Report",
        ReportFormat format = ReportFormat::Text);
    
    // Generate comparative report
    static std::string generateComparativeReport(
        const std::vector<ExperimentAnalysis>& analyses,
        const std::vector<std::string>& labels,
        const std::string& title = "Comparative Analysis Report",
        ReportFormat format = ReportFormat::Text);
    
    // === Visualization Generation ===
    
    // Generate progress chart
    static std::string generateProgressChart(
        const VisualizationData& data,
        ChartType type = ChartType::Progress,
        ReportFormat format = ReportFormat::Text);
    
    // Generate performance comparison chart
    static std::string generatePerformanceComparisonChart(
        const std::vector<ExperimentAnalysis>& analyses,
        ChartType type = ChartType::BarChart,
        ReportFormat format = ReportFormat::Text);
    
    // Generate learning curve visualization
    static std::string generateLearningCurveVisualization(
        const std::vector<Experiment::StepData>& stepData,
        const std::string& title = "Learning Curve",
        ReportFormat format = ReportFormat::Text);
    
    // Generate histogram visualization
    static std::string generateHistogramVisualization(
        const std::vector<double>& data,
        const std::string& title = "Data Distribution",
        const std::string& xLabel = "Value",
        const std::string& yLabel = "Frequency",
        ReportFormat format = ReportFormat::Text);
    
    // Generate box plot visualization
    static std::string generateBoxPlotVisualization(
        const std::vector<double>& data,
        const std::string& title = "Box Plot",
        ReportFormat format = ReportFormat::Text);
    
    // Generate scatter plot visualization
    static std::string generateScatterPlotVisualization(
        const std::vector<double>& xData,
        const std::vector<double>& yData,
        const std::string& title = "Scatter Plot",
        const std::string& xLabel = "X",
        const std::string& yLabel = "Y",
        ReportFormat format = ReportFormat::Text);
    
    // Generate heatmap visualization
    static std::string generateHeatmapVisualization(
        const std::vector<std::vector<double>>& data,
        const std::string& title = "Heatmap",
        ReportFormat format = ReportFormat::Text);
    
    // === Data Export ===
    
    // Export results to JSON with enhanced structure
    static bool exportResultsToEnhancedJSON(
        const std::vector<ExperimentResult>& results,
        const std::string& filepath,
        bool includeMetadata = true,
        bool includeStepData = true);
    
    // Export results to CSV with comprehensive statistics
    static bool exportResultsToEnhancedCSV(
        const std::vector<ExperimentResult>& results,
        const std::string& filepath,
        bool includeDetailedMetrics = true,
        bool includeStatisticalSummary = true);
    
    // Export results to HTML with embedded visualizations
    static bool exportResultsToHTML(
        const std::vector<ExperimentResult>& results,
        const std::string& filepath,
        const std::string& title = "Experiment Results Report",
        bool includeCharts = true,
        bool includeStatistics = true);
    
    // Export results to LaTeX with professional formatting
    static bool exportResultsToLaTeX(
        const std::vector<ExperimentResult>& results,
        const std::string& filepath,
        const std::string& title = "NLM Experiment Analysis",
        bool includeTables = true,
        bool includeFigures = true);
    
    // Export results to Markdown with formatting
    static bool exportResultsToMarkdown(
        const std::vector<ExperimentResult>& results,
        const std::string& filepath,
        const std::string& title = "NLM Experiment Report",
        bool includeCharts = true,
        bool includeStatistics = true);
    
    // === Statistical Analysis ===
    
    // Analyze experiment results
    static ExperimentAnalysis analyzeExperimentResults(
        const std::vector<ExperimentResult>& results);
    
    // Calculate confidence intervals
    static std::pair<double, double> calculateConfidenceInterval(
        const std::vector<double>& data,
        double confidenceLevel = 0.95);
    
    // Perform hypothesis testing
    static struct HypothesisTest {
        std::string testName;
        double testStatistic;
        double pValue;
        std::string conclusion;
    } performHypothesisTest(
        const std::vector<double>& sample1,
        const std::vector<double>& sample2,
        const std::string& testName = "Two-sample t-test");
    
    // Calculate effect size
    static double calculateEffectSize(
        const std::vector<double>& sample1,
        const std::vector<double>& sample2);
    
    // Detect outliers using various methods
    static std::vector<size_t> detectOutliers(
        const std::vector<double>& data,
        const std::string& method = "iqr"); // iqr, zscore, median_absolute_dev
    
    // === Trend Analysis ===
    
    // Analyze trends in step data
    static struct TrendAnalysis {
        double trendSlope;
        double rSquared;
        double pValue;
        std::string trendDescription;
        bool isSignificant;
    } analyzeTrend(
        const std::vector<Experiment::StepData>& stepData,
        const std::string& variable = "avgFiringRate");
    
    // Detect learning plateaus
    static struct PlateauDetection {
        bool hasPlateau;
        size_t plateauStart;
        size_t plateauEnd;
        double plateauLevel;
        double plateauDuration;
    } detectLearningPlateau(
        const std::vector<Experiment::StepData>& stepData,
        double threshold = 0.01,
        size_t windowSize = 100);
    
    // Analyze convergence rate
    static double analyzeConvergenceRate(
        const std::vector<Experiment::StepData>& stepData,
        double convergenceThreshold = 0.05);
    
    // === Report Template Generation ===
    
    // Generate HTML report template
    static std::string generateHTMLTemplate(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "NLM Experiment Report");
    
    // Generate LaTeX report template
    static std::string generateLaTeXTemplate(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "NLM Experiment Analysis");
    
    // Generate Markdown report template
    static std::string generateMarkdownTemplate(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "NLM Experiment Report");
    
    // === Utility Functions ===
    
    // Format number with precision
    static std::string formatNumber(double value, int precision = 4);
    
    // Generate timestamp
    static std::string generateTimestamp();
    
    // Generate unique ID
    static std::string generateUUID();
    
    // Validate data for visualization
    static bool validateData(const VisualizationData& data);
    
    // Normalize data for visualization
    static std::vector<double> normalizeData(const std::vector<double>& data);
    
    // Calculate statistics for visualization
    static struct DataStatistics {
        double mean;
        double median;
        double stddev;
        double variance;
        double min;
        double max;
        double q1;
        double q3;
    } calculateStatistics(const std::vector<double>& data);
    
    // Generate color palette for visualizations
    static std::vector<std::string> getColorPalette(size_t count = 10);
    
    // Generate font sizes for different report levels
    static struct FontSizes {
        int title;
        int heading1;
        int heading2;
        int heading3;
        int body;
        int caption;
    } getFontSizes();
};

// Professional visualization interface
class ProfessionalVisualizationInterface {
public:
    ProfessionalVisualizationInterface() = default;
    ~ProfessionalVisualizationInterface() = default;
    
    // Create enhanced visualization
    static std::string createEnhancedChart(
        const VisualizationData& data,
        ChartType type,
        ReportFormat format = ReportFormat::PNG,
        const std::string& outputPath = "chart.png");
    
    // Create comprehensive dashboard
    static std::string createDashboard(
        const std::vector<VisualizationData>& charts,
        ReportFormat format = ReportFormat::HTML,
        const std::string& title = "NLM Experiment Dashboard");
    
    // Create statistical dashboard
    static std::string createStatisticalDashboard(
        const std::vector<ExperimentAnalysis>& analyses,
        ReportFormat format = ReportFormat::HTML,
        const std::string& title = "Statistical Dashboard");
    
    // Create learning analysis dashboard
    static std::string createLearningAnalysisDashboard(
        const std::vector<ExperimentResult>& results,
        ReportFormat format = ReportFormat::HTML,
        const std::string& title = "Learning Analysis Dashboard");
    
    // Create performance comparison dashboard
    static std::string createPerformanceComparisonDashboard(
        const std::vector<ExperimentAnalysis>& analyses,
        ReportFormat format = ReportFormat::HTML,
        const std::string& title = "Performance Comparison Dashboard");
};

// Advanced reporting utilities
class AdvancedReportingUtilities {
public:
    AdvancedReportingUtilities() = default;
    ~AdvancedReportingUtilities() = default;
    
    // Generate executive summary
    static std::string generateExecutiveSummary(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Executive Summary");
    
    // Generate technical appendix
    static std::string generateTechnicalAppendix(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Technical Appendix");
    
    // Generate methodology section
    static std::string generateMethodologySection(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Methodology");
    
    // Generate results interpretation
    static std::string generateResultsInterpretation(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Results Interpretation");
    
    // Generate recommendations
    static std::string generateRecommendations(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Recommendations");
    
    // Generate conclusion
    static std::string generateConclusion(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Conclusion");
    
    // Generate future directions
    static std::string generateFutureDirections(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Future Directions");
    
    // Generate bibliography
    static std::string generateBibliography(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Bibliography");
    
    // Generate acknowledgments
    static std::string generateAcknowledgments(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Acknowledgments");
};

// Specialized visualization creators
class SpecializedVisualizationCreator {
public:
    SpecializedVisualizationCreator() = default;
    ~SpecializedVisualizationCreator() = default;
    
    // Create learning curve with trend analysis
    static std::string createLearningCurveWithTrend(
        const std::vector<Experiment::StepData>& stepData,
        const std::string& title = "Learning Curve with Trend Analysis");
    
    // Create performance distribution plot
    static std::string createPerformanceDistributionPlot(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Performance Distribution");
    
    // Create success rate comparison
    static std::string createSuccessRateComparison(
        const std::vector<ExperimentAnalysis>& analyses,
        const std::string& title = "Success Rate Comparison");
    
    // Create convergence analysis plot
    static std::string createConvergenceAnalysisPlot(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Convergence Analysis");
    
    // Create parameter sensitivity analysis
    static std::string createParameterSensitivityAnalysis(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Parameter Sensitivity Analysis");
    
    // Create robustness analysis
    static std::string createRobustnessAnalysis(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Robustness Analysis");
    
    // Create optimization visualization
    static std::string createOptimizationVisualization(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Optimization Visualization");
    
    // Create uncertainty quantification
    static std::string createUncertaintyQuantification(
        const std::vector<ExperimentResult>& results,
        const std::string& title = "Uncertainty Quantification");
};

} // namespace nlm