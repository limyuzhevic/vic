// Build System Implementation
// Complete implementation of the enhanced build system for NLM

#include "BuildSystem.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {
namespace build {

struct CMakeBuildSystem::Impl {
    BuildOptions options;
    BuildStats stats;
    std::string buildDirectory;
    std::string installDirectory;
    std::string outputDirectory;
    std::unordered_map<std::string, std::string> cache;
    std::vector<BuildStep> buildSteps;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::mutex mutex;
    bool initialized = false;
    bool configured = false;
    bool isBuilding = false;
    std::chrono::system_clock::time_point lastBuildTime;
};

CMakeBuildSystem::CMakeBuildSystem() : pImpl(std::make_unique<Impl>()) {}

bool CMakeBuildSystem::configure(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->options = options;
    pImpl->buildDirectory = options.buildDirectory;
    pImpl->installDirectory = options.installDirectory;
    pImpl->outputDirectory = options.outputDirectory;
    
    // Create directories
    std::filesystem::create_directories(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->installDirectory);
    std::filesystem::create_directories(pImpl->outputDirectory);
    
    pImpl->configured = true;
    return true;
}

BuildOptions CMakeBuildSystem::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->options;
}

bool CMakeBuildSystem::build(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->configured) {
        return false;
    }
    
    pImpl->isBuilding = true;
    pImpl->stats.startTime = std::chrono::system_clock::now();
    pImpl->stats.stepsExecuted = 0;
    pImpl->stats.stepsFailed = 0;
    
    // Generate CMakeLists.txt
    if (!generateCMakeLists(pImpl->options)) {
        pImpl->isBuilding = false;
        return false;
    }
    
    // Configure CMake
    if (!configureCMake(pImpl->options)) {
        pImpl->isBuilding = false;
        return false;
    }
    
    // Build targets
    bool success = runCMakeBuild(targets);
    
    pImpl->isBuilding = false;
    pImpl->stats.endTime = std::chrono::system_clock::now();
    pImpl->lastBuildTime = pImpl->stats.endTime;
    
    return success;
}

bool CMakeBuildSystem::build(BuildTarget target) {
    return build(std::vector<BuildTarget>{target});
}

bool CMakeBuildSystem::clean() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove build directory
    std::filesystem::remove_all(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->buildDirectory);
    
    // Clear cache
    pImpl->cache.clear();
    
    // Reset statistics
    pImpl->stats = BuildStats();
    
    return true;
}

bool CMakeBuildSystem::rebuild() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Clean and rebuild
    if (!clean()) {
        return false;
    }
    
    // Build all targets
    return build(std::vector<BuildTarget>{BuildTarget::All});
}

bool CMakeBuildSystem::isBuilt(BuildTarget target) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if build directory exists and contains expected files
    std::string targetDir = pImpl->buildDirectory + "/" + buildTargetToString(target);
    
    if (!std::filesystem::exists(targetDir)) {
        return false;
    }
    
    // Check for expected build artifacts
    // This would depend on the specific target
    
    return true;
}

std::vector<BuildTarget> CMakeBuildSystem::getBuiltTargets() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<BuildTarget> builtTargets;
    
    for (const auto& target : std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    }) {
        if (isBuilt(target)) {
            builtTargets.push_back(target);
        }
    }
    
    return builtTargets;
}

std::vector<BuildTarget> CMakeBuildSystem::getAvailableTargets() const {
    return std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    };
}

BuildStats CMakeBuildSystem::getBuildStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string CMakeBuildSystem::getBuildLog() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Read build log file
    std::string logFile = pImpl->buildDirectory + "/build.log";
    
    if (!std::filesystem::exists(logFile)) {
        return "Build log not found";
    }
    
    std::ifstream file(logFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

bool CMakeBuildSystem::addDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->dependencies[target].push_back(dependency);
    return true;
}

bool CMakeBuildSystem::removeDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(target);
    if (it == pImpl->dependencies.end()) {
        return false;
    }
    
    auto depIt = std::find(it->second.begin(), it->second.end(), dependency);
    if (depIt == it->second.end()) {
        return false;
    }
    
    it->second.erase(depIt);
    return true;
}

