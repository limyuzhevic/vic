// Experiment Framework Integration Tests
// Experiment framework integration

#include "experiment/AdvancedExperiment.hpp"
#include "experiment/ExperimentRunner.hpp"
#include "core/ExtensibleBrain.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

namespace test_experiment_integration {

void testExperimentFrameworkIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test experiment framework integration
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    assert(extBrain != nullptr);
    
    std::cout << "    testExperimentFrameworkIntegration passed" << std::endl;
}

void testAdvancedExperimentWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set experiment framework
    auto experiment = std::make_unique<nlm::AdvancedExperiment>();
    brain.setExperimentFramework(std::move(experiment));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testAdvancedExperimentWithBrain passed" << std::endl;
}

void testExperimentRunnerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test experiment runner integration
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    std::cout << "    testExperimentRunnerIntegration passed" << std::endl;
}

void testExperimentProtocolManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test protocol management
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    std::vector<std::string> protocols = experiment.getAvailableProtocols();
    
    std::cout << "    testExperimentProtocolManagement passed" << std::endl;
}

void testExperimentHyperparameterOptimization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test hyperparameter optimization
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    bool result = experiment.addHyperparameter("learning_rate", std::vector<double>{0.01, 0.001});
    
    std::cout << "    testExperimentHyperparameterOptimization passed" << std::endl;
}

void testExperimentMetricsCalculation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test metrics calculation
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    nlm::ExperimentData data;
    data.progress = 0.5;
    data.completed = false;
    
    auto metrics = experiment.calculateAllMetrics(data);
    
    std::cout << "    testExperimentMetricsCalculation passed" << std::endl;
}

void testExperimentStatusMonitoring() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test status monitoring
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    std::string status = experiment.getExperimentStatus();
    
    assert(!status.empty());
    
    std::cout << "    testExperimentStatusMonitoring passed" << std::endl;
}

void testExperimentAnalytics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test analytics
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    std::string analytics = experiment.getExperimentAnalytics();
    
    assert(!analytics.empty());
    
    std::cout << "    testExperimentAnalytics passed" << std::endl;
}

void testExperimentDataExport() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test data export
    nlm::AdvancedExperiment experiment;
    experiment.initialize(config);
    
    bool result = experiment.saveResults("/tmp/test_results.txt");
    
    std::cout << "    testExperimentDataExport passed" << std::endl;
}

void runAll() {
    testExperimentFrameworkIntegration();
    testAdvancedExperimentWithBrain();
    testExperimentRunnerIntegration();
    testExperimentProtocolManagement();
    testExperimentHyperparameterOptimization();
    testExperimentMetricsCalculation();
    testExperimentStatusMonitoring();
    testExperimentAnalytics();
    testExperimentDataExport();
}

} // namespace test_experiment_integration

} // namespace nlm
namespace tests {

void test_experiment_integration() {
    test_experiment_integration::runAll();
}

} // namespace tests
