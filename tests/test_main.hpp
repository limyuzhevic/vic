#pragma once

// Header file for main integration tests
// Tests for the main() function and top-level integration
// including configuration loading, argument parsing, and initialization sequence

namespace test_main {
    // Test main function with no arguments (default behavior)
    void testMainWithoutArgs();
    
    // Test main function with configuration file argument
    void testMainWithConfigFile();
    
    // Test main function with command line arguments
    void testMainWithCommandLineArgs();
    
    // Test configuration loading sequence
    void testConfigurationLoading();
    
    // Test argument parsing edge cases
    void testArgumentParsing();
    
    // Test final system state after main execution
    void testFinalSystemState();
    
    // Run all main tests
    void runAll();
}