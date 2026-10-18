// Configuration Management Tests
#include "core/Config/Config.hpp"
#include "core/Config/Management.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_management {

void testConfigComparator() {
    nlm::Config config1;
    config1.set("neuron_count", 1000LL);
    config1.set("simulation_timestep", 0.001);
    
    nlm::Config config2;
    config2.set("neuron_count", 1000LL);
    config2.set("simulation_timestep", 0.001);
    
    nlm::ConfigComparator comparator("detailed");
    
    // Test similarity
    double similarity = comparator.getSimilarityScore(config1, config2);
    assert(similarity == 1.0);
    
    // Test changed keys
    auto changed = comparator.getChangedKeys(config1, config2);
    assert(changed.empty());
    
    // Test new keys
    auto new_keys = comparator.getNewKeys(config1, config2);
    assert(new_keys.empty());
    
    // Test removed keys
    auto removed_keys = comparator.getRemovedKeys(config1, config2);
    assert(removed_keys.empty());
    
    // Test with different configs
    nlm::Config config3;
    config3.set("neuron_count", 2000);
    
    similarity = comparator.getSimilarityScore(config1, config3);
    assert(similarity == 0.5); // 1 common key out of 2 total
    
    auto changed_keys = comparator.getChangedKeys(config1, config3);
    assert(changed_keys.size() == 1);
    
    std::cout << "    testConfigComparator passed" << std::endl;
}

void testConfigDiffTool() {
    nlm::Config config1;
    config1.set("a", 1);
    config1.set("b", 2);
    config1.set("c", 3);
    
    nlm::Config config2;
    config2.set("a", 1);
    config2.set("b", 20);
    config2.set("d", 4);
    
    nlm::ConfigDiffTool diff_tool("test_diff");
    
    auto diffs = diff_tool.diff(config1, config2);
    assert(diffs.size() == 3); // a, b, c vs a, b, d
    
    auto unified_diff = diff_tool.unifiedDiff(config1, config2);
    assert(unified_diff.getKeys().size() == 2); // Only changed keys
    
    auto similarity = diff_tool.similarity(config1, config2);
    assert(similarity == 0.5); // 1 common key out of 3 total
    
    std::cout << "    testConfigDiffTool passed" << std::endl;
}

void testConfigValidationReport() {
    nlm::ConfigValidationReport report("test_report");
    
    assert(report.is_valid());
    assert(!report.has_warnings());
    
    report.addIssue("missing_param", "Parameter is required", 
                    nlm::ConfigValidationReport::Issue::Severity::Error);
    report.addWarning("deprecated_param", "This parameter is deprecated");
    
    assert(!report.is_valid());
    assert(report.has_warnings());
    assert(report.warnings.size() == 1);
    assert(report.errors.size() == 1);
    
    std::string json_report = report.generateJSONReport();
    assert(!json_report.empty());
    
    std::string html_report = report.generateHTMLReport();
    assert(!html_report.empty());
    
    std::string text_report = report.generateReport();
    assert(!text_report.empty());
    
    std::cout << "    testConfigValidationReport passed" << std::endl;
}

void testConfigMigrator() {
    nlm::Config config;
    config.set("old_param", 100);
    
    nlm::ConfigMigrator migrator("1.0", "2.0");
    
    // Add a migration step that renames a parameter
    migrator.addMigrationStep("1.0_to_2.0", 
        [](nlm::Config& c) {
            auto val = c.getHierarchical("old_param");
            if (val.has_value()) {
                c.setHierarchical("new_param", val.value());
                c.removeHierarchical("old_param");
            }
        },
        "Migrate old_param to new_param");
    
    migrator.migrate(config);
    
    assert(config.hasHierarchical("new_param"));
    assert(!config.hasHierarchical("old_param"));
    assert(config.getHierarchical<int64_t>("new_param") == 100);
    
    std::cout << "    testConfigMigrator passed" << std::endl;
}

