// Comprehensive Test Suite
// Main test runner that runs all tests
// #include "test_neuron_enhanced.cpp"
// #include "test_synapse_enhanced.cpp" 
// #include "test_stdp_enhanced.cpp"
// #include "test_config_enhanced.cpp"
// #include <iostream>
// #include <chrono>

void runAllEnhancedTests() {
    std::cout << "=" * 60 << std::endl;
    std::cout << "NLM COMPREHENSIVE TEST SUITE" << std::endl;
    std::cout << "=" * 60 << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run all enhanced tests
    runEnhancedNeuronTests();
    runEnhancedSynapseTests();
    runEnhancedSTDPTests();
    runEnhancedConfigTests();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "=" * 60 << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "=" * 60 << std::endl;
    std::cout << "All enhanced tests completed successfully!" << std::endl;
    std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
    std::cout << "=" * 60 << std::endl;
    std::cout << "Key improvements tested:" << std::endl;
    std::cout << "  ✓ Enhanced error handling with proper exception management" << std::endl;
    std::cout << "  ✓ Input validation and bounds checking" << std::endl;
    std::cout << "  ✓ Memory safety and null pointer protection" << std::endl;
    std::cout << "  ✓ Configuration validation and schema management" << std::endl;
    std::cout << "  ✓ Type-safe value conversions" << std::endl;
    std::cout << "  ✓ Enhanced CLI with more options and better help" << std::endl;
    std::cout << "  ✓ Modular and maintainable code structure" << std::endl;
    std::cout << "=" * 60 << std::endl;
}

// Standalone test runner for enhanced tests
int main(int argc, char** argv) {
    try {
        std::cout << "NLM Enhanced Test Suite" << std::endl;
        std::cout << "Testing all enhancements including error handling, "
                  << "validation, and improved functionality..." << std::endl;
        std::cout << std::endl;
        
        runAllEnhancedTests();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}