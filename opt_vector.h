//
// Created by sandwwraith
// ITMO University, 2015
//

#ifndef BIGINT_OPT_VECTOR_H
#define BIGINT_OPT_VECTOR_H

#include <vector>
#include <stdint.h>
#include <iosfwd>

struct mvector
{
    std::vector<uint32_t> data;
    size_t link_count;
};

struct opt_vector
{
private:
    union
    {
        mvector *big;
        uint32_t single_elem = 0;
    };
    size_t _size = 1;
    bool big_dick = false;
    void detach();
    void safe_delete();

public:
    opt_vector();
    ~opt_vector();

    opt_vector &operator=(opt_vector const &other);
    uint32_t& operator[](size_t index);
    uint32_t const& operator[](size_t index) const;

    void resize(size_t newsize);
    void push_back(uint32_t elem);
    void pop_back();
    size_t size() const;

    uint32_t &back();

    uint32_t const &back() const;

};
#endif //BIGINT_OPT_VECTOR_H
