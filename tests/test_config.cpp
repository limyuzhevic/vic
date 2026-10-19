// ============================================================================
// NEW FEATURE TESTS
// ============================================================================

// Batch Operations Tests
void testBatchOperations_BatchSet() {
    nlm::Config config;
    
    // Test batchSet with various types
    std::vector<std::pair<std::string, nlm::ConfigValue>> values = {
        {"batch_int", 42},
        {"batch_double", 3.14},
        {"batch_string", std::string("batch_value")},
        {"batch_bool", true},
        {"batch_vector", std::vector<int>({1, 2, 3})}
    };
    
    config.batchSet(values, nlm::ConfigSource::Runtime);
    
    // Verify all values were set correctly
    assert(config.has("batch_int"));
    assert(config.get<int>("batch_int").value() == 42);
    
    assert(config.has("batch_double"));
    assert(config.get<double>("batch_double").value() > 3.13 && config.get<double>("batch_double").value() < 3.15);
    
    assert(config.has("batch_string"));
    assert(config.get<std::string>("batch_string").value() == "batch_value");
    
    assert(config.has("batch_bool"));
    assert(config.get<bool>("batch_bool").value() == true);
    
    assert(config.has("batch_vector"));
    auto vec = config.get<std::vector<int>>("batch_vector");
    assert(vec.has_value());
    assert(vec.value().size() == 3);
    
    std::cout << "    testBatchOperations_BatchSet passed" << std::endl;
}

void testBatchOperations_BatchRemove() {
    nlm::Config config;
    
    // Set up initial values
    config.set("keep1", 1, nlm::ConfigSource::Runtime);
    config.set("keep2", 2, nlm::ConfigSource::Runtime);
    config.set("remove1", 3, nlm::ConfigSource::Runtime);
    config.set("remove2", 4, nlm::ConfigSource::Runtime);
    
    // Batch remove
    std::vector<std::string> toRemove = {"remove1", "remove2"};
    config.batchRemove(toRemove);
    
    // Verify kept values still exist
    assert(config.has("keep1"));
    assert(config.has("keep2"));
    assert(config.get<int>("keep1").value() == 1);
    assert(config.get<int>("keep2").value() == 2);
    
    // Verify removed values are gone
    assert(!config.has("remove1"));
    assert(!config.has("remove2"));
    
    std::cout << "    testBatchOperations_BatchRemove passed" << std::endl;
}

void testBatchOperations_BatchGet() {
    nlm::Config config;
    
    // Set up values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("missing_key", 0, nlm::ConfigSource::Runtime); // Will be overwritten
    
    // Batch get with multiple keys
    std::vector<std::string> keys = {"int_key", "string_key", "double_key", "bool_key", "missing_key"};
    auto results = config.batchGet<int>(keys);
    
    assert(results.size() == 4); // Only 4 existing int-like values
    assert(results.find("int_key") != results.end());
    assert(results["int_key"] == 42);
    assert(results.find("double_key") != results.end());
    assert(results.find("string_key") == results.end()); // String can't be converted to int
    
    // Test with string type
    auto stringResults = config.batchGet<std::string>(keys);
    assert(stringResults.size() == 2); // int_key, double_key can't be converted to string easily
    assert(stringResults.find("string_key") != results.end());
    assert(stringResults["string_key"] == "hello");
    
    std::cout << "    testBatchOperations_BatchGet passed" << std::endl;
}

void testBatchOperations_BatchUpdate() {
    nlm::Config config;
    
    // Set up initial values
    config.set("existing1", 100, nlm::ConfigSource::Runtime);
    config.set("existing2", "original", nlm::ConfigSource::Runtime);
    config.set("non_existing", 999, nlm::ConfigSource::Runtime);
    
    // Test batch update with existing keys
    std::unordered_map<std::string, nlm::ConfigValue> updates = {
        {"existing1", 200},
        {"existing2", std::string("updated")}
    };
    
    config.batchUpdate(updates, nlm::ConfigSource::Runtime);
    
    // Verify updates
    assert(config.get<int>("existing1").value() == 200);
    assert(config.get<std::string>("existing2").value() == "updated");
    assert(config.get<int>("non_existing").value() == 999); // Should remain unchanged
    
    std::cout << "    testBatchOperations_BatchUpdate passed" << std::endl;
}

