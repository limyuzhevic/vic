#pragma once

#include "BrainProfiler.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;

namespace nlm {

// Python bindings for BrainProfiler
PYBIND11_MODULE(profiling, m) {
    m.doc() = R"pbdoc(
        NLM Performance Profiling Module
        -------------------------------
        Provides comprehensive profiling tools for NLM brain simulations.
        Includes PerformanceProfiler, MemoryMonitor, EventProfiler, and BrainProfiler.
    )pbdoc";

    // Configure JSON library if available
#ifdef NLM_WITH_JSON
    m.attr("HAS_JSON") = true;
#else
    m.attr("HAS_JSON") = false;
#endif

    // BrainProfiler Config struct
    py::class_<BrainProfiler::Config>(m, "ProfilerConfig")
        .def(py::init<>())
        .def_readwrite("enableProfiler", &BrainProfiler::Config::enableProfiler)
        .def_readwrite("autoStart", &BrainProfiler::Config::autoStart)
        .def_readwrite("samplingInterval", &BrainProfiler::Config::samplingInterval)
        .def_readwrite("profileTimeSteps", &BrainProfiler::Config::profileTimeSteps)
        .def_readwrite("profileNeuronProcessing", &BrainProfiler::Config::profileNeuronProcessing)
        .def_readwrite("profileSynapseProcessing", &BrainProfiler::Config::profileSynapseProcessing)
        .def_readwrite("profilePlasticity", &BrainProfiler::Config::profilePlasticity)
        .def_readwrite("profileMemorySystems", &BrainProfiler::Config::profileMemorySystems)
        .def_readwrite("profileEventProcessing", &BrainProfiler::Config::profileEventProcessing)
        .def_readwrite("profileConcurrentOperations", &BrainProfiler::Config::profileConcurrentOperations)
        .def_readwrite("profileSpikes", &BrainProfiler::Config::profileSpikes)
        .def_readwrite("exportDirectory", &BrainProfiler::Config::exportDirectory)
        .def_readwrite("exportOnStop", &BrainProfiler::Config::exportOnStop);

    // PerformanceProfiler::Mode enum
    py::enum_<PerformanceProfiler::Mode>(m, "ProfilerMode")
        .value("OFF", PerformanceProfiler::Mode::OFF)
        .value("BASIC", PerformanceProfiler::Mode::BASIC)
        .value("DETAILED", PerformanceProfiler::Mode::DETAILED)
        .value("COMPREHENSIVE", PerformanceProfiler::Mode::COMPREHENSIVE)
        .export_values();

    // PerformanceProfiler class
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler")
        .def(py::init<>())
        .def("setMode", &PerformanceProfiler::setMode)
        .def("enableOperationProfiling", &PerformanceProfiler::enableOperationProfiling)
        .def("setMaxHistorySize", &PerformanceProfiler::setMaxHistorySize)
        .def("setProfilingInterval", &PerformanceProfiler::setProfilingInterval)
        .def("recordTimePoint", &PerformanceProfiler::recordTimePoint)
        .def("recordAllocation", &PerformanceProfiler::recordAllocation)
        .def("recordDeallocation", &PerformanceProfiler::recordDeallocation)
        .def("profileStepStart", &PerformanceProfiler::profileStepStart)
        .def("profileStepEnd", &PerformanceProfiler::profileStepEnd)
        .def("profileNeuronProcessing", &PerformanceProfiler::profileNeuronProcessing)
        .def("profileSynapseProcessing", &PerformanceProfiler::profileSynapseProcessing)
        .def("profilePlasticityApplication", &PerformanceProfiler::profilePlasticityApplication)
        .def("profileMemorySystem", &PerformanceProfiler::profileMemorySystem)
        .def("profileEventQueue", &PerformanceProfiler::profileEventQueue)
        .def("profileSpikeProcessing", &PerformanceProfiler::profileSpikeProcessing)
        .def("profileConcurrentOperation", &PerformanceProfiler::profileConcurrentOperation)
        .def("getOperationStats", &PerformanceProfiler::getOperationStats)
        .def("getTotalSimulationTime", &PerformanceProfiler::getTotalSimulationTime)
        .def("getTotalTimePoints", &PerformanceProfiler::getTotalTimePoints)
        .def("getMemoryLeakRate", &PerformanceProfiler::getMemoryLeakRate)
        .def("getThreadStats", &PerformanceProfiler::getThreadStats)
        .def("exportToJSON", &PerformanceProfiler::exportToJSON)
        .def("exportToCSV", &PerformanceProfiler::exportToCSV)
        .def("generateReport", &PerformanceProfiler::generateReport)
        .def("start", &PerformanceProfiler::start)
        .def("stop", &PerformanceProfiler::stop)
        .def("reset", &PerformanceProfiler::reset)
        .def("isRunning", &PerformanceProfiler::isRunning);

