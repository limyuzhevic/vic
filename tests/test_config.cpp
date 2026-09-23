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

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test empty string value
    config.set("empty_string", std::string(""), nlm::ConfigSource::Runtime);
    assert(config.has("empty_string"));
    
    auto emptyVal = config.get<std::string>("empty_string");
    assert(emptyVal.has_value());
    assert(*emptyVal == "");
    
    // Test very large integer
    config.set("large_int", 1000000, nlm::ConfigSource::Runtime);
    auto largeInt = config.get<int>("large_int");
    assert(largeInt.has_value());
    assert(*largeInt == 1000000);
    
    // Test negative integer
    config.set("negative_int", -1000, nlm::ConfigSource::Runtime);
    auto negInt = config.get<int>("negative_int");
    assert(negInt.has_value());
    assert(*negInt == -1000);
    
    // Test zero value
    config.set("zero", 0, nlm::ConfigSource::Runtime);
    auto zero = config.get<int>("zero");
    assert(zero.has_value());
    assert(*zero == 0);
    
    // Test negative double
    config.set("negative_double", -3.14, nlm::ConfigSource::Runtime);
    auto negDouble = config.get<double>("negative_double");
    assert(negDouble.has_value());
    assert(*negDouble == -3.14);
    
    // Test very small double
    config.set("small_double", 1e-6, nlm::ConfigSource::Runtime);
    auto smallDouble = config.get<double>("small_double");
    assert(smallDouble.has_value());
    assert(*smallDouble == 1e-6);
    
    // Test empty key rejection
    // Note: Currently set() accepts empty keys, but we can add validation
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigEdgeCases();
}

} // namespace test_config
