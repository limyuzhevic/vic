#!/usr/bin/env bash
# NLM Phase 6 Visualization Tool
# Comprehensive visualization system for NLM analysis and debugging

# Set up visualization environment
VIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$VIS_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Visualization configuration
VIS_MODES=("neural" "memory" "neuromodulation" "development" "combined")
OUTPUT_FORMATS=("ascii" "svg" "json" "html")
DEFAULT_MODE="combined"
DEFAULT_FORMAT="ascii"
DEFAULT_WIDTH=80
DEFAULT_HEIGHT=40

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[VIS]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[VIS]${NC} $1"
}

log_error() {
    echo -e "${RED}[VIS]${NC} $1"
}

log_header() {
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}========================================${NC}"
}

# Check if NLM visualization tools are available
check_visualization_tools() {
    # Check for available visualization tools
    local available_tools=()
    
    if command -v python3 &> /dev/null; then
        available_tools+=("python")
    fi
    
    if command -v node &> /dev/null; then
        available_tools+=("node")
    fi
    
    if command -v gnuplot &> /dev/null; then
        available_tools+=("gnuplot")
    fi
    
    if [[ ${#available_tools[@]} -eq 0 ]]; then
        log_warn "No visualization tools found. Python recommended for detailed visualizations."
        log_info "Available tools: ${available_tools[*]}"
    else
        log_info "Available visualization tools: ${available_tools[*]}"
    fi
    
    # Store available tools for later use
    VIS_TOOLS=("${available_tools[@]}")
}

# Generate ASCII neural visualization
generate_ascii_neural() {
    local neurons=$1
    local spike_counts=$2
    local width=${3:-$DEFAULT_WIDTH}
    local height=${4:-$DEFAULT_HEIGHT}
    
    log_info "Generating ASCII neural visualization for $neurons neurons..."
    
    # Create ASCII grid representation
    local grid_width=$((width / 10))
    local grid_height=$((height / 10))
    
    # Initialize grid with neurons
    local grid=()
    for ((i=0; i<grid_height; i++)); do
        local row=""
        for ((j=0; j<grid_width; j++)); do
            row+="·"
        done
        grid+=("$row")
    done
    
    # Place neurons on grid (simplified representation)
    local neuron_count=$((neurons / 100))
    if [[ $neuron_count -gt 0 ]]; then
        for ((i=0; i<neuron_count && i<grid_height/2; i++)); do
            local row=$((i + grid_height/4))
            local col=$((i % grid_width))
            if [[ $row -lt $grid_height ]]; then
                # Convert to ASCII representation
                local new_row=""
                for ((c=0; c<grid_width; c++)); do
                    if [[ $c -eq $col ]]; then
                        local activity=$(( (RANDOM % 100) / spike_counts ))
                        if [[ $activity -gt 80 ]]; then
                            new_row+="●"
                        elif [[ $activity -gt 50 ]]; then
                            new_row+="○"
                        elif [[ $activity -gt 20 ]]; then
                            new_row+="·"
                        else
                            new_row+=" "
                        fi
                    else
                        new_row+="${grid[$row]:$c}"
                    fi
                done
                grid[$row]="$new_row"
            fi
        done
    fi
    
    # Print ASCII visualization
    echo "Neural Activity Visualization (Neurons: $neurons)"
    echo "Spike Activity: $spike_counts"
    echo
    for row in "${grid[@]}"; do
        echo "$row"
    done
    echo
}

# Generate memory visualization
generate_ascii_memory() {
    local episode_count=$1
    local working_memory=$2
    local width=${3:-$DEFAULT_WIDTH}
    
    log_info "Generating ASCII memory visualization..."
    
    echo "Memory System Visualization"
    echo "======================="
    echo
    echo "Episodic Memory Episodes: $episode_count"
    echo "Working Memory Active Traces: $working_memory"
    echo
    
    # Episodic memory visualization
    if [[ $episode_count -gt 0 ]]; then
        echo "Episodic Memory Timeline:"
        for ((i=0; i<min($episode_count, 20); i++)); do
            local intensity=$(( (i * 100) / max($episode_count, 1) ))
            local bar_length=$(( (width * intensity) / 100 ))
            local bar=""
            for ((j=0; j<bar_length; j++)); do
                bar+="█"
            done
            echo "$bar Episode $((i+1))"
        done
        if [[ $episode_count -gt 20 ]]; then
            echo "... and $((episode_count - 20)) more episodes"
        fi
    else
        echo "No episodic memories stored"
    fi
    
    echo
    
    # Working memory visualization
    echo "Working Memory Traces:"
    if [[ $working_memory -gt 0 ]]; then
        for ((i=0; i<min($working_memory, 10); i++)); do
            local bar_length=$(( (width * (10-i)) / 10 ))
            local bar=""
            for ((j=0; j<bar_length; j++)); do
                bar+="▌"
            done
            echo "$bar Trace $((i+1))"
        done
        if [[ $working_memory -gt 10 ]]; then
            echo "... and $((working_memory - 10)) more traces"
        fi
    else
        echo "No active working memory traces"
    fi
}

# Generate neuromodulation visualization
generate_ascii_neuromodulation() {
    local dopamine=$1
    local curiosity=$2
    local novelty=$3
    
    log_info "Generating ASCII neuromodulation visualization..."
    
    echo "Neuromodulation System Visualization"
    echo "=================================="
    echo
    
    # Dopamine visualization
    echo "Dopamine (Reward/Reinforcement):"
    local dopamine_bar_length=$(( (60 * $dopamine) / 100 ))
    local dopamine_bar=""
    for ((i=0; i<dopamine_bar_length; i++)); do
        dopamine_bar+="█"
    done
    local empty_dopamine_bar=""
    for ((i=0; i<(60 - dopamine_bar_length); i++)); do
        empty_dopamine_bar+="░"
    done
    echo "[$dopamine_bar$empty_dopamine_bar] ${dopamine} (${dopamine}%")"
    echo "Function: Reinforces rewarded behaviors"
    echo
    
    # Curiosity visualization
    echo "Curiosity (Exploration Motivation):"
    local curiosity_bar_length=$(( (60 * $curiosity) / 100 ))
    local curiosity_bar=""
    for ((i=0; i<curiosity_bar_length; i++)); do
        curiosity_bar+="█"
    done
    local empty_curiosity_bar=""
    for ((i=0; i<(60 - curiosity_bar_length); i++)); do
        empty_curiosity_bar+="░"
    done
    echo "[$curiosity_bar$empty_curiosity_bar] ${curiosity} (${curiosity}%")"
    echo "Function: Drives exploration and novel experiences"
    echo
    
    # Novelty visualization
    echo "Novelty Detection (Surprise):"
    local novelty_bar_length=$(( (60 * $novelty) / 100 ))
    local novelty_bar=""
    for ((i=0; i<novelty_bar_length; i++)); do
        novelty_bar+="█"
    done
    local empty_novelty_bar=""
    for ((i=0; i<(60 - novelty_bar_length); i++)); do
        empty_novelty_bar+="░"
    done
    echo "[$novelty_bar$empty_novelty_bar] ${novelty} (${novelty}%")"
    echo "Function: Detects unexpected patterns and events"
    echo
    
    # Overall neuromodulation status
    local total_neuromodulation=$((dopamine + curiosity + novelty))
    echo "Overall Neuromodulation Level: ${total_neuromodulation} / 300 (${total_neuromodulation} * 100 = ${total_neuromodulation} %)"
    echo "Balance: Dopamine=${dopamine}, Curiosity=${curiosity}, Novelty=${novelty}"
}

