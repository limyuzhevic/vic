#include "CommandLineInterface.hpp"
#include "environment/Environment.hpp"
#include "motor/Action.hpp"
#include "agent/SensoryPercept.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

// Helper functions for the CLI
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "  No neurons found!" << std::endl;
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    std::cout << "  Injecting current into 10 neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f); // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "  Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "  ✓ Spikes propagate through network" << std::endl;
    } else {
        std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "    Neuron " << i << " V=" << neurons[i]->getMembranePotential()
                      << " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
    }
}

void runPlasticityLearningExperiment(std::shared_ptr<Brain> brain) {
    std::cout << "  Enabling plasticity on synapses..." << std::endl;
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    std::cout << "  Applying repeated input patterns (1000 steps)..." << std::endl;
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "  Step " << step << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    std::cout << "  Plasticity experiment complete" << std::endl;
}

void runStdpVerificationTest(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "  Not enough synapses for STDP test" << std::endl;
        return;
    }
    
    std::cout << "  Testing STDP on 5 synapses:" << std::endl;
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
        std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
    
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
    std::cout << "  After correlated activity:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        std::cout << "    Synapse " << i << " after: " << synapses[i]->getWeight()
                  << " (Δ=" << delta << ")" << std::endl;
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "  ! STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "  ! No change: STDP may not be triggering" << std::endl;
    }
}

void runDevelopmentalStages(std::shared_ptr<Brain> brain) {
    std::cout << "  Running developmental stages..." << std::endl;
    
    // Reset and initialize
    brain->reset();
    brain->initialize();
    
    double timestep = 0.001;
    double totalTime = 0.0;
    
    // Run for 15 minutes (900 seconds of simulation time)
    for (SimulationStep step = 0; step < 900000; ++step) {
        brain->step(step, totalTime);
        totalTime += timestep;
        
        // Log developmental stage changes
        if (step % 100000 == 0) {
            auto stage = brain->getDevelopmentalStage();
            std::cout << "    Stage: " << static_cast<int>(stage) << " (" << totalTime << "s)" << std::endl;
        }
    }
    
    std::cout << "  Developmental trajectory complete" << std::endl;
}

void runCuriosityExploration(std::shared_ptr<Brain> brain) {
    std::cout << "  Running curiosity-driven exploration..." << std::endl;
    
    // Enable curiosity neuromodulation
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->enable(true);
    }
    
    // Run exploration simulation
    brain->reset();
    brain->initialize();
    
    double timestep = 0.001;
    double totalTime = 0.0;
    
    for (SimulationStep step = 0; step < 300000; ++step) { // 300 seconds
        brain->step(step, totalTime);
        totalTime += timestep;
        
        if (step % 50000 == 0) {
            auto* curiosity = brain->getCuriosity();
            if (curiosity) {
                float curiosityLevel = curiosity->getLevel();
                std::cout << "    Step " << step << ", Curiosity level: " << curiosityLevel << std::endl;
            }
        }
    }
    
    std::cout << "  Curiosity-driven exploration complete" << std::endl;
}

void runRewardLearning(std::shared_ptr<Brain> brain) {
    std::cout << "  Running reward-based learning..." << std::endl;
    
    // Enable reward modulation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->enable(true);
    }
    
    // Reset and initialize
    brain->reset();
    brain->initialize();
    
    double timestep = 0.001;
    double totalTime = 0.0;
    
    for (SimulationStep step = 0; step < 500000; ++step) { // 500 seconds
        // Simulate reward signal
        if (step % 50000 == 0) {
            float reward = 1.0f;
            float predictedReward = 0.5f;
            
            // Apply reward modulation
            brain->applyNeuromodulation(Neuromodulator(NeuromodulatorType::Dopamine, reward - predictedReward));
        }
        
        brain->step(step, totalTime);
        totalTime += timestep;
        
        if (step % 50000 == 0) {
            std::cout << "    Step " << step << ", Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "  Reward-based learning complete" << std::endl;
}

} // namespace nlm