std::vector<std::string> CMakeBuildSystem::getDependencies(BuildTarget target) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string targetStr = buildTargetToString(target);
    auto it = pImpl->dependencies.find(targetStr);
    
    if (it == pImpl->dependencies.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

bool CMakeBuildSystem::setupEnvironment() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if CMake is available
    if (!isCMakeAvailable()) {
        return false;
    }
    
    // Set environment variables
    setenv("CMAKE_BUILD_TYPE", buildConfigurationToString(pImpl->options.config).c_str(), 1);
    
    // Create build scripts if needed
    // This would be platform-specific
    
    return true;
}

bool CMakeBuildSystem::validateEnvironment() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check for required tools
    if (!isCMakeAvailable()) {
        return false;
    }
    
    // Check for required directories
    if (!std::filesystem::exists(pImpl->buildDirectory)) {
        return false;
    }
    
    // Check for required configuration
    if (pImpl->options.buildDirectory.empty()) {
        return false;
    }
    
    return true;
}

bool CMakeBuildSystem::generateCMakeLists(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Generate main CMakeLists.txt
    std::string cmakeContent = generateCMakeListsContent(options);
    
    std::string cmakeFile = pImpl->buildDirectory + "/CMakeLists.txt";
    
    std::ofstream file(cmakeFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << cmakeContent;
    file.close();
    
    // Generate component-specific CMakeLists.txt files
    // This would generate files for plugins, factories, tests, etc.
    
    return true;
}

bool CMakeBuildSystem::configureCMake(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Run CMake configuration
    std::string cmakeCommand = "cmake -S " + pImpl->buildDirectory + " -B " + pImpl->buildDirectory;
    
    // Add CMake options based on build configuration
    switch (options.config) {
        case BuildConfiguration::Debug:
            cmakeCommand += " -DCMAKE_BUILD_TYPE=Debug";
            break;
        case BuildConfiguration::Release:
            cmakeCommand += " -DCMAKE_BUILD_TYPE=Release";
            break;
        case BuildConfiguration::RelWithDebInfo:
            cmakeCommand += " -DCMAKE_BUILD_TYPE=RelWithDebInfo";
            break;
        case BuildConfiguration::MinSizeRel:
            cmakeCommand += " -DCMAKE_BUILD_TYPE=MinSizeRel";
            break;
    }
    
    // Add additional options
    for (const auto& flag : options.additionalFlags) {
        cmakeCommand += " " + flag;
    }
    
    // Execute CMake command
    int result = system(cmakeCommand.c_str());
    
    return result == 0;
}

bool CMakeBuildSystem::runCMakeBuild(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Convert targets to CMake target names
    std::vector<std::string> cmakeTargets;
    
    for (const auto& target : targets) {
        std::string cmakeTarget = convertTargetToCMake(target);
        if (!cmakeTarget.empty()) {
            cmakeTargets.push_back(cmakeTarget);
        }
    }
    
    // Build specific targets or all targets
    std::string cmakeCommand;
    
    if (cmakeTargets.empty()) {
        cmakeCommand = "cmake --build " + pImpl->buildDirectory;
    } else {
        cmakeCommand = "cmake --build " + pImpl->buildDirectory + " --target ";
        for (size_t i = 0; i < cmakeTargets.size(); ++i) {
            if (i > 0) cmakeCommand += " ";
            cmakeCommand += cmakeTargets[i];
        }
    }
    
    // Add build parallelism options
    if (pImpl->options.parallelJobs > 1) {
        cmakeCommand += " --parallel " + std::to_string(pImpl->options.parallelJobs);
    }
    
    // Execute build command
    int result = system(cmakeCommand.c_str());
    
    if (result == 0) {
        pImpl->stats.stepsExecuted++;
    } else {
        pImpl->stats.stepsFailed++;
        pImpl->stats.errors.push_back("Build failed with error code: " + std::to_string(result));
    }
    
    return result == 0;
}

std::string CMakeBuildSystem::generateCMakeListsContent(const BuildOptions& options) {
    std::stringstream cmake;
    
    cmake << "cmake_minimum_required(VERSION 3.16)\n";
    cmake << "project(NLM VERSION 0.1.0 LANGUAGES CXX)\n\n";
    
    // Set C++ standard
    cmake << "set(CMAKE_CXX_STANDARD 20)\n";
    cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
    cmake << "set(CMAKE_CXX_EXTENSIONS OFF)\n\n";
    
    // Build type
    cmake << "if(NOT CMAKE_BUILD_TYPE)\n";
    cmake << "    set(CMAKE_BUILD_TYPE Release)\n";
    cmake << "endif()\n\n";
    
    // Enable warnings
    cmake << "if(MSVC)\n";
    cmake << "    add_compile_options(/W4)\n";
    cmake << "else()\n";
    cmake << "    add_compile_options(-Wall -Wextra -Wpedantic)\n";
    cmake << "endif()\n\n";
    
    // Include directories
    cmake << "include_directories(\n";
    for (const auto& include : options.includePaths) {
        cmake << "    " << include << "\n";
    }
    cmake << ")\n\n";
    
    // Library directories
    cmake << "link_directories(\n";
    for (const auto& library : options.libraryPaths) {
        cmake << "    " << library << "\n";
    }
    cmake << ")\n\n";
    
    // Add definitions
    cmake << "add_definitions(\n";
    for (const auto& def : options.definitions) {
        cmake << "    -D" << def.first << "=\"" << def.second << "\"\n";
    }
    cmake << ")\n\n";
    
    // Add subdirectories for components
    cmake << "add_subdirectory(src)\n\n";
    
    // Add examples if enabled
    if (options.testOptions.buildExamples) {
        cmake << "add_subdirectory(examples)\n\n";
    }
    
    // Add tests if enabled
    if (options.testOptions.buildUnitTests) {
        cmake << "enable_testing()\n";
        cmake << "add_subdirectory(tests)\n\n";
    }
    
    // Installation
    cmake << "install(DIRECTORY include/ DESTINATION include)\n";
    cmake << "install(TARGETS nlm DESTINATION lib)\n";
    
    return cmake.str();
}

std::string CMakeBuildSystem::convertTargetToCMake(BuildTarget target) {
    switch (target) {
        case BuildTarget::All:
            return "all";
        case BuildTarget::Core:
            return "nlm_core";
        case BuildTarget::Plugins:
            return "nlm_plugin";
        case BuildTarget::Factories:
            return "nlm_factory";
        case BuildTarget::NeuronModels:
            return "neuron_models";
        case BuildTarget::PlasticityRules:
            return "plasticity_rules";
        case BuildTarget::LearningAlgorithms:
            return "learning_algorithms";
        case BuildTarget::Examples:
            return "examples";
        case BuildTarget::Tests:
            return "test_all";
        default:
            return "";
    }
}

// MakefileBuildSystem implementation
MakefileBuildSystem::MakefileBuildSystem() : pImpl(std::make_unique<Impl>()) {}

bool MakefileBuildSystem::configure(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->options = options;
    pImpl->buildDirectory = options.buildDirectory;
    pImpl->installDirectory = options.installDirectory;
    pImpl->outputDirectory = options.outputDirectory;
    
    // Create directories
    std::filesystem::create_directories(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->installDirectory);
    std::filesystem::create_directories(pImpl->outputDirectory);
    
    pImpl->configured = true;
    return true;
}

BuildOptions MakefileBuildSystem::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->options;
}

