// Simple Config Tests
#include "core/Config/Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_stringstream.hpp>

namespace test_config {

TEST_CASE("Config creation and initial state", "[config_simple][creation]") {
    nlm::Config config;
    REQUIRE(!config.has("test"));
}

TEST_CASE("Config set and get operations", "[config_simple][set_get]") {
    nlm::Config config;
    
    config.set("int_value", 42);
    REQUIRE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    REQUIRE(intVal.has_value());
    REQUIRE(*intVal == 42);
    
    config.set("double_value", 3.14);
    auto doubleVal = config.get<double>("double_value");
    REQUIRE(doubleVal.has_value());
    REQUIRE(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"));
    auto stringVal = config.get<std::string>("string_value");
    REQUIRE(stringVal.has_value());
    REQUIRE(*stringVal == "hello");
    
    config.set("bool_value", true);
    auto boolVal = config.get<bool>("bool_value");
    REQUIRE(boolVal.has_value());
    REQUIRE(*boolVal == true);
}

TEST_CASE("Config getOr method", "[config_simple][getor]") {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    REQUIRE(val == 100);
    
    config.set("existing", 50);
    val = config.getOr<int>("existing", 100);
    REQUIRE(val == 50);
}

TEST_CASE("Config remove operation", "[config_simple][remove]") {
    nlm::Config config;
    
    config.set("test", 123);
    REQUIRE(config.has("test"));
    
    config.remove("test");
    REQUIRE(!config.has("test"));
}

TEST_CASE("Config keys operation", "[config_simple][keys]") {
    nlm::Config config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getKeys();
    REQUIRE(keys.size() == 3);
}

TEST_CASE("Config preset loading", "[config_simple][presets]") {
    nlm::Config config;
    
    // Test loading beginner preset
    REQUIRE(nlm::loadFromPreset(config, "beginner"));
    REQUIRE(config.has("neural.learning_rate"));
    REQUIRE(config.has("neural.hidden_size"));
    REQUIRE(config.getOr<double>("neural.hidden_size", 0) == 128.0);
    
    // Test loading expert preset
    config.clear();
    REQUIRE(nlm::loadFromPreset(config, "expert"));
    REQUIRE(config.has("neural.attention_heads"));
    REQUIRE(config.has("plasticity.stdp_enabled"));
    REQUIRE(config.getOr<bool>("plasticity.stdp_enabled", false) == true);
    
    // Test loading balanced preset
    config.clear();
    REQUIRE(nlm::loadFromPreset(config, "balanced"));
    REQUIRE(config.getOr<double>("neural.hidden_size", 0) == 256.0);
    REQUIRE(config.getOr<double>("performance.timeout", 0) == 120.0);
}

TEST_CASE("Config preset availability", "[config_simple][presets][availability]") {
    nlm::Config config;
    
    auto presets = nlm::Config::getPresets();
    REQUIRE(!presets.empty());
}

TEST_CASE("Config non-existent preset loading", "[config_simple][presets][error]") {
    nlm::Config config;
    
    REQUIRE(!nlm::loadFromPreset(config, "nonexistent"));
}

TEST_CASE("Config multiple values scenario", "[config_simple][multiple]") {
    nlm::Config config;
    
    config.set("int_val", 42);
    config.set("float_val", 3.14f);
    config.set("str_val", "test");
    config.set("bool_val", true);
    
    REQUIRE(config.has("int_val"));
    REQUIRE(config.has("float_val"));
    REQUIRE(config.has("str_val"));
    REQUIRE(config.has("bool_val"));
    
    REQUIRE(config.get<int>("int_val").value() == 42);
    REQUIRE(config.get<float>("float_val").value() == 3.14f);
    REQUIRE(config.get<std::string>("str_val").value() == "test");
    REQUIRE(config.get<bool>("bool_val").value() == true);
}

} // namespace test_config
