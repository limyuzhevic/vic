// ConfigTest.hpp
// Header file for configuration validation tests

#pragma once

namespace test_config {

void testDuplicateDopamineBaselineFix();
void testConfigurationValidation();
void testConfigurationBackwardCompatibility();
void runAllConfigTests();

} // namespace test_config