#include "ActivityRenderer.hpp"
#include "ColorManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_set>

namespace nlm {

ActivityRenderer::ActivityRenderer()
    : displayMode_(DisplayMode::RasterPlot), analysisType_(AnalysisType::FiringRate),
      timeWindow_(10.0f), showSpikes_(true), showHistograms_(true), showStatistics_(true),
      updateInterval_(0.1f), selectedNeuron_(0), timeOffset_(0.0f), currentTime_(0.0f),
      maxSpikeHistory_(1000), maxActivityHistory_(1000), smoothActivity_(true),
      smoothingFactor_(0.3f), colorScheme_("default") {
    initializeDefaultConfiguration();
}

void ActivityRenderer::initializeDefaultConfiguration() {
    // Initialize default rendering parameters
    maxSpikeHistory_ = 1000;
    maxActivityHistory_ = 1000;
    smoothActivity_ = true;
    smoothingFactor_ = 0.3f;
    colorScheme_ = "default";
}

void ActivityRenderer::render(const Brain& brain) {
    // Collect current data
    collectSpikeData(brain);
    collectActivityData(brain);
    updateStatistics();
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Render title
    ColorManager colorManager;
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightGreen) << "=== NEURAL ACTIVITY VISUALIZATION ===" << "\033[0m" << std::endl;
    
    // Render based on display mode
    switch (displayMode_) {
        case DisplayMode::RasterPlot:
            renderRasterPlot(brain);
            break;
        case DisplayMode::Histogram:
            renderFiringRates(brain);
            break;
        case DisplayMode::Heatmap:
            renderActivityHeatmap(brain);
            break;
        case DisplayMode::Timeline:
            renderTimeSeries(brain);
            break;
        case DisplayMode::CrossSection:
            renderCrossSection(brain);
            break;
        case DisplayMode::NetworkActivity:
            renderSpikes(brain);
            break;
        case DisplayMode::StatisticalOverlay:
            renderStatisticalOverlays(brain);
            break;
    }
    
    // Render additional elements
    if (showSpikes_) {
        renderSpikes(brain);
    }
    
    if (showHistograms_) {
        renderFiringRates(brain);
    }
    
    if (showStatistics_) {
        renderStatisticalPlots();
    }
    
    // Render controls
    renderControls();
}

void ActivityRenderer::update(const Brain& brain) {
    currentTime_ += updateInterval_;
    collectSpikeData(brain);
    updateStatistics();
}

void ActivityRenderer::collectSpikeData(const Brain& brain) {
    // Extract spike data from brain
    auto spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) return;
    
    // This is a simplified approach - in a real implementation,
    // you would need to access the actual spike events from the spike system
    // For now, we'll simulate spike data based on firing neuron count
    
    size_t firingNeurons = brain.getFiringNeuronCount();
    size_t totalNeurons = brain.getTotalNeuronCount();
    
    if (firingNeurons > 0 && totalNeurons > 0) {
        // Simulate spike distribution
        size_t samples = std::min(maxSpikeHistory_, firingNeurons * 5);
        Timestamp currentTime = static_cast<Timestamp>(brain.getCurrentStep()) * 0.001f;
        
        for (size_t i = 0; i < samples; ++i) {
            size_t neuronId = rand() % totalNeurons;
            float spikeTime = currentTime - (rand() % static_cast<int>(timeWindow_ * 1000)) / 1000.0f;
            
            spikeData_.push_back(SpikeData(neuronId, spikeTime, brain.getCurrentStep()));
            neuronSpikeTrains_[neuronId].push_back(spikeData_.back());
        }
        
        // Trim old data
        if (spikeData_.size() > maxSpikeHistory_) {
            spikeData_.erase(spikeData_.begin(), spikeData_.begin() + 
                           (spikeData_.size() - maxSpikeHistory_));
        }
    }
}

