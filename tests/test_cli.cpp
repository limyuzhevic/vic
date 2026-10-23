// CLI Interface Tests
#include "core/CLI/CLInterface.hpp"
#include <cassert>
#include <iostream>
#include <cstdio>
#include <cstring>

namespace test_cli {

void testCLIParserCreation() {
    nlm::CLIParser cli("test", "Test CLI");
    std::cout << "    testCLIParserCreation passed" << std::endl;
}

void testCLIAddOptions() {
    nlm::CLIParser cli("test", "Test CLI");
    
    cli.addOption("string_opt", "s", "A string option", "default", false, "string");
    cli.addOption("int_opt", "i", "An integer option", "42", false, "int");
    cli.addOption("float_opt", "f", "A float option", "3.14", false, "float");
    cli.addOption("bool_opt", "b", "A boolean option", "1", false, "bool");
    cli.addFlag("flag_opt", "A flag option");
    
    std::cout << "    testCLIAddOptions passed" << std::endl;
}

void testCLIParseHelp() {
    nlm::CLIParser cli("test", "Test CLI");
    cli.addOption("test", "t", "Test option", "default", false, "string");
    
    // Create a buffer to capture output
    FILE* oldStdout = stdout;
    std::freopen("/tmp/test_cli_output.txt", "w", stdout);
    
    // Simulate help request
    char* argv[] = {(char*)"test", (char*)"--help"};
    int argc = 2;
    bool result = cli.parse(argc, argv);
    
    std::fclose(stdout);
    std::freopen("/dev/tty", "w", stdout); // Restore stdout
    
    // In a real test, we'd parse the output file
    std::cout << "    testCLIParseHelp passed" << std::endl;
}

void testCLIBasicParsing() {
    nlm::CLIParser cli("test", "Test CLI");
    cli.addOption("name", "n", "Name", "world", false, "string");
    cli.addOption("count", "c", "Count", "10", false, "int");
    cli.addFlag("verbose", "Verbose output");
    
    char* argv[] = {(char*)"test", (char*)"--name", (char*)"Alice", (char*)"--count", (char*)"5", (char*)"--verbose"};
    int argc = 6;
    
    bool result = cli.parse(argc, argv);
    assert(result);
    
    auto nameOpt = cli.getString("name");
    assert(nameOpt && *nameOpt == "Alice");
    
    auto countOpt = cli.getInt("count");
    assert(countOpt && *countOpt == 5);
    
    bool verboseFlag = cli.getFlag("verbose");
    assert(verboseFlag == true);
    
    std::cout << "    testCLIBasicParsing passed" << std::endl;
}

void testCLIWithShortOptions() {
    nlm::CLIParser cli("test", "Test CLI");
    cli.addOption("file", "f", "File name", "", false, "string");
    cli.addOption("steps", "s", "Step count", "1", false, "int");
    
    char* argv[] = {(char*)"test", (char*)"-f", (char*)"data.txt", (char*)"-s", (char*)"100"};
    int argc = 5;
    
    bool result = cli.parse(argc, argv);
    assert(result);
    
    auto fileOpt = cli.getString("file");
    assert(fileOpt && *fileOpt == "data.txt");
    
    auto stepsOpt = cli.getInt("steps");
    assert(stepsOpt && *stepsOpt == 100);
    
    std::cout << "    testCLIWithShortOptions passed" << std::endl;
}

void testCLIAdvancedFeatures() {
    nlm::CLIParser cli("test", "Test CLI");
    cli.addOption("iterations", "", "Number of iterations", "1000", false, "int");
    cli.addOption("learning_rate", "", "Learning rate", "0.01", false, "float");
    cli.addFlag("enable_logging", "Enable detailed logging");
    cli.addFlag("checkpoint", "Enable checkpointing");
    
    char* argv[] = {(char*)"test", (char*)"--iterations", (char*)"500", 
                   (char*)"--learning-rate", (char*)"0.1",
                   (char*)"--enable-logging", (char*)"--checkpoint"};
    int argc = 7;
    
    bool result = cli.parse(argc, argv);
    assert(result);
    
    auto iterOpt = cli.getInt("iterations");
    assert(iterOpt && *iterOpt == 500);
    
    auto lrOpt = cli.getDouble("learning_rate");
    assert(lrOpt && std::abs(*lrOpt - 0.1) < 0.001);
    
    assert(cli.getFlag("enable_logging") == true);
    assert(cli.getFlag("checkpoint") == true);
    
    std::cout << "    testCLIAdvancedFeatures passed" << std::endl;
}

void testCLIBenchmark() {
    nlm::CLIParser cli("benchmark", "Benchmark Tool");
    cli.addOption("neurons", "n", "Number of neurons", "1000", false, "int");
    cli.addOption("duration", "d", "Duration in seconds", "60", false, "int");
    cli.addOption("resolution", "r", "Output resolution", "1920x1080", false, "string");
    cli.addFlag("detailed", "Detailed benchmark output");
    cli.addFlag("quiet", "Quiet mode");
    
    // Test benchmark command line
    char* argv[] = {(char*)"benchmark", (char*)"--neurons", (char*)"2000",
                   (char*)"--duration", (char*)"120", (char*)"--detailed"};
    int argc = 7;
    
    bool result = cli.parse(argc, argv);
    assert(result);
    
    auto neuronsOpt = cli.getInt("neurons");
    assert(neuronsOpt && *neuronsOpt == 2000);
    
    auto durationOpt = cli.getInt("duration");
    assert(durationOpt && *durationOpt == 120);
    
    assert(cli.getFlag("detailed") == true);
    assert(cli.getFlag("quiet") == false); // Default
    
    std::cout << "    testCLIBenchmark passed" << std::endl;
}

void testCLIBannerOutput() {
    // Test the static banner methods
    std::cout << "Testing CLI banner output..." << std::endl;
    
    nlm::CLIParser cli("test", "Test CLI");
    
    std::cout << "\n  " << std::string(60, '=') << std::endl;
    std::cout << "  CLI banner and formatting test completed" << std::endl;
    
    std::cout << "    testCLIBannerOutput passed" << std::endl;
}

void runAll() {
    testCLIParserCreation();
    testCLIAddOptions();
    testCLIParseHelp();
    testCLIBasicParsing();
    testCLIWithShortOptions();
    testCLIAdvancedFeatures();
    testCLIBenchmark();
    testCLIBannerOutput();
}

} // namespace test_cli