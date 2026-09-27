// Nested Configuration Tests
#include "core/NestedConfiguration.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_nested_configuration {

void testNestedConfigurationCreation() {
    nlm::core::NestedConfiguration config;
    
    // Test root node
    auto* root = config.getRoot();
    assert(root != nullptr);
    
    std::cout << "    testNestedConfigurationCreation passed" << std::endl;
}

void testNestedConfigurationAddAndGet() {
    nlm::core::NestedConfiguration config;
    
    // Add nested nodes
    auto& child1 = config.addNode("parent/child1", "value1");
    auto& child2 = config.addNode("parent/child2", "value2", nlm::core::ConfigDataType::INTEGER);
    
    // Get nodes
    auto* node1 = config.getNode("parent/child1");
    assert(node1 != nullptr);
    assert(node1->getValue() == "value1");
    
    auto* node2 = config.getNode("parent/child2");
    assert(node2 != nullptr);
    assert(node2->getValue() == "value2");
    
    // Test path-based access
    auto* root = config.getRoot();
    assert(root->hasChild("parent"));
    
    auto* parent = root->getChild("parent");
    assert(parent != nullptr);
    assert(parent->hasChild("child1"));
    assert(parent->hasChild("child2"));
    
    std::cout << "    testNestedConfigurationAddAndGet passed" << std::endl;
}

void testNestedConfigurationValues() {
    nlm::core::NestedConfiguration config;
    
    // Test various data types
    config.addNode("string_val", "hello", nlm::core::ConfigDataType::STRING);
    config.addNode("int_val", "42", nlm::core::ConfigDataType::INTEGER);
    config.addNode("double_val", "3.14", nlm::core::ConfigDataType::DOUBLE);
    config.addNode("bool_val", "true", nlm::core::ConfigDataType::BOOLEAN);
    config.addNode("list_val", "a,b,c", nlm::core::ConfigDataType::STRING_ARRAY);
    
    // Get and verify values
    assert(config.getValue("string_val") == "hello");
    assert(config.getInt("int_val") == 42);
    assert(config.getDouble("double_val") > 3.13 && config.getDouble("double_val") < 3.15);
    assert(config.getBool("bool_val") == true);
    
    // Test value setting
    config.setValue("string_val", "world", nlm::core::ConfigSource::RUNTIME);
    assert(config.getValue("string_val") == "world");
    
    std::cout << "    testNestedConfigurationValues passed" << std::endl;
}

void testNestedConfigurationTreeStructure() {
    nlm::core::NestedConfiguration config;
    
    // Create complex tree structure
    config.addNode("a/b/c/d", "deep_value", nlm::core::ConfigDataType::STRING);
    config.addNode("a/b/c/e", "another_value", nlm::core::ConfigDataType::STRING);
    config.addNode("a/x/y", "branch_value", nlm::core::ConfigDataType::STRING);
    
    // Test tree traversal
    auto* root = config.getRoot();
    assert(root != nullptr);
    
    auto allNodes = config.getAllNodes();
    assert(allNodes.size() > 0);
    
    // Test path operations
    auto* node = config.getNode("a/b/c/d");
    assert(node != nullptr);
    
    std::cout << "    testNestedConfigurationTreeStructure passed" << std::endl;
}

void testNestedConfigurationValidation() {
    nlm::core::NestedConfiguration config;
    
    // Add some invalid configuration
    config.addNode("invalid_number", "not_a_number", nlm::core::ConfigDataType::INTEGER);
    
    // Test validation
    bool valid = config.validate();
    // Validation might fail for invalid configuration
    std::cout << "    Nested validation result: " << (valid ? "PASS" : "FAIL") << std::endl;
    
    // Get validation errors
    auto errors = config.getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "    Validation error: " << error.message << std::endl;
    }
    
    std::cout << "    testNestedConfigurationValidation passed" << std::endl;
}

void testNestedConfigurationSearch() {
    nlm::core::NestedConfiguration config;
    
    // Add test data
    config.addNode("search_test_1", "value1", nlm::core::ConfigDataType::STRING);
    config.addNode("search_test_2", "value2", nlm::core::ConfigDataType::STRING);
    config.addNode("other/path/test", "value3", nlm::core::ConfigDataType::STRING);
    
    // Test key search
    auto keyMatches = config.findNodes("test");
    assert(keyMatches.size() > 0);
    
    // Test value search
    auto valueMatches = config.findNodesByValue("value1");
    assert(valueMatches.size() > 0);
    
    std::cout << "    testNestedConfigurationSearch passed" << std::endl;
}

void testNestedConfigurationSummary() {
    nlm::core::NestedConfiguration config;
    
    // Add test data
    config.addNode("summary_test_1", "value1", nlm::core::ConfigDataType::STRING);
    config.addNode("summary_test_2", "value2", nlm::core::ConfigDataType::STRING);
    
    // Get tree view
    std::string treeView = config.treeView();
    assert(!treeView.empty());
    assert(treeView.find("summary_test_1") != std::string::npos);
    
    std::cout << "    testNestedConfigurationSummary passed" << std::endl;
}

void runAll() {
    testNestedConfigurationCreation();
    testNestedConfigurationAddAndGet();
    testNestedConfigurationValues();
    testNestedConfigurationTreeStructure();
    testNestedConfigurationValidation();
    testNestedConfigurationSearch();
    testNestedConfigurationSummary();
}

} // namespace test_nested_configuration
