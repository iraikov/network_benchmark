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



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <getopt.h>

#include "network.hh"

using namespace neuron;


void throw_err(char const* err_message)
{
  fprintf(stderr, "Error: %s\n", err_message);
  abort();
}

void print_usage_full(char** argv)
{
  cout << "Usage: " << string(argv[0]) << " [OPTIONS] " << endl <<
    "Options:" << endl <<
    "-h         Print this help" << endl <<
    "-t TSTOP   Simulation until TSTOP ms " << endl <<
    "-o PATH    Write spike times to PATH " << endl <<
    "-s SEED    Random number generator seed " << endl <<
    endl;
}

int main(int argc, char **argv)
{
  
  int h2,m2,s2,h3,m3,s3;
  time_t timer1;

  int seed = 0;
  string output_file;
  double t_stop = 100.0;
  bool opt_t_stop = false;
  bool opt_output_file = false;
  bool opt_random_seed = false;
  // parse arguments
  static struct option long_options[] = {
    {0,         0,                 0,  0 }
  };
  char c;
  int option_index = 0;
  while ((c = getopt_long (argc, argv, "ht:o:s:", long_options, &option_index)) != -1)
    {
      stringstream ss;
      switch (c)
        {
        case 0:
          break;
        case 't':
          opt_t_stop = true;
          ss << string(optarg);
          ss >> t_stop;
          break;
        case 'o':
          opt_output_file = true;
          ss << string(optarg);
          ss >> output_file;
          break;
        case 's':
          opt_random_seed = true;
          ss << string(optarg);
          ss >> seed;
          break;
        case 'h':
          print_usage_full(argv);
          exit(0);
          break;
        default:
          throw_err("Input argument format error");
        }
    }

  
  // Construct the network 
  Network n(t_stop, seed);
  
  // Compute the beginning wall-clock time
  time(&timer1);
  h2 = localtime(&timer1)->tm_hour;
  m2 = localtime(&timer1)->tm_min;
  s2 = localtime(&timer1)->tm_sec;
  
  
  printf("Starting simulation; tstop = %f\n", t_stop);
  n.start();
  
  time(&timer1);
  h3 = localtime(&timer1)->tm_hour;
  m3 = localtime(&timer1)->tm_min;
  s3 = localtime(&timer1)->tm_sec;
  
  printf("# of Neurons : %d, Total # of Synapses : %lu, input synapses: %lu output synapses: %lu\n",
         n.nb_neurons,n.num_synapses,n.num_input_synapses,n.num_output_synapses);
  printf("# of Spikes : %lu\n", n.outputs.size());
  
  printf("Computation time : %d sec\n",(h3-h2)*3600+(m3-m2)*60+s3-s2);
  
  if (opt_output_file)
    {
      printf("Writing in %s...",output_file.c_str());		
      FILE *file = fopen(output_file.c_str(),"w");
  
      int k = 0;
      char buf[20];
      while (!(n.outputs.empty()))
        {
          k++;
          Spike s = n.outputs.front();
          n.outputs.pop();
          sprintf(buf,"%d",(s.sender));
          fputs(buf,file);
          sprintf(buf,"%f",s.t*Taum);
          fputs(" ",file);
          fputs(buf,file);
          fputs("\n",file);
        }
      fclose(file);
    }
  printf("All done!\n");
  
  
  return 0;
}



