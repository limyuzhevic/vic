// NLM Test Discovery Infrastructure
// Automated test discovery and categorization

#include "nlm_testing_framework.hpp"
#include <filesystem>
#include <fstream>
#include <regex>
#include <map>

namespace fs = std::filesystem;

namespace {
    // Test discovery utilities
    class TestDiscovery {
    private:
        std::string sourceDirectory;
        std::map<std::string, TestCategory> categoryMapping;
        
    public:
        TestDiscovery(const std::string& dir) : sourceDirectory(dir) {
            initializeCategoryMapping();
        }
        
    private:
        void initializeCategoryMapping() {
            categoryMapping[".*brain.*"] = TestCategory::UNIT;
            categoryMapping[".*neuron.*"] = TestCategory::UNIT;
            categoryMapping[".*synapse.*"] = TestCategory::UNIT;
            categoryMapping[".*stdp.*"] = TestCategory::UNIT;
            categoryMapping[".*plasticity.*"] = TestCategory::UNIT;
            categoryMapping[".*memory.*"] = TestCategory::UNIT;
            categoryMapping[".*neuromodulation.*"] = TestCategory::UNIT;
            categoryMapping[".*clock.*"] = TestCategory::UNIT;
            categoryMapping[".*random.*"] = TestCategory::UNIT;
            categoryMapping[".*config.*"] = TestCategory::CONFIGURATION;
            categoryMapping[".*perf.*"] = TestCategory::PERFORMANCE;
            categoryMapping[".*bench.*"] = TestCategory::PERFORMANCE;
            categoryMapping[".*error.*"] = TestCategory::ERROR_HANDLING;
            categoryMapping[".*integration.*"] = TestCategory::INTEGRATION;
            categoryMapping[".*system.*"] = TestCategory::SYSTEM_INTERACTION;
            categoryMapping[".*cli.*"] = TestCategory::CLI;
        }
        
        TestCategory determineCategory(const std::string& filename) {
            for (const auto& pattern : categoryMapping) {
                std::regex regex(pattern.first, std::regex::icase);
                if (std::regex_match(filename, regex)) {
                    return pattern.second;
                }
            }
            return TestCategory::UNIT; // Default category
        }
        
        bool isTestFile(const std::string& filename) {
            std::string ext = fs::path(filename).extension().string();
            return ext == ".cpp" || ext == ".cc" || ext == ".cxx";
        }
        
        bool containsTestCases(const std::string& filepath) {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Look for test patterns
            std::regex testPatterns[] = {
                std::regex("TEST_CASE\s*\("),
                std::regex("void\s+test_[a-zA-Z_]+\s*\("),
                std::regex("namespace\s+test_[a-zA-Z_]+"),
                std::regex("nlm::assert"),
                std::regex("TestAssert::")
            };
            
            for (const auto& pattern : testPatterns) {
                if (std::regex_search(content, pattern)) {
                    return true;
                }
            }
            
            return false;
        }
        
    public:
        std::vector<std::string> discoverTestFiles(const std::string& directory) {
            std::vector<std::string> testFiles;
            
            try {
                for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                    if (entry.is_regular_file() && isTestFile(entry.path().filename().string())) {
                        std::string filepath = entry.path().string();
                        if (containsTestCases(filepath)) {
                            testFiles.push_back(filepath);
                        }
                    }
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error discovering test files: " << e.what() << std::endl;
            }
            
            return testFiles;
        }
        
        std::vector<std::string> discoverTestSuites(const std::string& directory) {
            std::vector<std::string> suiteNames;
            
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file() && isTestFile(entry.path().filename().string())) {
                    std::string filename = entry.path().filename().string();
                    suiteNames.push_back(filename);
                }
            }
            
            return suiteNames;
        }
        
        void generateTestReport(const std::vector<std::string>& testFiles, const std::string& outputFile) {
            std::ofstream file(outputFile);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot create test report file: " << outputFile << std::endl;
                return;
            }
            
            file << "NLM Test Discovery Report" << std::endl;
            file << "=========================" << std::endl;
            file << "Generated on: " << __DATE__ << " " << __TIME__ << std::endl;
            file << std::endl;
            file << "Discovered Test Files:" << std::endl;
            file << "----------------------" << std::endl;
            
            int totalTests = 0;
            std::map<TestCategory, int> categoryCounts;
            
            for (const auto& testFile : testFiles) {
                std::string filename = fs::path(testFile).filename().string();
                TestCategory category = determineCategory(filename);
                categoryCounts[category]++;
                
                file << "  " << filename << " (Category: " << static_cast<int>(category) << ")" << std::endl;
                totalTests++;
            }
            
            file << std::endl;
            file << "Test Category Summary:" << std::endl;
            file << "---------------------" << std::endl;
            
            for (int i = 0; i < 7; ++i) {
                TestCategory category = static_cast<TestCategory>(i);
                auto it = categoryCounts.find(category);
                if (it != categoryCounts.end()) {
                    file << "  " << static_cast<int>(category) << ": " << it->second << " files" << std::endl;
                }
            }
            
            file << std::endl;
            file << "Total Test Files: " << totalTests << std::endl;
            file << std::endl;
            file << "Recommendations:" << std::endl;
            file << "---------------" << std::endl;
            
            if (categoryCounts[TestCategory::PERFORMANCE] == 0) {
                file << "  Add performance tests for benchmarking" << std::endl;
            }
            
            if (categoryCounts[TestCategory::INTEGRATION] == 0) {
                file << "  Add integration tests for system interactions" << std::endl;
            }
            
            if (categoryCounts[TestCategory::ERROR_HANDLING] == 0) {
                file << "  Add error handling and exception safety tests" << std::endl;
            }
            
            if (categoryCounts[TestCategory::CLI] == 0) {
                file << "  Add command-line interface tests" << std::endl;
            }
            
            file.close();
            
            std::cout << "Test report generated: " << outputFile << std::endl;
        }
    };
}

// Public interface for test discovery
namespace nlm {
    class TestDiscoveryAPI {
    public:
        static std::vector<std::string> discoverTests(const std::string& directory) {
            TestDiscovery discoverer(directory);
            return discoverer.discoverTestFiles(directory);
        }
        
        static std::vector<std::string> discoverTestSuites(const std::string& directory) {
            TestDiscovery discoverer(directory);
            return discoverer.discoverTestSuites(directory);
        }
        
        static void generateTestReport(const std::string& directory, const std::string& outputFile) {
            TestDiscovery discoverer(directory);
            auto testFiles = discoverer.discoverTestFiles(directory);
            discoverer.generateTestReport(testFiles, outputFile);
        }
    };
}