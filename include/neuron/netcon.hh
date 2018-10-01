/* 

   Representation of a synaptic connection.

Created by: Ivan Raikov <iraikov@stanford.edu>
Last updated : September 2018


*/

#ifndef NETCON_H
#define NETCON_H

#include "spike_source.hh"

namespace neuron
{

  
  /*! Network connection structure containing post-synaptic gid, reference to post-synaptic object, 
      and synaptic strength. */
  struct NetCon
  {
    NetCon(std::shared_ptr<SpikeSource>& source, std::shared_ptr<SpikeSource>& target, double Wmax, double strength)
    {
      this->source = source;
      this->target = target;
      this->s = strength;
      this->Wmax = Wmax;
    };
  
    std::shared_ptr<SpikeSource> source;
    std::shared_ptr<SpikeSource> target;

    double Wmax;
    double s;
    
    // STDP rule
    void update(int sender, double t, double gain);
  };

}

#endif
