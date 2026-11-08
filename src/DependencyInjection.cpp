// Dependency Injection Framework implementation

#include "DependencyInjection.hpp"
#include <stdexcept>

namespace nlm {

// Initialize static member
DependencyContainer* ServiceLocator::container = nullptr;

} // namespace nlm
