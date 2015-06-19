//
// Created by sandwwraith
// ITMO University, 2015
//

#include "opt_vector.h"

opt_vector::opt_vector()
{
    //I think it's nothing to do here.
}

opt_vector::~opt_vector()
{
    if (big_dick)
    {
        safe_delete();
    }
}

void opt_vector::safe_delete()
{
    if (big->link_count > 1) big->link_count--;
    else delete big;
}

void opt_vector::detach()
{
    if (big->link_count > 1)
    {
        mvector *newV = new mvector;
        newV->data = big->data;
        newV->link_count = 1;
        big->link_count--;
        std::swap(newV, big);
    }
}

opt_vector &opt_vector::operator=(opt_vector const &other)
{
    if (big_dick) safe_delete();
    if (!other.big_dick)
    {
        single_elem = other.single_elem;
    }
    else
    {
        other.big->link_count++;
        this->big = other.big;
    }
    this->_size = other._size;
    this->big_dick = other.big_dick;
    return *this;
}

uint32_t &opt_vector::operator[](size_t index)
{
    if (big_dick)
    {
        detach();
        return big->data[index];
    }
    return single_elem;
}

uint32_t const &opt_vector::operator[](size_t index) const
{
    if (big_dick) return big->data[index];
    else return single_elem;
}

void opt_vector::resize(size_t newsize)
{
    if (big_dick)
    {
        detach();
        big->data.resize(newsize);
        _size = newsize;
        return;
    }
    if (!big_dick && newsize >= 2)
    {
        uint32_t elem = single_elem;
        big = new mvector;
        big->link_count = 1;
        big->data.resize(newsize);
        big->data[0] = elem;
        _size = newsize;
        big_dick = true;
        return;
    }
}

void opt_vector::push_back(uint32_t elem)
{
    if (big_dick)
    {
        detach();
        big->data.push_back(elem);
        _size++;
        return;
    }
    uint32_t tmp = single_elem;
    big = new mvector;
    big->link_count = 1;
    big->data.push_back(tmp);
    big->data.push_back(elem);
    big_dick = true;
    _size++;
    return;
}

void opt_vector::pop_back()
{
    //Should think... But seems to be right.
    size_t newsize = _size-1;
    if (big_dick)
    {
        detach();
        big->data.pop_back();
        if (newsize == 1)
        {
            uint32_t tmp = big->data[0];
            safe_delete();
            big_dick = false;
            single_elem = tmp;
        }
        _size = newsize;
        return;
    }
}

size_t opt_vector::size() const
{
    return _size;
}

uint32_t &opt_vector::back()
{
    if (big_dick)
    {
        detach();
        return big->data.back();
    }
    return single_elem;
}

uint32_t const &opt_vector::back() const
{
    if (big_dick) return big->data.back();
    else return single_elem;
}