# Generate development visualization
generate_ascii_development() {
    local stage=$1
    local age=$2
    local plasticity=$3
    
    log_info "Generating ASCII development visualization..."
    
    echo "Development System Visualization"
    echo "=============================="
    echo
    echo "Developmental Stage: $stage"
    echo "Developmental Age: $age time units"
    echo "Plasticity Modifier: $plasticity"
    echo
    
    # Developmental stage visualization
    echo "Developmental Stage Progress:"
    local stages=("Embryonic" "Juvenile" "Adult" "Mature" "Expert")
    local stage_names=("Basic neural circuits" "Sensory processing" "Cognitive abilities" "Complex behavior" "Specialized skills")
    
    for ((i=0; i<5; i++)); do
        local bar_length=$(( (width * ((i+1) * 20)) / 100 ))
        local bar=""
        for ((j=0; j<bar_length; j++)); do
            if [[ $i -lt $stage ]]; then
                bar+="█"
            else
                bar+="░"
            fi
        done
        echo "$bar ${stages[$i]}"
        if [[ $i -lt $stage ]]; then
            echo "    $ ${stage_names[$i]}"
        fi
    done
    echo
    
    # Age visualization
    echo "Developmental Timeline:"
    local age_bar_length=$(( (width * min($age, 100)) / 100 ))
    local age_bar=""
    for ((i=0; i<age_bar_length; i++)); do
        age_bar+="█"
    done
    local remaining_bar=""
    for ((i=age_bar_length; i<width; i++)); do
        remaining_bar+="░"
    done
    echo "[$age_bar$remaining_bar] Age: $age/100"
    echo
    
    # Plasticity visualization
    echo "Plasticity Rate: ${plasticity} x baseline"
    if [[ $plasticity -gt 1.5 ]]; then
        echo "Status: HIGH (rapid learning, potential instability)"
    elif [[ $plasticity -gt 1.0 ]]; then
        echo "Status: NORMAL (balanced learning)"
    elif [[ $plasticity -gt 0.5 ]]; do
        echo "Status: LOW (stable, slow learning)"
    else
        echo "Status: VERY LOW (rigid, minimal adaptation)"
    fi
}

