#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <random>

namespace nlm {

class ParameterTuner {
private:
    struct ParameterRange {
        std::string name;
        double minValue;
        double maxValue;
        double stepSize;
        std::string distribution;
        double normalMean;
        double normalStddev;
        
        ParameterRange(const std::string& n, double minV, double maxV, 
                      double step = 0.1, const std::string& dist = "uniform",
                      double mean = 0.0, double stddev = 1.0);
    };
    
    struct ExperimentResult {
        std::vector<double> parameters;
        double fitnessScore;
        std::chrono::high_resolution_clock::duration executionTime;
        bool converged;
        
        ExperimentResult(const std::vector<double>& params, double fitness,
                        const std::chrono::high_resolution_clock::duration& time,
                        bool conv = false);
    };
    
    std::shared_ptr<Config> config;
    std::vector<ParameterRange> parameterRanges;
    int populationSize;
    int maxGenerations;
    double mutationRate;
    double crossoverRate;
    bool useTournamentSelection;
    int tournamentSize;
    std::string optimizationGoal;
    bool verbose;
    bool saveResults;
    std::mt19937 rng;
    
public:
    ParameterTuner();
    ~ParameterTuner();
    
    void run(int argc, char* argv[]);
    void printBanner();
    
private:
    void setupParameterRanges();
    void setupEvolutionConfig();
    void setupTunerConfig();
    std::vector<std::vector<double>> initializePopulation();
    double evaluateFitness(const std::vector<double>& parameters);
    std::vector<double> tournamentSelection(const std::vector<std::vector<double>>& population,
                                           const std::vector<double>& fitnesses);
    std::vector<double> crossover(const std::vector<double>& parent1,
                                const std::vector<double>& parent2);
    std::vector<double> mutate(const std::vector<double>& individual);
    std::vector<double> generateRandomParameters();
    ExperimentResult runGeneticAlgorithm();
    std::vector<ExperimentResult> runParameterSearch();
    void printResults(const ExperimentResult& result);
    void printPopulationStats(const std::vector<ExperimentResult>& results);
    void saveResultsToFile(const ExperimentResult& result, const std::string& filename);
    void exportBestParametersToConfig(const std::vector<double>& bestParameters);
    void runParameterTuner(int argc, char* argv[]);
    void showHelp();
    void showCurrentConfig();
};

} // namespace nlm