bool MakefileBuildSystem::build(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->configured) {
        return false;
    }
    
    pImpl->isBuilding = true;
    pImpl->stats.startTime = std::chrono::system_clock::now();
    pImpl->stats.stepsExecuted = 0;
    pImpl->stats.stepsFailed = 0;
    
    // Generate Makefiles
    if (!generateMakefiles(pImpl->options)) {
        pImpl->isBuilding = false;
        return false;
    }
    
    // Execute build steps
    bool success = executeBuildSteps(targets);
    
    pImpl->isBuilding = false;
    pImpl->stats.endTime = std::chrono::system_clock::now();
    pImpl->lastBuildTime = pImpl->stats.endTime;
    
    return success;
}

bool MakefileBuildSystem::build(BuildTarget target) {
    return build(std::vector<BuildTarget>{target});
}

bool MakefileBuildSystem::clean() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove build directory
    std::filesystem::remove_all(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->buildDirectory);
    
    // Clear cache
    pImpl->cache.clear();
    
    // Reset statistics
    pImpl->stats = BuildStats();
    
    return true;
}

bool MakefileBuildSystem::rebuild() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Clean and rebuild
    if (!clean()) {
        return false;
    }
    
    // Build all targets
    return build(std::vector<BuildTarget>{BuildTarget::All});
}

