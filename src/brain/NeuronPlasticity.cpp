#include "NeuronPlasticity.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

void NeuronPlasticity::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

}