// Brain Test Suite
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation();
void testBrainInitialization();
void testBrainStep();
void testBrainReset();
void testBrainRegions();
void testBrainActionProduction();

void runAllBrainTests() {
    std::cout << "============================================" << std::endl;
    std::cout << "Running Brain Tests" << std::endl;
    std::cout << "============================================" << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    
    std::cout << "============================================" << std::endl;
    std::cout << "All Brain Tests Passed! ✓" << std::endl;
    std::cout << "============================================" << std::endl;
}

} // namespace test_brain
