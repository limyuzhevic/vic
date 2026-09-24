#include "BrainManager.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include "tests/test_basic_connectivity.hpp"
#include "tests/test_plasticity.hpp"
#include "tests/test_stdp.hpp"

namespace nlm {

bool BrainManager::initializeAndRun(std::shared_ptr<Config> config) {
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    SimulationClock clock(timestep);
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    if (!nlm::TestRunner::runAllTests()) {
        NLM_LOG_ERROR("Basic connectivity test failed!");
        return false;
    }
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    if (!nlm::TestRunner::runAllTests()) {
        NLM_LOG_ERROR("Plasticity experiment test failed!");
        return false;
    }
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    if (!nlm::TestRunner::runAllTests()) {
        NLM_LOG_ERROR("STDP verification test failed!");
        return false;
    }
    
    // Log that test phase is complete
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Test Execution Complete ===");
    NLM_LOG_INFO("All neural computation tests completed successfully.");
    
    return true;
}

} // namespace nlm
