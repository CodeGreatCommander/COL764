#include "wordpiece.h"
#include "bpe.h"

using namespace std;

vector<string> headers;
bool delimiters[128]= {0};

uint32_t doc_count = 0;

unordered_map<string,uint32_t> word_count;

void json_wordcount(const string& line){

    string word = "";
    bool state = false;

    for(size_t i=0, l = line.length(); i<l; i++){

        char c = tolower(line[i]);
        if(c == '\"'&&i!=0&&line[i-1]!='\\'&&state){
            if(word.length())
            word_count[word]++;
            word.clear();
            state = false;
        }
        else if(c==' ' || (state&&delimiters[c])){
            if(word.length()==0)continue;
            if(state){
                word_count[word]++;
            }
            else{
                for(string& header: headers){
                    if(word==header){
                        state = true;
                        i++;
                        break;
                    }
                }
            }
            word.clear();
        }
        else if(c=='\\'){
            if(line[i+1]=='u'){i+=5;}
            else if(line[i+1]=='\\'){word.push_back('\\');i++;}
        }
        else{
            word.push_back(c);
        }
    }
}



int main(int argc, char*argv[]){
    string file_path = argv[1], option = argv[2];
    file_path+="/cord19-trec_covid-docs";
    cout<<file_path<<endl;
    cout<<option<<endl;

    headers = {"title", "abstract"};
    for(string& header: headers){
        header = "\""+header+"\":";
    }


    ifstream file(file_path);
    if(!file.is_open()){
        cout<<"Error opening file"<<endl;
        return 1;
    }
    string line;
    char delim[] = {' ','\n','\t',',','.',':',';','\"','\''};
    for(char c: delim){
        delimiters[c] = true;
    }

    for(size_t i =0;getline(file, line);i++){
        if(line!="")doc_count++;
    }
    doc_count = doc_count/2;
    file.clear();
    file.seekg(0);
    for(size_t i =1; getline(file, line);i++) {
        if(i>=doc_count)
        json_wordcount(line);
    }

    ofstream file2("output.dict");

    if(option=="0"){
        for(auto& x: word_count){
            file2<<x.first<<" ";
        }
    }
    else if(option == "1"){
        BPE bpe(word_count);
        bpe.train(20000);
        for(string& x: bpe.vocab)
        file2<<x<<" ";
    }
    else if(option=="2"){
        WordPiece wp(word_count);
        wp.train(20000);
        for(string& x: wp.vocab){
            file2<<x<<" ";
        }
    }

    file.close();
    file2.close();
    return 0;
}