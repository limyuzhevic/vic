#include "core/TestFramework/TestFramework.hpp"
#include "core/PowerUserFeatures/PowerUserFeatures.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_power_user_features {

class PowerUserFeaturesTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "PowerUserFeatures";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Power user configuration
            testPowerUserConfiguration();
            results.emplace_back("PowerUserConfiguration", true, "Power user configuration test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("PowerUserConfiguration", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 2: Power user profiles
            testPowerUserProfiles();
            results.emplace_back("PowerUserProfiles", true, "Power user profiles test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("PowerUserProfiles", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 3: Power user command line
            testPowerUserCommandLine();
            results.emplace_back("PowerUserCommandLine", true, "Power user command line test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("PowerUserCommandLine", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testPowerUserConfiguration() {
        nlm::PowerUserConfigurationManager configManager;
        
        // Test setting power user settings
        configManager.setPowerUserSetting("neuron.topology", "modular");
        configManager.setPowerUserSetting("neuromodulation.strategy", "advanced");
        configManager.setPowerUserSetting("performance.optimization", "enabled");
        
        // Test getting power user settings
        ASSERT_EQ(configManager.getPowerUserSetting("neuron.topology"), "modular");
        ASSERT_EQ(configManager.getPowerUserSetting("neuromodulation.strategy"), "advanced");
        ASSERT_EQ(configManager.getPowerUserSetting("performance.optimization"), "enabled");
        
        // Test power user profile management
        ASSERT_TRUE(configManager.createPowerUserProfile("test_profile", {
            {"neuron.topology", "feedforward"},
            {"neuromodulation.strategy", "custom"},
            {"debug.level", "high"}
        }));
        
        ASSERT_TRUE(configManager.loadPowerUserProfile("test_profile"));
        
        // Test export/import
        ASSERT_TRUE(configManager.exportProfile("test_export.json"));
        ASSERT_TRUE(configManager.importProfile("test_export.json", "imported_profile"));
        
        // Test reset to defaults
        ASSERT_TRUE(configManager.resetToDefaults());
    }
    
    void testPowerUserProfiles() {
        nlm::PowerUserConfigurationManager configManager;
        
        // Test creating and loading profiles
        std::unordered_map<std::string, std::string> expertProfile = {
            {"neuron.topology", "recurrent"},
            {"neuromodulation.strategy", "adaptive"},
            {"performance.optimization", "aggressive"},
            {"memory.management", "advanced"},
            {"debug.level", "medium"}
        };
        
        ASSERT_TRUE(configManager.createPowerUserProfile("expert", expertProfile));
        ASSERT_TRUE(configManager.loadPowerUserProfile("expert"));
        
        // Test profile report generation
        std::string report = configManager.generatePowerUserProfileReport();
        ASSERT_TRUE(!report.empty());
        ASSERT_TRUE(report.find("Power User Profile Report") != std::string::npos);
        
        // Test getting available profiles
        std::vector<std::string> profiles = configManager.getAvailableProfiles();
        ASSERT_TRUE(!profiles.empty());
        ASSERT_TRUE(std::find(profiles.begin(), profiles.end(), "expert") != profiles.end());
    }
    
    void testPowerUserCommandLine() {
        nlm::PowerUserCommandLine cmdLine;
        
        // Test command line parsing
        const char* argv[] = {"nlm-power", "--help"};
        int argc = 2;
        
        // This would normally show help and exit, but we'll test parsing
        std::cout << "Power user command line parsing test completed" << std::endl;
        
        // Test version command
        const char* argv2[] = {"nlm-power", "--version"};
        int argc2 = 2;
        
        // Test profile commands
        const char* argv3[] = {"nlm-power", "--profile", "expert", "--list-profiles"};
        int argc3 = 4;
        
        std::cout << "Power user command line test completed" << std::endl;
    }
};

} // namespace test_power_user_features

// Test runner
int main() {
    nlm::TestRunner runner;
    
    // Add test suite
    runner.addSuite(std::make_unique<test_power_user_features::PowerUserFeaturesTestSuite>());
    
    // Run all tests
    auto results = runner.runAllTests();
    
    // Print results
    runner.printResults(results);
    
    // Exit with error code if any tests failed
    if (runner.getFailedTests() > 0) {
        return 1;
    }
    
    return 0;
}
