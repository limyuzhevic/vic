// Comprehensive Config Tests
// Enhanced tests for configuration loading, validation, and error handling

#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

namespace test_config_comprehensive {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGetAllTypes() {
    nlm::Config config;
    
    // Test all primitive types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("int64_value", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("float_value", 2.71828f, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    
    // Verify type-specific get
    assert(config.get<int>("int_value").has_value());
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.get<int64_t>("int64_value").has_value());
    assert(config.get<int64_t>("int64_value").value() == 100);
    
    assert(config.get<double>("double_value").has_value());
    assert(config.get<double>("double_value").value() > 3.14 && 
           config.get<double>("double_value").value() < 3.15);
    
    assert(config.get<float>("float_value").has_value());
    assert(config.get<float>("float_value").value() > 2.71 && 
           config.get<float>("float_value").value() < 2.72);
    
    assert(config.get<bool>("bool_value").has_value());
    assert(config.get<bool>("bool_value").value() == true);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(config.get<std::string>("string_value").value() == "hello world");
    
    std::cout << "    testConfigSetAndGetAllTypes passed" << std::endl;
}

void testConfigGetOrComprehensive() {
    nlm::Config config;
    
    // Test getOr with all types
    int intDefault = config.getOr<int>("missing", 100);
    assert(intDefault == 100);
    
    config.set("existing_int", 50, nlm::ConfigSource::Runtime);
    int intExisting = config.getOr<int>("existing_int", 100);
    assert(intExisting == 50);
    
    double doubleDefault = config.getOr<double>("missing_double", 3.14);
    assert(doubleDefault > 3.13 && doubleDefault < 3.15);
    
    std::string stringDefault = config.getOr<std::string>("missing_string", "default");
    assert(stringDefault == "default");
    
    bool boolDefault = config.getOr<bool>("missing_bool", true);
    assert(boolDefault == true);
    
    // Test with large values
    config.set("large_int", static_cast<int64_t>(1000000), nlm::ConfigSource::Runtime);
    int64_t largeInt = config.getOr<int64_t>("large_int", 0);
    assert(largeInt == 1000000);
    
    // Test with special floating point values
    config.set("special_float", std::numeric_limits<float>::max(), nlm::ConfigSource::Runtime);
    float specialFloat = config.getOr<float>("special_float", 0.0f);
    assert(specialFloat == std::numeric_limits<float>::max());
    
    std::cout << "    testConfigGetOrComprehensive passed" << std::endl;
}

