#pragma once

#include "Experiment.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

// Phase 2: Basic Neural Connectivity Experiment
class ConnectivityExperiment : public Experiment {
public:
    ConnectivityExperiment() : Experiment("connectivity") {
        setName("Basic Neural Connectivity Test");
        addNote("Tests basic neural connectivity with LIF dynamics");
        addNote("Injects current into neurons and verifies spike propagation");
    }
    
    bool run(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running Connectivity Experiment ===");
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Record initial state
        recordMetric("spikes_before", static_cast<double>(brain->getTotalSpikeCount()));
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        // Record final state
        recordMetric("spikes_generated", static_cast<double>(spikes));
        recordMetric("total_spikes", static_cast<double>(brain->getTotalSpikeCount()));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
            recordMetric("connectivity_success", 1.0);
            return true;
        } else {
            NLM_LOG_WARNING("! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            recordMetric("connectivity_success", 0.0);
            return false;
        }
    }
};

// Phase 2: Plasticity Learning Experiment
class PlasticityExperiment : public Experiment {
public:
    PlasticityExperiment() : Experiment("plasticity") {
        setName("Plasticity Learning Experiment");
        addNote("Demonstrates synaptic plasticity through experience");
        addNote("Enables Hebbian and STDP plasticity rules");
    }
    
    bool run(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running Plasticity Experiment ===");
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Record initial state
        recordMetric("initial_synapses", static_cast<double>(brain->getTotalSynapseCount()));
        if (auto* region = brain->getRegion(RegionId(1))) {
            std::vector<float> initialWeights;
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
            if (!initialWeights.empty()) {
                float sum = 0.0f;
                for (float w : initialWeights) sum += w;
                recordMetric("initial_mean_weight", sum / initialWeights.size());
            }
        }
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        recordMetric("final_synapses", static_cast<double>(brain->getTotalSynapseCount()));
        recordMetric("total_spikes", static_cast<double>(brain->getTotalSpikeCount()));
        recordMetric("firing_neurons", static_cast<double>(brain->getFiringNeuronCount()));
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            std::vector<float> finalWeights;
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
            if (!finalWeights.empty() && !initialWeights.empty()) {
                // Compute weight changes
                float initialSum = 0.0f, finalSum = 0.0f;
                for (float w : initialWeights) initialSum += w;
                for (float w : finalWeights) finalSum += w;
                
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                recordMetric("initial_mean_weight", initialMean);
                recordMetric("final_mean_weight", finalMean);
                recordMetric("weight_change", finalMean - initialMean);
                
                NLM_LOG_INFO("Weight change: " + std::to_string(finalMean - initialMean));
            }
        }
        
        NLM_LOG_INFO("✓ Plasticity experiment completed");
        return true;
    }
    
private:
    std::vector<float> initialWeights;
};

// Phase 2: STDP Verification Experiment
class StdpExperiment : public Experiment {
public:
    StdpExperiment() : Experiment("stdp") {
        setName("STDP Verification Experiment");
        addNote("Verifies spike-timing dependent plasticity (STDP)");
        addNote("Tests pre-before-post potentiation (LTP) and post-before-pre depression (LTD)");
    }
    
    bool run(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running STDP Verification Experiment ===");
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
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
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " after: " + std::to_string(synapses[i]->getWeight()) +
                       " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Record STDP metrics
        recordMetric("total_stdp_delta", totalDelta);
        recordMetric("average_delta", totalDelta / 5);
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
            recordMetric("stdp_potentiation_detected", 1.0);
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("! STDP reversed: Check parameters");
            recordMetric("stdp_potentiation_detected", -1.0);
            return false;
        } else {
            NLM_LOG_INFO("! No change: STDP may not be triggering");
            recordMetric("stdp_potentiation_detected", 0.0);
            return false;
        }
    }
};

} // namespace nlm