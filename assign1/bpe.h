#include "utils.h"

using namespace std;

struct BPE{
    unordered_map<string,uint32_t> word_count;//actual word and its frequency
    vector<string> vocab;//vocabulary
    vector<pair<pair<size_t,size_t>,size_t>> rules;
    vector<string> words;
    unordered_map<string,size_t> word_index;//index of word
    unordered_map<string,size_t> vocab_index;//index of vocabulary
    vector<pair<list<size_t>,size_t>> word_decom;//decomposed word and its frequency
    set<pair<size_t,pair<size_t,size_t>>,PairComparator> inv_comb_count;//combination of two vocabularies and its frequency
    unordered_map<pair<size_t,size_t>,size_t,pair_hash> comb_count;//combination of two vocabularies and its frequency
    unordered_map<pair<size_t,size_t>,unordered_set<size_t>,pair_hash> comb_words;//combination of two vocabularies and its word index

    BPE(unordered_map<string,uint32_t> word_count);
    void train(uint32_t ncomb);
    void tokenise(const string& word);
};