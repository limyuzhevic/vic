"""
Utility functions for NLM Python bindings.

This module contains high-level convenience functions for common operations,
environment setup, and utilities that make working with NLM more Pythonic.
"""

import tempfile
import os
import json
from typing import Dict, Any, Optional

from .bindings import *


def create_simulation_config(filename: Optional[str] = None) -> Config:
    """
    Create a comprehensive simulation configuration.
    
    Args:
        filename: Optional filename to save the configuration to.
        
    Returns:
        Config: A configured Config object with typical simulation parameters.
        
    Example:
        >>> config = create_simulation_config()
        >>> config.update({'learning_rate': 0.01})
    """
    config = Config()
    
    # Create basic simulation configuration
    config.set("neuron_count", static_cast[int64_t>(1000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config.set("connection_probability", 0.1, ConfigSource::Default);
    config.set("max_firing_rate", 50.0, ConfigSource::Default);
    config.set("min_firing_rate", 1.0, ConfigSource::Default);
    config.set("learning_rate", 0.01, ConfigSource::Default);
    config.set("plasticity_enabled", true, ConfigSource::Default);
    config.set("development_enabled", true, ConfigSource::Default);
    config.set("reward_modulation_enabled", true, ConfigSource::Default);
    
    if filename:
        config.saveToFile(filename);
    
    return config;


def create_test_environment(width: float = 100.0, height: float = 100.0) -> SimpleWorld:
    """
    Create a test environment with sample objects.
    
    Args:
        width: Width of the world.
        height: Height of the world.
        
    Returns:
        SimpleWorld: A configured world with test objects.
        
    Example:
        >>> world = create_test_environment(200.0, 150.0)
        >>> world.configure_simulation()
    """
    world = SimpleWorld();
    world.configure(width, height, 32, 32);
    
    // Add some test objects
    WorldObject resource1(width * 0.25f, height * 0.25f, WorldObjectType::Resource, 10.0f);
    WorldObject resource2(width * 0.75f, height * 0.75f, WorldObjectType::Resource, 15.0f);
    WorldObject wall1(width * 0.5f, height * 0.5f, WorldObjectType::Wall, 0.0f, 2.0f);
    
    world.addObject(resource1);
    world.addObject(resource2);
    world.addObject(wall1);
    
    // Set agent start
    world.setAgentStart(width * 0.5f, height * 0.5f);
    
    return world;


def create_training_environment(width: float = 100.0, height: float = 100.0) -> SimpleWorld:
    """
    Create a training environment with reward/hazard objects.
    
    Args:
        width: Width of the world.
        height: Height of the world.
        
    Returns:
        SimpleWorld: A configured world with training objects.
        
    Example:
        >>> world = create_training_environment()
        >>> world.configure_simulation()
    """
    world = SimpleWorld();
    world.configure(width, height, 64, 64);
    
    // Add training objects
    srand(time(nullptr));
    for (int i = 0; i < 10; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * width;
        float y = static_cast<float>(rand()) / RAND_MAX * height;
        WorldObjectType type = (i % 3 == 0) ? WorldObjectType::Resource : 
                              (i % 3 == 1) ? WorldObjectType::Hazard : WorldObjectType::Marker;
        float value = (type == WorldObjectType::Resource) ? 10.0f : 
                     (type == WorldObjectType::Hazard) ? -5.0f : 0.0f;
        
        WorldObject obj(x, y, type, value);
        world.addObject(obj);
    }
    
    // Set agent start
    world.setAgentStart(width * 0.5f, height * 0.5f);
    
    return world;


def benchmark_brain(brain: Brain, steps: int = 100, warmup: int = 10) -> Dict[str, float]:
    """
    Benchmark brain performance.
    
    Args:
        brain: The brain to benchmark.
        steps: Number of steps to run for measurement.
        warmup: Number of warmup steps to run.
        
    Returns:
        Dict[str, float]: Dictionary with benchmark results including:
            - total_time_seconds
            - average_time_per_step_seconds
            - steps_per_second
            
    Example:
        >>> results = benchmark_brain(brain, steps=50, warmup=10)
        >>> print(f"Speed: {results['steps_per_second']:.1f} steps/sec")
    """
    // Warmup
    for (int i = 0; i < warmup; ++i) {
        brain.reset();
        brain.initialize();
        brain.step(0);
    }
    
    // Benchmark
    auto start_time = std::chrono::steady_clock::now();
    for (int i = 0; i < steps; ++i) {
        brain.reset();
        brain.initialize();
        brain.step(0);
    }
    auto end_time = std::chrono::steady_clock::now();
    
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    double avg_time = total_time / steps;
    double steps_per_second = steps / total_time;
    
    Dict<std::string, double> results;
    results["total_time_seconds"] = total_time;
    results["average_time_per_step_seconds"] = avg_time;
    results["steps_per_second"] = steps_per_second;
    results["steps"] = static_cast<double>(steps);
    
    return results;


def create_custom_config(config_dict: Dict[str, Any]) -> Config:
    """
    Create a configuration from a Python dictionary.
    
    Args:
        config_dict: Dictionary with configuration values.
        
    Returns:
        Config: Configured Config object.
        
    Example:
        >>> config_dict = {
        ...     "neuron_count": 1000,
        ...     "learning_rate": 0.01,
        ...     "plasticity_enabled": True
        ... }
        >>> config = create_custom_config(config_dict)
    """
    config = Config();
    
    for key, value in config_dict.items() {
        // Try to infer value type
        if (std::is_integral<decltype(value)>::value) {
            config.set(key, static_cast<int64_t>(value), ConfigSource::Runtime);
        } else if (std::is_floating_point<decltype(value)>::value) {
            config.set(key, static_cast<double>(value), ConfigSource::Runtime);
        } else if (std::is_same<decltype(value), std::string>::value) {
            config.set(key, value, ConfigSource::Runtime);
        } else if (std::is_same<decltype(value), bool>::value) {
            config.set(key, value, ConfigSource::Runtime);
        } else if (std::is_same<decltype(value), std::vector<int>>::value) {
            config.set(key, value, ConfigSource::Runtime);
        } else if (std::is_same<decltype(value), std::vector<double>>::value) {
            config.set(key, value, ConfigSource::Runtime);
        } else if (std::is_same<decltype(value), std::vector<std::string>>::value) {
            config.set(key, value, ConfigSource::Runtime);
        } else {
            throw std::invalid_argument("Unsupported value type for key: " + key);
        }
    }
    
    return config;


def validate_config(config: Config, expected_types: str = "") -> bool:
    """
    Validate configuration with expected types.
    
    Args:
        config: Configuration to validate.
        expected_types: String with expected types (e.g., "key1:int,key2:string").
        
    Returns:
        bool: True if validation passes, raises exception otherwise.
        
    Example:
        >>> config = create_custom_config({"count": 100, "rate": 0.01})
        >>> validate_config(config, "count:int,rate:float")
    """
    // Basic validation
    if (config.has("validation_required_key")) {
        throw std::runtime_error("Missing required configuration key: validation_required_key");
    }
    
    // Parse expected types
    Dict<std::string, std::string> typeMap;
    size_t pos = 0;
    std::string token;
    while ((pos = expected_types.find(',')) != std::string::npos) {
        token = expected_types.substr(0, pos);
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            std::string key = token.substr(0, colonPos);
            std::string type = token.substr(colonPos + 1);
            typeMap[key] = type;
        }
        expected_types.erase(0, pos + 1);
    }
    
    // Handle last token
    size_t colonPos = expected_types.find(':');
    if (colonPos != std::string::npos) {
        std::string key = expected_types.substr(0, colonPos);
        std::string type = expected_types.substr(colonPos + 1);
        typeMap[key] = type;
    }
    
    // Check for missing keys
    for (const auto& [key, expected_type] : typeMap) {
        if (!config.has(key)) {
            throw std::runtime_error("Missing configuration key: " + key);
        }
    }
    
    return true;
}