// Simple test to verify Config template functions work
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config template functions..." << std::endl;
    
    nlm::Config config;
    
    // Test setValidated with basic types
    assert(config.setValidated("int_val", 42));
    assert(config.setValidated("double_val", 3.14));
    assert(config.setValidated("string_val", "test"));
    assert(config.setValidated("bool_val", true));
    
    // Test custom validator
    auto positive_validator = [](int value) { return value > 0; };
    assert(config.setValidated("positive_val", 10, nlm::ConfigSource::Runtime, positive_validator));
    assert(!config.setValidated("positive_val", -5, nlm::ConfigSource::Runtime, positive_validator));
    
    // Test validator helpers
    auto int_validator = nlm::Config::createRangeValidator<int>(1, 10);
    assert(int_validator(5));
    assert(!int_validator(15));
    
    auto positive_int_validator = nlm::Config::createPositiveValidator<int>();
    assert(positive_int_validator(1));
    assert(!positive_int_validator(-1));
    
    std::cout << "All Config template function tests passed!" << std::endl;
    return 0;
}