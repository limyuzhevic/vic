// Phase6IntegrationTest.hpp
// Header file for Phase 6 integration tests

#pragma once

#include <memory>

namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
}

namespace test_phase6 {

void testRewardPredictionSystem();
void testCheckpointing();
void testConfiguration();
void testLearningEffectiveness();
void testBackwardCompatibility();
void runAllPhase6Tests();

} // namespace test_phase6