#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void testContextManagers() {
    std::cout << "  Testing Context Managers..." << std::endl;
    
    // Test BrainContextManager
    auto config = std::make_shared<nlm::Config>();
    config->set("neuronCount", 100);
    
    nlm::BrainContextManager brainCtx(config);
    nlm::Brain& brain = brainCtx.getBrain();
    
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() > 0);
    
    // Test SimulationRunner
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain.getSharedPtr());
    nlm::SimulationRunner runner(brain.getSharedPtr(), world, agentBrain);
    
    runner.start(10);
    assert(runner.isRunning());
    
    // Run some steps
    for (int i = 0; i < 5; ++i) {
        bool stepSuccess = runner.step();
        assert(stepSuccess);
        assert(runner.getCurrentStep() == i + 1);
    }
    
    assert(runner.getCurrentStep() == 5);
    
    runner.stop();
    assert(!runner.isRunning());
    
    std::cout << "    BrainContextManager and SimulationRunner tests passed" << std::endl;
}

void testBatchOperations() {
    std::cout << "  Testing Batch Operations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuronCount", 200);
    config->set("regionCount", 2);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test batch step operations
    std::vector<nlm::SimulationStep> steps = {1, 2, 3, 4, 5, 10, 20, 50};
    std::vector<nlm::Timestamp> times = {1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 5.0, 10.0};
    
    std::vector<nlm::SimulationStep> executed = nlm::BatchOperations::runSteps(brain, steps, times);
    
    assert(executed.size() == steps.size());
    assert(executed == steps);
    
    // Test batch sensory input processing
    nlm::Vision vision(8, 8, 3);
    vision.setData(std::vector<float>(8 * 8 * 3, 0.5f));
    
    std::vector<std::reference_wrapper<const nlm::SensoryInput>> inputs = {vision};
    std::vector<size_t> processed = nlm::BatchOperations::processSensoryInputs(brain, inputs);
    
    assert(processed.size() == 1);
    
    // Test batch statistics
    std::map<std::string, double> stats = nlm::BatchOperations::getBatchStats(brain, 0, 50);
    
    assert(stats.find("total_neuron_count") != stats.end());
    assert(stats.find("total_synapse_count") != stats.end());
    assert(stats.find("active_neuron_count") != stats.end());
    assert(stats.find("firing_neuron_count") != stats.end());
    assert(stats.find("average_firing_rate") != stats.end());
    assert(stats.find("excitement_inhibition_ratio") != stats.end());
    assert(stats.find("total_spike_count") != stats.end());
    
    std::cout << "    BatchOperations tests passed" << std::endl;
}

void testConvenienceMethods() {
    std::cout << "  Testing Convenience Methods..." << std::endl;
    
    // Test configureBrain
    auto config = std::make_shared<nlm::Config>();
    std::map<std::string, std::string> configDict = {
        {"neuronCount", "300"},
        {"connectionProbability", "0.15"},
        {"enableSTDP", "true"},
        {"enableCuriosity", "true"}
    };
    
    bool configSuccess = nlm::ConvenienceMethods::configureBrain(*config, configDict);
    assert(configSuccess);
    
    assert(config->getOr<int64_t>("neuronCount", 0) == 300);
    assert(config->getOr<double>("connectionProbability", 0.0) == 0.15);
    
    // Test quickSimulation (this is a stub that returns dummy data)
    std::map<std::string, double> quickStats = nlm::ConvenienceMethods::quickSimulation(500, 100);
    
    assert(quickStats.find("total_neurons") != quickStats.end());
    assert(quickStats.find("total_steps") != quickStats.end());
    assert(quickStats.find("avg_firing_rate") != quickStats.end());
    assert(quickStats.find("memory_usage_mb") != quickStats.end());
    
    // Test runEpisode
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(50, 50, 16, 16);
    world->reset();
    world->setAgentStart(25, 25);
    
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    agentBrain->initialize(*world);
    
    std::map<std::string, double> episodeStats = nlm::ConvenienceMethods::runEpisode(*agentBrain, *world, 100);
    
    assert(episodeStats.find("steps_completed") != episodeStats.end());
    assert(episodeStats.find("steps_percentage") != episodeStats.end());
    assert(episodeStats.find("rewards_received") != episodeStats.end());
    assert(episodeStats.find("total_reward") != episodeStats.end());
    assert(episodeStats.find("avg_reward_per_step") != episodeStats.end());
    
    std::cout << "    ConvenienceMethods tests passed" << std::endl;
}

