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
#include <deque>
#include <map>
#include <memory>
#include <random>

#include "nonstd/optional.hh"

using namespace std;
using nonstd::optional;
using nonstd::nullopt;

#ifndef SPIKE_SOURCE_H
#define SPIKE_SOURCE_H

namespace neuron
{
  /** 
   *  Enumerated type for the different classes of neurons
   */    
  enum NeuronType
    {
      Excitatory, Inhibitory, External
    };

  class NetCon;

  /*! An abstract class to represent the different classes of spiking sources. */
  struct SpikeSource
  {
    int id;                     /**< id for this spike source */
    std::map<int, NetCon> targets; /**< Post-synaptic targets */
    NeuronType type;            /**< Type of spike source */

    virtual ~SpikeSource();
    /** 
     * This method is invoked when a spike is received.
     * 
     * @param sender the id of the sender spike source
     * @param t the time of the spike
     * @param source_type the type of the sender
     */
    virtual void ReceivePulse (int sender, double t, NeuronType source_type, double s) = 0;
    /** 
     * This method is called at the time when the spike source must emit a spike.
     * 
     */
    virtual void pulse() = 0;
    /** 
     * 
     * This method returns the average time interval between spikes.
     * 
     * @return Average time interval between spikes.
     */
    virtual double average_interval() = 0;
  };
}
#endif
//SPIKE_SOURCE
