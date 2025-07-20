#include "wordpiece.h"

using namespace std;



WordPiece::WordPiece(unordered_map<string,uint32_t> word_count):word_count(word_count){
    bool check[128] = {0};
    for(const auto& t:word_count){
        bool f= true;
        for(const char& c: t.first){
            if(!check[c]){
                check[c] = true;
                string ch = string(1,c);
                vocab_index[ch] = vocab.size();
                vocab.push_back(ch);
                vocab_count.push_back(0);
                ch="@@"+ch;
                vocab_index[ch] = vocab.size();
                vocab.push_back(ch);
                vocab_count.push_back(0);
            }
            if(f){vocab_count[vocab_index[string(1,c)]]+=t.second;f=false;}
            else{vocab_count[vocab_index["@@"+string(1,c)]]+=t.second;}
        }
    }

    for(auto& t: word_count){
        word_index[t.first] = word_index.size();
        words.push_back(t.first);
        word_decom.push_back({list<size_t>(),t.second});
        string prev = "";
        bool flag = true;
        for(const char& c: t.first){
            string ch;
            if(flag){ch =string(1,c);flag=false;}
            else{ch="@@"+string(1,c);}
            word_decom.back().first.push_back(vocab_index[ch]);
            if(prev!=""){
                pair<size_t,size_t> p = {vocab_index[prev],vocab_index[ch]};
                if(scores.find(p)!=scores.end())inv_comb_count.erase({scores[p],p});
                comb_count[p]+=t.second;
                scores[p]=((1.0*comb_count[p]*comb_count[p])/vocab_count[p.first])/vocab_count[p.second];
                inv_comb_count.insert({scores[p],p});
                comb_words[p].insert(words.size()-1);
            }
            prev = ch;
        }
    }
}


void WordPiece::train(uint32_t ncomb){
    for(uint32_t i=1; i<=ncomb; i++){
        if(i%100==0){
            cout<<"\r"<<i<<"/"<<ncomb<<flush;
        }
        auto it = inv_comb_count.rbegin();
        double max_count = it->first;
        pair<size_t,size_t> max_comb= it->second;
        if(max_count==0)break;
        string new_word = vocab[max_comb.first]+vocab[max_comb.second].substr(2);
        vocab_index[new_word] = vocab.size();
        vocab.push_back(new_word);
        inv_comb_count.erase({max_count,max_comb});
        scores.erase(max_comb);
        vocab_count.push_back(comb_count[max_comb]);
        vocab_count[max_comb.first]-=comb_count[max_comb];
        vocab_count[max_comb.second]-=comb_count[max_comb];
        comb_count.erase(max_comb);
        for(const size_t& w: comb_words[max_comb]){
            list<size_t>& l = word_decom[w].first;
            auto it = l.begin();
            auto it2 = l.begin();
            it2++;
            while(it2!=l.end()){
                if(*it==max_comb.first&&*it2==max_comb.second){
                    if(it!=l.begin()){
                        it--;
                        it2--;
                        pair<size_t,size_t> p = {*it,*it2};
                        if(inv_comb_count.find({scores[p],p})!=inv_comb_count.end()){
                            inv_comb_count.erase({scores[p],p});
                        }
                        comb_count[p]-=word_decom[w].second;
                        scores[p]=((1.0*comb_count[p]*comb_count[p])/vocab_count[p.first])/vocab_count[p.second];
                        if(comb_count[p]>0)inv_comb_count.insert({scores[p],p});
                        p.second = vocab.size()-1;
                        if(inv_comb_count.find({scores[p],p})!=inv_comb_count.end()){
                            inv_comb_count.erase({scores[p],p});
                        }
                        comb_count[p]+=word_decom[w].second;
                        scores[p]=((1.0*comb_count[p]*comb_count[p])/vocab_count[p.first])/vocab_count[p.second];
                        if(comb_count[p]>0)inv_comb_count.insert({scores[p],p});
                        comb_words[{*it,vocab.size()-1}].insert(w);
                        it++;
                        it2++;
                    }
                    it2++;
                    if(it2!=l.end()){
                        it++;

                        pair<size_t,size_t> p = {*it,*it2};
                        if(inv_comb_count.find({scores[p],p})!=inv_comb_count.end()){
                            inv_comb_count.erase({scores[p],p});
                        }
                        comb_count[p]-=word_decom[w].second;
                        scores[p]=((1.0*comb_count[p]*comb_count[p])/vocab_count[p.first])/vocab_count[p.second];
                        if(comb_count[p]>0)inv_comb_count.insert({scores[p],p});
                        p.first = vocab.size()-1;
                        if(inv_comb_count.find({scores[p],p})!=inv_comb_count.end()){
                            inv_comb_count.erase({scores[p],p});
                        }
                        comb_count[p]+=word_decom[w].second;
                        scores[p]=((1.0*comb_count[p]*comb_count[p])/vocab_count[p.first])/vocab_count[p.second];
                        if(comb_count[p]>0)inv_comb_count.insert({scores[p],p});
                        comb_words[p].insert(w);
                        it--;
                    }
                    it2--;
                    it = l.erase(it);
                    it = l.erase(it);
                    it = l.insert(it,vocab.size()-1);
                    it2 = it;
                    it--;
                    if(it2==l.end())break;
                }
                it++;
                it2++;
            }
        }
        comb_words.erase(max_comb);
    }
    inv_comb_count.clear();
    comb_count.clear();
    comb_words.clear();
}