void ActivityRenderer::collectActivityData(const Brain& brain) {
    // Collect activity data from brain regions
    auto regions = brain.getRegions();
    
    // Clear previous activity data
    neuronActivity_.clear();
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        
        for (const auto* neuron : neurons) {
            size_t neuronId = neuron->getId().index();
            
            // Simulate activity based on firing state and potential
            float activity = 0.0f;
            if (neuron->isFiring()) {
                activity = 1.0f;
            }
            
            float membranePotential = neuron->getState().membranePotential;
            float restingPotential = neuron->getState().restingPotential;
            activity += std::abs((membranePotential - restingPotential) / 50.0f);
            
            // Smooth the activity
            if (smoothActivity_ && !neuronActivity_[neuronId].empty()) {
                activity = smoothingFactor_ * activity + (1.0f - smoothingFactor_) * neuronActivity_[neuronId].back();
            }
            
            neuronActivity_[neuronId].push_back(activity);
            
            // Trim old data
            if (neuronActivity_[neuronId].size() > maxActivityHistory_) {
                neuronActivity_[neuronId].erase(neuronActivity_[neuronId].begin());
            }
        }
    }
    
    // Update time series
    updateTimeSeries();
}

void ActivityRenderer::updateTimeSeries() {
    // Update rolling time series statistics
    size_t dataPoints = std::min(maxActivityHistory_, static_cast<size_t>(timeWindow_ * 10.0f)); // 10 points per second
    
    timePoints_.clear();
    meanActivity_.clear();
    varianceActivity_.clear();
    firingRate_.clear();
    
    for (size_t i = 0; i < dataPoints; ++i) {
        timePoints_.push_back(static_cast<Timestamp>(i) * 0.1f); // 100ms spacing
        
        // Calculate current statistics from collected data
        float mean = calculateMean(neuronActivity_.size() > 0 ? neuronActivity_.begin()->second : std::vector<float>());
        meanActivity_.push_back(mean);
        
        float variance = calculateVariance(neuronActivity_.size() > 0 ? neuronActivity_.begin()->second : std::vector<float>(), mean);
        varianceActivity_.push_back(variance);
        
        // Simple firing rate approximation
        float firingRate = static_cast<float>(spikeData_.size()) / timeWindow_;
        firingRate_.push_back(firingRate);
    }
}

void ActivityRenderer::calculateStatistics() {
    // Calculate various activity statistics
    analyzeFiringRate(spikeData_);
    analyzeSynchrony(spikeData_);
    analyzeBursting(spikeData_);
    analyzeCorrelations(spikeData_);
}

void ActivityRenderer::renderSpikes(const Brain& brain) {
    // Render spike raster plot
    ColorManager colorManager;
    
    int plotHeight = 20;
    int plotWidth = std::min(80, static_cast<int>(spikeData_.size()));
    
    // Draw axes
    for (int y = 0; y < plotHeight; ++y) {
        std::cout << " ";
        for (int x = 0; x < plotWidth; ++x) {
            std::cout << "-";
        }
        std::cout << "\n";
    }
    
    // Draw spike points
    for (const auto& spike : spikeData_) {
        int x = static_cast<int>((timeOffset_ + spike.time) / timeWindow_ * plotWidth);
        int y = plotHeight - static_cast<int>((spike.neuronId % 10) / 10.0f * plotHeight);
        
        if (x >= 0 && x < plotWidth && y >= 0 && y < plotHeight) {
            std::cout << colorManager.getSpikeColor(spike);
            std::cout << "*";
            std::cout << "\033[0m";
        }
    }
}

void ActivityRenderer::renderSpikeTrains(const Brain& brain) {
    // Render individual neuron spike trains
    ColorManager colorManager;
    
    int trainHeight = 15;
    int trainWidth = 60;
    
    for (const auto& neuronPair : neuronSpikeTrains_) {
        size_t neuronId = neuronPair.first;
        const auto& spikes = neuronPair.second;
        
        if (spikes.empty()) continue;
        
        // Draw spike train
        for (size_t i = 0; i < spikes.size() && i < static_cast<size_t>(trainWidth); ++i) {
            const SpikeData& spike = spikes[i];
            
            int x = static_cast<int>((timeOffset_ + spike.time) / timeWindow_ * trainWidth);
            int y = trainHeight - static_cast<int>((neuronId % 10) / 10.0f * trainHeight);
            
            if (x >= 0 && x < trainWidth && y >= 0 && y < trainHeight) {
                std::string color = colorManager.getSpikeColor(spike);
                if (neuronId == selectedNeuron_) {
                    color = colorManager.getColorCode(ColorManager::Color::BrightYellow);
                }
                
                std::cout << color << "|" << "\033[0m";
            }
        }
        
        std::cout << "\n";
    }
}

