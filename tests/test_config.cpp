// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigMoveSemantics() {
    nlm::Config config1;
    config1.set("test", 123, nlm::ConfigSource::Runtime);
    
    // Move constructor
    nlm::Config config2(std::move(config1));
    assert(config2.has("test"));
    
    // Move should leave source in valid state
    std::cout << "    testConfigMoveSemantics passed" << std::endl;
}

void testConfigMemoryCleanup() {
    {
        nlm::Config config;
        config.set("temp", "value", nlm::ConfigSource::Runtime);
        config.set("count", 42, nlm::ConfigSource::Runtime);
        
        // Verify config has values
        assert(config.has("temp"));
        assert(config.has("count"));
        
        auto tempVal = config.get<std::string>("temp");
        assert(tempVal.has_value());
        assert(*tempVal == "value");
    } // Config destroyed here
    
    // Config should be properly destroyed now
    std::cout << "    testConfigMemoryCleanup passed" << std::endl;
}

void testConfigArrayMemory() {
    const size_t configCount = 20;
    {
        // Create multiple configs in a loop
        for (size_t i = 0; i < configCount; ++i) {
            nlm::Config config;
            config.set("iteration", static_cast<int64_t>(i), nlm::ConfigSource::Runtime);
            config.set("data", std::string("test_" + std::to_string(i)), nlm::ConfigSource::Runtime);
            
            // Use the config
            auto iterVal = config.get<int64_t>("iteration");
            assert(iterVal.has_value());
            assert(*iterVal == static_cast<int64_t>(i));
        }
    }
    
#include <chrono>

void testConfigPerformance() {
    const size_t iterationCount = 10000;
    
    // Test config creation and destruction performance
    auto startTime = std::chrono::high_resolution_clock::now();
    {
        std::vector<nlm::Config> configs;
        configs.reserve(iterationCount);
        
        for (size_t i = 0; i < iterationCount; ++i) {
            nlm::Config config;
            config.set("iteration", static_cast<int64_t>(i), nlm::ConfigSource::Runtime);
            config.set("data", std::string("test_" + std::to_string(i)), nlm::ConfigSource::Runtime);
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "    Created/destroyed " << iterationCount << " configs in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Test config set/get performance
    nlm::Config config;
    startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterationCount; ++i) {
        config.set("key", static_cast<int64_t>(i), nlm::ConfigSource::Runtime);
        auto val = config.get<int64_t>("key");
        (void)val;
    }
    endTime = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "    Performed " << iterationCount << " set/get operations in " 
              << duration.count() << " microseconds (" 
              << (1000000.0 / duration.count() * iterationCount) << " operations/second)" << std::endl;
    
    std::cout << "    testConfigPerformance passed" << std::endl;
}

} // namespace test_config
