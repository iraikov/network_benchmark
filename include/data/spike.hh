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
*/

#ifndef SPIKE_H
#define SPIKE_H


#include "conf.hh"
namespace neuron
{
  
  struct Spike
  {
    Spike(int sender, double time)
    {
      this->sender = sender;
      this->t = time;
#ifdef WITH_LOWER_BOUND	
      computed = true;
#endif //WITH_LOWER_BOUND
            
    };
  
    int sender;
    double t; //the neuron 'from' will spike at 't' 
        
#ifdef WITH_LOWER_BOUND
    bool computed; //true if t is the exact spiking time, otherwise t is a lower bound of the spiking time
#endif //WITH_LOWER_BOUND

    friend bool operator==(const Spike& s1, const Spike& s2);
  };
  
  struct compare_spikes
  {
    bool operator()(Spike &a, Spike &b)
    {
      return (a.t > b.t);
    }
  };

}

#endif
