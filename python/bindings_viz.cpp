#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;
namespace nlm {

void bindVisualization(pybind11::module_& m) {
    py::class_<VisualizationInterface, std::shared_ptr<VisualizationInterface>>(m, "VisualizationInterface")
        .def(py::init<VisualizationMode>(), py::arg("mode") = VisualizationMode::Text)
        .def("initialize", &VisualizationInterface::initialize, 
             py::arg("title") = "NLM Phase 6 Visualization",
             py::arg("width") = 120, py::arg("height") = 80)
        .def("update", &VisualizationInterface::update,
             py::arg("brain"), py::arg("timestamp"))
        .def("render", &VisualizationInterface::render)
        .def("close", &VisualizationInterface::close)
        .def("isActive", &VisualizationInterface::isActive)
        .def("setMode", &VisualizationInterface::setMode)
        .def("getMode", &VisualizationInterface::getMode)
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate)
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate)
        .def("getCurrentMetrics", &VisualizationInterface::getCurrentMetrics)
        .def("toggleNetworkVisualization", &VisualizationInterface::toggleNetworkVisualization)
        .def("toggleActivityVisualization", &VisualizationInterface::toggleActivityVisualization)
        .def("toggleMemoryVisualization", &VisualizationInterface::toggleMemoryVisualization)
        .def("toggleNeuromodulationVisualization", &VisualizationInterface::toggleNeuromodulationVisualization)
        .def("togglePredictionVisualization", &VisualizationInterface::togglePredictionVisualization)
        .def("toggleDevelopmentVisualization", &VisualizationInterface::toggleDevelopmentVisualization)
        .def("exportData", &VisualizationInterface::exportData, py::arg("filepath"))
        .def("exportMetrics", &VisualizationInterface::exportMetrics, py::arg("filepath"))
        .def_static("getCommandLineHelp", &VisualizationInterface::getCommandLineHelp)
        .def_static("parseCommandLine", &VisualizationInterface::parseCommandLine,
                   py::arg("argc"), py::arg("argv"), py::arg("config"));

    // Bind VisualizationMode enum
    py::enum_<VisualizationMode>(m, "VisualizationMode")
        .value("Text", VisualizationMode::Text)
        .value("TextAdvanced", VisualizationMode::TextAdvanced)
        .value("Graphical", VisualizationMode::Graphical)
        .value("WebSocket", VisualizationMode::WebSocket)
        .value("PythonAPI", VisualizationMode::PythonAPI)
        .value("Compact", VisualizationMode::Compact)
        .export_values();

    // Bind VisualizationMetrics struct
    py::class_<VisualizationInterface::VisualizationMetrics>(m, "VisualizationMetrics")
        .def(py::init<>())
        .def_readwrite("total_reward", &VisualizationInterface::VisualizationMetrics::totalReward)
        .def_readwrite("avg_firing_rate", &VisualizationInterface::VisualizationMetrics::avgFiringRate)
        .def_readwrite("avg_synaptic_weight", &VisualizationInterface::VisualizationMetrics::avgSynapticWeight)
        .def_readwrite("novelty_level", &VisualizationInterface::VisualizationMetrics::noveltyLevel)
        .def_readwrite("curiosity_level", &VisualizationInterface::VisualizationMetrics::curiosityLevel)
        .def_readwrite("dopamine_level", &VisualizationInterface::VisualizationMetrics::dopamineLevel)
        .def_readwrite("working_memory_active_traces", &VisualizationInterface::VisualizationMetrics::workingMemoryActiveTraces)
        .def_readwrite("episodic_memory_episodes", &VisualizationInterface::VisualizationMetrics::episodicMemoryEpisodes)
        .def_readwrite("prediction_confidence", &VisualizationInterface::VisualizationMetrics::predictionConfidence)
        .def_readwrite("development_stage", &VisualizationInterface::VisualizationMetrics::developmentStage)
        .def_readwrite("simulation_time", &VisualizationInterface::VisualizationMetrics::simulationTime)
        .def_readwrite("wall_clock_time", &VisualizationInterface::VisualizationMetrics::wallClockTime)
        .def_readwrite("memory_integrated", &VisualizationInterface::VisualizationMetrics::memoryIntegrated)
        .def_readwrite("episodic_memory_integrated", &VisualizationInterface::VisualizationMetrics::episodicMemoryIntegrated)
        .def_readwrite("neuromodulation_integrated", &VisualizationInterface::VisualizationMetrics::neuromodulationIntegrated)
        .def_readwrite("prediction_integrated", &VisualizationInterface::VisualizationMetrics::predictionIntegrated)
        .def_readwrite("development_integrated", &VisualizationInterface::VisualizationMetrics::developmentIntegrated)
        .def_readwrite("checkpointing_works", &VisualizationInterface::VisualizationMetrics::checkpointingWorks)
        .def_readwrite("replay_works", &VisualizationInterface::VisualizationMetrics::replayWorks);

    // Bind VisualizationConfig struct
    py::class_<VisualizationInterface::VisualizationConfig>(m, "VisualizationConfig")
        .def(py::init<>())
        .def_readwrite("window_title", &VisualizationInterface::VisualizationConfig::windowTitle)
        .def_readwrite("width", &VisualizationInterface::VisualizationConfig::width)
        .def_readwrite("height", &VisualizationInterface::VisualizationConfig::height)
        .def_readwrite("enable_text_visualization", &VisualizationInterface::VisualizationConfig::enableTextVisualization)
        .def_readwrite("enable_graphical_visualization", &VisualizationInterface::VisualizationConfig::enableGraphicalVisualization)
        .def_readwrite("enable_websocket", &VisualizationInterface::VisualizationConfig::enableWebSocket)
        .def_readwrite("websocket_port", &VisualizationInterface::VisualizationConfig::webSocketPort)
        .def_readwrite("websocket_host", &VisualizationInterface::VisualizationConfig::websocketHost)
        .def_readwrite("enable_export", &VisualizationInterface::VisualizationConfig::enableExport)
        .def_readwrite("export_format", &VisualizationInterface::VisualizationConfig::exportFormat)
        .def_readwrite("export_path", &VisualizationInterface::VisualizationConfig::exportPath)
        .def_readwrite("auto_save", &VisualizationInterface::VisualizationConfig::autoSave)
        .def_readwrite("update_interval", &VisualizationInterface::VisualizationConfig::updateInterval)
        .def_readwrite("show_help", &VisualizationInterface::VisualizationConfig::showHelp)
        .def_readwrite("show_performance", &VisualizationInterface::VisualizationConfig::showPerformance)
        .def_readwrite("color_scheme", &VisualizationInterface::VisualizationConfig::colorScheme);
}

PYBIND11_MODULE(pynlm_viz, m) {
    m.doc() = R"pbdoc(
        NLM Visualization Python Bindings
        ---------------------------------
        Python bindings for NLM visualization tools.
        Provides classes for real-time brain visualization, metrics monitoring,
        and interactive visualization control.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind visualization interface
    bindVisualization(m);
}

} // namespace nlm