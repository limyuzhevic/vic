// Phase6Demo.hpp - Header file for Phase 6 demo executable
#pragma once

/**
 * Phase6Demo - Phase 6 demo executable interface
 * 
 * This file provides the interface for the Phase 6 demo executable,
 * which demonstrates the integration of all brain systems in NLM.
 */

#include "src/demo/Phase6DemoCLI.hpp"

namespace nlm {

/**
 * Run Phase 6 demo executable
 * 
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 for success, 1 for failure)
 */
int runPhase6Demo(int argc, char* argv[]);

} // namespace nlm
