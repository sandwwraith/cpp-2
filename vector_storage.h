//
// Created by sandwwraith
// ITMO University, 2015
//
#include "big_integer.h"

#ifndef BIGINT_VECTOR_STORAGE_H
#define BIGINT_VECTOR_STORAGE_H


struct vector_storage
{
    typedef big_integer value_type;
    typedef big_integer *iter;
    typedef big_integer const *iter_const;

    vector_storage();

    vector_storage(vector_storage const &other);

    vector_storage(size_t initial_capacity);

    //vector_storage &operator=(vector_storage const& other);
    vector_storage &operator=(vector_storage other);

    ~vector_storage();

    void clear();

    void push_back(value_type);

    void pop_back();

    value_type &back();

    value_type const &back() const;

    size_t size() const;

    value_type &operator[](size_t ind);

    value_type const &operator[](size_t ind) const;

    iter begin();

    iter_const begin() const;

    iter end();

    iter_const end() const;

    iter erase(iter place);

    iter erase(iter pl1, iter pl2);

    iter insert(iter place, value_type element);

private:
    value_type *array;
    size_t _size;
    size_t capacity;

    void safe_delete();

    void swap(vector_storage &other);

    void push_capacity_back(const value_type &elem);
};

#endif //BIGINT_VECTOR_STORAGE_H
