// Test configuration
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm_tests, m) {
    m.doc() = "NLM Test Module for Configuration Testing";

    // Include all test classes
    py::class_<Config>(m, "ConfigTest")
        .def(py::init<>())
        .def("run_all_tests", [](Config& self) {
            std::string results = "Running Configuration Tests...\n";
            
            // Test 1: Basic operations
            self.set("test.int", 42);
            self.set("test.float", 3.14);
            self.set("test.string", "hello");
            self.set("test.bool", true);
            
            if (self.get<int64_t>("test.int").value_or(0) != 42) {
                throw py::value_error("Test 1 failed: int value mismatch");
            }
            
            if (self.get<double>("test.float").value_or(0.0) != 3.14) {
                throw py::value_error("Test 1 failed: float value mismatch");
            }
            
            if (self.get<std::string>("test.string").value_or("") != "hello") {
                throw py::value_error("Test 1 failed: string value mismatch");
            }
            
            if (self.get<bool>("test.bool").value_or(false) != true) {
                throw py::value_error("Test 1 failed: bool value mismatch");
            }
            
            results += "✓ Test 1 PASSED: Basic operations\n";
            
            // Test 2: Type safety
            if (self.get<int64_t>("test.string").has_value()) {
                throw py::value_error("Test 2 failed: type safety violation");
            }
            
            results += "✓ Test 2 PASSED: Type safety\n";
            
            // Test 3: Default values
            if (self.getOr<int64_t>("missing", 999) != 999) {
                throw py::value_error("Test 3 failed: default value not working");
            }
            
            results += "✓ Test 3 PASSED: Default values\n";
            
            // Test 4: Key existence
            if (!self.has("test.int") || !self.has("test.float")) {
                throw py::value_error("Test 4 failed: key existence check");
            }
            
            results += "✓ Test 4 PASSED: Key existence\n";
            
            // Test 5: Clear operation
            self.clear();
            if (self.getKeys().size() != 0) {
                throw py::value_error("Test 5 failed: clear operation");
            }
            
            results += "✓ Test 5 PASSED: Clear operation\n";
            
            results += "\nAll configuration tests PASSED!";
            return results;
        }, "Run all configuration tests");

    py::class_<Config>(m, "ConfigAnalysisTest")
        .def(py::init<>())
        .def("analyze_and_validate", [](Config& self) {
            std::string results = "Running Configuration Analysis...\n";
            
            // Add test data
            self.set("neuron_count", 1000);
            self.set("connection_rate", 0.05);
            self.set("enabled_flag", true);
            self.set("model_name", "test_model");
            
            // Test analysis
            std::string analysis = self.analyze_config();
            if (analysis.find("Configuration Analysis") == std::string::npos) {
                throw py::value_error("Analysis failed: missing analysis header");
            }
            
            // Test quality score
            int quality = self.get_quality_score();
            if (quality < 0 || quality > 100) {
                throw py::value_error("Quality score out of range: " + std::to_string(quality));
            }
            
            // Test JSON export
            std::string json = self.exportToJson();
            if (json.find("neuron_count") == std::string::npos) {
                throw py::value_error("JSON export missing neuron_count");
            }
            
            results += "✓ Analysis completed successfully\n";
            results += "✓ Quality score: " + std::to_string(quality) + "/100\n";
            results += "✓ JSON export: " + std::to_string(json.size()) + " characters\n";
            results += "\nAnalysis tests PASSED!";
            return results;
        }, "Analyze and validate configuration");
}
