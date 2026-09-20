#include <iostream>
#include <memory>
#include "advanced/AdvancedSimulationController.hpp"
#include "advanced/PerformanceProfiler.hpp"
#include "advanced/NetworkVisualizer.hpp"
#include "advanced/AdvancedDebugger.hpp"
#include "advanced/ConfigurationTemplateManager.hpp"
#include "advanced/PowerUserAPI.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== Advanced Features Demo ===\n\n";
    
    // Create a basic configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    std::cout << "1. Testing AdvancedSimulationController...";
    auto controller = std::make_shared<nlm::AdvancedSimulationController>(
        brain, config, nullptr, nullptr, nullptr);
    
    // Create a simple stimulus
    nlm::CustomStimulusConfig stimulus;
    stimulus.name = "Demo Stimulus";
    stimulus.pattern = nlm::StimulusPattern::RegularSpiking;
    stimulus.frequency = 10.0;  // Hz
    stimulus.targetNeuronId = -1;  // All neurons
    stimulus.startTime = 0.0;
    stimulus.duration = 100.0;  // ms
    stimulus.enabled = true;
    
    if (controller->injectCustomStimulus(stimulus)) {
        std::cout << "  ✓ Stimulus injection configured successfully\n";
    } else {
        std::cout << "  ✗ Failed to configure stimulus\n";
    }
    
    std::cout << "2. Testing PerformanceProfiler...";
    auto profiler = std::make_shared<nlm::PerformanceProfiler>(brain, config);
    profiler->startMonitoring();
    
    // Run a few simulation steps
    for (size_t i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    profiler->onSimulationStep(10, 10.0, 10.0);
    profiler->stopMonitoring();
    
    auto metrics = profiler->getCurrentMetrics();
    std::cout << "  ✓ Performance metrics collected\n";
    std::cout << "    Simulation speed: " << metrics.simulationSpeed << " steps/sec\n";
    std::cout << "    Average step time: " << metrics.avgStepTime << " ms\n";
    std::cout << "    Memory usage: " << metrics.memoryUsage << " MB\n";
    
    std::cout << "3. Testing AdvancedDebugger...";
    auto debugger = std::make_shared<nlm::AdvancedDebugger>(brain, config);
    debugger->startDebugging();
    
    // Get debug info for first neuron
    auto neuronIds = controller->getBrain()->getAllNeurons();
    if (!neuronIds.empty()) {
        auto debugInfo = debugger->getNeuronDebugInfo(neuronIds[0]);
        std::cout << "  ✓ Neuron debug info retrieved (ID: " 
                  << static_cast<int>(debugInfo.id) << ")\n";
        std::cout << "    Membrane potential: " << debugInfo.membranePotential << " mV\n";
        std::cout << "    Firing rate: " << debugInfo.firingRate << " Hz\n";
    }
    
    debugger->stopDebugging();
    
    std::cout << "4. Testing ConfigurationTemplateManager...";
    auto templateManager = std::make_shared<nlm::ConfigurationTemplateManager>(config);
    
    // Create a simple template
    nlm::ConfigurationTemplate templateConfig;
    templateConfig.name = "Research Configuration";
    templateConfig.description = "Template for neural research simulations";
    templateConfig.category = "research";
    templateConfig.defaultScenario = "medium";
    
    nlm::TemplateParameter param1;
    param1.name = "neuron_count";
    param1.defaultValue = 1000;
    param1.type = "int";
    param1.description = "Number of neurons in the simulation";
    param1.required = true;
    param1.units = "neurons";
    
    nlm::TemplateParameter param2;
    param2.name = "connection_probability";
    param2.defaultValue = 0.1;
    param2.type = "float";
    param2.description = "Probability of connection between neurons";
    param2.required = false;
    param2.range = std::make_pair(0.0, 1.0);
    
    templateConfig.parameters = {param1, param2};
    
    if (templateManager->createTemplate(templateConfig)) {
        std::cout << "  ✓ Template created successfully\n";
    } else {
        std::cout << "  ✗ Failed to create template\n";
    }
    
    // Apply template
    auto templateConfigApplied = templateManager->applyTemplate("Research Configuration");
    if (templateConfigApplied) {
        std::cout << "  ✓ Template applied successfully\n";
        std::cout << "    Template contains " << templateConfigApplied->getKeys().size() 
                  << " parameters\n";
    } else {
        std::cout << "  ✗ Failed to apply template\n";
    }
    
    std::cout << "5. Testing PowerUserAPI...";
    auto powerAPI = std::make_shared<nlm::PowerUserAPI>(brain, config);
    
    // Inject current to all neurons
    if (powerAPI->injectCurrentToType(nlm::NeuronType::Internal, 5.0f)) {
        std::cout << "  ✓ Batch current injection successful\n";
    }
    
    // Get performance statistics
    auto stats = powerAPI->getPerformanceStats();
    std::cout << "  ✓ Performance stats retrieved\n";
    std::cout << "    Steps completed: " << stats.stepCount << "\n";
    std::cout << "    Average step time: " << stats.averageStepTime << " ms\n";
    std::cout << "    Total spikes: " << stats.totalSpikes << "\n";
    
    // Generate network report
    std::string networkReport = powerAPI->generateNetworkReport();
    std::cout << "  ✓ Network report generated\n";
    
    // Save state
    if (powerAPI->saveState("./demo_checkpoint.dat")) {
        std::cout << "  ✓ State saved successfully\n";
    }
    
    std::cout << "\n=== Demo Complete ===\n";
    
    // Cleanup
    controller->cleanup();
    profiler->cleanup();
    debugger->cleanup();
    powerAPI->cleanup();
    
    return 0;
}
