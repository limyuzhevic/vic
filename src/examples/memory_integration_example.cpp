// NLM - Memory Integration Example
// Demonstrates working memory, episodic memory, and associative memory integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>

int main() {
    std::cout << "=== NLM Memory Integration Example ===" << std::endl;
    std::cout << "Demonstrates integration of working memory, episodic memory, and associative memory systems." << std::endl;
    
    // Initialize logger for debugging
    initializeLogger("memory_integration_example.log", LogLevel::Info);
    
    try {
        // Create configuration optimized for memory systems
        nlm::Config config = nlm::Config::createDefaultConfig();
        
        // Configure for memory-intensive simulation
        config.set("brain.neuron_count", 600);
        config.set("brain.synapse_density", 0.12);
        config.set("memory.working.capacity", 150);
        config.set("memory.working.trace_length", 8);
        config.set("memory.episodic.capacity", 2000);
        config.set("memory.episodic.replay_interval", 50);
        config.set("memory.associative.pattern_count", 200);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        
        std::cout << "Configuration created with memory-optimized settings:" << std::endl;
        std::cout << "  - Working memory capacity: " << config.get<int>("memory.working.capacity") << std::endl;
        std::cout << "  - Episodic memory capacity: " << config.get<int>("memory.episodic.capacity") << std::endl;
        std::cout << "  - Associative pattern count: " << config.get<int>("memory.associative.pattern_count") << std::endl;
        
        // Create brain with memory configuration
        nlm::Brain brain(config);
        brain.initialize();
        
        std::cout << "Brain created successfully with memory systems." << std::endl;
        
        // Generate random seed for reproducible patterns
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        
        // Run simulation with memory operations
        std::cout << "\nStarting memory integration simulation..." << std::endl;
        std::cout << "Step, Working Memory Usage (%), Episodic Memories, "
                  << "Pattern Retrieval Success Rate, Replay Operations" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        int total_retrievals = 0;
        int successful_retrievals = 0;
        int total_replays = 0;
        
        for (int episode = 0; episode < 100; ++episode) {
            // Generate random experience pattern
            std::vector<double> experience_pattern(50, 0.0);
            for (int i = 0; i < 50; ++i) {
                experience_pattern[i] = dist(gen);
            }
            
            // Store in working memory
            brain.storeWorkingMemory(experience_pattern);
            
            // Record in episodic memory
            brain.recordEpisodicMemory(experience_pattern, episode, 0.7 + dist(gen) * 0.3);
            
            // Run brain step
            brain.step(episode);
            
            // Simulate memory retrieval - attempt to recall previous pattern
            if (episode > 10) {
                // Retrieve from working memory (current pattern)
                std::vector<double> current_pattern = brain.retrieveWorkingMemory(
                    experience_pattern, 0.8
                );
                
                if (!current_pattern.empty()) {
                    total_retrievals++;
                    successful_retrievals++;
                }
                
                // Replay episodic memory occasionally
                if (episode % 20 == 0) {
                    bool replay_success = brain.replayEpisodicMemory(episode, 0.5);
                    if (replay_success) {
                        total_replays++;
                    }
                }
            }
            
            // Record statistics every 10 episodes
            if (episode % 10 == 0) {
                double working_memory_usage = 
                    static_cast<double>(brain.getWorkingMemoryUsage()) / 
                    static_cast<double>(brain.getWorkingMemoryCapacity()) * 100.0;
                
                double retrieval_success_rate = total_retrievals > 0 ? 
                    static_cast<double>(successful_retrievals) / total_retrievals * 100.0 : 0.0;
                
                std::cout << episode << ", "
                          << std::fixed << std::setprecision(1) << working_memory_usage << "%, "
                          << brain.getEpisodicMemoryCount() << ", "
                          << retrieval_success_rate << "%, "
                          << total_replays << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nMemory integration simulation completed in " << duration.count() << " ms." << std::endl;
        std::cout << "Average episodes per second: " << (100.0 / (duration.count() / 1000.0)) << std::endl;
        
        // Print final memory statistics
        std::cout << "\nFinal Memory System Statistics:" << std::endl;
        std::cout << "  Working memory usage: " << brain.getWorkingMemoryUsage() << "/"
                  << brain.getWorkingMemoryCapacity() << " (" 
                  << (static_cast<double>(brain.getWorkingMemoryUsage()) / brain.getWorkingMemoryCapacity() * 100.0)
                  << "%)" << std::endl;
        std::cout << "  Episodic memories stored: " << brain.getEpisodicMemoryCount() << std::endl;
        std::cout << "  Total retrievals: " << total_retrievals << std::endl;
        std::cout << "  Successful retrievals: " << successful_retrievals << std::endl;
        std::cout << "  Retrieval success rate: " 
                  << (total_retrievals > 0 ? (static_cast<double>(successful_retrievals) / total_retrievals * 100.0) : 0.0)
                  << "%" << std::endl;
        std::cout << "  Total replay operations: " << total_replays << std::endl;
        std::cout << "  Brain firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        
        // Demonstrate associative memory patterns
        std::cout << "\nDemonstrating associative memory patterns..." << std::endl;
        
        // Create patterns for associative memory
        std::vector<std::vector<double>> patterns;
        for (int i = 0; i < 5; ++i) {
            std::vector<double> pattern(20, 0.0);
            for (int j = 0; j < 20; ++j) {
                pattern[j] = dist(gen);
            }
            patterns.push_back(pattern);
        }
        
        // Store patterns and simulate pattern completion
        for (int i = 0; i < patterns.size(); ++i) {
            // Store pattern
            brain.storeWorkingMemory(patterns[i]);
            
            // Simulate partial pattern and attempt completion
            std::vector<double> partial_pattern = patterns[i];
            std::fill(partial_pattern.begin(), partial_pattern.begin() + 10, 0.0); // First half zeroed
            
            std::vector<double> completed_pattern = brain.retrieveWorkingMemory(
                partial_pattern, 0.6
            );
            
            if (!completed_pattern.empty()) {
                double similarity = 0.0;
                for (int j = 0; j < std::min(partial_pattern.size(), completed_pattern.size()); ++j) {
                    similarity += std::abs(partial_pattern[j] - completed_pattern[j]);
                }
                similarity /= std::min(partial_pattern.size(), completed_pattern.size());
                
                std::cout << "Pattern " << i << " completion similarity: " 
                          << (1.0 - similarity) * 100.0 << "%" << std::endl;
            }
        }
        
        // Save final state
        brain.save("memory_integration_final.bin");
        std::cout << "\nMemory state saved to 'memory_integration_final.bin'" << std::endl;
        
        // Demonstrate memory consolidation
        std::cout << "\nDemonstrating memory consolidation..." << std::endl;
        
        // Create new brain and load state
        nlm::Brain brain2(config);
        brain2.load("memory_integration_final.bin");
        
        // Simulate consolidation by replaying important memories
        int consolidation_episodes = 5;
        for (int episode = 0; episode < consolidation_episodes; ++episode) {
            // Select important episode (every 10th episode from original)
            int selected_episode = episode * 10;
            
            // Replay with higher strength for consolidation
            bool consolidation_success = brain2.replayEpisodicMemory(
                selected_episode, 1.0
            );
            
            if (consolidation_success) {
                std::cout << "Consolidation of episode " << selected_episode 
                          << " successful" << std::endl;
            }
            
            // Run simulation step
            brain2.step(selected_episode + 100);
        }
        
        std::cout << "Memory consolidation completed." << std::endl;
        
        std::cout << "\n=== Memory Integration Example Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception in Memory Integration Example: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
