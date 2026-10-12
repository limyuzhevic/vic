#include "Brain.hpp"

namespace nlm {

// Clean Brain.cpp implementation
// Most functionality has been moved to dedicated files:
// - BrainInitialization.cpp: Constructor implementations
// - BrainSimulationStep.cpp: Step simulation and sub-steps
// - BrainInputOutput.cpp: Sensory/motor I/O operations
// - BrainSystemAccessors.cpp: System accessors and statistics
// - BrainCheckpointIO.cpp: Checkpoint I/O operations
// - BrainRegionManagement.cpp: Region management
// - BrainPlasticity.cpp: Neuromodulation and plasticity

// Move constructor - now works with std::unique_ptr
Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

// Move assignment operator - now works with std::unique_ptr
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Provide external declarations for the new Brain files
// All method implementations are now in separate files

} // namespace nlm