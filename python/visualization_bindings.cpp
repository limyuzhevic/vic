// Python bindings for NLM visualization and experiment systems
// This file contains bindings for visualization interfaces and experiment framework

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Phase3Experiment.hpp"
#include "../src/experiments/Phase4Experiment.hpp"

namespace py = pybind11;
namespace nlm {

void bindVisualizationInterface(py::module& m);
void bindExperiment(py::module& m);
void bindExperimentRunner(py::module& m);
void bindMetrics(py::module& m);
void bindPhase3Experiment(py::module& m);
void bindPhase4Experiment(py::module& m);

void include_visualization_bindings(py::module& m) {
    bindVisualizationInterface(m);
}

void include_experiment_bindings(py::module& m) {
    bindExperiment(m);
    bindExperimentRunner(m);
    bindMetrics(m);
    bindPhase3Experiment(m);
    bindPhase4Experiment(m);
}

void bindVisualizationInterface(py::module& m) {
    // VisualizationInterface class - Visualization system for NLM
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(
        Visualization system for NLM brain simulations.
        
        The VisualizationInterface class provides visualization capabilities
        for NLM brain simulations, including:
        - Real-time neural activity visualization
        - Synaptic connectivity display
        - Brain region representation
        - Simulation timeline visualization
        - Interactive visualization
        
        Visualization enables the brain to:
        - Monitor neural activity
        - Visualize learning processes
        - Analyze simulation results
        - Debug and optimize simulations
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create visualization interface
            >>> viz = pynlm.VisualizationInterface()
            >>> 
            >>> # Configure visualization
            >>> viz.setWindowSize(1024, 768)
            >>> viz.setShowNeurons(True)
            >>> viz.setShowSynapses(True)
            >>> viz.setShowRegions(True)
            >>> 
            >>> # Update visualization
            >>> for step in range(100):
            ...     viz.update(brain, step)
            ...     
            >>> 
            >>> # Save visualization
            >>> viz.save("brain_activity.png")
        
        Methods:
            __init__(): Create empty visualization interface
            update(brain, step): Update visualization with current brain state
            setWindowSize(width, height): Set window size
            getWindowSize(): Get window size
            setShowNeurons(show): Show/hide neurons
            getShowNeurons(): Get neuron visibility
            setShowSynapses(show): Show/hide synapses
            getShowSynapses(): Get synapse visibility
            setShowRegions(show): Show/hide regions
            getShowRegions(): Get region visibility
            save(filename): Save visualization to file
            show(): Show visualization window
            close(): Close visualization window
            isRunning(): Check if visualization is running
            setUpdateInterval(interval): Set update interval
            getUpdateInterval(): Get update interval
        
        Attributes:
            window_width: Visualization window width
            window_height: Visualization window height
            show_neurons: Neuron visibility
            show_synapses: Synapse visibility
            show_regions: Region visibility
            
        See Also:
            Brain, NeuralRegion, NeuralPopulation, Visualization
    )pbdoc")
        .def(py::init<>())
        
        // Visualization updates
        .def("update", &VisualizationInterface::update,
             py::arg("brain"), py::arg("step"),
             "Update visualization with current brain state")
        
        // Window configuration
        .def("setWindowSize", &VisualizationInterface::setWindowSize,
             py::arg("width"), py::arg("height"),
             "Set window size")
        
        .def("getWindowSize", &VisualizationInterface::getWindowSize,
             "Get window size")
        
        // Display options
        .def("setShowNeurons", &VisualizationInterface::setShowNeurons,
             py::arg("show"),
             "Show/hide neurons")
        
        .def("getShowNeurons", &VisualizationInterface::getShowNeurons,
             "Get neuron visibility")
        
        .def("setShowSynapses", &VisualizationInterface::setShowSynapses,
             py::arg("show"),
             "Show/hide synapses")
        
        .def("getShowSynapses", &VisualizationInterface::getShowSynapses,
             "Get synapse visibility")
        
        .def("setShowRegions", &VisualizationInterface::setShowRegions,
             py::arg("show"),
             "Show/hide regions")
        
        .def("getShowRegions", &VisualizationInterface::getShowRegions,
             "Get region visibility")
        
        // File operations
        .def("save", &VisualizationInterface::save, py::arg("filename"),
             "Save visualization to file")
        
        .def("show", &VisualizationInterface::show,
             "Show visualization window")
        
        .def("close", &VisualizationInterface::close,
             "Close visualization window")
        
        // Status
        .def("isRunning", &VisualizationInterface::isRunning,
             "Check if visualization is running")
        
        // Performance
        .def("setUpdateInterval", &VisualizationInterface::setUpdateInterval,
             py::arg("interval"),
             "Set update interval")
        
        .def("getUpdateInterval", &VisualizationInterface::getUpdateInterval,
             "Get update interval")
        
        .def("__repr__", [](const VisualizationInterface& viz) {
            return "<VisualizationInterface: " + std::to_string(viz.getWindowSize().x) +
                   "x" + std::to_string(viz.getWindowSize().y) + ">";
        }, "String representation of visualization interface");
}

void bindExperiment(py::module& m) {
    // Experiment class - Single experiment run
    py::class_<Experiment>(m, "Experiment", R"pbdoc(
        Single experiment run for NLM brain.
        
        The Experiment class represents a single run of an experiment
        for the NLM brain, including:
        - Experimental protocol: Specific sequence of actions
        - Hypothesis testing: Testing scientific hypotheses
        - Data collection: Recording experimental data
        - Result analysis: Analyzing experimental results
        - Validation: Validating experimental outcomes
        
        Experiments enable the brain to:
        - Test scientific hypotheses
        - Collect experimental data
        - Analyze and interpret results
        - Validate theoretical predictions
        - Advance scientific knowledge
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create experiment
            >>> experiment = pynlm.Experiment(
            ...     name="plasticity_experiment",
            ...     description="Test STDP learning",
            ...     hypothesis="Correlated activity strengthens synapses"
            ... )
            >>> 
            >>> # Add protocol steps
            >>> experiment.addProtocolStep("pre_test", lambda: runBaseline(brain))
            >>> experiment.addProtocolStep("stimulus", lambda: applyCorrelatedActivity(brain))
            >>> experiment.addProtocolStep("post_test", lambda: runPostTest(brain))
            >>> 
            >>> # Run experiment
            >>> experiment.run()
            >>> 
            >>> # Get results
            >>> results = experiment.getResults()
            >>> print(f"Experiment results: {results}")
            >>> print(f"Hypothesis confirmed: {experiment.isHypothesisConfirmed()}")
        
        Methods:
            __init__(name, description, hypothesis): Create experiment
            getName(): Get experiment name
            getDescription(): Get experiment description
            getHypothesis(): Get hypothesis
            setHypothesis(hypothesis): Set hypothesis
            addProtocolStep(name, callback): Add protocol step
            getProtocolSteps(): Get protocol steps
            removeProtocolStep(name): Remove protocol step
            run(): Run experiment
            isRunning(): Check if experiment is running
            getStatus(): Get experiment status
            getResults(): Get experiment results
            isHypothesisConfirmed(): Check if hypothesis confirmed
            getData(): Get experimental data
            getMetrics(): Get experiment metrics
            saveResults(filename): Save results to file
            loadResults(filename): Load results from file
        
        Attributes:
            name: Experiment name
            description: Experiment description
            hypothesis: Scientific hypothesis
            status: Experiment status
            results: Experiment results
            data: Experimental data
            
        See Also:
            ExperimentRunner, Metrics, Phase3Experiment, Phase4Experiment
    )pbdoc")
        .def(py::init<const std::string&, const std::string&, const std::string&>(),
             py::arg("name"), py::arg("description"), py::arg("hypothesis"),
             "Create experiment")
        
        // Metadata
        .def("getName", &Experiment::getName,
             "Get experiment name")
        
        .def("getDescription", &Experiment::getDescription,
             "Get experiment description")
        
        .def("getHypothesis", &Experiment::getHypothesis,
             "Get hypothesis")
        
        .def("setHypothesis", &Experiment::setHypothesis, py::arg("hypothesis"),
             "Set hypothesis")
        
        // Protocol management
        .def("addProtocolStep", &Experiment::addProtocolStep,
             py::arg("name"), py::arg("callback"),
             "Add protocol step")
        
        .def("getProtocolSteps", &Experiment::getProtocolSteps,
             "Get protocol steps")
        
        .def("removeProtocolStep", &Experiment::removeProtocolStep,
             py::arg("name"),
             "Remove protocol step")
        
        // Experiment execution
        .def("run", &Experiment::run,
             "Run experiment")
        
        .def("isRunning", &Experiment::isRunning,
             "Check if experiment is running")
        
        .def("getStatus", &Experiment::getStatus,
             "Get experiment status")
        
        // Results
        .def("getResults", &Experiment::getResults,
             "Get experiment results")
        
        .def("isHypothesisConfirmed", &Experiment::isHypothesisConfirmed,
             "Check if hypothesis confirmed")
        
        .def("getData", &Experiment::getData,
             "Get experimental data")
        
        .def("getMetrics", &Experiment::getMetrics,
             "Get experiment metrics")
        
        // File operations
        .def("saveResults", &Experiment::saveResults, py::arg("filename"),
             "Save results to file")
        
        .def("loadResults", &Experiment::loadResults, py::arg("filename"),
             "Load results from file")
        
        .def("__repr__", [](const Experiment& experiment) {
            return "<Experiment: " + experiment.getName() + 
                   " (status=" + experiment.getStatus() + ")>";
        }, "String representation of experiment");
}

void bindExperimentRunner(py::module& m) {
    // ExperimentRunner class - Experiment execution framework
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(
        Experiment execution framework for NLM brain.
        
        The ExperimentRunner class provides a framework for running
        experiments for the NLM brain, including:
        - Experiment scheduling: Managing experiment execution order
        - Resource management: Allocating resources for experiments
        - Results management: Collecting and storing experiment results
        - Reporting: Generating experiment reports
        - Validation: Validating experiment execution
        
        Experiment runner enables the brain to:
        - Execute complex experimental protocols
        - Manage multiple experiments
        - Collect and analyze experimental data
        - Generate experimental reports
        - Validate experimental results
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create experiment runner
            >>> runner = pynlm.ExperimentRunner()
            >>> 
            >>> # Configure runner
            >>> runner.setMaxExperiments(10)
            >>> runner.setExperimentTimeout(3600.0)  # 1 hour
            >>> runner.setParallelExecution(True)
            >>> 
            >>> # Add experiments
            >>> runner.addExperiment(plasticity_experiment)
            >>> runner.addExperiment(learning_experiment)
            >>> runner.addExperiment(development_experiment)
            >>> 
            >>> # Run experiments
            >>> runner.run()
            >>> 
            >>> # Get results
            >>> results = runner.getResults()
            >>> print(f"Completed {runner.getCompletedExperiments()} experiments")
            >>> print(f"Total experiments: {runner.getTotalExperiments()}")
        
        Methods:
            __init__(): Create empty experiment runner
            addExperiment(experiment): Add experiment
            removeExperiment(name): Remove experiment
            getExperiments(): Get all experiments
            clearExperiments(): Clear all experiments
            run(): Run all experiments
            isRunning(): Check if runner is running
            getStatus(): Get runner status
            getResults(): Get experiment results
            getCompletedExperiments(): Get completed experiments count
            getFailedExperiments(): Get failed experiments count
            getTotalExperiments(): Get total experiments count
            setMaxExperiments(max): Set maximum experiments
            getMaxExperiments(): Get maximum experiments
            setExperimentTimeout(timeout): Set experiment timeout
            getExperimentTimeout(): Get experiment timeout
            setParallelExecution(enable): Enable/disable parallel execution
            getParallelExecution(): Get parallel execution status
            saveResults(filename): Save results to file
            loadResults(filename): Load results from file
        
        Attributes:
            max_experiments: Maximum number of experiments
            experiment_timeout: Experiment timeout (seconds)
            parallel_execution: Whether to execute experiments in parallel
            status: Runner status
            results: Experiment results
            
        See Also:
            Experiment, ExperimentRunner, Metrics, VisualizationInterface
    )pbdoc")
        .def(py::init<>())
        
        // Experiment management
        .def("addExperiment", &ExperimentRunner::addExperiment,
             py::arg("experiment"),
             "Add experiment")
        
        .def("removeExperiment", &ExperimentRunner::removeExperiment,
             py::arg("name"),
             "Remove experiment")
        
        .def("getExperiments", &ExperimentRunner::getExperiments,
             "Get all experiments")
        
        .def("clearExperiments", &ExperimentRunner::clearExperiments,
             "Clear all experiments")
        
        // Experiment execution
        .def("run", &ExperimentRunner::run,
             "Run all experiments")
        
        .def("isRunning", &ExperimentRunner::isRunning,
             "Check if runner is running")
        
        .def("getStatus", &ExperimentRunner::getStatus,
             "Get runner status")
        
        // Results
        .def("getResults", &ExperimentRunner::getResults,
             "Get experiment results")
        
        .def("getCompletedExperiments", &ExperimentRunner::getCompletedExperiments,
             "Get completed experiments count")
        
        .def("getFailedExperiments", &ExperimentRunner::getFailedExperiments,
             "Get failed experiments count")
        
        .def("getTotalExperiments", &ExperimentRunner::getTotalExperiments,
             "Get total experiments count")
        
        // Configuration
        .def("setMaxExperiments", &ExperimentRunner::setMaxExperiments,
             py::arg("max"),
             "Set maximum experiments")
        
        .def("getMaxExperiments", &ExperimentRunner::getMaxExperiments,
             "Get maximum experiments")
        
        .def("setExperimentTimeout", &ExperimentRunner::setExperimentTimeout,
             py::arg("timeout"),
             "Set experiment timeout")
        
        .def("getExperimentTimeout", &ExperimentRunner::getExperimentTimeout,
             "Get experiment timeout")
        
        .def("setParallelExecution", &ExperimentRunner::setParallelExecution,
             py::arg("enable"),
             "Enable/disable parallel execution")
        
        .def("getParallelExecution", &ExperimentRunner::getParallelExecution,
             "Get parallel execution status")
        
        // File operations
        .def("saveResults", &ExperimentRunner::saveResults, py::arg("filename"),
             "Save results to file")
        
        .def("loadResults", &ExperimentRunner::loadResults, py::arg("filename"),
             "Load results from file")
        
        .def("__repr__", [](const ExperimentRunner& runner) {
            return "<ExperimentRunner: experiments=" + std::to_string(runner.getTotalExperiments()) +
                   ", completed=" + std::to_string(runner.getCompletedExperiments()) +
                   ", status=" + runner.getStatus() + ">";
        }, "String representation of experiment runner");
}

void bindMetrics(py::module& m) {
    // Metrics class - Experiment metrics and statistics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(
        Metrics and statistics for NLM experiments.
        
        The Metrics class provides comprehensive metrics and statistics
        for NLM brain experiments, including:
        - Performance metrics: Speed, efficiency, accuracy
        - Learning metrics: Learning rate, convergence, stability
        - Biological metrics: Synaptic changes, neural activity
        - Experimental metrics: Hypothesis validation, confidence
        - Statistical analysis: Mean, variance, standard deviation
        
        Metrics enable the brain to:
        - Analyze experiment performance
        - Evaluate learning effectiveness
        - Monitor biological processes
        - Validate experimental results
        - Generate statistical reports
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create metrics
            >>> metrics = pynlm.Metrics()
            >>> 
            >>> # Add performance metrics
            >>> metrics.addMetric("speed", 100.0, "ms")
            >>> metrics.addMetric("efficiency", 0.95, "ratio")
            >>> metrics.addMetric("accuracy", 0.87, "ratio")
            >>> 
            >>> # Add learning metrics
            >>> metrics.addMetric("learning_rate", 0.01, "per_step")
            >>> metrics.addMetric("convergence", 50.0, "steps")
            >>> metrics.addMetric("stability", 0.98, "ratio")
            >>> 
            >>> # Add biological metrics
            >>> metrics.addMetric("synaptic_change", 0.15, "normalized")
            >>> metrics.addMetric("neural_activity", 0.72, "ratio")
            >>> 
            >>> # Get summary
            >>> summary = metrics.getSummary()
            >>> print(f"Experiment summary: {summary}")
            >>> 
            >>> # Get specific metric
            >>> speed = metrics.getMetric("speed")
            >>> print(f"Speed: {speed.value} {speed.unit}")
        
        Methods:
            __init__(): Create empty metrics
            addMetric(name, value, unit): Add metric
            getMetric(name): Get metric by name
            updateMetric(name, value): Update metric value
            removeMetric(name): Remove metric
            hasMetric(name): Check if metric exists
            getAllMetrics(): Get all metrics
            clearMetrics(): Clear all metrics
            getMetricCount(): Get number of metrics
            getSummary(): Get metrics summary
            getAverageValue(): Get average value
            getMinValue(): Get minimum value
            getMaxValue(): Get maximum value
            getStandardDeviation(): Get standard deviation
            getMean(): Get mean
            getVariance(): Get variance
            getMedian(): Get median
            getPercentile(percentage): Get percentile
            getMetricNames(): Get all metric names
            getMetricUnits(): Get all metric units
            saveMetrics(filename): Save metrics to file
            loadMetrics(filename): Load metrics from file
        
        Attributes:
            metric_count: Number of metrics
            summary: Metrics summary
            
        See Also:
            Experiment, Metrics, VisualizationInterface, ExperimentRunner
    )pbdoc")
        .def(py::init<>())
        
