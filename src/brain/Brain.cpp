// Brain facade - forwards to BrainCore implementation
// This file provides the simple Brain interface for users
// The real implementation is in BrainCore

#pragma once

#include "BrainCore.hpp"

namespace nlm {

// Convenient facade - BrainCore is the real implementation
using Brain = nlm::BrainCore;

} // namespace nlm