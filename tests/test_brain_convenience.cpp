// Brain Convenience Method Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <map>

namespace test_brain_convenience {

void testConfigureConvenience();
void testStepWithStats();
void testGetSimplifiedStats();
void testIsReady();
void testCreateCopy();
void testConvenienceMethodChaining();

void runAllConvenienceTests() {
    std::cout << "Running Brain Convenience tests..." << std::endl;
    
    testConfigureConvenience();
    testStepWithStats();
    testGetSimplifiedStats();
    testIsReady();
    testCreateCopy();
    testConvenienceMethodChaining();
    
    std::cout << std::endl;
    std::cout << "All convenience method tests passed!" << std::endl;
}

} // namespace test_brain_convenience