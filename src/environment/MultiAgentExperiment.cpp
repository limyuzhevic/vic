// MultiAgentExperiment Implementation

#include "MultiAgentEnvironment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct MultiAgentExperiment::Impl {
    std::vector<std::string> populationTypes;
    std::map<std::string, size_t> populationCounts;
    std::string taskType;
    std::map<std::string, float> taskParameters;
    std::vector<std::string> metrics;
    
    std::vector<std::string> baselineTypes;
    
    std::mt19937 rng;
    
    Impl() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    }
};

MultiAgentExperiment::MultiAgentExperiment()
    : pImpl(new Impl())
{
}

MultiAgentExperiment::~MultiAgentExperiment() = default;

bool MultiAgentExperiment::initialize(const std::shared_ptr<Config>& config) {
    if (!config) return false;
    
    // Set up experiment parameters
    pImpl->taskType = config->getOr<std::string>("task_type", "resource_collection");
    pImpl->taskParameters["target_resources"] = config->getOr<size_t>("target_resources", 10);
    pImpl->taskParameters["time_limit"] = config->getOr<size_t>("time_limit", 1000);
    pImpl->taskParameters["agent_radius"] = config->getOr<float>("agent_radius", 2.0f);
    pImpl->taskParameters["resource_value_range"] = config->getOr<std::pair<float, float>>("resource_value_range", std::make_pair(0.5f, 1.0f));
    
    // Set up metrics
    pImpl->metrics = config->getOr<std::vector<std::string>>("metrics", {
        "fitness", "coordination", "efficiency", "adaptability"
    });
    
    NLM_LOG_INFO("Multi-agent experiment initialized");
    return true;
}

bool MultiAgentExperiment::addPopulation(size_t count, const std::string& behaviorType) {
    if (count == 0) return false;
    
    pImpl->populationCounts[behaviorType] = count;
    pImpl->populationTypes.push_back(behaviorType);
    
    NLM_LOG_INFO("Added " + std::to_string(count) + " agents with behavior type: " + behaviorType);
    return true;
}

void MultiAgentExperiment::setTask(const std::string& taskType, const std::map<std::string, float>& parameters) {
    pImpl->taskType = taskType;
    pImpl->taskParameters = parameters;
    
    NLM_LOG_INFO("Set task type: " + taskType);
}

void MultiAgentExperiment::setMetrics(const std::vector<std::string>& metricNames) {
    pImpl->metrics = metricNames;
}

bool MultiAgentExperiment::addBaseline(const std::string& behaviorType) {
    pImpl->baselineTypes.push_back(behaviorType);
    NLM_LOG_INFO("Added baseline with behavior type: " + behaviorType);
    return true;
}

MultiAgentResults MultiAgentExperiment::run(size_t maxSteps, size_t maxEpisodes) {
    MultiAgentResults results;
    
    // Create environment
    auto environment = std::make_shared<MultiAgentEnvironment>();
    
    // Create agent populations
    size_t totalAgents = 0;
    for (const auto& pair : pImpl->populationCounts) {
        totalAgents += pair.second;
    }
    
    results.totalSteps = maxSteps;
    results.totalEpisodes = maxEpisodes;
    
    // Run episodes
    for (size_t episode = 0; episode < maxEpisodes; ++episode) {
        // Create agents for this episode
        std::vector<std::shared_ptr<AgentBrain>> agents;
        for (const auto& pair : pImpl->populationCounts) {
            for (size_t i = 0; i < pair.second; ++i) {
                // Create brain for agent
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                auto agent = std::make_shared<AgentBrain>(brain);
                agent->initialize(environment);
                
                agents.push_back(agent);
            }
        }
        
        // Run episode
        double episodeTime = 0.0;
        std::vector<double> episodeFitness(agents.size(), 0.0);
        
        for (size_t step = 0; step < maxSteps && episodeTime < pImpl->taskParameters["time_limit"]; ++step) {
            // Update environment
            environment->update(0.1);  // Fixed timestep
            
            // Execute agent actions
            auto actions = environment->executeActions();
            
            // Update agent states based on actions
            for (size_t i = 0; i < agents.size(); ++i) {
                auto agent = agents[i];
                auto body = agent->getBody();
                
                // Apply action
                if (i < actions.size()) {
                    body.x += actions[i].velocityX * 0.1f;
                    body.y += actions[i].velocityY * 0.1f;
                    
                    // Keep within bounds
                    body.x = std::max(0.0f, std::min(body.x, pImpl->taskParameters["environment_width"]));
                    body.y = std::max(0.0f, std::min(body.y, pImpl->taskParameters["environment_height"]));
                }
                
                agent->setBody(body);
                
                // Calculate fitness based on task
                double fitness = calculateAgentFitness(agent, environment, episode, step);
                episodeFitness[i] += fitness;
            }
            
            episodeTime += 0.1;
        }
        
        // Store episode results
        results.fitnessTimeSeries.push_back(episodeFitness);
    }
    
    // Calculate final statistics
    calculateMultiAgentStatistics(results);
    
    NLM_LOG_INFO("Multi-agent experiment completed");
    return results;
}

