#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

big_integer::big_integer()
{
    sign = true;
    data.resize(1);
    data[0] = 0;
}

big_integer::big_integer(big_integer const &other)
{
    this->data = other.data;
    this->sign = other.sign;
}

big_integer::big_integer(int a)
{
    int64_t r = a;
    sign = true;
    if (r < 0)
    {
        sign = false;
        r = -r;
    }
    data.resize(1);
    data[0] = static_cast<uint32_t>(r);
}

big_integer::big_integer(uint32_t a)
{
    sign = true;
    data.resize(1);
    data[0] = a;
}

big_integer::big_integer(std::string const &str) : big_integer()
{
    size_t beginIndex = 0;
    if (str[0] == '-')
    {
        this->sign = false;
        beginIndex = 1;
    }

    for (size_t i = beginIndex; i < str.size(); i++)
    {
        int digit = str[i] - (char) '0';
        this->mul_long_short(10);
        this->add_long_short(static_cast<uint32_t>(digit));
    }
    remove_zeroes();
}

big_integer::~big_integer()
{ }

big_integer &big_integer::operator=(big_integer const &other)
{
    this->data = other.data;
    this->sign = other.sign;

    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs)
{
    if (this->sign == rhs.sign) return this->unsigned_add(rhs);
    if (this->compare_by_abs(rhs) >= 0)
    {
        return this->unsigned_sub(rhs);
    }
    else
    {
        this->sign = !this->sign;
        return this->unsigned_sub(rhs, true);
    }
}

big_integer &big_integer::operator-=(big_integer const &rhs)
{
    if (this->sign != rhs.sign) return this->unsigned_add(rhs);
    if (this->compare_by_abs(rhs) >= 0)
    {
        return this->unsigned_sub(rhs);
    }
    else
    {
        this->sign = !this->sign;
        return this->unsigned_sub(rhs, true);
    }
}

//Subtracts rhs from this (reverse = false) , this from rhs (reverse = true)
big_integer &big_integer::unsigned_sub(big_integer const &rhs, bool reverse)
{
    uint64_t carry = 0;
    size_t lim = reverse ? this->data.size() : rhs.data.size();
    for (size_t i = 0; i < lim || carry; i++)
    {
        int64_t a;
        if (!reverse)
            a = static_cast<int64_t>(this->data[i]) - carry -
                static_cast<int64_t>(i < rhs.data.size() ? rhs.data[i] : 0);
        else
            a = static_cast<int64_t>( rhs.data[i]) - carry -
                static_cast<int64_t>(i < this->data.size() ? this->data[i] : 0);
        if (a < 0)
        {
            carry = 1;
            a += BASE;
        }
        else
        {
            carry = 0;
        }
        this->data[i] = static_cast<uint32_t> (a);
    }

    remove_zeroes();
    return *this;
}

big_integer &big_integer::unsigned_add(big_integer const &other)
{
    uint64_t carry = 0;
    size_t const ma = std::max(this->data.size(), other.data.size());
    for (size_t i = 0; i < ma || carry; i++)
    {
        if (i == this->data.size()) this->data.push_back(0);
        uint64_t a = static_cast<uint64_t> (this->data[i]) +
                     static_cast<uint64_t> (i < other.data.size() ? other.data[i] : 0) + carry;
        this->data[i] = static_cast<uint32_t> (a % BASE);
        carry = a / BASE;
    }
    while (this->data.size() > 1 && this->data.back() == 0)
    {
        this->data.pop_back();
    }
    return *this;

}

