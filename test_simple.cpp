#include <iostream>
#include "src/core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Test basic config operations
    config.set("string_key", std::string("hello"));
    config.set("int_key", int(42));
    config.set("double_key", double(3.14159));
    config.set("bool_key", bool(true));
    
    std::cout << "Config summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test get with default
    std::string default_value = "default";
    std::string value = config.getOr<std::string>("string_key", default_value);
    std::cout << "Got string_key: '" << value << "'" << std::endl;
    
    return 0;
}