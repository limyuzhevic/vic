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

// Test new convenience methods for float, int, size_t, etc.
void testConfigConvenienceMethods() {
    nlm::Config config;
    
    // Test getFloat
    config.set("float_val", 3.14, nlm::ConfigSource::Runtime);
    float floatVal = 0.0f;
    assert(config.getFloat("float_val", floatVal));
    assert(floatVal > 3.13f && floatVal < 3.15f);
    
    // Test getInt
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    int intVal = 0;
    assert(config.getInt("int_val", intVal));
    assert(intVal == 42);
    
    // Test getDouble
    config.set("double_val", 2.718, nlm::ConfigSource::Runtime);
    double doubleVal = 0.0;
    assert(config.getDouble("double_val", doubleVal));
    assert(doubleVal > 2.717 && doubleVal < 2.719);
    
    // Test getBool
    config.set("bool_val", false, nlm::ConfigSource::Runtime);
    bool boolVal = true;
    assert(config.getBool("bool_val", boolVal));
    assert(boolVal == false);
    
    // Test getString
    config.set("str_val", std::string("test"), nlm::ConfigSource::Runtime);
    std::string stringVal;
    assert(config.getString("str_val", stringVal));
    assert(stringVal == "test");
    
    // Test getSizeT
    config.set("size_val", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    size_t sizeVal = 0;
    assert(config.getSizeT("size_val", sizeVal));
    assert(sizeVal == 1000);
    
    // Test getOr convenience methods
    float floatOr = config.getFloatOr("nonexistent", 1.0f);
    assert(floatOr == 1.0f);
    
    int intOr = config.getIntOr("nonexistent", 999);
    assert(intOr == 999);
    
    double doubleOr = config.getDoubleOr("nonexistent", 2.718);
    assert(doubleOr == 2.718);
    
    bool boolOr = config.getBoolOr("nonexistent", false);
    assert(boolOr == false);
    
    std::string stringOr = config.getStringOr("nonexistent", "default");
    assert(stringOr == "default");
    
    size_t sizeOr = config.getSizeTOr("nonexistent", 500);
    assert(sizeOr == 500);
    
    // Test getOpt methods
    auto floatOpt = config.getFloatOpt("float_val");
    assert(floatOpt.has_value());
    assert(*floatOpt > 3.13f && *floatOpt < 3.15f);
    
    auto intOpt = config.getIntOpt("int_val");
    assert(intOpt.has_value());
    assert(*intOpt == 42);
    
    auto doubleOpt = config.getDoubleOpt("double_val");
    assert(doubleOpt.has_value());
    
    auto boolOpt = config.getBoolOpt("bool_val");
    assert(boolOpt.has_value());
    
    auto stringOpt = config.getStringOpt("str_val");
    assert(stringOpt.has_value());
    
    auto sizeOpt = config.getSizeTOpt("size_val");
    assert(sizeOpt.has_value());
    assert(*sizeOpt == 1000);
    
    // Test non-existent keys return nullopt
    auto missingOpt = config.getFloatOpt("missing_key");
    assert(!missingOpt.has_value());
    
    std::cout << "    testConfigConvenienceMethods passed" << std::endl;
}

void testConfigDiagnosticInfo() {
    nlm::Config config;
    
    config.set("test1", 1, nlm::ConfigSource::Runtime);
    config.set("test2", 2.0, nlm::ConfigSource::File);
    config.set("test3", "hello", nlm::ConfigSource::CommandLine);
    
    std::string diagnostics = config.getDiagnosticInfo();
    
    // Check that diagnostic info contains expected content
    assert(diagnostics.find("Configuration Diagnostics:") != std::string::npos);
    assert(diagnostics.find("Total entries: 3") != std::string::npos);
    assert(diagnostics.find("Source Count") != std::string::npos);
    assert(diagnostics.find("test1") != std::string::npos);
    assert(diagnostics.find("test2") != std::string::npos);
    assert(diagnostics.find("test3") != std::string::npos);
    
    std::cout << "    testConfigDiagnosticInfo passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigConvenienceMethods();
    testConfigDiagnosticInfo();
}

} // namespace test_config
