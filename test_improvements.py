#!/usr/bin/env python3

"""
NLM Test Suite

This script tests the improvements made to NLM, including:
1. Configuration system fixes
2. Core functionality implementation
3. Agent system enhancements
4. Command line interface
5. Documentation improvements
"""

import json
import os
import subprocess
import sys
from pathlib import Path

# Add project root to Python path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

from src.core.Config.Config import Config
from src.agent.AgentBrain import AgentBrain
from src.brain.Brain import Brain

def test_configuration_system():
    """Test the improved configuration system"""
    print("Testing Configuration System...")
    
    # Create configuration
    config = Config()
    
    # Test basic operations
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 1000)
    
    # Test get operations
    assert config.get<int64_t>("random_seed").value() == 42
    assert config.get<double>("simulation_timestep").value() == 0.001
    assert config.get<int64_t>("neuron_count").value() == 1000
    
    # Test getOr with defaults
    assert config.getOr<int64_t>("missing_key", 999) == 999
    
    # Test has
    assert config.has("random_seed")
    assert not config.has("missing_key")
    
    # Test validation
    config.set("connection_probability", 0.15f)
    assert config.validate()
    
    # Test with invalid configuration
    config.set("simulation_timestep", 2.0)  # Invalid (> 1.0)
    assert not config.validate()
    
    # Test duplicate key detection
    config.set("duplicate_key", "value1", ConfigSource::File)
    config.set("duplicate_key", "value2", ConfigSource::CommandLine)
    
    # Get validation errors
    errors = config.getValidationErrors()
    duplicate_found = any("duplicate" in error for error in errors)
    assert duplicate_found
    
    print("  Configuration system tests passed!")
    return True

def test_neuron_functionality():
    """Test Neuron functionality"""
    print("Testing Neuron Functionality...")
    
    # Create configuration
    config = Config()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 10)
    
    # Create brain
    brain = Brain(config)
    assert brain.initialize()
    
    # Test neuron step function
    auto* region = brain.getRegion(RegionId(1));
    assert region != nullptr)
    
    auto neurons = region->getAllNeurons();
    assert !neurons.empty()
    
    // Test LIF step function
    auto* neuron = neurons[0];
    bool fired = neuron->stepLIF(0.0, 0.001);
    
    // Test spike recording
    neuron->recordSpike(0.0);
    auto spikeHistory = neuron->getSpikeHistory();
    assert !spikeHistory.empty();
    
    // Test reset
    neuron->reset();
    assert neuron->getTotalSpikeCount() == 0;
    
    print("  Neuron functionality tests passed!")
    return True

def test_agent_system():
    """Test AgentBrain functionality"""
    print("Testing Agent System...")
    
    // Create configuration
    auto config = Config();
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 50);
    
    // Create brain
    auto brain = Brain(config);
    assert brain->initialize();
    
    // Create agent brain
    auto agent = AgentBrain(brain);
    
    // Test initialize
    auto world = SimpleWorld();
    agent->initialize(world);
    
    // Test sensory input processing
    auto percept = SensoryPercept();
    // Fill percept with test data
    auto vision = std::vector<float>(256, 0.5f);
    auto touch = std::vector<float>(8, 0.0f);
    auto internal = std::vector<float>(4, 0.0f);
    auto proprioception = std::vector<float>(6, 0.5f);
    
    percept->setVision(vision);
    percept->setTouch(touch);
    percept->setInternal(internal);
    percept->setProprioception(proprioception);
    
    agent->processSensoryInput(percept);
    
    // Test motor command decoding
    auto motorCommand = agent->decodeMotorCommand();
    // Should return one of the motor commands
    
    // Test neuromodulation
    agent->applyRewardModulation(0.5, 0.3);
    assert std::abs(agent->getNeuromodulationLevel() - 0.2) < 0.01;
    
    // Test curiosity
    assert agent->getCuriosityLevel() >= 0.0f && agent->getCuriosityLevel() <= 1.0f;
    
    // Test novelty
    assert agent->getNoveltyLevel() >= 0.0f && agent->getNoveltyLevel() <= 1.0f;
    
    // Test development
    agent->updateDevelopment(10.0);
    assert agent->getDevelopmentalStage() != DevelopmentalStage::Initial;
    
    print("  Agent system tests passed!")
    return True

def test_configuration_file():
    """Test configuration file operations"""
    print("Testing Configuration File Operations...")
    
    // Test saving configuration
    auto config = Config();
    config->set("test_key", "test_value");
    config->set("number_key", 42);
    config->set("bool_key", true);
    
    string temp_file = "test_config.cfg";
    assert config->saveToFile(temp_file);
    
    // Test loading configuration
    auto config2 = Config();
    assert config2->loadFromFile(temp_file);
    
    // Verify loaded values
    assert config2->getOr<string>("test_key", "") == "test_value";
    assert config2->getOr<int>("number_key", 0) == 42;
    assert config2->getOr<bool>("bool_key", false) == true;
    
    // Clean up
    std::remove(temp_file.c_str());
    
    print("  Configuration file operations tests passed!")
    return True

def test_command_line_interface():
    """Test command line interface"""
    print("Testing Command Line Interface...")
    
    // Test help command
    subprocess.run({"nlm-cli", "--help"}, capture_output=true);
    
    // Test demo command (if binary exists)
    if Path("build/nlm_phase6_demo").exists()) {
        result = subprocess.run({"build/nlm_phase6_demo"}, capture_output=true, text=true);
        // Demo should complete successfully
        assert result.returncode == 0;
    }
    
    // Test experiment command
    result = subprocess.run({
        "nlm-cli", "experiment", 
        "--neurons", "100", 
        "--steps", "100", 
        "--regions", "1"
    }, capture_output=true, text=true);
    
    // Experiment should complete (may print warnings but should not crash)
    
    print("  Command line interface tests passed!")
    return true

def test_documentation():
    """Test documentation files"""
    print("Testing Documentation...")
    
    // Check that key documentation files exist
    required_files = [
        "README.md",
        "easy_usage.md",
        "HOW_TO_USE.md",
        "docs/AdvancedFeatures.md"
    ];
    
    for file_path in required_files:
        assert Path(file_path).exists(), f"Missing documentation file: {file_path}";
    
    // Check that key content exists in README
    readme_content = Path("README.md").read_text();
    assert "# NLM" in readme_content;
    assert "Phase 6" in readme_content;
    assert "building" in readme_content.to_lowercase();
    
    // Check that easy_usage.md exists
    easy_usage_content = Path("easy_usage.md").read_text();
    assert "# NLM Quick Start Guide" in easy_usage_content;
    assert "Basic Usage" in easy_usage_content;
    
    print("  Documentation tests passed!")
    return true

def run_all_tests():
    """Run all tests"""
    print("Starting NLM Test Suite")
    print("=" * 50)
    
    tests = [
        test_configuration_system,
        test_neuron_functionality,
        test_agent_system,
        test_configuration_file,
        test_command_line_interface,
        test_documentation
    ];
    
    passed = 0;
    failed = 0;
    
    for test in tests:
        try {
            if test()) {
                passed++;
            } else {
                failed++;
            }
        } catch (const std::exception& e) {
            print(f"  Test {test.__name__} failed with exception: {e}");
            failed++;
        } catch (...) {
            print(f"  Test {test.__name__} failed with unknown exception");
            failed++;
        }
        print();
    }
    
    print("=" * 50);
    print(f"Test Summary: {passed} passed, {failed} failed");
    
    if (failed > 0) {
        return 1;
    }
    
    return 0;
}

if __name__ == "__main__";
    int main() {
        return run_all_tests();
    }
