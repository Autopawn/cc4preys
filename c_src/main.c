#include "agent.h"

int main(int argc, char const *argv[]){
    // Config
    agentconfig conf;
    conf.hops_lynx = 2;
    conf.hops_lynx_eat = 2;
    conf.hops_hare = 1;
    conf.prob_lynx_eat = 0.5;
    conf.prob_lynx_dead = 0.08;
    conf.prob_lynx_repr = 0.8;
    conf.prob_hare_repr = 0.1;
    // Agentsym:
    agentsim *ag = new_agentsim(conf,50,50,100);
    for(int i=1;i<=100;i++){
        agentsim_step(ag);
        printf("%5d %6d %6d\n",i,ag->n_hares,ag->n_lynxs);
    }

    agentsim_del(ag);

    return 0;
}
