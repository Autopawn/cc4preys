#include "agent.h"

int dist(int size, int x1, int y1, int x2, int y2){
    int dx = abs(x1-x2);
    dx = dx<=size-dx? dx:size-dx;
    int dy = abs(y1-y2);
    dy = dy<=size-dy? dy:size-dy;
    return dx<dy? dy:dx;
}

agentsim *new_agentsim(agentconfig config, int n_hares, int n_lynxs, int size){
    agentsim *ag = malloc(sizeof(agentsim));
    ag->config = config;
    ag->size = size;
    // Place hares
    ag->n_hares = n_hares;
    ag->hares = malloc(sizeof(hare)*n_hares);
    for(int i=0;i<ag->n_hares;i++){
        ag->hares[i].x = rand()%ag->size;
        ag->hares[i].y = rand()%ag->size;
        ag->hares[i].ded = 0;
    }
    // Place lynxs
    ag->n_lynxs = n_lynxs;
    ag->lynxs = malloc(sizeof(lynx)*ag->n_lynxs);
    for(int i=0;i<ag->n_lynxs;i++){
        ag->lynxs[i].x = rand()%ag->size;
        ag->lynxs[i].y = rand()%ag->size;
        ag->lynxs[i].fat = 0;
    }
    //
    return ag;
}

void agentsim_step(agentsim *ag){
    // Hare hop:
    {for(int i=0;i<ag->n_hares;i++){
        hare *ha = &ag->hares[i];
        int jx = rand()%(2*ag->config.hops_hare+1)-ag->config.hops_hare;
        int jy = rand()%(2*ag->config.hops_hare+1)-ag->config.hops_hare;
        ha->x = (ha->x+jx+ag->size)%ag->size;
        ha->y = (ha->y+jy+ag->size)%ag->size;
    }}
    // Lynx hop and unfat:
    {for(int i=0;i<ag->n_lynxs;i++){
        lynx *li = &ag->lynxs[i];
        li->fat = 0;
        int jx = rand()%(2*ag->config.hops_lynx+1)-ag->config.hops_lynx;
        int jy = rand()%(2*ag->config.hops_lynx+1)-ag->config.hops_lynx;
        li->x = (li->x+jx+ag->size)%ag->size;
        li->y = (li->y+jy+ag->size)%ag->size;
    }}
    // Shuffle hares:
    {for(int i=0;i<ag->n_hares;i++){
        int t = i+rand()%(ag->n_hares-i);
        hare aux = ag->hares[i];
        ag->hares[i] = ag->hares[t];
        ag->hares[t] = aux;
    }}
    // Shuffle lynxs:
    {for(int i=0;i<ag->n_lynxs;i++){
        int t = i+rand()%(ag->n_lynxs-i);
        lynx aux = ag->lynxs[i];
        ag->lynxs[i] = ag->lynxs[t];
        ag->lynxs[t] = aux;
    }}
    // Hare being eaten
    {for(int i=0;i<ag->n_hares;i++){
        hare *ha = &ag->hares[i];
        for(int j=0;j<ag->n_lynxs;j++){
            lynx *li = &ag->lynxs[j];
            if(li->fat) continue;
            int d = dist(ag->size,ha->x,ha->y,li->x,li->y);
            if(d<=ag->config.hops_lynx_eat && ROLL()<ag->config.prob_lynx_eat){
                li->fat = 1;
                ha->ded = 1;
                break;
            }
        }
    }}
    // Kill and reproduce hares
    {
        int n_hares1 = 0;
        hare *hares1 = malloc(sizeof(hare)*2*ag->n_hares);
        for(int i=0;i<ag->n_hares;i++){
            hare *ha = &ag->hares[i];
            if(ha->ded) continue;
            // Reproduce hare
            if(ROLL()<ag->config.prob_hare_repr){
                // Hop hare after copying it:
                int jx = rand()%(2*ag->config.hops_hare+1)-ag->config.hops_hare;
                int jy = rand()%(2*ag->config.hops_hare+1)-ag->config.hops_hare;
                hares1[n_hares1] = *ha;
                hares1[n_hares1].x = (hares1[n_hares1].x+jx+ag->size)%ag->size;
                hares1[n_hares1].y = (hares1[n_hares1].y+jy+ag->size)%ag->size;
                n_hares1 += 1;
            }
            // Save alive hare
            hares1[n_hares1] = *ha;
            n_hares1 += 1;
        }
        ag->n_hares = n_hares1;
        free(ag->hares);
        ag->hares = hares1;
    }
    // Kill and reproduce lynxs
    {
        int n_lynxs1 = 0;
        lynx *lynxs1 = malloc(sizeof(lynx)*2*ag->n_lynxs);
        for(int i=0;i<ag->n_lynxs;i++){
            lynx *li = &ag->lynxs[i];
            // Reproduce lynx:
            if(li->fat && ROLL()<ag->config.prob_lynx_repr){
                int jx = rand()%(2*ag->config.hops_lynx+1)-ag->config.hops_lynx;
                int jy = rand()%(2*ag->config.hops_lynx+1)-ag->config.hops_lynx;
                lynxs1[n_lynxs1] = *li;
                lynxs1[n_lynxs1].x = (lynxs1[n_lynxs1].x+jx+ag->size)%ag->size;
                lynxs1[n_lynxs1].y = (lynxs1[n_lynxs1].y+jy+ag->size)%ag->size;
                n_lynxs1 += 1;
            }
            // Save alive lynx
            if(!(ROLL()<ag->config.prob_lynx_dead)){
                lynxs1[n_lynxs1] = *li;
                n_lynxs1 += 1;
            }
        }
        ag->n_lynxs = n_lynxs1;
        free(ag->lynxs);
        ag->lynxs = lynxs1;
    }
}


void agentsim_del(agentsim *ag){
    free(ag->lynxs);
    free(ag->hares);
    free(ag);
}
