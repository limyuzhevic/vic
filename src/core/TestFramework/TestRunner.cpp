#include "TestRunner.hpp"
#include "TestRegistry.hpp"
#include "ProgressTracker.hpp"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

namespace nlm {

std::vector<TestRunner::TestResult> TestRunner::runTests(
    const std::vector<std::unique_ptr<TestBase>>& tests,
    const TestConfig& config,
    std::shared_ptr<Brain> brain) {
    
    std::vector<TestResult> results;
    
    if (tests.empty()) {
        return results;
    }
    
    // Initialize progress tracking if enabled
    ProgressTracker progressTracker;
    if (config.getParameters().enableProgress && !config.getParameters().quietProgress) {
        progressTracker.setUpdateInterval(config.getParameters().progressInterval);
        progressTracker.setShowDetails(true);
    }
    
    // Setup test environment
    bool environmentSetup = false;
    if (!brain) {
        environmentSetup = setupTestEnvironment(config, brain);
    }
    
    if (!environmentSetup) {
        std::cerr << "Failed to setup test environment" << std::endl;
        for (const auto& test : tests) {
            results.emplace_back(test->getName(), test->getMetadata().category, test->getMetadata().phase, "Environment setup failed");
        }
        return results;
    }
    
    try {
        // Execute tests sequentially
        for (auto& test : tests) {
            std::string testName = test->getName();
            
            // Start progress tracking
            progressTracker.startTest(testName, test->getMetadata().category);
            
            // Run the test
            TestResult result = runTest(test, config, brain);
            
            // End progress tracking
            progressTracker.endTest(testName, result.passed, result.errorMessage);
            
            results.push_back(std::move(result));
            
            // Save brain state if requested
            if (config.getParameters().saveBrainState && !testName.empty()) {
                std::string savePath = config.getParameters().brainSavePath;
                if (savePath.empty()) {
                    savePath = config.getParameters().outputDirectory + "/brain_state.checkpoint";
                }
                saveBrainState(brain, savePath);
            }
            
            // Reset brain if requested
            if (config.getParameters().resetBrainBetweenTests) {
                resetBrain(brain, config);
            }
            
            // Stop if test failed and we're in fail-fast mode
            // (This could be made configurable)
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during test execution: " << e.what() << std::endl;
        
        // Create failed results for any remaining tests
        for (size_t i = results.size(); i < tests.size(); ++i) {
            results.emplace_back(tests[i]->getName(), 
                                tests[i]->getMetadata().category, 
                                tests[i]->getMetadata().phase, 
                                std::string("Exception: ") + e.what());
        }
    }
    
    // Teardown test environment
    teardownTestEnvironment(config, brain);
    
    // Display final progress summary
    if (config.getParameters().enableProgress && !config.getParameters().quietProgress) {
        progressTracker.displayFinalSummary();
    }
    
    return results;
}

std::vector<TestRunner::TestResult> TestRunner::runTestsWithProgress(
    const std::vector<std::unique_ptr<TestBase>>& tests,
    const TestConfig& config,
    std::shared_ptr<Brain> brain) {
    
    std::vector<TestResult> results;
    
    if (tests.empty()) {
        return results;
    }
    
    // Create progress tracker with real-time updates
    ProgressTracker progressTracker;
    progressTracker.setUpdateInterval(config.getParameters().progressInterval);
    progressTracker.setShowDetails(true);
    progressTracker.setColorOutput(!config.getParameters().quietProgress);
    
    // Setup test environment
    bool environmentSetup = false;
    if (!brain) {
        environmentSetup = setupTestEnvironment(config, brain);
    }
    
    if (!environmentSetup) {
        std::cerr << "Failed to setup test environment" << std::endl;
        for (const auto& test : tests) {
            results.emplace_back(test->getName(), test->getMetadata().category, test->getMetadata().phase, "Environment setup failed");
        }
        return results;
    }
    
    // Execute tests with progress updates
    for (auto& test : tests) {
        std::string testName = test->getName();
        
        // Start progress tracking
        progressTracker.startTest(testName, test->getMetadata().category);
        
        // Simulate progress updates during execution
        // In a real implementation, this would be called from within the test
        auto updateProgress = [&progressTracker, testName](float progress) {
            progressTracker.updateProgress(testName, progress, "Running test...");
            }
        
        // Run the test with progress updates
        TestResult result = runTest(test, config, brain, updateProgress);
        
        // End progress tracking
        progressTracker.endTest(testName, result.passed, result.errorMessage);
        
        results.push_back(std::move(result));
        
        // Display current progress
        progressTracker.displayProgress();
        
        // Stop if test failed and we're in fail-fast mode
        if (!config.getParameters().enableProgress && !result.passed) {
            break;
        }
    }
    
    // Teardown test environment
    teardownTestEnvironment(config, brain);
    
    // Display final summary
    progressTracker.displayFinalSummary();
    
    return results;
}

TestRunner::TestResult TestRunner::runTest(
    std::unique_ptr<TestBase>& test,
    const TestConfig& config,
    std::shared_ptr<Brain> brain,
    std::function<void(float)> progressCallback) {
    
    std::string testName = test->getName();
    std::string category = test->getMetadata().category;
    std::string phase = test->getMetadata().phase;
    
    TestResult result(testName, category, phase);
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Setup test
        if (!test->setup(brain, config.getConfig())) {
            result.passed = false;
            result.errorMessage = "Test setup failed";
            return result;
        }
        
        // Execute test with progress updates
        float progress = 0.0f;
        float progressStep = 1.0f / 3.0f; // Three phases: setup, execution, teardown
        
        // Progress during execution
        if (progressCallback) {
            progressCallback(progress);
            progress += progressStep;
        }
        
        // Execute the actual test
        result.passed = test->execute(brain, config.getConfig());
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        
        if (!result.passed) {
            result.errorMessage = "Test execution failed";
        }
        
        // Teardown test
        if (progressCallback) {
            progressCallback(progress);
            progress += progressStep;
        }
        
        if (!test->teardown(brain, config.getConfig())) {
            result.passed = false;
            if (!result.errorMessage.empty()) {
                result.errorMessage += "; ";
            }
            result.errorMessage += "Test teardown failed";
        }
        
        // Final progress
        if (progressCallback) {
            progressCallback(1.0f);
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
    }
    
    // Collect metrics
    result.metrics = test->getMetrics();
    
    // Add execution time to metrics
    result.metrics["execution_time_ms"] = std::to_string(result.duration.count());
    
    return result;
}

std::vector<std::unique_ptr<TestBase>> TestRunner::selectTests(
    const TestRegistry& registry,
    const TestConfig& config) {
    
    std::vector<std::unique_ptr<TestBase>> tests;
    
    // Get all tests from registry
    auto allTests = registry.getAllTests();
    
    // Apply filters
    std::vector<std::string> categories = config.getStringList("test_categories");
    std::vector<std::string> tags = config.getStringList("test_tags");
    std::vector<std::string> phases = config.getStringList("test_phases");
    std::vector<std::string> exclude = config.getStringList("exclude_tests");
    
    tests = registry.filterTests(categories, tags, phases, exclude);
    
    // Apply test name filter
    std::vector<std::string> testNames = config.getStringList("test_names");
    if (!testNames.empty()) {
        std::vector<std::unique_ptr<TestBase>> filteredTests;
        for (const auto& test : tests) {
            if (std::find(testNames.begin(), testNames.end(), test->getName()) != testNames.end()) {
                filteredTests.push_back(std::move(test));
            }
        }
        tests = std::move(filteredTests);
    }
    
    // Sort tests according to configuration
    std::string testOrder = config.getString("test_order", "alphabetical");
    if (testOrder == "category") {
        std::sort(tests.begin(), tests.end(), [](const auto& a, const auto& b) {
            return a->getMetadata().category < b->getMetadata().category;
        });
    } else if (testOrder == "phase") {
        std::sort(tests.begin(), tests.end(), [](const auto& a, const auto& b) {
            return a->getMetadata().phase < b->getMetadata().phase;
        });
    } else if (testOrder == "alphabetical") {
        std::sort(tests.begin(), tests.end(), [](const auto& a, const auto& b) {
            return a->getName() < b->getName();
        });
    }
    
    return tests;
}

bool TestRunner::analyzeResults(const std::vector<TestResult>& results) {
    if (results.empty()) {
        return true; // No tests to analyze
    }
    
    size_t passed = 0;
    size_t failed = 0;
    size_t skipped = 0;
    
    for (const auto& result : results) {
        if (result.skipped) {
            skipped++;
        } else if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    // Log summary
    std::cout << "Test Results Summary:" << std::endl;
    std::cout << "  Total Tests: " << results.size() << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "  Skipped: " << skipped << std::endl;
    
    // Calculate success rate
    float successRate = static_cast<float>(passed) / static_cast<float>(results.size() * 100.0f);
    
    if (failed > 0) {
        std::cout << "  Success Rate: " << successRate << "%" << std::endl;
        return false; // Tests failed
    }
    
    return true; // All tests passed
}

std::string TestRunner::generateSummary(const std::vector<TestResult>& results) {
    std::stringstream ss;
    
    ss << "Test Execution Summary" << std::endl;
    ss << "=====================" << std::endl;
    ss << "Total Tests: " << results.size() << std::endl;
    
    size_t passed = 0, failed = 0, skipped = 0;
    for (const auto& result : results) {
        if (result.skipped) {
            skipped++;
        } else if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    ss << "Passed: " << passed << std::endl;
    ss << "Failed: " << failed << std::endl;
    ss << "Skipped: " << skipped << std::endl;
    
    if (failed > 0) {
        ss << std::endl << "Failed Tests:" << std::endl;
        for (const auto& result : results) {
            if (!result.passed && !result.skipped) {
                ss << "  - " << result.testName << ": " << result.errorMessage << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::map<std::string, std::string> TestRunner::calculateStatistics(const std::vector<TestResult>& results) {
    std::map<std::string, std::string> stats;
    
    if (results.empty()) {
        return stats;
    }
    
    size_t total = results.size();
    size_t passed = 0, failed = 0, skipped = 0;
    std::chrono::milliseconds totalDuration(0);
    
    for (const auto& result : results) {
        if (result.skipped) {
            skipped++;
        } else if (result.passed) {
            passed++;
        } else {
            failed++;
        }
        totalDuration += result.duration;
    }
    
    stats["total_tests"] = std::to_string(total);
    stats["passed_tests"] = std::to_string(passed);
    stats["failed_tests"] = std::to_string(failed);
    stats["skipped_tests"] = std::to_string(skipped);
    stats["success_rate"] = (total > 0) ? std::to_string((static_cast<float>(passed) / static_cast<float>(total)) * 100.0f) : "0.0";
    stats["total_duration_ms"] = std::to_string(totalDuration.count());
    stats["average_duration_ms"] = (total > 0) ? std::to_string(static_cast<float>(totalDuration.count()) / static_cast<float>(total)) : "0.0";
    
    return stats;
}

bool TestRunner::setupTestEnvironment(const TestConfig& config, std::shared_ptr<Brain>& brain) {
    try {
        // Create brain with configuration
        brain = createBrain(config);
        if (!brain) {
            std::cerr << "Failed to create brain" << std::endl;
            return false;
        }
        
        // Initialize brain
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Load brain state if requested
        if (config.getParameters().restoreBrainState) {
            std::string loadPath = config.getParameters().brainLoadPath;
            if (loadPath.empty()) {
                loadPath = config.getParameters().outputDirectory + "/brain_state.checkpoint";
            }
            
            if (!loadBrainState(brain, loadPath)) {
                std::cerr << "Failed to load brain state from " << loadPath << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during test environment setup: " << e.what() << std::endl;
        return false;
    }
}

bool TestRunner::teardownTestEnvironment(const TestConfig& config, std::shared_ptr<Brain>& brain) {
    try {
        if (!brain) {
            return true;
        }
        
        // Save brain state if requested
        if (config.getParameters().saveBrainState) {
            std::string savePath = config.getParameters().brainSavePath;
            if (savePath.empty()) {
                savePath = config.getParameters().outputDirectory + "/brain_state.checkpoint";
            }
            
            if (!saveBrainState(brain, savePath)) {
                std::cerr << "Failed to save brain state to " << savePath << std::endl;
                return false;
            }
        }
        
        // Reset brain
        resetBrain(brain, config);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during test environment teardown: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Brain> TestRunner::createBrain(const TestConfig& config) {
    try {
        // Create config from test configuration
        auto brainConfig = std::make_shared<Config>();
        
        // Set basic brain parameters
        brainConfig->set("neuron_count", static_cast<int64_t>(500));
        brainConfig->set("region_count", static_cast<int64_t>(1));
        brainConfig->set("connection_probability", 0.15f);
        
        // Set timestep
        double timestep = config.getParameters().timestep;
        brainConfig->set("simulation_timestep", timestep);
        
        // Set random seed
        int64_t randomSeed = config.getInt("random_seed", 42);
        brainConfig->set("random_seed", randomSeed);
        
        // Set plasticity parameters
        brainConfig->set("stdp_ltp_weight", 0.02f);
        brainConfig->set("stdp_ltd_weight", 0.015f);
        brainConfig->set("stdp_tau", 20.0f);
        brainConfig->set("synaptogenesis_rate", 0.0001f);
        brainConfig->set("pruning_rate", 0.00001f);
        
        // Create brain instance
        auto brain = std::make_shared<Brain>(brainConfig);
        
        return brain;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception creating brain: " << e.what() << std::endl;
        return nullptr;
    }
}

bool TestRunner::resetBrain(std::shared_ptr<Brain> brain, const TestConfig& config) {
    if (!brain) {
        return false;
    }
    
    try {
        brain->reset();
        
        // Re-initialize if needed
        if (config.getParameters().resetBrainBetweenTests) {
            if (!brain->initialize()) {
                std::cerr << "Failed to re-initialize brain" << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception resetting brain: " << e.what() << std::endl;
        return false;
    }
}

bool TestRunner::saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        std::cerr << "Cannot save brain state: brain is null" << std::endl;
        return false;
    }
    
    try {
        if (brain->save(filepath)) {
            std::cout << "Brain state saved to " << filepath << std::endl;
            return true;
        } else {
            std::cerr << "Failed to save brain state to " << filepath << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception saving brain state: " << e.what() << std::endl;
        return false;
    }
}

bool TestRunner::loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        std::cerr << "Cannot load brain state: brain is null" << std::endl;
        return false;
    }
    
    try {
        if (brain->load(filepath)) {
            std::cout << "Brain state loaded from " << filepath << std::endl;
            return true;
        } else {
            std::cerr << "Failed to load brain state from " << filepath << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception loading brain state: " << e.what() << std::endl;
        return false;
    }
}

bool TestRunner::validateTestConfig(const TestConfig& config, std::vector<std::string>& errors) {
    return config.validate(errors);
}

bool TestRunner::validateTestDependencies(const std::vector<std::unique_ptr<TestBase>>& tests) {
    // Check for circular dependencies
    // In a real implementation, this would analyze the dependency graph
    return true;
}

} // namespace nlm
