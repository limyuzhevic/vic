// Experiment system bindings

#include "bindings.h"

namespace nlm {

void bind_experiments_module(py::module_& m) {
    // Experiment class
    py::class_<Experiment>(m, "Experiment", R"pbdoc(
        Base class for NLM experiments.
        
        Experiment provides:
        - Experimental protocol definition
        - Data collection and storage
        - Results analysis
        - Experiment execution
        
        Key features:
        - Protocol-based execution
        - Data logging
        - Result aggregation
        - Statistical analysis
    )pbdoc")
        .def(py::init<>())
        .def("run", &Experiment::run, "Run the experiment")
        .def("stop", &Experiment::stop, "Stop the experiment")
        .def("isRunning", &Experiment::isRunning, "Check if experiment is running")
        .def("getResults", &Experiment::getResults, py::return_value_policy::reference_internal,
             "Get experiment results")
        .def("getProgress", &Experiment::getProgress, "Get experiment progress (0.0 to 1.0)")
        .def("getEpoch", &Experiment::getEpoch, "Get current epoch")
        .def("getStepCount", &Experiment::getStepCount, "Get total step count")
        .def("getStep", &Experiment::getStep, py::arg("index"),
             py::return_value_policy::reference_internal, "Get specific step")
        .def("clearResults", &Experiment::clearResults, "Clear experiment results");

    // ExperimentRunner class
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(
        Experiment runner for executing NLM experiments.
        
        ExperimentRunner provides:
        - Multiple experiment management
        - Experiment scheduling
        - Data collection
        - Results aggregation
        
        Key features:
        - Sequential experiment execution
        - Parallel experiment support
        - Results aggregation
        - Progress tracking
    )pbdoc")
        .def(py::init<>())
        .def("addExperiment", &ExperimentRunner::addExperiment, py::arg("experiment"),
             "Add experiment to runner")
        .def("removeExperiment", &ExperimentRunner::removeExperiment, py::arg("name"),
             "Remove experiment by name")
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("name"),
             "Run specific experiment")
        .def("runAllExperiments", &ExperimentRunner::runAllExperiments, "Run all experiments")
        .def("stopAllExperiments", &ExperimentRunner::stopAllExperiments, "Stop all experiments")
        .def("getExperiment", &ExperimentRunner::getExperiment, py::arg("name"),
             py::return_value_policy::reference_internal, "Get experiment by name")
        .def("getExperiments", &ExperimentRunner::getExperiments, py::return_value_policy::reference_internal,
             "Get all experiments")
        .def("getResults", &ExperimentRunner::getResults, py::return_value_policy::reference_internal,
             "Get all experiment results")
        .def("clearResults", &ExperimentRunner::clearResults, "Clear all results")
        .def("exportResults", &ExperimentRunner::exportResults, py::arg("filename"),
             "Export all results to file");

    // AblationSystem class
    py::class_<AblationSystem>(m, "AblationSystem", R"pbdoc(
        Ablation system for testing brain component importance.
        
        AblationSystem provides:
        - Component removal for testing
        - Baseline comparison
        - Differential analysis
        - Component importance assessment
        
        Key features:
        - Remove specific brain components
        - Compare with intact system
        - Analyze component contributions
        - Generate ablation reports
    )pbdoc")
        .def(py::init<>())
        .def("ablateComponent", &AblationSystem::ablateComponent, py::arg("component"),
             "Ablate a brain component")
        .def("restoreComponent", &AblationSystem::restoreComponent, py::arg("component"),
             "Restore ablated component")
        .def("isComponentAblated", &AblationSystem::isComponentAblated, py::arg("component"),
             "Check if component is ablated")
        .def("getAblatedComponents", &AblationSystem::getAblatedComponents,
             py::return_value_policy::reference_internal, "Get list of ablated components")
        .def("runAblationStudy", &AblationSystem::runAblationStudy, py::arg("brain"),
             "Run ablation study on brain")
        .def("getComparisonResults", &AblationSystem::getComparisonResults,
             py::return_value_policy::reference_internal, "Get ablation comparison results")
        .def("clear", &AblationSystem::clear, "Clear all ablations");

    // Phase3Experiment class
    py::class_<Phase3Experiment>(m, "Phase3Experiment", R"pbdoc(
        Phase 3 experiment testing world interaction loop.
        
        Phase3Experiment provides:
        - World interaction testing
        - Reward prediction error testing
        - Developmental stage testing
        - Novelty and curiosity testing
        
        Key features:
        - Test core world integration
        - Validate reward systems
        - Test developmental processes
        - Test motivation systems
    )pbdoc")
        .def(py::init<>())
        .def("runWorldInteractionTest", &Phase3Experiment::runWorldInteractionTest,
             "Run world interaction test")
        .def("runRewardPredictionErrorTest", &Phase3Experiment::runRewardPredictionErrorTest,
             "Run reward prediction error test")
        .def("runDevelopmentTest", &Phase3Experiment::runDevelopmentTest,
             "Run developmental stage test")
        .def("runMotivationTest", &Phase3Experiment::runMotivationTest,
             "Run novelty and curiosity test")
        .def("getWorldInteractionResults", &Phase3Experiment::getWorldInteractionResults,
             py::return_value_policy::reference_internal, "Get world interaction results")
        .def("getRPErrors", &Phase3Experiment::getRPErrors, py::return_value_policy::reference_internal,
             "Get reward prediction errors")
        .def("getDevelopmentMetrics", &Phase3Experiment::getDevelopmentMetrics,
             py::return_value_policy::reference_internal, "Get development metrics")
        .def("getMotivationMetrics", &Phase3Experiment::getMotivationMetrics,
             py::return_value_policy::reference_internal, "Get motivation metrics");

