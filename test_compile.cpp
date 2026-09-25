// Simple compilation test for Config class
#include "src/core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Test basic functionality
    config.set("test", 42, nlm::ConfigSource::Runtime);
    config.set("test2", "hello", nlm::ConfigSource::Runtime);
    config.set("test3", 3.14, nlm::ConfigSource::Runtime);
    config.set("test4", true, nlm::ConfigSource::Runtime);
    
    // Test retrieval
    auto val1 = config.get<int>("test");
    auto val2 = config.get<std::string>("test2");
    auto val3 = config.get<double>("test3");
    auto val4 = config.get<bool>("test4");
    
    // Test save (this will create a file)
    config.saveToFile("/tmp/test_output.cfg");
    
    return 0;
}
