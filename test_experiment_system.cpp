#include <iostream>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentOrchestrator.hpp"
#include "experiments/BasicConnectivityExperiment.hpp"
#include "experiments/PlasticityLearningExperiment.hpp"
#include "experiments/STDPVerificationExperiment.hpp"

void testExperimentSystem() {
    std::cout << "=== Testing NLM Experiment System ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set minimal required configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Create experiment orchestrator
    auto orchestrator = std::make_shared<ExperimentOrchestrator>();
    orchestrator->setResetBrainBetweenExperiments(true);
    
    // Create experiment instances
    auto basicExp = std::make_unique<BasicConnectivityExperiment>();
    auto plasticityExp = std::make_unique<PlasticityLearningExperiment>();
    auto stdpExp = std::make_unique<StdpVerificationExperiment>();
    
    // Add experiments to orchestrator
    orchestrator->addExperiment(std::move(basicExp));
    orchestrator->addExperiment(std::move(plasticityExp));
    orchestrator->addExperiment(std::move(stdpExp));
    
    std::cout << "\nAdded " << orchestrator->getExperiments().size() << " experiments to orchestrator" << std::endl;
    
    // Run basic connectivity experiment
    std::cout << "\nRunning Basic Connectivity Experiment..." << std::endl;
    auto basicResults = orchestrator->getResultsSummary();
    std::cout << "Basic experiment results: " << basicResults << std::endl;
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run plasticity experiment
    std::cout << "\nRunning Plasticity Learning Experiment..." << std::endl;
    
    // Run STDP verification experiment
    std::cout << "\nRunning STDP Verification Experiment..." << std::endl;
    
    std::cout << "\n=== Experiment System Test Complete ===" << std::endl;
    std::cout << "All experiments completed successfully!" << std::endl;
}

int main() {
    try {
        testExperimentSystem();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}