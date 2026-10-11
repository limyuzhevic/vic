// NLM Memory Leak Detection Tests Implementation
// Phase 7: Comprehensive Test Suite

#include "test_infrastructure/memory_leak_tests/test_memory_leak_detection.hpp"

// Global memory leak test runner
MemoryLeakTestRunner global_memory_leak_runner;

// Test configuration factory
class MemoryLeakTestConfigFactory {
public:
    static std::vector<MemoryLeakTestConfig> create_default_memory_leak_tests() {
        std::vector<MemoryLeakTestConfig> configs;
        
        // Neuron memory leak tests
        configs.push_back(MemoryLeakTestConfig(
            "test_neuron_creation_leak_detection",
            "Test for memory leaks during neuron creation and destruction",
            100,
            {"neuron", "creation", "destruction"}
        ));
        
        configs.back().tags = {"memory", "neuron", "creation", "destruction"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_neuron_parameter_leak_detection",
            "Test for memory leaks during neuron parameter operations",
            50,
            {"neuron", "parameter", "initialization"}
        ));
        
        configs.back().tags = {"memory", "neuron", "parameter", "initialization"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_neuron_lifecycle_leak_detection",
            "Test for memory leaks during neuron lifecycle management",
            75,
            {"neuron", "lifecycle", "management"}
        ));
        
        configs.back().tags = {"memory", "neuron", "lifecycle", "management"};
        
        // Synapse memory leak tests
        configs.push_back(MemoryLeakTestConfig(
            "test_synapse_creation_leak_detection",
            "Test for memory leaks during synapse creation and destruction",
            100,
            {"synapse", "creation", "destruction"}
        ));
        
        configs.back().tags = {"memory", "synapse", "creation", "destruction"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_synapse_plasticity_leak_detection",
            "Test for memory leaks during synapse plasticity operations",
            50,
            {"synapse", "plasticity", "memory"}
        ));
        
        configs.back().tags = {"memory", "synapse", "plasticity", "memory"};
        
        // Brain memory leak tests
        configs.push_back(MemoryLeakTestConfig(
            "test_brain_creation_leak_detection",
            "Test for memory leaks during brain creation and destruction",
            25,
            {"brain", "creation", "destruction"}
        ));
        
        configs.back().tags = {"memory", "brain", "creation", "destruction"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_brain_neuron_management_leak_detection",
            "Test for memory leaks during brain neuron management",
            50,
            {"brain", "neuron", "management"}
        ));
        
        configs.back().tags = {"memory", "brain", "neuron", "management"};
        
        return configs;
    }
    
    static std::vector<MemoryLeakTestConfig> create_stress_memory_leak_tests() {
        std::vector<MemoryLeakTestConfig> configs;
        
        // High-load memory leak tests
        configs.push_back(MemoryLeakTestConfig(
            "test_high_load_neuron_creation",
            "Test for memory leaks under high neuron creation load",
            1000,
            {"stress", "neuron", "creation", "high_load"}
        ));
        
        configs.back().tags = {"memory", "stress", "neuron", "creation", "high_load"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_high_load_synapse_creation",
            "Test for memory leaks under high synapse creation load",
            1000,
            {"stress", "synapse", "creation", "high_load"}
        ));
        
        configs.back().tags = {"memory", "stress", "synapse", "creation", "high_load"};
        
        configs.push_back(MemoryLeakTestConfig(
            "test_high_load_brain_operations",
            "Test for memory leaks under high brain load",
            100,
            {"stress", "brain", "operations", "high_load"}
        ));
        
        configs.back().tags = {"memory", "stress", "brain", "operations", "high_load"};
        
        return configs;
    }
};

// Test configuration loader
class MemoryLeakTestConfigLoader {
public:
    static bool load_memory_leak_tests_from_file(const std::string& filename,
                                                std::vector<MemoryLeakTestConfig>& configs) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open memory leak test configuration file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        MemoryLeakTestConfig current_config("", "", 0, {});
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Parse configuration key-value pairs
            size_t separator = line.find('=');
            if (separator != std::string::npos) {
                std::string key = line.substr(0, separator);
                std::string value = line.substr(separator + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                // Parse configuration
                if (key == "test_name") {
                    if (!current_config.test_name.empty()) {
                        configs.push_back(current_config);
                    }
                    current_config = MemoryLeakTestConfig(value, "", 0, {});
                } else if (key == "description") {
                    current_config.description = value;
                } else if (key == "allocation_count") {
                    current_config.allocation_count = std::stoi(value);
                } else if (key == "component_types") {
                    // Parse comma-separated component types
                    std::stringstream ss(value);
                    std::string component;
                    std::vector<std::string> components;
                    while (std::getline(ss, component, ',')) {
                        components.push_back(component);
                    }
                    current_config.component_types = components;
                }
            }
        }
        
