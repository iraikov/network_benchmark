/*     ###########     Exact simulation of integrate-and-fire models with synaptic conductances ######################

This code is associated with the paper :
"Brette Romain (2006) , Exact simulation of integrate-and-fire models with synaptic conductances" :
http://www.di.ens.fr/~brette/papers/Brette2005NC.htm
*************************
*************************
*************************
Cohen Benjamin
benjamin.cohen _AT_ ens-lyon.fr
Last updated : Jul 2006 

If you modify the source file, please don't delete this header
***********************************************************************************************************

Modified by: Ivan Raikov <iraikov@stanford.edu>
Last updated : August 2018

*/


#include <vector>
#include <memory>
#include <queue>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "neuron.hh"

namespace neuron
{
  
  struct Network
  {
    Network(double t, int seed);
    ~Network();
  
    void start(); // Start the simulation
    int step();	// One simulation step
    std::queue<Spike> outputs; // Output spikes
    
    double t;
    double tstop;

    int nb_neurons;	// number of neurons
    size_t num_synapses; // number of synapses
    std::vector<shared_ptr<SpikeSource>> pop_vec; // vector of neurons
    std::shared_ptr<Ncq> q;  // priority queue with spike times
  };


}