# Generate combined visualization
generate_ascii_combined() {
    local neurons=$1
    local spikes=$2
    local episodes=$3
    local dopamine=$4
    local curiosity=$5
    local novelty=$6
    local stage=$7
    local working_memory=$8
    
    log_info "Generating combined ASCII visualization..."
    
    echo "NLM Phase 6 Comprehensive Visualization"
    echo "====================================="
    echo
    
    # System status
    echo "=== SYSTEM STATUS ==="
    echo "Neural Network: $neurons neurons, $spikes total spikes"
    echo "Memory Systems: $episodes episodes, $working_memory working traces"
    echo "Neuromodulation: DA=$dopamine, Cu=$curiosity, Nv=$novelty"
    echo "Development: Stage $stage, Plasticity active"
    echo
    
    # Activity indicators
    echo "=== ACTIVITY INDICATORS ==="
    if [[ $spikes -gt 1000 ]]; then
        echo "🔥 HIGH: Network very active"
    elif [[ $spikes -gt 100 ]]; then
        echo "⚡ MEDIUM: Network moderately active"
    else
        echo "💤 LOW: Network quiet"
    fi
    
    if [[ $episodes -gt 10 ]]; then
        echo "🧠 RICH: Extensive episodic memory"
    elif [[ $episodes -gt 0 ]]; then
        echo "📝 SOME: Some episodic memories"
    else
        echo "📭 EMPTY: No episodic memories"
    fi
    
    local total_neuromodulation=$((dopamine + curiosity + novelty))
    if [[ $total_neuromodulation -gt 150 ]]; then
        echo "🎯 HIGH: Strong neuromodulation"
    elif [[ $total_neuromodulation -gt 50 ]]; then
        echo "🎯 MODERATE: Moderate neuromodulation"
    else
        echo "🎯 LOW: Weak neuromodulation"
    fi
    
    if [[ $stage -ge 3 ]]; then
        echo "🌟 MATURE: Well-developed cognitive abilities"
    elif [[ $stage -ge 2 ]]; then
        echo "🌱 DEVELOPING: Growing cognitive abilities"
    else
        echo "👶 INFINITE: Early development"
    fi
}

# Generate JSON data for export
generate_json_output() {
    local mode=$1
    local neurons=$2
    local spikes=$3
    local episodes=$4
    local dopamine=$5
    local curiosity=$6
    local novelty=$7
    local stage=$8
    local working_memory=$9
    local output_file=${10:-"nlm_visualization.json"}
    
    log_info "Generating JSON output: $output_file"
    
    local json_data="{"
    json_data+="\n  \"timestamp\": \"$(date -Iseconds)\","
    json_data+="\n  \"mode\": \"$mode\","
    json_data+="\n  \"neural_network\": {"
    json_data+="\n    \"neurons\": $neurons,"
    json_data+="\n    \"total_spikes\": $spikes"
    json_data+="\n  },"
    json_data+="\n  \"memory_systems\": {"
    json_data+="\n    \"episodes\": $episodes,"
    json_data+="\n    \"working_memory_traces\": $working_memory"
    json_data+="\n  },"
    json_data+="\n  \"neuromodulation\": {"
    json_data+="\n    \"dopamine\": $dopamine,"
    json_data+="\n    \"curiosity\": $curiosity,"
    json_data+="\n    \"novelty\": $novelty"
    json_data+="\n  },"
    json_data+="\n  \"development\": {"
    json_data+="\n    \"stage\": $stage,"
    json_data+="\n    \"plasticity_enabled\": true"
    json_data+="\n  }"
    json_data+="\n}"
    
    echo "$json_data" > "$output_file"
    log_info "JSON data exported to: $output_file"
}

