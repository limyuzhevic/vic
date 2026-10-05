#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class ExperimentModule : public BindingModule {
    public:
        std::string getName() const override { return "experiment"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Experiment system bindings for NLM (Experiment, ExperimentRunner, Metrics)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Experiment class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // Experiment class
            builder.class_<Experiment>("Experiment", R"pbdoc(Base experiment class)pbdoc")
                .def(py::init<>())
                .def("initialize", &Experiment::initialize, "Initialize experiment")
                .def("run", &Experiment::run, "Run experiment")
                .def("reset", &Experiment::reset, "Reset experiment state")
                .def("isRunning", &Experiment::isRunning, "Check if experiment is running")
                .def("getDuration", &Experiment::getDuration, "Get experiment duration")
                .def("getResults", &Experiment::getResults,
                     py::return_value_policy::reference_internal,
                     "Get experiment results")
                .def("addMetric", &Experiment::addMetric, py::arg("name"), py::arg("value"),
                     "Add metric to experiment")
                .def("getMetric", &Experiment::getMetric, py::arg("name"),
                     "Get metric by name")
                .def("getMetrics", &Experiment::getMetrics,
                     py::return_value_policy::reference_internal,
                     "Get all metrics");
            
            // ExperimentRunner class
            builder.class_<ExperimentRunner>("ExperimentRunner", R"pbdoc(Experiment runner)pbdoc")
                .def(py::init<>())
                .def("addExperiment", &ExperimentRunner::addExperiment, py::arg("experiment"),
                     "Add experiment to runner")
                .def("removeExperiment", &ExperimentRunner::removeExperiment, py::arg("name"),
                     "Remove experiment by name")
                .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("name"),
                     "Run specific experiment")
                .def("runAllExperiments", &ExperimentRunner::runAllExperiments,
                     "Run all experiments")
                .def("stopExperiment", &ExperimentRunner::stopExperiment, py::arg("name"),
                     "Stop specific experiment")
                .def("stopAllExperiments", &ExperimentRunner::stopAllExperiments,
                     "Stop all experiments")
                .def("getExperiment", &ExperimentRunner::getExperiment, py::arg("name"),
                     py::return_value_policy::reference_internal,
                     "Get experiment by name")
                .def("getExperiments", &ExperimentRunner::getExperiments,
                     py::return_value_policy::reference_internal,
                     "Get all experiments")
                .def("clearExperiments", &ExperimentRunner::clearExperiments,
                     "Clear all experiments")
                .def("runBatch", &ExperimentRunner::runBatch, py::arg("experiments"),
                     "Run batch of experiments");
            
            // Metrics class
            builder.class_<Metrics>("Metrics", R"pbdoc(Experiment metrics)pbdoc")
                .def(py::init<>())
                .def("addMetric", &Metrics::addMetric, py::arg("name"), py::arg("value"), py::arg("timestamp") = 0.0,
                     "Add metric with timestamp")
                .def("getMetric", &Metrics::getMetric, py::arg("name"),
                     "Get metric by name")
                .def("getMetricAtTime", &Metrics::getMetricAtTime, py::arg("name"), py::arg("timestamp"),
                     "Get metric value at specific time")
                .def("getAllMetrics", &Metrics::getAllMetrics,
                     py::return_value_policy::reference_internal,
                     "Get all metrics")
                .def("getMetricNames", &Metrics::getMetricNames,
                     "Get all metric names")
                .def("getMetricCount", &Metrics::getMetricCount,
                     "Get number of metrics")
                .def("clear", &Metrics::clear, "Clear all metrics")
                .def("getSummary", &Metrics::getSummary,
                     "Get metrics summary")
                .def("getAverage", &Metrics::getAverage, py::arg("name"),
                     "Get average value of metric")
                .def("getMax", &Metrics::getMax, py::arg("name"),
                     "Get maximum value of metric")
                .def("getMin", &Metrics::getMin, py::arg("name"),
                     "Get minimum value of metric")
                .def("getStandardDeviation", &Metrics::getStandardDeviation, py::arg("name"),
                     "Get standard deviation of metric");
            
            // Context manager support for experiment operations
            builder.class_<Experiment>("ExperimentContext", R"pbdoc(Context manager for Experiment operations)pbdoc")
                .def(py::init<Experiment>())
                .def("enter", [](Experiment& self) { return self; })
                .def("exit", [](Experiment&, const py::args& args) { return true; });
            
            builder.class_<ExperimentRunner>("ExperimentRunnerContext", R"pbdoc(Context manager for ExperimentRunner operations)pbdoc")
                .def(py::init<ExperimentRunner>())
                .def("enter", [](ExperimentRunner& self) { return self; })
                .def("exit", [](ExperimentRunner&, const py::args& args) { return true; });
            
            // Experiment factory functions
            m.def("create_experiment", []() {
                return std::make_shared<Experiment>();
            }, "Create a new experiment");
            
            m.def("create_experiment_runner", []() {
                return std::make_shared<ExperimentRunner>();
            }, "Create a new experiment runner");
            
            m.def("create_metrics", []() {
                return std::make_shared<Metrics>();
            }, "Create a new metrics collector");
            
            // Experiment utilities
            m.def("get_experiment_system_names", []() {
                return std::vector<std::string>{"experiment", "experiment_runner", "metrics"};
            }, "Get list of available experiment systems");
            
            m.def("experiment_system_info", [](const std::string& systemName) {
                // Implementation would return system information
                return std::map<std::string, std::string>{{ "name", systemName }, { "description", "" }};
            }, py::arg("systemName"), "Get information about an experiment system");
            
            // Experiment validation
            m.def("validate_experiment_state", [](const Experiment& experiment) {
                // Implementation would validate experiment state
                return experiment.isRunning();
            }, py::arg("experiment"), "Validate experiment state");
            
            m.def("validate_experiment_runner_state", [](const ExperimentRunner& runner) {
                // Implementation would validate runner state
                return true;
            }, py::arg("runner"), "Validate experiment runner state");
            
            // Experiment simulation utilities
            m.def("run_single_experiment", [](Experiment& experiment) {
                experiment.run();
            }, py::arg("experiment"), "Run single experiment");
            
            m.def("run_experiment_batch", [](ExperimentRunner& runner, const std::vector<std::string>& names) {
                // Implementation would run batch of experiments
                std::vector<std::string> results;
                for (const auto& name : names) {
                    runner.runExperiment(name);
                    results.push_back(name + " completed");
                }
                return results;
            }, py::arg("runner"), py::arg("names"), "Run batch of experiments");
            
            // Experiment results utilities
            m.def("export_experiment_results", [](const Experiment& experiment) {
                // Implementation would export experiment results
                return std::map<std::string, py::object>{};
            }, py::arg("experiment"), "Export experiment results");
            
            m.def("export_metrics_data", [](const Metrics& metrics) {
                // Implementation would export metrics data
                return std::vector<std::pair<double, std::map<std::string, double>>>{};
            }, py::arg("metrics"), "Export metrics data");
        }
        
        void initialize() override {
            // Initialize experiment module
        }
        
        void shutdown() override {
            // Cleanup experiment module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "experiment_factory" || feature == "experiment_utils" || 
                   feature == "experiment_runner" || feature == "experiment_simulation" ||
                   feature == "experiment_context" || feature == "experiment_validation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "experiment_factory") {
                return py::module_::import("nlm.experiment").attr("experiment_factory");
            } else if (feature == "experiment_utils") {
                return py::module_::import("nlm.experiment").attr("experiment_utils");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the experiment module
    static ExperimentModule register_experiment_module() {
        auto module = std::make_shared<ExperimentModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(experiment_bindings, m) {
        static ExperimentModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Experiment Python Bindings
            -----------------------------
            
            This module provides Python bindings for the NLM experiment system,
            including Experiment, ExperimentRunner, and Metrics classes.
            
            It provides experiment interfaces for running and managing
            scientific experiments, with context managers and experiment utilities.
            
            .. currentmodule:: experiment_bindings
            
            Experiment API
            ===============""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<ExperimentModule>(module));
    }
}