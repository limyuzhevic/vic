#include "BasicConnectivityTest.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace basic_connectivity {

void BasicConnectivityTest::run(std::shared_ptr<Brain> brain) {
    TestUtils::logSubsection("Test 1: Basic Neural Connectivity");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        TestUtils::logError("Region 1 not found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        TestUtils::logError("No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    TestUtils::logResult("Injecting current", "Into 10 neurons");
    injectCurrentPattern(brain, std::min(size_t(10), neurons.size()), 50.0f);
    
    // Run a few steps
    TestUtils::logProgress(0, 50, "Basic connectivity test");
    runConnectivitySteps(brain, 50);
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikes = finalSpikes - initialSpikes;
    TestUtils::logResult("Spikes generated", std::to_string(spikes));
    
    analyzeResults(brain, initialSpikes, finalSpikes);
}

void BasicConnectivityTest::injectCurrentPattern(std::shared_ptr<Brain> brain, size_t neuronCount, float current) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    for (size_t i = 0; i < std::min(neuronCount, neurons.size()); ++i) {
        neurons[i]->injectCurrent(current);
    }
}

void BasicConnectivityTest::runConnectivitySteps(std::shared_ptr<Brain> brain, size_t steps) {
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        if (step % 10 == 0) {
            TestUtils::logProgress(step, steps, "Basic connectivity test");
        }
    }
}

void BasicConnectivityTest::analyzeResults(std::shared_ptr<Brain> brain, size_t initialSpikes, size_t finalSpikes) {
    size_t spikes = finalSpikes - initialSpikes;
    
    if (spikes > 0) {
        TestUtils::logResult("✓ Status", "Spikes propagate through network");
    } else {
        TestUtils::logResult("! Status", "No spikes - checking neuron parameters");
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                    TestUtils::logResult(std::string("Neuron ") + std::to_string(i),
                        "V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
                }
            }
        }
    }
}

} // namespace basic_connectivity
} // namespace nlm