// Advanced Features Test Suite
// Tests for Phase 7 advanced power-user features

#include "advanced/AdvancedSimulationController.hpp"
#include "advanced/PerformanceProfiler.hpp"
#include "advanced/NetworkVisualizer.hpp"
#include "advanced/AdvancedDebugger.hpp"
#include "advanced/ConfigurationTemplateManager.hpp"
#include "advanced/PowerUserAPI.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace test_advanced {

void testAdvancedSimulationController() {
    std::cout << "Testing AdvancedSimulationController..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto controller = std::make_shared<nlm::AdvancedSimulationController>(
        brain, config, nullptr, nullptr, nullptr);
    
    // Test stimulus injection configuration
    nlm::CustomStimulusConfig stimulus;
    stimulus.name = "Test Stimulus";
    stimulus.pattern = nlm::StimulusPattern::Poisson;
    stimulus.meanRate = 50.0;
    stimulus.enabled = true;
    
    assert(controller->injectCustomStimulus(stimulus));
    std::cout << "  ✓ Stimulus injection configured" << std::endl;
    
    // Test synchronization control
    nlm::SyncControlConfig syncConfig;
    syncConfig.speedupFactor = 2.0;
    syncConfig.enableFixedSteps = true;
    syncConfig.maxSteps = 100;
    
    controller->configureSyncControl(syncConfig);
    std::cout << "  ✓ Synchronization control configured" << std::endl;
    
    // Test batch operations
    nlm::BatchOperationConfig batchConfig;
    batchConfig.name = "Test Batch";
    batchConfig.runInParallel = false;
    batchConfig.parameterVariations = {0.5, 1.0, 1.5};
    
    auto results = controller->runBatchOperation(batchConfig, 3);
    std::cout << "  ✓ Batch operation completed: " << results.size() << " results" << std::endl;
    
    // Test checkpoint and replay
    assert(controller->saveCheckpoint("./test_checkpoint.dat", "Test checkpoint"));
    std::cout << "  ✓ Checkpoint saved" << std::endl;
    
    assert(controller->loadCheckpoint("./test_checkpoint.dat"));
    std::cout << "  ✓ Checkpoint loaded" << std::endl;
    
    controller->cleanup();
    std::cout << "  ✓ AdvancedSimulationController tests passed" << std::endl;
}

void testPerformanceProfiler() {
    std::cout << "Testing PerformanceProfiler..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto profiler = std::make_shared<nlm::PerformanceProfiler>(brain, config);
    
    // Test monitoring start/stop
    profiler->startMonitoring();
    std::cout << "  ✓ Monitoring started" << std::endl;
    
    // Run some simulation steps
    for (size_t i = 0; i < 5; ++i) {
        brain->step(i);
        profiler->onSimulationStep(i, static_cast<double>(i) * 10.0, static_cast<double>(i) * 0.1);
    }
    
    profiler->stopMonitoring();
    std::cout << "  ✓ Monitoring stopped" << std::endl;
    
    // Test metrics collection
    auto metrics = profiler->getCurrentMetrics();
    assert(metrics.simulationSpeed > 0.0);
    std::cout << "  ✓ Metrics collected (speed: " << metrics.simulationSpeed << " steps/sec)" << std::endl;
    
    // Test memory tracking
    profiler->startMemoryTracking();
    profiler->stopMemoryTracking();
    std::cout << "  ✓ Memory tracking completed" << std::endl;
    
    // Test performance analysis
    profiler->analyzeSimulationSpeed();
    profiler->analyzeMemoryUsage();
    profiler->analyzeEventProcessing();
    std::cout << "  ✓ Performance analysis completed" << std::endl;
    
    // Test reporting
    auto perfReport = profiler->generatePerformanceReport();
    auto memReport = profiler->generateMemoryReport();
    assert(!perfReport.empty());
    assert(!memReport.empty());
    std::cout << "  ✓ Reports generated" << std::endl;
    
    profiler->cleanup();
    std::cout << "  ✓ PerformanceProfiler tests passed" << std::endl;
}

