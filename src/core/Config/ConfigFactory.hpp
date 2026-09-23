#pragma once

#include "ConfigFactory.hpp"
#include <stdexcept>
#include <sstream>

namespace nlm {

std::unordered_map<std::string, ConfigSchema> ConfigFactory::createNeuralNetworkSchema() {
    ConfigBuilder builder;
    
    builder.add("nn.input_size", ConfigSchema("input_size", ConfigSchema::Type::Integer, true))
        .withRange(1, 10000)
        .withDefault(784)
        .addValidator<int>([](const int& v) { return v > 0; },
                          [](const int& v) { return "Input size must be positive"; });
    
    builder.add("nn.hidden_layers", ConfigSchema("hidden_layers", ConfigSchema::Type::IntVector, false))
        .withDefault(std::vector<int>{128, 64, 32});
    
    builder.add("nn.learning_rate", ConfigSchema("learning_rate", ConfigSchema::Type::Double, false, "Neural network learning rate"))
        .withRange(0.0, 1.0)
        .withDefault(0.001)
        .addValidator<double>([](const double& v) { return v >= 0.0 && v <= 1.0; },
                            [](const double& v) { return "Learning rate must be between 0.0 and 1.0"; });
    
    builder.add("nn.activation", ConfigSchema("activation", ConfigSchema::Type::String, false, "Activation function"))
        .withLengthRange(2, 20)
        .withDefault(std::string("relu"))
        .addValidator<std::string>([](const std::string& v) { 
            return v == "relu" || v == "sigmoid" || v == "tanh"; },
                                      [](const std::string& v) { 
            return "Activation must be 'relu', 'sigmoid', or 'tanh'"; });
    
    return builder.build();
}

std::unordered_map<std::string, ConfigSchema> ConfigFactory::createSimulationSchema() {
    ConfigBuilder builder;
    
    builder.add("sim.duration", ConfigSchema("duration", ConfigSchema::Type::Double, true, "Simulation duration"))
        .withRange(0.1, 1000.0)
        .withDefault(10.0);
    
    builder.add("sim.time_step", ConfigSchema("time_step", ConfigSchema::Type::Double, true, "Time step for simulation"))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
        .addValidator<double>([](const double& v) { return v > 0.0; },
                            [](const double& v) { return "Time step must be positive"; });
    
    builder.add("sim.random_seed", ConfigSchema("random_seed", ConfigSchema::Type::Integer, false, "Random seed for reproducibility"))
        .withRange(0, 2147483647)
        .withDefault(42);
    
    return builder.build();
}

std::unordered_map<std::string, ConfigSchema> ConfigFactory::createLoggingSchema() {
    ConfigBuilder builder;
    
    builder.add("log.level", ConfigSchema("level", ConfigSchema::Type::String, false, "Logging level"))
        .withLengthRange(1, 20)
        .withDefault(std::string("info"))
        .addValidator<std::string>([](const std::string& v) { 
            return v == "trace" || v == "debug" || v == "info" || 
                   v == "warning" || v == "error" || v == "fatal"; },
                                      [](const std::string& v) { 
            return "Invalid logging level: " + v; });
    
    builder.add("log.file.path", ConfigSchema("file.path", ConfigSchema::Type::String, false, "Log file path"))
        .withLengthRange(1, 255)
        .withDefault(std::string("nlm.log"));
    
    builder.add("log.max_size_mb", ConfigSchema("max_size_mb", ConfigSchema::Type::Integer, false, "Maximum log file size in MB"))
        .withRange(1, 1000)
        .withDefault(100);
    
    return builder.build();
}

std::unordered_map<std::string, ConfigSchema> ConfigFactory::createMemorySchema() {
    ConfigBuilder builder;
    
    builder.add("mem.working.capacity", ConfigSchema("capacity", ConfigSchema::Type::Integer, true, "Working memory capacity"))
        .withRange(100, 1000000)
        .withDefault(10000);
    
    builder.add("mem.episodic.limit", ConfigSchema("limit", ConfigSchema::Type::Integer, false, "Episodic memory limit"))
        .withRange(1, 1000)
        .withDefault(100);
    
    return builder.build();
}

} // namespace nlm