void ActivityRenderer::renderRasterPlot(const Brain& brain) {
    // Raster plot showing spike times vs neuron IDs
    ColorManager colorManager;
    
    int plotHeight = 25;
    int plotWidth = 70;
    
    // Draw axes
    std::cout << "Y:";
    for (int y = 0; y < plotHeight; y += 5) {
        std::cout << " ";
        for (int x = 0; x < plotWidth; ++x) std::cout << "-";
        std::cout << "\n   ";
    }
    
    std::cout << "   ";
    for (int x = 0; x < plotWidth; ++x) {
        if (x % 10 == 0) std::cout << x / 10;
        else std::cout << " ";
    }
    std::cout << "\n   Time:";
    
    // Draw spikes
    for (const auto& spike : spikeData_) {
        int x = static_cast<int>((timeOffset_ + spike.time) / timeWindow_ * plotWidth);
        int y = plotHeight - static_cast<int>((spike.neuronId % 25) / 25.0f * plotHeight);
        
        if (x >= 0 && x < plotWidth && y >= 0 && y < plotHeight) {
            std::cout << colorManager.getSpikeColor(spike);
            std::cout << "*";
            std::cout << "\033[0m";
        }
    }
}

void ActivityRenderer::renderFiringRates(const Brain& brain) {
    // Render firing rate histogram
    ColorManager colorManager;
    
    int histHeight = 15;
    int histWidth = 50;
    
    // Calculate firing rates for each neuron
    std::map<size_t, float> firingRates;
    for (const auto& neuronPair : neuronActivity_) {
        size_t neuronId = neuronPair.first;
        const auto& activity = neuronPair.second;
        
        float meanActivity = calculateMean(activity);
        firingRates[neuronId] = meanActivity * 10.0f; // Convert to Hz
    }
    
    // Find max rate for scaling
    float maxRate = 0.0f;
    for (const auto& rate : firingRates) {
        maxRate = std::max(maxRate, rate);
    }
    
    // Draw histogram
    for (int y = histHeight - 1; y >= 0; --y) {
        for (int x = 0; x < histWidth; ++x) {
            size_t neuronId = x;
            if (firingRates.find(neuronId) != firingRates.end()) {
                float rate = firingRates[neuronId];
                float normalized = rate / maxRate;
                
                if (y < normalized * histHeight) {
                    std::string color = colorManager.getFiringRateColor(rate);
                    std::cout << color << "#" << "\033[0m";
                } else {
                    std::cout << " ";
                }
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

void ActivityRenderer::renderActivityHeatmap(const Brain& brain) {
    // Render 2D heatmap of neuron activity
    ColorManager colorManager;
    
    int heatmapSize = 30;
    
    // Create activity grid
    std::vector<std::vector<float>> activityGrid(heatmapSize, std::vector<float>(heatmapSize, 0.0f));
    
    // Fill grid with activity data
    size_t gridIndex = 0;
    for (const auto& neuronPair : neuronActivity_) {
        if (gridIndex >= static_cast<size_t>(heatmapSize * heatmapSize)) break;
        
        int row = gridIndex / heatmapSize;
        int col = gridIndex % heatmapSize;
        
        float totalActivity = 0.0f;
        for (float activity : neuronPair.second) {
            totalActivity += activity;
        }
        
        if (!neuronPair.second.empty()) {
            activityGrid[row][col] = totalActivity / neuronPair.second.size();
        }
        
        gridIndex++;
    }
    
    // Render heatmap
    for (int y = heatmapSize - 1; y >= 0; --y) {
        for (int x = 0; x < heatmapSize; ++x) {
            float activity = activityGrid[y][x];
            std::string color = colorManager.getActivityGradient(activity, 0.0f, 1.0f);
            std::cout << color << "#" << "\033[0m";
        }
        std::cout << "\n";
    }
}

void ActivityRenderer::renderTimeSeries(const Brain& brain) {
    // Render time series of neural activity
    ColorManager colorManager;
    
    int tsHeight = 15;
    int tsWidth = 60;
    
    if (timePoints_.empty()) return;
    
    // Find min/max for scaling
    float minVal = *std::min_element(meanActivity_.begin(), meanActivity_.end());
    float maxVal = *std::max_element(meanActivity_.begin(), meanActivity_.end());
    
    if (maxVal == minVal) {
        maxVal = minVal + 1.0f;
    }
    
    // Draw time series
    for (int y = tsHeight - 1; y >= 0; --y) {
        for (int x = 0; x < tsWidth; ++x) {
            if (x < static_cast<int>(timePoints_.size())) {
                float value = meanActivity_[x];
                float normalized = (value - minVal) / (maxVal - minVal);
                
                if (y < normalized * tsHeight) {
                    std::string color = colorManager.getActivityGradient(normalized, 0.0f, 1.0f);
                    std::cout << color << "*" << "\033[0m";
                } else {
                    std::cout << " ";
                }
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    // Draw time axis
    std::cout << "\n   Time: 0.0s";
    for (int i = 1; i < tsWidth; ++i) {
        if (i % 10 == 0) {
            std::cout << " " << i * timeWindow_ / 10.0f << "s";
        } else {
            std::cout << "   ";
        }
    }
}

void ActivityRenderer::renderCrossSection(const Brain& brain) {
    // Render cross-sectional view of brain activity
    ColorManager colorManager;
    
    int sectionHeight = 25;
    
    // Simulate cross-section through brain
    for (int y = sectionHeight - 1; y >= 0; --y) {
        for (int x = 0; x < 40; ++x) {
            // Create a simple gradient pattern
            float activity = static_cast<float>(y) / sectionHeight;
            std::string color = colorManager.getActivityGradient(activity, 0.0f, 1.0f);
            
            // Add some noise based on actual brain data
            float noise = static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f;
            float finalActivity = std::max(0.0f, std::min(1.0f, activity + noise));
            
            std::cout << color << "#" << "\033[0m";
        }
        std::cout << "\n";
    }
}

void ActivityRenderer::renderStatisticalOverlays(const Brain& brain) {
    // Render statistical overlays
    ColorManager colorManager;
    
    int plotHeight = 20;
    int plotWidth = 50;
    
    // Plot firing rate distribution
    if (!spikeData_.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "Firing Rate Distribution:" << "\033[0m" << std::endl;
        
        // Simple histogram of firing rates
        for (int y = plotHeight - 1; y >= 0; --y) {
            for (int x = 0; x < plotWidth; ++x) {
                // Use spike data to create distribution
                float histValue = 0.0f;
                if (!spikeData_.empty()) {
                    histValue = static_cast<float>(spikeData_.size()) / plotWidth;
                }
                
                if (y < histValue * plotHeight) {
                    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "*" << "\033[0m";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
        }
    }
}

void ActivityRenderer::renderStatisticalPlots() const {
    // Render statistical plots
    ColorManager colorManager;
    std::string statColor = colorManager.getColorCode(ColorManager::Color::White);
    
    std::cout << statColor << "=== STATISTICAL SUMMARY ===" << "\033[0m" << std::endl;
    std::cout << statColor << "Mean Firing Rate: " << firingRateStats_.mean << " Hz" << "\033[0m" << std::endl;
    std::cout << statColor << "Std Dev: " << firingRateStats_.stddev << " Hz" << "\033[0m" << std::endl;
    std::cout << statColor << "Skewness: " << firingRateStats_.skewness << "\033[0m" << std::endl;
    std::cout << statColor << "Kurtosis: " << firingRateStats_.kurtosis << "\033[0m" << std::endl;
    std::cout << statColor << "Active Neurons: " << "NA" << "\033[0m" << std::endl;
    std::cout << statColor << "Total Spikes: " << spikeData_.size() << "\033[0m" << std::endl;
}

void ActivityRenderer::analyzeFiringRate(const std::vector<SpikeData>& spikes) {
    // Analyze firing rate statistics
    if (spikes.empty()) return;
    
    std::vector<float> rates;
    for (const auto& spike : spikes) {
        rates.push_back(1.0f); // Simplified - each spike has rate 1.0
    }
    
    firingRateStats_.mean = calculateMean(rates);
    firingRateStats_.variance = calculateVariance(rates, firingRateStats_.mean);
    firingRateStats_.stddev = std::sqrt(firingRateStats_.variance);
    firingRateStats_.skewness = calculateSkewness(rates, firingRateStats_.mean, firingRateStats_.variance);
    firingRateStats_.kurtosis = calculateKurtosis(rates, firingRateStats_.mean, firingRateStats_.variance);
    
    // Find min/max (simplified)
    firingRateStats_.min = 0;
    firingRateStats_.max = static_cast<size_t>(rates.size());
}

void ActivityRenderer::analyzeSynchrony(const std::vector<SpikeData>& spikes) {
    // Analyze spike synchrony (simplified)
    synchronyStats_.mean = 0.3f; // Placeholder
    synchronyStats_.stddev = 0.1f;
    synchronyStats_.skewness = 0.0f;
    synchronyStats_.kurtosis = 0.0f;
}

void ActivityRenderer::analyzeBursting(const std::vector<SpikeData>& spikes) {
    // Analyze bursting patterns (simplified)
    burstingStats_.mean = 0.5f; // Placeholder
    burstingStats_.stddev = 0.2f;
    burstingStats_.skewness = 0.0f;
    burstingStats_.kurtosis = 0.0f;
}

void ActivityRenderer::analyzeCorrelations(const std::vector<SpikeData>& spikes) {
    // Analyze correlations between neurons (simplified)
    // Placeholder implementation
}

void ActivityRenderer::renderControls() const {
    ColorManager colorManager;
    std::string helpColor = colorManager.getColorCode(ColorManager::Color::BrightYellow);
    
    std::cout << "\033[2J\033[H"; // Clear screen (temporary)
    
    std::cout << helpColor << "=== ACTIVITY VISUALIZATION CONTROLS ===" << "\033[0m" << std::endl;
    std::cout << helpColor << "1-6: Change Display Mode (Raster/Histogram/Heatmap/Timeline/Cross-Section/Network)" << "\033[0m" << std::endl;
    std::cout << helpColor << "S: Toggle Spikes" << "\033[0m" << std::endl;
    std::cout << helpColor << "H: Toggle Histograms" << "\033[0m" << std::endl;
    std::cout << helpColor << "T: Toggle Statistics" << "\033[0m" << std::endl;
    std::cout << helpColor << "A: Change Analysis (Firing Rate/Synchrony/Bursting/Correlations)" << "\033[0m" << std::endl;
    std::cout << helpColor << "W: Adjust Time Window" << "\033[0m" << std::endl;
    std::cout << helpColor << "P: Pause Update" << "\033[0m" << std::endl;
    std::cout << helpColor << "Q: Quit" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Show current settings
    std::cout << helpColor << "Display: " << static_cast<int>(displayMode_) 
              << "  Analysis: " << static_cast<int>(analysisType_)
              << "  Time Window: " << timeWindow_ << "s" << "\033[0m" << std::endl;
}

float ActivityRenderer::calculateMean(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

float ActivityRenderer::calculateVariance(const std::vector<float>& data, float mean) const {
    if (data.empty()) return 0.0f;
    
    float sumSquares = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return sumSquares / data.size();
}

float ActivityRenderer::calculateSkewness(const std::vector<float>& data, float mean, float variance) const {
    if (data.empty() || variance == 0.0f) return 0.0f;
    
    float skew = 0.0f;
    size_t n = data.size();
    
    for (float value : data) {
        float diff = value - mean;
        skew += (diff * diff * diff) / n;
    }
    
    skew /= std::cbrt(variance * variance);
    return skew;
}

float ActivityRenderer::calculateKurtosis(const std::vector<float>& data, float mean, float variance) const {
    if (data.empty() || variance == 0.0f) return 0.0f;
    
    float kurt = 0.0f;
    size_t n = data.size();
    
    for (float value : data) {
        float diff = value - mean;
        float diff4 = diff * diff * diff * diff;
        kurt += diff4 / n;
    }
    
    kurt /= (variance * variance);
    return kurt - 3.0f; // Excess kurtosis
}

} // namespace nlm
