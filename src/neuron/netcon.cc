
#include <vector>
#include "conf.hh"
#include "netcon.hh"

namespace neuron
{
  
  using namespace std;

  void NetCon::update(int sender, double t_presyn, double Wmax, double gain)
  {
    double w=0.0;
    double t_postsyn = this->target->last_pulse;
    double x = t_presyn - t_postsyn;
    
    if (x > 0.0)
      {
        w = Wmax * (Wmax - A_LTP * exp(-x / Tau_LTP)) * gain;
      }
    else
      {
        w = (-A_LTD) * exp(-x / Tau_LTD) * gain;
      }

    this->s += w;
  }
}
