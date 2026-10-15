#pragma once

#include "TestBase.hpp"
#include "TestRegistry.hpp"
#include "CLIParser.hpp"
#include "TestConfig.hpp"
#include "TestRunner.hpp"
#include "ProgressTracker.hpp"
#include "Reporter.hpp"
#include "ErrorHandler.hpp"
#include "PluginManager.hpp"
#include "ScenarioManager.hpp"

namespace nlm {

// Main test runner entry point
int runTests(int argc, char** argv);

// Utility functions
std::string generateTestReport(const std::vector<TestRunner::TestResult>& results);
void setupTestEnvironment(const TestConfig& config);
void cleanupTestEnvironment();

} // namespace nlm
