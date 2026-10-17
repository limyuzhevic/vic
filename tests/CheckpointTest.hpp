#pragma once

#include <string>
#include <map>
#include <vector>
#include <future>
#include "../performance/CheckpointSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class CheckpointTest {
public:
    CheckpointTest();
    ~CheckpointTest();
    
    // Run all checkpoint tests
    void runAllTests();
    
    // Test categories
    void runSaveLoadRoundtripTests();
    void runValidationTests();
    void runCorruptionRecoveryTests();
    void runIncrementalCheckpointingTests();
    void runCompressionTests();
    void runDeduplicationTests();
    void runParallelCheckpointTests();
    void runVersioningTests();
    void runManagerIntegrationTests();
    void runPerformanceTests();
    
    // Test utilities
    bool verifyStateConsistency(const Brain& brain1, const Brain& brain2);
    
private:
    // Helper functions for compression tests
    static bool compressData(const std::vector<uint8_t>& input, 
                           std::vector<uint8_t>& output, 
                           CompressionLevel level);
    
    static bool decompressData(const std::vector<uint8_t>& input, 
                             std::vector<uint8_t>& output, 
                             CompressionLevel level);
    
    static std::string compressionToString(CompressionLevel level);
    
    // Test results storage
    std::map<std::string, bool> testResults;
};

} // namespace nlm