        // Metric management
        .def("addMetric", &Metrics::addMetric,
             py::arg("name"), py::arg("value"), py::arg("unit"),
             "Add metric")
        
        .def("getMetric", &Metrics::getMetric, py::arg("name"),
             "Get metric by name")
        
        .def("updateMetric", &Metrics::updateMetric,
             py::arg("name"), py::arg("value"),
             "Update metric value")
        
        .def("removeMetric", &Metrics::removeMetric, py::arg("name"),
             "Remove metric")
        
        .def("hasMetric", &Metrics::hasMetric, py::arg("name"),
             "Check if metric exists")
        
        .def("getAllMetrics", &Metrics::getAllMetrics,
             "Get all metrics")
        
        .def("clearMetrics", &Metrics::clearMetrics,
             "Clear all metrics")
        
        .def("getMetricCount", &Metrics::getMetricCount,
             "Get number of metrics")
        
        // Statistical analysis
        .def("getSummary", &Metrics::getSummary,
             "Get metrics summary")
        
        .def("getAverageValue", &Metrics::getAverageValue,
             "Get average value")
        
        .def("getMinValue", &Metrics::getMinValue,
             "Get minimum value")
        
        .def("getMaxValue", &Metrics::getMaxValue,
             "Get maximum value")
        
        .def("getStandardDeviation", &Metrics::getStandardDeviation,
             "Get standard deviation")
        
