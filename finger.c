#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <buffer.h>
#include "chord.h"
#include <assert.h>

int
range(const unsigned char *a, const char unsigned *b, const char unsigned *c)
{
  /*
  if (low < high) {
    if (low < id  && id <= high) return 1;
  } else {
    if (high <= id && id <= low) {
       // if in [high, low] then not in [low, high]
       return 0;
    } else return 1;
  }
  */

  unsigned int m=FTBLSIZ - 1;
  int i = 0;
  char p[HASHLEN*2]; /* a => low  => modx */
  char q[HASHLEN*2]; /* b => high => mody */
  char r[HASHLEN*2]; /* c => id   => modz */

  mpz_t x, y, z, mod, modx, mody, modz;
  h2s(p,a);
  h2s(q,b);
  h2s(r,c);
  assert(mpz_init_set_str(x,p,16) == 0);
  assert(mpz_init_set_str(y,q,16) == 0);
  assert(mpz_init_set_str(z,r,16) == 0);
  mpz_init(mod);
  mpz_init(modx);
  mpz_init(mody);
  mpz_init(modz);
  mpz_ui_pow_ui(mod, 2, m);
  mpz_mod(modx,x,mod);
  mpz_mod(mody,y,mod);
  mpz_mod(modz,z,mod);

  if ( mpz_cmp(modx,mody) < 0) {
    if (mpz_cmp(modx,modz) < 0 && mpz_cmp(modz,mody) <= 0)
      i = 1;
  } else {
    if ( mpz_cmp(mody,modz) <= 0 && mpz_cmp(modz,modx) <= 0) {
      i = 0;
    } else {
      i = 1;
    }
  }
  mpz_clear(x);
  mpz_clear(y);
  mpz_clear(z);
  mpz_clear(modx);
  mpz_clear(mody);
  mpz_clear(modz);
  mpz_clear(mod);
  return i;
}


int
half_range(const unsigned char *a, const char unsigned *b, const char unsigned *c)
{
  /*
  if (x < y) {
    if (x < z  && z < y) return 1;
  } else {
    if (y < z && z < x) {
       // if in [y, x] then not in [x, y]
       return 0;
    } else return 1;
  }
  */


  unsigned int m=FTBLSIZ - 1;
  int i = 0;
  char p[HASHLEN*2]; /* a => x => modx */
  char q[HASHLEN*2]; /* b => y => mody */
  char r[HASHLEN*2]; /* c => z => modz */

  mpz_t x, y, z, mod, modx, mody, modz;
  h2s(p,a);
  h2s(q,b);
  h2s(r,c);
  assert(mpz_init_set_str(x,p,16) == 0);
  assert(mpz_init_set_str(y,q,16) == 0);
  assert(mpz_init_set_str(z,r,16) == 0);
  mpz_init(mod);
  mpz_init(modx);
  mpz_init(mody);
  mpz_init(modz);
  mpz_ui_pow_ui(mod, 2, m);
  mpz_mod(modx,x,mod);
  mpz_mod(mody,y,mod);
  mpz_mod(modz,z,mod);
  if ( mpz_cmp(modx,mody) < 0) {
    if (mpz_cmp(modx,modz) < 0 && mpz_cmp(modz,mody) < 0)
      i = 1;
  } else {
    if ( mpz_cmp(mody,modz) < 0 && mpz_cmp(modz,modx) < 0) {
      i = 0;
    } else {
      i = 1;
    }
  }
  mpz_clear(x);
  mpz_clear(y);
  mpz_clear(z);
  mpz_clear(modx);
  mpz_clear(mody);
  mpz_clear(modz);
  mpz_clear(mod);
  return i;
}

int
hash_compare(const unsigned char *a, const char unsigned *b)
{
  int i;
  char p[HASHLEN*2];
  char q[HASHLEN*2];

  mpz_t x, y;
  h2s(p,a);
  h2s(q,b);
  assert(mpz_init_set_str(x,p,16) == 0);
  assert(mpz_init_set_str(y,q,16) == 0);
  i = mpz_cmp(x,y);
  mpz_clear(x);
  mpz_clear(y);
  return i;
}

char *
next_finger_id(const char *id, unsigned int key)
{
  unsigned int m=FTBLSIZ - 1;
  mpz_t n, two, mod, kminus1, twokone, nmod, sum, final;
  unsigned long int k;
  k = key - 1;

  assert(mpz_init_set_str(n,id,16) == 0);
  assert(mpz_init_set_str(two,"2",16) == 0);
  mpz_init(mod);
  mpz_init(twokone);
  mpz_init(nmod);
  mpz_init(sum);
  mpz_init(final);
  mpz_init(kminus1);
  mpz_set_ui(kminus1,k);

  /* (n + 2^(k-1)) % 2^m =>  ( (n % 2^m) + ( 2^(k-1) % 2^m)) % 2^m   */

  /* 2^m */
  mpz_ui_pow_ui(mod, 2, m);

  /* (2^k-1) % 2^m  */
  mpz_powm(twokone,two, kminus1, mod);

  /* n % 2^m  */
  mpz_mod(nmod,n,mod);

  /* (n + 2^(k-1)) % 2^m  */
  mpz_add(sum,nmod,twokone);

  mpz_mod(final,sum,mod);
  /* convert result to sha1 string */
  char *r;
  r = (char *)malloc(mpz_sizeinbase (final, 16) + 2);
  if (!r) return NULL;
  mpz_get_str(r, 16, final);

  /* cleanup */
  mpz_clear(n);
  mpz_clear(two);
  mpz_clear(mod);
  mpz_clear(kminus1);
  mpz_clear(twokone);
  mpz_clear(nmod);
  mpz_clear(sum);
  mpz_clear(final);
  return r;
}
/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
