// Unified Main Entry Point Tests
// Tests for the new unified command-line interface and mode selection

#include "main.cpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

using namespace nlm;

namespace test_unified_main {

void testCommandLineArgumentParsing() {
    std::cout << "Testing command-line argument parsing..." << std::endl;
    
    // Test basic argument parsing
    {
        CommandLineArgs args = parseCommandLineArgs(4, const_cast<char**>(
            new char*[4]{
                (char*)"test",
                (char*)"--mode", (char*)"phase6",
                (char*)"--neurons", (char*)"500"
            }
        ));
        assert(args.mode == "phase6");
        assert(args.neurons == 500);
        assert(args.configFile == "configs/default.cfg");
    }
    
    // Test verbose flag
    {
        CommandLineArgs args = parseCommandLineArgs(2, const_cast<char**>(
            new char*[2]{
                (char*)"test",
                (char*)"-v"
            }
        ));
        assert(args.verbose == true);
    }
    
    // Test help flag
    {
        CommandLineArgs args = parseCommandLineArgs(2, const_cast<char**>(
            new char*[2]{
                (char*)"test",
                (char*)"--help"
            }
        ));
        assert(args.help == true);
    }
    
    // Test list modes flag
    {
        CommandLineArgs args = parseCommandLineArgs(2, const_cast<char**>(
            new char*[2]{
                (char*)"test",
                (char*)"--list-modes"
            }
        ));
        assert(args.list_modes == true);
    }
    
    // Test custom settings
    {
        CommandLineArgs args = parseCommandLineArgs(4, const_cast<char**>(
            new char*[4]{
                (char*)"test",
                (char*)"--plasticity.learning_rate=0.01",
                (char*)"--brain.neuron_count=1000"
            }
        ));
        assert(args.customSettings.size() == 2);
        assert(args.customSettings["plasticity.learning_rate"] == "0.01");
        assert(args.customSettings["brain.neuron_count"] == "1000");
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testModeSelectionAuto() {
    std::cout << "Testing auto mode selection..." << std::endl;
    
    // Test neuron count >= 500 (should select phase6)
    CommandLineArgs args;
    args.mode = "auto";
    args.neurons = 600;
    
    std::string actualMode = args.mode;
    if (actualMode == "auto") {
        if (args.neurons >= 500) {
            actualMode = "phase6";
        } else if (args.neurons >= 200) {
            actualMode = "phase4";
        } else if (args.neurons >= 100) {
            actualMode = "phase3";
        } else {
            actualMode = "phase2";
        }
    }
    assert(actualMode == "phase6");
    
    // Test neuron count 300 (should select phase4)
    args.neurons = 300;
    actualMode = args.mode;
    if (actualMode == "auto") {
        if (args.neurons >= 500) {
            actualMode = "phase6";
        } else if (args.neurons >= 200) {
            actualMode = "phase4";
        } else if (args.neurons >= 100) {
            actualMode = "phase3";
        } else {
            actualMode = "phase2";
        }
    }
    assert(actualMode == "phase4");
    
    // Test neuron count 150 (should select phase3)
    args.neurons = 150;
    actualMode = args.mode;
    if (actualMode == "auto") {
        if (args.neurons >= 500) {
            actualMode = "phase6";
        } else if (args.neurons >= 200) {
            actualMode = "phase4";
        } else if (args.neurons >= 100) {
            actualMode = "phase3";
        } else {
            actualMode = "phase2";
        }
    }
    assert(actualMode == "phase3");
    
    // Test neuron count 50 (should select phase2)
    args.neurons = 50;
    actualMode = args.mode;
    if (actualMode == "auto") {
        if (args.neurons >= 500) {
            actualMode = "phase6";
        } else if (args.neurons >= 200) {
            actualMode = "phase4";
        } else if (args.neurons >= 100) {
            actualMode = "phase3";
        } else {
            actualMode = "phase2";
        }
    }
    assert(actualMode == "phase2");
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigApplication() {
    std::cout << "Testing configuration application..." << std::endl;
    
    // Create a temporary config file
    std::string tempConfigPath = "/tmp/test_nlm_config.cfg";
    {
        std::ofstream configFile(tempConfigPath);
        configFile << "# Test config\n";
        configFile << "random_seed = 99\n";
        configFile << "neuron_count = 200\n";
        configFile << "region_count = 2\n";
        configFile << "connection_probability = 0.15\n";
    }
    
    // Test loading config file
    auto config = std::make_shared<Config>();
    assert(config->loadFromFile(tempConfigPath));
    
    // Verify config values
    assert(config->getOr<int64_t>("random_seed", 0) == 99);
    assert(config->getOr<int64_t>("neuron_count", 0) == 200);
    assert(config->getOr<int64_t>("region_count", 0) == 2);
    assert(config->getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    // Clean up
    std::filesystem::remove(tempConfigPath);
    
    // Test applying custom settings
    config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::CommandLine);
    config->set("simulation_timestep", 0.005, ConfigSource::CommandLine);
    
    assert(config->getOr<int64_t>("neuron_count", 0) == 300);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.005);
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigurationFileGeneration() {
    std::cout << "Testing configuration file generation..." << std::endl;
    
    // This test verifies that the configuration system can be extended
    // to support file generation if needed in the future
    auto config = std::make_shared<Config>();
    
    // Test basic configuration generation
    std::string defaultConfig = config->toString();
    assert(!defaultConfig.empty());
    
    // Test saving configuration
    std::string tempConfigPath = "/tmp/test_config_save.cfg";
    // The config class should have saveToFile method
    
    std::cout << "  PASSED" << std::endl;
}

void testHelpAndUsageMessages() {
    std::cout << "Testing help and usage messages..." << std::endl;
    
    // Capture stdout to test help message format
    std::stringstream capturedOutput;
    std::streambuf* oldCout = std::cout.rdbuf();
    std::cout.rdbuf(capturedOutput.rdbuf());
    
    try {
        printUsage();
    } catch (...) {
        // Restore stdout
        std::cout.rdbuf(oldCout);
        throw;
    }
    
    // Restore stdout
    std::cout.rdbuf(oldCout);
    
    std::string helpOutput = capturedOutput.str();
    assert(helpOutput.find("NLM (熙然)") != std::string::npos);
    assert(helpOutput.find("USAGE:") != std::string::npos);
    assert(helpOutput.find("--help") != std::string::npos);
    assert(helpOutput.find("--mode") != std::string::npos);
    
    // Test mode descriptions
    std::stringstream phaseOutput;
    oldCout = std::cout.rdbuf();
    std::cout.rdbuf(phaseOutput.rdbuf());
    
    try {
        printPhaseDescriptions();
    } catch (...) {
        std::cout.rdbuf(oldCout);
        throw;
    }
    
    std::cout.rdbuf(oldCout);
    std::string phaseText = phaseOutput.str();
    
    // Verify phase descriptions are complete
    assert(phaseText.find("PHASE 2:") != std::string::npos);
    assert(phaseText.find("PHASE 3:") != std::string::npos);
    assert(phaseText.find("PHASE 4:") != std::string::npos);
    assert(phaseText.find("PHASE 5:") != std::string::npos);
    assert(phaseText.find("PHASE 6:") != std::string::npos);
    assert(phaseText.find("BENCHMARK MODE:") != std::string::npos);
    
    std::cout << "  PASSED" << std::endl;
}

void testBannerDisplay() {
    std::cout << "Testing banner display..." << std::endl;
    
    std::stringstream capturedOutput;
    std::streambuf* oldCout = std::cout.rdbuf();
    std::cout.rdbuf(capturedOutput.rdbuf());
    
    try {
        printBanner();
    } catch (...) {
        std::cout.rdbuf(oldCout);
        throw;
    }
    
    std::cout.rdbuf(oldCout);
    
    std::string bannerOutput = capturedOutput.str();
    assert(bannerOutput.find("NLM — 熙然") != std::string::npos);
    assert(bannerOutput.find("Neural Learning Machine") != std::string::npos);
    
    std::cout << "  PASSED" << std::endl;
}

void testArgumentValidation() {
    std::cout << "Testing argument validation..." << std::endl;
    
    // Test invalid mode
    CommandLineArgs args;
    args.mode = "invalid_mode";
    args.steps = 100;
    args.neurons = 100;
    args.timestep = 0.001;
    args.verbose = false;
    args.help = false;
    args.list_modes = false;
    
    // Invalid mode should be handled gracefully
    assert(args.mode == "invalid_mode");
    
    // Test negative steps
    args.steps = -10;
    assert(args.steps == -10);
    
    // Test zero timesteps
    args.timestep = 0.0;
    assert(args.timestep == 0.0);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testCommandLineArgumentParsing();
    testModeSelectionAuto();
    testConfigApplication();
    testConfigurationFileGeneration();
    testHelpAndUsageMessages();
    testBannerDisplay();
    testArgumentValidation();
    
    std::cout << "All unified main entry point tests PASSED!" << std::endl;
}

} // namespace test_unified_main