        // Add last configuration
        if (!current_config.test_name.empty()) {
            configs.push_back(current_config);
        }
        
        file.close();
        return true;
    }
    
    static void save_memory_leak_tests_to_file(const std::vector<MemoryLeakTestConfig>& configs,
                                              const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create memory leak test configuration file: " + filename);
        }
        
        file << "# NLM Memory Leak Test Configuration" << std::endl;
        file << "# Generated automatically by memory leak test configuration system" << std::endl;
        file << std::endl;
        
        for (const auto& config : configs) {
            file << "test_name = " << config.test_name << std::endl;
            file << "description = " << config.description << std::endl;
            file << "allocation_count = " << config.allocation_count << std::endl;
            
            if (!config.component_types.empty()) {
                file << "component_types = ";
                for (size_t i = 0; i < config.component_types.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << config.component_types[i];
                }
                file << std::endl;
            }
            
            file << std::endl;
        }
        
        file.close();
    }
};

// Memory leak test reporter
class MemoryLeakTestReporter {
public:
    static std::string generate_html_report(const std::vector<MemoryLeakTestResult>& results) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html>\n";
        ss << "<head>\n";
        ss << "  <title>NLM Memory Leak Test Report</title>\n";
        ss << "  <style>\n";
        ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
        ss << "    h1 { color: #333; }\n";
        ss << "    h2 { color: #555; border-bottom: 2px solid #ccc; padding-bottom: 5px; }\n";
        ss << "    .passed { background-color: #d4edda; color: #155724; }\n";
        ss << "    .failed { background-color: #f8d7da; color: #721c24; }\n";
        ss << "    table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }\n";
        ss << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        ss << "    th { background-color: #f2f2f2; }\n";
        ss << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
        ss << "    .leak-detected { background-color: #fff3cd; color: #856404; }\n";
        ss << "  </style>\n";
        ss << "</head>\n";
        ss << "<body>\n";
        ss << "  <h1>NLM Memory Leak Test Report</h1>\n";
        ss << "  <p>Generated on: " << get_current_time() << "</p>\n";
        ss << "\n";
        
        ss << "  <h2>Test Summary</h2>\n";
        ss << "  <table>\n";
        ss << "    <thead>\n";
        ss << "      <tr>\n";
        ss << "        <th>Test Name</th>\n";
        ss << "        <th>Status</th>\n";
        ss << "        <th>Detected Leaks</th>\n";
        ss << "        <th>Expected Leaks</th>\n";
        ss << "        <th>Leak Percentage</th>\n";
        ss << "        <th>Execution Time (s)</th>\n";
        ss << "      </tr>\n";
        ss << "    </thead>\n";
        ss << "    <tbody>\n";
        
