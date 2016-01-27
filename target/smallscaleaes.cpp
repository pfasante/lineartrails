/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
#include "smallscaleaes.h"

//#define HEXOUTPUT
//#define LATEX
//#define TRUNCATED

SmallScaleAESState::SmallScaleAESState()
    : StateMask() {
}

SmallScaleAESState* SmallScaleAESState::clone() {
  SmallScaleAESState* obj = new SmallScaleAESState();
  // TODO implement this :)
  return obj;
}

void SmallScaleAESState::print(std::ostream& stream) {
  stream << *this;
}


#ifdef HEXOUTPUT
#ifdef LATEX
std::ostream& operator<<(std::ostream& stream, const SmallScaleAESState& statemask) {
  return stream;
}
#else
#ifdef TRUNCATED
std::ostream& operator<<(std::ostream& stream, const SmallScaleAESState& statemask) {
  return stream;
}
#else
std::ostream& operator<<(std::ostream& stream, const SmallScaleAESState& statemask) {
  return stream;
}
#endif // TRUNCATED
#endif // LATEX
#else
std::ostream& operator<<(std::ostream& stream, const SmallScaleAESState& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] { "\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0",
      "\033[1;33m?\033[0m" };
#endif // TERMINALCOLORS
  return stream;
}
#endif // HEXOUTPUT

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

BitVector SmallScaleAESSbox(BitVector in) {
  // with x0 as MSB
  static const BitVector sbox[32] = {
    0
  };
  return sbox[in % 32] & 0x1f;
}