# Generate HTML visualization
generate_html_output() {
    local mode=$1
    local neurons=$2
    local spikes=$3
    local episodes=$4
    local dopamine=$5
    local curiosity=$6
    local novelty=$7
    local stage=$8
    local working_memory=$9
    local output_file=${10:-"nlm_visualization.html"}
    
    log_info "Generating HTML output: $output_file"
    
    cat > "$output_file" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>NLM Phase 6 Visualization</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .header {
            text-align: center;
            color: #333;
            border-bottom: 2px solid #4CAF50;
            padding-bottom: 10px;
            margin-bottom: 20px;
        }
        .section {
            margin: 20px 0;
            padding: 15px;
            border: 1px solid #ddd;
            border-radius: 5px;
            background-color: #fafafa;
        }
        .metric {
            display: inline-block;
            margin: 10px;
            padding: 10px;
            background-color: #e3f2fd;
            border-radius: 5px;
            text-align: center;
            min-width: 150px;
        }
        .metric h3 {
            margin: 0 0 5px 0;
            color: #1976d2;
        }
        .metric .value {
            font-size: 24px;
            font-weight: bold;
            color: #0d47a1;
        }
        .status-indicator {
            display: inline-block;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            margin: 0 5px;
        }
        .high { background-color: #4caf50; }
        .medium { background-color: #ff9800; }
        .low { background-color: #f44336; }
        .timeline {
            width: 100%;
            height: 30px;
            background-color: #e0e0e0;
            border-radius: 15px;
            margin: 10px 0;
        }
        .timeline-fill {
            height: 100%;
            background-color: #4caf50;
            border-radius: 15px;
            text-align: center;
            line-height: 30px;
            color: white;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Phase 6 Comprehensive Visualization</h1>
            <p>Generated on $(date)</p>
        </div>

        <div class="section">
            <h2>System Overview</h2>
            <div class="metric">
                <h3>Neural Network</h3>
                <div class="value">$neurons</div>
                <p>Neurons</p>
            </div>
            <div class="metric">
                <h3>Total Spikes</h3>
                <div class="value">$spikes</div>
                <p>Activity Count</p>
            </div>
            <div class="metric">
                <h3>Episodic Memory</h3>
                <div class="value">$episodes</div>
                <p>Episodes Stored</p>
            </div>
            <div class="metric">
                <h3>Working Memory</h3>
                <div class="value">$working_memory</div>
                <p>Active Traces</p>
            </div>
        </div>

        <div class="section">
            <h2>Neuromodulation System</h2>
            <div class="metric">
                <h3>Dopamine</h3>
                <div class="value" style="color: #4caf50;">$dopamine</div>
                <p>Reward Processing</p>
            </div>
            <div class="metric">
                <h3>Curiosity</h3>
                <div class="value" style="color: #ff9800;">$curiosity</div>
                <p>Exploration Drive</p>
            </div>
            <div class="metric">
                <h3>Novelty</h3>
                <div class="value" style="color: #2196f3;">$novelty</div>
                <p>Surprise Detection</p>
            </div>
            <div class="metric">
                <h3>Overall Level</h3>
                <div class="value">$((dopamine + curiosity + novelty))</div>
                <p>Combined Influence</p>
            </div>
        </div>

        <div class="section">
            <h2>Development Stage</h2>
            <p>Current Stage: <strong>$stage</strong></p>
            <div class="timeline">
                <div class="timeline-fill" style="width: $((stage * 20))%">$stage/5</div>
            </div>
            <p>Developmental progress through neural maturation phases</p>
        </div>

        <div class="section">
            <h2>System Status Indicators</h2>
            <p>
                <span class="status-indicator high"></span>
                <strong>Network Activity:</strong> $([[ $spikes -gt 1000 ]] && echo "HIGH" || echo "NORMAL")
            </p>
            <p>
                <span class="status-indicator [[ $episodes -gt 10 ]] && echo high || echo medium]]"></span>
                <strong>Memory richness:</strong> $([[ $episodes -gt 10 ]] && echo "RICH" || echo "MODERATE")
            </p>
            <p>
                <span class="status-indicator [[ $((dopamine + curiosity + novelty)) -gt 150 ]] && echo high || echo low]]"></span>
                <strong>Neuromodulation:</strong> $([[ $((dopamine + curiosity + novelty)) -gt 150 ]] && echo "STRONG" || echo "WEAK")
            </p>
            <p>
                <span class="status-indicator [[ $stage -ge 3 ]] && echo high || echo medium]]"></span>
                <strong>Development:</strong> $([[ $stage -ge 3 ]] && echo "MATURE" || echo "DEVELOPING")
            </p>
        </div>

        <div class="section">
            <h2>Technical Information</h2>
            <p><strong>Generated:</strong> $(date)</p>
            <p><strong>Mode:</strong> $mode</p>
            <p><strong>Visualization Type:</strong> ASCII-based text visualization</p>
            <p><strong>Purpose:</strong> NLM Phase 6 integration test visualization and analysis</p>
        </div>
    </div>
</body>
</html>
EOF
    
    log_info "HTML visualization exported to: $output_file"
}

# Main visualization execution
main() {
    # Parse command line arguments
    local mode="$DEFAULT_MODE"
    local format="$DEFAULT_FORMAT"
    local width="$DEFAULT_WIDTH"
    local height="$DEFAULT_HEIGHT"
    local output_file=""
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --mode|-m)
                mode="$2"
                shift 2
                ;;
            --format|-f)
                format="$2"
                shift 2
                ;;
            --width|-w)
                width="$2"
                shift 2
                ;;
            --height|-h)
                height="$2"
                shift 2
                ;;
            --output|-o)
                output_file="$2"
                shift 2
                ;;
            --neurons)
                neurons="$2"
                shift 2
                ;;
            --spikes)
                spikes="$2"
                shift 2
                ;;
            --episodes)
                episodes="$2"
                shift 2
                ;;
            --dopamine)
                dopamine="$2"
                shift 2
                ;;
            --curiosity)
                curiosity="$2"
                shift 2
                ;;
            --novelty)
                novelty="$2"
                shift 2
                ;;
            --stage)
                stage="$2"
                shift 2
                ;;
            --working-memory)
                working_memory="$2"
                shift 2
                ;;
            --help|-h)
                log_header "NLM Phase 6 Visualization Tool"
                cat << EOF
