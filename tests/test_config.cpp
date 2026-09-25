// Config Tests
#include "core/Config/Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_section.hpp>

namespace test_config {

TEST_CASE("Config creation and initial state", "[config][creation]") {
    nlm::Config config;
    REQUIRE(!config.has("test"));
}

TEST_CASE("Config integer operations", "[config][integer]") {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    REQUIRE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    REQUIRE(intVal.has_value());
    REQUIRE(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    REQUIRE(doubleVal.has_value());
    REQUIRE(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    REQUIRE(stringVal.has_value());
    REQUIRE(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    REQUIRE(boolVal.has_value());
    REQUIRE(*boolVal == true);
}

TEST_CASE("Config getOr method", "[config][getor]") {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    REQUIRE(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    REQUIRE(val == 50);
}

TEST_CASE("Config clear and reset operations", "[config][clear]") {
    nlm::Config config;
    
    config.set("test_value", 123, nlm::ConfigSource::Runtime);
    REQUIRE(config.has("test_value"));
    
    config.clear();
    REQUIRE(!config.has("test_value"));
}

TEST_CASE("Config source tracking", "[config][source]") {
    nlm::Config config;
    
    config.set("runtime_value", 1, nlm::ConfigSource::Runtime);
    config.set("default_value", 2, nlm::ConfigSource::Default);
    config.set("file_value", 3, nlm::ConfigSource::File);
    
    auto runtimeVal = config.get<int>("runtime_value");
    auto defaultVal = config.get<int>("default_value");
    auto fileVal = config.get<int>("file_value");
    
    REQUIRE(runtimeVal.has_value() && *runtimeVal == 1);
    REQUIRE(defaultVal.has_value() && *defaultVal == 2);
    REQUIRE(fileVal.has_value() && *fileVal == 3);
}

TEST_CASE("Config multiple values management", "[config][multiple]") {
    nlm::Config config;
    
    // Set multiple different types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("float_val", 3.14f, nlm::ConfigSource::Runtime);
    config.set("str_val", "test", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Verify all are present
    REQUIRE(config.has("int_val"));
    REQUIRE(config.has("float_val"));
    REQUIRE(config.has("str_val"));
    REQUIRE(config.has("bool_val"));
    
    // Verify values
    REQUIRE(config.get<int>("int_val").value() == 42);
    REQUIRE(config.get<float>("float_val").value() == 3.14f);
    REQUIRE(config.get<std::string>("str_val").value() == "test");
    REQUIRE(config.get<bool>("bool_val").value() == true);
}

} // namespace test_config