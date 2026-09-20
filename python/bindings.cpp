#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/visualization/EnhancedVisualization.hpp"
#include "../src/debugging/DebugTools.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_viz, m) {
    m.doc() = R"pbdoc(
        NLM Visualization Python Bindings
        ---------------------------------
        Python bindings for NLM visualization and debugging functionality.
        Provides enhanced visualization tools, debugging capabilities, and
        performance monitoring with real-time graphics support.
    )pbdoc";

    // Enhanced Visualization bindings
    py::class_<nlm::EnhancedVisualization>(m, "EnhancedVisualization")
        .def(py::init<>())
        .def("initialize", &nlm::EnhancedVisualization::initialize,
             py::arg("width") = 1600, py::arg("height") = 900,
             py::arg("title") = "NLM Neural Network Visualization")
        .def("update", &nlm::EnhancedVisualization::update)
        .def("render", &nlm::EnhancedVisualization::render)
        .def("close", &nlm::EnhancedVisualization::close)
        .def("isActive", &nlm::EnhancedVisualization::isActive)
        .def("setUpdateRate", &nlm::EnhancedVisualization::setUpdateRate)
        .def("setSimulationSpeed", &nlm::EnhancedVisualization::setSimulationSpeed)
        .def("togglePause", &nlm::EnhancedVisualization::togglePause)
        .def("isPaused", &nlm::EnhancedVisualization::isPaused)
        .def("toggleDebugMode", &nlm::EnhancedVisualization::toggleDebugMode)
        .def("toggleGrid", &nlm::EnhancedVisualization::toggleGrid)
        .def("toggleLabels", &nlm::EnhancedVisualization::toggleLabels)
        .def("setColorScheme", &nlm::EnhancedVisualization::setColorScheme)
        .def("exportVisualizationData", &nlm::EnhancedVisualization::exportVisualizationData)
        .def("handleInput", &nlm::EnhancedVisualization::handleInput)
        .def("setSimulationSpeed", &nlm::EnhancedVisualization::setSimulationSpeed)
        .def("inspectNeuralState", &nlm::EnhancedVisualization::inspectNeuralState)
        .def("updateMemoryUsage", &nlm::EnhancedVisualization::updateMemoryUsage)
        .def("updatePerformanceMetrics", &nlm::EnhancedVisualization::updatePerformanceMetrics)
        .def("addEvent", &nlm::EnhancedVisualization::addEvent)
        .def("clearEvents", &nlm::EnhancedVisualization::clearEvents)
        .def("resetMetrics", &nlm::EnhancedVisualization::resetMetrics);

    // Debug Tools bindings
    py::class_<nlm::DebugTools>(m, "DebugTools")
        .def(py::init<>())
        .def("initialize", &nlm::DebugTools::initialize)
        .def("startDebugging", &nlm::DebugTools::startDebugging)
        .def("stopDebugging", &nlm::DebugTools::stopDebugging)
        .def("isDebugging", &nlm::DebugTools::isDebugging)
        .def("inspectBrain", &nlm::DebugTools::inspectBrain)
        .def("displayNeuronInfo", &nlm::DebugTools::displayNeuronInfo)
        .def("displaySynapseInfo", &nlm::DebugTools::displaySynapseInfo)
        .def("displayNetworkStats", &nlm::DebugTools::displayNetworkStats)
        .def("startMemoryMonitoring", &nlm::DebugTools::startMemoryMonitoring)
        .def("stopMemoryMonitoring", &nlm::DebugTools::stopMemoryMonitoring)
        .def("checkMemoryLeaks", &nlm::DebugTools::checkMemoryLeaks)
        .def("displayMemoryReport", &nlm::DebugTools::displayMemoryReport)
        .def("startProfiling", &nlm::DebugTools::startProfiling)
        .def("stopProfiling", &nlm::DebugTools::stopProfiling)
        .def("addProfilePoint", &nlm::DebugTools::addProfilePoint)
        .def("displayProfileReport", &nlm::DebugTools::displayProfileReport)
        .def("addSpikeDebugInfo", &nlm::DebugTools::addSpikeDebugInfo)
        .def("displaySpikeAnalysis", &nlm::DebugTools::displaySpikeAnalysis)
        .def("visualizeSpikeTimeline", &nlm::DebugTools::visualizeSpikeTimeline)
        .def("updatePerformanceCounters", &nlm::DebugTools::updatePerformanceCounters)
        .def("displayBottleneckReport", &nlm::DebugTools::displayBottleneckReport)
        .def("logEvent", &nlm::DebugTools::logEvent)
        .def("displayEventLog", &nlm::DebugTools::displayEventLog)
        .def("exportEventLog", &nlm::DebugTools::exportEventLog)
        .def("toggleDebugOverlay", &nlm::DebugTools::toggleDebugOverlay)
        .def("togglePerformanceProfiler", &nlm::DebugTools::togglePerformanceProfiler)
        .def("toggleMemoryProfiler", &nlm::DebugTools::toggleMemoryProfiler)
        .def("captureCrashDump", &nlm::DebugTools::captureCrashDump)
        .def("generateDebugReport", &nlm::DebugTools::generateDebugReport)
        .def("sendDebugInfo", &nlm::DebugTools::sendDebugInfo)
        .def("setDebugHook", &nlm::DebugTools::setDebugHook)
        .def("executeDebugHooks", &nlm::DebugTools::executeDebugHooks)
        .def("registerDebugCallback", &nlm::DebugTools::registerDebugCallback)
        .def("exportDebugData", &nlm::DebugTools::exportDebugData)
        .def("exportNeuralState", &nlm::DebugTools::exportNeuralState)
        .def("exportPerformanceData", &nlm::DebugTools::exportPerformanceData)
        .def("setMaxMemoryUsage", &nlm::DebugTools::setMaxMemoryUsage)
        .def("setProfileInterval", &nlm::DebugTools::setProfileInterval)
        .def("setDebugVerbosity", &nlm::DebugTools::setDebugVerbosity)
        .def("setOutputFile", &nlm::DebugTools::setOutputFile)
        .def("reset", &nlm::DebugTools::reset)
        .def("pause", &nlm::DebugTools::pause)
        .def("resume", &nlm::DebugTools::resume)
        .def("updateFromPython", &nlm::DebugTools::updateFromPython)
        .def("getDebugState", &nlm::DebugTools::getDebugState);

    // Memory Profiler bindings
    py::class_<nlm::MemoryProfiler>(m, "MemoryProfiler")
        .def(py::init<>())
        .def("start", &nlm::MemoryProfiler::start)
        .def("stop", &nlm::MemoryProfiler::stop)
        .def("isActive", &nlm::MemoryProfiler::isActive)
        .def("trackAllocation", &nlm::MemoryProfiler::trackAllocation,
             py::arg("ptr"), py::arg("size"), py::arg("context"))
        .def("trackDeallocation", &nlm::MemoryProfiler::trackDeallocation)
        .def("analyzeLeaks", &nlm::MemoryProfiler::analyzeLeaks)
        .def("reportLeaks", &nlm::MemoryProfiler::reportLeaks)
        .def("getMemoryLeakSize", &nlm::MemoryProfiler::getMemoryLeakSize)
        .def("getLeakContexts", &nlm::MemoryProfiler::getLeakContexts)
        .def("getCurrentMemoryUsage", &nlm::MemoryProfiler::getCurrentMemoryUsage)
        .def("getPeakMemoryUsage", &nlm::MemoryProfiler::getPeakMemoryUsage)
        .def("exportMemoryReport", &nlm::MemoryProfiler::exportMemoryReport);

    // Performance Analyzer bindings
    py::class_<nlm::PerformanceAnalyzer>(m, "PerformanceAnalyzer")
        .def(py::init<>())
        .def("startOperation", &nlm::PerformanceAnalyzer::startOperation)
        .def("endOperation", &nlm::PerformanceAnalyzer::endOperation)
        .def("recordOperationTime", &nlm::PerformanceAnalyzer::recordOperationTime)
        .def("analyzeBottlenecks", &nlm::PerformanceAnalyzer::analyzeBottlenecks)
        .def("getBottleneckOperations", &nlm::PerformanceAnalyzer::getBottleneckOperations)
        .def("getOperationTime", &nlm::PerformanceAnalyzer::getOperationTime)
        .def("getAverageTime", &nlm::PerformanceAnalyzer::getAverageTime)
        .def("setThreshold", &nlm::PerformanceAnalyzer::setThreshold)
        .def("setPerformanceWarningLevel", &nlm::PerformanceAnalyzer::setPerformanceWarningLevel)
        .def("setPerformanceCriticalLevel", &nlm::PerformanceAnalyzer::setPerformanceCriticalLevel)
        .def("displayPerformanceReport", &nlm::PerformanceAnalyzer::displayPerformanceReport)
        .def("exportPerformanceReport", &nlm::PerformanceAnalyzer::exportPerformanceReport);

    // Neural State Inspector bindings
    py::class_<nlm::NeuralStateInspector>(m, "NeuralStateInspector")
        .def(py::init<>())
        .def("inspectBrainState", &nlm::NeuralStateInspector::inspectBrainState)
        .def("displayNeuronDetails", &nlm::NeuralStateInspector::displayNeuronDetails)
        .def("displaySynapseDetails", &nlm::NeuralStateInspector::displaySynapseDetails)
        .def("displayRegionInfo", &nlm::NeuralStateInspector::displayRegionInfo)
        .def("displayPopulationInfo", &nlm::NeuralStateInspector::displayPopulationInfo)
        .def("analyzeNeuralActivity", &nlm::NeuralStateInspector::analyzeNeuralActivity)
        .def("displayFiringRateStats", &nlm::NeuralStateInspector::displayFiringRateStats)
        .def("displayConnectionStats", &nlm::NeuralStateInspector::displayConnectionStats)
        .def("displaySpikingActivity", &nlm::NeuralStateInspector::displaySpikingActivity)
        .def("prepareInspectionData", &nlm::NeuralStateInspector::prepareInspectionData)
        .def("getAvailableInspectionItems", &nlm::NeuralStateInspector::getAvailableInspectionItems)
        .def("hasInspectionItem", &nlm::NeuralStateInspector::hasInspectionItem);

    // Event Timeline bindings
    py::class_<nlm::EventTimeline>(m, "EventTimeline")
        .def(py::init<>())
        .def("addEvent", &nlm::EventTimeline::addEvent,
             py::arg("type"), py::arg("description"), py::arg("timestamp"),
             py::arg("details") = "")
        .def("clearEvents", &nlm::EventTimeline::clearEvents)
        .def("removeOldEvents", &nlm::EventTimeline::removeOldEvents)
        .def("setTimelineRange", &nlm::EventTimeline::setTimelineRange)
        .def("setEventColors", &nlm::EventTimeline::setEventColors)
        .def("setMaxEventsPerFrame", &nlm::EventTimeline::setMaxEventsPerFrame)
        .def("getEventsInRange", &nlm::EventTimeline::getEventsInRange,
             py::arg("startTime"), py::arg("endTime"))
        .def("exportTimeline", &nlm::EventTimeline::exportTimeline);

    // Visualization data structures
    py::class_<nlm::VisualizationData>(m, "VisualizationData")
        .def_readwrite("timestamp", &nlm::VisualizationData::timestamp)
        .def_readwrite("neuronCount", &nlm::VisualizationData::neuronCount)
        .def_readwrite("synapseCount", &nlm::VisualizationData::synapseCount)
        .def_readwrite("firingRates", &nlm::VisualizationData::firingRates)
        .def_readwrite("spikeCount", &nlm::VisualizationData::spikeCount)
        .def_readwrite("events", &nlm::VisualizationData::events);

    // Helper functions
    m.def("createDebugTools", &nlm::createDebugTools);
    m.def("createEnhancedVisualization", &nlm::createEnhancedVisualization);
    m.def("createMemoryProfiler", &nlm::createMemoryProfiler);
    m.def("createPerformanceAnalyzer", &nlm::createPerformanceAnalyzer);
    m.def("createNeuralStateInspector", &nlm::createNeuralStateInspector);
    m.def("createEventTimeline", &nlm::createEventTimeline);

    // Factory functions for common configurations
    m.def("createStandardDebugSetup", []() {
        auto debugTools = nlm::createDebugTools();
        auto memProfiler = nlm::createMemoryProfiler();
        auto perfAnalyzer = nlm::createPerformanceAnalyzer();
        auto neuralInspector = nlm::createNeuralStateInspector();
        auto eventTimeline = nlm::createEventTimeline();
        return py::make_tuple(debugTools, memProfiler, perfAnalyzer, neuralInspector, eventTimeline);
    });

    m.def("createVisualizationPreset", [](const std::string& preset) {
        if (preset == "standard") {
            return nlm::createEnhancedVisualization(1600, 900, "Standard Visualization");
        } else if (preset == "compact") {
            return nlm::createEnhancedVisualization(800, 600, "Compact Visualization");
        } else if (preset == "detailed") {
            return nlm::createEnhancedVisualization(2000, 1200, "Detailed Visualization");
        }
        return nlm::createEnhancedVisualization();
    });

    // Utility functions
    m.def("exportAllData", [](const std::string& baseFilename, const nlm::EnhancedVisualization& viz,
                           const nlm::DebugTools& debugTools, const nlm::Brain& brain) {
        std::string vizFilename = baseFilename + "_visualization.json";
        std::string debugFilename = baseFilename + "_debug.json";
        std::string brainFilename = baseFilename + "_brain.json";
        
        viz.exportVisualizationData(vizFilename);
        debugTools.exportDebugData(debugFilename);
        debugTools.exportNeuralState(brainFilename, brain);
        
        return std::vector<std::string>({vizFilename, debugFilename, brainFilename});
    });

    // Constants
    m.attr("VERSION") = "2.0.0";
    m.attr("MAX_EVENTS_PER_TIMELINE") = 1000;
    m.attr("DEFAULT_FRAME_RATE") = 60.0;
    m.attr("DEFAULT_UPDATE_RATE") = 30.0;
}
