#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

class VisualizationDashboard {
private:
    struct VisualizationData {
        std::chrono::high_resolution_clock::time_point timestamp;
        float brainActivity;
        float firingRate;
        float novelty;
        float curiosity;
        float neuromodulation;
        float energy;
        std::vector<float> visionData;
        std::vector<float> motorOutput;
        std::string action;
        float reward;
        
        VisualizationData();
    };
    
    std::vector<VisualizationData> dataHistory;
    std::shared_ptr<Config> config;
    bool running;
    bool paused;
    bool saveToFile;
    std::string outputFile;
    int refreshRate;
    int dataPoints;
    
public:
    VisualizationDashboard();
    ~VisualizationDashboard();
    
    void run(int argc, char* argv[]);
    void printBanner();
    void showHelp();
    
private:
    void setupTerminal();
    void restoreTerminal();
    bool kbhit();
    char getch();
    void clearScreen();
    void displayHeader();
    void displaySystemInfo();
    void displayBrainActivity();
    void displayNeuromodulation();
    void displaySensoryData();
    void displayMotorOutput();
    void displayMetrics();
    void displayControls();
    void displayHelp();
    void collectData(std::shared_ptr<Brain> brain, std::shared_ptr<Vision> vision,
                   std::shared_ptr<MotorSystem> motorSystem, float reward);
    void processData();
    void saveData();
    void setupDashboardConfig();
    void runDashboard(int argc, char* argv[]);
    void dashboardLoop(std::shared_ptr<Brain> brain, std::shared_ptr<Vision> vision,
                      std::shared_ptr<MotorSystem> motorSystem);
};

} // namespace nlm