bool MakefileBuildSystem::isBuilt(BuildTarget target) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if build directory exists and contains expected files
    std::string targetDir = pImpl->buildDirectory + "/" + buildTargetToString(target);
    
    if (!std::filesystem::exists(targetDir)) {
        return false;
    }
    
    // Check for expected build artifacts
    // This would depend on the specific target
    
    return true;
}

std::vector<BuildTarget> MakefileBuildSystem::getBuiltTargets() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<BuildTarget> builtTargets;
    
    for (const auto& target : std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    }) {
        if (isBuilt(target)) {
            builtTargets.push_back(target);
        }
    }
    
    return builtTargets;
}

std::vector<BuildTarget> MakefileBuildSystem::getAvailableTargets() const {
    return std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    };
}

BuildStats MakefileBuildSystem::getBuildStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string MakefileBuildSystem::getBuildLog() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Read build log file
    std::string logFile = pImpl->buildDirectory + "/build.log";
    
    if (!std::filesystem::exists(logFile)) {
        return "Build log not found";
    }
    
    std::ifstream file(logFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

bool MakefileBuildSystem::addDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->dependencies[target].push_back(dependency);
    return true;
}

bool MakefileBuildSystem::removeDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(target);
    if (it == pImpl->dependencies.end()) {
        return false;
    }
    
    auto depIt = std::find(it->second.begin(), it->second.end(), dependency);
    if (depIt == it->second.end()) {
        return false;
    }
    
    it->second.erase(depIt);
    return true;
}

std::vector<std::string> MakefileBuildSystem::getDependencies(BuildTarget target) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string targetStr = buildTargetToString(target);
    auto it = pImpl->dependencies.find(targetStr);
    
    if (it == pImpl->dependencies.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

bool MakefileBuildSystem::setupEnvironment() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if make is available
    if (!isMakeAvailable()) {
        return false;
    }
    
    // Set environment variables
    setenv("MAKEFLAGS", ("-j" + std::to_string(pImpl->options.parallelJobs)).c_str(), 1);
    
    return true;
}

bool MakefileBuildSystem::validateEnvironment() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check for required tools
    if (!isMakeAvailable()) {
        return false;
    }
    
    // Check for required directories
    if (!std::filesystem::exists(pImpl->buildDirectory)) {
        return false;
    }
    
    return true;
}

bool MakefileBuildSystem::generateMakefiles(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Generate main Makefile
    std::string makefileContent = generateMakefileContent(options);
    
    std::string makefileFile = pImpl->buildDirectory + "/Makefile";
    
    std::ofstream file(makefileFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << makefileContent;
    file.close();
    
    // Generate component-specific Makefiles
    // This would generate files for plugins, factories, tests, etc.
    
    return true;
}

std::string MakefileBuildSystem::generateMakefileContent(const BuildOptions& options) {
    std::stringstream makefile;
    
    makefile << "# Generated Makefile for NLM Build System\n";
    makefile << "# Configuration: " << buildConfigurationToString(options.config) << "\n\n";
    
    makefile << "CC = gcc\n";
    makefile << "CXX = g++\n";
    makefile << "CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic\n";
    
    if (options.config == BuildConfiguration::Debug) {
        makefile << "CXXFLAGS += -g -O0\n";
    } else {
        makefile << "CXXFLAGS += -O2\n";
    }
    
    makefile << "LDFLAGS =\n";
    
    // Add include directories
    makefile << "INCLUDES = ";
    for (size_t i = 0; i < options.includePaths.size(); ++i) {
        if (i > 0) makefile << " ";
        makefile << "-I" << options.includePaths[i];
    }
    makefile << "\n\n";
    
    // Add definitions
    makefile << "DEFINES = ";
    for (size_t i = 0; i < options.definitions.size(); ++i) {
        if (i > 0) makefile << " ";
        makefile << "-D" << options.definitions.begin()->first << "=\"" << options.definitions.begin()->second << "\"";
    }
    makefile << "\n\n";
    
    // Targets
    makefile << "all: build-all\n\n";
    
    makefile << "build-all: ";
    for (size_t i = 0; i < std::vector<BuildTarget>{BuildTarget::Core, BuildTarget::Plugins, BuildTarget::Factories}.size(); ++i) {
        if (i > 0) makefile << " ";
        makefile << convertTargetToMakefile(BuildTarget::Core);
    }
    makefile << "\n\n";
    
    // Build core
    makefile << convertTargetToMakefile(BuildTarget::Core) << ":\n";
    makefile << "\t$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c src/main.cpp -o $@\n\n";
    
    // Build plugins
    makefile << convertTargetToMakefile(BuildTarget::Plugins) << ":\n";
    makefile << "\t$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c src/plugin/PluginInterface.cpp -o $@\n\n";
    
    // Build factories
    makefile << convertTargetToMakefile(BuildTarget::Factories) << ":\n";
    makefile << "\t$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c src/factory/FactorySystem.cpp -o $@\n\n";
    
    // Clean target
    makefile << "clean:\n";
    makefile << "\trm -f *.o *.so *.a\n";
    makefile << "\trm -rf $(OUTPUT_DIR)\n\n";
    
    // Install target
    makefile << "install:\n";
    makefile << "\tcp -r src/include/nlm $(INSTALL_DIR)/include/\n";
    makefile << "\tcp -r $(BUILD_DIR)/lib/*.so $(INSTALL_DIR)/lib/\n";
    
    return makefile.str();
}

std::string MakefileBuildSystem::convertTargetToMakefile(BuildTarget target) {
    switch (target) {
        case BuildTarget::All:
            return "all-target";
        case BuildTarget::Core:
            return "build/core";
        case BuildTarget::Plugins:
            return "build/plugins";
        case BuildTarget::Factories:
            return "build/factories";
        case BuildTarget::NeuronModels:
            return "neuron_models";
        case BuildTarget::PlasticityRules:
            return "plasticity_rules";
        case BuildTarget::LearningAlgorithms:
            return "learning_algorithms";
        case BuildTarget::Examples:
            return "examples";
        case BuildTarget::Tests:
            return "tests";
        default:
            return "";
    }
}

bool MakefileBuildSystem::executeBuildSteps(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Resolve build steps
    std::vector<BuildStep> buildSteps = resolveBuildSteps(targets);
    
    // Execute build steps
    return executeParallelBuildSteps(buildSteps, pImpl->options);
}

std::vector<BuildStep> MakefileBuildSystem::resolveBuildSteps(BuildTarget target) {
    std::vector<BuildStep> steps;
    
    // Add dependencies
    std::vector<std::string> dependencies = getDependencies(target);
    
    // Create build steps for each dependency
    for (const auto& dep : dependencies) {
        BuildStep step;
        step.name = "Dependency: " + dep;
        step.command = "make " + dep;
        step.isCritical = true;
        steps.push_back(step);
    }
    
    // Add build step for target
    BuildStep targetStep;
    targetStep.name = "Build target: " + buildTargetToString(target);
    targetStep.command = "make " + convertTargetToMakefile(target);
    targetStep.isCritical = true;
    steps.push_back(targetStep);
    
    return steps;
}

bool MakefileBuildSystem::executeBuildSteps(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Resolve all build steps
    std::vector<BuildStep> allSteps;
    for (const auto& target : targets) {
        std::vector<BuildStep> targetSteps = resolveBuildSteps(target);
        allSteps.insert(allSteps.end(), targetSteps.begin(), targetSteps.end());
    }
    
    return executeParallelBuildSteps(allSteps, pImpl->options);
}

bool MakefileBuildSystem::executeParallelBuildSteps(const std::vector<BuildStep>& steps,
                                                   const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Execute steps in parallel
    std::vector<std::future<bool>> futures;
    
    for (const auto& step : steps) {
        futures.push_back(std::async(std::launch::async, [step, options]() {
            return executeBuildStep(step, options);
        }));
    }
    
    // Wait for all steps to complete
    bool success = true;
    for (auto& future : futures) {
        if (!future.get()) {
            success = false;
        }
    }
    
    if (success) {
        pImpl->stats.stepsExecuted = steps.size();
    } else {
        pImpl->stats.stepsFailed = steps.size();
    }
    
    return success;
}

bool MakefileBuildSystem::executeBuildStep(const BuildStep& step,
                                          const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if step should be skipped
    if (step.skipCondition && step.skipCondition(options)) {
        pImpl->stats.stepsExecuted++;
        return true;
    }
    
    // Execute build command
    int result = system(step.command.c_str());
    
    if (result == 0) {
        pImpl->stats.stepsExecuted++;
        pImpl->stats.logFileSteps.push_back(step.name);
        return true;
    } else {
        pImpl->stats.stepsFailed++;
        pImpl->stats.errors.push_back("Build step failed: " + step.name + ", error code: " + std::to_string(result));
        return false;
    }
}

// NinjaBuildSystem implementation
NinjaBuildSystem::NinjaBuildSystem() : pImpl(std::make_unique<Impl>()) {}

bool NinjaBuildSystem::configure(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->options = options;
    pImpl->buildDirectory = options.buildDirectory;
    pImpl->installDirectory = options.installDirectory;
    pImpl->outputDirectory = options.outputDirectory;
    
    // Create directories
    std::filesystem::create_directories(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->installDirectory);
    std::filesystem::create_directories(pImpl->outputDirectory);
    
    pImpl->configured = true;
    return true;
}

BuildOptions NinjaBuildSystem::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->options;
}

