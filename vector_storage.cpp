//
// Created by sandwwraith
// ITMO University, 2015
//

#include <cassert>
#include "vector_storage.h"

typedef big_integer value_type;
typedef big_integer *iter;
typedef big_integer const *iter_const;

vector_storage::vector_storage()
{
    array = static_cast<value_type *> (operator new(10 * sizeof(value_type)));
    capacity = 10;
    _size = 0;
}

vector_storage::vector_storage(size_t initial_capacity)
{
    array = (value_type *) operator new(initial_capacity * sizeof(value_type));
    capacity = initial_capacity;
    _size = 0;
}

vector_storage::vector_storage(vector_storage const &other)
{
    array = (value_type *) operator new(other.capacity * sizeof(value_type));
    size_t created = 0;
    try
    {
        for (created = 0; created < other._size; created++)
        {
            new(array + created) value_type(other.array[created]);
        }
    } catch (std::exception &)
    {

        for (size_t i = 0; i < created; i++)
        {
            reinterpret_cast<iter>(array + i)->~value_type();
        }
        operator delete(array);
        throw;
    }
    this->_size = other._size;
    this->capacity = other.capacity;
}

vector_storage &vector_storage::operator=(vector_storage other)
{
    swap(other);
    return *this;
}

void vector_storage::swap(vector_storage &other)
{
    std::swap(this->array, other.array);
    std::swap(this->capacity, other.capacity);
    std::swap(this->_size, other._size);
}

vector_storage::~vector_storage()
{
    safe_delete();
}

void vector_storage::clear()
{
    vector_storage tmp;
    swap(tmp);
}

void vector_storage::push_back(value_type elem)
{
    if (_size == capacity)
    {
        vector_storage tmp(capacity * 2);

        for (size_t i = 0; i != _size; ++i)
        {
            tmp.push_capacity_back(array[i]);
        }

        tmp.push_capacity_back(elem);

        swap(tmp);
    }
    else
        push_capacity_back(elem);
}

void vector_storage::push_capacity_back(const value_type &elem)
{
    assert(_size < capacity);
    new(array + _size) value_type(elem);
    ++_size;
}


void vector_storage::pop_back()
{
    if (_size != 0)
    {
        reinterpret_cast<iter>(array + _size - 1)->~value_type();
        _size--;
    }
}

vector_storage::value_type &vector_storage::back()
{
    return array[_size - 1];
}

vector_storage::value_type const &vector_storage::back() const
{
    return array[_size - 1];
}

size_t vector_storage::size() const
{
    return _size;
}

vector_storage::value_type &vector_storage::operator[](size_t ind)
{
    return array[ind];
}

vector_storage::value_type const &vector_storage::operator[](size_t ind) const
{
    return array[ind];
}

vector_storage::iter vector_storage::begin()
{
    return array;
}

vector_storage::iter_const vector_storage::begin() const
{
    return array;
}

vector_storage::iter vector_storage::end()
{
    return array + _size;
}

vector_storage::iter_const vector_storage::end() const
{
    return array + _size;
}

vector_storage::iter vector_storage::erase(iter place)
{
    for (iter i = place; i < end() - 1; i++)
    {
        std::swap(*i, *(i + 1));
    }
    this->pop_back();
    return place;
}

vector_storage::iter vector_storage::erase(iter pl1, iter pl2)
{
    size_t erase_size = pl2 - pl1;
    for (iter i = pl1; i < end() - erase_size; i++)
    {
        std::swap(*i, *(i + erase_size));
    }
    for (size_t i = 0; i < erase_size; i++)
    {
        this->pop_back();
    }
    return pl1;
}

vector_storage::iter vector_storage::insert(iter place, value_type element)
{
    this->push_back(element);
    for (iter i = end() - 1; i > place; i--)
    {
        std::swap(*i, *(i - 1));
    }
    return place;
}

void vector_storage::safe_delete()
{
    for (size_t i = 0; i < _size; i++)
    {
        reinterpret_cast<iter>(array + i)->~value_type();
    }
    operator delete(array);
}


