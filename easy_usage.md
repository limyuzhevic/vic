# NLM - Easy Usage

## Quick Start

**Building**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Running Phase 6 Integration Demo**
```bash
./nlm_phase6_demo
```

**Running Phase 2 Basic Test**
```bash
./nlm
```

**Running All Tests**
```bash
cd tests
./test_runner
```

## Basic Commands

- `./nlm` - Phase 2 basic neural computation demo
- `./nlm_phase3_demo` - Phase 3 experimental setup  
- `./nlm_phase4_demo` - Phase 4 advanced experiments
- `./nlm_phase6_demo` - Phase 6 full integration test

## Configuration

Modify `configs/default.cfg` or use command line:
```bash
./nlm --neuron_count=1000 --region_count=4 --connection_probability=0.2
```

## What Happens

1. Brain initializes with specified number of neurons
2. Real LIF neuron dynamics simulate spiking behavior  
3. Synaptic plasticity (STDP + Hebbian) modifies connections
4. Learning experiment demonstrates measurable changes
5. Network shows activity-dependent synaptic modification

## Results

The output shows:
- Network activity and firing rates
- Synaptic weight changes (learning detection)
- Integration verification status
- Performance metrics