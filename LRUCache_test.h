#pragma once

#include <random>
#include <ctime>
#include <fstream>

#include "HashTable.h"
#include "LRUCache.h"

template <typename T>
void shuffle(ArraySequence<T> &v, int rnd) {
    for (int i = v.size() - 1; i > 0; --i) {
        std::default_random_engine gen(rnd);
        std::uniform_int_distribution<int> distr(0, i+1);
        std::swap(v[i], v[distr(gen)]);
    }
}

void generate_random_data(unsigned int n, int shuffle_rnd, int data_rnd) {
    ArraySequence<unsigned int> v(n);
    for (int i = 0; i < n; ++i) {
        v[i] = i;
    }
    shuffle(v, shuffle_rnd);
    std::default_random_engine gen(data_rnd);
    std::uniform_real_distribution<double> distr(-1000, 1000);
    std::string Path2 = R"(C:\Users\Ivan\programming\labs_3sem\lab2_3sem\RandomData.csv)";
    std::ofstream file(Path2);
    file << "key,value\n";
    for (int i = 0; i < n; ++i) {
        file << v[i] << ",";
        file << distr(gen) << "\n";
    }
}