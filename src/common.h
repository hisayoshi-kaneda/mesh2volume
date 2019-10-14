#ifdef _MSC_VER
#pragma once
#endif

#ifndef COMMON_H
#define COMMON_H

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <bits/stdc++.h>
using namespace std;

//array
template <typename T>
ostream &operator<<(ostream &os, pair<T *, int> p) {
    os << "{";
    for (int i = 0; i < p.second; i++) {
        os << p.first[i] << (i + 1 == p.second ? "" : ", ");
    }
    os << "}";
    return os;
}

//vector
template <typename T>
ostream &operator<<(ostream &os, vector<T> &vec) {
    os << "{";
    for (int i = 0; i < vec.size(); i++) {
        os << vec[i] << (i + 1 == vec.size() ? "" : ", ");
    }
    os << "}";
    return os;
}

// pair�o��
template <typename T, typename U>
ostream &operator<<(ostream &os, pair<T, U> &pair_var) {
    os << "(" << pair_var.first << ", " << pair_var.second << ")";
    return os;
}
// map�o��
template <typename T, typename U>
ostream &operator<<(ostream &os, map<T, U> &map_var) {
    os << "{";
    for (auto itr = map_var.begin(); itr != map_var.end(); itr++) {
        os << "(" << itr->first << ", " << itr->second << ")";
        itr++;
        if (itr != map_var.end())
            os << ", ";
        itr--;
    }
    os << "}";
    return os;
}
// set �o��
template <typename T>
ostream &operator<<(ostream &os, set<T> &set_var) {
    os << "{";
    for (auto itr = set_var.begin(); itr != set_var.end(); itr++) {
        os << *itr;
        ++itr;
        if (itr != set_var.end())
            os << ", ";
        itr--;
    }
    os << "}";
    return os;
}

#define DUMPOUT cerr

inline void dump_func() {
    DUMPOUT << endl;
}
template <class Head, class... Tail>
inline void dump_func(Head &&head, Tail &&... tail) {
    DUMPOUT << head;
    if (sizeof...(Tail) > 0) {
        DUMPOUT << ", ";
    }
    dump_func(std::move(tail)...);
}

// -----------------------------------------------------------------------------
// Assertion with message
// -----------------------------------------------------------------------------

#ifndef __FUNCTION_NAME__
#if defined(_WIN32) || defined(__WIN32__)
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __func__
#endif
#endif

#ifndef NDEBUG
#define Assertion(PREDICATE, ...)                                         \
    do {                                                                  \
        if (!(PREDICATE)) {                                               \
            std::cerr << "Asssertion \""                                  \
                      << #PREDICATE << "\" failed in " << __FILE__        \
                      << " line " << __LINE__                             \
                      << " in function \"" << (__FUNCTION_NAME__) << "\"" \
                      << " : ";                                           \
            fprintf(stderr, __VA_ARGS__);                                 \
            std::cerr << std::endl;                                       \
            std::abort();                                                 \
        }                                                                 \
    } while (false)

#define dump(...)                                                       \
    DUMPOUT << "  " << string(#__VA_ARGS__) << ": "                     \
            << "[" << to_string(__LINE__) << ":" << __FUNCTION__ << "]" \
            << endl                                                     \
            << "    ",                                                  \
        dump_func(__VA_ARGS__)
#else // NDEBUG
#define Assertion(PREDICATE, ...) \
    do {                          \
    } while (false)
#define dump(...)
#endif // NDEBUG

#endif //COMMON_H