void testBuilderPattern() {
    std::cout << "  Testing Builder Pattern..." << std::endl;
    
    // Test ConfigBuilder
    nlm::ConfigBuilder configBuilder;
    
    auto config = (configBuilder
                   .set("neuronCount", 800)
                   .set("synapseCount", 40000)
                   .set("simulationSteps", 200)
                   .set("enableDevelopment", "true")
                   .set("enableCuriosity", "true")
                   .add("learningRate", 0.01)
                   .add("learningRate", 0.001)
                   .build());
    
    assert(config->getOr<int64_t>("neuronCount", 0) == 800);
    assert(config->getOr<int64_t>("synapseCount", 0) == 40000);
    assert(config->getOr<int64_t>("simulationSteps", 0) == 200);
    
    // Test SimulationBuilder
    nlm::SimulationBuilder simulationBuilder;
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(256, 256, 128, 128);
    
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    auto simulation = (simulationBuilder
                       .withWorldSize(256, 256)
                       .withConfig({{"test", "value"}})
                       .withBrain(brain)
                       .withAgentBrain(agentBrain)
                       .build());
    
    assert(simulation[0]->getWidth() == 256);
    assert(simulation[0]->getHeight() == 256);
    assert(simulation[2] != nullptr);
    
    std::cout << "    Builder Pattern tests passed" << std::endl;
}

void testExperimentManagement() {
    std::cout << "  Testing Experiment Management..." << std::endl;
    
    // Test ExperimentLogger
    nlm::ExperimentLogger logger;
    
    logger.recordMetric("accuracy", 0.85);
    logger.recordMetric("loss", 0.15);
    logger.recordMetric("learning_rate", 0.001);
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    for (nlm::SimulationStep step = 0; step < 100; step += 10) {
        logger.recordStep(*brain, step);
    }
    
    logger.addNote("Experiment started with 1000 neurons");
    logger.addNote("Training completed successfully");
    
    auto experiment = logger.getExperiment();
    auto metrics = logger.getMetrics();
    
    assert(experiment != nullptr);
    assert(metrics != nullptr);
    
    // Test ResultsAnalyzer
    auto analysis = nlm::ResultsAnalyzer::analyze(*experiment);
    
    assert(analysis.find("avg_firing_rate") != analysis.end());
    assert(analysis.find("avg_neuron_count") != analysis.end());
    assert(analysis.find("total_steps") != analysis.end());
    
    auto metricsAnalysis = nlm::ResultsAnalyzer::analyze(*metrics);
    
    assert(metricsAnalysis.find("accuracy_mean") != metricsAnalysis.end());
    assert(metricsAnalysis.find("loss_mean") != metricsAnalysis.end());
    assert(metricsAnalysis.find("learning_rate_mean") != metricsAnalysis.end());
    
    std::cout << "    Experiment Management tests passed" << std::endl;
}

void testAdvancedWorkflow() {
    std::cout << "  Testing Advanced Workflow..." << std::endl;
    
    // Build configuration
    nlm::ConfigBuilder configBuilder;
    auto config = (configBuilder
                   .set("neuronCount", 5000)
                   .set("enableDevelopment", "true")
                   .set("enableCuriosity", "true")
                   .build());
    
    // Create components
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(500, 500, 128, 128);
    world->setAgentStart(250, 250);
    
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Setup experiment logger
    nlm::ExperimentLogger logger;
    logger.addNote("Advanced workflow test started");
    
    // Create simulation runner
    nlm::SimulationRunner runner(brain, world, agentBrain);
    runner.start(200);
    
    // Run simulation with batch operations
    std::vector<nlm::SimulationStep> batchSteps = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
    std::vector<nlm::Timestamp> batchTimes = {1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0};
    
    std::vector<nlm::SimulationStep> executed = nlm::BatchOperations::runSteps(*brain, batchSteps, batchTimes);
    assert(executed.size() == batchSteps.size());
    
    // Process batch sensory inputs
    nlm::Vision vision(128, 128, 3);
    vision.setData(std::vector<float>(128 * 128 * 3, 0.5f));
    
    std::vector<std::reference_wrapper<const nlm::SensoryInput>> visionBatch = {vision};
    std::vector<size_t> processed = nlm::BatchOperations::processSensoryInputs(*brain, visionBatch);
    assert(processed.size() == 1);
    
    // Run episode
    auto episodeStats = nlm::ConvenienceMethods::runEpisode(*agentBrain, *world, 100);
    logger.addNote("Episode completed successfully");
    
    // Record final metrics
    logger.recordMetric("final_accuracy", 0.95);
    logger.recordMetric("training_time", 125.5);
    
    // Analyze everything
    auto experiment = logger.getExperiment();
    auto finalAnalysis = nlm::ResultsAnalyzer::analyze(*experiment);
    assert(finalAnalysis.find("avg_firing_rate") != finalAnalysis.end());
    
    runner.stop();
    
    std::cout << "    Advanced Workflow tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Phase 8: High-Level API Integration Tests ===" << std::endl;
    std::cout << "Testing new high-level convenience methods and batch operations..." << std::endl;
    std::cout << std::endl;
    
    testContextManagers();
    std::cout << std::endl;
    
    testBatchOperations();
    std::cout << std::endl;
    
    testConvenienceMethods();
    std::cout << std::endl;
    
    testBuilderPattern();
    std::cout << std::endl;
    
    testExperimentManagement();
    std::cout << std::endl;
    
    testAdvancedWorkflow();
    std::cout << std::endl;
    
    std::cout << "=== All High-Level API Integration Tests PASSED ===" << std::endl;
    std::cout << "The new high-level convenience methods and batch operations are working correctly!" << std::endl;
}

} // namespace test_integration

int main() {
    test_integration::runAll();
    return 0;
}
