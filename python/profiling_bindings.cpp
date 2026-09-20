// Profiling module bindings for Python
// This file contains the pybind11 bindings for the performance profiling tools

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include "BrainProfiler.hpp"

namespace py = pybind11;

PYBIND11_MODULE(profiling, m) {
    m.doc() = "Performance profiling tools for NLM neural simulation system";
    
    // Define the config struct for BrainProfiler
    py::class_<nlm::BrainProfiler::Config>(m, "ProfilerConfig")
        .def(py::init<>())
        .def_readwrite("enableProfiler", &nlm::BrainProfiler::Config::enableProfiler)
        .def_readwrite("autoStart", &nlm::BrainProfiler::Config::autoStart)
        .def_readwrite("samplingInterval", &nlm::BrainProfiler::Config::samplingInterval)
        .def_readwrite("profileTimeSteps", &nlm::BrainProfiler::Config::profileTimeSteps)
        .def_readwrite("profileNeuronProcessing", &nlm::BrainProfiler::Config::profileNeuronProcessing)
        .def_readwrite("profileSynapseProcessing", &nlm::BrainProfiler::Config::profileSynapseProcessing)
        .def_readwrite("profilePlasticity", &nlm::BrainProfiler::Config::profilePlasticity)
        .def_readwrite("profileMemorySystems", &nlm::BrainProfiler::Config::profileMemorySystems)
        .def_readwrite("profileEventProcessing", &nlm::BrainProfiler::Config::profileEventProcessing)
        .def_readwrite("profileConcurrentOperations", &nlm::BrainProfiler::Config::profileConcurrentOperations)
        .def_readwrite("profileSpikes", &nlm::BrainProfiler::Config::profileSpikes)
        .def_readwrite("exportDirectory", &nlm::BrainProfiler::Config::exportDirectory)
        .def_readwrite("exportOnStop", &nlm::BrainProfiler::Config::exportOnStop);

    // Define the BrainProfiler class
    py::class_<nlm::BrainProfiler>(m, "BrainProfiler")
        .def(py::init<std::shared_ptr<nlm::Brain>, nlm::BrainProfiler::Config>(), py::arg("brain"), py::arg("config"))
        .def("updateConfig", &nlm::BrainProfiler::updateConfig)
        .def("getConfig", &nlm::BrainProfiler::getConfig)
        .def("start", &nlm::BrainProfiler::start)
        .def("stop", &nlm::BrainProfiler::stop)
        .def("pause", &nlm::BrainProfiler::pause)
        .def("resume", &nlm::BrainProfiler::resume)
        .def("isRunning", &nlm::BrainProfiler::isRunning)
        .def("isPaused", &nlm::BrainProfiler::isPaused)
        .def("getCurrentSnapshot", &nlm::BrainProfiler::getCurrentSnapshot)
        .def("getSnapshotsInRange", &nlm::BrainProfiler::getSnapshotsInRange)
        .def("getAllSnapshots", &nlm::BrainProfiler::getAllSnapshots)
        .def("integrateWithBrain", &nlm::BrainProfiler::integrateWithBrain)
        .def("exportToJSON", &nlm::BrainProfiler::exportToJSON)
        .def("exportToCSV", &nlm::BrainProfiler::exportToCSV)
        .def("generateReport", &nlm::BrainProfiler::generateReport)
        .def("getReportText", &nlm::BrainProfiler::getReportText)
        .def("getPerformanceProfiler", &nlm::BrainProfiler::getPerformanceProfiler)
        .def("getMemoryMonitor", &nlm::BrainProfiler::getMemoryMonitor)
        .def("getEventProfiler", &nlm::BrainProfiler::getEventProfiler)
        .def("enableProfiler", &nlm::BrainProfiler::enableProfiler)
        .def("setProfilerMode", &nlm::BrainProfiler::setProfilerMode)
        .def("enableMemoryMonitoring", &nlm::BrainProfiler::enableMemoryMonitoring)
        .def("enableEventProfiling", &nlm::BrainProfiler::enableEventProfiling);

    // Factory function for default config
    m.def("createDefaultProfilerConfig", []() {
        nlm::BrainProfiler::Config config;
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

    // Factory function for BrainProfiler
    m.def("createBrainProfiler", [](std::shared_ptr<nlm::Brain> brain, const nlm::BrainProfiler::Config& config = createDefaultProfilerConfig()) {
        return std::make_shared<nlm::BrainProfiler>(brain, config);
    });
}
