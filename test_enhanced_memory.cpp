#include <iostream>
#include "../memory_management/memory_manager.h"
#include "../checkpoint_management/checkpoint_manager.h"
#include "../memory_monitoring/memory_usage_monitor.h"
#include "../backup_management/backup_system.h"

int main() {
    std::cout << "=== Testing Enhanced Memory Management and Checkpoint Systems ===\n" << std::endl;
    
    try {
        // Test 1: Memory Manager
        std::cout << "Test 1: Memory Manager" << std::endl;
        nlm::MemoryManager memoryManager;
        
        // Test basic working memory operations
        memoryManager.setWorkingMemoryCapacity(100);
        memoryManager.storeToWorkingMemory(1, 0.5f);
        memoryManager.storeToWorkingMemory(2, 0.8f);
        
        float value1 = memoryManager.retrieveFromWorkingMemory(1);
        float value2 = memoryManager.retrieveFromWorkingMemory(2);
        
        std::cout << "  Stored and retrieved values: " << value1 << ", " << value2 << std::endl;
        
        // Test episodic memory
        nlm::AdvancedEpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.reward = 0.9f;
        memoryManager.storeToEpisodicMemory(episode);
        
        const nlm::AdvancedEpisodicMemoryItem* retrieved = memoryManager.retrieveFromEpisodicMemory(1);
        if (retrieved) {
            std::cout << "  Episodic memory stored and retrieved successfully" << std::endl;
        }
        
        // Test associative memory
        nlm::AdvancedPattern pattern1({0.1f, 0.2f, 0.3f});
        nlm::AdvancedPattern pattern2({0.4f, 0.5f, 0.6f});
        memoryManager.storeAssociation(pattern1, pattern2, 0.7f);
        
        std::vector<float> recalled = memoryManager.recallPattern(pattern1);
        std::cout << "  Associative memory recall: " << recalled.size() << " patterns" << std::endl;
        
        // Test memory statistics
        nlm::MemoryStatistics stats = memoryManager.getMemoryStatistics();
        std::cout << "  Memory statistics: " << stats.usedMemory << " bytes used, " << 
                     stats.efficiency * 100 << "% efficiency" << std::endl;
        
        // Test 2: Checkpoint Manager
        std::cout << "\nTest 2: Checkpoint Manager" << std::endl;
        nlm::CheckpointManager checkpointManager;
        
        checkpointManager.setCheckpointDirectory("./checkpoints");
        checkpointManager.setCheckpointFrequency(100);
        checkpointManager.setCheckpointCompression(true);
        
        std::string checkpointId = checkpointManager.createCheckpoint("Test checkpoint");
        std::cout << "  Created checkpoint: " << checkpointId << std::endl;
        
        bool restored = checkpointManager.restoreCheckpoint(checkpointId);
        std::cout << "  Restored checkpoint: " << (restored ? "true" : "false") << std::endl;
        
        checkpointManager.setCompressionAlgorithm(nlm::CompressionAlgorithm::Gzip);
        std::vector<char> compressed = checkpointManager.compressData(std::vector<char>{'t', 'e', 's', 't'}, nlm::CompressionAlgorithm::Gzip);
        std::cout << "  Compression test: compressed " << 4 << " bytes to " << compressed.size() << " bytes" << std::endl;
        
        // Test 3: Memory Usage Monitor
        std::cout << "\nTest 3: Memory Usage Monitor" << std::endl;
        nlm::MemoryUsageMonitor memoryMonitor;
        
        memoryMonitor.startTracking();
        memoryMonitor.trackMemoryAllocation(1024, "test_alloc");
        memoryMonitor.trackMemoryDeallocation(512, "test_alloc");
        
        nlm::MemoryStatistics monitorStats = memoryMonitor.getCurrentMemoryStats();
        std::cout << "  Memory usage stats: " << monitorStats.usedMemory << " bytes used" << std::endl;
        
        std::vector<std::string> recommendations = memoryMonitor.getOptimizationRecommendations();
        std::cout << "  Optimization recommendations: " << recommendations.size() << std::endl;
        
        // Test 4: Backup System
        std::cout << "\nTest 4: Backup System" << std::endl;
        nlm::BackupSystem backupSystem;
        
        backupSystem.setBackupDirectory("./backups");
        backupSystem.setBackupEncryption(false);
        
        std::string backupId = backupSystem.createBackup("Test backup");
        std::cout << "  Created backup: " << backupId << std::endl;
        
        bool restoredBackup = backupSystem.restoreBackup(backupId);
        std::cout << "  Restored backup: " << (restoredBackup ? "true" : "false") << std::endl;
        
        bool backupValid = backupSystem.validateBackup(backupId);
        std::cout << "  Backup validation: " << (backupValid ? "valid" : "invalid") << std::endl;
        
        std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
