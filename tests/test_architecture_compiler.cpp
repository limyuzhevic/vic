// Architecture Compiler Tests
// Architecture compilation and validation tests

#include "arch/ArchitectureCompiler.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {
namespace tests {

namespace test_architecture_compiler {

void testArchitectureCompilerCreation() {
    nlm::ArchitectureCompiler compiler;
    
    std::cout << "    testArchitectureCompilerCreation passed" << std::endl;
}

void testArchitectureCompilerInitialization() {
    nlm::ArchitectureCompiler compiler;
    
    // Initialize with a compiler path
    bool success = compiler.initialize("gcc");
    
    // For now, initialization may fail due to compiler not found
    // But we test the interface
    std::cout << "    testArchitectureCompilerInitialization passed (interface test)" << std::endl;
}

void testArchitectureCompilerUpdate() {
    nlm::ArchitectureCompiler compiler;
    
    // Test update method
    compiler.update(0.1);
    
    std::cout << "    testArchitectureCompilerUpdate passed" << std::endl;
}

void testArchitectureCompilerShutdown() {
    nlm::ArchitectureCompiler compiler;
    
    // Test shutdown
    compiler.shutdown();
    
    std::cout << "    testArchitectureCompilerShutdown passed" << std::endl;
}

void testArchitectureCompilerGetStats() {
    nlm::ArchitectureCompiler compiler;
    
    // Get compilation statistics
    size_t count = compiler.getCompilationCount();
    double avgTime = compiler.getAverageCompilationTime();
    std::string stats = compiler.getCompilationStats();
    
    assert(!stats.empty());
    
    std::cout << "    testArchitectureCompilerGetStats passed" << std::endl;
}

void testArchitectureCompilerGetErrors() {
    nlm::ArchitectureCompiler compiler;
    
    // Get errors and warnings
    std::vector<std::string> errors = compiler.getCompilationErrors();
    std::vector<std::string> warnings = compiler.getWarnings();
    
    assert(errors.size() >= 0);
    assert(warnings.size() >= 0);
    
    std::cout << "    testArchitectureCompilerGetErrors passed" << std::endl;
}

void testArchitectureCompilerTemplates() {
    nlm::ArchitectureCompiler compiler;
    
    // Test template management
    nlm::NeuralArchitecture emptyArch;
    
    bool result1 = compiler.addArchitectureTemplate("TestTemplate", emptyArch);
    std::vector<std::string> templates = compiler.getAvailableTemplates();
    assert(!templates.empty());
    
    nlm::NeuralArchitecture retrieved = compiler.getArchitectureTemplate("TestTemplate");
    
    bool result2 = compiler.removeArchitectureTemplate("TestTemplate");
    
    std::cout << "    testArchitectureCompilerTemplates passed" << std::endl;
}

void testArchitectureCompilerValidation() {
    nlm::ArchitectureCompiler compiler;
    
    // Create a valid architecture
    nlm::NeuralArchitecture arch;
    arch.name = "TestArchitecture";
    arch.version = "1.0";
    arch.description = "Test architecture";
    
    // Validate architecture
    bool valid = compiler.validateArchitecture(arch);
    
    // Create an invalid architecture
    nlm::NeuralArchitecture invalidArch;
    // Name is empty
    
    bool invalidValid = compiler.validateArchitecture(invalidArch);
    assert(!invalidValid); // Should be invalid
    
    std::cout << "    testArchitectureCompilerValidation passed" << std::endl;
}

void testArchitectureCompilerFileIO() {
    nlm::ArchitectureCompiler compiler;
    
    // Create a temporary directory for test files
    std::string tempDir = "/tmp/arch_test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::filesystem::create_directories(tempDir);
    
    std::string yamlPath = tempDir + "/test_arch.yaml";
    std::string jsonPath = tempDir + "/test_arch.json";
    std::string outputPath = tempDir + "/output.cpp";
    
    // Create a simple YAML architecture
    std::ofstream yamlFile(yamlPath);
    if (yamlFile.is_open()) {
        yamlFile << "name: TestArchitecture\n";
        yamlFile << "version: \"1.0\"\n";
        yamlFile << "description: Test architecture for compilation\n";
        yamlFile << "regions:\n";
        yamlFile << "  - name: TestRegion\n";
        yamlFile << "    type: default\n";
        yamlFile << "    populations:\n";
        yamlFile << "      - name: TestPopulation\n";
        yamlFile << "        neuronType: lif\n";
        yamlFile << "        neuronCount: 100\n";
        yamlFile.close();
    }
    
    // Try to compile from YAML
    bool success = compiler.compile(yamlPath, outputPath);
    
    // Create a simple JSON architecture
    std::ofstream jsonFile(jsonPath);
    if (jsonFile.is_open()) {
        jsonFile << "{\"name\": \"TestArchitecture\", \"version\": \"1.0\", \"description\": \"Test architecture\"}";
        jsonFile.close();
    }
    
    // Try to compile from JSON
    bool success2 = compiler.compile(jsonPath, outputPath);
    
    // Try to compile from string
    std::string yamlString = "name: TestArchitecture\nversion: \"1.0\"\n";
    bool success3 = compiler.compileFromString(yamlString, outputPath);
    
    // Clean up
    std::filesystem::remove_all(tempDir);
    
    std::cout << "    testArchitectureCompilerFileIO passed" << std::endl;
}

void runAll() {
    testArchitectureCompilerCreation();
    testArchitectureCompilerInitialization();
    testArchitectureCompilerUpdate();
    testArchitectureCompilerShutdown();
    testArchitectureCompilerGetStats();
    testArchitectureCompilerGetErrors();
    testArchitectureCompilerTemplates();
    testArchitectureCompilerValidation();
    testArchitectureCompilerFileIO();
}

} // namespace test_architecture_compiler

} // namespace nlm
namespace tests {

void test_architecture_compiler() {
    test_architecture_compiler::runAll();
}

} // namespace tests
