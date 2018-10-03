
#include <vector>
#include "conf.hh"
#include "netcon.hh"

namespace neuron
{
  
  using namespace std;

  void NetCon::update(int sender, double t_presyn, double gain)
  {
    double t_postsyn = this->target->last_pulse;
    double x = t_presyn - t_postsyn;
    double w=0.0;
    if ((Wmax > 0.0) && (gain > 1e-6))
      {

        if (x > 0.0)
          {
            w = (Wmax - A_LTP * exp(-x / Tau_LTP)) * gain;
            if (w < 0.0) w = 0.0;
          }
        else
          {
            w = (-A_LTD) * exp(-x / Tau_LTD) * gain;
          }

        /*
        if (gain > 0.0)
          {
            printf("stdp update %d -> %d: x = %f gain = %f w = %f\n", this->source->id, this->target->id, x, gain, w);
          }
        */

      }
    else if (Wmax > 0.0)
      {
        w = (-A_LTD) * exp(-x / Tau_LTD);
        if (w < Wmin) w = Wmin;
      }
    this->s += w;
  }
}
