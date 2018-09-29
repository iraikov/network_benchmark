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
#include "neuron.hh"

namespace neuron
{
  
  using namespace std;

  PoissonSource::~PoissonSource()
  {
  };

  PoissonSource::PoissonSource(std::shared_ptr<Ncq>& queue, int id, double tauminit, int seed, double rate)
  {
    this->id = id;
    this->queue = queue;
    this->rate = rate;
    this->taum = tauminit;
    this->spike_count = 0;
    this->last_pulse = (-2.)*REFRACT;
    this->time = 0.;
    this->spike = nullopt;
    this->lambda = rate/1000.0;
    this->type = External;
    
    rand.seed(seed);
    
    typename std::exponential_distribution<double>::param_type new_lambda(lambda);
    sample_spike.param(new_lambda);
    this->update_spike();
  }

  void PoissonSource::ReceivePulse(int sender, double t, NeuronType source_type, double s)
  {
    printf("Spike sent to PoissonSource id %d: sender = %d", id, sender);
    throw;
  }

  void PoissonSource::update_spike()
  {
    if (rate > 0.)
      {
        double sample = sample_spike(rand);

        optional<Spike> newSpike = Spike(id, time + sample/taum);
        spike = newSpike;

        queue->insert(*spike);
      }
    
  }
  
  void PoissonSource::pulse()
  {

    if (spike.has_value())
      {
        if (spike_count > 0)
          {
            while (window.size() >= MOVING_AVG_WINDOW_SIZE)
              {
                window.pop_front();
              }
            window.push_back((time - last_pulse) * taum);
          }
        for (auto const& item : targets)
          {
            item.second.target->ReceivePulse(id,spike->t+DELAY,type,item.second.s);
          }
        last_pulse = time;
        time = spike->t;
        spike_count++;
      }

    update_spike();
  }

  double PoissonSource::average_interval()
  {
    double result = 0.0;
    double sum = 0.0;
    if ((spike_count > 1) && (window.size() > 0))
      {
        for(auto it=window.begin(); it!=window.end(); ++it)
          {
            sum += *it;
          }
        result = sum / (double)(window.size());
      }
    return result;
  }

  
  Neuron::~Neuron()
  {
  };


  Neuron::Neuron(std::shared_ptr<Ncq>& queue_, std::shared_ptr<double>& DA_, double Vinit, double Vrinit, double Vtinit,double Elinit,double Eeinit, double Eiinit, double geinit, double giinit, double tausinit, double tauminit, double dgiinit, double dgeinit, NeuronType type_, int id_)  {
    V = Vinit;
    El = Elinit;
  
    ge = geinit;
    gi = giinit;
    Vr = Vrinit;
    Vt = Vtinit;
    Ee = Eeinit;
    Ei = Eiinit;
    dgi = dgiinit;
    dge = dgeinit;
    taus = tausinit;
    taum = tauminit;
    type = type_;
    B = Vt - EPS;
    id = id_;
    queue = queue_;
    spike_count = 0;
  
    last_pulse = (-2.)*REFRACT;
    time = 0.;
    
    if (ge<0.) ge *= -1.;  
    if (gi<0.) gi *= -1.;
    if (V >= Vt) { V = Vt - 0.5; }
  
  
    Es = (ge*Eeinit + gi*Eiinit)/(ge+gi);
    g = ge + gi;

    stdp = 0.0;
    DA = DA_;
    
    spike = update_spike(false);
    if (spike.has_value())
      {
        queue->insert(*spike);
      }
  };





  void Neuron::ReceivePulse(int sender, double t, NeuronType source_type, double s)
  {
	
    double gPulse;

    if (!(time <= t))
      {
        printf("id %d: ReceivePulse: time = %f last_pulse = %f sender = %d t = %f\n",
               id, time, last_pulse, sender, t);
      }
    
    assert(time <= t);
    //1)Update V
    double start_time = (last_pulse > 0. ? last_pulse : 0.) + REFRACT; // end of the refractory period
    double period = t - time; //length of the period since the last update
  
    if (time>=start_time)
      {
        V = calcV(period,V,g);
      }
    else
      { //time of last_pulse and last_pulse + refractory period  
    
        if ( t >= start_time )
          { 
            
            double active_time =  t -  start_time ;	 
            V = calcV(active_time,Vr,g*exp(-(start_time-time)/taus));	
            
          }  //otherwise V = Vr...
      }  //otherwise V = Vr...
  
    //2) Compute g at t = "receiving the pulse"
    gPulse = g * exp(-(period)/taus); 
  
    //3) Receive the spike :
    switch(source_type)
      {
      case Inhibitory:
        {
          // new parameters at t = ReceivePulse
          Es = (gPulse*Es + s * dgi * Ei)/(gPulse+dgi);
          gPulse += dgi;
        }
        break;
      case Excitatory:
        {
          Es = (gPulse*Es + s * dge * Ee)/(gPulse+dge);
          gPulse += dge;
        }
        break;
      case External:
        {
          Es = (gPulse*Es + s * dge * Ee)/(gPulse+dge);
          gPulse += dge;
        }
        break;
      default:
        {
          printf("ReceivePulse: unknown sender type %d\n", source_type);
          throw;
        }
      }
        
    // Go to t and update g
    g = gPulse;
    time = t;

    // Test with new values if the neuron will spike
    optional<Spike> newSpike = update_spike(false);

    if (spike.has_value())
      {
        queue->remove(*spike);
      }

    spike = newSpike;
  
    if (spike.has_value())
      {
        queue->insert(*spike);
      }

    while (window.size() > MOVING_AVG_WINDOW_SIZE)
      {
        window.pop_front();
      }

	
  };