big_integer &big_integer::operator*=(big_integer const &rhs)
{
    this->sign = this->sign == rhs.sign;
    if (rhs.data.size() == 1)
    {
        this->mul_long_short(rhs.data[0]);
        return *this;
    }

    uint64_t t, mulcarry = 0, addcarry = 0;

    for (ptrdiff_t i = this->data.size() - 1; i >= 0; i--)
    {
        mulcarry = 0;
        uint64_t mul = this->data[i];
        for (size_t j = 0; j < rhs.data.size(); j++)
        {
            t = mul * static_cast<int64_t>(rhs.data[j]) + mulcarry;
            mulcarry = t / BASE;
            t = t % BASE;

            if (j == 0) this->data[i] = 0;
            size_t k = i + j;
            if (k == this->data.size()) this->data.push_back(0);
            uint64_t tmp2 = (uint64_t) this->data[k] + t;
            addcarry = tmp2 / BASE;
            tmp2 = tmp2 % BASE;

            this->data[k] = static_cast<uint32_t> (tmp2);

            while (addcarry != 0)
            {
                k++;
                if (k == this->data.size()) this->data.push_back(0);
                tmp2 = (uint64_t) this->data[k] + addcarry;
                addcarry = tmp2 / BASE;
                this->data[k] = static_cast<uint32_t> (tmp2 % BASE);
            }
        }

        size_t k = i + rhs.data.size();
        while (mulcarry != 0)
        {
            if (k == this->data.size()) this->data.push_back(0);
            uint64_t tmp2 = (uint64_t) this->data[k] + mulcarry;
            mulcarry = tmp2 / BASE;
            this->data[k] = static_cast<uint32_t > (tmp2 % BASE);
            k++;
        }
    }
    remove_zeroes();

    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs)
{
    if (this->compare_by_abs(rhs) < 0)
    {
        return *this = big_integer(0);
    }
    bool sign = this->sign == rhs.sign;
    if (rhs.data.size() == 1)
    {
        this->div_long_short(rhs.data[0]);
        this->sign = sign;
        return *this;
    }

    this->sign = true;
    big_integer divn(rhs);
    divn.sign = true;

    uint32_t normalize = static_cast<uint32_t> (BASE / (divn.data.back() + 1));
    this->mul_long_short(normalize);
    divn.mul_long_short(normalize);

    uint32_t cached = divn.data.back();
    size_t n = divn.data.size();
    size_t m = this->data.size() - divn.data.size();

    big_integer res;
    res.data.resize(m + 1);

    big_integer shifted(divn);
    shifted <<= (m * 32);
    if (*this >= shifted)
    {
        res.data[m] = 1;
        *this -= shifted;
    }
    else
    {
        res.data[m] = 0;
    }

    for (int i = m - 1; i >= 0; i--)
    {
        uint64_t cur = BASE * static_cast<uint64_t>(this->data[n + i])
                       + static_cast<uint64_t>(this->data[n + i - 1]);
        cur /= static_cast<uint64_t>(cached);
        if (cur > BASE - 1)
        {
            cur = BASE - 1;
        }
        res.data[i] = static_cast<uint32_t>(cur);

        shifted >>= 32;
        big_integer k = shifted * (uint32_t) cur;
        *this -= (k);

        while (*this < 0)
        {
            res.data[i]--;
            *this += shifted;
        }

    }

    *this = res;
    this->sign = sign;
    remove_zeroes();

    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs)
{
    return *this -= ((*this / rhs) * rhs);
}

big_integer &big_integer::operator&=(big_integer const &rhs)
{
    big_integer right(rhs);
    this->convert();
    right.convert();

    for (size_t i = 0, s = std::max(this->data.size(), right.data.size()); i < s; i++)
    {
        uint32_t l, r;
        l = (i < this->data.size() ? this->data[i] : (this->sign ? 0 : std::numeric_limits<uint32_t>::max()));
        r = (i < right.data.size() ? right.data[i] : (right.sign ? 0 : std::numeric_limits<uint32_t>::max()));
        if (i == this->data.size()) this->data.push_back(0);
        this->data[i] = l & r;
    }

    this->sign = this->sign || right.sign;

    this->convert();
    remove_zeroes();

    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs)
{
    big_integer right(rhs);
    this->convert();
    right.convert();

    for (size_t i = 0, s = std::max(this->data.size(), right.data.size()); i < s; i++)
    {
        uint32_t l, r;
        l = (i < this->data.size() ? this->data[i] : (this->sign ? 0 : std::numeric_limits<uint32_t>::max()));
        r = (i < right.data.size() ? right.data[i] : (right.sign ? 0 : std::numeric_limits<uint32_t>::max()));
        if (i == this->data.size()) this->data.push_back(0);
        this->data[i] = l | r;
    }

    this->sign = this->sign && right.sign;

    this->convert();
    remove_zeroes();

    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs)
{
    big_integer right(rhs);
    this->convert();
    right.convert();

    for (size_t i = 0, s = std::max(this->data.size(), right.data.size()); i < s; i++)
    {
        uint32_t l, r;
        l = (i < this->data.size() ? this->data[i] : (this->sign ? 0 : std::numeric_limits<uint32_t>::max()));
        r = (i < right.data.size() ? right.data[i] : (right.sign ? 0 : std::numeric_limits<uint32_t>::max()));
        if (i == this->data.size()) this->data.push_back(0);
        this->data[i] = l ^ r;
    }

    this->sign = !(this->sign ^ right.sign);

    this->convert();
    remove_zeroes();

    return *this;
}

