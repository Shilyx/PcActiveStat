#ifndef _SETOPERATORS_H
#define _SETOPERATORS_H

#include <algorithm>
#include <set>
#include <map>
#include <ostream>

template <class T>
std::set<T> operator-(const std::set<T> &s1, const std::set<T> &s2)
{
    std::set<T> result;

    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result, result.begin()));

    return result;
}

template <class T>
std::set<T> operator+(const std::set<T> &s1, const std::set<T> &s2)
{
    std::set<T> result;

    std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result, result.begin()));

    return result;
}

template <class T>
std::set<T> operator&(const std::set<T> &s1, const std::set<T> &s2)
{
    std::set<T> result;

    std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result, result.begin()));

    return result;
}

template <class T>
std::set<T> operator|(const std::set<T> &s1, const std::set<T> &s2)
{
    return s1 + s2;
}

template <class T>
std::set<T> operator^(const std::set<T> &s1, const std::set<T> &s2)
{
    std::set<T> result;

    std::set_symmetric_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result, result.begin()));

    return result;
}

template <class T>
std::set<T> &operator-=(std::set<T> &s1, const std::set<T> &s2)
{
    (s1 - s2).swap(s1);
    return s1;
}

template <class T>
std::set<T> &operator+=(std::set<T> &s1, const std::set<T> &s2)
{
    (s1 + s2).swap(s1);
    return s1;
}

template <class T>
std::set<T> &operator&=(std::set<T> &s1, const std::set<T> &s2)
{
    (s1 & s2).swap(s1);
    return s1;
}

template <class T>
std::set<T> &operator|=(std::set<T> &s1, const std::set<T> &s2)
{
    (s1 | s2).swap(s1);
    return s1;
}

template <class T>
std::set<T> &operator^=(std::set<T> &s1, const std::set<T> &s2)
{
    (s1 ^ s2).swap(s1);
    return s1;
}

template <class T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &s)
{
    bool bFirst = true;

    for (std::set<T>::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        if (bFirst)
        {
            bFirst = false;
        }
        else
        {
            os<<", ";
        }

        os<<*it;
    }

    return os;
}

template <class T, class T2>
std::set<T> GetMapKeySet(const std::map<T, T2> &m)
{
    std::set<T> result;

    for (std::map<T, T2>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        result.insert(it->first);
    }

    return result;
}

template <class T, class T2>
std::multiset<T> GetMapKeySet(const std::multimap<T, T2> &m)
{
    std::multiset<T> result;

    for (std::multimap<T, T2>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        result.insert(it->first);
    }

    return result;
}

#endif /* _SETOPERATORS_H */