    // MemoryMonitor class
    py::class_<MemoryMonitor>(m, "MemoryMonitor")
        .def(py::init<>())
        .def("start", &MemoryMonitor::start)
        .def("stop", &MemoryMonitor::stop)
        .def("setEnabled", &MemoryMonitor::setEnabled)
        .def("isEnabled", &MemoryMonitor::isEnabled)
        .def("setSamplingInterval", &MemoryMonitor::setSamplingInterval)
        .def("setMaxMemoryUsage", &MemoryMonitor::setMaxMemoryUsage)
        .def("setMemoryPoolsEnabled", &MemoryMonitor::setMemoryPoolsEnabled)
        .def("recordAllocation", &MemoryMonitor::recordAllocation)
        .def("recordDeallocation", &MemoryMonitor::recordDeallocation)
        .def("updatePoolStats", &MemoryMonitor::updatePoolStats)
        .def("recordMemoryUsage", &MemoryMonitor::recordMemoryUsage)
        .def("getCurrentMemoryUsage", &MemoryMonitor::getCurrentMemoryUsage)
        .def("checkForLeaks", &MemoryMonitor::checkForLeaks)
        .def("getMemoryLeakRate", &MemoryMonitor::getMemoryLeakRate)
        .def("getFragmentationLevel", &MemoryMonitor::getFragmentationLevel)
        .def("getPeakMemoryUsage", &MemoryMonitor::getPeakMemoryUsage)
        .def("getTotalAllocated", &MemoryMonitor::getTotalAllocated)
        .def("getTotalDeallocated", &MemoryMonitor::getTotalDeallocated)
        .def("getComponentStats", &MemoryMonitor::getComponentStats)
        .def("getPoolStats", &MemoryMonitor::getPoolStats)
        .def("generateReport", &MemoryMonitor::generateReport)
        .def("exportToJSON", &MemoryMonitor::exportToJSON)
        .def("exportToCSV", &MemoryMonitor::exportToCSV);

    // EventProfiler::EventType enum
    py::enum_<EventProfiler::EventType>(m, "EventType")
        .value("SPIKE", EventProfiler::EventType::SPIKE)
        .value("DELAYED_SPIKE", EventProfiler::EventType::DELAYED_SPIKE)
        .value("PLASTICITY_UPDATE", EventProfiler::EventType::PLASTICITY_UPDATE)
        .value("NEUROMODULATION", EventProfiler::EventType::NEUROMODULATION)
        .value("MEMORY_ACCESS", EventProfiler::EventType::MEMORY_ACCESS)
        .value("SYNAPTIC_EVENT", EventProfiler::EventType::SYNAPTIC_EVENT)
        .value("SYSTEM_EVENT", EventProfiler::EventType::SYSTEM_EVENT)
        .export_values();