    // Phase4Experiment class
    py::class_<Phase4Experiment>(m, "Phase4Experiment", R"pbdoc(
        Phase 4 experiment testing cognitive systems.
        
        Phase4Experiment provides:
        - Neural prediction testing
        - Working memory testing
        - Episodic memory testing
        - Concept formation testing
        
        Key features:
        - Test prediction systems
        - Validate memory systems
        - Test cognitive functions
        - Validate attention systems
    )pbdoc")
        .def(py::init<>())
        .def("runPredictionTest", &Phase4Experiment::runPredictionTest, "Run prediction test")
        .def("runWorkingMemoryTest", &Phase4Experiment::runWorkingMemoryTest,
             "Run working memory test")
        .def("runEpisodicMemoryTest", &Phase4Experiment::runEpisodicMemoryTest,
             "Run episodic memory test")
        .def("runConceptFormationTest", &Phase4Experiment::runConceptFormationTest,
             "Run concept formation test")
        .def("getPredictionResults", &Phase4Experiment::getPredictionResults,
             py::return_value_policy::reference_internal, "Get prediction results")
        .def("getWorkingMemoryResults", &Phase4Experiment::getWorkingMemoryResults,
             py::return_value_policy::reference_internal, "Get working memory results")
        .def("getEpisodicMemoryResults", &Phase4Experiment::getEpisodicMemoryResults,
             py::return_value_policy::reference_internal, "Get episodic memory results")
        .def("getConceptFormationResults", &Phase4Experiment::getConceptFormationResults,
             py::return_value_policy::reference_internal, "Get concept formation results");

    // Phase5Experiment class
    py::class_<Phase5Experiment>(m, "Phase5Experiment", R"pbdoc(
        Phase 5 experiment testing performance optimizations.
        
        Phase5Experiment provides:
        - Performance optimization testing
        - Memory pool testing
        - Event queue testing
        - Parallel processing testing
        
        Key features:
        - Test performance optimizations
        - Validate memory management
        - Test event processing
        - Test parallel execution
    )pbdoc")
        .def(py::init<>())
        .def("runPerformanceOptimizationTest", &Phase5Experiment::runPerformanceOptimizationTest,
             "Run performance optimization test")
        .def("runMemoryPoolTest", &Phase5Experiment::runMemoryPoolTest, "Run memory pool test")
        .def("runEventQueueTest", &Phase5Experiment::runEventQueueTest, "Run event queue test")
        .def("runParallelProcessingTest", &Phase5Experiment::runParallelProcessingTest,
             "Run parallel processing test")
        .def("getPerformanceMetrics", &Phase5Experiment::getPerformanceMetrics,
             py::return_value_policy::reference_internal, "Get performance metrics")
        .def("getMemoryPoolMetrics", &Phase5Experiment::getMemoryPoolMetrics,
             py::return_value_policy::reference_internal, "Get memory pool metrics")
        .def("getEventQueueMetrics", &Phase5Experiment::getEventQueueMetrics,
             py::return_value_policy::reference_internal, "Get event queue metrics")
        .def("getParallelProcessingMetrics", &Phase5Experiment::getParallelProcessingMetrics,
             py::return_value_policy::reference_internal, "Get parallel processing metrics");

    // Phase6IntegratedExperiment class
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(
        Phase 6 integrated experiment testing final integration.
        
        Phase6IntegratedExperiment provides:
        - Complete system integration testing
        - Cross-component interaction testing
        - End-to-end simulation testing
        - Final validation
        
        Key features:
        - Test all system integrations
        - Validate cross-component interactions
        - Run complete simulation
        - Perform final validation
    )pbdoc")
        .def(py::init<>())
        .def("runIntegrationTest", &Phase6IntegratedExperiment::runIntegrationTest,
             "Run integration test")
        .def("runCrossComponentTest", &Phase6IntegratedExperiment::runCrossComponentTest,
             "Run cross-component interaction test")
        .def("runEndToEndTest", &Phase6IntegratedExperiment::runEndToEndTest,
             "Run end-to-end simulation test")
        .def("getIntegrationResults", &Phase6IntegratedExperiment::getIntegrationResults,
             py::return_value_policy::reference_internal, "Get integration results")
        .def("getCrossComponentResults", &Phase6IntegratedExperiment::getCrossComponentResults,
             py::return_value_policy::reference_internal, "Get cross-component results")
        .def("getEndToEndResults", &Phase6IntegratedExperiment::getEndToEndResults,
             py::return_value_policy::reference_internal, "Get end-to-end results");

    // ScalingBenchmark class
    py::class_<ScalingBenchmark>(m, "ScalingBenchmark", R"pbdoc(
        Performance scaling benchmark.
        
        ScalingBenchmark provides:
        - Performance benchmarking
        - Scaling analysis
        - Resource usage measurement
        - Performance optimization guidance
        
        Key features:
        - Benchmark neural processing performance
        - Analyze scaling characteristics
        - Measure resource consumption
        - Guide performance optimization
    )pbdoc")
        .def(py::init<>())
        .def("runScalingTest", &ScalingBenchmark::runScalingTest,
             py::arg("neuronCounts"), py::arg("timesteps"),
             "Run scaling test with different neuron counts")
        .def("getBenchmarkResults", &ScalingBenchmark::getBenchmarkResults,
             py::return_value_policy::reference_internal, "Get benchmark results")
        .def("exportResults", &ScalingBenchmark::exportResults, py::arg("filename"),
             "Export benchmark results to file")
        .def("analyzeScaling", &ScalingBenchmark::analyzeScaling,
             "Analyze scaling characteristics")
        .def("getPerformancePerNeuron", &ScalingBenchmark::getPerformancePerNeuron,
             py::return_value_policy::reference_internal, "Get performance per neuron");
}

} // namespace nlm