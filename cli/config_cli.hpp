// NLM CLI Config Interface
// Configuration management CLI interface

#ifndef CONFIG_CLI_HPP
#define CONFIG_CLI_HPP

#include <string>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {
namespace cli {

class ConfigCLI {
public:
    ConfigCLI();
    ~ConfigCLI() = default;
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    void printUsage() const;
    
    int handleShowCommand(int argc, char** argv);
    int handleSetCommand(int argc, char** argv);
    int handleLoadCommand(int argc, char** argv);
    int handleSaveCommand(int argc, char** argv);
    int handleValidateCommand(int argc, char** argv);
    int handleExportCommand(int argc, char** argv);
    int handleImportCommand(int argc, char** argv);
    int handleListCommand(int argc, char** argv);
    
    struct ConfigEntry;
    ConfigEntry* getConfigEntry(std::shared_ptr<Config> config, const std::string& key);
};

} // namespace cli
} // namespace nlm

#endif // CONFIG_CLI_HPP
