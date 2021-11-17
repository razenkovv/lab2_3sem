#pragma once

#include "HashTable.h"

template <typename T>
class LRUCache {
private:
    HashTable<T> cache;
    HashTable<List_Iterator<unsigned int>> key_to_list;
    ListSequence<unsigned int> lru_queue;
    unsigned int size;

public:
    explicit LRUCache(unsigned int _size) : cache(_size), key_to_list(_size), lru_queue(), size(_size) {}

    Hash_Iterator<T> get(unsigned int key); // вовзращает итератор. если ключ не найден, вернется past-the-end итератор
    void push(unsigned int key, const T& value); // кладет значение в кэш

    Hash_Iterator<T> end() { return cache.end(); }
};

template<typename T>
Hash_Iterator<T> LRUCache<T>::get(unsigned int key) {
    if (cache.get_(key) != cache.end()) { // если ключ есть
        auto list_it = (*(key_to_list.get_(key))).get_second();
        if (list_it != lru_queue.begin()) {
            lru_queue.erase(list_it); // надо переместить ключ в очереди
            lru_queue.push_front(key);
            key_to_list.insert(key, lru_queue.begin()); // обновление таблицы key_to_list
        }
    }
    return cache.get_(key);
}

template<typename T>
void LRUCache<T>::push(unsigned int key, const T& value) {
    if (cache.get_(key) != cache.end()) { //если ключ уже есть в кэше, надо обновить информацию о последнем вызове
        auto list_it = (*(key_to_list.get_(key))).get_second(); // находим ключ в таблице key_to_list, получаем итератор, который указывает на позицию ключа в очереди
        if (list_it == lru_queue.begin()) { // если ключ и так на первом месте, очередь трогать не надо
            cache.insert(key, value); // надо только обновить значение по ключу в кэше
            return;
        }
        lru_queue.erase(list_it); //удаляем ключ из своего места в очереди по итератору list_it

    } else {
        if (lru_queue.size() == size) { //если кэш заполнен
            unsigned int last_used_key = *(lru_queue.last());
            lru_queue.pop_back(); //убираем из очереди последний элемент
            cache.remove(last_used_key); //убираем этот элемент из кэша
            key_to_list.remove(last_used_key); //убираем его из таблицы key_to_list
        }
    }

    lru_queue.push_front(key); //ставим ключ в начало очереди
    key_to_list.insert(key, lru_queue.begin()); //обновляем информацию об этом ключе в таблице key_to_list
    cache.insert(key, value); //вставляем ключ в кэш (обновляем значение, если ключ уже был в кэше)
}