#pragma once
#include "Sequences.h"
#include "Pair.h"

template <typename T>
class Hash_Iterator;

template <typename T>
class HashTable {
protected:
    ArraySequence<ListSequence<Pair<unsigned int, T>>> table; //хеш-таблица
    unsigned int table_size; //размер таблицы
    unsigned int elem_number; //число элементов в таблице
    const unsigned int q_factor; //по умолчанию 5
    const double hash_const; //0.5 * (sqrt(5) - 1)
    double load_factor; //отношение elem_number к table_size
    const double rehash_factor; //0.75
    unsigned int hash_func(unsigned int key); //хеш-функция

public:
    HashTable() : table(), table_size(0), elem_number(0), q_factor(5), hash_const(0.5 * (sqrt(5) - 1)), load_factor(0), rehash_factor(0.75) {}
    explicit HashTable(unsigned int n) : table(n, ListSequence<Pair<unsigned int, T>>()), table_size(n), elem_number(0),
                                            q_factor(5), hash_const(0.5 * (sqrt(5) - 1)), load_factor(0), rehash_factor(0.75) {}
    ~HashTable();

    bool insert(Pair<unsigned int, T>); //вставка нового ключа. если он уже есть - значение изменяется
    bool remove(unsigned int key); //удаление ключа. если его нет, возвращается false
    bool contain(unsigned int key); //возвращает true если ключ есть, false - если ключа нет
    bool get(unsigned int key, T& value); //в value будет записано значение, найденное по ключу (если ключ есть, иначе возвращается false)

    List_Iterator<Pair<unsigned int, T>> get_(unsigned int key); //тоже get, но вовзращает итератор. если ключ не найден, вернется past-the-end итератор

    void rehash_table(); //увеличение размера хеш-таблицы в q_factor раз при достижении load_factor значения rehash_factor.

    void print();
    void get_size();

    ArraySequence<ListSequence<Pair<unsigned int, T>>>* get_array() { return &table; }

    Hash_Iterator<T> begin();
    Hash_Iterator<T> end();
};

template <typename T>
class Hash_Iterator : public HashTable<T> {
protected:
    HashTable<T>* m_table;
    List_Iterator<Pair<unsigned int, T>> list_it;
    Array_Iterator<ListSequence<Pair<unsigned int, T>>> array_it;

public:
    Hash_Iterator(HashTable<T>* table, List_Iterator<Pair<unsigned int, T>> list_it_,
                        Array_Iterator<ListSequence<Pair<unsigned int, T>>> array_it_) : m_table(table), list_it(list_it_), array_it(array_it_) {};

    Pair<unsigned int, T>& operator*() {
        if (list_it == (*array_it).end()) {
            throw std::runtime_error("\n[Hash_Iterator] operator *: index out of range\n");
        }
        return *list_it;
    }

    Hash_Iterator& operator++() {
        if (list_it == (*array_it).end()) {
            throw std::runtime_error("\n[Hash_Iterator] operator ++: index out of range\n");
        }
        if (list_it != (*array_it).last()) {
            ++list_it;
        } else  {
            do {
                ++array_it;
                if (array_it == m_table->get_array()->end())
                    break;
            } while ((*array_it).empty());
        }
        if (array_it != m_table->get_array()->end()) {
            list_it = (*array_it).begin();
        }
        else {
            array_it = m_table->get_array()->last();
            list_it = (*array_it).end();
        }
        return *this;
    }

    friend bool operator!= (Hash_Iterator &it1, Hash_Iterator &it2) {
        bool a(it1.array_it == it2.array_it);
        bool b(it1.list_it == it2.list_it);
        return !(a & b);
    }

    friend bool operator== (Hash_Iterator &it1, Hash_Iterator &it2) {
        bool a(it1.array_it == it2.array_it);
        bool b(it1.list_it == it2.list_it);
        return (a & b);
    }
};

template<typename T>
Hash_Iterator<T> HashTable<T>::begin() {
    auto array_it = table.begin();
    for (auto &a: table) {
        if (!a.empty())
            return Hash_Iterator<T>(this, a.begin(), array_it);
        ++array_it;
    }
    throw std::runtime_error("\n[Hash_Table] begin(): hash_table is empty\n");
}

template<typename T>
Hash_Iterator<T> HashTable<T>::end() {
    if (elem_number == 0)
        throw std::runtime_error("\n[Hash_Table] end(): hash_table is empty\n");
    return Hash_Iterator<T>(this, (*(this->table.last())).end(), this->table.last());
}

template<typename T>
unsigned int HashTable<T>::hash_func(unsigned int key) {
    unsigned int res = table_size * key * hash_const;
    return res % table_size;
}

template<typename T>
bool HashTable<T>::insert(Pair<unsigned int, T> p) {
    bool found(false);
    auto i = hash_func(p.get_first());
    for (auto &a: table[i]) {
        if (a.get_first() == p.get_first()) {
            a.get_second() = p.get_second();
            found = true;
            break;
        }
    }
    if (!found) {
        table[i].push_back(p);
        ++elem_number;
        load_factor = (double) elem_number / table.size();
    }
    if (load_factor > rehash_factor) rehash_table();
    return found;
}

template<typename T>
bool HashTable<T>::remove(unsigned int key) {
    bool found(false);
    auto i = hash_func(key);
    auto it = table[i].begin();
    for (auto &a: table[i]) {
        if (a.get_first() == key) {
            table[i].erase(it);
            found = true;
            break;
        }
        ++it;
    }
    if (found) {
        --elem_number;
        load_factor = (double) elem_number / table.size();
    }
    return found;
}

template<typename T>
bool HashTable<T>::contain(unsigned int key) {
    bool found(false);
    auto i = hash_func(key);
    for (auto &a: table[i]) {
        if (a.get_first() == key) {
            found = true;
            break;
        }
    }
    return found;
}

template<typename T>
bool HashTable<T>::get(unsigned int key, T& value) {
    bool found(false);
    auto i = hash_func(key);
    for (auto &a: table[i]) {
        if (a.get_first() == key) {
            found = true;
            value = a.get_second();
        }
    }
    return found;
}

template<typename T>
List_Iterator<Pair<unsigned int, T>> HashTable<T>::get_(unsigned int key) {
    auto i = hash_func(key);
    for (auto a = table[i].begin(); a != table[i].end(); ++a) {
        if ((*a).get_first() == key) {
            return a;
        }
    }
    return table[table_size - 1].end();
}

template<typename T>
void HashTable<T>::rehash_table() {
    table_size *= q_factor;
    ArraySequence<ListSequence<Pair<unsigned int, T>>> new_table(table_size, ListSequence<Pair<unsigned int, T>>());
    for (auto &l: table) {
        for (auto &a: l) {
            auto i = hash_func(a.get_first());
            new_table[i].push_back(a);
        }
    }
    table = new_table;
    new_table.clear();
}

template<typename T>
void HashTable<T>::print() {
    for (auto &a: table) {
        if (!a.empty()) a.print();
        //
        else std::cout << "xxx\n";
        //
    }
}

template <typename T>
HashTable<T>::~HashTable<T>() {
    table.clear();
}

template<typename T>
void HashTable<T>::get_size() {
    std::cout << "size: " << table_size << " " << table.size() << "\n";
}