        .def("getMean", &Metrics::getMean,
             "Get mean")
        
        .def("getVariance", &Metrics::getVariance,
             "Get variance")
        
        .def("getMedian", &Metrics::getMedian,
             "Get median")
        
        .def("getPercentile", &Metrics::getPercentile, py::arg("percentage"),
             "Get percentile")
        
        // Metadata
        .def("getMetricNames", &Metrics::getMetricNames,
             "Get all metric names")
        
        .def("getMetricUnits", &Metrics::getMetricUnits,
             "Get all metric units")
        
        // File operations
        .def("saveMetrics", &Metrics::saveMetrics, py::arg("filename"),
             "Save metrics to file")
        
        .def("loadMetrics", &Metrics::loadMetrics, py::arg("filename"),
             "Load metrics from file")
        
        .def("__repr__", [](const Metrics& metrics) {
            return "<Metrics: metrics=" + std::to_string(metrics.getMetricCount()) + ">";
        }, "String representation of metrics");
}

void bindPhase3Experiment(py::module& m) {
    // Phase3Experiment class - Phase 3 experiment
    py::class_<Phase3Experiment, Experiment>(m, "Phase3Experiment", R"pbdoc(
        Phase 3 experiment for NLM brain.
        
        The Phase3Experiment class implements Phase 3 experiments based on
        the NLM project roadmap:
        - Real neural computation: LIF neuron dynamics
        - Event-driven dynamics: Spike propagation with delays
        - STDP and Hebbian plasticity: Synaptic learning rules
        - Structural plasticity: Synaptogenesis and pruning
        - Learning experiments: Demonstrating measurable changes
        
        Phase 3 experiments enable the brain to:
        - Test real neural computation
        - Demonstrate synaptic plasticity
        - Show learning through experience
        - Validate biological plausibility
        - Establish experimental foundations
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create Phase 3 experiment
            >>> phase3_exp = pynlm.Phase3Experiment()
            >>> 
            >>> # Configure experiment
            >>> phase3_exp.setExperimentName("basic_connectivity")
            >>> phase3_exp.setHypothesis("Neurons can establish connections")
            >>> 
            >>> # Run experiment
            >>> phase3_exp.run()
            >>> 
            >>> # Get results
            >>> results = phase3_exp.getResults()
            >>> print(f"Connectivity: {results.getConnectivity()}")
            >>> print(f"Learning detected: {results.isLearningDetected()}")
        
        Methods:
            __init__(): Create empty Phase 3 experiment
            run(): Run Phase 3 experiment
            getExperimentType(): Get experiment type
            getExpectedResults(): Get expected results
            validateResults(results): Validate experiment results
        
        Attributes:
            experiment_type: Experiment type ("phase3")
            
        See Also:
            Experiment, Phase4Experiment, VisualizationInterface, Metrics
    )pbdoc")
        .def(py::init<>())
        
        // Experiment execution
        .def("run", &Phase3Experiment::run,
             "Run Phase 3 experiment")
        
        // Experiment information
        .def("getExperimentType", &Phase3Experiment::getExperimentType,
             "Get experiment type")
        
        .def("getExpectedResults", &Phase3Experiment::getExpectedResults,
             "Get expected results")
        
        .def("validateResults", &Phase3Experiment::validateResults,
             py::arg("results"),
             "Validate experiment results");
}

