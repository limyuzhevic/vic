// Config CLI Tools
#ifndef NLM_CONFIG_CLI_H
#define NLM_CONFIG_CLI_H

#include <map>
#include <string>

class ConfigCli {
public:
    static void validateConfig(const std::map<std::string, std::string>& options);
    static void generateConfig(const std::map<std::string, std::string>& options);
    static void compareConfigs(const std::map<std::string, std::string>& options);
    static void mergeConfigs(const std::map<std::string, std::string>& options);
    static void exportConfig(const std::map<std::string, std::string>& options);
    static void importConfig(const std::map<std::string, std::string>& options);
    static void validateKeys(const std::map<std::string, std::string>& options);
    static void checkValues(const std::map<std::string, std::string>& options);
};

#endif // NLM_CONFIG_CLI_H
