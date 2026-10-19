#include "VisualizationSystem.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace nlm {
namespace python {

// Python binding for RealTimeMonitor
void bindRealTimeMonitor(py::module& m) {
    py::class_<RealTimeMonitor, std::shared_ptr<RealTimeMonitor>>(m, "RealTimeMonitor")
        .def(py::init<>())
        .def("initialize", &RealTimeMonitor::initialize, py::arg("config"))
        .def("shutdown", &RealTimeMonitor::shutdown)
        .def("startMonitoring", &RealTimeMonitor::startMonitoring, py::arg("updateIntervalHz") = 30.0)
        .def("stopMonitoring", &RealTimeMonitor::stopMonitoring)
        .def("isMonitoring", &RealTimeMonitor::isMonitoring)
        .def("updateMetrics", &RealTimeMonitor::updateMetrics)
        .def("recordEvent", &RealTimeMonitor::recordEvent, py::arg("eventType"), py::arg("details") = "")
        .def("addCustomMetric", &RealTimeMonitor::addCustomMetric)
        .def("setMetricsCallback", &RealTimeMonitor::setMetricsCallback)
        .def("setAlertCallback", &RealTimeMonitor::setAlertCallback)
        .def("triggerAlert", &RealTimeMonitor::triggerAlert, 
             py::arg("message"), py::arg("severity") = 1.0f, py::arg("category") = "General")
        .def("getLatestMetrics", &RealTimeMonitor::getLatestMetrics)
        .def("getMetricsHistory", &RealTimeMonitor::getMetricsHistory, py::arg("maxCount") = 1000)
        .def("getActiveAlerts", &RealTimeMonitor::getActiveAlerts)
        .def("getMetricMean", &RealTimeMonitor::getMetricMean)
        .def("getMetricStdDev", &RealTimeMonitor::getMetricStdDev)
        .def("getMetricMax", &RealTimeMonitor::getMetricMax)
        .def("getMetricMin", &RealTimeMonitor::getMetricMin)
        .def("getRecentEvents", &RealTimeMonitor::getRecentEvents, py::arg("maxCount") = 100)
        .def("enablePerformanceMonitoring", &RealTimeMonitor::enablePerformanceMonitoring)
        .def("setPerformanceThresholds", &RealTimeMonitor::setPerformanceThresholds)
        .def("setUpdateInterval", &RealTimeMonitor::setUpdateInterval)
        .def("getUpdateInterval", &RealTimeMonitor::getUpdateInterval)
        .def("exportMetrics", &RealTimeMonitor::exportMetrics, 
             py::arg("filepath"), py::arg("format") = "csv")
        .def("exportEvents", &RealTimeMonitor::exportEvents,
             py::arg("filepath"), py::arg("format") = "json")
        
        // Pythonic properties
        .def_property_readonly("avg_firing_rate", &RealTimeMonitor::getMetricMean)
        .def_property_readonly("total_spike_count", [] (const RealTimeMonitor& self) {
            auto metrics = self.getLatestMetrics();
            return metrics.totalSpikeCount;
        })
        .def_property_readonly("working_memory_load", [] (const RealTimeMonitor& self) {
            auto metrics = self.getLatestMetrics();
            return metrics.workingMemoryLoad;
        })
        .def_property_readonly("dopamine_level", [] (const RealTimeMonitor& self) {
            auto metrics = self.getLatestMetrics();
            return metrics.dopamineLevel;
        });
}

// Python binding for VisualizationSystem
void bindVisualizationSystem(py::module& m) {
    py::enum_<VisualizationSystem::VisualizationType>(m, "VisualizationType")
        .value("NETWORK", VisualizationSystem::VisualizationType::NETWORK)
        .value("ACTIVITY", VisualizationSystem::VisualizationType::ACTIVITY)
        .value("MEMORY", VisualizationSystem::VisualizationType::MEMORY)
        .value("NEUROMODULATION", VisualizationSystem::VisualizationType::NEUROMODULATION)
        .value("PLASTICITY", VisualizationSystem::VisualizationType::PLASTICITY)
        .value("DEVELOPMENT", VisualizationSystem::VisualizationType::DEVELOPMENT)
        .value("PREDICTION", VisualizationSystem::VisualizationType::PREDICTION)
        .value("PLANNING", VisualizationSystem::VisualizationType::PLANNING)
        .value("SOCIAL_LEARNING", VisualizationSystem::VisualizationType::SOCIAL_LEARNING)
        .value("CONTINUAL_LEARNING", VisualizationSystem::VisualizationType::CONTINUAL_LEARNING)
        .value("PERFORMANCE", VisualizationSystem::VisualizationType::PERFORMANCE)
        .value("CUSTOM", VisualizationSystem::VisualizationType::CUSTOM)
        .export_values();
    
    py::enum_<VisualizationSystem::BackendType>(m, "BackendType")
        .value("OPENGL", VisualizationSystem::BackendType::OPENGL)
        .value("VULKAN", VisualizationSystem::BackendType::VULKAN)
        .value("SOFTWARE_RENDERER", VisualizationSystem::BackendType::SOFTWARE_RENDERER)
        .value("PYTHON_PLOTTING", VisualizationSystem::BackendType::PYTHON_PLOTTING)
        .value("WEBGL", VisualizationSystem::BackendType::WEBGL)
        .value("SVG", VisualizationSystem::BackendType::SVG)
        .value("CUSTOM", VisualizationSystem::BackendType::CUSTOM)
        .export_values();
    
    py::class_<VisualizationSystem, std::shared_ptr<VisualizationSystem>>(m, "VisualizationSystem")
        .def(py::init<>())
        .def("initialize", &VisualizationSystem::initialize, 
             py::arg("config"), py::arg("backend") = VisualizationSystem::BackendType::SOFTWARE_RENDERER)
        .def("shutdown", &VisualizationSystem::shutdown)
        .def("startVisualization", &VisualizationSystem::startVisualization, py::arg("fps") = 60.0)
        .def("stopVisualization", &VisualizationSystem::stopVisualization)
        .def("isRunning", &VisualizationSystem::isRunning)
        .def("setBrainData", &VisualizationSystem::setBrainData)
        .def("setMonitor", &VisualizationSystem::setMonitor)
        .def("addVisualization", &VisualizationSystem::addVisualization, 
             py::arg("type"), py::arg("name") = "")
        .def("removeVisualization", &VisualizationSystem::removeVisualization)
        .def("updateVisualization", &VisualizationSystem::updateVisualization)
        .def("setBackend", &VisualizationSystem::setBackend)
        .def("getBackendType", &VisualizationSystem::getBackendType)
        .def("getAvailableBackends", &VisualizationSystem::getAvailableBackends)
        .def("setCameraPosition", &VisualizationSystem::setCameraPosition)
        .def("setCameraTarget", &VisualizationSystem::setCameraTarget)
        .def("setZoom", &VisualizationSystem::setZoom)
        .def("rotateCamera", &VisualizationSystem::rotateCamera)
        .def("exportVisualization", &VisualizationSystem::exportVisualization, 
             py::arg("filepath"), py::arg("format") = "png")
        .def("exportAnimation", &VisualizationSystem::exportAnimation,
             py::arg("filepath"), py::arg("duration"), py::arg("fps") = 30.0)
        .def("setVisualizationScale", &VisualizationSystem::setVisualizationScale)
        .def("setUpdateFrequency", &VisualizationSystem::setUpdateFrequency)
        .def("enableAntialiasing", &VisualizationSystem::enableAntialiasing)
        .def("setColorScheme", &VisualizationSystem::setColorScheme)
        .def("setVisualizationReadyCallback", &VisualizationSystem::setVisualizationReadyCallback)
        .def("getTotalNeuronCount", &VisualizationSystem::getTotalNeuronCount)
        .def("getRenderingFPS", &VisualizationSystem::getRenderingFPS)
        .def("getMemoryUsage", &VisualizationSystem::getMemoryUsage)
        
        // Pythonic properties
        .def_property_readonly("total_neurons", &VisualizationSystem::getTotalNeuronCount)
        .def_property_readonly("rendering_fps", &VisualizationSystem::getRenderingFPS);
}

// Python binding for NeuralNetworkVisualizer
void bindNeuralNetworkVisualizer(py::module& m) {
    py::class_<NeuralNetworkVisualizer, std::shared_ptr<NeuralNetworkVisualizer>>(m, "NeuralNetworkVisualizer")
        .def(py::init<>())
        .def("setLayout", &NeuralNetworkVisualizer::setLayout)
        .def("setMinDistance", &NeuralNetworkVisualizer::setMinDistance)
        .def("setMaxDistance", &NeuralNetworkVisualizer::setMaxDistance)
        .def("setShowSynapticWeights", &NeuralNetworkVisualizer::setShowSynapticWeights)
        .def("setShowConnectionStrength", &NeuralNetworkVisualizer::setShowConnectionStrength)
        .def("setColorByNeuronType", &NeuralNetworkVisualizer::setColorByNeuronType)
        .def("setColorByFiringRate", &NeuralNetworkVisualizer::setColorByFiringRate)
        .def("setShowSpines", &NeuralNetworkVisualizer::setShowSpines)
        .def("updateNeurons", &NeuralNetworkVisualizer::updateNeurons)
        .def("updateSynapses", &NeuralNetworkVisualizer::updateSynapses)
        .def("updateNetworkTopology", &NeuralNetworkVisualizer::updateNetworkTopology)
        .def("calculateNetworkDensity", &NeuralNetworkVisualizer::calculateNetworkDensity)
        .def("calculateClusteringCoefficient", &NeuralNetworkVisualizer::calculateClusteringCoefficient)
        .def("calculateModularity", &NeuralNetworkVisualizer::calculateModularity)
        .def("exportLayout", &NeuralNetworkVisualizer::exportLayout)
        .def("saveNetworkGraph", &NeuralNetworkVisualizer::saveNetworkGraph);
}

// Python binding for MemorySystemVisualizer
void bindMemorySystemVisualizer(py::module& m) {
    py::class_<MemorySystemVisualizer, std::shared_ptr<MemorySystemVisualizer>>(m, "MemorySystemVisualizer")
        .def(py::init<>())
        .def("setShowWorkingMemoryTrails", &MemorySystemVisualizer::setShowWorkingMemoryTrails)
        .def("setShowEpisodicReplay", &MemorySystemVisualizer::setShowEpisodicReplay)
        .def("setShowMemoryTimeSeries", &MemorySystemVisualizer::setShowMemoryTimeSeries)
        .def("setMemoryTimeWindow", &MemorySystemVisualizer::setMemoryTimeWindow)
        .def("setReplayAnimationSpeed", &MemorySystemVisualizer::setReplayAnimationSpeed)
        .def("updateWorkingMemory", &MemorySystemVisualizer::updateWorkingMemory)
        .def("updateEpisodicMemory", &MemorySystemVisualizer::updateEpisodicMemory)
        .def("updateMemoryTimeSeries", &MemorySystemVisualizer::updateMemoryTimeSeries)
        .def("calculateMemoryCapacityUtilization", &MemorySystemVisualizer::calculateMemoryCapacityUtilization)
        .def("calculateMemoryConsolidationRate", &MemorySystemVisualizer::calculateMemoryConsolidationRate)
        .def("calculateReplayEfficiency", &MemorySystemVisualizer::calculateReplayEfficiency)
        .def("exportMemoryGraph", &MemorySystemVisualizer::exportMemoryGraph)
        .def("exportTimeSeries", &MemorySystemVisualizer::exportTimeSeries);
}

// Python binding for NeuromodulationVisualizer
void bindNeuromodulationVisualizer(py::module& m) {
    py::class_<NeuromodulationVisualizer, std::shared_ptr<NeuromodulationVisualizer>>(m, "NeuromodulationVisualizer")
        .def(py::init<>())
        .def("setShowModulationFields", &NeuromodulationVisualizer::setShowModulationFields)
        .def("setShowModulationArrows", &NeuromodulationVisualizer::setShowModulationArrows)
        .def("setShowEffectZones", &NeuromodulationVisualizer::setShowEffectZones)
        .def("setTimeScale", &NeuromodulationVisualizer::setTimeScale)
        .def("updateNeuromodulators", &NeuromodulationVisualizer::updateNeuromodulators)
        .def("updateModulationEffects", &NeuromodulationVisualizer::updateModulationEffects)
        .def("calculateModulationBalance", &NeuromodulationVisualizer::calculateModulationBalance)
        .def("calculateNoveltySurprise", &NeuromodulationVisualizer::calculateNoveltySurprise)
        .def("detectModulationPatterns", &NeuromodulationVisualizer::detectModulationPatterns)
        .def("exportModulationData", &NeuromodulationVisualizer::exportModulationData)
        .def("exportSignalTimeSeries", &NeuromodulationVisualizer::exportSignalTimeSeries);
}

// Python binding for PlasticityVisualizer
void bindPlasticityVisualizer(py::module& m) {
    py::class_<PlasticityVisualizer, std::shared_ptr<PlasticityVisualizer>>(m, "PlasticityVisualizer")
        .def(py::init<>())
        .def("setShowSTDPTraces", &PlasticityVisualizer::setShowSTDPTraces)
        .def("setShowHebbianCurves", &PlasticityVisualizer::setShowHebbianCurves)
        .def("setShowPlasticityRules", &PlasticityVisualizer::setShowPlasticityRules)
        .def("setLearningCurveTimeWindow", &PlasticityVisualizer::setLearningCurveTimeWindow)
        .def("setTraceDecayVisualization", &PlasticityVisualizer::setTraceDecayVisualization)
        .def("updateSTDPTraces", &PlasticityVisualizer::updateSTDPTraces)
        .def("updateHebbianCurves", &PlasticityVisualizer::updateHebbianCurves)
        .def("updatePlasticityRules", &PlasticityVisualizer::updatePlasticityRules)
        .def("calculatePlasticityBalance", &PlasticityVisualizer::calculatePlasticityBalance)
        .def("calculateLearningEfficiency", &PlasticityVisualizer::calculateLearningEfficiency)
        .def("calculateAdaptationSpeed", &PlasticityVisualizer::calculateAdaptationSpeed)
        .def("detectPlasticityPatterns", &PlasticityVisualizer::detectPlasticityPatterns)
        .def("exportPlasticityData", &PlasticityVisualizer::exportPlasticityData)
        .def("exportLearningHistory", &PlasticityVisualizer::exportLearningHistory);
}

// Python binding for DevelopmentVisualizer
void bindDevelopmentVisualizer(py::module& m) {
    py::class_<DevelopmentVisualizer, std::shared_ptr<DevelopmentVisualizer>>(m, "DevelopmentVisualizer")
        .def(py::init<>())
        .def("setShowDevelopmentalStages", &DevelopmentVisualizer::setShowDevelopmentalStages)
        .def("setShowMaturationCurves", &DevelopmentVisualizer::setShowMaturationCurves)
        .def("setDevelopmentTimeScale", &DevelopmentVisualizer::setDevelopmentTimeScale)
        .def("setMaturationAnimationSpeed", &DevelopmentVisualizer::setMaturationAnimationSpeed)
        .def("updateDevelopmentalStages", &DevelopmentVisualizer::updateDevelopmentalStages)
        .def("updateMaturationCurves", &DevelopmentVisualizer::updateMaturationCurves)
        .def("calculateDevelopmentalProgress", &DevelopmentVisualizer::calculateDevelopmentalProgress)
        .def("calculateMaturationLevel", &DevelopmentVisualizer::calculateMaturationLevel)
        .def("detectDevelopmentalPatterns", &DevelopmentVisualizer::detectDevelopmentalPatterns)
        .def("exportDevelopmentData", &DevelopmentVisualizer::exportDevelopmentData)
        .def("exportMaturationHistory", &DevelopmentVisualizer::exportMaturationHistory);
}

// Python binding for PredictionVisualizer
void bindPredictionVisualizer(py::module& m) {
    py::class_<PredictionVisualizer, std::shared_ptr<PredictionVisualizer>>(m, "PredictionVisualizer")
        .def(py::init<>())
        .def("setShowPredictionErrors", &PredictionVisualizer::setShowPredictionErrors)
        .def("setShowConfidenceIntervals", &PredictionVisualizer::setShowConfidenceIntervals)
        .def("setShowPlanningTrees", &PredictionVisualizer::setShowPlanningTrees)
        .def("setPlanningTreeDepth", &PredictionVisualizer::setPlanningTreeDepth)
        .def("setErrorTimeWindow", &PredictionVisualizer::setErrorTimeWindow)
        .def("updatePredictionErrors", &PredictionVisualizer::updatePredictionErrors)
        .def("updateConfidenceIntervals", &PredictionVisualizer::updateConfidenceIntervals)
        .def("updatePlanningTrees", &PredictionVisualizer::updatePlanningTrees)
        .def("calculatePredictionAccuracy", &PredictionVisualizer::calculatePredictionAccuracy)
        .def("calculatePredictionUncertainty", &PredictionVisualizer::calculatePredictionUncertainty)
        .def("calculatePlanningEfficiency", &PredictionVisualizer::calculatePlanningEfficiency)
        .def("detectPredictionPatterns", &PredictionVisualizer::detectPredictionPatterns)
        .def("exportPredictionData", &PredictionVisualizer::exportPredictionData)
        .def("exportPlanningHistory", &PredictionVisualizer::exportPlanningHistory);
}

namespace utils {

/**
 * @brief Create a real-time plotting dashboard
 */
py::dict createDashboard(std::shared_ptr<RealTimeMonitor> monitor,
                         const std::vector<std::string>& metrics,
                         double updateInterval) {
    py::dict dashboard;
    
    // Create dashboard components
    dashboard["monitor"] = monitor;
    dashboard["metrics"] = metrics;
    dashboard["update_interval"] = updateInterval;
    dashboard["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return dashboard;
}

/**
 * @brief Export visualization data to Python formats
 */
py::dict exportToPythonFormat(std::shared_ptr<VisualizationSystem> visualizer,
                              const std::string& format) {
    py::dict exportedData;
    
    // Export visualization data
    exportedData["format"] = format;
    exportedData["neuron_count"] = visualizer->getTotalNeuronCount();
    exportedData["fps"] = visualizer->getRenderingFPS();
    exportedData["memory_usage"] = visualizer->getMemoryUsage();
    
    return exportedData;
}

/**
 * @brief Create interactive visualization with matplotlib
 */
py::object createMatplotlibVisualization(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<RealTimeMonitor> monitor,
                                         const py::dict& options) {
    // Create matplotlib visualization
    // This would create interactive plots for neural activity
    return py::none();
}

/**
 * @brief Create web-based visualization interface
 */
std::string createWebInterface(std::shared_ptr<VisualizationSystem> visualizer,
                               const std::string& host,
                               int port) {
    // Create web-based visualization interface
    // This would start a web server with visualization endpoints
    return "http://" + host + ":" + std::to_string(port) + "/visualization";
}

/**
 * @brief Performance profiling for visualization
 */
py::dict profileVisualization(std::shared_ptr<VisualizationSystem> visualizer,
                              double duration,
                              const py::dict& options) {
    py::dict profileResults;
    
    // Profile visualization performance
    profileResults["duration"] = duration;
    profileResults["neuron_count"] = visualizer->getTotalNeuronCount();
    profileResults["avg_fps"] = visualizer->getRenderingFPS();
    profileResults["memory_usage"] = visualizer->getMemoryUsage();
    
    return profileResults;
}

} // namespace utils

} // namespace python
} // namespace nlm

