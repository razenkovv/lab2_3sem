#pragma once
#include <iostream>

template <typename T1, typename T2>
class Pair;

//template<typename T1, typename T2>
//std::ostream &operator<< (std::ostream &, const Pair<T1, T2> &);

template <typename T1, typename T2>
class Pair {
private:
    T1 object1;
    T2 object2;
public:
    Pair() : object1(), object2() {}
    Pair(const T1& obj1, const T2& obj2);
    Pair(const Pair<T1, T2> &other_pair);

    T1& get_first() { return object1; }
    T2& get_second() { return object2; }

    void print();
//    friend std::ostream &operator<< <T1, T2>(std::ostream &out, const Pair<T1, T2> &p);
};

template<typename T1, typename T2>
Pair<T1, T2>::Pair(const T1 &obj1, const T2 &obj2) {
    new(&object1) T1(obj1);
    new(&object2) T2(obj2);
}

template<typename T1, typename T2>
void Pair<T1, T2>::print() {
    std::cout << object1 << " " << object2 << "\n";
}

//template<typename T1, typename T2>
//std::ostream &operator<<(std::ostream &out, const Pair<T1, T2> &p) {
//    out << p.object1 << " " << p.object2;
//    return out;
//}

template<typename T1, typename T2>
Pair<T1, T2>::Pair(const Pair<T1, T2> &other_pair) {
    new(&object1) T1(other_pair.get_first());
    new(&object2) T2(other_pair.get_second());
}