big_integer &big_integer::operator<<=(int rhs)
{
    if (rhs == 0 || is_zero()) return *this;
    if (rhs < 0) return this->operator>>=(-rhs);
    uint32_t blocks = static_cast<uint32_t>(rhs) / 32;
    uint32_t bytes = static_cast<uint32_t>(rhs) % 32;
    data.push_back(0);

    if (bytes != 0)
    {
        for (size_t i = data.size() - 1; i > 0; i--)
        {
            uint32_t next, cur;
            next = ~((1U << (32U - bytes)) - 1U); //Mask
            next &= data[i - 1];
            next >>= (32 - bytes);

            cur = data[i - 1];
            cur <<= bytes;

            data[i - 1] = cur;
            data[i] |= next;
        }
    }

    size_t size0 = data.size();
    data.resize(data.size() + blocks);

    for (int i = size0 + blocks - 1; i >= static_cast<int>(blocks); --i) //Workaround for blocks = 0
    {
        data[i] = data[i - blocks];
    }
    for (size_t i = 0; i < blocks; ++i)
    {
        data[i] = 0;
    }

    remove_zeroes();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs)
{
    if (rhs == 0 || is_zero()) return *this;
    if (rhs < 0) return this->operator<<=(-rhs);

    uint32_t blocks = static_cast<uint32_t>(rhs) / 32;
    uint32_t bytes = static_cast<uint32_t>(rhs) % 32;

    this->convert();

    if (bytes != 0)
    {
        for (size_t i = blocks; i < data.size(); i++)
        {
            uint32_t cur, prev;
            cur = data[i];
            cur >>= bytes;
            if (i == data.size() - 1 && !this->sign)
            {
                uint32_t mask = ~((1U << (32U - bytes)) - 1U); //Mask, where first BYTES bytes are 1
                cur |= mask;
            }
            data[i] = cur;
            if (i < data.size() - 1)
            {
                prev = (1U << bytes) - 1;
                prev &= data[i + 1];
                prev <<= (32 - bytes);
                data[i] |= prev;
            }
        }
    }

    for (size_t i = blocks; i < data.size(); i++)
    {
        data[i - blocks] = data[i];
    }

    uint32_t mask = (this->sign ? 0 : std::numeric_limits<uint32_t>::max());
    for (size_t i = 0; i < blocks; i++)
    {
        data[data.size() - 1 - i] = mask;
    }

    this->convert();
    remove_zeroes();

    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    big_integer r(*this);
    r.sign = !r.sign;
    return r;
}

big_integer big_integer::operator~() const
{
    big_integer r(*this);
    r += 1;
    r.sign = !r.sign;
    return r;
}

big_integer &big_integer::operator++() //Prefix
{
    this->operator+=(1);
    return *this;
}

big_integer big_integer::operator++(int) //Postfix
{
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--()
{
    this->operator-=(1);
    return *this;
}

big_integer big_integer::operator--(int)
{
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b)
{
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b)
{
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) < 0;
}

bool operator>(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) > 0;
}

bool operator<=(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b)
{
    return a.compare_to(b) >= 0;
}

