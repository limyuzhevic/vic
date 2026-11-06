// Helper Functions for Main Application Header
// Contains utility functions for brain monitoring and status reporting

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

void printBrainSummary(const std::shared_ptr<Brain>& brain);

} // namespace nlm
