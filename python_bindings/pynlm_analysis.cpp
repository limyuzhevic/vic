#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"

namespace py = pybind11;

void init_pynlm_analysis(py::module& m) {
    // AnalysisMetricType enum
    py::enum_<AnalysisMetricType>(m, "AnalysisMetricType")
        .value("NEURON_COUNT", AnalysisMetricType::NEURON_COUNT)
        .value("SYNAPSE_COUNT", AnalysisMetricType::SYNAPSE_COUNT)
        .value("FIRING_RATE", AnalysisMetricType::FIRING_RATE)
        .value("EXCITATION_INHIBITION", AnalysisMetricType::EXCITATION_INHIBITION)
        .value("CONNECTION_DENSITY", AnalysisMetricType::CONNECTION_DENSITY)
        .value("ACTIVITY_CORRELATION", AnalysisMetricType::ACTIVITY_CORRELATION)
        .value("REGION_HIERARCHY", AnalysisMetricType::REGION_HIERARCHY)
        .value("FUNCTIONAL_SPECIALIZATION", AnalysisMetricType::FUNCTIONAL_SPECIALIZATION)
        .export_values();

    // RegionAnalysis struct
    py::class_<RegionAnalysis>(m, "RegionAnalysis")
        .def(py::init<>())
        .def_readwrite("region_id", &RegionAnalysis::region_id)
        .def_readwrite("region_name", &RegionAnalysis::region_name)
        .def_readwrite("neuron_count", &RegionAnalysis::neuron_count)
        .def_readwrite("synapse_count", &RegionAnalysis::synapse_count)
        .def_readwrite("average_firing_rate", &RegionAnalysis::average_firing_rate)
        .def_readwrite("excitation_inhibition_ratio", &RegionAnalysis::excitation_inhibition_ratio)
        .def_readwrite("connectivity_score", &RegionAnalysis::connectivity_score)
        .def_readwrite("functional_specialization", &RegionAnalysis::functional_specialization)
        .def_readwrite("activity_patterns", &RegionAnalysis::activity_patterns)
        .def_readwrite("metadata", &RegionAnalysis::metadata);

    // BrainConnectivityGraph struct
    py::class_<BrainConnectivityGraph>(m, "BrainConnectivityGraph")
        .def(py::init<>())
        .def_readwrite("adjacency_matrix", &BrainConnectivityGraph::adjacency_matrix)
        .def_readwrite("node_labels", &BrainConnectivityGraph::node_labels)
        .def_readwrite("edge_weights", &BrainConnectivityGraph::edge_weights)
        .def_readwrite("node_degrees", &BrainConnectivityGraph::node_degrees)
        .def_readwrite("graph_metrics", &BrainConnectivityGraph::graph_metrics)
        .def_readwrite("communities", &BrainConnectivityGraph::communities);

    // ActivityPattern struct
    py::class_<ActivityPattern>(m, "ActivityPattern")
        .def(py::init<>())
        .def_readwrite("step", &ActivityPattern::step)
        .def_readwrite("timestamp", &ActivityPattern::timestamp)
        .def_readwrite("active_neurons", &ActivityPattern::active_neurons)
        .def_readwrite("firing_neuron_count", &ActivityPattern::firing_neuron_count)
        .def_readwrite("average_firing_rate", &ActivityPattern::average_firing_rate)
        .def_readwrite("excitation_inhibition_ratio", &ActivityPattern::excitation_inhibition_ratio)
        .def_readwrite("region_activity", &ActivityPattern::region_activity)
        .def_readwrite("global_activity", &ActivityPattern::global_activity)
        .def_readwrite("metadata", &ActivityPattern::metadata);

    // BrainAnalyzer class
    py::class_<BrainAnalyzer>(m, "BrainAnalyzer")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("analyze_structure", &BrainAnalyzer::analyze_structure)
        .def("analyze_functional", &BrainAnalyzer::analyze_functional)
        .def("analyze_activity_patterns", &BrainAnalyzer::analyze_activity_patterns,
             py::arg("steps") = 1000, py::arg("window_size") = 100)
        .def("analyze_connectivity", &BrainAnalyzer::analyze_connectivity)
        .def("generate_visualization_data", &BrainAnalyzer::generate_visualization_data,
             py::arg("analysis_type") = "structure",
             py::arg("output_format") = "json")
        .def("compare_brains", &BrainAnalyzer::compare_brains)
        .def("detect_anomalies", &BrainAnalyzer::detect_anomalies)
        .def("get_insights", &BrainAnalyzer::get_insights)
        .def("export_results", &BrainAnalyzer::export_results,
             py::arg("filepath"), py::arg("format") = "json",
             py::arg("analysis_types") = py::list());

    // High-level API functions
    m.def("analyze_brain", &analyze_brain,
          py::arg("brain"), py::arg("analysis_types") = py::list());
    m.def("generate_visualization_data", &generate_visualization_data,
          py::arg("brain"), py::arg("analysis_type") = "structure",
          py::arg("output_format") = "json");
    m.def("compare_brain_analyses", &compare_brain_analyses,
          py::arg("brain1"), py::arg("brain2"));
}

PYBIND11_MODULE(pynlm_analysis, m) {
    init_pynlm_analysis(m);
}