bool NinjaBuildSystem::build(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->configured) {
        return false;
    }
    
    pImpl->isBuilding = true;
    pImpl->stats.startTime = std::chrono::system_clock::now();
    pImpl->stats.stepsExecuted = 0;
    pImpl->stats.stepsFailed = 0;
    
    // Generate Ninja build files
    if (!generateNinjaBuild(pImpl->options)) {
        pImpl->isBuilding = false;
        return false;
    }
    
    // Run build
    bool success = runNinjaBuild(targets);
    
    pImpl->isBuilding = false;
    pImpl->stats.endTime = std::chrono::system_clock::now();
    pImpl->lastBuildTime = pImpl->stats.endTime;
    
    return success;
}

bool NinjaBuildSystem::build(BuildTarget target) {
    return build(std::vector<BuildTarget>{target});
}

bool NinjaBuildSystem::clean() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove build directory
    std::filesystem::remove_all(pImpl->buildDirectory);
    std::filesystem::create_directories(pImpl->buildDirectory);
    
    // Clear cache
    pImpl->cache.clear();
    
    // Reset statistics
    pImpl->stats = BuildStats();
    
    return true;
}

bool NinjaBuildSystem::rebuild() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Clean and rebuild
    if (!clean()) {
        return false;
    }
    
    // Build all targets
    return build(std::vector<BuildTarget>{BuildTarget::All});
}

bool NinjaBuildSystem::isBuilt(BuildTarget target) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if build directory exists and contains expected files
    std::string targetDir = pImpl->buildDirectory + "/" + buildTargetToString(target);
    
    if (!std::filesystem::exists(targetDir)) {
        return false;
    }
    
    // Check for expected build artifacts
    // This would depend on the specific target
    
    return true;
}

std::vector<BuildTarget> NinjaBuildSystem::getBuiltTargets() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<BuildTarget> builtTargets;
    
    for (const auto& target : std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    }) {
        if (isBuilt(target)) {
            builtTargets.push_back(target);
        }
    }
    
    return builtTargets;
}

std::vector<BuildTarget> NinjaBuildSystem::getAvailableTargets() const {
    return std::vector<BuildTarget>{
        BuildTarget::All, BuildTarget::Core, BuildTarget::Plugins,
        BuildTarget::Factories, BuildTarget::NeuronModels,
        BuildTarget::PlasticityRules, BuildTarget::LearningAlgorithms,
        BuildTarget::Examples, BuildTarget::Tests
    };
}

BuildStats NinjaBuildSystem::getBuildStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string NinjaBuildSystem::getBuildLog() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Read build log file
    std::string logFile = pImpl->buildDirectory + "/build.log";
    
    if (!std::filesystem::exists(logFile)) {
        return "Build log not found";
    }
    
    std::ifstream file(logFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

bool NinjaBuildSystem::addDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->dependencies[target].push_back(dependency);
    return true;
}

