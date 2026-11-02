#include "StdpVerification.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace stdp_verification {

void StdpVerification::run(std::shared_ptr<Brain> brain) {
    TestUtils::logSubsection("Test 3: STDP Verification");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        TestUtils::logError("Region 1 not found for STDP test");
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        TestUtils::logError("No synapses for STDP test");
        return;
    }
    
    Synapse* syn = getTestSynapse(brain);
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    
    TestUtils::logResult("Testing STDP on 1 synapse", "");
    TestUtils::logResult("  Synapse before", std::to_string(beforeWeight) +
                         " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                         ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    // Create correlated pre->post activity (potentiation)
    TestUtils::logResult("Creating correlated activity", "50 trials (potentiation)");
    createCorrelatedPrePostActivity(brain, syn, 50);
    
    float delta = computeSynapticChange(syn, beforeWeight);
    TestUtils::logResult("After correlated activity", "Synapse weight: " + std::to_string(syn->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
    
    analyzeStdpResults(delta);
}

Synapse* StdpVerification::getTestSynapse(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return nullptr;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) return nullptr;
    
    // Use one real synapse and its own pre/post neurons
    // The old test fired neurons[0]/neurons[1] and measured synapses[0..4],
    // which are usually unrelated to that pair, so its result said nothing about STDP.
    return synapses[0].get();
}

void StdpVerification::createCorrelatedPrePostActivity(std::shared_ptr<Brain> brain, Synapse* syn, int trials) {
    for (int trial = 0; trial < trials; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
}

float StdpVerification::computeSynapticChange(Synapse* syn, float beforeWeight) {
    return syn->getWeight() - beforeWeight;
}

void StdpVerification::analyzeStdpResults(float delta) {
    if (delta > 0.001f) {
        TestUtils::logResult("✓ Status", "STDP WORKING: Pre-before-post produced potentiation");
    } else if (delta < -0.001f) {
        TestUtils::logResult("! Status", "STDP reversed: Check parameters");
    } else {
        TestUtils::logResult("! Status", "No change: STDP may not be triggering");
    }
}

} // namespace stdp_verification
} // namespace nlm