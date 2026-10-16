// Simple compilation test for enhanced NLM Python bindings
// This test ensures the bindings compile correctly with the new memory management features

#include <pybind11/pybind11.h>
#include "../src/core/Config/Config.hpp"
#include "include/nlm/memory/EnhancedMemoryManager.hpp"

namespace py = pybind11;
namespace nlm {

// Simple test function to verify compilation
void testCompilation() {
    // Test basic Config functionality
    nlm::Config config;
    config.set("test_key", "test_value");
    
    // Test memory management functionality
    nlm::MemoryConfig memConfig = nlm::MemoryConfig::getDefaultConfig();
    nlm::MemoryStatsTracker::initialize(memConfig);
    
    // Test smart pointer creation
    auto smartPtr = nlm::makeSmartPointer(&config, "Config", "test");
    
    // Test memory tracking
    nlm::MemoryUtils::recordAllocation(smartPtr.get(), sizeof(nlm::Config), 
                                      "Config", __FILE__, __LINE__);
    
    // Test memory system shutdown
    nlm::MemoryUtils::shutdown();
}

PYBIND11_MODULE(test_enhanced, m) {
    m.doc() = R"pbdoc(
        Test module for enhanced NLM Python bindings compilation verification
        
        This module is used to test that the enhanced bindings compile correctly
        with all the new memory management features.
    )pbdoc";
    
    m.def("test_compilation", &testCompilation, "Test compilation of enhanced bindings");
}

} // namespace nlm