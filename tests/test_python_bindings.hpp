// Python Bindings Test Header
// Enhanced Python API testing for NLM

#pragma once

namespace test_python_bindings {
    void testConfigPythonicMethods();
    void testConfigConvenienceMethods();
    void testBrainPythonicInterface();
    void testAgentBrainPythonicFeatures();
    void testWorldPythonicInterface();
    void testCompleteSimulationPipeline();
    void testErrorHandling();
    void testMemoryManagement();
    void runAll();
}

// Include Python headers for pybind11 testing
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>