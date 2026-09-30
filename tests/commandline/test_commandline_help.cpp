// Command-line help system tests
// Tests for enhanced help system functionality

#include "core/CommandLine/CommandLineHelp.hpp"
#include "core/CommandLine/CommandLineOptions.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

namespace test_commandline_help {

void testHelpSystemInitialization() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    // Initialize options
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test that help system is properly initialized
    assert(help.getSectionTitle(nlm::HelpSection::Overview) == "NLM Command Line Interface");
    
    std::cout << "    testHelpSystemInitialization passed" << std::endl;
}

void testSectionHelpDisplay() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test that help can be displayed for each section without crashing
    help.displaySectionHelp(nlm::HelpSection::Performance);
    help.displaySectionHelp(nlm::HelpSection::Debugging);
    help.displaySectionHelp(nlm::HelpSection::Experiment);
    help.displaySectionHelp(nlm::HelpSection::Validation);
    help.displaySectionHelp(nlm::HelpSection::Output);
    
    std::cout << "    testSectionHelpDisplay passed" << std::endl;
}

void testBriefHelp() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test brief help display
    std::stringstream output;
    help.setOutputStream(output);
    help.displayBriefHelp();
    
    std::string helpText = output.str();
    assert(!helpText.empty());
    assert(helpText.find("Usage:") != std::string::npos || 
           helpText.find("Usage:") != std::string::npos); // Note: capitalization may vary
    
    std::cout << "    testBriefHelp passed" << std::endl;
}

void testArgumentHelp() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test that help can be displayed for specific arguments
    help.displayArgumentHelp("--threads");
    help.displayArgumentHelp("--debug-level");
    
    std::cout << "    testArgumentHelp passed" << std::endl;
}

void testOptionExamples() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test example display
    help.displayOptionExamples();
    
    std::cout << "    testOptionExamples passed" << std::endl;
}

void testCommonPatterns() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test common patterns display
    help.displayCommonPatterns();
    
    std::cout << "    testCommonPatterns passed" << std::endl;
}

void testTroubleshooting() {
    nlm::CommandLineHelp help;
    nlm::CommandLineOptions options;
    
    options.initializeAllOptions();
    help.setOptions(&options);
    
    // Test troubleshooting display
    help.displayTroubleshooting();
    
    std::cout << "    testTroubleshooting passed" << std::endl;
}

void runAll() {
    testHelpSystemInitialization();
    testSectionHelpDisplay();
    testBriefHelp();
    testArgumentHelp();
    testOptionExamples();
    testCommonPatterns();
    testTroubleshooting();
}

} // namespace test_commandline_help