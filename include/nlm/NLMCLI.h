#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

class ICommand;
class CommandRunner;
class Config;

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    static NLMCLI& getInstance();
    
    bool initialize(const std::string& configFile = "configs/default.cfg");
    int execute(int argc, char** argv);
    void runInteractive();
    
    CommandRunner& getCommandRunner();
    void registerCommand(std::unique_ptr<ICommand> command);
    
    std::vector<std::string> getCommands() const;
    std::string getCommandHelp(const std::string& commandName) const;
    bool hasCommand(const std::string& commandName) const;
    
    std::shared_ptr<Config> getConfig() const;
    void setConfig(std::shared_ptr<Config> config);
    
    bool loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath);
    
    const std::map<std::string, std::string>& getGlobalConfig() const;
    void setGlobalConfig(const std::map<std::string, std::string>& config);
    
    static std::string getVersion();
    static std::string getBuildInfo();
    static void printHelp();
    static void printVersion();
    
    bool isInitialized() const;
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

inline NLMCLI& getCLI() { return NLMCLI::getInstance(); }
inline bool initializeCLI(const std::string& configFile = "configs/default.cfg") {
    return NLMCLI::getInstance().initialize(configFile);
}
inline int executeCLI(int argc, char** argv) {
    return NLMCLI::getInstance().execute(argc, argv);
}
inline void runInteractiveCLI() {
    NLMCLI::getInstance().runInteractive();
}

} // namespace nlm