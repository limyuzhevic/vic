// Basic Neural Connectivity Test
// Injects current and verifies spike propagation with optimizations

#ifndef BASIC_CONNECTIVITY_TEST_HPP
#define BASIC_CONNECTIVITY_TEST_HPP

#include "TestRunner.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Optimizations/Optimizations.hpp"
#include <memory>

namespace nlm {

class BasicConnectivityTest {
public:
    static bool run(std::shared_ptr<Brain> brain) {
        const std::string testName = "Basic Neural Connectivity";
        
        // Inject current into neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("  No region found for connectivity test");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        nlm::optimizations::StringBuilder sb;
        sb.append("  Injecting current into 10 neurons...");
        NLM_LOG_INFO(sb.build());
        
        const size_t neuronCount = std::min(static_cast<size_t>(10), neurons.size());
        for (size_t i = 0; i < neuronCount; ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        nlm::optimizations::StringBuilder spikeSB;
        spikeSB.append("  Spikes generated: ").append(spikes);
        NLM_LOG_INFO(spikeSB.build());
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            nlm::optimizations::StringBuilder noSpikeSB;
            noSpikeSB.append("  ! No spikes - checking neuron parameters...");
            NLM_LOG_INFO(noSpikeSB.build());
            
            const size_t checkCount = std::min(static_cast<size_t>(3), neurons.size());
            for (size_t i = 0; i < checkCount; ++i) {
                nlm::optimizations::StringBuilder neuronSB;
                neuronSB.append("    Neuron ").append(i)
                       .append(" V=").append(neurons[i]->getMembranePotential())
                       .append(" thresh=").append(neurons[i]->getThreshold());
                NLM_LOG_INFO(neuronSB.build());
            }
            return false;
        }
    }
};

} // namespace nlm

#endif // BASIC_CONNECTIVITY_TEST_HPP