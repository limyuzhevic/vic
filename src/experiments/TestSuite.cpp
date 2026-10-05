#include "TestSuite.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

bool TestSuite::runBasicConnectivityTest(std::shared_ptr<Brain> brain) const {
    return runBasicConnectivityTestImpl(brain);
}

bool TestSuite::runPlasticityExperiment(std::shared_ptr<Brain> brain) const {
    return runPlasticityExperimentImpl(brain);
}

bool TestSuite::runStdpVerification(std::shared_ptr<Brain> brain) const {
    return runStdpVerificationImpl(brain);
}

bool TestSuite::runAllTests(std::shared_ptr<Brain> brain) const {
    bool allPassed = true;
    
    printTestBanner();
    
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Test 1: Basic connectivity
    std::cout << "Running Basic Connectivity Test..." << std::endl;
    if (!runBasicConnectivityTestImpl(brain)) {
        logTestResult("Basic Connectivity", false, "Connectivity test failed");
        allPassed = false;
    } else {
        logTestResult("Basic Connectivity", true, "Neurons connected successfully");
    }
    
    // Reset and prepare for plasticity test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity experiment
    std::cout << "Running Plasticity Experiment..." << std::endl;
    if (!runPlasticityExperimentImpl(brain)) {
        logTestResult("Plasticity Experiment", false, "Learning experiment failed");
        allPassed = false;
    } else {
        logTestResult("Plasticity Experiment", true, "Synaptic changes detected");
    }
    
    // Reset and prepare for STDP verification
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    std::cout << "Running STDP Verification..." << std::endl;
    if (!runStdpVerificationImpl(brain)) {
        logTestResult("STDP Verification", false, "STDP mechanism not working");
        allPassed = false;
    } else {
        logTestResult("STDP Verification", true, "Pre-before-post LTP confirmed");
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
    }
    
    return allPassed;
}

bool TestSuite::runBasicConnectivityTestImpl(std::shared_ptr<Brain> brain) const {
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        logProgress("No region found!");
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        logProgress("No neurons found!");
        return false;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    logProgress("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    logProgress("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        logProgress("✓ Spikes propagate through network");
        return true;
    } else {
        logProgress("! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            logProgress("  Neuron " + std::to_string(i) + 
                       " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                       " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
        return false;
    }
}

bool TestSuite::runPlasticityExperimentImpl(std::shared_ptr<Brain> brain) const {
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    logProgress("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            logProgress("  Step " + std::to_string(step) + 
                       " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                       " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Create and run learning experiment
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    // Apply patterns (same as above)
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    return experiment.learningOccurred();
}

bool TestSuite::runStdpVerificationImpl(std::shared_ptr<Brain> brain) const {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        logProgress("No region found!");
        return false;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        logProgress("Not enough synapses for STDP test");
        return false;
    }
    
    logProgress("Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        logProgress("  Synapse " + std::to_string(i) + 
                   " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    logProgress("");
    logProgress("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
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
    logProgress("After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        logProgress("  Synapse " + std::to_string(i) + 
                   " after: " + std::to_string(synapses[i]->getWeight()) +
                   " (Δ=" + std::to_string(delta) + ")");
        totalDelta += delta;
    }
    
    logProgress("");
    if (totalDelta > 0.001f) {
        logProgress("✓ STDP WORKING: Pre-before-post produced potentiation");
        return true;
    } else if (totalDelta < -0.001f) {
        logProgress("! STDP reversed: Check parameters");
        return false;
    } else {
        logProgress("! No change: STDP may not be triggering");
        return false;
    }
}

} // namespace nlm