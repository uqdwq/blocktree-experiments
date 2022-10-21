#include <iostream>

#ifndef MERSENNE_RABINKARP_H
#define MERSENNE_RABINKARP_H

template <class T>
class MersenneRabinKarp
{
public:
    uint64_t hash_;
    uint64_t prime_;
    uint64_t sigma_;
    uint64_t max_sigma_;
    uint64_t init_;
    uint64_t length_;
    uint64_t power_;
    const T *text_;
    MersenneRabinKarp(const T *text, uint64_t prime, uint64_t sigma, uint64_t init, uint64_t length, uint64_t power) : text_(text), prime_(prime), sigma_(sigma), init_(init), length_(length), power_(power)
    {
        uint64_t sigma_factor = 1;
        hash_ = 0;
        for (int i = init_ ; i <  init_ + length_; i++)
        {   
            // hash_ = (((hash_ * sigma_ ) % prime_) + (text_[i] % prime_)) % prime_;
            hash_ = mersenneModulo(hash_ * sigma_);
            hash_ = mersenneModulo(hash_ + mersenneModulo(text_[i]));
            sigma_factor = mersenneModulo(sigma_factor * sigma_);
//            std::cout << hash_ << std::endl;
        }
        max_sigma_ = sigma_factor/sigma_;
    };
    uint64_t mersenneModulo(uint64_t k) {
//        if (k > prime_) std::cout << "Prime small" << std::endl;
        return k % prime_;
        // uint64_t i = (k & prime_) + (k >> power_);
        // return (i >= prime_) ? i - prime_ : i;
    };
    void next() {
        T outchar = text_[init_];
        T inchar = text_[init_ + length_];
//        hash_ = (hash_ + kp_ - rm_*(s_[init_]+128) % kp_) % kp_;
//        init_++;
//       hash_ = (hash_*sigma_ + s_[init_+size_-1]+128) % kp_
//        hash_ = (hash_ + prime_ - max_sigma_)
        hash_ = mersenneModulo(prime_ + hash_ - mersenneModulo(outchar * max_sigma_));
        hash_ = mersenneModulo(hash_ * sigma_);
        hash_ = mersenneModulo(hash_ + inchar);
        init_++;
    };
    uint64_t hash() {
        return hash_;
    };
};

#endif // MERSENNE_RABINKARP_H