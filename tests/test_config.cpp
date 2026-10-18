// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

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

void testHierarchicalConfig() {
    nlm::Config config;
    
    config.setHierarchical("simulation.neuron_count", 1000LL);
    config.setHierarchical("simulation.timestep", 0.001);
    config.setHierarchical("environment.name", std::string("GridWorld"));
    
    assert(config.hasHierarchical("simulation.neuron_count"));
    assert(config.hasHierarchical("simulation.timestep"));
    assert(config.hasHierarchical("environment.name"));
    
    auto neuron_count = config.getHierarchical<int64_t>("simulation.neuron_count");
    assert(neuron_count.has_value());
    assert(*neuron_count == 1000LL);
    
    std::cout << "    testHierarchicalConfig passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test1", 1, nlm::ConfigSource::Runtime);
    config.set("test2", std::string("hello"), nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration (2 entries)") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    nlm::ConfigSchema schema("test_schema");
    
    nlm::ConfigSchema::FieldSchema field;
    field.name = "required_param";
    field.type = "int";
    field.default_value = 0;
    field.required = true;
    field.validator = [](const nlm::ConfigValue& v) { return std::get<int>(v) > 0; };
    schema.addField(field);
    
    config.set("required_param", 42, nlm::ConfigSource::Runtime);
    assert(config.validate(schema));
    
    config.set("required_param", -1, nlm::ConfigSource::Runtime);
    assert(!config.validate(schema));
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigExportImport() {
    nlm::Config config;
    config.set("neuron_count", 1000LL);
    config.set("timestep", 0.001);
    config.set("environment_name", std::string("GridWorld"));
    
    // Test JSON export
    nlm::ConfigExporter exporter("test_exporter");
    exporter.setFormat(nlm::ConfigExporter::ExportFormat::JSON);
    std::string json_output = exporter.exportConfig(config);
    assert(!json_output.empty());
    
    // Test YAML export
    exporter.setFormat(nlm::ConfigExporter::ExportFormat::YAML);
    std::string yaml_output = exporter.exportConfig(config);
    assert(!yaml_output.empty());
    
    // Test TOML export
    exporter.setFormat(nlm::ConfigExporter::ExportFormat::TOML);
    std::string toml_output = exporter.exportConfig(config);
    assert(!toml_output.empty());
    
    // Test import
    nlm::ConfigImporter importer("test_importer");
    Config imported_config = importer.importFromJSON(json_output);
    assert(imported_config.getOr<int64_t>("neuron_count", 0) == 1000);
    
    std::cout << "    testConfigExportImport passed" << std::endl;
}

void testConfigTemplateManager() {
    auto templates = nlm::ConfigTemplateManager().getBuiltInTemplates();
    assert(!templates.empty());
    
    // Test development template
    auto dev_config = nlm::ConfigTemplateManager::loadDevelopmentTemplate();
    assert(dev_config.getOr<int64_t>("neuron_count", 0) == 500);
    assert(dev_config.getOr<std::string>("environment_name", "") == "GridWorld");
    
    // Test testing template
    auto test_config = nlm::ConfigTemplateManager::loadTestingTemplate();
    assert(test_config.getOr<int64_t>("neuron_count", 0) == 100);
    
    // Test production template
    auto prod_config = nlm::ConfigTemplateManager::loadProductionTemplate();
    assert(prod_config.getOr<int64_t>("neuron_count", 0) == 10000);
    
    std::cout << "    testConfigTemplateManager passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testHierarchicalConfig();
    testConfigSummary();
    testConfigSchemaValidation();
    testConfigExportImport();
    testConfigTemplateManager();
    
    std::cout << std::endl << "All tests passed!" << std::endl;
}

} // namespace test_config