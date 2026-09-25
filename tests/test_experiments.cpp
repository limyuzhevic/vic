// Experiments Tests
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_experiments {

TEST_CASE("Experiment creation and initialization", "[experiments][experiment][creation]") {
    nlm::Experiment experiment;
    
    REQUIRE(!experiment.isInitialized());
}

TEST_CASE("Experiment initialization with config", "[experiments][experiment][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("experiment.name", "test_experiment", nlm::ConfigSource::Runtime);
    config->set("experiment.type", "neuroscience", nlm::ConfigSource::Runtime);
    config->set("experiment.duration", 100, nlm::ConfigSource::Runtime);
    
    nlm::Experiment experiment;
    experiment.initialize(config);
    REQUIRE(experiment.isInitialized());
}

TEST_CASE("ExperimentRunner creation and basic functionality", "[experiments][runner][creation]") {
    nlm::ExperimentRunner runner;
    
    REQUIRE(runner.getExperimentCount() == 0);
    REQUIRE(!runner.isRunning());
}

TEST_CASE("ExperimentRunner experiment management", "[experiments][runner][management]") {
    nlm::ExperimentRunner runner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("experiment.name", "managed_experiment", nlm::ConfigSource::Runtime);
    config->set("experiment.type", "learning", nlm::ConfigSource::Runtime);
    
    nlm::Experiment experiment;
    experiment.initialize(config);
    
    runner.addExperiment(experiment);
    REQUIRE(runner.getExperimentCount() == 1);
}

TEST_CASE("Metrics collection and analysis", "[experiments][metrics][collection]") {
    nlm::Metrics metrics;
    
    REQUIRE(metrics.getMetricCount() == 0);
    
    metrics.addMetric("accuracy", 0.95f);
    metrics.addMetric("precision", 0.87f);
    metrics.addMetric("recall", 0.92f);
    
    REQUIRE(metrics.getMetricCount() == 3);
    REQUIRE(metrics.getMetric("accuracy") == 0.95f);
    REQUIRE(metrics.getMetric("precision") == 0.87f);
    REQUIRE(metrics.getMetric("recall") == 0.92f);
}

TEST_CASE("ExperimentRunner execution and results", "[experiments][runner][execution]") {
    nlm::ExperimentRunner runner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("experiment.name", "execution_test", nlm::ConfigSource::Runtime);
    config->set("experiment.type", "behavioral", nlm::ConfigSource::Runtime);
    config->set("experiment.duration", 10, nlm::ConfigSource::Runtime);
    
    nlm::Experiment experiment;
    experiment.initialize(config);
    
    runner.addExperiment(experiment);
    
    // Run experiments
    runner.runAllExperiments();
    
    REQUIRE(runner.getExperimentCount() == 1);
}

TEST_CASE("Experiment with different parameters", "[experiments][config][variations]") {
    // Test experiments with different parameters
    std::vector<std::string> experimentTypes = {"neural", "cognitive", "behavioral", "perceptual"};
    
    for (const auto& expType : experimentTypes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("experiment.name", "variation_test", nlm::ConfigSource::Runtime);
        config->set("experiment.type", expType, nlm::ConfigSource::Runtime);
        config->set("experiment.duration", 5, nlm::ConfigSource::Runtime);
        
        nlm::Experiment experiment;
        experiment.initialize(config);
        
        REQUIRE(experiment.isInitialized());
    }
}

TEST_CASE("Experiment metrics tracking and reporting", "[experiments][metrics][tracking]") {
    nlm::Experiment experiment;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("experiment.name", "metrics_test", nlm::ConfigSource::Runtime);
    config->set("experiment.type", "comprehensive", nlm::ConfigSource::Runtime);
    
    experiment.initialize(config);
    
    nlm::Metrics metrics;
    
    // Track various metrics
    metrics.addMetric("learning_rate", 0.001f);
    metrics.addMetric("convergence_time", 100.0f);
    metrics.addMetric("performance_score", 0.85f);
    
    REQUIRE(metrics.getMetricCount() == 3);
    
    // Generate report
    std::string report = metrics.generateReport();
    REQUIRE(!report.empty());
}

TEST_CASE("ExperimentRunner with multiple experiments", "[experiments][runner][multiple]") {
    nlm::ExperimentRunner runner;
    
    // Add multiple experiments
    for (int i = 0; i < 5; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("experiment.name", "experiment_" + std::to_string(i), nlm::ConfigSource::Runtime);
        config->set("experiment.type", "type_" + std::to_string(i), nlm::ConfigSource::Runtime);
        config->set("experiment.duration", 3, nlm::ConfigSource::Runtime);
        
        nlm::Experiment experiment;
        experiment.initialize(config);
        
        runner.addExperiment(experiment);
    }
    
    REQUIRE(runner.getExperimentCount() == 5);
    
    // Run all experiments
    runner.runAllExperiments();
    
    // All experiments should be completed
    REQUIRE(runner.getCompletedExperimentCount() == 5);
}

TEST_CASE("ExperimentRunner pause and resume functionality", "[experiments][runner][pause_resume]") {
    nlm::ExperimentRunner runner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("experiment.name", "pause_test", nlm::ConfigSource::Runtime);
    config->set("experiment.type", "runnable", nlm::ConfigSource::Runtime);
    
    nlm::Experiment experiment;
    experiment.initialize(config);
    
    runner.addExperiment(experiment);
    
    REQUIRE(runner.getExperimentCount() == 1);
    
    // Pause experiment
    runner.pauseExperiment(0);
    REQUIRE(runner.isExperimentPaused(0));
    
    // Resume experiment
    runner.resumeExperiment(0);
    REQUIRE(!runner.isExperimentPaused(0));
}

} // namespace test_experiments