Usage: $0 [OPTIONS]

Visualization Modes:
  neural         Neural activity visualization (default)
  memory        Memory systems visualization
  neuromodulation Neuromodulation system visualization
  development   Development system visualization
  combined      All systems combined visualization

Output Formats:
  ascii         ASCII text visualization (default)
  html         HTML web visualization
  json         JSON data export

Examples:
  $0                                    Show combined ASCII visualization
  $0 --mode neural --format html        Show neural activity in HTML
  $0 --mode memory --format json         Export memory data as JSON
  $0 --width 120 --height 60             Custom visualization dimensions

For more information, see the README or documentation.
EOF
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Check visualization tools
    check_visualization_tools
    
    # Validate mode
    local mode_valid=false
    for vis_mode in "${VIS_MODES[@]}"; do
        if [[ "$mode" == "$vis_mode" ]]; then
            mode_valid=true
            break
        fi
    done
    
    if [[ "$mode_valid" == false ]]; then
        log_error "Invalid mode: $mode"
        log_info "Valid modes: ${VIS_MODES[*]}"
        exit 1
    fi
    
    # Set default values if not provided via command line
    local neurons=${neurons:-500}
    local spikes=${spikes:-0}
    local episodes=${episodes:-0}
    local dopamine=${dopamine:-50}
    local curiosity=${curiosity:-30}
    local novelty=${novelty:-20}
    local stage=${stage:-2}
    local working_memory=${working_memory:-5}
    
    # Generate visualization based on mode
    log_header "NLM Phase 6 Visualization: $mode"
    
    case "$mode" in
        neural)
            generate_ascii_neural "$neurons" "$spikes" "$width" "$height"
            ;;
        memory)
            generate_ascii_memory "$episodes" "$working_memory" "$width"
            ;;
        neuromodulation)
            generate_ascii_neuromodulation "$dopamine" "$curiosity" "$novelty"
            ;;
        development)
            generate_ascii_development "$stage" 50 "$((100 - stage * 20))"
            ;;
        combined)
            generate_ascii_combined "$neurons" "$spikes" "$episodes" "$dopamine" "$curiosity" "$novelty" "$stage" "$working_memory"
            ;;
    esac
    
    # Generate additional output formats if requested
    if [[ "$format" == "html" ]]; then
        generate_html_output "$mode" "$neurons" "$spikes" "$episodes" "$dopamine" "$curiosity" "$novelty" "$stage" "$working_memory" "${output_file:-nlm_visualization.html}"
    elif [[ "$format" == "json" ]]; then
        generate_json_output "$mode" "$neurons" "$spikes" "$episodes" "$dopamine" "$curiosity" "$novelty" "$stage" "$working_memory" "${output_file:-nlm_visualization.json}"
    fi
    
    log_header "Visualization Complete"
    if [[ -n "$output_file" ]]; then
        log_info "Primary output saved to: $output_file"
    fi
}

# Execute main function
main "$@"