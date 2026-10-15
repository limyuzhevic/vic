// Advanced Experiment Tests
// Advanced experiment framework tests

#include "experiment/AdvancedExperiment.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

namespace test_advanced_experiment {

void testAdvancedExperimentCreation() {
    nlm::AdvancedExperiment experiment;
    
    std::cout << "    testAdvancedExperimentCreation passed" << std::endl;
}

void testAdvancedExperimentInitialize() {
    nlm::AdvancedExperiment experiment;
    
    // Initialize experiment
    nlm::Config config;
    bool success = experiment.initialize(config);
    
    std::cout << "    testAdvancedExperimentInitialize passed" << std::endl;
}

void testAdvancedExperimentUpdate() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test update with timestep
    experiment.update(0.1);
    
    std::cout << "    testAdvancedExperimentUpdate passed" << std::endl;
}

void testAdvancedExperimentShutdown() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test shutdown
    experiment.shutdown();
    
    std::cout << "    testAdvancedExperimentShutdown passed" << std::endl;
}

void testAdvancedExperimentProtocol() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test protocol management
    std::vector<std::string> protocols = experiment.getAvailableProtocols();
    
    std::cout << "    testAdvancedExperimentProtocol passed" << std::endl;
}

void testAdvancedExperimentHyperparameter() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test hyperparameter management
    bool result = experiment.addHyperparameter("learning_rate", std::vector<double>{0.001, 0.01, 0.1});
    
    std::cout << "    testAdvancedExperimentHyperparameter passed" << std::endl;
}

void testAdvancedExperimentOptimization() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test optimization objective
    bool result = experiment.addOptimizationObjective("accuracy", 
        [](const nlm::Config& c) { return 0.85; });
    
    std::cout << "    testAdvancedExperimentOptimization passed" << std::endl;
}

void testAdvancedExperimentMetrics() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test metrics calculation
    nlm::ExperimentData data;
    data.progress = 0.75;
    data.completed = true;
    
    auto metrics = experiment.calculateAllMetrics(data);
    
    std::cout << "    testAdvancedExperimentMetrics passed" << std::endl;
}

void testAdvancedExperimentRun() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test running experiment (should fail for now, no protocols)
    bool result = experiment.run("non_existent_protocol");
    
    std::cout << "    testAdvancedExperimentRun passed" << std::endl;
}

void testAdvancedExperimentStatus() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test status reporting
    std::string status = experiment.getExperimentStatus();
    
    assert(!status.empty());
    
    std::cout << "    testAdvancedExperimentStatus passed" << std::endl;
}

void testAdvancedExperimentAnalytics() {
    nlm::AdvancedExperiment experiment;
    nlm::Config config;
    experiment.initialize(config);
    
    // Test analytics
    std::string analytics = experiment.getExperimentAnalytics();
    
    assert(!analytics.empty());
    
    std::cout << "    testAdvancedExperimentAnalytics passed" << std::endl;
}

void runAll() {
    testAdvancedExperimentCreation();
    testAdvancedExperimentInitialize();
    testAdvancedExperimentUpdate();
    testAdvancedExperimentShutdown();
    testAdvancedExperimentProtocol();
    testAdvancedExperimentHyperparameter();
    testAdvancedExperimentOptimization();
    testAdvancedExperimentMetrics();
    testAdvancedExperimentRun();
    testAdvancedExperimentStatus();
    testAdvancedExperimentAnalytics();
}

} // namespace test_advanced_experiment

} // namespace nlm
namespace tests {

void test_advanced_experiment() {
    test_advanced_experiment::runAll();
}

} // namespace tests
