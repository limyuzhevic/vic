// Phase2Demo.hpp - Header for Phase 2 demo in unified CLI
//
// This provides the Phase 2 demo interface for the unified CLI system.

#pragma once

#include "core/Config/Config.hpp"

using namespace nlm;

class Phase2Demo {
public:
    Phase2Demo() = default;
    ~Phase2Demo() = default;
    
    // Disable copying and moving
    Phase2Demo(const Phase2Demo&) = delete;
    Phase2Demo& operator=(const Phase2Demo&) = delete;
    Phase2Demo(Phase2Demo&&) = delete;
    Phase2Demo& operator=(Phase2Demo&&) = delete;
    
    // Run the Phase 2 demo
    bool run(const Config& config);
};