bool NinjaBuildSystem::removeDependency(const std::string& target, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencies.find(target);
    if (it == pImpl->dependencies.end()) {
        return false;
    }
    
    auto depIt = std::find(it->second.begin(), it->second.end(), dependency);
    if (depIt == it->second.end()) {
        return false;
    }
    
    it->second.erase(depIt);
    return true;
}

std::vector<std::string> NinjaBuildSystem::getDependencies(BuildTarget target) const {
    std inválido::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string targetStr = buildTargetToString(target);
    auto it = pImpl->dependencies.find(targetStr);
    
    if (it == pImpl->dependencies.end()) {
        return std::vector<std::string>();
    }
    
    return it->second;
}

bool NinjaBuildSystem::setupEnvironment() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if ninja is available
    if (!isNinjaAvailable()) {
        return false;
    }
    
    // Set environment variables
    setenv("NINJA_VERBOSITY", "1", 1);
    
    return true;
}

bool NinjaBuildSystem::validateEnvironment() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check for required tools
    if (!isNinjaAvailable()) {
        return false;
    }
    
    // Check for required directories
    if (!std::filesystem::exists(pImpl->buildDirectory)) {
        return false;
    }
    
    return true;
}

bool NinjaBuildSystem::generateNinjaBuild(const BuildOptions& options) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Generate build.ninja file
    std::string ninjaContent = generateNinjaContent(options);
    
    std::string ninjaFile = pImpl->buildDirectory + "/build.ninja";
    
    std::ofstream file(ninjaFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << ninjaContent;
    file.close();
    
    // Generate build rules
    // This would generate rules for plugins, factories, tests, etc.
    
    return true;
}

std::string NinjaBuildSystem::generateNinjaContent(const BuildOptions& options) {
    std::stringstream ninja;
    
    ninja << "# Generated Ninja build file for NLM Build System\n";
    ninja << "# Configuration: " << buildConfigurationToString(options.config) << "\n\n";
    
    ninja << "builddir: build\n";
    ninja << "ninja_version = 1.10\n\n";
    
    // Default build rule
    ninja << "default all\n\n";
    
    // All target
    ninja << "build all: phony build-all\n\n";
    
    ninja << "build build-all: phony | ";
    for (size_t i = 0; i < std::vector<BuildTarget>{BuildTarget::Core, BuildTarget::Plugins, BuildTarget::Factories}.size(); ++i) {
        if (i > 0) ninja << " ";
        ninja << convertTargetToNinja(BuildTarget::Core);
    }
    ninja << "\n\n";
    
    // Core target
    ninja << "build " << convertTargetToNinja(BuildTarget::Core) << ": phony\n";
    ninja << "\t@echo Building NLM Core\n\n";
    
    // Plugins target
    ninja << "build " << convertTargetToNinja(BuildTarget::Plugins) << ": phony\n";
    ninja << "\t@echo Building NLM Plugins\n\n";
    
    // Factories target
    ninja << "build " << convertTargetToNinja(BuildTarget::Factories) << ": phony\n";
    ninja << "\t@echo Building NLM Factories\n\n";
    
    return ninja.str();
}

std::string NinjaBuildSystem::convertTargetToNinja(BuildTarget target) {
    switch (target) {
        case BuildTarget::All:
            return "phony_all";
        case BuildTarget::Core:
            return "phony_core";
        case BuildTarget::Plugins:
            return "phony_plugins";
        case BuildTarget::Factories:
            return "phony_factories";
        case BuildTarget::NeuronModels:
            return "phony_neuron_models";
        case BuildTarget::PlasticityRules:
            return "phony_plasticity_rules";
        case BuildTarget::LearningAlgorithms:
            return "phony_learning_algorithms";
        case BuildTarget::Examples:
            return "phony_examples";
        case BuildTarget::Tests:
            return "phony_tests";
        default:
            return "";
    }
}

