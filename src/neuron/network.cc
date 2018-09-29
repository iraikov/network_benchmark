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

#include <memory>
#include "nonstd/optional.hh"
#include "netcon.hh"
#include "network.hh"

using namespace std;
using nonstd::optional;
using nonstd::nullopt;

namespace neuron
{
  //#define IFSC_PREC 1e-4 
  
  double log_e(double a)
  {
    return(log(a)*2.30258509299);
  }

  SpikeSource::~SpikeSource()
  {};

  Network::Network(double tstop_, int seed)
  {

    t = 0.0;
    t_DA = -1.0;
    tstop = tstop_;
    DA = unique_ptr<double>(new double (0.0));
    nb_neurons = NEURONS + EXT_INPUTS + EXT_OUTPUTS;
    num_synapses = 0;
    num_input_synapses = 0;
    num_output_synapses = 0;
    
    q = shared_ptr<Ncq>(new Ncq());
    
    // To compute rho
    IFSC_Init(Taui);

    time_t timer1;
    time(&timer1);
    
    std::default_random_engine rand;
    rand.seed(seed);
    
    std::normal_distribution<double> sample_Vinit(-5.0,5.0);
    std::normal_distribution<double> sample_geinit(4.0,1.5);
    std::normal_distribution<double> sample_giinit(20.0,12.0);
    std::uniform_real_distribution<double> sample_connect(0.0,1.0);
  
    // initialization of external input units.
    for (int i=0;i<EXT_INPUTS;i++)
      {
        shared_ptr <PoissonSource> p(new PoissonSource(q,i,Taum,i,EXT_RATE));
        pop_vec.push_back(p);
      }

    int offset_inhibitory = EXT_INPUTS+INHIBITORY_NEURONS;
    int start_inhibitory = EXT_INPUTS;
    
    int offset_excitatory = EXT_INPUTS+NEURONS;
    int start_excitatory = EXT_INPUTS+INHIBITORY_NEURONS;
    
    int offset_output = EXT_INPUTS+EXT_OUTPUTS+NEURONS;
    int start_output = EXT_INPUTS+NEURONS;
    
    // initialization of inhibitory and excitatory neurons.
    for (int i=start_inhibitory;i<offset_inhibitory;i++)
      {
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <Neuron> p (new Neuron(q,DA,Vinit,Vr_,Vt_,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Inhibitory,i));
        pop_vec.push_back(p);
      }
    
    for (int i=start_excitatory;i<offset_excitatory;i++)
      {
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <Neuron> p (new Neuron(q,DA,Vinit,Vr_,Vt_,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Excitatory,i));
        pop_vec.push_back(p);
      }
    
    for (int i=start_output;i<offset_output;i++)
      {
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <Neuron> p (new Neuron(q,DA,Vinit,Vr_,Vt_,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Excitatory,i));
        pop_vec.push_back(p);
      }


    // Recurrent connectivity within inhibitory + excitatory population
    for (int i=EXT_INPUTS;i<EXT_INPUTS+NEURONS;i++)
      {
        for (int j=EXT_INPUTS;j<EXT_INPUTS+NEURONS;j++)
          {
            if (i!=j)
              {
                double prob = sample_connect(rand);
                if (prob <= PROB_SYNAPSES)
                  {
                    NetCon nc(pop_vec.at(i), pop_vec.at(j), 1.0);
                    pop_vec.at(i)->targets.insert(make_pair(j, nc));
                    num_synapses++;
                  }
              }
          }
      }
    
    // Convergence onto output neurons
    for (int i=EXT_INPUTS;i<EXT_INPUTS+NEURONS;i++)
      {
        for (int j=EXT_INPUTS+NEURONS;j<EXT_INPUTS+NEURONS+EXT_OUTPUTS;j++)
          {
            if (i!=j)
              {
                double prob = sample_connect(rand);
                if (prob <= PROB_OUTPUT_SYNAPSES)
                  {
                    NetCon nc(pop_vec.at(i), pop_vec.at(j), 1.0);
                    pop_vec.at(i)->targets.insert(make_pair(j, nc));
                    num_synapses++;
                    num_output_synapses++;
                  }
              }
          }
      }
    // External inputs to inhibitory + excitatory population
    for (int i=0;i<EXT_INPUTS;i++)
      {
        for (int j=EXT_INPUTS;j<EXT_INPUTS+NEURONS;j++)
          {
            if (i!=j)
              {
                double prob = sample_connect(rand);
                if (prob <= PROB_EXT_SYNAPSES)
                  {
                    NetCon nc(pop_vec.at(i), pop_vec.at(j), 1.0);
                    pop_vec.at(i)->targets.insert(make_pair(j, nc));
                    num_synapses++;
                    num_input_synapses++;
                  }
              }
          }
      }

      
  };



  Network::~Network()
  {
    IFSC_Done();
  };


  void Network::start()
  {
    while(t<=tstop/Taum && !(q->empty()))
      {
        step();
      }
  };
  
  int Network::step()
  {

    int result=0;
    if(t<=tstop/Taum && !(q->empty()))
      {

        optional<Spike> s = q->pop(); // s = Next spike

        if (s != nullopt)
          {
#ifdef WITH_LOWER_BOUND
            if (!(s->computed))
              { 
                pop_vec.at(s->sender)->update();
              }
            
            else
              { //the time is exact
#endif //WITH_LOWER_BOUND
                t = s->t;
                if (!reward.empty())
                  {
                    if (reward.top() <= (t*Taum))
                      {
                        double t_reward = reward.top();
                        reward.pop();

                        *DA = *DA * exp(-DA_LAMBDA * (t-t_DA)) + alpha((t*Taum)-t_reward, DA_T_PEAK_);
                      }
                    else
                      {
                        *DA = *DA * exp(-DA_LAMBDA * (t-t_DA));
                      }
                  }
                else
                  {
                    *DA = *DA * exp(-DA_LAMBDA * (t-t_DA));
                  }
                t_DA = t;
                
                outputs.push(*s);
                pop_vec.at(s->sender)->pulse();

                double average_interval = pop_vec.at(s->sender)->average_interval();
                printf("%d %f %f\n",s->sender,s->t*Taum,
                       average_interval > 0.0 ? (1000.0 / average_interval) : 0.0);
              }
#ifdef WITH_LOWER_BOUND
          }
#endif

        result = 0;
      }
    else
      {
        result = -1;
      }

    return result;
  };


}
