#pragma once

#include "HashTable.h"

template <typename T>
class LRUCache {
private:
    HashTable<T> cache;
    HashTable<List_Iterator<T>> key_to_list;
    ListSequence<int> lru_deck;
    unsigned int size;

public:
    explicit LRUCache(unsigned int _size) : cache(), key_to_list(), lru_deck(), size(_size) {}

    List_Iterator<T> get(unsigned int key);
    void set (Pair<unsigned int, T> p)
};

template<typename T>
List_Iterator<T> LRUCache<T>::get(unsigned int key) {
    return cache.get_(key);
}

template<typename T>
void LRUCache<T>::set(Pair<unsigned int, T> p) {

}
