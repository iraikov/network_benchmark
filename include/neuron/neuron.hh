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
Last updated : September 2018

*/



#include <vector>
#include <deque>
#include <map>
#include <memory>
#include <random>

#include "nonstd/optional.hh"
#include "utils.hh"
#include "Ncq.hh"
#include "spike_source.hh"
#include "netcon.hh"

using namespace std;
using nonstd::optional;
using nonstd::nullopt;

#ifndef NEURON_H
#define NEURON_H

namespace neuron
{

  /*! Source of spikes that are Poisson-distributed in time. */  
  struct PoissonSource : public SpikeSource
  {
    nonstd::optional<Spike> spike; /**< Spike of the neuron. If no spike, spike = nullopt */
    std::shared_ptr<Ncq> queue; /**< Queue of spikes: shared with the network and all other SpikeSource instances. */
    std::deque<float> window; /**< Sliding window of intervals between recent spikes. */
    size_t spike_count; /**< Current spike count. */
    double taum; /**< Time units of simulation. */
    double time; /**< Current time for the neuron. */
    double rate; /**< spike frequency, Hz */
    double lambda; /**< interval parameter, ms */
    std::exponential_distribution<double> sample_spike; /**< distribution of spike intervals */
    std::default_random_engine rand; /**< random number generator */
    std::shared_ptr<double> DA;/**< level of dopamine above the baseline */

    ~PoissonSource();
    PoissonSource(std::shared_ptr<Ncq>& queue, std::shared_ptr<double>& DA, int id, double tauminit, int seed, double rate);

    /** 
     * Not supported in class PoissonSource.
     * 
     * @param sender the id of the sender spike source
     * @param t the time of the spike
     * @param source_type the type of the sender
     */
    void ReceivePulse(int sender, double t, NeuronType source_type, double s);
    
    /** 
     * Calls ReceiveSpike on all post-synaptic targets and computes the time of the next spike.
     * 
     */
    void pulse(); 

    /** 
     * Computes the time of the next spike.
     * 
     */

    void update_spike();
    /** 
     * Computes and returns the average time interval between spikes.
     * 
     * @return Average time interval between spikes.
     */
    double average_interval();
    
  };

  
  /*! Source of spikes from a vector of spike times. */  
  struct VectorSource : public SpikeSource
  {
    nonstd::optional<Spike> spike; /**< Spike of the neuron. If no spike, spike = nullopt */
    std::shared_ptr<Ncq> queue; /**< Queue of spikes: shared with the network and all other SpikeSource instances. */
    std::deque<float> window; /**< Sliding window of intervals between recent spikes. */
    size_t spike_count; /**< Current spike count. */
    double taum; /**< Time units of simulation. */
    double time; /**< Current time for the neuron. */
    std::vector<double> spikes; /**< future spike times, in taum units */
    std::shared_ptr<double> DA;/**< level of dopamine above the baseline */

    ~VectorSource();
    VectorSource(std::shared_ptr<Ncq>& queue, std::shared_ptr<double>& DA, int id, double tauminit, const std::vector <double>& vec );

    /** 
     * Not supported in class VectorSource.
     * 
     * @param sender the id of the sender spike source
     * @param t the time of the spike
     * @param source_type the type of the sender
     */
    void ReceivePulse(int sender, double t, NeuronType source_type, double s);
    
    /** 
     * Calls ReceiveSpike on all post-synaptic targets and computes the time of the next spike.
     * 
     */
    void pulse(); 

    /** 
     * Computes the time of the next spike.
     * 
     */

    void update_spike();
    /** 
     * Computes and returns the average time interval between spikes.
     * 
     * @return Average time interval between spikes.
     */
    double average_interval();
    
  };

  

  /*! Exact simulation of integrate-and-fire neuron. */  
  struct EIFNeuron : public SpikeSource
  {

    // Constructor & destructor
    ~EIFNeuron();

    /** 
     * Neuron constructor.
     * 
     * @param queue Queue of spikes shared with the network and all other SpikeSource instances.
     * @param Vinit initial potential 
     * @param Vrinit reset potential
     * @param Vtinit spike threshold potential
     * @param Elinit leak reversal potential
     * @param Eeinit excitatory reversal potential
     * @param Eiinit inhibitory reversal potential
     * @param geinit initial excitatory conductance
     * @param giinit initial inhibitory conductance
     * @param tausinit synaptic time constant
     * @param tauminit membrane time constant
     * @param dgiinit inhibitory synaptic weight
     * @param dgeinit excitatory synaptic weight
     * @param type_ enumerated value from NeuronType
     * @param id id of the neuron
     * 
     */
    EIFNeuron(std::shared_ptr<Ncq>& queue, std::shared_ptr<double>& DA, double Vinit, double Vrinit, double Vtinit,double Elinit,double Eeinit, double Eiinit, double geinit, double giinit, double tausinit, double tauminit, double dgiinit, double dgeinit, NeuronType type_, int id);

    void ReceivePulse(int sender, double t, NeuronType source_type, double s); 
    void pulse(); 
	
#ifdef WITH_LOWER_BOUND
    void update(); // Called by the network if  we need the exact spiking time ( in fact when the "lower bound spike" is on the top of the priority queue )
#endif //WITH_LOWER_BOUND

    nonstd::optional<Spike> spike; /**< Spike of the neuron. If no spike, spike = nullopt */

    double time; /**< Current time for the neuron */
    double V,Vt,Vr; /**< V = Current potential, threshold, reset potential */
    double El,Es,Ee,Ei; /**< Reversal potential */
    double taus,taum; /**< Synaptic and membrane time constants */
    double g,ge,gi; /**< Membrane and synaptic conductances */
    double dge,dgi; /**< Synaptic weights */

    std::shared_ptr<double> DA;/**< level of dopamine above the baseline */
    
    std::shared_ptr<Ncq> queue;      /**< Queue of spikes shared with the network and all other SpikeSource instances. */
    std::deque<float> window; /**< Sliding window of intervals between recent spikes. */
    size_t spike_count; /**< Spike count */

    /** 
     * Computes V at t = newtime (relative time) if there is no spike, and if at t = 0, V = V0, g = g0
     * 
     * @param newtime 
     * @param V0 
     * @param g0 
     * 
     * @return updated membrane potential
     */
    double calcV(double newtime, double V0, double g0);

    /** 
     * Returns the next spike, otherwise nullopt. if exact then we compute the exact spiking time, otherwise only a lower bound.
     * 
     * @param exact 
     * 
     * @return optional Spike object
     */
    optional <Spike> update_spike(bool exact);        

    /** 
     * Computes the time of the next spike using newton-raphson method for (V(t) - Vt) if at t = 0, V = V0, g = g0, if there is no spike.
     * 
     * @param V0 
     * @param g0 
     * 
     * @return spike time
     */
    double findNextSpike(double V0, double g0); 


#ifdef WITH_LOWER_BOUND
    /** 
     * Computes a lower bound of the time of the next spike using newton-raphson method for (V(t) - Vt) if at t = 0, V = V0, g = g0, if there is no spike, and with MAX_ITER iterations.
     * 
     * @param V0 
     * @param g0 
     * 
     * @return 
     */
    double lowerBound(double V0, double g0); 
#endif

    double B; /**< Bound for Newton Raphson Method */
    /** 
     * Computes the average time interval between spikes.
     * 
     * 
     * @return Average time interval between spikes in milliseconds.
     */
    double average_interval();
  };

}
#endif //NEURON
