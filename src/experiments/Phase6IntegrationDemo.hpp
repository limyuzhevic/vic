#pragma once

/**
 * Phase6IntegrationDemo.hpp - Header for Phase 6 Integration Demo
 * 
 * This file provides the main entry point for the Phase 6 integration demo.
 * The demo demonstrates that all Phase 6 systems are properly integrated
 * into a coherent artificial brain.
 * 
 * Key features demonstrated:
 * - Verification that all systems are connected (working memory, episodic memory,
 *   neuromodulation, prediction, development)
 * - Individual system integration tests
 * - Comprehensive integration experiment
 * - Checkpointing and replay capabilities
 * 
 * Note: This demo is the primary Phase 6 demonstration. For text-specific
 * demonstrations, use nlm_phase6_text_demo.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

/**
 * Phase 6 Integration Demo Main Function
 * 
 * Runs the complete Phase 6 integration test demonstrating that all
 * brain systems work together as a unified artificial brain.
 * 
 * @return 0 on success, 1 on integration verification failure
 */
int main(int argc, char* argv[]);

} // namespace nlm