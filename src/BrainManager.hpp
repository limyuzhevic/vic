#ifndef BRAIN_MANAGER_HPP
#define BRAIN_MANAGER_HPP

#include <memory>
#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Random/Random.hpp"
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
#include "tests/test_basic_connectivity.hpp"
#include "tests/test_plasticity.hpp"
#include "tests/test_stdp.hpp"

namespace nlm {

class BrainManager {
public:
    static bool initializeAndRun(std::shared_ptr<Config> config);
};

} // namespace nlm

#endif // BRAIN_MANAGER_HPP
