#include "Neuron.hpp"

namespace nlm {

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

} // namespace nlm
