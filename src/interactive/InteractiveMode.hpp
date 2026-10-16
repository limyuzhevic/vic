#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <algorithm>
#include <termios.h>
#include <unistd.h>

namespace nlm {

class InteractiveMode {
public:
    struct Settings {
        std::string prompt = "> ";
        bool showTimestamps = true;
        bool showProgress = true;
        bool enableColor = true;
        bool enableHistory = true;
        std::string historyFile = "interactive_history.txt";
        std::string configFile = "config.json";
        bool enableTabCompletion = true;
        bool enableCommandAutoComplete = true;
        bool useRawInput = true;
        std::vector<std::string> commandCompletions;
    };
    
    InteractiveMode(std::shared_ptr<Brain> brain, const Settings& settings = Settings());
    ~InteractiveMode();
    
    bool start();
    void stop();
    bool isRunning() const;
    
    void addCommand(const std::string& name, const std::string& description,
                   std::function<void(const std::vector<std::string>&)> handler);
    void removeCommand(const std::string& name);
    void listCommands() const;
    
    void setStatus(const std::string& status);
    void displayStatus() const;
    
    void showHelp() const;
    void showHistory() const;
    void clearScreen() const;
    void displayLogo() const;
    
    void runCommand(const std::string& commandLine);
    std::string readLine(const std::string& prompt = "> ") const;
    
    void enableProgressDisplay(bool enable);
    void updateProgress(double progress, const std::string& status = "");
    void finishProgress(const std::string& message = "");
    
    void setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    
    void saveSession(const std::string& filename) const;
    bool loadSession(const std::string& filename);
    
    void setConfigFile(const std::string& configFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm