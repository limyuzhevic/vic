# Test skeleton for CLI integration
#include <gtest/gtest.h>
#include "AdvancedCLI.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class CLIIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize mock brain and config
        config = std::make_shared<Config>();
        logger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->setGlobal(logger);
        
        // Create mock brain
        brain = std::make_shared<Brain>(config);
        
        // Initialize advanced CLI
        advancedCLI = std::make_shared<AdvancedCLI>(config, brain, logger);
    }
    
    void TearDown() override {
        advancedCLI.reset();
        brain.reset();
        config.reset();
        logger.reset();
    }
    
protected:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AdvancedCLI> advancedCLI;
};

TEST_F(CLIIntegrationTest, OriginalCLIBackwardCompatibility) {
    // Test that original CLI still works
    int argc = 3;
    char* argv[] = {(char*)"nlm_cli", (char*)"--help", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runOriginalCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIBasic) {
    // Test basic advanced CLI functionality
    int argc = 2;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIBrainInfo) {
    // Test brain info command
    int argc = 4;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--brain-info", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLISimulation) {
    // Test simulation command
    int argc = 4;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--simulate", (char*)"10", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIInspection) {
    // Test inspection command
    int argc = 4;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--inspect-neurons", (char*)"5", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIAnalysis) {
    // Test analysis command
    int argc = 3;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--analyze-performance", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIDebugging) {
    // Test debugging command
    int argc = 3;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--debug-spike-events", (char*)"10", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLIExportImport) {
    // Test export/import command
    int argc = 4;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--export-neural-graph", (char*)"graph.json", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, AdvancedCLISystemIntegration) {
    // Test complete system integration
    int argc = 3;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--experiment-list", (char*)nullptr};
    
    EXPECT_TRUE(advancedCLI->runAdvancedCLI(argc, argv));
}

TEST_F(CLIIntegrationTest, CommandLineParsing) {
    // Test command line parsing
    advancedCLI->parseCommandLine(3, (char*[]) {(char*)"nlm_cli", (char*)"--config", (char*)"test.cfg", (char*)nullptr});
    
    EXPECT_EQ(advancedCLI->getOptions().configFile, "test.cfg");
    
    advancedCLI->parseAdvancedCommandLine(3, (char*[]) {(char*)"nlm_cli", (char*)"--advanced", (char*)"--brain-info", (char*)nullptr});
    
    EXPECT_TRUE(advancedCLI->getOptions().advanced);
}

TEST_F(CLIIntegrationTest, HelpSystem) {
    // Test help system
    advancedCLI->printHelp();
    advancedCLI->printAdvancedHelp();
    
    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(CLIIntegrationTest, ErrorHandling) {
    // Test error handling
    int argc = 4;
    char* argv[] = {(char*)"nlm_cli", (char*)"--advanced", (char*)"--invalid-command", (char*)nullptr};
    
    EXPECT_FALSE(advancedCLI->runAdvancedCLI(argc, argv));
}

} // namespace nlm
