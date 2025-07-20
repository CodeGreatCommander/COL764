#pragma once
#include <iostream>
#include <fstream>
#include <utility>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <set>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>
#include <cmath>

using namespace std;

struct pair_hash {
    template <class T1, class T2> 
    size_t operator()(const pair<T1, T2> &p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        size_t seed = h1;
        return h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

struct PairComparator {
    bool operator()(const std::pair<size_t, std::pair<size_t, size_t>>& lhs,
                    const std::pair<size_t, std::pair<size_t, size_t>>& rhs) const;
};

struct PairComparatorDouble {
    bool operator()(const std::pair<double, std::pair<size_t, size_t>>& lhs,
                    const std::pair<double, std::pair<size_t, size_t>>& rhs) const;
};

struct trie{
    unordered_map<char,trie*> children;
    bool is_end;
    size_t index;
    trie();
    void insert(const std::string& word, size_t index);
    std::vector<size_t> tokenise(const std::string& word, bool wordpiece);
    size_t search(const std::string& word);
    ~trie();
};