// Scripting Support Tests
void testScriptingSupport_ExecuteScript_Basic() {
    nlm::Config config;
    
    // Set up initial config
    config.set("initial_value", 10, nlm::ConfigSource::Runtime);
    
    // Execute script
    std::vector<std::string> scriptLines = {
        "set new_key 42",
        "get initial_value",
        "remove new_key",
        "clear"
    };
    
    std::string script;
    for (const auto& line : scriptLines) {
        script += line + " ";
    }
    
    std::vector<std::string> output = config.executeScript(script);
    
    // Verify output
    assert(output.size() >= 3);
    assert(output[0].find("Set new_key") != std::string::npos);
    assert(output[1].find("initial_value = 10") != std::string::npos);
    assert(output[2].find("Removed new_key") != std::string::npos);
    
    std::cout << "    testScriptingSupport_ExecuteScript_Basic passed" << std::endl;
}

void testScriptingSupport_ScriptVar() {
    nlm::Config config;
    
    // Set up variables
    config.set("username", std::string("alice"), nlm::ConfigSource::Runtime);
    config.set("count", 5, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Test variable substitution
    std::string script = "Welcome ${username}, you have ${count} active sessions (${enabled})";
    std::string result = config.scriptVar(script);
    
    assert(result.find("alice") != std::string::npos);
    assert(result.find("5") != std::string::npos);
    assert(result.find("true") != std::string::npos);
    
    std::cout << "    testScriptingSupport_ScriptVar passed" << std::endl;
}

void testScriptingSupport_ScriptEval() {
    nlm::Config config;
    
    // Set up variables for expression evaluation
    config.set("a", 10, nlm::ConfigSource::Runtime);
    config.set("b", 5, nlm::ConfigSource::Runtime);
    
    // Test arithmetic expressions
    assert(config.scriptEval("${a} + ${b}") == "15");
    assert(config.scriptEval("${a} - ${b}") == "5");
    assert(config.scriptEval("${a} * ${b}") == "50");
    assert(config.scriptEval("${a} / ${b}") == "2");
    
    std::cout << "    testScriptingSupport_ScriptEval passed" << std::endl;
}

// Advanced Filtering Tests
void testAdvancedFiltering_FilterBySource() {
    nlm::Config config;
    
    // Set values from different sources
    config.set("runtime_key", 1, nlm::ConfigSource::Runtime);
    config.set("file_key", 2, nlm::ConfigSource::File);
    config.set("command_line_key", 3, nlm::ConfigSource::CommandLine);
    
    // Test filter by source
    auto runtimeKeys = config.filterBySource(nlm::ConfigSource::Runtime);
    assert(runtimeKeys.size() == 1);
    assert(runtimeKeys[0] == "runtime_key");
    
    auto fileKeys = config.filterBySource(nlm::ConfigSource::File);
    assert(fileKeys.size() == 1);
    assert(fileKeys[0] == "file_key");
    
    auto commandLineKeys = config.filterBySource(nlm::ConfigSource::CommandLine);
    assert(commandLineKeys.size() == 1);
    assert(commandLineKeys[0] == "command_line_key");
    
    std::cout << "    testAdvancedFiltering_FilterBySource passed" << std::endl;
}

void testAdvancedFiltering_FilterByType() {
    nlm::Config config;
    
    // Set values of different types
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Test filter by type
    auto intKeys = config.filterByType<int>();
    assert(intKeys.size() == 2); // int_key and int_vector
    
    auto doubleKeys = config.filterByType<double>();
    assert(doubleKeys.size() == 1);
    assert(doubleKeys[0] == "double_key");
    
    auto stringKeys = config.filterByType<std::string>();
    assert(stringKeys.size() == 1);
    assert(stringKeys[0] == "string_key");
    
    auto boolKeys = config.filterByType<bool>();
    assert(boolKeys.size() == 1);
    assert(boolKeys[0] == "bool_key");
    
    auto vectorKeys = config.filterByType<std::vector<int>>();
    assert(vectorKeys.size() == 1);
    assert(vectorKeys[0] == "int_vector");
    
    std::cout << "    testAdvancedFiltering_FilterByType passed" << std::endl;
}

void testAdvancedFiltering_FindKeys() {
    nlm::Config config;
    
    // Set up keys with various patterns
    config.set("user.name", "alice", nlm::ConfigSource::Runtime);
    config.set("user.age", 30, nlm::ConfigSource::Runtime);
    config.set("admin.enabled", true, nlm::ConfigSource::Runtime);
    config.set("database.host", "localhost", nlm::ConfigSource::Runtime);
    config.set("database.port", 5432, nlm::ConfigSource::Runtime);
    
    // Test wildcard matching
    auto userKeys = config.findKeys("user.*");
    assert(userKeys.size() == 2);
    
    auto databaseKeys = config.findKeys("database.*");
    assert(databaseKeys.size() == 2);
    
    auto enabledKeys = config.findKeys("*enabled*");
    assert(enabledKeys.size() == 1);
    assert(enabledKeys[0] == "admin.enabled");
    
    std::cout << "    testAdvancedFiltering_FindKeys passed" << std::endl;
}

// Import/Export Tests
void testImportExport_ExportToJSON() {
    nlm::Config config;
    
    // Set up config
    config.set("name", std::string("test_config"), nlm::ConfigSource::Runtime);
    config.set("version", 1, nlm::ConfigSource::Runtime);
    config.set("active", true, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    std::string json = config.exportToJSON();
    
    // Basic JSON validation
    assert(json.find("{") != std::string::npos);
    assert(json.find("name") != std::string::npos);
    assert(json.find("test_config") != std::string::npos);
    assert(json.find("}") != std::string::npos);
    
    std::cout << "    testImportExport_ExportToJSON passed" << std::endl;
}

void testImportExport_ImportFromJSON() {
    nlm::Config config;
    
    // Create JSON string
    std::string json = "{\"key1\": 42, \"key2\": \"value\", \"key3\": true, \"key4\": 3.14}";
    
    // Import from JSON
    bool success = config.importFromJSON(json);
    assert(success);
    
    // Verify values
    assert(config.has("key1"));
    assert(config.get<int>("key1").value() == 42);
    
    assert(config.has("key2"));
    assert(config.get<std::string>("key2").value() == "value");
    
    assert(config.has("key3"));
    assert(config.get<bool>("key3").value() == true);
    
    assert(config.has("key4"));
    assert(config.get<double>("key4").value() > 3.13 && config.get<double>("key4").value() < 3.15);
    
    std::cout << "    testImportExport_ImportFromJSON passed" << std::endl;
}

void testImportExport_ExportToYAML() {
    nlm::Config config;
    
    // Set up config with description
    config.set("service.name", std::string("myapp"), nlm::ConfigSource::Runtime);
    config.set("service.port", 8080, nlm::ConfigSource::Runtime);
    
    // Export to YAML
    std::string yaml = config.exportToYAML();
    
    // Basic YAML validation
    assert(yaml.find("service.name") != std::string::npos);
    assert(yaml.find("myapp") != std::string::npos);
    assert(yaml.find("service.port") != std::string::npos);
    assert(yaml.find("8080") != std::string::npos);
    
    std::cout << "    testImportExport_ExportToYAML passed" << std::endl;
}

void testImportExport_ImportFromYAML() {
    nlm::Config config;
    
    // Create YAML string
    std::string yaml = 
        "key1: 100\n"
        "key2: \"quoted value\"\n"
        "key3: true\n"
        "key4: 2.718\n";
    
    // Import from YAML
    bool success = config.importFromYAML(yaml);
    assert(success);
    
    // Verify values
    assert(config.has("key1"));
    assert(config.get<int>("key1").value() == 100);
    
    assert(config.has("key2"));
    assert(config.get<std::string>("key2").value() == "quoted value");
    
    assert(config.has("key3"));
    assert(config.get<bool>("key3").value() == true);
    
    assert(config.has("key4"));
    assert(config.get<double>("key4").value() > 2.71 && config.get<double>("key4").value() < 2.73);
    
    std::cout << "    testImportExport_ImportFromYAML passed" << std::endl;
}

void testImportExport_Diff() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set up different configurations
    config1.set("common", 1, nlm::ConfigSource::Runtime);
    config1.set("only_in_1", "value1", nlm::ConfigSource::Runtime);
    
    config2.set("common", 2, nlm::ConfigSource::Runtime); // Different value
    config2.set("only_in_2", "value2", nlm::ConfigSource::Runtime); // Additional key
    
    // Get differences
    std::vector<std::string> differences = config1.diff(config2);
    
    // Should have 3 differences: changed common, added only_in_2, removed only_in_1
    assert(differences.size() >= 3);
    
    // Check for expected difference messages
    bool hasAdded = false, hasChanged = false, hasRemoved = false;
    for (const auto& diff : differences) {
        if (diff.find("Added") != std::string::npos) hasAdded = true;
        if (diff.find("Changed") != std::string::npos) hasChanged = true;
        if (diff.find("Removed") != std::string::npos) hasRemoved = true;
    }
    
    assert(hasAdded && hasChanged && hasRemoved);
    
    std::cout << "    testImportExport_Diff passed" << std::endl;
}

// Configuration Template Tests
void testConfigurationTemplate_ApplyTemplate() {
    nlm::Config config;
    
    // Apply production template
    assert(config.applyTemplate("production"));
    
    // Verify template settings
    assert(config.get<bool>("security.debug").value() == false);
    assert(config.get<int>("performance.max_connections").value() == 100);
    assert(config.get<std::string>("logging.level").value() == "ERROR");
    assert(config.get<bool>("caching.enabled").value() == true);
    assert(config.get<bool>("monitoring.enabled").value() == true);
    
    // Apply development template
    config.clear();
    assert(config.applyTemplate("development"));
    
    // Verify different settings
    assert(config.get<bool>("security.debug").value() == true);
    assert(config.get<int>("performance.max_connections").value() == 10);
    assert(config.get<std::string>("logging.level").value() == "DEBUG");
    assert(config.get<bool>("caching.enabled").value() == false);
    assert(config.get<bool>("monitoring.enabled").value() == false);
    
    std::cout << "    testConfigurationTemplate_ApplyTemplate passed" << std::endl;
}

void testConfigurationTemplate_CreateTemplate() {
    nlm::Config config;
    
    // Set up some config
    config.set("custom.setting1", 123, nlm::ConfigSource::Runtime);
    config.set("custom.setting2", "test", nlm::ConfigSource::Runtime);
    
    // Create template
    config.createTemplate("my_template", "A custom test template");
    
    // Verify template was created (listTemplates should include it)
    auto templates = config.listTemplates();
    assert(templates.find("my_template") != templates.end());
    assert(templates["my_template"] == "A custom test template");
    
    std::cout << "    testConfigurationTemplate_CreateTemplate passed" << std::endl;
}

void testConfigurationTemplate_ListTemplates() {
    nlm::Config config;
    
    // List templates
    auto templates = config.listTemplates();
    
    // Check that predefined templates exist
    assert(templates.find("production") != templates.end());
    assert(templates.find("development") != templates.end());
    assert(templates.find("testing") != templates.end());
    assert(templates.find("minimal") != templates.end());
    
    // Check descriptions
    assert(templates["production"].find("security") != std::string::npos);
    assert(templates["development"].find("debug") != std::string::npos);
    assert(templates["testing"].find("mock") != std::string::npos);
    assert(templates["minimal"].find("essential") != std::string::npos);
    
    std::cout << "    testConfigurationTemplate_ListTemplates passed" << std::endl;
}

// ============================================================================
// NEW FEATURE TEST RUNNER
// ============================================================================

void runNewFeatureTests() {
    std::cout << "Running NEW FEATURE Config tests..." << std::endl << std::endl;
    
    // Batch Operations Tests
    testBatchOperations_BatchSet();
    testBatchOperations_BatchRemove();
    testBatchOperations_BatchGet();
    testBatchOperations_BatchUpdate();
    
    std::cout << std::endl;
    
    // Scripting Support Tests
    testScriptingSupport_ExecuteScript_Basic();
    testScriptingSupport_ScriptVar();
    testScriptingSupport_ScriptEval();
    
    std::cout << std::endl;
    
    // Advanced Filtering Tests
    testAdvancedFiltering_FilterBySource();
    testAdvancedFiltering_FilterByType();
    testAdvancedFiltering_FindKeys();
    
    std::cout << std::endl;
    
    // Import/Export Tests
    testImportExport_ExportToJSON();
    testImportExport_ImportFromJSON();
    testImportExport_ExportToYAML();
    testImportExport_ImportFromYAML();
    testImportExport_Diff();
    
    std::cout << std::endl;
    
    // Configuration Template Tests
    testConfigurationTemplate_ApplyTemplate();
    testConfigurationTemplate_CreateTemplate();
    testConfigurationTemplate_ListTemplates();
    
    std::cout << std::endl << "=== ALL NEW FEATURE TESTS PASSED ===" << std::endl;
}

} // namespace test_config