// Visualization and analytics utilities for NLM
// Comprehensive visualization and analytics tools for NLM simulations

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations
class VisualizationEngine;
class AnalyticsEngine;

// Visualization data structures
struct NeuralActivityData {
    std::vector<double> firingRates;
    std::vector<double> membranePotentials;
    std::vector<double> weights;
    std::vector<double> spikeTimes;
    std::vector<std::string> neuronTypes;
    double timestamp;
    std::string label;
    
    NeuralActivityData() : timestamp(0.0), label("Default") {}
    NeuralActivityData(const std::string& lbl) : timestamp(0.0), label(lbl) {}
};

struct NetworkTopology {
    std::vector<std::string> neuronIds;
    std::vector<std::vector<double>> adjacencyMatrix;
    std::vector<std::string> connectionTypes;
    std::vector<double> connectionWeights;
    std::string layout;
    std::map<std::string, std::vector<double>> nodePositions;
    std::map<std::string, std::string> nodeAttributes;
};

struct VisualizationResult {
    std::string type;           // "plot", "chart", "heatmap", "graph", "animation"
    std::string format;          // "png", "svg", "json", "csv", "html"
    std::string data;            // JSON string containing visualization data
    std::string filePath;        // Path to saved visualization file
    std::string title;
    std::string description;
    double generationTime;
    size_t dataSize;
    bool success;
    std::string errorMessage;
    
    VisualizationResult() : generationTime(0.0), dataSize(0), success(false) {}
};

struct AnalyticsReport {
    std::string reportType;      // "performance", "learning", "connectivity", "stability"
    std::string timePeriod;      // "step", "epoch", "simulation"
    std::map<std::string, double> metrics;      // Key metrics
    std::map<std::string, std::string> insights; // Key insights
    std::vector<std::string> recommendations;   // Recommendations
    std::string summary;          // Summary of findings
    double confidence;           // Confidence level (0.0 to 1.0)
    std::chrono::system_clock::time_point timestamp;
    
