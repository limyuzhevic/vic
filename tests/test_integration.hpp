// Integration Tests for NLM Phase 6: Full AgentBrain Integration
// 
// These tests verify the complete integration of all cognitive systems
// in the AgentBrain, demonstrating the full pipeline from sensory input
// to action selection with proper world loop integration.

#ifndef TEST_INTEGRATION_HPP
#define TEST_INTEGRATION_HPP

#include <memory>
#include <string>

namespace nlm {

class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class ConsoleLogger;
class Logger;
}

// Integration test namespace
namespace test_integration {

// Main integration test suite
void runAll();

} // namespace test_integration

#endif // TEST_INTEGRATION_HPP