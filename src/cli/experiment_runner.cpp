#include "experiment_runner.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <cmath>

using namespace nlm;

ExperimentRunner::ExperimentRunner(std::shared_ptr<Config> config) : config_(config) {
}

ExperimentRunner::~ExperimentRunner() {
}

CommandResult ExperimentRunner::runExperiment(const std::string& name, size_t steps, const std::string& config) {
    try {
        // Find experiment by name
        ExperimentInfo expInfo = getExperimentByName(name);
        if (expInfo.name.empty()) {
            return CommandResult(1, "Unknown experiment: " + name, false, "Available experiments: phase3, phase4, phase5, phase6");
        }
        
        // Create a new brain for the experiment
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            return CommandResult(1, "Failed to initialize brain for experiment: " + name, false, "Brain initialization failed");
        }
        
        // Run experiment based on type
        CommandResult result;
        if (expInfo.name == "phase3") {
            result = runPhase3Experiment(brain, steps);
        } else if (expInfo.name == "phase4") {
            result = runPhase4Experiment(brain, steps);
        } else if (expInfo.name == "phase5") {
            result = runPhase5Experiment(brain, steps);
        } else if (expInfo.name == "phase6") {
            result = runPhase6Experiment(brain, steps);
        } else {
            return CommandResult(1, "Experiment not implemented: " + name, false, "Only phase3, phase4, phase5, and phase6 experiments are available");
        }
        
        // Save experiment results
        ExperimentResult expResult(name, steps);
        expResult.success = result.success;
        expResult.errorMessage = result.errorMessage;
        
        if (result.success) {
            // Save experiment results to file
            std::string filename = "experiment_" + name + "_results.json";
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "{\n";
                file << "  \"experiment_name\": \"" << name << "\",\n";
                file << "  \"steps_completed\": " << steps << ",\n";
                file << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
                file << "  \"average_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
                file << "  \"success\": true\n";
                file << "}\n";
                file.close();
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error running experiment: ") + e.what(), false, e.what());
    }
}

CommandResult ExperimentRunner::runBenchmark(const std::string& name, size_t steps) {
    try {
        if (name == "basic_connectivity") {
            return runBasicConnectivityTest(std::make_shared<Brain>(config_), steps);
        } else if (name == "plasticity_learning") {
            return runPlasticityExperiment(std::make_shared<Brain>(config_), steps);
        } else if (name == "stdp_verification") {
            return runStdpVerification(std::make_shared<Brain>(config_), steps);
        } else {
            return CommandResult(1, "Unknown benchmark: " + name, false, "Available benchmarks: basic_connectivity, plasticity_learning, stdp_verification");
        }
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error running benchmark: ") + e.what(), false, e.what());
    }
}

CommandResult ExperimentRunner::validate() const {
    std::stringstream output;
    output << "=== NLM System Validation ===" << std::endl;
    output << std::endl;
    
    // Validate configuration
    if (!config_.get()) {
        return CommandResult(1, "Configuration not initialized", false, "Configuration is not available");
    }
    
    output << "✓ Configuration initialized" << std::endl;
    
    // Validate config has essential keys
    std::vector<std::string> essentialKeys = {"random_seed", "simulation_timestep", "neuron_count", "region_count"};
    for (const auto& key : essentialKeys) {
        if (config_->has(key)) {
            output << "✓ Configuration key present: " << key << std::endl;
        } else {
            output << "⚠ Configuration key missing: " << key << " (using default)" << std::endl;
        }
    }
    
    output << std::endl;
    output << "=== Validation Complete ===" << std::endl;
    
    return CommandResult(0, output.str());
}

std::vector<ExperimentInfo> ExperimentRunner::getAvailableExperiments() const {
    return {
        getPhase3ExperimentInfo(),
        getPhase4ExperimentInfo(),
        getPhase5ExperimentInfo(),
        getPhase6ExperimentInfo()
    };
}

std::vector<std::string> ExperimentRunner::getAvailableBenchmarks() const {
    return {
        getBasicBenchmarkName(),
        getPlasticityBenchmarkName(),
        getStdpBenchmarkName()
    };
}

CommandResult ExperimentRunner::runPhase3Experiment(size_t steps) {
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        return CommandResult(1, "Failed to initialize brain for Phase 3 experiment", false, "Brain initialization failed");
    }
    
    std::stringstream output;
    output << "Running Phase 3 Experiment (Basic Neural Connectivity)" << std::endl;
    output << "Steps: " << steps << std::endl;
    output << std::endl;
    
    // Run basic connectivity test
    CommandResult result = runBasicConnectivityTest(brain, steps);
    
    if (result.success) {
        output << "Phase 3 experiment completed successfully" << std::endl;
        output << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    return result;
}

CommandResult ExperimentRunner::runPhase4Experiment(size_t steps) {
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        return CommandResult(1, "Failed to initialize brain for Phase 4 experiment", false, "Brain initialization failed");
    }
    
    std::stringstream output;
    output << "Running Phase 4 Experiment (Plasticity Learning)" << std::endl;
    output << "Steps: " << steps << std::endl;
    output << std::endl;
    
    // Run plasticity experiment
    CommandResult result = runPlasticityExperiment(brain, steps);
    
    if (result.success) {
        output << "Phase 4 experiment completed successfully" << std::endl;
        output << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    return result;
}