// Module initialization
PYBIND11_MODULE(pynlm_viz, m) {
    m.doc() = R"pbdoc(
        NLM Visualization and Monitoring Module
        ==========================================
        
        This module provides comprehensive visualization and monitoring capabilities
        for the NLM (Neural Learning Machine) framework. It includes:
        
        - Real-time brain state monitoring
        - Advanced neural network visualization
        - Memory system visualization
        - Neuromodulation visualization
        - Plasticity visualization
        - Development visualization
        - Prediction system visualization
        - Planning visualization
        - Performance monitoring
        - Python integration APIs
        - Web-based visualization interfaces
        
        Key features:
        - Real-time data visualization
        - Multi-backend support (OpenGL, Vulkan, Python plotting, WebGL, etc.)
        - Cross-platform compatibility
        - Performance-optimized rendering
        - Comprehensive monitoring capabilities
        - Python-friendly API design
        
        Usage:
            import pynlm_viz
            
            # Create visualization system
            viz = pynlm_viz.VisualizationSystem()
            
            # Create monitor
            monitor = pynlm_viz.RealTimeMonitor()
            
            # Create specific visualizers
            network_viz = pynlm_viz.NeuralNetworkVisualizer()
            memory_viz = pynlm_viz.MemorySystemVisualizer()
            neuromod_viz = pynlm_viz.NeuromodulationVisualizer()
            
    )pbdoc";
    
    // Bind visualization components
    nlm::python::bindRealTimeMonitor(m);
    nlm::python::bindVisualizationSystem(m);
    nlm::python::bindNeuralNetworkVisualizer(m);
    nlm::python::bindMemorySystemVisualizer(m);
    nlm::python::bindNeuromodulationVisualizer(m);
    nlm::python::bindPlasticityVisualizer(m);
    nlm::python::bindDevelopmentVisualizer(m);
    nlm::python::bindPredictionVisualizer(m);
    
    // Bind utility functions
    m.def("create_dashboard", &nlm::python::utils::createDashboard,
          py::arg("monitor"), py::arg("metrics"), py::arg("updateInterval") = 1.0);
    
    m.def("export_to_python_format", &nlm::python::utils::exportToPythonFormat,
          py::arg("visualizer"), py::arg("format") = "numpy");
    
    m.def("create_matplotlib_visualization", 
          &nlm::python::utils::createMatplotlibVisualization,
          py::arg("brain"), py::arg("monitor"), py::arg("options") = py::dict());
    
    m.def("create_web_interface", &nlm::python::utils::createWebInterface,
          py::arg("visualizer"), py::arg("host") = "localhost", py::arg("port") = 8080);
    
    m.def("profile_visualization", &nlm::python::utils::profileVisualization,
          py::arg("visualizer"), py::arg("duration"), py::arg("options") = py::dict());
    
    // Constants
    m.attr("VISUALIZATION_TYPES") = py::cast(
        std::vector<std::string>{
            "NETWORK", "ACTIVITY", "MEMORY", "NEUROMODULATION",
            "PLASTICITY", "DEVELOPMENT", "PREDICTION", "PLANNING",
            "SOCIAL_LEARNING", "CONTINUAL_LEARNING", "PERFORMANCE", "CUSTOM"
        });
    
    m.attr("BACKEND_TYPES") = py::cast(
        std::vector<std::string>{
            "OPENGL", "VULKAN", "SOFTWARE_RENDERER", "PYTHON_PLOTTING",
            "WEBGL", "SVG", "CUSTOM"
        });
}
