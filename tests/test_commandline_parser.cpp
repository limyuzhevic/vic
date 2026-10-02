// Test for CommandLineParser
#include "application/CommandLineParser.hpp"
#include <cassert>
#include <iostream>

namespace test_commandline_parser {
    void runAll() {
        std::cout << "Testing CommandLineParser..." << std::endl;
        
        // Test default values
        CommandLineParser parser;
        assert(parser.getConfigFile() == "configs/default.cfg");
        assert(parser.getLogLevel() == "INFO");
        assert(parser.getExperimentType() == 0);
        assert(parser.getTimestep() == 0.001);
        assert(parser.getNeuronCount() == 500);
        assert(parser.getRegionCount() == 1);
        assert(parser.getConnectionProbability() == 0.15f);
        assert(parser.getMaxSteps() == 10000);
        
        // Test help and version (would normally exit, so we can't test this)
        // Test parsing of arguments (would normally be done in main)
        
        std::cout << "  PASSED" << std::endl;
    }
}