void testNetworkVisualizer() {
    std::cout << "Testing NetworkVisualizer..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto visualizer = std::make_shared<nlm::NetworkVisualizer>(brain, config);
    
    // Test initialization
    assert(visualizer->initialize());
    std::cout << "  ✓ Visualizer initialized" << std::endl;
    
    // Test configuration
    visualizer->setVisualizationType(nlm::VisualizationType::NetworkTopology);
    visualizer->setColorScheme(nlm::ColorScheme::Viridis);
    visualizer->setUpdateRate(10.0);
    std::cout << "  ✓ Visualization configured" << std::endl;
    
    // Test data collection
    visualizer->collectBrainData(*brain);
    std::cout << "  ✓ Brain data collected" << std::endl;
    
    // Test spike visualization
    nlm::SpikeVisualization spike;
    spike.neuronId = 1;
    spike.step = 5;
    spike.time = 50.0;
    spike.amplitude = 20.0;
    spike.color = "red";
    spike.size = 2.0;
    
    visualizer->addSpikeVisualization(spike);
    std::cout << "  ✓ Spike visualization added" << std::endl;
    
    // Test analysis
    auto clusters = visualizer->performClustering(*brain);
    std::cout << "  ✓ Clustering performed: " << clusters.size() << " clusters" << std::endl;
    
    auto hubs = visualizer->identifyHubNeurons(*brain, 0.5f);
    std::cout << "  ✓ Hub neurons identified: " << hubs.size() << " hubs" << std::endl;
    
    // Test reporting
    auto topologyReport = visualizer->generateTopologyReport();
    auto spikeReport = visualizer->generateSpikeAnalysisReport();
    assert(!topologyReport.empty());
    assert(!spikeReport.empty());
    std::cout << "  ✓ Analysis reports generated" << std::endl;
    
    visualizer->cleanup();
    std::cout << "  ✓ NetworkVisualizer tests passed" << std::endl;
}

