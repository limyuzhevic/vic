#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing enhanced NLM configuration system..." << std::endl;
    
    nlm::Config config;
    
    // Test basic configuration
    config.set("test.int", 42);
    config.set("test.double", 3.14);
    config.set("test.string", "Hello World");
    config.set("test.bool", true);
    
    // Test getting values
    int intVal = config.getOr<int>("test.int", 0);
    double doubleVal = config.getOr<double>("test.double", 0.0);
    std::string stringVal = config.getOr<std::string>("test.string", "");
    bool boolVal = config.getOr<bool>("test.bool", false);
    
    std::cout << "int: " << intVal << std::endl;
    std::cout << "double: " << doubleVal << std::endl;
    std::cout << "string: " << stringVal << std::endl;
    std::cout << "bool: " << (boolVal ? "true" : "false") << std::endl;
    
    // Test configuration schema
    nlm::ConfigSchema schema;
    schema.addElement(nlm::ConfigSchemaElement("required.value", nlm::ConfigDataType::INT64, true, 100LL, "A required int64"));
    schema.addElement(nlm::ConfigSchemaElement("optional.value", nlm::ConfigDataType::DOUBLE, false, 1.5, "An optional double"));
    
    config.setSchema(schema);
    config.set("required.value", 200LL);
    
    bool isValid = config.isValid();
    std::cout << "\nConfiguration valid: " << (isValid ? "yes" : "no") << std::endl;
    
    if (!isValid) {
        std::cout << "Validation errors:" << std::endl;
        auto errors = config.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Test stats
    auto stats = config.getStats();
    std::cout << "\nStats:" << std::endl;
    std::cout << "  Total entries: " << stats.totalEntries << std::endl;
    std::cout << "  Valid entries: " << stats.validEntries << std::endl;
    std::cout << "  Invalid entries: " << stats.invalidEntries << std::endl;
    
    std::cout << "\nEnhanced configuration system test completed successfully!" << std::endl;
    return 0;
}