        size_t total_tests = results.size();
        size_t passed_tests = 0;
        size_t failed_tests = 0;
        size_t total_leaks = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed_tests++;
            } else {
                failed_tests++;
            }
            total_leaks += result.detected_leaks;
        }
        
        ss << "      <tr>\n";
        ss << "        <td>Total Tests</td>\n";
        ss << "        <td>" << total_tests << "</td>\n";
        ss << "        <td>" << passed_tests << "</td>\n";
        ss << "        <td>" << failed_tests << "</td>\n";
        ss << "        <td>" << total_leaks << "</td>\n";
        ss << "        <td>-</td>\n";
        ss << "        <td>-</td>\n";
        ss << "      </tr>\n";
        
        for (const auto& result : results) {
            std::string status_class = result.passed ? "passed" : "failed";
            std::string leak_class = result.has_leaks() ? "leak-detected" : "";
            
            ss << "      <tr class=\"" << status_class << "\">\n";
            ss << "        <td>" << result.test_name << "</td>\n";
            ss << "        <td>" << (result.passed ? "PASSED" : "FAILED") << "</td>\n";
            ss << "        <td class=\"" << leak_class << "\">" << result.detected_leaks << "</td>\n";
            ss << "        <td>" << result.expected_leaks << "</td>\n";
            ss << "        <td>" << std::fixed << std::setprecision(2) << result.leak_percentage() << "%</td>\n";
            ss << "        <td>" << std::fixed << std::setprecision(3) << result.execution_time << "</td>\n";
            ss << "      </tr>\n";
        }
        
        ss << "    </tbody>\n";
        ss << "  </table>\n";
        ss << "\n";
        
        ss << "  <h2>Detailed Results</h2>\n";
        
        for (const auto& result : results) {
            ss << "  <h3>" << result.test_name << "</h3>\n";
            ss << "  <p>Status: <strong>" << (result.passed ? "PASSED" : "FAILED") << "</strong></p>\n";
            ss << "  <p>Detected Leaks: <strong>" << result.detected_leaks << "</strong></p>\n";
            ss << "  <p>Expected Leaks: <strong>" << result.expected_leaks << "</strong></p>\n";
            ss << "  <p>Leak Percentage: <strong>" << std::fixed << std::setprecision(2) << result.leak_percentage() << "%</strong></p>\n";
            ss << "  <p>Execution Time: <strong>" << std::fixed << std::setprecision(3) << result.execution_time << " seconds</strong></p>\n";
            
            if (!result.error_message.empty()) {
                ss << "  <p>Error: <strong>" << escape_html(result.error_message) << "</strong></p>\n";
            }
            
            if (!result.leak_descriptions.empty()) {
                ss << "  <p>Leak Descriptions:</p>\n";
                ss << "  <ul>\n";
                for (const auto& desc : result.leak_descriptions) {
                    ss << "    <li>" << escape_html(desc) << "</li>\n";
                }
                ss << "  </ul>\n";
            }
            
            ss << "\n";
        }
        
        ss << "</body>\n";
        ss << "</html>\n";
        
        return ss.str();
    }
    
    static std::string generate_json_report(const std::vector<MemoryLeakTestResult>& results) {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"report_type\": \"memory_leak\",\n";
        ss << "  \"generated_time\": \"" << get_current_time() << "\",\n";
        ss << "  \"test_count\": " << results.size() << ",\n";
        ss << "  \"passed_tests\": " << std::count_if(results.begin(), results.end(),
            [](const MemoryLeakTestResult& r) { return r.passed; }) << ",\n";
        ss << "  \"failed_tests\": " << std::count_if(results.begin(), results.end(),
            [](const MemoryLeakTestResult& r) { return !r.passed; }) << ",\n";
        ss << "  \"total_leaks_detected\": " << std::accumulate(results.begin(), results.end(), 0,
            [](int sum, const MemoryLeakTestResult& r) { return sum + r.detected_leaks; }) << ",\n";
        ss << "  \"results\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            ss << "    {\n";
            ss << "      \"test_name\": \"" << escape_json(result.test_name) << "\",\n";
            ss << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
            ss << "      \"detected_leaks\": " << result.detected_leaks << ",\n";
            ss << "      \"expected_leaks\": " << result.expected_leaks << ",\n";
            ss << "      \"leak_percentage\": " << result.leak_percentage() << ",\n";
            ss << "      \"execution_time\": " << result.execution_time << ",\n";
            if (!result.error_message.empty()) {
                ss << "      \"error_message\": \"" << escape_json(result.error_message) << "\",\n";
            }
            if (!result.leak_descriptions.empty()) {
                ss << "      \"leak_descriptions\": [";
                for (size_t j = 0; j < result.leak_descriptions.size(); ++j) {
                    if (j > 0) ss << ", ";
                    ss << "\"" << escape_json(result.leak_descriptions[j]) << "\"";
                }
                ss << "],\n";
            }
            ss << "      \"has_leaks\": " << (result.has_leaks() ? "true" : "false") << "\n";
            
            if (i < results.size() - 1) {
                ss << "    },\n";
            } else {
                ss << "    }\n";
            }
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    static std::string escape_html(const std::string& input) {
        std::string output = input;
        std::replace(output.begin(), output.end(), '&', "&amp;");
        std::replace(output.begin(), output.end(), '<', "&lt;");
        std::replace(output.begin(), output.end(), '>', "&gt;");
        std::replace(output.begin(), output.end(), '"', "&quot;");
        std::replace(output.begin(), output.end(), '\'', "&apos;");
        return output;
    }
    
    static std::string escape_json(const std::string& input) {
        std::string output = input;
        std::replace(output.begin(), output.end(), '\\', "\\\\");
        std::replace(output.begin(), output.end(), '"', "\\\"");
        std::replace(output.begin(), output.end(), '\b', "\\b");
        std::replace(output.begin(), output.end(), '\f', "\\f");
        std::replace(output.begin(), output.end(), '\n', "\\n");
        std::replace(output.begin(), output.end(), '\r', "\\r");
        std::replace(output.begin(), output.end(), '\t', "\\t");
        return output;
    }
    
    static std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};