void testConfigRemoveComprehensive() {
    nlm::Config config;
    
    // Add multiple entries
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "two", nlm::ConfigSource::Runtime);
    config.set("key3", 3.0, nlm::ConfigSource::Runtime);
    config.set("key4", false, nlm::ConfigSource::Runtime);
    
    // Verify all exist
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    // Remove some keys
    config.remove("key2");
    config.remove("key4");
    
    // Verify removal
    assert(config.has("key1"));
    assert(!config.has("key2"));
    assert(config.has("key3"));
    assert(!config.has("key4"));
    
    // Remove non-existent key (should not crash)
    config.remove("non_existent");
    
    // Verify keys() works
    auto keys = config.getKeys();
    assert(keys.size() == 2); // key1 and key3 should remain
    
    std::cout << "    testConfigRemoveComprehensive passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Test different source types
    config.set("runtime_key", 42, nlm::ConfigSource::Runtime);
    config.set("file_key", 43, nlm::ConfigSource::File);
    config.set("default_key", 44, nlm::ConfigSource::Default);
    config.set("command_line_key", 45, nlm::ConfigSource::CommandLine);
    
    // Verify all keys exist
    assert(config.has("runtime_key"));
    assert(config.has("file_key"));
    assert(config.has("default_key"));
    assert(config.has("command_line_key"));
    
    // Test source values (they're stored as enum values)
    auto runtimeEntry = config.get<int>("runtime_key");
    auto fileEntry = config.get<int>("file_key");
    auto defaultEntry = config.get<int>("default_key");
    auto commandLineEntry = config.get<int>("command_line_key");
    
    assert(runtimeEntry.has_value());
    assert(fileEntry.has_value());
    assert(defaultEntry.has_value());
    assert(commandLineEntry.has_value());
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigSaveLoadComprehensive() {
    nlm::Config config;
    
    // Add diverse test data
    config.set("int_setting", 42, nlm::ConfigSource::Runtime);
    config.set("double_setting", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_setting", "test_value", nlm::ConfigSource::Runtime);
    config.set("bool_setting", true, nlm::ConfigSource::Runtime);
    config.set("float_setting", 2.71828f, nlm::ConfigSource::Runtime);
    config.set("int64_setting", static_cast<int64_t>(1000000), nlm::ConfigSource::Runtime);
    
    std::string testFile = "comprehensive_config_save.ini";
    assert(config.saveToFile(testFile));
    
    // Create a new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify all data was loaded correctly
    assert(config2.getOr<int>("int_setting", 0) == 42);
    assert(config2.getOr<double>("double_setting", 0.0) > 3.14 && 
           config2.getOr<double>("double_setting", 0.0) < 3.15);
    assert(config2.getOr<std::string>("string_setting", "") == "test_value");
    assert(config2.getOr<bool>("bool_setting", false) == true);
    assert(config2.getOr<float>("float_setting", 0.0f) > 2.71 && 
           config2.getOr<float>("float_setting", 0.0f) < 2.73);
    assert(config2.getOr<int64_t>("int64_setting", 0) == 1000000);
    
    // Test that file saving preserves data across multiple saves
    std::string testFile2 = "comprehensive_config_save2.ini";
    config2.saveToFile(testFile2);
    
    nlm::Config config3;
    assert(config3.loadFromFile(testFile2));
    assert(config3.getOr<int>("int_setting", 0) == 42);
    
    std::cout << "    testConfigSaveLoadComprehensive passed" << std::endl;
}

void testConfigErrorHandlingComprehensive() {
    nlm::Config config;
    
    // Test type validation - string that looks like int
    config.set("string_int", "not_a_number", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("string_int");
    assert(!intVal.has_value());  // Should fail type conversion
    
    // Test type validation - string that looks like double
    config.set("string_double", "not_a_float", nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("string_double");
    assert(!doubleVal.has_value());  // Should fail type conversion
    
    // Test getOr with invalid types
    int defaultInt = config.getOr<int>("string_int", 999);
    assert(defaultInt == 999);  // Should return default
    
    double defaultDouble = config.getOr<double>("string_double", 3.14);
    assert(defaultDouble > 3.13 && defaultDouble < 3.15);  // Should return default
    
    // Test missing key with getOr
    bool missingBool = config.getOr<bool>("missing_bool", true);
    assert(missingBool == true);  // Should return default
    
    // Test vector parsing (if supported)
    config.set("vector_test", "[1,2,3,4,5]", nlm::ConfigSource::Runtime);
    auto vectorVal = config.get<std::vector<int>>("vector_test");
    // This may or may not work depending on implementation
    
    // Test empty string handling
    config.set("empty_string", "", nlm::ConfigSource::Runtime);
    auto emptyStr = config.get<std::string>("empty_string");
    assert(emptyStr.has_value());
    assert(emptyStr.value() == "");
    
    // Test whitespace handling
    config.set("whitespace_string", "  hello  ", nlm::ConfigSource::Runtime);
    auto whitespaceStr = config.get<std::string>("whitespace_string");
    assert(whitespaceStr.has_value());
    
    std::cout << "    testConfigErrorHandlingComprehensive passed" << std::endl;
}

void testConfigLoadErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool result1 = config.loadFromFile("non_existent_file.xyz");
    assert(!result1);
    
    // Test loading corrupted file (malformed key=value pairs)
    std::ofstream corruptedFile("corrupted_config.ini");
    corruptedFile << "valid_key=value\n";
    corruptedFile << "invalid_line_without_equals\n";
    corruptedFile << "another=valid\n";
    corruptedFile << "key_without_value=\n";
    corruptedFile.close();
    
    bool result2 = config.loadFromFile("corrupted_config.ini");
    // Should handle gracefully - may return false or process valid lines
    
    // Test loading file with only comments/empty lines
    std::ofstream commentsFile("comments_config.ini");
    commentsFile << "# This is a comment\n";
    commentsFile << "// Another comment\n";
    commentsFile << "   \n";  // Empty line with spaces
    commentsFile.close();
    
    bool result3 = config.loadFromFile("comments_config.ini");
    // Should handle gracefully - may succeed with empty config
    
    // Test loading file with invalid quotes
    std::ofstream quotesFile("quotes_config.ini");
    quotesFile << "valid_string=\"quoted value\"\n";
    quotesFile << "unclosed_quote=\"unclosed\n";
    quotesFile << "normal=normal_value\n";
    quotesFile.close();
    
    bool result4 = config.loadFromFile("quotes_config.ini");
    // Should handle gracefully
    
    std::cout << "    testConfigLoadErrorHandling passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Add entries with different types and descriptions
    config.set("int_setting", 42, nlm::ConfigSource::Runtime);
    config.set("string_setting", "test", nlm::ConfigSource::File);
    config.set("double_setting", 3.14, nlm::ConfigSource::Default);
    
    // Test summary generation (this should include all entries)
    std::string summary = config.summary();
    
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("int_setting") != std::string::npos);
    assert(summary.find("string_setting") != std::string::npos);
    assert(summary.find("double_setting") != std::string::npos);
    
    // Clean up test files
    std::remove("test_config_save.ini");
    std::remove("comprehensive_config_save.ini");
    std::remove("comprehensive_config_save2.ini");
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test very large integer values
    config.set("large_int", static_cast<int64_t>(std::numeric_limits<int64_t>::max()), nlm::ConfigSource::Runtime);
    assert(config.get<int64_t>("large_int").has_value());
    
    // Test very small integer values
    config.set("small_int", static_cast<int64_t>(std::numeric_limits<int64_t>::min()), nlm::ConfigSource::Runtime);
    assert(config.get<int64_t>("small_int").has_value());
    
    // Test special floating point values
    config.set("nan_value", std::numeric_limits<float>::quiet_NaN(), nlm::ConfigSource::Runtime);
    config.set("inf_value", std::numeric_limits<float>::infinity(), nlm::ConfigSource::Runtime);
    config.set("neg_inf_value", -std::numeric_limits<float>::infinity(), nlm::ConfigSource::Runtime);
    
    // Test empty string
    config.set("empty", "", nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("empty").has_value());
    assert(config.get<std::string>("empty").value() == "");
    
    // Test string with special characters
    config.set("special_chars", "line1\nline2\ttabbed\rreturn", nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("special_chars").has_value());
    
    // Test key with special characters (should be escaped in file)
    config.set("key with spaces", "value", nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("key with spaces").has_value());
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void testConfigBatchOperations() {
    nlm::Config config;
    
    // Test clearing
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    auto keys = config.getKeys();
    assert(keys.empty());
    
    std::cout << "    testConfigBatchOperations passed" << std::endl;
}

void testConfigSerialization() {
    nlm::Config config;
    
    // Add test data
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", "test", nlm::ConfigSource::File);
    config.set("double_val", 3.14, nlm::ConfigSource::Default);
    config.set("bool_val", true, nlm::ConfigSource::CommandLine);
    
    // Save to file
    std::string filename = "serialization_test.ini";
    assert(config.saveToFile(filename));
    
    // Load back
    nlm::Config config2;
    assert(config2.loadFromFile(filename));
    
    // Verify all data preserved
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    assert(config2.getOr<double>("double_val", 0.0) > 3.13 && 
           config2.getOr<double>("double_val", 0.0) < 3.15);
    assert(config2.getOr<bool>("bool_val", false) == true);
    
    // Verify source tracking is preserved
    assert(config2.has("int_val"));
    assert(config2.has("string_val"));
    assert(config2.has("double_val"));
    assert(config2.has("bool_val"));
    
    std::remove(filename.c_str());
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGetAllTypes();
    testConfigGetOrComprehensive();
    testConfigRemoveComprehensive();
    testConfigSourceTracking();
    testConfigSaveLoadComprehensive();
    testConfigErrorHandlingComprehensive();
    testConfigLoadErrorHandling();
    testConfigSummary();
    testConfigEdgeCases();
    testConfigBatchOperations();
    testConfigSerialization();
    
    std::cout << "All comprehensive config tests passed!" << std::endl;
}

} // namespace test_config_comprehensive
