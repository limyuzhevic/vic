#include "src/core/Config/Config.hpp"

int main() {
    nlm::Config config;

    // Test basic functionality
    config.setString("name", "Test Config");
    config.setInt("iterations", 100);
    config.setDouble("learning_rate", 0.01);
    config.setBool("verbose", true);

    // Test type-safe getters
    auto name = config.getString("name");
    if (name.has_value()) {
        // Use the value
    }

    auto iterations = config.getInt("iterations");
    if (iterations.has_value()) {
        // Use the value
    }

    // Test with schema
    auto builder = nlm::Config::createBuilder();
    builder.add("new_param", nlm::ConfigSchema("new_param", nlm::ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.5)
        .addValidator<double>([](const double& v) { return v >= 0.0 && v <= 1.0; },
                            [](const double& v) { return "Value must be between 0 and 1"; });

    // Test factory presets
    auto neuralNetSchema = nlm::ConfigFactory::createNeuralNetworkSchema();
    auto simSchema = nlm::ConfigFactory::createSimulationSchema();

    std::cout << config.summary() << std::endl;

    return 0;
}