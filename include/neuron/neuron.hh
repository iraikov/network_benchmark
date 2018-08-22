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
#include "utils.hh"
#include "Ncq.hh"

using namespace std;
using nonstd::optional;
using nonstd::nullopt;

namespace neuron
{
        
  enum NeuronType
    {
      Excitatory, Inhibitory, External
    };
  
  
  struct SpikeSource
  {
    int id;
    std::map<int, std::shared_ptr<SpikeSource>> targets; // Synapses
    NeuronType type; 

    virtual ~SpikeSource();
    virtual void ReceivePulse (int sender, double t, NeuronType source_type) = 0;
    virtual void pulse() = 0;
    virtual double average_interval() = 0;
  };

  
  struct PoissonSource : public SpikeSource
  {
    nonstd::optional<Spike> spike; // Spike of the neuron. If no spike, spike = nullopt
    std::shared_ptr<Ncq> queue;      // Queue of spikes
    std::deque<float> window; // Sliding window of intervals between recent spikes
    size_t spike_count;
    double taum; // Time units of simulation
    double time; //Current time for the neuron
    double last_pulse; // time of the last pulse
    double rate; // spike frequency, Hz
    double lambda; // interval parameter
    std::exponential_distribution<double> sample_spike;
    std::default_random_engine rand;
    
    ~PoissonSource();
    PoissonSource(std::shared_ptr<Ncq>& queue, int id, double tauminit, int seed, double rate);

    void ReceivePulse(int sender, double t, NeuronType source_type); // Called if the neuron receive a spike
    void pulse(); //Called if the neuron must pulse
    void update_spike();
    double average_interval();
    
  };
  
  struct Neuron : public SpikeSource
  {

    // Constructor & destructor
    ~Neuron();
    Neuron(std::shared_ptr<Ncq>& queue, double Vinit, double Vrinit, double Vtinit,double Elinit,double Eeinit, double Eiinit, double geinit, double giinit, double tausinit, double tauminit ,double dgiinit, double dgeinit, NeuronType type_, int id);
    /*Constructor :
      queue = Queue of spikes 
      Vinit = initial Potential 
      Vrinit = Reset Potential
      Vtinit = Threshold potential
      Elinit = Leak reversal potential
      Eeinit = Excitatory reversal potential
      Eiinit = Inhibitory reversal potential
      geinit = initial Excitatory conductance
      giinit = initial Inhibitory conductance
      tausinit = Synaptic time constant
      tauminit = Membrane time constant
      dgiinit = Inhibitory synaptic weight
      dgeinit = Excitatory synaptic weight
      type_ = enumerated value from NeuronType
      id_ = id of the neuron
    */

    void ReceivePulse(int sender, double t, NeuronType source_type); // Called if the neuron receive a spike
    void pulse(); //Called if the neuron must pulse
	
#ifdef WITH_LOWER_BOUND
    void update(); //Call by the network if  we need the exact spiking time ( in fact when the "lower bound spike" is on the top of the priority queue )
#endif //WITH_LOWER_BOUND

    nonstd::optional<Spike> spike; // Spike of the neuron. If no spike, spike = nullopt

    double time; //Current time for the neuron
    double V,Vt,Vr; //V = Current potential
    double El,Es, Ee, Ei; //see the constructor
    double taus,taum;	
    double g,ge,gi;
    double dge,dgi;
    std::shared_ptr<Ncq> queue;      // Queue of spikes
    std::deque<float> window; // Sliding window of intervals between recent spikes
    size_t spike_count;
    double last_pulse; //time since the last pulse

  
    double calcV(double newtime, double V0, double g0); //compute V at t = newtime ( relative time ) if there is no spike , and if at t = 0, V = V0, g = g0
    optional <Spike> update_spike(bool exact);        // Return the next spike, otherwise NULL. if exact then we compute the exact spiking time, otherwise only a lowerbound
  
    double findNextSpike(double V0, double g0); //give the time of the next spike using newton-raphson method for (V(t) - Vt) if at t = 0, V = V0, g = g0, if there is no spike


#ifdef WITH_LOWER_BOUND
    double lowerBound(double V0, double g0); //give a lower bound of the time of the next spike using newton-raphson method for (V(t) - Vt) if at t = 0, V = V0, g = g0, if there is no spike, and with MAX_ITER iterations
#endif

    double B; //Bound for Newton Raphson Method

    double average_interval(); // average interval between spikes
  };

}