std::string to_string(big_integer const &a)
{
    if (a.is_zero()) return "0";
    big_integer x(a);
    std::string res = "";
    while (!x.is_zero())
    {
        int digit = x.div_long_short(10);
        res.push_back(static_cast<char>(digit) + (char) '0');
    }
    while (res[res.size() - 1] == '0')
    {
        res.pop_back();
    }
    if (!x.sign) res.push_back('-');

    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a)
{
    return s << to_string(a);
}

//Returns 1 if THIS greater than OTHER
//        0 if equals
//        -1 if less
int big_integer::compare_to(big_integer const &other) const
{
    if (this->is_zero() && other.is_zero()) return 0; // Zero and minus zero
    if (this->sign && !other.sign) return 1;
    if (!this->sign && other.sign) return -1;
    if (this->sign && other.sign) //both non-negative
    {
        if (this->data.size() > other.data.size()) return 1;
        if (this->data.size() < other.data.size()) return -1;
        for (size_t i = this->data.size(); i > 0; i--)
        {
            if (this->data[i - 1] > other.data[i - 1]) return 1;
            if (this->data[i - 1] < other.data[i - 1]) return -1;
        }
    }
    else //both negative
    {
        if (this->data.size() > other.data.size()) return -1;
        if (this->data.size() < other.data.size()) return 1;
        for (size_t i = this->data.size(); i > 0; i--)
        {
            if (this->data[i - 1] > other.data[i - 1]) return -1;
            if (this->data[i - 1] < other.data[i - 1]) return 1;
        }
    }
    return 0;
}

int big_integer::compare_by_abs(big_integer const &other) const
{
    if (this->data.size() > other.data.size()) return 1;
    if (this->data.size() < other.data.size()) return -1;
    for (size_t i = this->data.size(); i > 0; i--)
    {
        if (this->data[i - 1] > other.data[i - 1]) return 1;
        if (this->data[i - 1] < other.data[i - 1]) return -1;
    }
    return 0;
}

bool big_integer::is_zero() const
{
    return (this->data.size() == 1 && this->data[0] == 0);
}

big_integer &big_integer::flip_bytes(bool flip_sign)
{
    if (flip_sign) this->sign = !this->sign;
    for (size_t i = 0; i < data.size(); i++)
    {
        data[i] = ~data[i];
    }
    return *this;
}

//Converts to addition code and back
big_integer &big_integer::convert()
{
    if (!this->sign)
    {
        this->unsigned_sub(1);
        this->flip_bytes();
    }
    return *this;
}

big_integer &big_integer::add_long_short(uint32_t x)
{
    uint64_t tmp, carry = 0;
    tmp = (uint64_t) this->data[0] + (uint64_t) x;
    carry = tmp / BASE;
    this->data[0] = static_cast<uint32_t > (tmp % BASE);
    size_t k = 1;
    while (carry != 0)
    {
        if (k == this->data.size()) data.push_back(0);
        tmp = (uint64_t) data[k] + carry;
        carry = tmp / BASE;
        data[k] = static_cast<uint32_t > (tmp % BASE);
        k++;
    }
    return *this;
}

big_integer &big_integer::mul_long_short(uint32_t x)
{
    //Seems to be working now
    uint64_t tmp, carry = 0;
    for (size_t i = 0; i < data.size(); i++)
    {
        tmp = (uint64_t) this->data[i] * (uint64_t) x + carry;
        carry = tmp / BASE;
        this->data[i] = static_cast<uint32_t > (tmp % BASE);

    }
    size_t k = data.size();
    while (carry != 0)
    {
        if (k == this->data.size()) data.push_back(0);
        tmp = (uint64_t) data[k] + carry;
        carry = tmp / BASE;
        data[k] = static_cast<uint32_t > (tmp % BASE);
        k++;
    }
    return *this;
}

//Returns remainder
uint32_t big_integer::div_long_short(uint32_t x)
{
    uint64_t carry = 0;
    for (size_t i = this->data.size(); i > 0; --i)
    {
        uint64_t temp = static_cast<uint64_t >(this->data[i - 1]) + carry * BASE;
        this->data[i - 1] = (size_t) (temp / (uint64_t) (x));
        carry = temp % (uint64_t) (x);
    }

    remove_zeroes();
    return static_cast<uint32_t> (carry);
}