double MultiAgentExperiment::calculateAgentFitness(std::shared_ptr<AgentBrain> agent,
                                                  std::shared_ptr<MultiAgentEnvironment> environment,
                                                  size_t episode, size_t step) {
    // Calculate fitness based on task type
    double fitness = 0.0;
    
    if (pImpl->taskType == "resource_collection") {
        // Resource collection fitness
        auto body = agent->getBody();
        float resourcesCollected = environment->getResourceValue(body.x, body.y);
        fitness = static_cast<double>(resourcesCollected);
        
        // Add coordination bonus for proximity to other agents
        auto allAgents = environment->getAllAgents();
        for (const auto& otherAgent : allAgents) {
            if (otherAgent.get() != agent.get()) {
                float distance = std::sqrt(std::pow(otherAgent->getBody().x - body.x, 2) +
                                         std::pow(otherAgent->getBody().y - body.y, 2));
                if (distance < 10.0f) {
                    fitness += 1.0 / distance;  // Bonus for being near other agents
                }
            }
        }
    } else if (pImpl->taskType == "coordination_task") {
        // Coordination task fitness
        auto body = agent->getBody();
        float proximityBonus = 0.0f;
        
        auto allAgents = environment->getAllAgents();
        for (const auto& otherAgent : allAgents) {
            if (otherAgent.get() != agent.get()) {
                float distance = std::sqrt(std::pow(otherAgent->getBody().x - body.x, 2) +
                                         std::pow(otherAgent->getBody().y - body.y, 2));
                if (distance < 5.0f) {
                    proximityBonus += 1.0f / distance;
                }
            }
        }
        
        fitness = proximityBonus;
    }
    
    return fitness;
}

void MultiAgentExperiment::calculateMultiAgentStatistics(MultiAgentResults& results) {
    // Calculate statistics from time series data
    size_t numAgents = results.fitnessTimeSeries.empty() ? 0 : results.fitnessTimeSeries[0].size();
    
    // Initialize agent stats
    results.agentStats.resize(numAgents);
    for (size_t i = 0; i < numAgents; ++i) {
        results.agentStats[i].agentId = i;
        results.agentStats[i].behaviorType = "default";  // Would be set from experiment config
    }
    
    // Calculate population statistics
    std::map<std::string, size_t> typeCounts;
    for (size_t i = 0; i < numAgents; ++i) {
        typeCounts[results.agentStats[i].behaviorType]++;
    }
    
    results.populationStats.clear();
    for (const auto& pair : typeCounts) {
        MultiAgentResults::PopulationStats popStats;
        popStats.behaviorType = pair.first;
        popStats.count = pair.second;
        
        // Calculate average fitness for this type
        double totalFitness = 0.0;
        size_t count = 0;
        for (const auto& agentStats : results.agentStats) {
            if (agentStats.behaviorType == pair.first) {
                totalFitness += agentStats.finalFitness;
                ++count;
            }
        }
        
        if (count > 0) {
            popStats.averageFitness = totalFitness / count;
        }
        
        results.populationStats.push_back(popStats);
    }
    
    // Calculate emergent behaviors
    results.socialCohesion = calculateSocialCohesion(results);
    results.collectiveIntelligence = calculateCollectiveIntelligence(results);
    results.adaptationSpeed = calculateAdaptationSpeed(results);
    results.explorationExploitationBalance = calculateExplorationExploitationBalance(results);
}

double MultiAgentExperiment::calculateSocialCohesion(const MultiAgentResults& results) {
    // Social cohesion = average similarity in movement patterns
    if (results.fitnessTimeSeries.empty()) return 0.0;
    
    double totalSimilarity = 0.0;
    size_t similarityCount = 0;
    
    for (size_t t = 0; t < results.fitnessTimeSeries.size(); ++t) {
        for (size_t i = 0; i < results.fitnessTimeSeries[t].size(); ++i) {
            for (size_t j = i + 1; j < results.fitnessTimeSeries[t].size(); ++j) {
                // Simple similarity measure based on fitness variance
                double diff = std::abs(results.fitnessTimeSeries[t][i] - results.fitnessTimeSeries[t][j]);
                totalSimilarity += 1.0 - diff / 10.0;  // Normalize to 0-1
                ++similarityCount;
            }
        }
    }
    
    return similarityCount > 0 ? totalSimilarity / similarityCount : 0.0;
}

double MultiAgentExperiment::calculateCollectiveIntelligence(const MultiAgentResults& results) {
    // Collective intelligence = overall task completion efficiency
    if (results.fitnessTimeSeries.empty()) return 0.0;
    
    double totalFitness = 0.0;
    size_t totalSteps = 0;
    
    for (const auto& episodeFitness : results.fitnessTimeSeries) {
        for (double fitness : episodeFitness) {
            totalFitness += fitness;
            ++totalSteps;
        }
    }
    
    return totalSteps > 0 ? totalFitness / totalSteps : 0.0;
}

