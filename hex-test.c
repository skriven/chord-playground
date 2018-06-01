#include <stdio.h>
#include <assert.h>
#include <errmsg.h>
#include <errno.h>
#include <buffer.h>
#include <byte.h>
#include <stralloc.h>
#include <scan.h>
#include <fmt.h>
#include <str.h>
#include "chord.h"
#include "sha1.h"

int
main (int argc, char **argv)
{

  if (argc < 2)
    die (1, "usage: ./hex-test 10.99.99.10:7975");

  unsigned char h1[HASHLEN];
  unsigned char h2[HASHLEN];
  unsigned char h3[HASHLEN];
  char hs2[HASHLEN*2];
  char hs3[] = "149592d7d610a1b0699b6c06feedb091e6325d65";

  SHA1_CTX ctx;
  SHA1Init (&ctx);
  SHA1Update (&ctx,(unsigned char *) argv[1], str_len(argv[1]));
  SHA1Final (h1, &ctx);

  hashout(h1); buffer_putnlflush(buffer_1);
 
  /* hex byte to string */
  strout("convert h1 byte to hs2 string -> h2s(hs2,h1)\n");
  h2s(hs2,h1);

  strout("hs2: "); strout(hs2); buffer_putnlflush(buffer_1);
  strout("hs3: "); strout(hs3); buffer_putnlflush(buffer_1);

  /* string to hex byte */
  printf("convert hs2 string to h2 byte -> s2h(h2,hs2)\n");
  s2h(h2,hs2);

  /* string to hex byte */
  printf("convert hs3 string to h3 byte -> s2h(h3,hs3)\n");
  s2h(h3,hs3);

  printf("byte_equal h1 h2: %d\n",byte_equal(h1,HASHLEN,h2));
  printf("byte_equal h1 h3: %d\n",byte_equal(h1,HASHLEN,h3));
  printf("byte_equal h2 h3: %d\n",byte_equal(h2,HASHLEN,h3));
  printf("byte_equal hs2 hs3: %d\n",byte_equal(hs2,HASHLEN,hs3));

  assert(byte_equal(h1,HASHLEN,h2));
  assert(byte_equal(h1,HASHLEN,h3));
  assert(byte_equal(h2,HASHLEN,h3));
  assert(byte_equal(hs2,HASHLEN,hs3));
  return 0;
}
/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
