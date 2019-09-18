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
#include <algorithm>
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

  Coords::Coords(int phi): phi(phi) {};

  Coords::~Coords() {};

  Network::Network(double tstop_, int seed, vector <vector <double> > input_vector)
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
    
    std::normal_distribution<double> sample_Vinit(-60.0,1.0);
    std::normal_distribution<double> sample_geinit(1.0,1.0);
    std::normal_distribution<double> sample_giinit(-2.0,1.0);
    std::normal_distribution<double> sample_Vt(Vt_,2.0);

    int offset_inhibitory = EXT_INPUTS+INHIBITORY_NEURONS;
    int start_inhibitory = EXT_INPUTS;
    
    int offset_excitatory = EXT_INPUTS+NEURONS;
    int start_excitatory = EXT_INPUTS+INHIBITORY_NEURONS;

    int offset_neurons = EXT_INPUTS+NEURONS;
    int start_neurons = start_inhibitory;
    
    int offset_outputs = EXT_INPUTS+EXT_OUTPUTS+NEURONS;
    int start_outputs = EXT_INPUTS+NEURONS;

    int offset_inputs = EXT_INPUTS;
    int start_inputs = 0;

    size_t coord_extent = NEURONS;
    size_t coord_step_input = round(coord_extent / EXT_INPUTS);
    size_t coord_step_output = round(coord_extent / EXT_OUTPUTS);
    size_t coord_step_excitatory = round(coord_extent / EXCITATORY_NEURONS);
    size_t coord_step_inhibitory = round(coord_extent / INHIBITORY_NEURONS);
      
    // initialization of external input units.
    if (input_vector.size() == 0) 
      {
        // if empty input vector, initialize external inputs as Poisson spike sources
        for (int i=start_inputs;i<offset_inputs;i++)
          {
            shared_ptr <PoissonSource> p(new PoissonSource(q,DA,i,Taum,i,EXT_RATE));
            pop_vec.push_back(p);
            size_t c = i*coord_step_input;
            coords.push_back(Coords(c%coord_extent));
          }
      }
    else
      {
        // initialize external inputs using the provided input vector
        for (int i=start_inputs;i<offset_inputs;i++)
          {
            shared_ptr <VectorSource> p(new VectorSource(q,DA,i,Taum,input_vector.at(i)));
            pop_vec.push_back(p);
            size_t c = i*coord_step_input;
            coords.push_back(Coords(c%coord_extent));
          }
      }
    
    // initialization of inhibitory and excitatory neurons.
    for (int i=start_inhibitory;i<offset_inhibitory;i++)
      {
        double Vt = sample_Vt(rand);
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <EIFNeuron> p (new EIFNeuron(q,DA,Vinit,Vr_,Vt,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Inhibitory,i));
        pop_vec.push_back(p);
        size_t c = i*coord_step_inhibitory;
        coords.push_back(Coords(c%coord_extent));
        //cout << "Inhibitory neuron " << i << " coordinates: " << c%coord_extent << endl;
      }
    
    for (int i=start_excitatory;i<offset_excitatory;i++)
      {
        double Vt = sample_Vt(rand);
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <EIFNeuron> p (new EIFNeuron(q,DA,Vinit,Vr_,Vt,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Excitatory,i));
        pop_vec.push_back(p);
        size_t c = i*coord_step_excitatory;
        coords.push_back(Coords(c%coord_extent));
        //cout << "Excitatory neuron " << i << " coordinates: " << c%coord_extent << endl;
      }
    
    for (int i=start_outputs;i<offset_outputs;i++)
      {
        int module = (i%4) + 1;
        double Vt = sample_Vt(rand) + module*3.;
        double Vinit = sample_Vinit(rand);
        double geinit = sample_geinit(rand);
        double giinit = sample_giinit(rand);
        shared_ptr <EIFNeuron> p (new EIFNeuron(q,DA,Vinit,Vr_,Vt,El_,Ee_,Ei_,geinit,giinit,Taui,Taum,Dgi_,Dge_,Excitatory,i));
        pop_vec.push_back(p);
        size_t c = i*coord_step_output;
        coords.push_back(Coords(c%coord_extent));
      }

    ConnectionGenerator cg(seed, coord_extent, pop_vec, coords,
                           (distance_t)EXC_CONNECTION_DISTANCE, (distance_t)INH_CONNECTION_DISTANCE,
                           offset_inhibitory, start_inhibitory,
                           offset_excitatory, start_excitatory,
                           offset_neurons, start_neurons,
                           offset_outputs, start_outputs,
                           offset_inputs, start_inputs);

    cg.skewnormal_ring(num_synapses, num_input_synapses, num_output_synapses);
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
                        printf("reward: %f\n", t_reward);
                        printf("DA prev: %f\n", exp(-DA_LAMBDA * (t-t_DA)));
                        printf("DA period: %f\n", (t*Taum)-t_reward);
                        printf("DA rhs: %f\n", alpha((t*Taum)-t_reward, DA_T_PEAK_));
                        if (t_DA > 0.)
                          {
                            *DA = exp(-DA_LAMBDA * (t-t_DA)) + alpha((t*Taum)-t_reward, DA_T_PEAK_);
                          }
                        else
                          {
                             *DA = alpha((t*Taum)-t_reward, DA_T_PEAK_);
                          }
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
                printf("DA: %f at %f\n", *DA, t_DA*Taum);
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

  /*! Connection generator for recurrent neural network. */  
  ConnectionGenerator::ConnectionGenerator(int seed, size_t coords_extent,
                                           
                                           std::vector<shared_ptr<SpikeSource>>& pop_vec,
                                           std::vector< Coords >& coords,

                                           distance_t exc_connection_distance,
                                           distance_t inh_connection_distance,
                                                                   
                                           int offset_inhibitory,
                                           int start_inhibitory,
                                           
                                           int offset_excitatory,
                                           int start_excitatory,
                                           
                                           int offset_neurons,
                                           int start_neurons,
                                           
                                           int offset_outputs,
                                           int start_outputs,
                                           
                                           int offset_inputs,
                                           int start_inputs):
  
    seed(seed), coords_extent(coords_extent),
    
    pop_vec(pop_vec),
    coords(coords),
    exc_connection_distance(exc_connection_distance),
    inh_connection_distance(inh_connection_distance),

    offset_inhibitory(offset_inhibitory),
    start_inhibitory(start_inhibitory),
    
    offset_excitatory(offset_excitatory),
    start_excitatory(start_excitatory),
    
    offset_neurons(offset_neurons),
    start_neurons(start_neurons),
    
    offset_outputs(offset_outputs),
    start_outputs(start_outputs),
    
    offset_inputs(offset_inputs),
    start_inputs(start_inputs)
    
    {}


    /** 
     * Instantiate skew normal ring architecture.
     * 
     */
  void ConnectionGenerator::skewnormal_ring(size_t& num_synapses, size_t& num_input_synapses, size_t& num_output_synapses)
  {
    std::default_random_engine rand;
    rand.seed(seed);

    num_synapses = 0;
    num_input_synapses = 0;
    num_output_synapses = 0;
    
    // Connectivity of excitatory population
    for (int i=start_excitatory;i<offset_excitatory;i++)
      {
        double Wmin = WMIN;
        double Wmax = 0.0;
        if (pop_vec[i]->type == Excitatory)
          {
            Wmax = WMAX;
          }

        { // All excitatory neurons connect to inhibitory neurons according to a normal distribution
          
          vector< std::pair<int,distance_t> > neighbors;
          vector< std::pair<int,double> > neighbors_probdist;
          vector< int > selected_neighbors;
          
          get_neighbors(i, start_inhibitory, offset_inhibitory, exc_connection_distance, neighbors);
          
          get_norm_probdist(neighbors, exc_connection_distance, neighbors_probdist);
          choose_connections(rand, neighbors_probdist, NUM_SYNAPSES, selected_neighbors);

          
          for (int j : selected_neighbors)
            {
              if (i!=j)
                {
                  NetCon nc(pop_vec.at(i), pop_vec.at(j), Wmin, Wmax, 1.0);
                  pop_vec.at(i)->targets.insert(make_pair(j, nc));
                  num_synapses++;
                }
            }
        }

        { // Excitatory neurons connect to other excitatory neurons as determined by a skew normal distribution
          
          vector< std::pair<int,distance_t> > neighbors;
          vector< std::pair<int,double> > neighbors_probdist;
          vector< int > selected_neighbors;
          
          get_neighbors(i, start_excitatory, offset_excitatory, exc_connection_distance, neighbors);
          get_skewnorm_probdist(neighbors, exc_connection_distance, EXC_CONNECTION_SKEW, neighbors_probdist);
          choose_connections(rand, neighbors_probdist, NUM_SYNAPSES, selected_neighbors);
          
          for (int j : selected_neighbors)
            {
              if (i!=j)
                {
                  NetCon nc(pop_vec.at(i), pop_vec.at(j), Wmin, Wmax, 1.0);
                  pop_vec.at(i)->targets.insert(make_pair(j, nc));
                  num_synapses++;
                }
            }
        }

      }

    // Connectivity from inhibitory to excitatory population
    for (int i=start_inhibitory;i<offset_inhibitory;i++)
      {
                  
        vector< std::pair<int,distance_t> > neighbors;
        vector< std::pair<int,double> > neighbors_probdist;
        vector< int > selected_neighbors;
        
        get_neighbors(i, start_excitatory, offset_excitatory, inh_connection_distance, neighbors);
        get_norm_probdist(neighbors, inh_connection_distance, neighbors_probdist);
        choose_connections(rand, neighbors_probdist, NUM_SYNAPSES, selected_neighbors);
        
        double Wmin = WMIN;
        double Wmax = 0.0;
        if (pop_vec[i]->type == Excitatory)
          {
            Wmax = WMAX;
          }
        for (int j : selected_neighbors)
          {
            if (i!=j)
              {
                NetCon nc(pop_vec.at(i), pop_vec.at(j), Wmin, Wmax, 1.0);
                pop_vec.at(i)->targets.insert(make_pair(j, nc));
                num_synapses++;
              }
          }
      }
    
    // Convergence onto output neurons
    for (int i=start_excitatory;i<offset_excitatory;i++)
      {
        double Wmin = WMIN;
        double Wmax = 0.0;
        if (pop_vec[i]->type == Excitatory)
          {
            Wmax = WMAX;
          }

        vector< std::pair<int,distance_t> > neighbors;
        vector< std::pair<int,double> > neighbors_probdist;
        vector< int > selected_neighbors;
          
        get_neighbors(i, start_outputs, offset_outputs, exc_connection_distance, neighbors);
        get_skewnorm_probdist(neighbors, exc_connection_distance, EXC_CONNECTION_SKEW, neighbors_probdist);
        choose_connections(rand, neighbors_probdist, NUM_OUTPUT_SYNAPSES, selected_neighbors);

        for (int j : selected_neighbors)
          {
            if (i!=j)
              {
                NetCon nc(pop_vec.at(i), pop_vec.at(j), Wmin, Wmax, 1.0);
                pop_vec.at(i)->targets.insert(make_pair(j, nc));
                num_output_synapses++;
              }
          }

      }
  

    // External inputs to inhibitory + excitatory population
    for (int i=start_inputs;i<offset_inputs;i++)
      {
        double Wmin = WMIN;
        double Wmax = 0.0;

        vector< std::pair<int,distance_t> > neighbors;
        vector< std::pair<int,double> > neighbors_probdist;
        vector< int > selected_neighbors;
          
        get_neighbors(i, start_neurons, offset_neurons, exc_connection_distance, neighbors);
        get_norm_probdist(neighbors, exc_connection_distance, neighbors_probdist);

        choose_connections(rand, neighbors_probdist, NUM_EXT_SYNAPSES, selected_neighbors);
        
        for (int j : selected_neighbors)
          {
            if (i!=j)
              {
                if ((pop_vec[i]->type == External) &&
                    (pop_vec[j]->type == Excitatory))
                      {
                        Wmax = WMAX;
                      }
                else
                      {
                        Wmax = 0.0;
                      }
                NetCon nc(pop_vec.at(i), pop_vec.at(j), Wmin, Wmax, 1.0);
                pop_vec.at(i)->targets.insert(make_pair(j, nc));
                num_synapses++;
                num_input_synapses++;
              }
          }
      }


  }

  
  void ConnectionGenerator::get_neighbors(int loc, int start, int offset, distance_t connection_distance,
                                          std::vector <std::pair<int,distance_t> >& neighbors)
  {
    for (int i = start; i<offset; i++)
      {
        // wrap-around distance on 1-D grid
        distance_t distance_mod = coords[i].phi > coords[loc].phi ? ((coords_extent + coords[loc].phi) - coords[i].phi) : (coords[loc].phi - coords[i].phi);
        distance_t distance_abs = abs(coords[loc].phi - coords[i].phi);
        distance_t distance = min(distance_mod, distance_abs);
          
        if (distance < connection_distance)
          {
            neighbors.push_back(make_pair(i, (distance_t)distance));
          }

      }
  }
  
  
  void ConnectionGenerator::choose_connections(std::default_random_engine& rand,
                                               const std::vector< std::pair<int,double> >& neighbors_probdist,
                                               size_t num_synapses,
                                               std::vector <int>& selected_neighbors)
  {
    vector<double> weights;
    for (auto p : neighbors_probdist)
      {
        weights.push_back(p.second);
      }

    std::discrete_distribution<> d(weights.begin(), weights.end());
    
    for (size_t i=0; i<num_synapses; i++)
      {
        int n = d(rand);
        selected_neighbors.push_back(neighbors_probdist[n].first);
      }
  }

  void ConnectionGenerator::get_norm_probdist(const std::vector< std::pair<int,distance_t> >& neighbors,
                                              distance_t connection_distance,
                                              std::vector< std::pair<int,double> >& neighbors_probdist)
  {
    double scale = connection_distance / 3.0;

    for (auto p : neighbors)
      {
        double w = snormpdf(p.second / scale);
        neighbors_probdist.push_back(make_pair(p.first, w));
      }
    
  }
  
  void ConnectionGenerator::get_skewnorm_probdist(const std::vector< std::pair<int,distance_t> >& neighbors,
                                                  distance_t connection_distance, double a,
                                                  std::vector< std::pair<int,double> >& neighbors_probdist)
  {
    double scale = connection_distance / 3.0;

    for (auto p : neighbors)
      {
        double w = skewnormpdf(p.second, a, 0.0, scale);
        neighbors_probdist.push_back(make_pair(p.first, w));
      }
    
  }

  ConnectionGenerator::~ConnectionGenerator() {};

}
