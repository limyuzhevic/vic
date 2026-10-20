#pragma once

#include "TestFramework.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TestReporter {
public:
    static void writeJUnitXML(const std::string& filename, const std::vector<test_framework::TestResult>& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        file << "<testsuites tests=\"" << results.size() << "\">" << std::endl;
        
        for (const auto& result : results) {
            file << "  <testsuite name=\"" << result.name << "\">" << std::endl;
            file << "    <testcase name=\"" << result.name << "\" time=\"" 
                  << result.executionTimeMs / 1000.0 << "\"";
            
            if (result.status == test_framework::TestStatus::Failed) {
                file << ">" << std::endl;
                file << "      <failure message=\"" << result.errorDetails << "\"/>" << std::endl;
                file << "    </testcase>" << std::endl;
            } else {
                file << "/>" << std::endl;
            }
            
            file << "  </testsuite>" << std::endl;
        }
        
        file << "</testsuites>" << std::endl;
        file.close();
    }
    
    static void writeJSON(const std::string& filename, const std::vector<test_framework::TestResult>& results) {
        json j;
        j["tests"] = json::array();
        
        for (const auto& result : results) {
            json test;
            test["name"] = result.name;
            test["category"] = testCategoryToString(result.category);
            test["status"] = testStatusToString(result.status);
            test["execution_time_ms"] = result.executionTimeMs;
            test["memory_usage"] = result.memoryUsage;
            test["message"] = result.message;
            test["error_details"] = result.errorDetails;
            test["parameterized"] = result.parameterized;
            test["tags"] = result.tags;
            
            j["tests"].push_back(test);
        }
        
        std::ofstream file(filename);
        file << j.dump(2);
        file.close();
    }
    
    static void writeCSV(const std::string& filename, const std::vector<test_framework::TestResult>& results) {
        std::ofstream file(filename);
        file << "Name,Category,Status,ExecutionTimeMs,MemoryUsage,Message,ErrorDetails,Parameterized" << std::endl;
        
        for (const auto& result : results) {
            file << "\"" << result.name << "\",";
            file << "\"" << testCategoryToString(result.category) << "\",";
            file << "\"" << testStatusToString(result.status) << "\",";
            file << result.executionTimeMs << ",";
            file << result.memoryUsage << ",";
            file << "\"" << escapeCSV(result.message) << "\",";
            file << "\"" << escapeCSV(result.errorDetails) << "\",";
            file << (result.parameterized ? "true" : "false") << std::endl;
        }
        
        file.close();
    }
    
    static void generateReport(const std::string& baseFilename, const std::vector<test_framework::TestResult>& results) {
        writeJUnitXML(baseFilename + ".xml", results);
        writeJSON(baseFilename + ".json", results);
        writeCSV(baseFilename + ".csv", results);
        
        std::cout << "Reports generated:" << std::endl;
        std::cout << "  - " << baseFilename << ".xml (JUnit format)" << std::endl;
        std::cout << "  - " << baseFilename << ".json (JSON format)" << std::endl;
        std::cout << "  - " << baseFilename << ".csv (CSV format)" << std::endl;
    }
    
private:
    static std::string testCategoryToString(test_framework::TestCategory category) {
        switch (category) {
            case test_framework::TestCategory::Unit: return "Unit";
            case test_framework::TestCategory::Integration: return "Integration";
            case test_framework::TestCategory::Performance: return "Performance";
            case test_framework::TestCategory::Configuration: return "Configuration";
            case test_framework::TestCategory::AgentBehavior: return "AgentBehavior";
            case test_framework::TestCategory::WorldSimulation: return "WorldSimulation";
            case test_framework::TestCategory::Stress: return "Stress";
            case test_framework::TestCategory::Fuzz: return "Fuzz";
            case test_framework::TestCategory::PropertyBased: return "PropertyBased";
            default: return "Unknown";
        }
    }
    
    static std::string testStatusToString(test_framework::TestStatus status) {
        switch (status) {
            case test_framework::TestStatus::Pending: return "Pending";
            case test_framework::TestStatus::Running: return "Running";
            case test_framework::TestStatus::Passed: return "Passed";
            case test_framework::TestStatus::Failed: return "Failed";
            case test_framework::TestStatus::Skipped: return "Skipped";
            case test_framework::TestStatus::Error: return "Error";
            default: return "Unknown";
        }
    }
    
    static std::string escapeCSV(const std::string& value) {
        std::string escaped = value;
        if (value.find('"') != std::string::npos) {
            std::replace(escaped.begin(), escaped.end(), '"', '""');
        }
        if (value.find(',') != std::string::npos || value.find('\n') != std::string::npos) {
            escaped = "\"" + escaped + "\"";
        }
        return escaped;
    }
};