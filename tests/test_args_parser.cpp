#include "ArgsParser.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

void testArgMetadata() {
    std::cout << "Testing ArgMetadata..." << std::endl;
    
    ArgMetadata coreArg("neuron_count", "Number of neurons in the brain",
                       "--neuron_count=1000", "500", false,
                       ArgsCategory::Core, "n", "neuron-count");
    
    assert(coreArg.name == "neuron_count");
    assert(coreArg.description == "Number of neurons in the brain");
    assert(coreArg.example == "--neuron_count=1000");
    assert(coreArg.defaultValue == "500");
    assert(coreArg.required == false);
    assert(coreArg.category == ArgsCategory::Core);
    assert(coreArg.hasShortOption == true);
    assert(coreArg.shortOption == "n");
    assert(coreArg.hasLongOption == true);
    assert(coreArg.longOption == "neuron-count");
    
    std::cout << "✓ ArgMetadata test passed" << std::endl;
}

void testCmdArg() {
    std::cout << "Testing CmdArg..." << std::endl;
    
    CmdArg cmdArg("v", "verbose", "true", true);
    
    assert(cmdArg.shortOption == "v");
    assert(cmdArg.longOption == "verbose");
    assert(cmdArg.value == "true");
    assert(cmdArg.hasValue == true);
    
    std::cout << "✓ CmdArg test passed" << std::endl;
}

void testFileFormatDetector() {
    std::cout << "Testing FileFormatDetector..." << std::endl;
    
    // Create test files
    std::ofstream jsonFile("test.json");
    jsonFile << "{\"neuron_count\": 1000}";
    jsonFile.close();
    
    std::ofstream yamlFile("test.yaml");
    yamlFile << "neuron_count: 1000";
    yamlFile.close();
    
    std::ofstream iniFile("test.ini");
    iniFile << "[section]\nneuron_count = 1000";
    iniFile.close();
    
    std::ofstream plainFile("test.cfg");
    plainFile << "neuron_count = 1000";
    plainFile.close();
    
    // Test JSON detection
    FileFormatInfo jsonInfo = FileFormatDetector::detectFormat("test.json");
    assert(jsonInfo.isValid);
    assert(jsonInfo.detectedFormat == "JSON");
    
    // Test YAML detection
    FileFormatInfo yamlInfo = FileFormatDetector::detectFormat("test.yaml");
    assert(yamlInfo.isValid);
    assert(yamlInfo.detectedFormat == "YAML");
    
    // Test INI detection
    FileFormatInfo iniInfo = FileFormatDetector::detectFormat("test.ini");
    assert(iniInfo.isValid);
    assert(iniInfo.detectedFormat == "INI");
    
    // Test plain detection
    FileFormatInfo plainInfo = FileFormatDetector::detectFormat("test.cfg");
    assert(plainInfo.isValid);
    assert(plainInfo.detectedFormat == "PLAIN");
    
    // Clean up test files
    std::remove("test.json");
    std::remove("test.yaml");
    std::remove("test.ini");
    std::remove("test.cfg");
    
    std::cout << "✓ FileFormatDetector test passed" << std::endl;
}

void testCmdLineParser() {
    std::cout << "Testing CmdLineParser..." << std::endl;
    
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--neuron-count",
        (char*)"1000",
        (char*)"-v",
        (char*)"--enable-plasticity"
    };
    
    std::vector<CmdArg> cmdArgs = CmdLineParser::parse(argc, argv);
    
    assert(cmdArgs.size() == 3);
    assert(cmdArgs[0].longOption == "neuron-count");
    assert(cmdArgs[0].value == "1000");
    assert(cmdArgs[0].hasValue == true);
    assert(cmdArgs[1].shortOption == "v");
    assert(cmdArgs[2].longOption == "enable-plasticity");
    assert(cmdArgs[2].value == "");
    
    std::cout << "✓ CmdLineParser test passed" << std::endl;
}