void bindPhase4Experiment(py::module& m) {
    // Phase4Experiment class - Phase 4 experiment
    py::class_<Phase4Experiment, Experiment>(m, "Phase4Experiment", R"pbdoc(
        Phase 4 experiment for NLM brain.
        
        The Phase4Experiment class implements Phase 4 experiments based on
        the NLM project roadmap:
        - Performance optimizations: Memory pools and event queues
        - SIMD vectorization: Parallel processing optimization
        - Parallel processing: Multi-threading and concurrency
        - Checkpoint system: State persistence and recovery
        - Learning experiments: Complex learning behaviors
        
        Phase 4 experiments enable the brain to:
        - Test performance optimizations
        - Demonstrate parallel processing
        - Show checkpoint system functionality
        - Validate performance improvements
        - Establish scalability foundations
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create Phase 4 experiment
            >>> phase4_exp = pynlm.Phase4Experiment()
            >>> 
            >>> # Configure experiment
            >>> phase4_exp.setExperimentName("performance_test")
            >>> phase4_exp.setHypothesis("Performance optimizations improve speed")
            >>> 
            >>> # Run experiment
            >>> phase4_exp.run()
            >>> 
            >>> # Get results
            >>> results = phase4_exp.getResults()
            >>> print(f"Performance improvement: {results.getPerformanceImprovement():.2f}x")
            >>> print(f"Memory efficiency: {results.getMemoryEfficiency():.2f}x")
        
        Methods:
            __init__(): Create empty Phase 4 experiment
            run(): Run Phase 4 experiment
            getExperimentType(): Get experiment type
            getExpectedResults(): Get expected results
            validateResults(results): Validate experiment results
        
        Attributes:
            experiment_type: Experiment type ("phase4")
            
        See Also:
            Experiment, Phase3Experiment, VisualizationInterface, Metrics
    )pbdoc")
        .def(py::init<>())
        
        // Experiment execution
        .def("run", &Phase4Experiment::run,
             "Run Phase 4 experiment")
        
        // Experiment information
        .def("getExperimentType", &Phase4Experiment::getExperimentType,
             "Get experiment type")
        
        .def("getExpectedResults", &Phase4Experiment::getExpectedResults,
             "Get expected results")
        
        .def("validateResults", &Phase4Experiment::validateResults,
             py::arg("results"),
             "Validate experiment results");
}
