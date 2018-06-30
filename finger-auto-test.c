#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errmsg.h>
#include <errno.h>
#include <buffer.h>
#include <byte.h>
#include <str.h>
#include <scan.h>
#include <fmt.h>
#include "sha1.h"
#include "chord.h"

#define RING_SIZE 40

void
print_ring(node **ring)
{
  node **n;
  n = ring;
  system("clear");
  while (*n != NULL) {
    print_node(*n);
    n++;
  }
}

void dump_fingers(node *n)
{
  finger *f;
  int i;
  for (i = 2; i < FTBLSIZ; ++i) {
    f = &n->finger[i];
    buffer_putm(buffer_1,"  finger ");
    buffer_putulong(buffer_1,(unsigned long)i);
    buffer_puts(buffer_1," => ");
	  hashout(f->id);
    buffer_puts(buffer_1," f->node => ");
    if (f->node != NULL) hashout(f->node->id);
    buffer_putnlflush(buffer_1);
  }
}

void
fix_and_stabilize(node **ring)
{
  int i;
  node **n;
  for (i = RING_SIZE;i >= 0; --i) {
    n = ring;
    print_ring(ring);
    while (*n != NULL) {
         chord_stabilize(*n);
         chord_fix_fingers(*n);
         n++;
    }
    usleep(850);
  }
}

int
main (void)
{
  char s[12];
  node *n, *p; 
  unsigned int i;
  node *ring[255] =  { NULL };
  stralloc ss = { 0 };

  n=node_init("10.10.10.10",7975);
  bootstrap(n);

  ring[0] = n;
  for (i = 1; i < RING_SIZE; i++) {
    stralloc_zero(&ss);
    fmt_uint(s,i+10);
    stralloc_cats(&ss,"10.10.10.");
    stralloc_cats(&ss,s);
    stralloc_0(&ss);
    p = node_init(ss.s,7975);
    chord_join(n,p);
    ring[i-1] = p;
  }

  fix_and_stabilize(ring);

  for (i = 0; i < RING_SIZE; i++) {
    free(ring[i]);
  }
}
/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