    // EventProfiler class
    py::class_<EventProfiler>(m, "EventProfiler")
        .def(py::init<>())
        .def("setEnabled", &EventProfiler::setEnabled)
        .def("setMaxEventHistorySize", &EventProfiler::setMaxEventHistorySize)
        .def("setSamplingInterval", &EventProfiler::setSamplingInterval)
        .def("recordEvent", &EventProfiler::recordEvent)
        .def("recordSpikeEvent", &EventProfiler::recordSpikeEvent)
        .def("recordDelayedSpikeEvent", &EventProfiler::recordDelayedSpikeEvent)
        .def("recordQueueOperation", &EventProfiler::recordQueueOperation)
        .def("recordPriorityEvent", &EventProfiler::recordPriorityEvent)
        .def("getLastEvent", &EventProfiler::getLastEvent)
        .def("getEventsInTimeRange", &EventProfiler::getEventsInTimeRange)
        .def("getQueueStats", &EventProfiler::getQueueStats)
        .def("getSpikeDeliveryStats", &EventProfiler::getSpikeDeliveryStats)
        .def("getPriorityStats", &EventProfiler::getPriorityStats)
        .def("getAverageProcessingTime", &EventProfiler::getAverageProcessingTime)
        .def("getQueueOverflowRate", &EventProfiler::getQueueOverflowRate)
        .def("exportToJSON", &EventProfiler::exportToJSON)
        .def("exportToCSV", &EventProfiler::exportToCSV)
        .def("generateReport", &EventProfiler::generateReport)
        .def("start", &EventProfiler::start)
        .def("stop", &EventProfiler::stop)
        .def("reset", &EventProfiler::reset)
        .def("isRunning", &EventProfiler::isRunning);

    // BrainProfiler class
    py::class_<BrainProfiler>(m, "BrainProfiler")
        .def(py::init<std::shared_ptr<Brain>, BrainProfiler::Config>())
        .def("updateConfig", &BrainProfiler::updateConfig)
        .def("getConfig", &BrainProfiler::getConfig)
        .def("start", &BrainProfiler::start)
        .def("stop", &BrainProfiler::stop)
        .def("pause", &BrainProfiler::pause)
        .def("resume", &BrainProfiler::resume)
        .def("isRunning", &BrainProfiler::isRunning)
        .def("isPaused", &BrainProfiler::isPaused)
        .def("getCurrentSnapshot", &BrainProfiler::getCurrentSnapshot)
        .def("getSnapshotsInRange", &BrainProfiler::getSnapshotsInRange)
        .def("getAllSnapshots", &BrainProfiler::getAllSnapshots)
        .def("integrateWithBrain", &BrainProfiler::integrateWithBrain)
        .def("exportToJSON", &BrainProfiler::exportToJSON)
        .def("exportToCSV", &BrainProfiler::exportToCSV)
        .def("generateReport", &BrainProfiler::generateReport)
        .def("getReportText", &BrainProfiler::getReportText)
        .def("getPerformanceProfiler", &BrainProfiler::getPerformanceProfiler)
        .def("getMemoryMonitor", &BrainProfiler::getMemoryMonitor)
        .def("getEventProfiler", &BrainProfiler::getEventProfiler)
        .def("enableProfiler", &BrainProfiler::enableProfiler)
        .def("setProfilerMode", &BrainProfiler::setProfilerMode)
        .def("enableMemoryMonitoring", &BrainProfiler::enableMemoryMonitoring)
        .def("enableEventProfiling", &BrainProfiler::enableEventProfiling);

    // Convenience factory functions
    m.def("createDefaultProfilerConfig", []() {
        BrainProfiler::Config config;
        config.enableProfiler = true;
        config.autoStart = false;
        config.samplingInterval = 0.1;
        config.profileTimeSteps = true;
        config.profileNeuronProcessing = true;
        config.profileSynapseProcessing = true;
        config.profilePlasticity = true;
        config.profileMemorySystems = true;
        config.profileEventProcessing = true;
        config.profileConcurrentOperations = true;
        config.profileSpikes = true;
        return config;
    });

    m.def("createBrainProfiler", [](std::shared_ptr<Brain> brain, const BrainProfiler::Config& config = createDefaultProfilerConfig()) {
        return std::make_shared<BrainProfiler>(brain, config);
    });
}
