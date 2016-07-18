#include <stdint.h>
#include <sys/types.h>
#include <asm/byteorder.h>

const uint16_t bswap16(uint16_t x)
{
    x= (x>>8) | (x<<8);
    return x;
}

void swab(const void *from, void *to, ssize_t n) {
  const int16_t *in = (int16_t*)from;
  int16_t *out = (int16_t*)to;
  int i;
  n /= 2;
  for (i = 0 ; i < n; i++) {
    out[i] = bswap16(in[i]);
  }
}
