// NLM Command Line Test Application
// Simple command-line interface for running NLM tests

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "tests/TestResults.hpp"
#include <iostream>
#include <string>
#include <map>

namespace nlm {

class CommandLineInterface {
public:
    static int runTest(const std::string& testName, std::shared_ptr<Brain> brain) {
        if (testName == "basic") {
            runBasicConnectivityTest(brain);
            return 0;
        } else if (testName == "plasticity") {
            runPlasticityExperiment(brain);
            return 0;
        } else if (testName == "stdp") {
            runStdpVerification(brain);
            return 0;
        } else {
            std::cerr << "Unknown test: " << testName << std::endl;
            return 1;
        }
    }
    
    static void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Connectivity Test ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Basic connectivity test PASSED");
        } else {
            NLM_LOG_INFO("  ✗ Basic connectivity test FAILED - no spikes");
        }
    }
    
    static void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Plasticity Experiment Test ===");
        
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        NLM_LOG_INFO("Applying plasticity patterns...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                             " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                             " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
    }
    
    static void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Verification Test ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("  Creating correlated pre->post activity...");
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " after: " + std::to_string(synapses[i]->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
        }
        
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP VERIFICATION PASSED - LTP observed");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP REVERSED - LTD observed (check parameters)");
        } else {
            NLM_LOG_INFO("  ! NO CHANGE - STDP may not be triggering");
        }
    }
};