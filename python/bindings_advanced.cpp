#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/analysis/AdvancedAnalysisTools.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Analysis Tools Python Bindings
        --------------------------------------------
        
        This module provides Python bindings for NLM's advanced analysis capabilities.
        Includes network topology analysis, neural pattern analysis, statistical analysis,
        clustering algorithms, and data export/import functionality for power users.
        
        Key Features:
        - Network topology analysis (degree distribution, clustering coefficients)
        - Neural pattern analysis and classification
        - Statistical analysis (mean, median, variance, skew, kurtosis)
        - Activity burst detection
        - Population correlation analysis
        - Time-frequency analysis
        - Outlier detection
        - Dimensionality reduction (PCA)
        - Granger causality analysis
        - Advanced configuration export/import
        
        All analysis tools are designed to be optional and non-invasive to
        the core NLM functionality.
    )pbdoc";

    // Configure Python module
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumeration bindings
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Configuration class
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // AdvancedAnalysisTools class
    py::class_<nlm::AdvancedAnalysisTools>(m, "AdvancedAnalysisTools", R"pbdoc(Advanced analysis tools for neural network analysis)pbdoc")
        .def_static("cluster_neural_activity", &nlm::AdvancedAnalysisTools::clusterNeuronsByActivity,
                   py::arg("brain"), py::arg("eps") = 2.0, py::arg("min_samples") = 5,
                   "Cluster neurons based on their activity patterns using DBSCAN-like algorithm")
        .def_static("analyze_network_topology", &nlm::AdvancedAnalysisTools::analyzeNetworkTopology,
                   py::arg("brain"),
                   "Analyze network topology and calculate metrics (degree, clustering, etc.)")
        .def_static("analyze_neural_patterns", &nlm::AdvancedAnalysisTools::analyzeNeuralPatterns,
                   py::arg("brain"), py::arg("region_id"),
                   "Analyze neural activity patterns in a specific region")
        .def_static("compute_neural_statistics", &nlm::AdvancedAnalysisTools::computeNeuralStatistics,
                   py::arg("data"),
                   "Compute statistical measures on neural activity data")
        .def_static("detect_activity_bursts", &nlm::AdvancedAnalysisTools::detectActivityBursts,
                   py::arg("data"), py::arg("threshold") = 2.0f,
                   "Detect activity bursts in neural time series data")
        .def_static("compute_population_correlation", &nlm::AdvancedAnalysisTools::computePopulationCorrelation,
                   py::arg("brain"), py::arg("population_ids"),
                   "Compute correlation matrix between neural populations")
        .def_static("compute_time_frequency", &nlm::AdvancedAnalysisTools::computeTimeFrequency,
                   py::arg("brain"), py::arg("region_id"), py::arg("sampling_rate") = 1000.0f,
                   "Generate time-frequency representation of neural activity")
        .def_static("calculate_network_efficiency", &nlm::AdvancedAnalysisTools::calculateNetworkEfficiency,
                   py::arg("brain"),
                   "Calculate network efficiency metrics")
        .def_static("perform_pca", &nlm::AdvancedAnalysisTools::performPCA,
                   py::arg("data"), py::arg("target_dim") = 2,
                   "Perform principal component analysis (dimensionality reduction)")
        .def_static("detect_outliers", &nlm::AdvancedAnalysisTools::detectOutliers,
                   py::arg("data"), py::arg("method") = std::string("zscore"), py::arg("threshold") = 2.0f,
                   "Detect outliers in neural activity data")
        .def_static("compute_granger_causality", &nlm::AdvancedAnalysisTools::computeGrangerCausality,
                   py::arg("signal1"), py::arg("signal2"), py::arg("max_lag") = 10,
                   "Compute Granger causality between neural signals")
        .def_static("generate_activity_report", &nlm::AdvancedAnalysisTools::generateActivityReport,
                   py::arg("brain"),
                   "Generate comprehensive neural activity report")
        .def_static("export_results", &nlm::AdvancedAnalysisTools::exportResults,
                   py::arg("results"), py::arg("filepath"), py::arg("format") = std::string("text"),
                   "Export analysis results to file");

    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional utility functions
    m.def("save_analysis_results", [](const std::string& results, const std::string& filepath, const std::string& format = "text") {
        return nlm::AdvancedAnalysisTools::exportResults(results, filepath, format);
    }, py::arg("results"), py::arg("filepath"), py::arg("format") = std::string("text"),
       "Save analysis results to file (convenience function)");

    m.def("create_sample_neural_data", [](size_t size) {
        std::vector<float> data(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dist(0.0, 1.0);
        for (size_t i = 0; i < size; ++i) {
            data[i] = dist(gen);
        }
        return data;
    }, py::arg("size"), "Generate sample neural data for testing");

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM Advanced Analysis Tools Python Bindings";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
}
