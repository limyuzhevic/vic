#pragma once

#include "TestSuite.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <filesystem>

namespace nlm {

class ConfigTest : public TestBase {
public:
    std::string getName() const override {
        return "Config Tests";
    }
    
    void run() override {
        // Test 1: Create and set config values
        auto config = std::make_shared<Config>();
        
        config->set("test_int", 42);
        config->set("test_double", 3.14);
        config->set("test_bool", true);
        config->set("test_string", "hello");
        
        // Verify values
        assert(config->get<int>("test_int") == 42);
        assert(config->get<double>("test_double") == 3.14);
        assert(config->get<bool>("test_bool") == true);
        assert(config->get<std::string>("test_string") == "hello");
        
        // Test 2: Get or default values
        assert(config->getOr<int>("missing_int", 99) == 99);
        assert(config->getOr<std::string>("missing_string", "default") == "default");
        
        // Test 3: Test has/has not
        assert(config->has("test_int") == true);
        assert(config->has("missing_key") == false);
        
        // Test 4: Test save and load
        std::string testFile = "test_config.cfg";
        
        // Save config
        bool saveSuccess = config->saveToFile(testFile);
        assert(saveSuccess);
        
        // Create new config and load
        auto config2 = std::make_shared<Config>();
        bool loadSuccess = config2->loadFromFile(testFile);
        assert(loadSuccess);
        
        // Verify loaded values
        assert(config2->get<int>("test_int") == 42);
        assert(config2->get<double>("test_double") == 3.14);
        assert(config2->get<bool>("test_bool") == true);
        assert(config2->get<std::string>("test_string") == "hello");
        
        // Test 5: Test command line arguments
        int argc = 5;
        char* argv[] = {
            (char*)"test",
            (char*)"--test_arg1",
            (char*)"value1",
            (char*)"-test_arg2",
            (char*)"value2"
        };
        
        config2->loadFromArgs(argc, argv);
        
        // Test 6: Test vector types
        std::vector<int> intVector = {1, 2, 3, 4, 5};
        std::vector<double> doubleVector = {1.5, 2.5, 3.5};
        std::vector<std::string> stringVector = {"a", "b", "c"};
        
        config2->set("int_vector", intVector);
        config2->set("double_vector", doubleVector);
        config2->set("string_vector", stringVector);
        
        auto loadedIntVector = config2->get<std::vector<int>>("int_vector");
        assert(loadedIntVector == intVector);
        
        auto loadedDoubleVector = config2->get<std::vector<double>>("double_vector");
        assert(loadedDoubleVector == doubleVector);
        
        auto loadedStringVector = config2->get<std::vector<std::string>>("string_vector");
        assert(loadedStringVector == stringVector);
        
        // Clean up
        std::remove(testFile.c_str());
        
        NLM_LOG_INFO("All Config tests passed");
    }
};

} // namespace nlm