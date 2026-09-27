#ifndef BRAIN_COMMAND_LINE_INTERFACE_HPP
#define BRAIN_COMMAND_LINE_INTERFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {
namespace cli {

class BrainCommandLineInterface {
public:
    BrainCommandLineInterface();
    virtual ~BrainCommandLineInterface();

    bool Initialize(int argc, char* argv[]);
    bool Run();
    void Shutdown();

    void RegisterCommand(const std::string& name, 
                        const std::string& description,
                        std::function<bool(const std::vector<std::string>&)> handler);
    bool ExecuteCommand(const std::string& command, const std::vector<std::string>& args);
    void DisplayHelp() const;
    void DisplayVersion() const;

    const std::string& GetConfigPath() const { return configPath; }
    const std::string& GetOutputPath() const { return outputPath; }
    const std::string& GetLogLevel() const { return logLevel; }

protected:
    virtual bool OnInitialize() = 0;
    virtual bool OnRun() = 0;
    virtual void OnShutdown() = 0;

    std::string configPath;
    std::string outputPath;
    std::string logLevel;
    bool running;
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commandMap;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_COMMAND_LINE_INTERFACE_HPP