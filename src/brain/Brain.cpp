// Brain implementation
// Provides the main Brain class implementation by delegating to modular components

#include "Brain.hpp"

namespace nlm {

// Forward declaration for implementation
struct Brain::Impl;

// Constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Destructor
Brain::~Brain() = default;

// Move constructor
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// The implementation is now split into multiple modules:
// - BrainCore.cpp: Core brain initialization, construction, and basic operations
// - BrainIntegration.cpp: System integration and configuration
// - BrainAccessors.cpp: Accessor methods for brain systems

// This approach follows the Single Responsibility Principle and improves modularity

} // namespace nlm