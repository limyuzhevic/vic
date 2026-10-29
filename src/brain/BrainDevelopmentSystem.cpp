// Brain development system implementation
// Handles development system initialization and operations

#include "brain/BrainDevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

void BrainDevelopmentSystem::initializeDevelopmentSystem(Brain* brain) {
    if (!brain->pImpl->developmentSystem) {
        brain->pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    brain->pImpl->developmentSystem->initialize(brain);
}

} // namespace nlm
