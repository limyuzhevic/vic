#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <map>
#include "Visualizer.hpp"

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

/**
 * HTML export system for NLM
 * 
 * Provides comprehensive HTML export capabilities for:
 * - Neural network visualizations
 * - Spike activity plots
 * - Learning progress charts
 * - Performance metrics dashboards
 * - Interactive web pages
 * - Responsive HTML5 pages
 * - Embedded JavaScript visualizations
 * - External chart libraries integration
 * - Custom HTML templates
 * - Report generation
 * - Documentation generation
 * - Web-based presentations
 * - Static site generation
 * - HTML5 canvas exports
 * - Data table exports
 * - Excel-compatible HTML
 * 
 * Supported HTML formats:
 * - Standard HTML5
 * - Responsive design
 * - Mobile-friendly
 * - Print-optimized
 * - PDF-convertible
 * - Email-compatible
 * - Shareable links
 * - Browser-based viewing
 * - No server required
 * - Offline capable
 * 
 * Features:
 * - Multiple template support
 * - Custom styling
 * - JavaScript integration
 * - Chart.js support
 * - D3.js support
 * - Plotly support
 * - Three.js support
 * - Canvas rendering
 * - SVG export
 * - Progress tracking
 * - Error handling
 * - Compression
 * - Validation
 * - Accessibility support
 * - SEO optimization
 * 
 * Integration with:
 * - All visualization systems
 * - Export systems
 * * Data collection systems
 * - WebDashboard
 * - AnimationGenerator
 * - Experiment management
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 * @see WebDashboard
 */
class HTMLExporter {
public:
    /**
     * HTML template enumeration
     */
    enum class TemplateType {
        DEFAULT,
        DASHBOARD,
        REPORT,
        PRESENTATION,
        EXPERIMENTAL,
        DEBUG,
        MINIMAL,
        MODERN,
        CLASSIC,
        CUSTOM
    };

    /**
     * Chart library enumeration
     */
    enum class ChartLibrary {
        NONE,
        CHARTJS,
        PLOTLY,
        D3JS,
        THREEJS,
        PICTORJS,
        LOLLYPOP,
        VUE_CHART,
        REACT_CHART
    };

    /**
     * Page configuration
     */
    struct PageConfig {
        std::string title = "NLM Visualization Report";
        std::string description = "Neural Learning Machine Visualization Report";
        std::string author = "NLM Team";
        std::string keywords = "NLM, visualization, neural network, spikes, learning";
        std::string theme = "light"; // light, dark, auto
        bool responsive = true;
        bool mobileOptimized = true;
        bool printOptimized = false;
        std::string favicon = "favicon.ico";
        std::string cssFile = "styles.css";
        std::string jsFile = "script.js";
        std::string chartLibrary = "Chart.js";
        bool includeCharts = true;
        bool includeInteractivity = true;
        bool enableAnimations = true;
        double animationSpeed = 1.0;
        std::string dateFormat = "%Y-%m-%d %H:%M:%S";
        bool includeMetadata = true;
        bool includeTOC = true;
        std::vector<std::string> customCSS;
        std::vector<std::string> customJS;
    };

    /**
     * Chart configuration
     */
    struct ChartConfig {
        std::string id;
        std::string title;
        std::string type; // line, bar, scatter, pie, etc.
        std::vector<std::string> dataColumns;
        std::string xAxisLabel;
        std::string yAxisLabel;
        std::string colorScheme = "default";
        bool showLegend = true;
        bool showGrid = true;
        bool responsive = true;
        double width = 800.0;
        double height = 400.0;
        std::string dataSource; // URL, file, or inline data
        std::string chartOptions;
        std::string chartPlugins;
    };

    /**
     * Section configuration
     */
    struct Section {
        std::string id;
        std::string title;
        std::string type; // content, chart, table, code, image
        int order = 0;
        bool visible = true;
        std::string content;
        std::vector<ChartConfig> charts;
        std::vector<std::pair<std::string, std::string>> dataTable;
        std::string codeSnippet;
        std::string imagePath;
        std::unordered_map<std::string, std::string> metadata;
    };

    /**
     * HTML export configuration
     */
    struct HTMLExportConfig {
        TemplateType template = TemplateType::DASHBOARD;
        PageConfig pageConfig;
        std::string outputPath = ".";
        std::string filename = "visualization_report";
        std::string cssPath = "styles.css";
        std::string jsPath = "script.js";
        std::string chartLibrary = "Chart.js";
        bool includeCharts = true;
        bool includeChartsLibrary = true;
        bool enableNavigation = true;
        bool enableSearch = true;
        bool enablePrint = true;
        bool generatePDF = false;
        bool generateSVG = false;
        bool includeAllVisualizations = true;
        std::vector<std::string> additionalFiles;
        std::unordered_map<std::string, std::string> customSettings;
        bool enableCompression = true;
        bool validateHTML = true;
        std::string validationTool = "html5validator";
    };

    /**
     * Export statistics
     */
    struct ExportStats {
        int totalSections = 0;
        int totalCharts = 0;
        int totalImages = 0;
        int totalFiles = 0;
        double totalFileSizeMB = 0.0;
        double generationTime = 0.0;
        bool completed = false;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    /**
     * Export callback function
     */
    using ExportCallback = std::function<void(double progress, const std::string& status)>;

    /**
     * HTMLExporter constructor
     */
    HTMLExporter();
    
    /**
     * HTMLExporter constructor with configuration
     */
    explicit HTMLExporter(const HTMLExportConfig& config);
    
    /**
     * HTMLExporter destructor
     */
    ~HTMLExporter();

    /**
     * Configure HTML export
     */
    void configure(const HTMLExportConfig& config);
    
    /**
     * Export brain visualization to HTML
     */
    bool exportBrainVisualization(const Brain& brain, const std::string& outputPath = ".");
    
    /**
     * Export spike visualization to HTML
     */
    bool exportSpikeVisualization(const std::vector<SpikeEvent>& spikes, const std::string& outputPath = ".");
    
    /**
     * Export network visualization to HTML
     */
    bool exportNetworkVisualization(const Brain& brain, const std::string& outputPath = ".");
    
    /**
     * Export learning progress to HTML
     */
    bool exportLearningProgress(const std::string& outputPath = ".");
    
    /**
     * Export performance metrics to HTML
     */
    bool exportPerformanceMetrics(const std::string& outputPath = ".");
    
    /**
     * Export comprehensive report
     */
    bool exportComprehensiveReport(const Brain& brain, const std::vector<SpikeEvent>& spikes, const std::string& outputPath = ".");
    
    /**
     * Add section to report
     */
    void addSection(const Section& section);
    
    /**
     * Add chart to report
     */
    void addChart(const ChartConfig& chart);
    
    /**
     * Set data source
     */
    void setDataSource(const std::function<std::vector<std::vector<std::string>>(double time)>& dataSource);
    
    /**
     * Get export statistics
     */
    ExportStats getStats() const;
    
    /**
     * Check if export is complete
     */
    bool isComplete() const;
    
    /**
     * Generate HTML from sections
     */
    std::string generateHTML() const;
    
    /**
     * Generate CSS styles
     */
    std::string generateCSS() const;
    
    /**
     * Generate JavaScript
     */
    std::string generateJavaScript() const;
    
    /**
     * Generate charts
     */
    std::string generateCharts() const;
    
    /**
     * Generate navigation
     */
    std::string generateNavigation() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    HTMLExporter(const HTMLExporter&) = delete;
    HTMLExporter& operator=(const HTMLExporter&) = delete;
};

} // namespace nlm