  // Called after spike of the neuron has been taken off the queue.
  void Neuron::pulse()
  {

    if (!spike.has_value())
      {
        printf("neuron %d has no spike value\n", id);
      }
    
    //update state variable
    V = Vr;
    g *= exp(-(spike->t - time)/taus);
    time = spike->t;

    while (window.size() >= MOVING_AVG_WINDOW_SIZE)
      {
        window.pop_front();
      }
    window.push_back((time - last_pulse) * taum);
    last_pulse = time;
    
    //update next_spike
    spike = update_spike(false);
  
    if (spike.has_value())
      {
        queue->insert(*spike);
      }
  
    // the neuron spikes 
    for (auto& item : targets)
      {
        // Update post-synaptic neurons
        item.second.target->ReceivePulse(id,time+DELAY,type,item.second.s);
        // Update synaptic weights according to STDP rule
        item.second.update(id,time+DELAY,1.0,*DA);
      }

    spike_count++;
  };



  // Return the new updated spike of the neuron
  optional<Spike> Neuron::update_spike(bool exact)
  {
    double g_star;
    double V_g_star;
  
    if (V>=Vt) { Spike s(id, time); return s; }
  
    g_star = Vt/(Es-Vt); // = 1./((Es/Vt)-1.);
  
    if ((g<g_star) || (Es  < Vt ))
      { 
        return nullopt;	
      }
    //ELSE
  
    double t_begin; //relative time as soon as we start to look for the cross 
    double gt; // = g if not in the REFRACT. period, otherwise  = g(end of the REFRACT period)
    double st = time - last_pulse;
  
    if (st>= REFRACT)
      {
        gt = g;
        t_begin = 0.;
      }
    else
      {
        t_begin = REFRACT - st;
        gt = g*exp(-t_begin/taus);
      }
  
    V_g_star = -taus*Es*g_star*rho(g_star)+pow(g_star/gt,taus)*exp(taus*(g_star-gt))*(V+taus*Es*gt*rho(gt));
  
    if (V_g_star < Vt )
      {
        return nullopt;
      }
  
    //We can now compute the time for the next spike
  
#ifdef WITH_LOWER_BOUND
    if (exact)
      {
#endif //WITH_LOWER_BOUND
      
        Spike s(id, time + t_begin + findNextSpike(V,gt));
      
#ifdef WITH_LOWER_BOUND
      }

    else
      { //exact = false
        Spike s(id, time + t_begin + lowerBound(V,gt));
        s.computed = false;
      }
#endif //WITH_LOWER_BOUND

    return s;
  };


  double Neuron::calcV(double newtime, double V0, double g0)
  {
  
    //gt = g0(newtime)
    double	gt = g0 * exp(-newtime/taus); 
    return(-taus*Es*gt*rho(gt)+exp(-newtime+taus*(gt-g0))*(V0+taus*Es*g0*rho(g0)));
  }


  double Neuron::findNextSpike(double V0, double g0)
  {

    double T = 0.;
    double gT = g0;
    double Vn = calcV(T,V0,g0);

    while(Vn<B)
      {
      
        //Newton Raphson Method : Tn+1 = Tn - F(Tn)/F'(Tn)    with F(T) = calcV(T) - Vt
        T += (Vt-Vn)/(-Vn+gT*(Es-Vn));
        gT = g0 * exp(-T/taus);
        Vn = calcV(T,V0,g0);
      
      }
  
    return T;
  };




#ifdef WITH_LOWER_BOUND

  //The same as findNextSpike() but with only few iterations for the newton raphson method
  double Neuron::lowerBound(double V0, double g0)
  {
    double T = 0.;
    double gT = g0;
    int iter = 0;
  
    double Vn = calcV(T,V0,g0);
 
    while(iter<MAX_ITER)
      {
      
        //Newton Raphson Method : Tn+1 = Tn - F(Tn)/F'(Tn)    with F(T) = calcV(T) - Vt
        T += (Vt-Vn)/(-Vn+gT*(Es-Vn));
        gT = g0 * exp(-T/taus);
        Vn = calcV(T,V0,g0);
        iter++;
        
      }
  
    return T;
  };



  void Neuron::update()
  {
    spike = update_spike(true);
    if (spike != null)
      {
        queue->insert(*ds);
      }
  };
#endif //WITH_LOWER_BOUND

  double Neuron::average_interval()
  {
    double result = 0.0;
    double sum = 0.0;
    if ((spike_count > 1) && (window.size() > 0))
      {
        for(auto it=window.begin(); it!=window.end(); ++it)
          {
            sum += *it;
          }
        result = sum / (double)(window.size());
      }
    return result;
  }

}