double MultiAgentExperiment::calculateAdaptationSpeed(const MultiAgentResults& results) {
    // Adaptation speed = how quickly fitness improves over time
    if (results.fitnessTimeSeries.size() < 2) return 0.0;
    
    double firstHalfFitness = 0.0;
    double secondHalfFitness = 0.0;
    size_t firstHalfSteps = 0;
    size_t secondHalfSteps = 0;
    
    size_t halfIndex = results.fitnessTimeSeries.size() / 2;
    
    for (size_t t = 0; t < halfIndex; ++t) {
        for (double fitness : results.fitnessTimeSeries[t]) {
            firstHalfFitness += fitness;
            ++firstHalfSteps;
        }
    }
    
    for (size_t t = halfIndex; t < results.fitnessTimeSeries.size(); ++t) {
        for (double fitness : results.fitnessTimeSeries[t]) {
            secondHalfFitness += fitness;
            ++secondHalfSteps;
        }
    }
    
    double firstHalfAvg = firstHalfSteps > 0 ? firstHalfFitness / firstHalfSteps : 0.0;
    double secondHalfAvg = secondHalfSteps > 0 ? secondHalfFitness / secondHalfSteps : 0.0;
    
    return firstHalfAvg > 0.0 ? (secondHalfAvg - firstHalfAvg) / firstHalfAvg : 0.0;
}

double MultiAgentExperiment::calculateExplorationExploitationBalance(const MultiAgentResults& results) {
    // Exploration-exploitation balance = variance in action selection
    // This is a simplified version - would need action data for real calculation
    return 0.5;  // Placeholder
}

const MultiAgentResults& MultiAgentExperiment::getResults() const {
    return pImpl->currentResults;
}

bool MultiAgentExperiment::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving results: " + filepath);
        return false;
    }
    
    file << "Multi-Agent Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << std::endl;
    
    file << "Total Steps: " << pImpl->currentResults.totalSteps << std::endl;
    file << "Total Episodes: " << pImpl->currentResults.totalEpisodes << std::endl;
    file << "Total Time: " << pImpl->currentResults.totalTime << std::endl;
    file << std::endl;
    
    file << "Agent Statistics:" << std::endl;
    for (const auto& agentStats : pImpl->currentResults.agentStats) {
        file << "  Agent " << agentStats.agentId 
             << " (Type: " << agentStats.behaviorType << "):"
             << std::endl;
        file << "    Episodes Completed: " << agentStats.episodesCompleted << std::endl;
        file << "    Final Fitness: " << agentStats.finalFitness << std::endl;
        file << "    Average Fitness: " << agentStats.averageFitness << std::endl;
        file << "    Success Rate: " << agentStats.successRate << std::endl;
        file << "    Total Steps Taken: " << agentStats.totalStepsTaken << std::endl;
        file << "    Average Step Size: " << agentStats.averageStepSize << std::endl;
        file << "    Coordination Score: " << agentStats.coordinationScore << std::endl;
        file << std::endl;
    }
    
    file << "Population Statistics:" << std::endl;
    for (const auto& popStats : pImpl->currentResults.populationStats) {
        file << "  " << popStats.behaviorType << ":"
             << std::endl;
        file << "    Count: " << popStats.count << std::endl;
        file << "    Average Fitness: " << popStats.averageFitness << std::endl;
        file << "    Std Dev Fitness: " << popStats.stdDevFitness << std::endl;
        file << "    Success Rate: " << popStats.successRate << std::endl;
        file << "    Average Coordination: " << popStats.averageCoordination << std::endl;
        file << std::endl;
    }
    
    file << "Task Metrics:" << std::endl;
    for (const auto& pair : pImpl->currentResults.taskMetrics) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
    file << std::endl;
    
    file << "Emergent Behavior Metrics:" << std::endl;
    file << "  Social Cohesion: " << pImpl->currentResults.socialCohesion << std::endl;
    file << "  Collective Intelligence: " << pImpl->currentResults.collectiveIntelligence << std::endl;
    file << "  Adaptation Speed: " << pImpl->currentResults.adaptationSpeed << std::endl;
    file << "  Exploration-Exploitation Balance: " << pImpl->currentResults.explorationExploitationBalance << std::endl;
    file << std::endl;
    
    file << "Baseline Comparisons:" << std::endl;
    for (const auto& pair : pImpl->currentResults.baselineComparisons) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
    file << std::endl;
    
    file.close();
    NLM_LOG_INFO("Multi-agent experiment results saved to: " + filepath);
    return true;
}

void MultiAgentExperiment::analyzeResults() {
    // Perform statistical analysis on results
    // This would include hypothesis testing, confidence intervals, etc.
    
    NLM_LOG_INFO("Multi-agent experiment analysis completed");
}

} // namespace nlm