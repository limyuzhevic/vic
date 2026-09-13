// Test program to verify Config refactoring
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_85be9288-5eb9-452f-92dd-cb4b15333251

cat > test_final.cpp << 'EOF'
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    nlm::Config config;
    
    // Test basic functionality
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime, "Test string");
    config.set("bool_val", true, nlm::ConfigSource::Runtime, "Test boolean");
    
    assert(config.has("int_val"));
    assert(config.has("double_val"));
    assert(config.has("string_val"));
    assert(config.has("bool_val"));
    
    auto int_val = config.get<int>("int_val");
    assert(int_val.has_value() && *int_val == 42);
    
    auto string_val = config.get<std::string>("string_val");
    assert(string_val.has_value() && *string_val == "hello");
    
    // Test validation report
    std::string report = config.generateValidationReport();
    assert(!report.empty() && report.find("Summary:") != std::string::npos);
    
    // Test backward compatibility
    config.set("legacy_int", 100, nlm::ConfigSource::Runtime);
    auto legacy_val = config.get<int>("legacy_int");
    assert(legacy_val.has_value() && *legacy_val == 100);
    
    std::cout << "All tests passed! Config refactoring completed successfully." << std::endl;
    return 0;
}
EOF

echo "Created test_final.cpp"
echo "Compilation command: g++ -std=c++17 -I . test_final.cpp src/core/Config/Config.cpp -o test_final && ./test_final"