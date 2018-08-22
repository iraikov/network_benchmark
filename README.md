# network_benchmark

An implementation of of a spiking neural network based on the algorithms described in the following paper:

 Exact simulation of integrate-and-fire models with synaptic conductances. Brette, R. (2006).
 Neural Computation 18(8): 2004-2027.
 
In this implementation, tau_e = tau_i = 5 ms. This is an intrinsic
limitation of the method described in the paper mentioned above. 

This repository contains the following files:

- conf.h: parameters of the simulation.
- ./include/data/spike.hh: defines a class for spike events.
- ./include/data/Ncq.hh: defines a priority queue class for spikes
- ./include/data/spike.hh: defines a class for spike events.
- ./include/data/util.hh: definitions of numerical solver routines
- ./include/neuron/neuron.hh: definitions of spike sources in the network (neurons and Poisson spike sources) 
- ./include/neuron/network.hh: definition of the network class
- ./src/data/Ncq.cpp: implementation of spike priority queue
- ./src/data/util.cpp: implementations of solver routines
- ./src/neuron/neuron.cpp: implementations of spike sources classes
- ./src/neuron/network.cpp: implementations of network class
- ./src/driver/network_driver.cpp: runs the simulation and writes the output spike times to a file

[Code API documentation](api.md)

## Configuration

The network model is configured via parameters specified in file ./include/conf.hh:


- EXT_INPUTS: number of external inputs
- EXT_RATE: average firing rate of external inputs
- NEURONS: number of neurons		
- AVERAGE_SYNAPSES: average number of synapses per neuron
- AVERAGE_EXT_SYNAPSES: average number of synapses per external source
- INHIBITORY_PROPORTION: proportion of inhibitory neurons

- REFRACT_ : length of the refractory period [ms]
- DELAY_ 1.0 : length of the delay
- Taum: membrane time constant [ms]
- Dge_: excitatory synaptic weight
- Dgi_: inhibitory synaptic weight


## Building

Please select your target platform by running one of the following commands:

  make PLATFORM=gnu.linux

## Running

To run the network simulator, run the following command:

  ./build/network_driver [OPTIONS]
  
Options:
 -h         Print help
 -t TSTOP   Simulation until TSTOP ms 
 -o PATH    Write spike times to PATH 
 -s SEED    Random number generator seed 

