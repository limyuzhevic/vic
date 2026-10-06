#pragma once

// Header file for config tests
// Tests for the nlm::Config class functionality

namespace test_config {
    // Test basic configuration object creation
    void testConfigCreation();
    
    // Test setting and getting different value types
    void testConfigSetAndGet();
    
    // Test getting values with default fallback
    void testConfigGetOr();
    
    // Test removing configuration keys
    void testConfigRemove();
    
    // Test getting all configuration keys
    void testConfigKeys();
    
    // Run all configuration tests
    void runAll();
}