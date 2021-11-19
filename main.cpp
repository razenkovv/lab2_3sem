#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <chrono>

#include "Sequences.h"
#include "HashTable.h"
#include "LRUCache.h"

class Timer {
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;
    std::chrono::time_point<clock_t> m_beg;
public:
    Timer() : m_beg(clock_t::now()) {}
    void reset() { m_beg = clock_t::now(); }
    double elapsed() const { return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count(); }
};

bool LRUCache_testing(unsigned int size) {
    LRUCache<int> a{size};
    bool check(true);
    std::ifstream fin;
    std::string Path1 = R"(C:\Users\Ivan\programming\labs_3sem\lab2_3sem\LRUCache_test.txt)";
    fin.open(Path1);
    if (!fin.is_open())
        throw std::runtime_error("\nLRUCache_test.txt: File wasn't opened\n");
    std::string tmp; getline(fin, tmp);
    int command, key, val;
    fin >> command;
    while (command != -1) {
        if (command == 0) {
            fin >> key;
            std::cout << "get " << key << "\n";
            a.get(key);
        } else {
            fin >> key; fin >> val;
            std::cout << "push " << key << "\n";
            a.push(key, val);
        }
        fin >> command;
    }
    int r, i(0);
    fin >> r;
    auto queue = a.get_lru_queue();
    while (r != -1) {
        if (i >= (*queue).size()) {
            check = false;
            break;
        }
        if (r != (*queue).get(i)) {
            check = false;
            break;
        }
        ++i;
        fin >> r;
    }
    if (i < (*queue).size())
        check = false;
    std::cout <<"\n\n";
    a.print();
    return check;
}

template <typename T>
void shuffle(ArraySequence<T> &v, int rnd) {
    for (int i = v.size() - 1; i > 0; --i) {
        std::default_random_engine gen(rnd);
        std::uniform_int_distribution<int> distr(0, i+1);
        std::swap(v[i], v[distr(gen)]);
    }
}

void generate_random_data(unsigned int n, int shuffle_rnd=1, int data_rnd=2) {
    ArraySequence<unsigned int> v(n);
    for (int i = 0; i < n; ++i) {
        v[i] = i + 1;
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

double find_in_file(int key, const std::string& filename) {
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("\ncsv file wasn't opened\n");
    std::string skip, k, v;
    std::getline(file, skip);
    while (std::getline(file, k, ',')) {
        std::getline(file, v, '\n');
        int key_ = std::stoi(k, nullptr);
        if (key_ == key) {
            double value = std::stod(v, nullptr);
            return value;
        }
    }
    throw std::runtime_error("\nfind_in_file: key wasn't found\n");
}

Pair<int, int>* generate_random_calls_with_cache(LRUCache<double>& cache, int n, int size, const std::string& filename, int percentage=10, int frequency=10, int rnd=1) {
    int found(0), not_found(0);
    double _size = size * percentage; _size /= 100;
    std::default_random_engine gen(rnd);
    std::uniform_int_distribution<int> distr_part(1, (int)_size);
    std::uniform_int_distribution<int> distr_all(1, size);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; (j < frequency-1) && (i < n); ++j) {
            ++i;
            int key = distr_part(gen);
            auto it = cache.get(key);
            if (it == cache.end()) {
                ++not_found;
                auto v = find_in_file(key,  filename);
                //std::cout << "not found: " << key << " " << v << "\n";
                cache.push(key, v);
            } else {
                ++found;
                //std::cout << "found: " << key << " " << (*it).get_second() << "\n";
            }
        }
        if (i < n) {
            int key = distr_all(gen);
            auto it = cache.get(key);
            if (it == cache.end()) {
                ++not_found;
                auto v = find_in_file(key, filename);
                //std::cout << "not found: " << key << " " << v << "\n";
                cache.push(key, v);
            } else {
                ++found;
                //std::cout << "found: " << key << " " << (*it).get_second() << "\n";
            };
        }
        std::cout << i << "\n";
    }
    auto* res = new Pair<int, int>(found, not_found);
    return res;
}

void generate_random_calls_without_cache(int n, int size, const std::string& filename, int percentage=10, int frequency=10, int rnd=1) {
    double _size = size * percentage; _size /= 100;
    std::default_random_engine gen(rnd);
    std::uniform_int_distribution<int> distr_part(1, (int)_size);
    std::uniform_int_distribution<int> distr_all(1, size);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; (j < frequency-1) && (i < n); ++j) {
            ++i;
            int key = distr_part(gen);
            find_in_file(key,  filename);
        }
        if (i < n) {
            int key = distr_all(gen);
            find_in_file(key, filename);
        }
        std::cout << i << "\n";
    }
}

void time_test(int a, int b, int x) {
    std::string path = R"(C:\Users\Ivan\programming\labs_3sem\lab2_3sem\RandomData.csv)";
    generate_random_data(a);
    LRUCache<double> cache(a / x);
    Timer t1;
    auto p = generate_random_calls_with_cache(cache, b, a, path);
    auto T1 = t1.elapsed();
    Timer t2;
    generate_random_calls_without_cache(b, a, path);
    auto T2 = t2.elapsed();

    std::string path2 = R"(C:\Users\Ivan\programming\labs_3sem\lab2_3sem\TimeTestResult)";
    std::ofstream fout(path2);
    fout << "\nWith LRUCache: " << T1 << " sec\n";
    fout << "\nNumber of calls that were found in the cache: " << (*p).get_first() << "; not found: " << (*p).get_second() << "\n";
    fout << "Number of elements in cache: " << cache.get_elem_number() << "\n";
    fout << "Max bucket height in the end: " << cache.get_max_bucket_height() << "\n";
    fout << "\n\nWithout LRUCache: " << T2 << " sec\n";
}

void test(int _size) {
    auto t = LRUCache_testing(_size);
    if (t) std::cout << "\n\nCORRECT\n";
    else std::cout << "\n\nERROR\n";
}

int main() {
    try {
        //test(4); //размер кэша
        time_test(10000, 100000, 20); //число записей в csv файле, число генерируемых вызовов и во сколько раз вместимость кэша меньше вместимости исходного хранилища
    } catch (const std::runtime_error &msg) {
       std::cerr << msg.what();
    }
    return 0;
}