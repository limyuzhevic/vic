// NLM Test Framework
// Comprehensive test suite for NLM project

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_benchmark.hpp>

// Include existing test files
#include "tests/test_main.cpp"
#include "tests/test_types.cpp"
#include "tests/test_config.cpp"
#include "tests/test_random.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_performance.cpp"
#include "tests/test_refactored_agentbrain.cpp"

// Include new test files (to be created)
#include "tests/test_agentbrain.cpp"
#include "tests/test_config_enhanced.cpp"
#include "tests/test_visualization.cpp"
#include "tests/test_metrics.cpp"
#include "tests/test_research_tools.cpp"
#include "tests/test_integration.cpp"
#include "tests/test_e2e.cpp"
#include "tests/test_benchmarks.cpp"

int main(int argc, char* argv[]) {
    Catch::Session session;
    
    // Configure test session
    session.configData().showSuccessfulTests = true;
    session.configData().showSkippedTests = true;
    session.configData().showDifficulty = true;
    session.configData().allowThrows = true;
    session.configData().warnAboutNoTests = true;
    
    return session.run(argc, argv);
}