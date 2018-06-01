#include <stdlib.h>
#include <scan.h>
#include <fmt.h>
#include <buffer.h>
#include <str.h>
#include "sha1.h"
#include "chord.h"

char a[] = "0123456789abcdef";

void say2_error(const char * routine)
{
    buffer_puts(buffer_2, routine);
    buffer_puts(buffer_2, ": ");
    buffer_puterror(buffer_2);
    buffer_putnlflush(buffer_2);
}

void
panic(const char *routine)
{
    say2_error(routine);
    exit(111);
}

int
strout (char *s)
{
	return buffer_putsflush(buffer_1, s);
}

size_t
h2s(char *d, const void * const s)
{
    const unsigned char *t;
    int i;
    t = s;
    for (i=0; i < HASHLEN; ++i)
    {
      d[2*i]   = a[t[i] >> 4];
      d[2*i+1] = a[t[i] & 0x0f];
    }
    d[2*HASHLEN] = 0;
    return i;
}

size_t
s2h (unsigned char *d, const void * const s)
{
   int i, j;
   const unsigned char *t;
   t = s;
   for (i=0, j=0; i < HASHLEN; ++i, j += 2)
      d[i] = (scan_fromhex(t[j])<<4) | (scan_fromhex(t[j+1]));
   return j;
}

void hashout2(unsigned char *c)
{
    char p;
    int i;
    for (i=0;i<HASHLEN;i++) {
        p = a[c[i] >> 4];
        if (p < 10) buffer_put(buffer_1,"0",1);
        buffer_put(buffer_1,&p,1);

        p = a[c[i] & 15];
        if (p < 10) buffer_put(buffer_1,"0",1);
        buffer_put(buffer_1,&p,1);
    }
}

void hashout(unsigned char *c)
{
    char p;
    int i;
    for (i=0;i<HASHLEN;i++) {
        p = a[c[i] >> 4];
        if (p < 10) buffer_put(buffer_1,"0",1);
        buffer_put(buffer_1,&p,1);

        p = a[c[i] & 15];
        if (p < 10) buffer_put(buffer_1,"0",1);
        buffer_put(buffer_1,&p,1);
    }
}

void
digest (unsigned char *hash, unsigned char *data, size_t len)
{
  SHA1_CTX ctx;
  SHA1Init (&ctx);
  SHA1Update (&ctx, data, len);
  SHA1Final (hash, &ctx);
}

int hdiff(const void* a, const void* b, size_t len) {
  size_t i;
  const unsigned char *tmp1,*tmp2;
  unsigned char x[1];
  unsigned char y[1];
  tmp1=a;
  tmp2=b;
  for (i=0; i<len; ++i) {
      x[1]=scan_fromhex(tmp1[i]);
      y[1]=scan_fromhex(tmp2[i]);
      int r=x[1]-y[1];
      if (r) return r;
  }
  return 0;
}
/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
