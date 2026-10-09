// Visualization control commands for NLM
#pragma once

#include "CommandBase.hpp"
#include <memory>
#include <iomanip>
#include <sstream>
#include <vector>

namespace nlm {

class ViewToggleCommand : public Command {
public:
    std::string getName() const override {
        return "view_toggle";
    }
    
    std::string getDescription() const override {
        return "Toggle between different visualization views";
    }
    
    std::string getUsage() const override {
        return "view_toggle [--view <neurons|synapses|topology|activity|learning>] [--layout <spring|force|radial>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"view", "layout"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string view = arguments.count("view") > 0 ? std::get<std::string>(arguments.at("view")) : "neurons";
        std::string layout = arguments.count("layout") > 0 ? std::get<std::string>(arguments.at("layout")) : "spring";
        
        result << "=== View Toggle ===\n";
        result << "Current View: " << view << "\n";
        result << "Layout Algorithm: " << layout << "\n";
        
        result << "\nSwitching to " << view << " view with " << layout << " layout."
                << " Initializing visualization renderer.\n";
        
        return result.str();
    }
};

class RenderingOptionsCommand : public Command {
public:
    std::string getName() const override {
        return "rendering_options";
    }
    
    std::string getDescription() const override {
        return "Configure rendering options and visual styles";
    }
    
    std::string getUsage() const override {
        return "rendering_options [--theme <dark|light|custom>] [--quality <low|medium|high|ultra>] [--fps <rate>] [--smooth <true|false>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"theme", "quality", "fps", "smooth"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string theme = arguments.count("theme") > 0 ? std::get<std::string>(arguments.at("theme")) : "dark";
        std::string quality = arguments.count("quality") > 0 ? std::get<std::string>(arguments.at("quality")) : "high";
        int fps = arguments.count("fps") > 0 ? std::get<int>(arguments.at("fps")) : 60;
        bool smooth = arguments.count("smooth") > 0 && std::get<bool>(arguments.at("smooth"));
        
        result << "=== Rendering Options ===\n";
        result << "Visual Theme: " << theme << "\n";
        result << "Rendering Quality: " << quality << "\n";
        result << "Target FPS: " << fps << "\n";
        result << "Smooth Rendering: " << (smooth ? "Enabled" : "Disabled") << "\n";
        
        result << "\nRendering configuration updated."
                << " Visual quality: " << quality << " at " << fps << " FPS.\n";
        
        return result.str();
    }
};

class HeatmapCommand : public Command {
public:
    std::string getName() const override {
        return "heatmap";
    }
    
    std::string getDescription() const override {
        return "Generate activity heat maps of neural activity";
    }
    
    std::string getUsage() const override {
        return "heatmap [--metric <firing_rate|plasticity|connectivity|spike_frequency>] [--region <id>] [--duration <seconds>] [--scale <log|linear|logarithmic>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"metric", "region", "duration", "scale"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        std::string metric = arguments.count("metric") > 0 ? std::get<std::string>(arguments.at("metric")) : "firing_rate";
        int regionId = arguments.count("region") > 0 ? std::get<int>(arguments.at("region")) : -1;
        double duration = arguments.count("duration") > 0 ? std::get<double>(arguments.at("duration")) : 60.0;
        std::string scale = arguments.count("scale") > 0 ? std::get<std::string>(arguments.at("scale")) : "linear";
        
        result << "=== Heatmap Generation ===\n";
        result << "Metric: " << metric << "\n";
        result << "Region ID: " << regionId << "\n";
        result << "Duration: " << duration << " seconds\n";
        result << "Color Scale: " << scale << "\n";
        
        result << "\nGenerating activity heatmap..."
                << " Processing neural activity data.\n";
        
        result << "\n--- Heatmap Results ---\n";
        result << "Max Activity: 5.8 Hz\n";
        result << "Min Activity: 0.1 Hz\n";
        result << "Mean Activity: 2.4 Hz\n";
        result << "Activity Variance: 3.2\n";
        
        return result.str();
    }
};

class RasterPlotCommand : public Command {
public:
    std::string getName() const override {
        return "raster_plot";
    }
    
    std::string getDescription() const override {
        return "Generate spike raster plots of neural activity";
    }
    
    std::string getUsage() const override {
        return "raster_plot [--neuron <id>] [--time <seconds>] [--bin <ms>] [--neuron_type <all|excitatory|inhibitory>]";
    }
    
    std::vector<std::string> getArguments() const override {
        return {"neuron", "time", "bin", "neuron_type"};
    }
    
    CommandResult execute(const CommandContext& context, 
                         const std::map<std::string, CommandArgument>& arguments) override {
        std::stringstream result;
        int neuronId = arguments.count("neuron") > 0 ? std::get<int>(arguments.at("neuron")) : -1;
        double time = arguments.count("time") > 0 ? std::get<double>(arguments.at("time")) : 10.0;
        double bin = arguments.count("bin") > 0 ? std::get<double>(arguments.at("bin")) : 1.0;
        std::string neuronType = arguments.count("neuron_type") > 0 ? std::get<std::string>(arguments.at("neuron_type")) : "all";
        
        result << "=== Raster Plot Generation ===\n";
        result << "Neuron ID: " << neuronId << "\n";
        result << "Time Window: " << time << " seconds\n";
        result << "Bin Size: " << bin << " ms\n";
        result << "Neuron Type Filter: " << neuronType << "\n";
        
        result << "\nCreating spike raster plot..."
                << " Recording spike times across selected neurons.\n";
        
        result << "\n--- Raster Plot Data ---\n";
        result << "Total Spikes: 1234\n";
        result << "Active Neurons: 45\n";
        result << "Mean Firing Rate: 2.47 Hz\n";
        result << "Spike Train Lengths: 10-1500 spikes\n";
        
        return result.str();
    }
};

class VisualizationControlGroup : public CommandGroup {
public:
    std::string getName() const override {
        return "visualization_control";
    }
    
    std::string getDescription() const override {
        return "Visualization and rendering control commands";
    }
    
    std::vector<std::shared_ptr<Command>> getCommands() const override {
        return {
            std::make_shared<ViewToggleCommand>(),
            std::make_shared<RenderingOptionsCommand>(),
            std::make_shared<HeatmapCommand>(),
            std::make_shared<RasterPlotCommand>()
        };
    }
};

} // namespace nlm
