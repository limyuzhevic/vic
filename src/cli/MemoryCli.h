// Memory CLI Tools
#ifndef NLM_MEMORY_CLI_H
#define NLM_MEMORY_CLI_H

#include <map>
#include <string>

class MemoryCli {
public:
    static void monitorMemory(const std::map<std::string, std::string>& options);
    static void profileMemory(const std::map<std::string, std::string>& options);
    static void detectMemoryLeaks(const std::map<std::string, std::string>& options);
    static void optimizeMemory(const std::map<std::string, std::string>& options);
    static void takeMemorySnapshot(const std::map<std::string, std::string>& options);
    static void compareMemory(const std::map<std::string, std::string>& options);
    static void generateMemoryReport(const std::map<std::string, std::string>& options);
    static void traceMemoryAllocation(const std::map<std::string, std::string>& options);
};

#endif // NLM_MEMORY_CLI_H
