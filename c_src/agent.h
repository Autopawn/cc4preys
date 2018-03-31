#ifndef AGENT_H
#define AGENT_H

#include <stdlib.h>
#include <stdio.h>

#define ROLL() (rand()/((float)RAND_MAX+1))

typedef struct {
    int x,y;
    int fat;
} lynx;

typedef struct {
    int x,y;
    int ded;
} hare;

typedef struct {
    int hops_lynx;
    int hops_lynx_eat;
    int hops_hare;
    float prob_lynx_eat;
    float prob_lynx_dead;
    float prob_lynx_repr;
    float prob_hare_repr;
} agentconfig;

typedef struct {
    int size;
    int n_hares;
    hare *hares;
    int n_lynxs;
    lynx *lynxs;
    agentconfig config;
} agentsim;

agentsim *new_agentsim(agentconfig config, int n_hares, int n_lynxs, int size);

void agentsim_step(agentsim *ag);

void agentsim_del(agentsim *ag);



#endif