bool NinjaBuildSystem::runNinjaBuild(const std::vector<BuildTarget>& targets) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Convert targets to Ninja target names
    std::vector<std::string> ninjaTargets;
    
    for (const auto& target : targets) {
        std::string ninjaTarget = convertTargetToNinja(target);
        if (!ninjaTarget.empty()) {
            ninjaTargets.push_back(ninjaTarget);
        }
    }
    
    // Build specific targets or all targets
    std::string ninjaCommand;
    
    if (ninjaTargets.empty()) {
        ninjaCommand = "ninja -C " + pImpl->buildDirectory + " build";
    } else {
        ninjaCommand = "ninja -C " + pImpl->buildDirectory + " build ";
        for (size_t i = 0; i < ninjaTargets.size(); ++i) {
            if (i > 0) ninjaCommand += " ";
            ninjaCommand += ninjaTargets[i];
        }
    }
    
    // Add parallel build options
    if (pImpl->options.parallelJobs > 1) {
        ninjaCommand += " -j " + std::to_string(pImpl->options.parallelJobs);
    }
    
    // Execute build command
    int result = system(ninjaCommand.c_str());
    
    if (result == 0) {
        pImpl->stats.stepsExecuted = targets.size();
    } else {
        pImpl->stats.stepsFailed = targets.size();
        pImpl->stats.errors.push_back("Ninja build failed with error code: " + std::to_string(result));
    }
    
    return result == 0;
}

// Build utilities implementation
namespace utils {
    std::string buildConfigurationToString(BuildConfiguration config) {
        switch (config) {
            case BuildConfiguration::Debug:
                return "Debug";
            case BuildConfiguration::Release:
                return "Release";
            case BuildConfiguration::RelWithDebInfo:
                return "RelWithDebInfo";
            case BuildConfiguration::MinSizeRel:
                return "MinSizeRel";
            default:
                return "Unknown";
        }
    }
    
    BuildConfiguration stringToBuildConfiguration(const std::string& str) {
        if (str == "Debug") return BuildConfiguration::Debug;
        if (str == "Release") return BuildConfiguration::Release;
        if (str == "RelWithDebInfo") return BuildConfiguration::RelWithDebInfo;
        if (str == "MinSizeRel") return BuildConfiguration::MinSizeRel;
        return BuildConfiguration::Debug;
    }
    
    std::string buildTargetToString(BuildTarget target) {
        switch (target) {
            case BuildTarget::All:
                return "all";
            case BuildTarget::Core:
                return "core";
            case BuildTarget::Plugins:
                return "plugins";
            case BuildTarget::Factories:
                return "factories";
            case BuildTarget::NeuronModels:
                return "neuron_models";
            case BuildTarget::PlasticityRules:
                return "plasticity_rules";
            case BuildTarget::LearningAlgorithms:
                return "learning_algorithms";
            case BuildTarget::Examples:
                return "examples";
            case BuildTarget::Tests:
                return "tests";
            default:
                return "unknown";
        }
    }
    
    bool isCMakeAvailable() {
        return std::system("which cmake > /dev/null 2>&1") == 0;
    }
    
    bool isNinjaAvailable() {
        return std::system("which ninja > /dev/null 2>&1") == 0;
    }
    
    bool isMakeAvailable() {
        return std::system("which make > /dev/null 2>&1") == 0;
    }
    
    std::string detectBuildSystem(const std::string& directory) {
        // Check for build system files
        if (std::filesystem::exists(directory + "/CMakeLists.txt")) {
            return "cmake";
        }
        if (std::filesystem::exists(directory + "/build.ninja")) {
            return "ninja";
        }
        if (std::filesystem::exists(directory + "/Makefile")) {
            return "make";
        }
        return "unknown";
    }
    
    std::string detectPlatform() {
        // Detect platform from system
        if (std::system("uname -s | grep -i linux > /dev/null 2>&1") == 0) {
            return "linux";
        } else if (std::system("uname -s | grep -i darwin > /dev/null 2>&1") == 0) {
            return "macos";
        } else if (std::system("uname -s | grep -i windows > /dev/null 2>&1") == 0) {
            return "windows";
        }
        return "unknown";
    }
    
    std::string detectArchitecture() {
        // Detect architecture
        if (std::system("uname -m | grep -i x86_64 > /dev/null 2>&1") == 0) {
            return "x86_64";
        } else if (std::system("uname -m | grep -i arm > /dev/null 2>&1") == 0) {
            return "arm";
        }
        return "unknown";
    }
}

} // namespace build
} // namespace nlm