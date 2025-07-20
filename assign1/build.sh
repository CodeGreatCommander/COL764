mkdir -p exe
g++ -g -c -o exe/utils.o utils.cpp
g++ -g -c -o exe/bpe.o bpe.cpp
g++ -g -c -o exe/wordpiece.o wordpiece.cpp
g++ -g -c -o exe/dict_cons.o dict_cons.cpp
g++ -g -c -o exe/invidx.o invidx_cons.cpp
g++ -g -c -o exe/tfidf.o tf_idf.cpp
g++ -g -o exe/dict_cons exe/utils.o exe/bpe.o exe/wordpiece.o exe/dict_cons.o
g++ -g -o exe/invidx_cons exe/utils.o exe/bpe.o exe/wordpiece.o exe/invidx.o
g++ -g -o exe/tfidf exe/utils.o exe/bpe.o exe/wordpiece.o exe/tfidf.o


rm -rf exe/*.o