void testArgsParserBasic() {
    std::cout << "Testing ArgsParser basic functionality..." << std::endl;
    
    ArgsParser parser;
    
    // Register some arguments
    ArgMetadata neuronArg("neuron_count", "Number of neurons",
                         "--neuron-count=1000", "500", false,
                         ArgsCategory::Core, "n", "neuron-count");
    
    ArgMetadata plasticityArg("enable_plasticity", "Enable plasticity",
                             "--enable-plasticity=true", "false", false,
                             ArgsCategory::Plasticity, "p", "enable-plasticity");
    
    parser.registerArgument(neuronArg);
    parser.registerArgument(plasticityArg);
    
    // Test parsing command line
    int argc = 3;
    char* argv[] = {
        (char*)"program",
        (char*)"--neuron-count",
        (char*)"1000"
    };
    
    bool success = parser.parse(argc, argv);
    assert(success);
    
    assert(parser.has("neuron_count"));
    assert(!parser.has("enable_plasticity")); // Not set
    
    auto neuronVal = parser.get<int64_t>("neuron_count");
    assert(neuronVal.has_value());
    assert(neuronVal.value() == 1000);
    
    auto plasticityVal = parser.get<bool>("enable_plasticity");
    assert(!plasticityVal.has_value()); // Not set, should return nullopt
    
    std::cout << "✓ ArgsParser basic test passed" << std::endl;
}

void testArgsParserConfigFile() {
    std::cout << "Testing ArgsParser config file parsing..." << std::endl;
    
    ArgsParser parser;
    
    // Create a test config file
    std::ofstream configFile("test_config.cfg");
    configFile << "# Test configuration file" << std::endl;
    configFile << "neuron_count = 2000" << std::endl;
    configFile << "enable_plasticity = true" << std::endl;
    configFile << "simulation_timestep = 0.005" << std::endl;
    configFile.close();
    
    bool success = parser.parseConfigFile("test_config.cfg");
    assert(success);
    
    assert(parser.has("neuron_count"));
    assert(parser.has("enable_plasticity"));
    assert(parser.has("simulation_timestep"));
    
    auto neuronVal = parser.get<int64_t>("neuron_count");
    assert(neuronVal.value() == 2000);
    
    auto plasticityVal = parser.get<bool>("enable_plasticity");
    assert(plasticityVal.value() == true);
    
    auto timestepVal = parser.get<double>("simulation_timestep");
    assert(timestepVal.value() == 0.005);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "✓ ArgsParser config file test passed" << std::endl;
}

void testHelpSystem() {
    std::cout << "Testing help system..." << std::endl;
    
    ArgsParser parser;
    
    // Register some arguments
    ArgMetadata coreArg("neuron_count", "Number of neurons",
                       "--neuron-count=1000", "500", false,
                       ArgsCategory::Core, "n", "neuron-count");
    
    ArgMetadata brainArg("brain_type", "Type of brain architecture",
                        "--brain-type=standard", "standard", false,
                        ArgsCategory::Brain, "b", "brain-type");
    
    parser.registerArgument(coreArg);
    parser.registerArgument(brainArg);
    
    std::string help = parser.getHelp();
    
    assert(help.find("=== Argument Parser Help ===") != std::string::npos);
    assert(help.find("Core Parameters") != std::string::npos);
    assert(help.find("Brain Configuration") != std::string::npos);
    assert(help.find("--neuron-count") != std::string::npos);
    assert(help.find("--brain-type") != std::string::npos);
    
    std::cout << "✓ Help system test passed" << std::endl;
}

void testSummary() {
    std::cout << "Testing summary generation..." << std::endl;
    
    ArgsParser parser;
    
    // Set some values
    parser.set("neuron_count", static_cast<int64_t>(1000));
    parser.set("enable_plasticity", true);
    parser.set("simulation_timestep", 0.001);
    
    std::string summary = parser.summary();
    
    assert(summary.find("=== Configuration Summary ===") != std::string::npos);
    assert(summary.find("neuron_count") != std::string::npos);
    assert(summary.find("enable_plasticity") != std::string::npos);
    assert(summary.find("simulation_timestep") != std::string::npos);
    
    std::cout << "✓ Summary test passed" << std::endl;
}

