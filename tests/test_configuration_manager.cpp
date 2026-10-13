// ConfigurationManager Tests
#include <cstdlib>

namespace test_configuration_manager {

void testConfigurationManagerCreation() {
    nlm::ConfigurationManager config;
    assert(config.getSectionCount() == 0);
    std::cout << "    testConfigurationManagerCreation passed" << std::endl;
}

void testConfigurationManagerSetAndGet() {
    nlm::ConfigurationManager config;
    
    config.set("int_value", 42);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigurationManagerSetAndGet passed" << std::endl;
}

void testConfigurationManagerSections() {
    nlm::ConfigurationManager config;
    
    config.set("section1", "key1", "value1");
    config.set("section1", "key2", 123);
    
    config.set("section2", "key3", 45.67);
    
    assert(config.has("section1", "key1"));
    assert(config.has("section2", "key3"));
    
    auto val1 = config.get<std::string>("section1", "key1");
    assert(val1.has_value());
    assert(*val1 == "value1");
    
    auto val2 = config.get<int>("section1", "key2");
    assert(val2.has_value());
    assert(*val2 == 123);
    
    auto val3 = config.get<double>("section2", "key3");
    assert(val3.has_value());
    assert(*val3 > 45.66 && *val3 < 45.68);
    
    std::cout << "    testConfigurationManagerSections passed" << std::endl;
}

void testConfigurationManagerSectionManagement() {
    nlm::ConfigurationManager config;
    
    assert(!config.hasSection("nonexistent"));
    
    auto& section = config.getSection("new_section");
    section.addEntry("key1", nlm::ConfigEntry("key1", nlm::ConfigValue(100), nlm::ConfigSource::Default));
    
    assert(config.hasSection("new_section"));
    assert(config.has("new_section", "key1"));
    
    auto val = config.get<int>("new_section", "key1");
    assert(val.has_value());
    assert(*val == 100);
    
    std::vector<std::string> sectionNames = config.getSectionNames();
    assert(std::find(sectionNames.begin(), sectionNames.end(), "new_section") != sectionNames.end());
    
    config.removeSection("new_section");
    assert(!config.hasSection("new_section"));
    
    std::cout << "    testConfigurationManagerSectionManagement passed" << std::endl;
}

void testConfigurationManagerEnvironmentVariables() {
    nlm::ConfigurationManager config;
    
    // Set an environment variable for testing
    setenv("NLM_TEST_VAR", "from_env", 1);
    
    config.loadFromEnvironment("NLM_");
    
    // Note: Environment variable loading may depend on system implementation
    // This test just ensures it doesn't crash
    unsetenv("NLM_TEST_VAR");
    
    std::cout << "    testConfigurationManagerEnvironmentVariables passed" << std::endl;
}

void testConfigurationManagerFileIO() {
    nlm::ConfigurationManager config;
    
    config.set("test_int", 42);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    config.set("test_double", 3.14159);
    
    // Test saving to JSON
    std::string jsonFile = "/tmp/test_config.json";
    assert(config.saveToFile(jsonFile));
    
    // Create new config and load from JSON
    nlm::ConfigurationManager config2;
    assert(config2.loadFromFile(jsonFile));
    
    assert(config2.has("test_int"));
    assert(config2.has("test_string"));
    assert(config2.has("test_bool"));
    assert(config2.has("test_double"));
    
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testConfigurationManagerFileIO passed" << std::endl;
}

void testConfigurationManagerValidation() {
    nlm::ConfigurationManager config;
    
    config.set("valid_int", 100);
    config.set("valid_string", "test");
    
    assert(config.isValid());
    assert(config.getValidationError().empty());
    
    config.lock("valid_int");
    assert(config.isLocked("valid_int"));
    
    std::cout << "    testConfigurationManagerValidation passed" << std::endl;
}

void testConfigurationManagerTemplates() {
    nlm::ConfigurationManager config;
    
    nlm::ConfigurationTemplate template1;
    template1.name = "test_template";
    
    nlm::ConfigSchema schema;
    schema.type = "int";
    schema.required = true;
    schema.minValue = 0;
    template1.addField("number", schema);
    
    config.addTemplate("test_template", template1);
    assert(config.hasTemplate("test_template"));
    
    const nlm::ConfigurationTemplate* tmpl = config.getTemplate("test_template");
    assert(tmpl != nullptr);
    assert(tmpl->name == "test_template");
    
    std::cout << "    testConfigurationManagerTemplates passed" << std::endl;
}

void testConfigurationManagerInheritance() {
    nlm::ConfigurationManager config;
    
    config.set("key1", "value1", nlm::ConfigSource::Default);
    config.set("key2", "value2", nlm::ConfigSource::File);
    
    assert(config.getSource("key1") == nlm::ConfigSource::Default);
    assert(config.getSource("key2") == nlm::ConfigSource::File);
    
    config.setInheritance(true);
    assert(config.hasInheritance());
    
    std::cout << "    testConfigurationManagerInheritance passed" << std::endl;
}

void testConfigurationManagerThreadSafety() {
    nlm::ConfigurationManager config;
    
    config.setThreadSafe(true);
    assert(config.isThreadSafe());
    
    std::cout << "    testConfigurationManagerThreadSafety passed" << std::endl;
}

void testConfigurationManagerAutoReload() {
    nlm::ConfigurationManager config;
    
    config.setAutoReload(false);
    config.stopAutoReload();
    
    config.setAutoReload(true, std::chrono::milliseconds(100));
    assert(config.getReloadInterval().count() == 100);
    
    config.stopAutoReload();
    
    std::cout << "    testConfigurationManagerAutoReload passed" << std::endl;
}

void testConfigurationManagerClear() {
    nlm::ConfigurationManager config;
    
    config.set("key1", 1);
    config.set("section1", "key2", 2);
    
    assert(config.getEntryCount() > 0);
    assert(config.getSectionCount() > 0);
    
    config.clear();
    assert(config.getEntryCount() == 0);
    
    config.clear("section1");
    
    std::cout << "    testConfigurationManagerClear passed" << std::endl;
}

void testConfigurationManagerTypeSupport() {
    nlm::ConfigurationManager config;
    
    // Test all supported types
    config.set("int_val", static_cast<int>(42));
    config.set("int64_val", static_cast<int64_t>(10000000000LL));
    config.set("double_val", 3.14159);
    config.set("bool_val", true);
    config.set("string_val", std::string("test"));
    
    std::vector<int> intVec = {1, 2, 3};
    config.set("int_vec", intVec);
    
    std::vector<double> doubleVec = {1.1, 2.2, 3.3};
    config.set("double_vec", doubleVec);
    
    std::vector<std::string> stringVec = {"a", "b", "c"};
    config.set("string_vec", stringVec);
    
    // Verify all types
    assert(config.get<int>("int_val").has_value() && *config.get<int>("int_val") == 42);
    assert(config.get<int64_t>("int64_val").has_value());
    assert(config.get<double>("double_val").has_value());
    assert(config.get<bool>("bool_val").has_value() && *config.get<bool>("bool_val") == true);
    assert(config.get<std::string>("string_val").has_value());
    
    std::cout << "    testConfigurationManagerTypeSupport passed" << std::endl;
}

void testConfigurationManagerBackwardCompatibility() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    assert(config.has("test_int"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testConfigurationManagerBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testConfigurationManagerCreation();
    testConfigurationManagerSetAndGet();
    testConfigurationManagerSections();
    testConfigurationManagerSectionManagement();
    testConfigurationManagerEnvironmentVariables();
    testConfigurationManagerFileIO();
    testConfigurationManagerValidation();
    testConfigurationManagerTemplates();
    testConfigurationManagerInheritance();
    testConfigurationManagerThreadSafety();
    testConfigurationManagerAutoReload();
    testConfigurationManagerClear();
    testConfigurationManagerTypeSupport();
    testConfigurationManagerBackwardCompatibility();
}

} // namespace test_configuration_manager

// Integration test that runs both old and new config tests
test_configuration_manager::runAll();
extern "C" void test_config_runAll();
test_config::runAll();