Implement core functionality for utility classes

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "bindings_utility.cpp"

namespace py = pybind11;
namespace nlm {

// BrainTrainer implementation
class BrainTrainer::Impl {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    
    int episodeSteps;
    bool verbose;
    double learningRate;
    double explorationRate;
    double discountFactor;
    
    double totalFiringRate;
    size_t maxFiringRate;
    double trainingProgress;
    int episodesCompleted;
    int targetEpisodes;
};

BrainTrainer::BrainTrainer(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    pImpl = std::make_unique<Impl>();
    pImpl->brain = brain;
    pImpl->world = world;
 FMC_ERRORLIMIT=285858 from /workspace/app/python/bindings_utility.cpp:6615: \'\\\' in template. The fundamental cause for this error message is: invalid conversion from type \'std::vector<float>\' to type \'float\'\'.
... can't do that" -10.0000 19.7963\n            ]

      output line | location | message
      14 | 12 | std::reference_policy::unique
      14 | 12 | There is no matching function that takes 2 arguments
      14 | 12 | Not a valid overload of \'getRegion\'
      14 | 12 | invalid conversion from \'std::reference_wrapper<NeuralRegion>\' to type \'NeuralRegion*\'
      14 | 12 | no known conversion from \'const std::reference_wrapper<NeuralRegion>\' to \'NeuralRegion*\'

The problem is in BrainTrainer.cpp:12:

    auto* region = pImpl->brain->getRegion(1);

Fix it by using the correct method signature:

    auto* region = pImpl->brain->getRegion(RegionId(1));

1) Try again with this fix by re-pasting the corrected code.

2) Try again with auto-fix (recommended): Auto-fix will automatically correct the above error and recompile the file.
