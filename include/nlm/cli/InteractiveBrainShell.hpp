#ifndef INTERACTIVE_BRAIN_SHELL_HPP
#define INTERACTIVE_BRAIN_SHELL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <thread>
#include <atomic>

namespace nlm {
namespace cli {

class InteractiveBrainShell {
public:
    InteractiveBrainShell();
    ~InteractiveBrainShell();

    bool Initialize();
    void Run();
    void Shutdown();

    void RegisterCommand(const std::string& name, const std::string& description,
                        std::function<bool(const std::vector<std::string>&)> handler);
    bool ExecuteCommand(const std::string& commandLine);
    void DisplayPrompt() const;
    void DisplayHelp() const;

    bool IsRunning() const { return running; }
    void SetRunning(bool running) { this->running = running; }

protected:
    bool running;
    std::string currentPath;
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commandMap;
    std::vector<std::string> commandHistory;
    std::atomic<bool> stopRequested;

    bool ProcessCommand(const std::string& commandLine);
    void PrintHistory() const;
    std::vector<std::string> ParseCommandLine(const std::string& commandLine) const;
};

} // namespace cli
} // namespace nlm

#endif // INTERACTIVE_BRAIN_SHELL_HPP