// Simulation utilities module for NLM Python bindings
// Enhanced simulation utilities including parallel processing, statistical analysis, etc.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "SimulationUtils.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_simulations, m) {
    m.doc() = R"pbdoc(
        NLM Simulation Utilities
        --------------------------
        Enhanced simulation utilities for NLM (Neural Learning Machine).
        
        Provides advanced simulation capabilities including:
        - Progress tracking and monitoring
        - Parallel simulation execution
        - Statistical analysis of results
        - Configuration comparison and validation
        - Data export/import utilities
        - Benchmarking and performance analysis
        - Demo configuration management
    )pbdoc";

    // Bind SimulationUtilities class
    py::class_<nlm::SimulationUtilities>(m, "SimulationUtilities")
        .def("runSimulationWithProgress", &nlm::SimulationUtilities::runSimulationWithProgress,
             py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
             py::arg("progressCallback"), py::arg("timestep") = std::chrono::milliseconds(100),
             "Run simulation with progress callback")
        .def("runParallelSimulations", &nlm::SimulationUtilities::runParallelSimulations,
             py::arg("numSimulations"), py::arg("config"), py::arg("stepsPerSimulation"),
             py::arg("progressCallback") = nullptr,
             "Run multiple simulations in parallel")
        .def("analyzeSimulationResults", &nlm::SimulationUtilities::analyzeSimulationResults,
             py::arg("brains"),
             "Analyze simulation results and return statistics")
        .def("compareConfigurations", &nlm::SimulationUtilities::compareConfigurations,
             py::arg("config1"), py::arg("config2"),
             "Compare two configurations and return differences")
        .def("exportResultsToCSV", &nlm::SimulationUtilities::exportResultsToCSV,
             py::arg("brains"), py::arg("filename"),
             "Export simulation results to CSV file")
        .def("importResultsFromCSV", &nlm::SimulationUtilities::importResultsFromCSV,
             py::arg("filename"),
             "Import simulation results from CSV file")
        .def("runBenchmarkSuite", &nlm::SimulationUtilities::runBenchmarkSuite,
             py::arg("brains"), py::arg("stepsPerBenchmark"),
             "Run benchmarking suite on multiple brains")
        .def("createDemoConfigs", &nlm::SimulationUtilities::createDemoConfigs,
             "Create a collection of demonstration configurations");

    // Convenience functions
    m.def("run_batched_simulations", [](int numSimulations, std::shared_ptr<nlm::Config> config, 
                                        int stepsPerSimulation, 
                                        std::function<void(int, std::string)> progressCallback) {
        return nlm::SimulationUtilities::runParallelSimulations(
            numSimulations, config, stepsPerSimulation, progressCallback);
    }, py::arg("numSimulations"), py::arg("config"), py::arg("stepsPerSimulation"),
       py::arg("progressCallback") = nullptr,
       "Run batched simulations (convenience function)");

    m.def("analyze_simulation_batch", &nlm::SimulationUtilities::analyzeSimulationResults,
         py::arg("brains"),
         "Analyze batch simulation results (convenience function)");

    m.def("benchmark_simulations", [](std::vector<std::shared_ptr<nlm::Brain>> brains, int steps) {
        return nlm::SimulationUtilities::runBenchmarkSuite(brains, steps);
    }, py::arg("brains"), py::arg("steps"),
       "Benchmark simulations (convenience function)");

    // Get demo configurations
    m.def("get_demo_configs", &nlm::SimulationUtilities::createDemoConfigs,
         "Get demonstration configurations");

    // Utility functions
    m.def("get_simulation_stats", [](const std::vector<std::shared_ptr<nlm::Brain>>& brains) {
        auto stats = nlm::SimulationUtilities::analyzeSimulationResults(brains);
        return py::dict(
            "mean_neurons", stats["mean_neurons"],
            "mean_spikes", stats["mean_spikes"],
            "mean_firing_rate", stats["mean_firing_rate"],
            "mean_ei_ratio", stats["mean_ei_ratio"],
            "std_neurons", stats["neuron_std"],
            "std_spikes", stats["spike_std"],
            "spike_p50", stats["spike_p50"],
            "spike_p95", stats["spike_p95"]
        );
    }, py::arg("brains"),
       "Get formatted simulation statistics");

    m.def("compare_configurations", &nlm::SimulationUtilities::compareConfigurations,
         py::arg("config1"), py::arg("config2"),
         "Compare configurations and return formatted results");
}
