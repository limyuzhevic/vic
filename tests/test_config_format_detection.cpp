// Test configuration format detection
// This test verifies that the configuration system correctly detects file formats

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config_format_detection {

void testFormatDetection() {
    // Test format detection based on file extension
    std::string jsonExt = nlm::Config::getConfigFormat("test.json");
    assert(jsonExt == ".json");
    
    std::string yamlExt = nlm::Config::getConfigFormat("test.yaml");
    assert(yamlExt == ".yaml");
    
    std::string ymlExt = nlm::Config::getConfigFormat("test.yml");
    assert(ymlExt == ".yml");
    
    std::string cfgExt = nlm::Config::getConfigFormat("test.cfg");
    assert(cfgExt == ".cfg");
    
    std::string txtExt = nlm::Config::getConfigFormat("test.txt");
    assert(txtExt == ".txt");
    
    std::string noExt = nlm::Config::getConfigFormat("test");
    assert(noExt == "");
    
    std::cout << "testFormatDetection passed" << std::endl;
}

} // namespace test_config_format_detection

int main() {
    test_config_format_detection::testFormatDetection();
    std::cout << "All configuration format detection tests passed!" << std::endl;
    return 0;
}
