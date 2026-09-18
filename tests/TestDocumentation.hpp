#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

// Test documentation structure for improved test maintainability
struct TestDocumentation {
    // Test documentation metadata
    struct Metadata {
        std::string title;
        std::string description;
        std::string purpose;
        std::string author;
        std::string dateCreated;
        std::string lastModified;
        std::string tags;
        std::string category;
        std::string references;
        std::string prerequisites;
        std::string relatedTests;
        std::string expectedOutcome;
        std::string failureConditions;
    } metadata;
    
    // Test case documentation
    struct TestCaseDoc {
        std::string name;
        std::string description;
        std::string preconditions;
        std::string postconditions;
        std::string steps;
        std::string expectedResults;
        std::string failureModes;
        std::string testType;
        double estimatedTimeMs = 0.0;
        int estimatedComplexity = 1;  // 1=Low, 2=Medium, 3=High
        bool requiresIsolation = false;
        std::vector<std::string> dependencies;
        std::vector<std::string> testDataRequirements;
    };
    
    // Test suite documentation
    struct TestSuiteDoc {
        std::string name;
        std::string description;
        std::string purpose;
        std::string testEnvironment;
        std::string setupRequirements;
        std::string cleanupRequirements;
        std::vector<TestCaseDoc> testCases;
        std::unordered_map<std::string, std::string> configurationNotes;
    } suiteDoc;
    
    // Test configuration documentation
    struct ConfigDoc {
        std::string parameterName;
        std::string type;
        std::string defaultValue;
        std::string description;
        std::string validationRules;
        std::string impactOnTest;
        bool tunable = true;
        std::string unit;
    };
    
    // Test result documentation
    struct ResultDoc {
        std::string testName;
        std::string timestamp;
        bool passed;
        std::string errorDetails;
        std::string executionContext;
        double executionTimeMs;
        std::vector<std::string> logs;
    };
    
    // Constructor
    TestDocumentation() = default;
    
    // Add test case documentation
    void addTestCase(const TestCaseDoc& testCase) {
        suiteDoc.testCases.push_back(testCase);
    }
    
    // Get test case by name
    const TestCaseDoc* getTestCase(const std::string& name) const {
        for (const auto& testCase : suiteDoc.testCases) {
            if (testCase.name == name) {
                return &testCase;
            }
        }
        return nullptr;
    }
    
    // Generate documentation for test suite
    std::string generateSuiteDocumentation() const {
        std::string doc;
        
        doc += "=== Test Suite Documentation ===\n\n";
        doc += "Suite: " + suiteDoc.name + "\n";
        doc += "Description: " + suiteDoc.description + "\n";
        doc += "Purpose: " + suiteDoc.purpose + "\n\n";
        
        doc += "Test Environment: " + suiteDoc.testEnvironment + "\n";
        doc += "Setup Requirements: " + suiteDoc.setupRequirements + "\n";
        doc += "Cleanup Requirements: " + suiteDoc.cleanupRequirements + "\n\n";
        
        doc += "Test Cases:\n";
        for (const auto& testCase : suiteDoc.testCases) {
            doc += "  - " + testCase.name + "\n";
            doc += "    Description: " + testCase.description + "\n";
            doc += "    Preconditions: " + testCase.preconditions + "\n";
            doc += "    Postconditions: " + testCase.postconditions + "\n";
            doc += "    Steps: " + testCase.steps + "\n";
            doc += "    Expected Results: " + testCase.expectedResults + "\n";
            doc += "    Failure Modes: " + testCase.failureModes + "\n";
            doc += "    Estimated Time: " + std::to_string(testCase.estimatedTimeMs) + " ms\n";
            doc += "    Complexity: " + std::to_string(testCase.estimatedComplexity) + "\n\n";
        }
        
        doc += "Configuration Notes:\n";
        for (const auto& note : suiteDoc.configurationNotes) {
            doc += "  " + note.first + ": " + note.second + "\n";
        }
        
        return doc;
    }
    
    // Generate test configuration documentation
    std::string generateConfigDocumentation() const {
        std::string doc;
        
        doc += "=== Test Configuration Documentation ===\n\n";
        
        return doc;
    }
    
    // Record test result documentation
    void recordResult(const ResultDoc& result) {
        // In a real implementation, this would write to a results file or database
        // For now, we'll just store it in memory
        // You might want to add a results vector to store this information
    }
    
    // Get documentation for a specific test
    std::string getTestDocumentation(const std::string& testName) const {
        const TestCaseDoc* testCase = getTestCase(testName);
        if (!testCase) {
            return "Test not found: " + testName + "\n";
        }
        
        std::string doc;
        doc += "=== Test Documentation ===\n\n";
        doc += "Test Name: " + testCase->name + "\n";
        doc += "Description: " + testCase->description + "\n";
        doc += "Preconditions: " + testCase->preconditions + "\n";
        doc += "Postconditions: " + testCase->postconditions + "\n";
        doc += "Steps: " + testCase->steps + "\n";
        doc += "Expected Results: " + testCase->expectedResults + "\n";
        doc += "Failure Modes: " + testCase->failureModes + "\n";
        doc += "Test Type: " + testCase->testType + "\n";
        doc += "Estimated Time: " + std::to_string(testCase->estimatedTimeMs) + " ms\n";
        doc += "Complexity: " + std::to_string(testCase->estimatedComplexity) + "\n";
        
        return doc;
    }
};

// Global test documentation instance
inline TestDocumentation& getTestDocumentation() {
    static TestDocumentation doc;
    return doc;
}

// Helper functions for test documentation
inline void initializeTestDocumentation(const std::string& suiteName, const std::string& description) {
    TestDocumentation& doc = getTestDocumentation();
    doc.suiteDoc.name = suiteName;
    doc.suiteDoc.description = description;
}

inline void addTestCaseDocumentation(const std::string& name, const std::string& description, 
                                   const std::string& steps, const std::string& expectedResults) {
    TestDocumentation::TestCaseDoc testCase;
    testCase.name = name;
    testCase.description = description;
    testCase.steps = steps;
    testCase.expectedResults = expectedResults;
    
    getTestDocumentation().addTestCase(testCase);
}

inline void printTestDocumentation() {
    std::cout << getTestDocumentation().generateSuiteDocumentation() << std::endl;
}

} // namespace nlm
