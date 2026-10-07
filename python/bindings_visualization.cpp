// Visualization system bindings

#include "bindings.h"

namespace nlm {

void bind_visualization_module(py::module_& m) {
    // VisualizationInterface class
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(
        Visualization interface for NLM simulation output.
        
        VisualizationInterface provides:
        - Neural activity visualization
        - Brain state monitoring
        - Performance metrics display
        - Simulation debugging
        
        Key features:
        - Real-time visualization
        - Multiple visualization modes
        - Data export capabilities
        - Performance monitoring
    )pbdoc")
        .def(py::init<>())
        .def("update", &VisualizationInterface::update,
             py::arg("brain"), py::arg("world"), "Update visualization with current state")
        .def("render", &VisualizationInterface::render, "Render visualization")
        .def("show", &VisualizationInterface::show, "Show visualization window")
        .def("hide", &VisualizationInterface::hide, "Hide visualization window")
        .def("saveFrame", &VisualizationInterface::saveFrame, py::arg("filename"), "Save current frame")
        .def("setSize", &VisualizationInterface::setSize, py::arg("width"), py::arg("height"),
             "Set visualization window size")
        .def("setTitle", &VisualizationInterface::setTitle, py::arg("title"), "Set visualization title")
        .def("isVisible", &VisualizationInterface::isVisible, "Check if visualization is visible")
        .def("getPerformanceStats", &VisualizationInterface::getPerformanceStats,
             py::return_value_policy::reference_internal, "Get performance statistics")
        .def("reset", &VisualizationInterface::reset, "Reset visualization");

    // Metrics class
    py::class_<Metrics>(m, "Metrics", R"pbdoc(
        Performance metrics collection and analysis.
        
        Metrics provides:
        - Simulation performance tracking
        - Brain system statistics
        - Resource usage monitoring
        - Progress tracking
        
        Key features:
        - Real-time metrics collection
        - Statistical analysis
        - Performance optimization
        - Progress monitoring
    )pbdoc")
        .def(py::init<>())
        .def("recordMetric", &Metrics::recordMetric,
             py::arg("name"), py::arg("value"), py::arg("timestamp") = 0.0,
             "Record a performance metric")
        .def("getMetric", &Metrics::getMetric, py::arg("name"),
             py::return_value_policy::reference_internal, "Get metric value")
        .def("getMetrics", &Metrics::getMetrics, py::return_value_policy::reference_internal,
             "Get all metrics")
        .def("getAverage", &Metrics::getAverage, py::arg("name"), "Get average metric value")
        .def("getMax", &Metrics::getMax, py::arg("name"), "Get maximum metric value")
        .def("getMin", &Metrics::getMin, py::arg("name"), "Get minimum metric value")
        .def("getStandardDeviation", &Metrics::getStandardDeviation, py::arg("name"),
             "Get metric standard deviation")
        .def("clear", &Metrics::clear, "Clear all metrics")
        .def("exportToCSV", &Metrics::exportToCSV, py::arg("filename"), "Export metrics to CSV file");
}

} // namespace nlm