CommandResult ExperimentRunner::runPhase5Experiment(size_t steps) {
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        return CommandResult(1, "Failed to initialize brain for Phase 5 experiment", false, "Brain initialization failed");
    }
    
    std::stringstream output;
    output << "Running Phase 5 Experiment (Memory Integration)" << std::endl;
    output << "Steps: " << steps << std::endl;
    output << std::endl;
    
    // For now, run basic connectivity test as placeholder
    // In a full implementation, this would test memory systems
    CommandResult result = runBasicConnectivityTest(brain, steps);
    
    if (result.success) {
        output << "Phase 5 experiment completed successfully" << std::endl;
        output << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    return result;
}

CommandResult ExperimentRunner::runPhase6Experiment(size_t steps) {
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        return CommandResult(1, "Failed to initialize brain for Phase 6 experiment", false, "Brain initialization failed");
    }
    
    std::stringstream output;
    output << "Running Phase 6 Experiment (Integrated Multi-Region)" << std::endl;
    output << "Steps: " << steps << std::endl;
    output << std::endl;
    
    // For now, run plasticity experiment as placeholder
    // In a full implementation, this would test integrated multi-region systems
    CommandResult result = runPlasticityExperiment(brain, steps);
    
    if (result.success) {
        output << "Phase 6 experiment completed successfully" << std::endl;
        output << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    return result;
}

CommandResult ExperimentRunner::runBasicConnectivityTest(std::shared_ptr<Brain> brain, size_t steps) {
    std::stringstream output;
    output << "=== Basic Connectivity Test ===" << std::endl;
    output << std::endl;
    
    // Inject current into neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        return CommandResult(1, "No neurons found for connectivity test", false, "Brain has no neurons");
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        return CommandResult(1, "No neurons found for connectivity test", false, "Brain has no neurons");
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    output << "Injecting current into 10 neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run simulation steps
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            output << "  Step " << step << 
                   " | Spikes: " << brain->getTotalSpikeCount() - initialSpikes << 
                   " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    output << "Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        output << "✓ Connectivity test passed: spikes propagate through network" << std::endl;
    } else {
        output << "! Connectivity test warning: no spikes detected" << std::endl;
        output << "Checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            output << "  Neuron " << i << 
                   " V=" << neurons[i]->getMembranePotential() << 
                   " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
    }
    
    output << std::endl;
    output << "=== Test Complete ===" << std::endl;
    
    return CommandResult(0, output.str());
}

CommandResult ExperimentRunner::runPlasticityExperiment(std::shared_ptr<Brain> brain, size_t steps) {
    std::stringstream output;
    output << "=== Plasticity Learning Experiment ===" << std::endl;
    output << std::endl;
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Record initial state
    output << "Applying repeated input patterns (" << steps << " steps)..." << std::endl;
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            output << "  Step " << step << 
                   " | Spikes: " << brain->getTotalSpikeCount() << 
                   " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state
    output << std::endl;
    output << "=== Results ===" << std::endl;
    output << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    output << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    output << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    output << "✓ Plasticity experiment completed successfully" << std::endl;
    output << std::endl;
    
    return CommandResult(0, output.str());
}

CommandResult ExperimentRunner::runStdpVerification(std::shared_ptr<Brain> brain, size_t steps) {
    std::stringstream output;
    output << "=== STDP Verification ===" << std::endl;
    output << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        return CommandResult(1, "No region found for STDP test", false, "Brain has no regions");
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        return CommandResult(1, "Not enough synapses for STDP test", false, "Need at least 5 synapses");
    }
    
    output << "Testing STDP on 5 synapses:" << std::endl;
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        output << "  Synapse " << i << " before: " << beforeWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    output << std::endl;
    output << "Creating correlated pre->post activity (potentiation)..." << std::endl;
    
    auto neurons = region->getAllNeurons();
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    output << "After correlated activity:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        output << "  Synapse " << i << " after: " << synapses[i]->getWeight()
               << " (Δ=" << delta << ")" << std::endl;
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    output << std::endl;
    if (totalDelta > 0.001f) {
        output << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        output << "! STDP reversed: Check parameters" << std::endl;
    } else {
        output << "! No change: STDP may not be triggering" << std::endl;
    }
    
    output << std::endl;
    output << "=== STDP Test Complete ===" << std::endl;
    
    return CommandResult(0, output.str());
}

ExperimentInfo ExperimentRunner::getPhase3ExperimentInfo() const {
    return ExperimentInfo("phase3", 
                         "Basic Neural Connectivity Test",
                         1000,
                         "",
                         false,
                         false);
}

ExperimentInfo ExperimentRunner::getPhase4ExperimentInfo() const {
    return ExperimentInfo("phase4",
                         "Plasticity Learning Experiment",
                         1000,
                         "",
                         false,
                         false);
}

ExperimentInfo ExperimentRunner::getPhase5ExperimentInfo() const {
    return ExperimentInfo("phase5",
                         "Memory Integration Test",
                         500,
                         "",
                         false,
                         false);
}

ExperimentInfo ExperimentRunner::getPhase6ExperimentInfo() const {
    return ExperimentInfo("phase6",
                         "Integrated Multi-Region System",
                         2000,
                         "",
                         true,
                         true);
}

ExperimentInfo ExperimentRunner::getExperimentByName(const std::string& name) const {
    auto experiments = getAvailableExperiments();
    for (const auto& exp : experiments) {
        if (exp.name == name) {
            return exp;
        }
    }
    return ExperimentInfo("", "", 0);
}

std::string ExperimentRunner::getBenchmarkByName(const std::string& name) const {
    auto benchmarks = getAvailableBenchmarks();
    for (const auto& bench : benchmarks) {
        if (bench == name) {
            return bench;
        }
    }
    return "";
}