void testPresets() {
    std::cout << "Testing presets..." << std::endl;
    
    ArgsParser parser;
    
    // Create a preset
    Preset standardPreset("standard", "Standard configuration for general use", 1.0);
    parser.addPreset(standardPreset);
    
    // Test preset application
    bool success = parser.applyPreset("standard");
    assert(success);
    
    // Preset should have added some default values
    std::cout << "Preset applied successfully" << std::endl;
    
    std::cout << "✓ Presets test passed" << std::endl;
}

void testValidation() {
    std::cout << "Testing validation..." << std::endl;
    
    ArgsParser parser;
    
    // Register arguments with validation
    ArgMetadata validArg("neuron_count", "Number of neurons",
                        "--neuron-count=1000", "500", false,
                        ArgsCategory::Core, "n", "neuron-count");
    parser.registerArgument(validArg);
    
    // Set an out-of-range value (in real implementation, this would be caught by validation)
    parser.set("neuron_count", static_cast<int64_t>(-1));
    
    std::vector<std::string> errors;
    bool valid = parser.validate(errors);
    
    // Validation should catch the error
    assert(!valid);
    assert(!errors.empty());
    
    std::cout << "✓ Validation test passed" << std::endl;
}

void testValueParsing() {
    std::cout << "Testing value parsing..." << std::endl;
    
    ArgsParser parser;
    
    // Test integer parsing
    ArgsValue intVal = parser.parseValue("1000");
    assert(std::holds_alternative<int64_t>(intVal));
    assert(std::get<int64_t>(intVal) == 1000);
    
    // Test double parsing
    ArgsValue doubleVal = parser.parseValue("0.005");
    assert(std::holds_alternative<double>(doubleVal));
    assert(std::get<double>(doubleVal) == 0.005);
    
    // Test boolean parsing
    ArgsValue boolVal = parser.parseValue("true");
    assert(std::holds_alternative<bool>(boolVal));
    assert(std::get<bool>(boolVal) == true);
    
    // Test string parsing
    ArgsValue stringVal = parser.parseValue("test_string");
    assert(std::holds_alternative<std::string>(stringVal));
    assert(std::get<std::string>(stringVal) == "test_string");
    
    // Test vector parsing
    ArgsValue vectorVal = parser.parseValue("[1, 2, 3]");
    assert(std::holds_alternative<std::vector<double>>(vectorVal));
    auto vec = std::get<std::vector<double>>(vectorVal);
    assert(vec.size() == 3);
    assert(vec[0] == 1.0);
    assert(vec[1] == 2.0);
    assert(vec[2] == 3.0);
    
    std::cout << "✓ Value parsing test passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    ArgsParser parser;
    
    // Try to parse invalid command line
    int argc = 2;
    char* argv[] = {
        (char*)"program",
        (char*)"--invalid-option"
    };
    
    bool success = parser.parse(argc, argv);
    assert(!success);
    
    // Check that error was recorded
    std::vector<std::string> errors = parser.getErrors();
    assert(!errors.empty());
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

void testFileFormatDetection() {
    std::cout << "Testing comprehensive file format detection..." << std::endl;
    
    // Test various file formats
    std::vector<std::pair<std::string, std::string>> testFiles = {
        {"test.json", "{\"test\": 1}"},
        {"test.yaml", "test: 1"},
        {"test.toml", "[test]\nvalue = 1"},
        {"test.ini", "[section]\nkey = value"},
        {"test.cfg", "key = value"},
        {"test.txt", "key = value"}
    };
    
    for (const auto& testFile : testFiles) {
        std::ofstream file(testFile.first);
        file << testFile.second;
        file.close();
        
        FileFormatInfo formatInfo = FileFormatDetector::detectFormat(testFile.first);
        assert(formatInfo.isValid);
        
        std::remove(testFile.first.c_str());
    }
    
    std::cout << "✓ File format detection test passed" << std::endl;
}

int main() {
    std::cout << "Running NLM Argument Parser Tests..." << std::endl;
    std::cout << std::endl;
    
    try {
        testArgMetadata();
        testCmdArg();
        testFileFormatDetector();
        testCmdLineParser();
        testArgsParserBasic();
        testArgsParserConfigFile();
        testHelpSystem();
        testSummary();
        testPresets();
        testValidation();
        testValueParsing();
        testErrorHandling();
        testFileFormatDetection();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}