void testConfigHistoryManager() {
    nlm::ConfigHistoryManager history("test_history.json");
    history.auto_save = false; // Don't auto-save to file
    
    nlm::Config config1;
    config1.set("param1", "value1");
    
    history.saveSnapshot(config1, "v1.0", {{ "environment", "test" }});
    
    nlm::Config config2;
    config2.set("param2", 42);
    
    history.saveSnapshot(config2, "v2.0", {{ "environment", "production" }});
    
    assert(history.getHistorySize() == 2);
    
    auto snapshot1 = history.getSnapshot(0);
    assert(snapshot1.config.getOr<std::string>("param1", "") == "value1");
    
    auto snapshot2 = history.getSnapshot(1);
    assert(snapshot2.config.getOr<int64_t>("param2", 0) == 42);
    
    // Test rollback
    auto rolled_back = history.rollbackToSnapshot(0);
    assert(rolled_back.getOr<std::string>("param1", "") == "value1");
    assert(!rolled_back.has("param2"));
    
    // Test diff
    auto diff = history.getDiffBetweenSnapshots(0, 1);
    assert(diff.getKeys().size() == 1);
    
    std::cout << "    testConfigHistoryManager passed" << std::endl;
}

void testConfigMonitor() {
    nlm::ConfigMonitor monitor("test_monitor");
    
    int change_count = 0;
    monitor.addRule("test_param", 
        [](const nlm::ConfigValue& v) { return std::get<std::string>(v) == "changed"; },
        [&change_count](const nlm::ConfigValue& v, nlm::ConfigSource s) { change_count++; },
        "Monitor test_param for changes");
    
    nlm::Config config;
    config.set("test_param", std::string("original"));
    monitor.checkConfig(config);
    assert(change_count == 0);
    
    config.set("test_param", std::string("changed"));
    monitor.checkConfig(config);
    assert(change_count == 1);
    
    std::cout << "    testConfigMonitor passed" << std::endl;
}

void testConfigExporter() {
    nlm::Config config;
    config.set("int_value", 42);
    config.set("string_value", std::string("hello"));
    config.set("float_value", 3.14);
    config.set("bool_value", true);
    
    nlm::ConfigExporter exporter("test_exporter");
    
    // Test JSON export
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
    
    // Test CSV export
    exporter.setFormat(nlm::ConfigExporter::ExportFormat::CSV);
    std::string csv_output = exporter.exportConfig(config);
    assert(!csv_output.empty());
    assert(csv_output.find("int_value") != std::string::npos);
    
    std::cout << "    testConfigExporter passed" << std::endl;
}

void testConfigImporter() {
    nlm::Config config;
    config.set("a", 1);
    config.set("b", std::string("test"));
    
    nlm::ConfigExporter exporter;
    exporter.setFormat(nlm::ConfigExporter::ExportFormat::JSON);
    std::string json_output = exporter.exportConfig(config);
    
    nlm::ConfigImporter importer("test_importer");
    
    nlm::ConfigImporter::ImportOptions options;
    options.override_existing = true;
    importer.setOptions(options);
    
    auto imported_config = importer.importFromJSON(json_output);
    assert(imported_config.getOr<int>("a", 0) == 1);
    assert(imported_config.getOr<std::string>("b", "") == "test");
    
    std::cout << "    testConfigImporter passed" << std::endl;
}

void testConfigFactory() {
    nlm::ConfigFactory factory;
    
    auto dev_config = nlm::ConfigFactory::createDevelopmentConfig();
    assert(dev_config.getOr<int64_t>("neuron_count", 0) == 500);
    assert(dev_config.getOr<std::string>("environment_name", "") == "GridWorld");
    
    auto test_config = nlm::ConfigFactory::createTestingConfig();
    assert(test_config.getOr<int64_t>("neuron_count", 0) == 100);
    
    auto prod_config = nlm::ConfigFactory::createProductionConfig();
    assert(prod_config.getOr<int64_t>("neuron_count", 0) == 10000);
    
    auto minimal_config = nlm::ConfigFactory::createMinimalConfig();
    assert(minimal_config.getOr<int64_t>("neuron_count", 0) == 1000);
    
    std::cout << "    testConfigFactory passed" << std::endl;
}

void runAll() {
    testConfigComparator();
    testConfigDiffTool();
    testConfigValidationReport();
    testConfigMigrator();
    testConfigHistoryManager();
    testConfigMonitor();
    testConfigExporter();
    testConfigImporter();
    testConfigFactory();
    
    std::cout << std::endl << "All management tests passed!" << std::endl;
}

} // namespace test_management