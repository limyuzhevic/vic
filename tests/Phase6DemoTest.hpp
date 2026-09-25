// Phase 6 Demo Test Header
// This file declares all test functions for Phase 6 integration testing

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "Phase6IntegrationFix.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace nlm {

/**
 * Test Phase 6 Demo integration
 */
void testPhase6Demo();

/**
 * Test Phase 6 Configuration
 */
void testPhase6Config();

/**
 * Test Phase 6 Integration Result
 */
void testPhase6IntegrationResult();

/**
 * Test Phase 6 Integration Fix
 */
void testPhase6IntegrationFix();

/**
 * Test Phase 6 Agent Integration
 */
void testPhase6AgentIntegration();

/**
 * Test Phase 6 Memory Integration
 */
void testPhase6MemoryIntegration();

/**
 * Test Phase 6 Neuromodulation Integration
 */
void testPhase6NeuromodulationIntegration();

/**
 * Run all Phase 6 tests
 */
void runAllPhase6Tests();

} // namespace nlm