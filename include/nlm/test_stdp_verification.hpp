// Test 3: STDP Verification Test Header
// Improved version with focused testing and better error handling

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm
