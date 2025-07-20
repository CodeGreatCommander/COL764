#include <bits/stdc++.h>
#include <sstream>

using namespace std;

// Function to split a string based on multiple delimiters
vector<string> split(const string& str, const string& delimiters) {
    vector<string> tokens;
    size_t start = 0;
    size_t end = str.find_first_of(delimiters);

    while (end != string::npos) {
        if (end != start) {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find_first_of(delimiters, start);
    }

    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}

int main() {
    string file = "/home/rohan/col764/assign1/data/cord19-trec_covid-qrels";
    ifstream in(file);
    ofstream out("fscore.txt");
    string line;
    string delimiters = ",{}:\" \t";
    map<string, set<string>> qrels,qurels;
    while (getline(in, line)) {
        vector<string> tokens = split(line, delimiters);
        if(stoi(tokens[5])!=0)
        qrels[tokens[1]].insert(tokens[3]);
        else
        qurels[tokens[1]].insert(tokens[3]);
    }

    in.close();
    vector<vector<double>> p1(25),r1(25),f1(25),p2(25),r2(25),f2(25);
    vector<int> correct(25);
    in = ifstream("/home/rohan/col764/assign1/result");
    while (getline(in, line)) {
        vector<string> tokens = split(line, delimiters);
        if(tokens.size()!=4)continue;
        int ni = stoi(tokens[0]);
        ni--;
        bool check= qrels[tokens[0]].find(tokens[2])!=qrels[tokens[0]].end();
        bool check2 = qurels[tokens[0]].find(tokens[2])!=qurels[tokens[0]].end();
        double size = p1[ni].size()+1;
        if(check)correct[ni]++;
        p1[ni].push_back(correct[ni]/size);
        r1[ni].push_back(1.0*correct[ni]/qrels[tokens[0]].size());
        if(p1[ni].back()+r1[ni].back()==0)f1[ni].push_back(0);
        else
        f1[ni].push_back(2*p1[ni].back()*r1[ni].back()/(p1[ni].back()+r1[ni].back()));
        if(check||check2){
            size = p2[ni].size()+1;
            p2[ni].push_back(correct[ni]/size);
            r2[ni].push_back(1.0*correct[ni]/qrels[tokens[0]].size());
            if(p2[ni].back()+r2[ni].back()==0)f2[ni].push_back(0);
            else
            f2[ni].push_back(2*p2[ni].back()*r2[ni].back()/(p2[ni].back()+r2[ni].back()));
        }
    }
    for(size_t i =0;i<25;i++){
        vector<int> points = {10,20,50,100};
        // cout<<qrels[ni].size()<<endl;
        for(int& p:points){
            out<<i+1<<"\t";
            out<<p<<"\t"<<p1[i][p-1]<<"\t"<<r1[i][p-1]<<"\t"<<f1[i][p-1];
            if(p2[i].size()>=p)
            out<<"\t"<<p2[i][p-1]<<"\t"<<r2[i][p-1]<<"\t"<<f2[i][p-1]<<"\t";
            out<<endl;
        }
    }
    return 0;
}