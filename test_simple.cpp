#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    config.set("test", "value", nlm::ConfigSource::Runtime);
    std::string val;
    auto result = config.get<std::string>("test", val);
    if (result.isSuccess()) {
        std::cout << "Got value: " << val << std::endl;
        return 0;
    } else {
        std::cout << "Error: " << result.getMessage() << std::endl;
        return 1;
    }
}
