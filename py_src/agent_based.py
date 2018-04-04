from numba import jit
import numpy as np
import random

default_params = {
    # Hops:
    "hops_lynx": 2,
    "hops_lynx_eat": 2,
    "hops_hare": 1,
    # Probs:
    "prob_hare_hop": 1.0,
    "prob_hare_repr": 0.1,
    "prob_lynx_hop": 1.0,
    "prob_lynx_eat": 0.9,
    "prob_lynx_dead": 0.08,
    "prob_lynx_repr": 0.8,
}

class AgentModel:
    @jit
    def __init__(self,size,n_hares,n_lynxs,prs=default_params,seed=None):
        if seed is None: seed = np.random.randint(0,10000000)
        self.rgen = np.random.RandomState(seed)
        self.params = prs
        self.size = size
        # Hares
        self.hares = np.zeros((self.size,self.size),dtype="int")
        for i in range(n_hares):
            self.hares[self.rgen.randint(0,self.size),self.rgen.randint(0,self.size)] += 1
        self.n_hares = n_hares
        # Lynxs
        self.lynxs = np.zeros((self.size,self.size),dtype="int")
        for i in range(n_lynxs):
            self.lynxs[self.rgen.randint(0,self.size),self.rgen.randint(0,self.size)] += 1
        self.n_lynxs = n_lynxs
        # Hunts
        self.hunts = np.zeros((self.size,self.size),dtype="int")

    @jit
    def step(self):
        # Dont step when there are 0 lynxs
        self.n_interactions = 0
        if self.n_lynxs==0:
            return False
        # Hops
        for x in range(self.size):
            for y in range(self.size):
                # Hare hop
                for k in range(self.hares[x,y]):
                    if self.rgen.rand()<self.params["prob_hare_hop"]:
                        sx = self.rgen.randint(-self.params["hops_hare"],self.params["hops_hare"]+1)
                        nx = (x+sx)%self.size
                        sy = self.rgen.randint(-self.params["hops_hare"],self.params["hops_hare"]+1)
                        ny = (y+sy)%self.size
                        self.hares[x,y] -= 1
                        self.hares[nx,ny] += 1
                # Lynx hop
                for k in range(self.lynxs[x,y]):
                    if self.rgen.rand()<self.params["prob_lynx_hop"]:
                        sx = self.rgen.randint(-self.params["hops_lynx"],self.params["hops_lynx"]+1)
                        nx = (x+sx)%self.size
                        sy = self.rgen.randint(-self.params["hops_lynx"],self.params["hops_lynx"]+1)
                        ny = (y+sy)%self.size
                        self.lynxs[x,y] -= 1
                        self.lynxs[nx,ny] += 1
        # Lynx eat
        rang = self.params["hops_lynx_eat"]
        rang2 = (2*rang+1)*(2*rang+1)
        self.hunts = np.zeros((self.size,self.size),dtype="int")
        for x in range(self.size):
            for y in range(self.size):
                for k in range(self.hares[x,y]):
                    # In order to give a random preferency to surrounding lynxs:
                    p = self.rgen.randint(0,rang2)
                    eaten = False
                    for i in range(rang2):
                        pp = p+i-rang2 if p+i>=rang2 else p+i
                        xx = (x+(pp%(2*rang+1))-rang)%self.size
                        yy = (y+(pp//(2*rang+1))-rang)%self.size
                        self.n_interactions += self.lynxs[xx,yy]
                        if not eaten:
                            iters = self.lynxs[xx,yy]
                            for l in range(iters):
                                if self.rgen.rand()<self.params["prob_lynx_eat"]:
                                    self.hares[x,y] -= 1
                                    self.lynxs[xx,yy] -= 1
                                    self.hunts[xx,yy] += 1
                                    eaten=True
                                    break
        # Hare reproduce
        new_hares = np.zeros((self.size,self.size),dtype="int")
        for x in range(self.size):
            for y in range(self.size):
                iters = self.hares[x,y]
                for k in range(iters):
                    if self.rgen.rand()<self.params["prob_hare_repr"]:
                        sx = self.rgen.randint(-self.params["hops_hare"],self.params["hops_hare"]+1)
                        nx = (x+sx)%self.size
                        sy = self.rgen.randint(-self.params["hops_hare"],self.params["hops_hare"]+1)
                        ny = (y+sy)%self.size
                        new_hares[nx,ny] += 1
        self.hares += new_hares
        # Lynx reproduce
        for x in range(self.size):
            for y in range(self.size):
                iters = self.hunts[x,y]
                for k in range(iters):
                    if self.rgen.rand()<self.params["prob_lynx_repr"]:
                        sx = self.rgen.randint(-self.params["hops_lynx"],self.params["hops_lynx"]+1)
                        nx = (x+sx)%self.size
                        sy = self.rgen.randint(-self.params["hops_lynx"],self.params["hops_lynx"]+1)
                        ny = (y+sy)%self.size
                        self.lynxs[nx,ny] += 1
        # Recover hunts:
        self.lynxs += self.hunts
        # Lynx die
        for x in range(self.size):
            for y in range(self.size):
                iters = self.lynxs[x,y]
                for k in range(iters):
                    if self.rgen.rand()<self.params["prob_lynx_dead"]:
                        self.lynxs[x,y] -= 1
        # Update sums
        self.n_lynxs = np.sum(self.lynxs)
        self.n_hares = np.sum(self.hares)
        # Cool and good
        return True
