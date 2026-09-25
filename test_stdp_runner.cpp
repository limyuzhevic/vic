#include <iostream>
#include <cassert>

// Forward declare the test functions
namespace test_stdp {
    void testSTDPCreation();
    void testSTDPConfiguration();
    void testSTDPConfigure();
    void testSTDPName();
    void testSTDPPotentiation();
    void testSTDPDepression();
    void testSTDPEmptySpikes();
    void testSTDPWeightBounds();
}

int main() {
    std::cout << "Running STDP tests..." << std::endl;
    
    try {
        test_stdp::testSTDPCreation();
        std::cout << "  testSTDPCreation passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPCreation FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPCreation FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPConfiguration();
        std::cout << "  testSTDPConfiguration passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPConfiguration FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPConfiguration FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPConfigure();
        std::cout << "  testSTDPConfigure passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPConfigure FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPConfigure FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPName();
        std::cout << "  testSTDPName passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPName FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPName FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPPotentiation();
        std::cout << "  testSTDPPotentiation passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPPotentiation FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPPotentiation FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPDepression();
        std::cout << "  testSTDPDepression passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPDepression FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPDepression FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPEmptySpikes();
        std::cout << "  testSTDPEmptySpikes passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPEmptySpikes FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPEmptySpikes FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    try {
        test_stdp::testSTDPWeightBounds();
        std::cout << "  testSTDPWeightBounds passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  testSTDPWeightBounds FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "  testSTDPWeightBounds FAILED: Unknown error" << std::endl;
        return 1;
    }
    
    std::cout << "=== All STDP tests PASSED ===" << std::endl;
    return 0;
}