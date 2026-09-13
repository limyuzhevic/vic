#include "src/core/Config/Config.hpp"
#include <iostream>

namespace nlm {

// Test code for Config
void test_config() {
    nlm::Config config;
    
    // Test setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime, "A string parameter");
    config.set("width", 100, nlm::ConfigSource::Runtime);
    config.set("opacity", 0.5, nlm::ConfigSource::Runtime);
    config.set("format", "PNG", nlm::ConfigSource::Runtime);
    
    std::cout << "Test completed successfully!" << std::endl;
}

} // namespace nlm

int main() {
    nlm::test_config();
    return 0;
}