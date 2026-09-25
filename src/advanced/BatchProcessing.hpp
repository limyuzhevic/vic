#ifndef NLM_BATCH_PROCESSING_HPP
#define NLM_BATCH_PROCESSING_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

namespace nlm {

class BatchProcessor {
public:
    static void processBatch(const std::string& configFile);
    static bool loadConfigFromFile(const std::string& filepath, std::shared_ptr<Config>& config);
    static void runSimulationFromConfig(const std::string& configPath, size_t index, size_t total);
};

} // namespace nlm

#endif // NLM_BATCH_PROCESSING_HPP