    AnalyticsReport() : confidence(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

class VisualizationEngine {
public:
    enum class VisualizationType {
        NEURAL_ACTIVITY,
        NETWORK_TOPOLOGY,
        SPIKE_RATIOS,
        WEIGHT_DISTRIBUTION,
        SYNAPTIC_CONNECTIVITY,
        TEMPORAL_DYNAMICS,
        COMPARATIVE_ANALYSIS,
        HEATMAP,
        PROGRESSION,
        STATISTICS
    };
    
    enum class OutputFormat {
        PNG,
        SVG,
        JSON,
        CSV,
        HTML,
        PDF,
        TEXT,
        PYTHON
    };
    
    struct Config {
        VisualizationType type;
        OutputFormat format;
        std::string title;
        std::string filename;
        double width = 800.0;
        double height = 600.0;
        std::map<std::string, std::string> options;
        bool includeLegend = true;
        bool interactive = false;
        std::string colorScheme = "default";
        double dpi = 300.0;
        std::string fontFamily = "Arial";
        int fontSize = 12;
    };
    
    static std::shared_ptr<VisualizationEngine> create() {
        return std::make_shared<VisualizationEngine>();
    }
    
    // Core visualization methods
    VisualizationResult visualizeNeuralActivity(std::shared_ptr<Brain> brain, const Config& config);
    VisualizationResult visualizeNetworkTopology(std::shared_ptr<Brain> brain, const Config& config);
    VisualizationResult visualizeSpikeRatios(std::shared_ptr<Brain> brain, const Config& config);
    VisualizationResult visualizeWeightDistribution(std::shared_ptr<Brain> brain, const Config& config);
    VisualizationResult visualizeSynapticConnectivity(std::shared_ptr<Brain> brain, const Config& config);
    VisualizationResult visualizeTemporalDynamics(const std::vector<NeuralActivityData>& timeSeries, const Config& config);
    VisualizationResult createComparativeAnalysis(const std::vector<std::shared_ptr<Brain>>& brains, const Config& config);
    VisualizationResult createHeatmap(const std::vector<std::vector<double>>& data, const Config& config);
    VisualizationResult createProgressionPlot(const std::vector<NeuralActivityData>& progressionData, const Config& config);
    VisualizationResult createStatisticsPlot(const std::map<std::string, double>& statistics, const Config& config);
    
    // Utility methods
    static std::string generateJSON(const std::map<std::string, double>& data);
    static std::string generateCSV(const std::vector<std::vector<double>>& data);
    static std::vector<std::vector<double>> compute2DHistogram(const std::vector<double>& data1, const std::vector<double>& data2, int bins = 50);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static std::vector<double> normalizeData(const std::vector<double>& data, double min = 0.0, double max = 1.0);
    static std::string getColorScheme(const std::string& scheme, int index, int total);
    
    // Export methods
    static bool exportToJSON(const std::string& filepath, const std::map<std::string, double>& data);
    static bool exportToCSV(const std::string& filepath, const std::vector<std::vector<double>>& data);
    static bool exportToHTML(const std::string& filepath, const std::string& htmlContent);
    
    // Configuration helpers
    static Config createNeuralActivityConfig(const std::string& title = "Neural Activity", 
                                             const std::string& filename = "neural_activity.png");
    static Config createNetworkTopologyConfig(const std::string& title = "Network Topology",
                                             const std::string& filename = "network_topology.png");
    static Config createSpikeRatioConfig(const std::string& title = "Spike Ratios",
                                         const std::string& filename = "spike_ratios.png");
    static Config createDefaultConfig() {
        Config config;
        config.type = VisualizationType::NEURAL_ACTIVITY;
        config.format = OutputFormat::PNG;
        config.width = 800.0;
        config.height = 600.0;
        config.includeLegend = true;
        config.interactive = false;
        return config;
    }
};

class AnalyticsEngine {
public:
    enum class AnalyticsType {
        PERFORMANCE_ANALYSIS,
        LEARNING_PROGRESSION,
        CONNECTIVITY_ANALYSIS,
        STABILITY_ASSESSMENT,
        NOVELTY_DETECTION,
        PATTERN_RECOGNITION,
        PREDICTION_ANALYSIS,
        SYSTEM_HEALTH
    };
    
    struct AnalysisConfig {
        AnalyticsType type;
        std::string timeRange;     // "step", "epoch", "all"
        int maxSamples = 1000;
        double confidenceThreshold = 0.95;
        std::map<std::string, double> parameters;
        bool includeStatisticalTests = true;
        bool generatePredictions = false;
        std::vector<std::string> customMetrics;
    };
    
    static std::shared_ptr<AnalyticsEngine> create() {
        return std::make_shared<AnalyticsEngine>();
    }
    
    // Core analytics methods
    AnalyticsReport analyzePerformance(std::shared_ptr<Brain> brain, const AnalysisConfig& config);
    AnalyticsReport analyzeLearningProgression(const std::vector<NeuralActivityData>& timeSeries, const AnalysisConfig& config);
    AnalyticsReport analyzeConnectivity(std::shared_ptr<Brain> brain, const AnalysisConfig& config);
    AnalyticsReport assessStability(const std::vector<double>& metricTimeSeries, const AnalysisConfig& config);
    AnalyticsReport detectNovelty(const std::vector<double>& currentData, const std::vector<std::vector<double>>& baselineData, const AnalysisConfig& config);
    AnalyticsReport recognizePattern(const std::vector<double>& data, const AnalysisConfig& config);
    AnalyticsReport analyzePredictions(std::shared_ptr<Brain> brain, const AnalysisConfig& config);
    AnalyticsReport assessSystemHealth(std::shared_ptr<Brain> brain, const AnalysisConfig& config);
    
    // Utility methods
    static std::vector<double> computeMovingAverage(const std::vector<double>& data, int windowSize);
    static double computeStandardDeviation(const std::vector<double>& data);
    static double computeMean(const std::vector<double>& data);
    static double computeCorrelationCoefficient(const std::vector<double>& x, const std::vector<double>& y);
    static std::vector<double> normalizeTimeSeries(const std::vector<double>& data);
    static std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    static std::string generateInsight(const std::string& metricName, double currentValue, double baselineValue, double threshold);
    static std::vector<std::string> generateRecommendations(const AnalyticsReport& report);
    
    // Configuration helpers
    static AnalysisConfig createPerformanceAnalysisConfig(const std::string& timeRange = "step");
    static AnalysisConfig createLearningProgressionConfig(int maxSamples = 1000);
    static AnalysisConfig createConnectivityAnalysisConfig();
    static AnalysisConfig createStabilityAssessmentConfig(double confidenceThreshold = 0.95);
};

class NLMAnalytics {
public:
    // High-level analytics interface
    static AnalyticsReport runComprehensiveAnalysis(std::shared_ptr<Brain> brain, 
                                                   const std::string& timeRange = "step",
                                                   const std::map<std::string, double>& customParams = {});
    
    static VisualizationResult createAnalysisVisualization(std::shared_ptr<Brain> brain,
                                                            const std::string& analysisType = "performance",
                                                            const std::string& outputFormat = "png");
    
    static std::string generateAnalysisReport(std::shared_ptr<Brain> brain,
                                              const std::string& reportFormat = "json",
                                              const std::string& includeSections = "all");
    
    static std::map<std::string, double> extractMetrics(std::shared_ptr<Brain> brain,
                                                        const std::vector<std::string>& metrics = {});
    
    static bool exportAnalysisResults(std::shared_ptr<Brain> brain,
                                      const std::string& outputDir = "./analysis_results",
                                      const std::vector<std::string>& formats = {"json", "csv", "png"});
};

PYBIND11_MODULE(pynlm, m) {
    // Visualization module
    py::class_<VisualizationEngine>(m, "VisualizationEngine", R"pbdoc(Visualization engine for NLM neural data)pbdoc")
        .def_static("create", &VisualizationEngine::create)
        .def_static("visualizeNeuralActivity", &VisualizationEngine::visualizeNeuralActivity, 
                   py::arg("brain"), py::arg("config"))
        .def_static("visualizeNetworkTopology", &VisualizationEngine::visualizeNetworkTopology,
                   py::arg("brain"), py::arg("config"))
        .def_static("visualizeSpikeRatios", &VisualizationEngine::visualizeSpikeRatios,
                   py::arg("brain"), py::arg("config"))
        .def_static("visualizeWeightDistribution", &VisualizationEngine::visualizeWeightDistribution,
                   py::arg("brain"), py::arg("config"))
        .def_static("visualizeTemporalDynamics", &VisualizationEngine::visualizeTemporalDynamics,
                   py::arg("timeSeries"), py::arg("config"))
        .def_static("createComparativeAnalysis", &VisualizationEngine::createComparativeAnalysis,
                   py::arg("brains"), py::arg("config"))
        .def_static("createHeatmap", &VisualizationEngine::createHeatmap,
                   py::arg("data"), py::arg("config"))
        .def_static("createDefaultConfig", &VisualizationEngine::createDefaultConfig);

    // Analytics module
    py::class_<AnalyticsEngine>(m, "AnalyticsEngine", R"pbdoc(Analytics engine for NLM neural data)pbdoc")
        .def_static("create", &AnalyticsEngine::create)
        .def_static("analyzePerformance", &AnalyticsEngine::analyzePerformance,
                   py::arg("brain"), py::arg("config"))
        .def_static("analyzeLearningProgression", &AnalyticsEngine::analyzeLearningProgression,
                   py::arg("timeSeries"), py::arg("config"))
        .def_static("analyzeConnectivity", &AnalyticsEngine::analyzeConnectivity,
                   py::arg("brain"), py::arg("config"))
        .def_static("assessStability", &AnalyticsEngine::assessStability,
                   py::arg("metricTimeSeries"), py::arg("config"))
        .def_static("detectNovelty", &AnalyticsEngine::detectNovelty,
                   py::arg("currentData"), py::arg("baselineData"), py::arg("config"))
        .def_static("recognizePattern", &AnalyticsEngine::recognizePattern,
                   py::arg("data"), py::arg("config"))
        .def_static("analyzePredictions", &AnalyticsEngine::analyzePredictions,
                   py::arg("brain"), py::arg("config"))
        .def_static("assessSystemHealth", &AnalyticsEngine::assessSystemHealth,
                   py::arg("brain"), py::arg("config"));

    // High-level analytics interface
    m.def("runComprehensiveAnalysis", &NLMAnalytics::runComprehensiveAnalysis,
          py::arg("brain"), py::arg("timeRange") = "step", 
          py::arg("customParams") = std::map<std::string, double>());

    m.def("createAnalysisVisualization", &NLMAnalytics::createAnalysisVisualization,
          py::arg("brain"), py::arg("analysisType") = "performance", 
          py::arg("outputFormat") = "png");

    m.def("generateAnalysisReport", &NLMAnalytics::generateAnalysisReport,
          py::arg("brain"), py::arg("reportFormat") = "json", 
          py::arg("includeSections") = "all");

    m.def("extractMetrics", &NLMAnalytics::extractMetrics,
          py::arg("brain"), py::arg("metrics") = std::vector<std::string>());

    m.def("exportAnalysisResults", &NLMAnalytics::exportAnalysisResults,
          py::arg("brain"), py::arg("outputDir") = "./analysis_results", 
          py::arg("formats") = std::vector<std::string>({ "json", "csv", "png" }));

    // Data structures
    py::class_<NeuralActivityData>(m, "NeuralActivityData", R"pbdoc(Neural activity data for visualization and analysis)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("label"))
        .def_readwrite("firingRates", &NeuralActivityData::firingRates)
        .def_readwrite("membranePotentials", &NeuralActivityData::membranePotentials)
        .def_readwrite("weights", &NeuralActivityData::weights)
        .def_readwrite("spikeTimes", &NeuralActivityData::spikeTimes)
        .def_readwrite("neuronTypes", &NeuralActivityData::neuronTypes)
        .def_readwrite("timestamp", &NeuralActivityData::timestamp)
        .def_readwrite("label", &NeuralActivityData::label);

    py::class_<NetworkTopology>(m, "NetworkTopology", R"pbdoc(Network topology for visualization)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronIds", &NetworkTopology::neuronIds)
        .def_readwrite("adjacencyMatrix", &NetworkTopology::adjacencyMatrix)
        .def_readwrite("connectionTypes", &NetworkTopology::connectionTypes)
        .def_readwrite("connectionWeights", &NetworkTopology::connectionWeights)
        .def_readwrite("layout", &NetworkTopology::layout)
        .def_readwrite("nodePositions", &NetworkTopology::nodePositions)
        .def_readwrite("nodeAttributes", &NetworkTopology::nodeAttributes);

    py::class_<VisualizationResult>(m, "VisualizationResult", R"pbdoc(Result of visualization operation)pbdoc")
        .def(py::init<>())
        .def_readwrite("type", &VisualizationResult::type)
        .def_readwrite("format", &VisualizationResult::format)
        .def_readwrite("data", &VisualizationResult::data)
        .def_readwrite("filePath", &VisualizationResult::filePath)
        .def_readwrite("title", &VisualizationResult::title)
        .def_readwrite("description", &VisualizationResult::description)
        .def_readwrite("generationTime", &VisualizationResult::generationTime)
        .def_readwrite("dataSize", &VisualizationResult::dataSize)
        .def_readwrite("success", &VisualizationResult::success)
        .def_readwrite("errorMessage", &VisualizationResult::errorMessage);

    py::class_<AnalyticsReport>(m, "AnalyticsReport", R"pbdoc(Result of analytics operation)pbdoc")
        .def(py::init<>())
        .def_readwrite("reportType", &AnalyticsReport::reportType)
        .def_readwrite("timePeriod", &AnalyticsReport::timePeriod)
        .def_readwrite("metrics", &AnalyticsReport::metrics)
        .def_readwrite("insights", &AnalyticsReport::insights)
        .def_readwrite("recommendations", &AnalyticsReport::recommendations)
        .def_readwrite("summary", &AnalyticsReport::summary)
        .def_readwrite("confidence", &AnalyticsReport::confidence);

    // Constants for visualization types
    py::enum_<VisualizationEngine::VisualizationType>(m, "VisualizationType")
        .value("NEURAL_ACTIVITY", VisualizationEngine::VisualizationType::NEURAL_ACTIVITY)
        .value("NETWORK_TOPOLOGY", VisualizationEngine::VisualizationType::NETWORK_TOPOLOGY)
        .value("SPIKE_RATIOS", VisualizationEngine::VisualizationType::SPIKE_RATIOS)
        .value("WEIGHT_DISTRIBUTION", VisualizationEngine::VisualizationType::WEIGHT_DISTRIBUTION)
        .value("SYNAPTIC_CONNECTIVITY", VisualizationEngine::VisualizationType::SYNAPTIC_CONNECTIVITY)
        .value("TEMPORAL_DYNAMICS", VisualizationEngine::VisualizationType::TEMPORAL_DYNAMICS)
        .value("COMPARATIVE_ANALYSIS", VisualizationEngine::VisualizationType::COMPARATIVE_ANALYSIS)
        .value("HEATMAP", VisualizationEngine::VisualizationType::HEATMAP)
        .value("PROGRESSION", VisualizationEngine::VisualizationType::PROGRESSION)
        .value("STATISTICS", VisualizationEngine::VisualizationType::STATISTICS);

    // Constants for analytics types
    py::enum_<AnalyticsEngine::AnalyticsType>(m, "AnalyticsType")
        .value("PERFORMANCE_ANALYSIS", AnalyticsEngine::AnalyticsType::PERFORMANCE_ANALYSIS)
        .value("LEARNING_PROGRESSION", AnalyticsEngine::AnalyticsType::LEARNING_PROGRESSION)
        .value("CONNECTIVITY_ANALYSIS", AnalyticsEngine::AnalyticsType::CONNECTIVITY_ANALYSIS)
        .value("STABILITY_ASSESSMENT", AnalyticsEngine::AnalyticsType::STABILITY_ASSESSMENT)
        .value("NOVELTY_DETECTION", AnalyticsEngine::AnalyticsType::NOVELTY_DETECTION)
        .value("PATTERN_RECOGNITION", AnalyticsEngine::AnalyticsType::PATTERN_RECOGNITION)
        .value("PREDICTION_ANALYSIS", AnalyticsEngine::AnalyticsType::PREDICTION_ANALYSIS)
        .value("SYSTEM_HEALTH", AnalyticsEngine::AnalyticsType::SYSTEM_HEALTH);

    // Constants for output formats
    py::enum_<VisualizationEngine::OutputFormat>(m, "OutputFormat")
        .value("PNG", VisualizationEngine::OutputFormat::PNG)
        .value("SVG", VisualizationEngine::OutputFormat::SVG)
        .value("JSON", VisualizationEngine::OutputFormat::JSON)
        .value("CSV", VisualizationEngine::OutputFormat::CSV)
        .value("HTML", VisualizationEngine::OutputFormat::HTML)
        .value("PDF", VisualizationEngine::OutputFormat::PDF)
        .value("TEXT", VisualizationEngine::OutputFormat::TEXT)
        .value("PYTHON", VisualizationEngine::OutputFormat::PYTHON);

    // Visualization and analytics helper functions
    m.def("createNeuralActivityData", [](const std::string& label) {
        return NeuralActivityData(label);
    }, py::arg("label") = "Default");

    m.def("createNetworkTopology", []() {
        return NetworkTopology();
    });

    m.def("computeMovingAverage", &VisualizationEngine::computeMovingAverage,
          py::arg("data"), py::arg("windowSize"));

    m.def("computeStandardDeviation", &VisualizationEngine::computeStandardDeviation,
          py::arg("data"));

    m.def("computeMean", &VisualizationEngine::computeMean,
          py::arg("data"));

    m.def("computeCorrelationCoefficient", &VisualizationEngine::computeCorrelationCoefficient,
          py::arg("x"), py::arg("y"));

    m.def("normalizeTimeSeries", &VisualizationEngine::normalizeTimeSeries,
          py::arg("data"));

    m.def("linearRegression", &VisualizationEngine::linearRegression,
          py::arg("x"), py::arg("y"));

    m.def("generateInsight", &VisualizationEngine::generateInsight,
          py::arg("metricName"), py::arg("currentValue"), py::arg("baselineValue"), 
          py::arg("threshold"));

    m.def("generateRecommendations", &VisualizationEngine::generateRecommendations,
          py::arg("report"));

    // System capabilities
    m.def("getSystemCapabilities", []() {
        std::map<std::string, std::string> capabilities;
        capabilities["visualization"] = "Neural activity plots, network topology, spike ratios, weight distributions, heatmaps, comparative analysis";
        capabilities["analytics"] = "Performance analysis, learning progression, connectivity analysis, stability assessment, novelty detection, pattern recognition";
        capabilities["advanced_features"] = "Batch processing, performance monitoring, state inspection, advanced plasticity, reward-modulated learning";
        capabilities["export_formats"] = "JSON, CSV, PNG, SVG, HTML, PDF, text reports";
        capabilities["integration"] = "Seamless integration with NLM C++ framework and Python ecosystem";
        return capabilities;
    }, "Get system visualization and analytics capabilities");

    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM neural simulation framework with advanced visualization and analytics capabilities for comprehensive brain research and analysis.";

} // namespace nlm

} // namespace nlm
