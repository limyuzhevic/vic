#pragma once

// Forward declarations for all Brain integration classes
class Brain;  // Forward declares BrainCore (the real implementation)

namespace nlm {

// Convenient facade - BrainCore is the real implementation
using Brain = nlm::BrainCore;

} // namespace nlm
