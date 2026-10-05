#include "experiments/ImprovedExperimentRunner.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <algorithm>

using namespace nlm;

void ImprovedExperimentRunner::runLearningExperiment(std::shared_ptr<Brain> brain, bool verbose) {
    // Enhanced learning experiment with more sophisticated patterns
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Enable advanced plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // All plasticity enabled
        }
    }
    
    // Create complex input patterns
    for (int pattern = 0; pattern < 5; ++pattern) {
        NLM_LOG_INFO("Running learning pattern " + std::to_string(pattern + 1) + "/5");
        
        for (int step = 0; step < 500; ++step) {
            // Create pattern based on step number
            float inputValue = 30.0f + 20.0f * std::sin(step * 0.01f);
            
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 5; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, inputValue);
            }
            
            brain->step(step, step * 0.001);
            
            if (verbose && step % 100 == 0) {
                NLM_LOG_INFO("  Pattern " + std::to_string(pattern + 1) + 
                             ", Step " + std::to_string(step) +
                             ", Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        // Small pause between patterns
        if (pattern < 4) {
            for (int step = 0; step < 50; ++step) {
                brain->step(step + 500, (step + 500) * 0.001);
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Enhanced learning experiment completed in " + 
                 std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
}

void ImprovedExperimentRunner::runStdpAnalysis(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("Running enhanced STDP analysis with multiple protocols...");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 10) return;
    
    // LTP protocol
    NLM_LOG_INFO("  Testing LTP (pre-before-post)...");
    std::vector<float> ltpWeights;
    for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
        ltpWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Run LTP trials
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            neurons[0]->injectCurrent(100.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            neurons[1]->injectCurrent(100.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Calculate LTP
    float totalLTP = 0.0f;
    for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
        float delta = synapses[i]->getWeight() - ltpWeights[i];
        totalLTP += delta;
        if (verbose) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " LTP: " + std::to_string(delta));
        }
    }
    
    NLM_LOG_INFO("  Total LTP: " + std::to_string(totalLTP));
    
    // LTD protocol (reverse order)
    NLM_LOG_INFO("  Testing LTD (post-before-pre)...");
    std::vector<float> ltdWeights;
    for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
        ltdWeights.push_back(synapses[i]->getWeight());
    }
    
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            neurons[1]->injectCurrent(100.0f);
            brain->step(trial + 200, (trial + 200) * 0.001);
            neurons[0]->injectCurrent(100.0f);
            brain->step(trial + 201, (trial + 201) * 0.001);
        }
    }
    
    float totalLTD = 0.0f;
    for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
        float delta = synapses[i]->getWeight() - ltdWeights[i];
        totalLTD += delta;
        if (verbose) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " LTD: " + std::to_string(delta));
        }
    }
    
    NLM_LOG_INFO("  Total LTD: " + std::to_string(totalLTD));
    
    // Re-enable Hebbian for continued learning
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
}

void ImprovedExperimentRunner::runAdaptiveExperiment(std::shared_ptr<Brain> brain, const std::string& mode) {
    NLM_LOG_INFO("Running adaptive experiment in mode: " + mode);
    
    if (mode == "reinforcement") {
        // Simple reinforcement learning experiment
        NLM_LOG_INFO("  Implementing reward-based learning...");
        // Would integrate with reward system
    } else if (mode == "curiosity") {
        // Curiosity-driven exploration
        NLM_LOG_INFO("  Implementing curiosity-driven exploration...");
        // Would integrate with curiosity system
    } else if (mode == "novelty") {
        // Novelty detection and learning
        NLM_LOG_INFO("  Implementing novelty detection...");
        // Would integrate with novelty detection
    }
    
    // Run general steps
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         ", Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
}

void ImprovedExperimentRunner::runVisualization(std::shared_ptr<Brain> brain, const std::string& outputPath) {
    NLM_LOG_INFO("Running visualization and analysis...");
    
    // Generate basic statistics for visualization
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Create simple visualization data
    std::ofstream file(outputPath);
    if (file.is_open()) {
        file << "NLM Brain Visualization Data\n";
        file << "========================\n\n";
        
        file << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        file << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
        file << "Firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        file << "\nNeuron Statistics:\n";
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
            const auto& state = neurons[i]->getState();
            file << "  Neuron " << i << ": "
                 << "V=" << state.membranePotential << " "
                 << "R=" << state.restingPotential << " "
                 << "Thresh=" << state.threshold << " "
                 << "SpikeTime=" << state.lastSpikeTime << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Visualization data saved to: " + outputPath);
    } else {
        NLM_LOG_ERROR("Failed to save visualization data to: " + outputPath);
    }
}

} // namespace nlm