void testAdvancedDebugger() {
    std::cout << "Testing AdvancedDebugger..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto debugger = std::make_shared<nlm::AdvancedDebugger>(brain, config);
    
    // Test debugging start/stop
    debugger->startDebugging();
    std::cout << "  ✓ Debugging started" << std::endl;
    
    // Test state inspection
    auto allNeurons = brain->getAllNeurons();
    if (!allNeurons.empty()) {
        auto debugInfo = debugger->getNeuronDebugInfo(allNeurons[0]);
        assert(debugInfo.id == allNeurons[0]);
        std::cout << "  ✓ Neuron debug info retrieved" << std::endl;
    }
    
    // Test event tracing
    debugger->startEventTracing(nlm::EventFilterCategory::Spiking, nlm::EventType::Fired);
    debugger->stopEventTracing();
    std::cout << "  ✓ Event tracing completed" << std::endl;
    
    // Test plasticity analysis
    debugger->analyzeSTDP();
    debugger->analyzeHebbian();
    debugger->analyzeRewardModulated();
    std::cout << "  ✓ Plasticity analysis completed" << std::endl;
    
    // Test development controls
    debugger->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(debugger->getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    std::cout << "  ✓ Development stage control working" << std::endl;
    
    // Test reporting
    auto neuronReport = debugger->generateNeuronReport(allNeurons.empty() ? nlm::NeuronId(0) : allNeurons[0]);
    auto networkReport = debugger->generateNetworkReport();
    auto plasticityReport = debugger->generatePlasticityReport();
    assert(!neuronReport.empty());
    assert(!networkReport.empty());
    assert(!plasticityReport.empty());
    std::cout << "  ✓ Debug reports generated" << std::endl;
    
    debugger->cleanup();
    std::cout << "  ✓ AdvancedDebugger tests passed" << std::endl;
}

void testConfigurationTemplateManager() {
    std::cout << "Testing ConfigurationTemplateManager..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    auto templateManager = std::make_shared<nlm::ConfigurationTemplateManager>(config);
    
    // Create built-in templates
    templateManager->createResearchTemplates();
    templateManager->createProductionTemplates();
    templateManager->createBenchmarkTemplates();
    templateManager->createExperimentalTemplates();
    std::cout << "  ✓ Built-in templates created" << std::endl;
    
    // List templates
    auto templates = templateManager->listTemplates();
    assert(!templates.empty());
    std::cout << "  ✓ Templates listed: " << templates.size() << " templates" << std::endl;
    
    // Test template application
    if (!templates.empty()) {
        auto templateConfig = templateManager->applyTemplate(templates[0]);
        if (templateConfig) {
            std::cout << "  ✓ Template applied: " << templateConfig->getKeys().size() << " parameters" << std::endl;
        }
    }
    
    // Test template validation
    nlm::ConfigurationTemplate testTemplate;
    testTemplate.name = "Test Template";
    testTemplate.description = "Test template for validation";
    
    nlm::TemplateParameter param;
    param.name = "test_param";
    param.defaultValue = 42;
    param.type = "int";
    param.required = true;
    
    testTemplate.parameters = {param};
    
    auto validationResult = templateManager->validateTemplate(testTemplate);
    std::cout << "  ✓ Template validation completed: " << (validationResult.valid ? "valid" : "invalid") << std::endl;
    
    // Test configuration validation
    auto configValidation = templateManager->validateConfig(*config);
    std::cout << "  ✓ Configuration validation completed: " << (configValidation.valid ? "valid" : "invalid") << std::endl;
    
    templateManager->cleanup();
    std::cout << "  ✓ ConfigurationTemplateManager tests passed" << std::endl;
}

void testPowerUserAPI() {
    std::cout << "Testing PowerUserAPI..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto powerAPI = std::make_shared<nlm::PowerUserAPI>(brain, config);
    
    // Test neuron access
    auto allNeurons = powerAPI->getAllNeurons();
    assert(!allNeurons.empty());
    std::cout << "  ✓ Neuron access: " << allNeurons.size() << " neurons" << std::endl;
    
    // Test property access
    auto neuronInfo = powerAPI->getNeuronInfo(allNeurons[0]);
    assert(neuronInfo.id == allNeurons[0]);
    std::cout << "  ✓ Neuron property access working" << std::endl;
    
    // Test synapse access
    auto allSynapses = powerAPI->findSynapsesByWeight(0.0f, 1.0f);
    std::cout << "  ✓ Synapse access: " << allSynapses.size() << " synapses" << std::endl;
    
    // Test population access
    auto allPopulations = powerAPI->getAllRegions();
    std::cout << "  ✓ Population access: " << allPopulations.size() << " regions" << std::endl;
    
    // Test plasticity control
    assert(powerAPI->setPlasticityRule(nlm::PlasticityType::STDP, 0.1f, 0.01f));
    std::cout << "  ✓ Plasticity rule control working" << std::endl;
    
    // Test spike analysis
    auto spikePatterns = powerAPI->analyzeAllSpikePatterns();
    std::cout << "  ✓ Spike pattern analysis: " << spikePatterns.size() << " patterns" << std::endl;
    
    // Test connectivity analysis
    auto connectivity = powerAPI->analyzeConnectivity();
    std::cout << "  ✓ Connectivity analysis completed" << std::endl;
    
    // Test low-level controls
    assert(powerAPI->injectCurrentToType(nlm::NeuronType::Internal, 10.0f));
    std::cout << "  ✓ Low-level current injection working" << std::endl;
    
    // Test simulation controls
    powerAPI->pauseSimulation(true);
    assert(powerAPI->isSimulationPaused());
    std::cout << "  ✓ Simulation pause control working" << std::endl;
    
    // Test batch operations
    auto batchResult = powerAPI->batchInjectCurrent(allNeurons, 1.0f);
    std::cout << "  ✓ Batch operation: " << batchResult.size() << " successful" << std::endl;
    
    // Test reporting
    auto networkReport = powerAPI->generateNetworkReport();
    auto plasticityReport = powerAPI->generatePlasticityReport();
    assert(!networkReport.empty());
    assert(!plasticityReport.empty());
    std::cout << "  ✓ Power user reports generated" << std::endl;
    
    // Test state management
    assert(powerAPI->saveState("./power_api_test_checkpoint.dat"));
    assert(powerAPI->loadState("./power_api_test_checkpoint.dat"));
    std::cout << "  ✓ State management working" << std::endl;
    
    powerAPI->cleanup();
    std::cout << "  ✓ PowerUserAPI tests passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Advanced Features Test Suite ===\n" << std::endl;
    
    try {
        testAdvancedSimulationController();
        std::cout << std::endl;
        
        testPerformanceProfiler();
        std::cout << std::endl;
        
        testNetworkVisualizer();
        std::cout << std::endl;
        
        testAdvancedDebugger();
        std::cout << std::endl;
        
        testConfigurationTemplateManager();
        std::cout << std::endl;
        
        testPowerUserAPI();
        std::cout << std::endl;
        
        std::cout << "=== All Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        exit(1);
    }
}

} // namespace test_advanced

int main() {
    test_advanced::runAllTests();
    return 0;
}
