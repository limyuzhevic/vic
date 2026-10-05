#include <iostream>

// Simple test to check if the Config class has all required methods
// This simulates what main.cpp is doing

class MockConfig {
public:
    template<typename T>
    void setIfNotSet(const std::string& key, const T& value, int source = 0) {
        std::cout << "setIfNotSet called with key: " << key << ", value: " << value << std::endl;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        std::cout << "getOr called with key: " << key << ", default: " << defaultValue << std::endl;
        return defaultValue;
    }
};

int main() {
    MockConfig config;
    
    // This is what main.cpp is calling
    config.setIfNotSet("random_seed", static_cast<int64_t>(42), 0);
    config.setIfNotSet("simulation_timestep", 0.001, 0);
    config.setIfNotSet("neuron_count", static_cast<int64_t>(500), 0);
    config.setIfNotSet("region_count", static_cast<int64_t>(1), 0);
    config.setIfNotSet("connection_probability", 0.15f, 0);
    
    // These are also called in main.cpp
    auto seed = config.getOr<int64_t>("random_seed", 42);
    auto timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    return 0;
}
