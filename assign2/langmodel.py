import numpy as np
from collections import Counter

class GlobLangModel:
    def __init__(self):
        self.index_count = Counter()
        self.total_count = 0

    def train(self, data):
        for ind in data:
            self.index_count[ind] += 1
            self.total_count += 1
        

    def get_prob(self, query_index_list):
        term_prob = [self.index_count[ind[0]] / self.total_count for ind in query_index_list]
        return np.array(term_prob)
       
    
class LangModel:
    def __init__(self, u = 500):
        self.index_count = Counter()
        self.total_count = 0
        self.u = u

    def train(self, data):
        for ind in data:
            self.index_count[ind] += 1
            self.total_count += 1
        

    def get_prob(self, query_index_list, global_term_prob):
        local_term_prob = np.array([self.index_count[ind[0]] for ind in query_index_list])
        weights = np.array([ind[1] for ind in query_index_list])
        comb_term_prob = (self.u*global_term_prob + local_term_prob)/(self.u + self.total_count)
        weights = weights[comb_term_prob > 0]
        comb_term_prob = np.log(comb_term_prob[comb_term_prob > 0])
        comb_term_prob = comb_term_prob*weights
        prob =  np.sum(comb_term_prob)
        return prob
