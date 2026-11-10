// Brain CLI Tools
#ifndef NLM_BRAIN_CLI_H
#define NLM_BRAIN_CLI_H

#include <map>
#include <string>

class BrainCli {
public:
    static void createBrain(const std::map<std::string, std::string>& options);
    static void loadBrain(const std::map<std::string, std::string>& options);
    static void saveBrain(const std::map<std::string, std::string>& options);
    static void inspectBrain(const std::map<std::string, std::string>& options);
    static void simulateBrain(const std::map<std::string, std::string>& options);
    static void debugBrain(const std::map<std::string, std::string>& options);
    static void analyzeBrain(const std::map<std::string, std::string>& options);
    static void exportBrain(const std::map<std::string, std::string>& options);
    static void compareBrains(const std::map<std::string, std::string>& options);
};

#endif // NLM_BRAIN_CLI_H
