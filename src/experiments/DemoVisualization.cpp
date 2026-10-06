// Visualization Tools Demo
// This demo showcases the Visualization Tools capabilities

#include "VisualizationTools.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Experiment.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <map>
#include <string>

using namespace nlm;

class VisualizationToolsDemo {
public:
    VisualizationToolsDemo() {}
    ~VisualizationToolsDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "VISUALIZATION TOOLS DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize visualization tools
        VisualizationTools visualizer;
        
        // Create sample experimental data
        std::cout << "--- Creating Sample Experimental Data ---\n";
        
        // Create experiment
        Experiment experiment("Visualization_Study_001");
        experiment.setSeed(12345u);
        
        // Simulate experimental runs with metrics
        std::vector<ExperimentRun> experimentalData;
        int numRuns = 3;
        int stepsPerRun = 500;
        
        for (int run = 0; run < numRuns; ++run) {
            std::cout << "  Creating visualization run " << run << "...\n";
            
            ExperimentRun runData;
            runData.runId = run;
            runData.startTime = std::chrono::system_clock::now();
            
            // Create brain configuration
            auto config = createConfig(500 + run * 200, 4 + run, 0.15f + run * 0.05f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create world
            SimpleWorld world;
            world.initialize(16, 16);
            
            // Create agent
            AgentBrain agent(brain);
            agent.initialize(world);
            
            // Run simulation and collect metrics
            float totalReward = 0.0f;
            float totalFiring = 0.0f;
            
            for (int step = 0; step < stepsPerRun; ++step) {
                // Get observation
                SensoryPercept percept = world.observe(brain->getRegions()[0].get());
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action
                world.applyAction(brain->getRegions()[0].get(), cmd);
                
                // Compute reward
                float reward = world.computeReward(brain->getRegions()[0].get());
                totalReward += reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent.updateDevelopment(0.001);
                
                // Record metrics
                Experiment::StepData stepData;
                stepData.step = step;
                stepData.neuronCount = brain->getTotalNeuronCount();
                stepData.synapseCount = brain->getTotalSynapseCount();
                stepData.firingCount = brain->getFiringNeuronCount();
                stepData.avgFiringRate = brain->getAverageFiringRate();
                
                experiment.recordStep(stepData.step, stepData.neuronCount, 
                                    stepData.synapseCount, stepData.firingCount, 
                                    stepData.avgFiringRate);
                
                runData.stepData.push_back(stepData);
                
                // Record time-series metrics
                runData.metrics.push_back({"reward", reward});
                runData.metrics.push_back({"firing_rate", brain->getAverageFiringRate()});
                runData.metrics.push_back({"novelty", agent.getNoveltyLevel()});
                runData.metrics.push_back({"curiosity", agent.getCuriosityLevel()});
                runData.metrics.push_back({"dopamine", agent.getNeuromodulationLevel()});
            }
            
            // Finalize run
            runData.endTime = std::chrono::system_clock::now();
            runData.totalReward = totalReward / stepsPerRun;
            runData.averageFiringRate = totalFiring / stepsPerRun;
            
            experimentalData.push_back(runData);
            
            experiment.addNote("Run " + std::to_string(run) + " completed with average reward " + 
                            std::to_string(runData.totalReward));
        }
        
        std::cout << "✓ Created " << numRuns << " visualization datasets\n\n";
        
        // Generate brain network visualization
        std::cout << "--- Generating Brain Network Visualization ---\n";
        
        std::string networkVizPath = "/tmp/brain_network_viz.png";
        visualizer.generateBrainNetworkVisualization(brain->getRegions()[0].get(), networkVizPath, {
            {"neuron_type_color", "true"},
            {"synapse_weight", "true"},
            {"region_visualization", "true"}
        });
        std::cout << "✓ Brain network visualization generated: " << networkVizPath << "\n";
        
        // Generate neural activity heatmap
        std::cout << "--- Generating Neural Activity Heatmap ---\n";
        
        std::string activityHeatmapPath = "/tmp/neural_activity_heatmap.png";
        visualizer.generateNeuralActivityHeatmap(experimentalData, activityHeatmapPath, {
            {"time_window", "1000"},
            {"metric", "firing_rate"},
            {"visualization_type", "heatmap"},
            {"color_scale", "viridis"}
        });
        std::cout << "✓ Neural activity heatmap generated: " << activityHeatmapPath << "\n";
        
        // Generate reward learning curve
        std::cout << "--- Generating Reward Learning Curve ---\n";
        
        std::string rewardCurvePath = "/tmp/reward_learning_curve.png";
        visualizer.generateRewardLearningCurve(experimentalData, rewardCurvePath, {
            {"window_size", "50"},
            {"smoothing", "true"},
            {"show_confidence", "true"},
            {"include_baseline", "true"}
        });
        std::cout << "✓ Reward learning curve generated: " << rewardCurvePath << "\n";
        
        // Generate memory usage plot
        std::cout << "--- Generating Memory Usage Plot ---\n";
        
        std::string memoryPlotPath = "/tmp/memory_usage_plot.png";
        visualizer.generateMemoryUsagePlot(experimentalData, memoryPlotPath, {
            {"metric", "working_memory"},
            {"overlay", "episodic_memory"},
            {"time_scale", "steps"},
            {"include_capacity", "true"}
        });
        std::cout << "✓ Memory usage plot generated: " << memoryPlotPath << "\n";
        
        // Generate neuromodulation dynamics
        std::cout << "--- Generating Neuromodulation Dynamics ---\n";
        
        std::string neuromodulationPath = "/tmp/neuromodulation_dynamics.png";
        visualizer.generateNeuromodulationDynamics(experimentalData, neuromodulationPath, {
            {"metrics", "dopamine,curiosity,novelty"},
            {"time_window", "200"},
            {"smoothening", "true"},
            {"baseline_correction", "true"}
        });
        std::cout << "✓ Neuromodulation dynamics generated: " << neuromodulationPath << "\n";
        
        // Generate development progression
        std::cout << "--- Generating Development Progression ---\n";
        
        std::string developmentPath = "/tmp/development_progression.png";
        visualizer.generateDevelopmentProgression(experimentalData, developmentPath, {
            {"stages", "all"},
            {"include_metrics", "novelty,curiosity,plasticity"},
            {"smooth_transition", "true"},
            {"show_critical_periods", "true"}
        });
        std::cout << "✓ Development progression generated: " << developmentPath << "\n";
        
        // Generate multi-panel dashboard
        std::cout << "--- Generating Multi-Panel Dashboard ---\n";
        
        std::string dashboardPath = "/tmp/experimental_dashboard.png";
        visualizer.generateMultiPanelDashboard(experimentalData, dashboardPath, {
            {"panels", "performance,memory,neuromodulation,development"},
            {"layout", "grid"},
            {"refresh_rate", "30"},
            {"include_interactive", "true"}
        });
        std::cout << "✓ Multi-panel dashboard generated: " << dashboardPath << "\n";
        
        // Generate 3D visualization
        std::cout << "--- Generating 3D Visualization ---\n";
        
        std::string threeDVizPath = "/tmp/experimental_3d_viz.png";
        visualizer.generate3DVisualization(experimentalData, threeDVizPath, {
            {"projection_type", "orthographic"},
            {"view_angle", "30"},
            {"show_connections", "true"},
            {"animate_over_time", "true"}
        });
        std::cout << "✓ 3D visualization generated: " << threeDVizPath << "\n";
        
        // Generate comparison plots
        std::cout << "--- Generating Comparison Plots ---\n";
        
        // Create multiple experimental configurations for comparison
        std::vector<ExperimentRun> config1Data = experimentalData;
        std::vector<ExperimentRun> config2Data = experimentalData;
        
        // Modify config2 data for comparison
        for (auto& run : config2Data) {
            for (auto& stepData : run.stepData) {
                stepData.avgFiringRate *= 1.2f;  // Simulate different firing rate
            }
        }
        
        std::string comparisonPath = "/tmp/experimental_comparison.png";
        visualizer.generateComparisonPlots(config1Data, config2Data, comparisonPath, {
            {"metrics", "reward,firing_rate,memory_usage"},
            {"normalize", "true"},
            {"statistical_test", "true"},
            {"confidence_interval", "95"}
        });
        std::cout << "✓ Comparison plots generated: " << comparisonPath << "\n";
        
        // Generate statistical visualization
        std::cout << "--- Generating Statistical Visualization ---\n";
        
        std::string statisticalPath = "/tmp/experimental_statistics.png";
        visualizer.generateStatisticalVisualization(experimentalData, statisticalPath, {
            {"analysis_type", "ANOVA"},
            {"statistical_tests", "t-test,kruskal-wallis"},
            {"p_value_threshold", "0.05"},
            {"multiple_comparison_correction", "bonferroni"}
        });
        std::cout << "✓ Statistical visualization generated: " << statisticalPath << "\n";
        
        // Generate temporal evolution plots
        std::cout << "--- Generating Temporal Evolution Plots ---\n";
        
        std::string temporalPath = "/tmp/experimental_temporal_evolution.png";
        visualizer.generateTemporalEvolutionPlots(experimentalData, temporalPath, {
            {"time_series_length", "1000"},
            {"include_arima", "true"},
            {"forecast_length", "100"},
            {"confidence_bands", "true"}
        });
        std::cout << "✓ Temporal evolution plots generated: " << temporalPath << "\n";
        
        // Export visualization parameters
        std::cout << "--- Exporting Visualization Parameters ---\n";
        
        std::string vizParamsPath = "/tmp/visualization_parameters.json";
        visualizer.exportVisualizationParameters(vizParamsPath);
        std::cout << "✓ Visualization parameters exported to: " << vizParamsPath << "\n";
        
        // Generate animated visualization
        std::cout << "--- Generating Animated Visualization ---\n";
        
        std::string animationPath = "/tmp/experimental_animation.gif";
        visualizer.generateAnimation(experimentalData, animationPath, {
            {"duration_per_frame", "50"},
            {"fps", "30"},
            {"loop_count", "1"},
            {"include_audio", "false"}
        });
        std::cout << "✓ Animated visualization generated: " << animationPath << "\n";
        
        // Generate report with visualizations
        std::cout << "\n--- Generating Visualization Report ---\n";
        
        std::string reportPath = "/tmp/visualization_report.html";
        visualizer.generateReportWithVisualizations(experimentalData, reportPath, {
            {"template", "professional"},
            {"include_charts", "all"},
            {"interactive", "true"},
            {"export_data", "true"}
        });
        std::cout << "✓ Visualization report generated: " << reportPath << "\n";
        
        // Create presentation slides
        std::cout << "--- Creating Presentation Slides ---\n";
        
        std::string slidesPath = "/tmp/experimental_presentation.pptx";
        visualizer.createPresentationSlides(experimentalData, slidesPath, {
            {"theme", "scientific"},
            {"slide_count", "10"},
            {"include_charts", "all"},
            {"include_notes", "true"}
        });
        std::cout << "✓ Presentation slides generated: " << slidesPath << "\n";
        
        // Generate embedding visualization
        std::cout << "--- Generating Embedding Visualization ---\n";
        
        std::string embeddingPath = "/tmp/experimental_embeddings.png";
        visualizer.generateEmbeddingVisualization(experimentalData, embeddingPath, {
            {"method", "t-SNE"},
            {"perplexity", "30"},
            {"color_by", "run_id"},
            {"show_boundaries", "true"}
        });
        std::cout << "✓ Embedding visualization generated: " << embeddingPath << "\n";
        
        // Generate interactive dashboard
        std::cout << "--- Generating Interactive Dashboard ---\n";
        
        std::string dashboardHTMLPath = "/tmp/experimental_interactive_dashboard.html";
        visualizer.generateInteractiveDashboard(experimentalData, dashboardHTMLPath, {
            {"framework", "javascript"},
            {"library", "d3"},
            {"include_selection", "true"},
            {"include_filtering", "true"}
        });
        std::cout << "✓ Interactive dashboard generated: " << dashboardHTMLPath << "\n";
        
        // Generate video summary
        std::cout << "--- Generating Video Summary ---\n";
        
        std::string videoPath = "/tmp/experimental_summary_video.mp4";
        visualizer.generateVideoSummary(experimentalData, videoPath, {
            {"duration", "30"},
            {"frame_rate", "24"},
            {"resolution", "1920x1080"},
            {"include_charts", "true"},
            {"include_audio", "true"}
        });
        std::cout << "✓ Video summary generated: " << videoPath << "\n";
        
        // Generate virtual reality visualization
        std::cout << "--- Generating Virtual Reality Visualization ---\n";
        
        std::string vrPath = "/tmp/experimental_vr_visualization.zip";
        visualizer.generateVRVisualization(experimentalData, vrPath, {
            {"resolution", "1920x1080"},
            {"field_of_view", "90"},
            {"include_interactivity", "true"},
            {"export_format", "openxr"}
        });
        std::cout << "✓ VR visualization generated: " << vrPath << "\n";
        
        // Generate augmented reality markers
        std::cout << "--- Generating Augmented Reality Markers ---\n";
        
        std::string arPath = "/tmp/experimental_ar_markers.mind";
        visualizer.generateARMarkers(experimentalData, arPath, {
            {"trackable_objects", "brain,memory,neural_activity"},
            {"marker_size", "50"},
            {"output_format", "mind"}
        });
        std::cout << "✓ AR markers generated: " << arPath << "\n";
        
        // Generate 3D printable models
        std::cout << "--- Generating 3D Printable Models ---\n";
        
        std::string stlPath = "/tmp/experimental_neural_structure.stl";
        visualizer.generate3DPrintableModel(experimentalData, stlPath, {
            {"scale", "1.0"},
            {"export_format", "STL"},
            {"include_details", "true"},
            {"material", "pla"}
        });
        std::cout << "✓ 3D printable model generated: " << stlPath << "\n";
        
        // Generate API documentation
        std::cout << "--- Generating API Documentation ---\n";
        
        std::string apiDocsPath = "/tmp/visualization_api_docs.html";
        visualizer.generateAPIDocumentation(apiDocsPath, {
            {"language", "javascript"},
            {"framework", "webgl"},
            {"include_examples", "true"},
            {"include_tutorials", "true"}
        });
        std::cout << "✓ API documentation generated: " << apiDocsPath << "\n";
        
        // Generate plugin for other tools
        std::cout << "--- Generating Plugin for Other Tools ---\n";
        
        std::string pluginPath = "/tmp/visualization_plugin.py";
        visualizer.generatePluginCode(pluginPath, {
            {"language", "python"},
            {"framework", "matplotlib"},
            {"include_gui", "true"},
            {"export_format", "json"}
        });
        std::cout << "✓ Plugin generated: " << pluginPath << "\n";
        
        std::cout << "\n========================================\n";
        std::cout << "VISUALIZATION TOOLS DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
    
private:
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        return config;
    }
};

int main(int argc, char* argv[]) {
    nlm::VisualizationToolsDemo demo;
    demo.run();
    return 0;
}
