#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Demonstrating the improved NLM brain system..." << std::endl << std::endl;
    
    // Create Phase 6 demo
    NLMPhase6Demo demo;
    
    std::cout << "1. Running integration verification..." << std::endl;
    if (demo.verifyIntegration()) {
        std::cout << "   ✓ Integration verification passed!" << std::endl;
    } else {
        std::cout << "   ✗ Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "2. Testing memory integration..." << std::endl;
    if (demo.testMemoryIntegration()) {
        std::cout << "   ✓ Memory integration test passed!" << std::endl;
    } else {
        std::cout << "   ✗ Memory integration test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "3. Testing neuromodulation integration..." << std::endl;
    if (demo.testNeuromodulationIntegration()) {
        std::cout << "   ✓ Neuromodulation integration test passed!" << std::endl;
    } else {
        std::cout << "   ✗ Neuromodulation integration test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "4. Testing checkpoint save/load..." << std::endl;
    if (demo.testCheckpointing()) {
        std::cout << "   ✓ Checkpoint test passed!" << std::endl;
    } else {
        std::cout << "   ✗ Checkpoint test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "5. Testing replay system..." << std::endl;
    if (demo.testReplay()) {
        std::cout << "   ✓ Replay test passed!" << std::endl;
    } else {
        std::cout << "   ✗ Replay test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "The NLM Phase 6 system now includes:" << std::endl;
    std::cout << "• Full checkpoint save/load functionality" << std::endl;
    std::cout << "• Command-line interface (nlm_cli_exec)" << std::endl;
    std::cout << "• Interactive mode for brain experimentation" << std::endl;
    std::cout << "• Data export in multiple formats (txt, json)" << std::endl;
    std::cout << "• Improved Brain::save() and Brain::load() methods" << std::endl;
    std::cout << "• Comprehensive CheckpointSystem infrastructure" << std::endl;
    std::cout << std::endl;
    std::cout << "Key improvements:" << std::endl;
    std::cout << "✓ CheckpointSystem.cpp - Complete implementation with Reader/Writer/Manager" << std::endl;
    std::cout << "✓ Brain::save() - Now properly implements checkpointing with CheckpointWriter" << std::endl;
    std::cout << "✓ Brain::load() - Now properly implements checkpoint loading with CheckpointReader" << std::endl;
    std::cout << "✓ nlm_cli_exec - New CLI interface for brain manipulation" << std::endl;
    std::cout << "✓ nlm_cli_exec.cpp - CLI implementation with command parsing" << std::endl;
    std::cout << "✓ cli_main.cpp - Entry point for CLI application" << std::endl;
    std::cout << "✓ Updated CMakeLists.txt - Includes CLI executable in build" << std::endl;
    
    return 0;
}