/* 

   Representation of a synaptic connection.

Created by: Ivan Raikov <iraikov@stanford.edu>
Last updated : August 2018


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
    NetCon(std::shared_ptr<SpikeSource>& source, std::shared_ptr<SpikeSource>& target, double strength)
    {
      this->source = source;
      this->target = target;
      this->s = strength;
    };
  
    std::shared_ptr<SpikeSource> source;
    std::shared_ptr<SpikeSource> target;
    double s;
  };

}

#endif
