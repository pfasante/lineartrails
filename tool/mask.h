#ifndef MASK_H_
#define MASK_H_

#include <vector>
#include <array>
#include <iostream>
#include <assert.h>

#include "updatequeue.h"

#define BM_DUNNO  3
#define BM_0      2
#define BM_1      1
#define BM_CONTRA 0

typedef char BitMask;                  // mask for 1 bit (10=0, 01=1, 11=?)
typedef std::vector<BitMask> WordMask;
typedef uint64_t BitVector;            // n-bit vector

struct WordMaskCare {
  WordMaskCare& operator=(const WordMaskCare& rhs);
  WordMaskCare(unsigned bitsize);
  WordMaskCare(const WordMaskCare& other);
  WordMaskCare(BitVector canbe1, BitVector care);
  void Reset();

  BitVector canbe1;                    // 1=canbe1, 0=mustbe1 // LSB = WordMask[0]!!!
  BitVector care;                      // 1=0/1, 0=?)
};

struct ProbabilityPair{
  char sign; //could have stored in one, but meh.
  double bias; //ld of bias
};

struct Mask {
  Mask& operator=(const Mask& rhs);
  Mask(unsigned bitsize);
  Mask(const Mask& other);
  Mask(std::initializer_list<char> other);
  Mask(WordMask& other);
  Mask(WordMaskCare& other);
  void init_caremask();
  void reinit_caremask();
  void init_bitmasks();
  void set_bit(BitMask bit, const int index);

  friend std::ostream& operator<<(std::ostream& stream, const Mask& mask);

  WordMask bitmasks; // maybe don't store this & use just for initialization
  WordMaskCare caremask;
};

struct StateMask {
  virtual StateMask* clone() = 0;
  virtual void print() = 0;
  virtual void SetState(BitMask value) = 0;
  virtual void SetBit(BitMask value, int word_pos, int bit_pos) = 0;
  virtual std::vector<UpdatePos> diff(const StateMask& other) = 0;
  virtual Mask& operator[](const int index) = 0;
  virtual const Mask& operator[](const int index) const = 0;
};

#endif // MASK_H_
