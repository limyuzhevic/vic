#include "Brain.hpp"

namespace nlm {

SimulationStep Brain::getCurrentStep() const {
    return pImpl->currentStep;
}

} // namespace nlm