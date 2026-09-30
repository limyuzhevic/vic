// Command-line option tests
// Tests for CommandLineOptions registration and validation

#include "core/CommandLine/CommandLineOptions.hpp"
#include <cassert>
#include <iostream>

namespace test_commandline_options {

void testOptionsRegistration() {
    nlm::CommandLineOptions options;
    
    // Initialize all options
    options.initializeAllOptions();
    
    // Check that options were registered
    assert(options.size() > 0);
    
    // Get options from different categories
    auto performanceOptions = options.getOptionsInCategory(nlm::OptionCategory::Performance);
    assert(!performanceOptions.empty());
    
    auto debugOptions = options.getOptionsInCategory(nlm::OptionCategory::Debugging);
    assert(!debugOptions.empty());
    
    auto experimentOptions = options.getOptionsInCategory(nlm::OptionCategory::Experiment);
    assert(!experimentOptions.empty());
    
    auto validationOptions = options.getOptionsInCategory(nlm::OptionCategory::Validation);
    assert(!validationOptions.empty());
    
    auto outputOptions = options.getOptionsInCategory(nlm::OptionCategory::Output);
    assert(!outputOptions.empty());
    
    std::cout << "    testOptionsRegistration passed" << std::endl;
}

void testOptionRetrieval() {
    nlm::CommandLineOptions options;
    options.initializeAllOptions();
    
    // Test getting option by long name
    const nlm::OptionMetadata* perfOption = options.getOption("--threads");
    assert(perfOption != nullptr);
    assert(perfOption->longName == "--threads");
    assert(perfOption->type == "int");
    
    // Test getting option by short name
    const nlm::OptionMetadata* simdOption = options.getOptionByShort("-simd");
    assert(simdOption != nullptr);
    assert(simdOption->shortName == "-simd");
    
    // Test option validator
    if (perfOption->validator) {
        assert(perfOption->validator("5"));  // Valid thread count
        assert(!perfOption->validator("0")); // Invalid thread count
        assert(!perfOption->validator("-1")); // Invalid thread count
    }
    
    std::cout << "    testOptionRetrieval passed" << std::endl;
}

void testOptionCategories() {
    nlm::CommandLineOptions options;
    options.initializeAllOptions();
    
    // Get active categories
    auto categories = options.getActiveCategories();
    assert(!categories.empty());
    
    // Check that key categories are present
    bool hasPerformance = false;
    bool hasDebugging = false;
    bool hasExperiment = false;
    bool hasValidation = false;
    bool hasOutput = false;
    
    for (auto cat : categories) {
        if (cat == nlm::OptionCategory::Performance) hasPerformance = true;
        if (cat == nlm::OptionCategory::Debugging) hasDebugging = true;
        if (cat == nlm::OptionCategory::Experiment) hasExperiment = true;
        if (cat == nlm::OptionCategory::Validation) hasValidation = true;
        if (cat == nlm::OptionCategory::Output) hasOutput = true;
    }
    
    assert(hasPerformance);
    assert(hasDebugging);
    assert(hasExperiment);
    assert(hasValidation);
    assert(hasOutput);
    
    std::cout << "    testOptionCategories passed" << std::endl;
}

void testOptionHelpGeneration() {
    nlm::CommandLineOptions options;
    options.initializeAllOptions();
    
    // Test that help can be generated without crashing
    // (This is more of an integration test)
    options.displayHelpByCategory();
    
    std::cout << "    testOptionHelpGeneration passed" << std::endl;
}

void runAll() {
    testOptionsRegistration();
    testOptionRetrieval();
    testOptionCategories();
    testOptionHelpGeneration();
}

} // namespace test_commandline_options