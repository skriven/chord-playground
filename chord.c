#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stralloc.h>
#include <buffer.h>
#include <byte.h>
#include <scan.h>
#include <fmt.h>
#include <stdlib.h>
#include <str.h>
#include <gmp.h>
#include <nacl/crypto_hash.h>
#include "sha1.h"
#include "chord.h"

void
fingers(node *n)
{
  finger *f;
  int i;
  for (i = 1; i < FTBLSIZ; ++i) {
    f = &n->finger[i];
    buffer_putm(buffer_1,"  finger ");
    buffer_putulong(buffer_1,(unsigned long)i);
    buffer_puts(buffer_1," => ");
	  hashout(f->id);
    buffer_puts(buffer_1," f->node => ");
    if (f->node != NULL) buffer_puts(buffer_1,f->node->ip);
    buffer_putnlflush(buffer_1);
  }
}

void
print_node(node *n)
{
  if (n->predecessor->id) {
    buffer_putm(buffer_1,n->predecessor->ip, " -> ");
  } else {
    buffer_putm(buffer_1,"nil -> ");
  }
  buffer_putm(buffer_1,n->ip, " -> ");
  if (n->successor->id) {
    buffer_putm(buffer_1,n->successor->ip);
  } else {
    buffer_puts(buffer_1,"nil");
  }
  buffer_putnlflush(buffer_1);
}

void
bootstrap(node *n)
{
  unsigned int i;
  char id[HASHLEN*2];
  char *t;
  finger *f;

  n->predecessor = n;
  n->successor = n;

  /* init finger table to self -- single node */
  for (i = 1; i < FTBLSIZ; ++i) {
    h2s(id,n->id);
    t = next_finger_id(id,i);
    f = malloc(sizeof(finger));
    f->node = n;
    s2h(f->id,t);
    n->finger[i] = *f;
    free(f);
    free(t);
  }
}

void node_free(node *n)
{
  if (n) {
    if (n->ip) free(n->ip);
    free(n);
  }
}

node *
node_init(char *ip, int port)
{
  node *n;

  stralloc sa_data = { 0 };
  stralloc_cats (&sa_data, ip);
  stralloc_cats (&sa_data, ":");
  stralloc_catulong0 (&sa_data, port, 1);
  stralloc_0(&sa_data);

  n = calloc(1, sizeof(node));
  if(n == NULL) return NULL;

  digest(n->id, (unsigned char *)sa_data.s, sa_data.len);

  n->predecessor = NULL;
  n->successor = NULL;
  n->ip = (char *) malloc(str_len(ip) * sizeof(char));
  str_copy(n->ip,ip);
  n->port = port;
  stralloc_free(&sa_data);
  finger_init(n);
  return n;
}

node *
find_successor(node * n, unsigned char *id)
{
  node *p;
  if (half_range(n->id,n->successor->id,id))
      return n->successor;
  p = closest_preceding_node(n,id);
  if (byte_equal(n->id,HASHLEN,p->id))
    return p;
  return find_successor(p,id);
}

/*
node *
find_successor(node *n, unsigned char *id)
{
  node *q;
  q = find_predecessor(n,id);
  return q->successor;
}
*/

node *
find_predecessor(node * n, unsigned char *id)
{
  node *p;
  p = n;
  while ( range(p->id,p->successor->id,id) == 0)
    p = closest_preceding_node(p,id);
  return p;
}

node *
closest_preceding_node(node * n, unsigned char *id)
{
  int i;
  finger *f;
  for (i = FTBLSIZ - 1; i > 0; --i)
  {
	  f = &n->finger[i];
    if (f->node != NULL)
      if (range(n->id,id,f->node->id)) {
        return f->node;
      }
  }
  return n;
}

void
chord_join(node *n, node *r)
{
   r->predecessor = NULL;
   r->successor = find_successor(n,r->id);
   r->predecessor = find_predecessor(n,r->id);
   chord_stabilize(r);
   chord_fix_fingers(r);
}

int chord_stabilize(node *n)
{
   node *x;
   assert(n->successor->predecessor != NULL);
   x = n->successor->predecessor;
   if (range(n->id,n->successor->id,x->id))
     n->successor = x;
   //chord_send_notify(n->successor);
   return chord_notify(n->successor,n);
}

int
chord_notify(node *n, node *r)
{
   if ((n->predecessor ==  NULL) || (range(n->predecessor->id,n->id,r->id))) {
     n->predecessor = r;
     return 1;
   }
   return 0;
}

void
finger_init(node *n)
{
  unsigned int i;
  char id[HASHLEN*2];
  char *t;
  finger *f;

  for (i = 1; i < FTBLSIZ; ++i) {
    h2s(id,n->id);
    t = next_finger_id(id,i);
    f = malloc(sizeof(finger));
    n->finger[i] = *f;
    s2h(n->finger[i].id,t) ;
    n->finger[i].node = NULL;
    free(f);
    free(t);
  }
}

void
init_finger_table(node *n, node *s)
{
  unsigned int i;
  char id[HASHLEN*2];
  char *t;
  finger *f;
  s->finger[1].node = s->successor;
  s->predecessor = s->successor->predecessor;
  s->successor->predecessor = n;

  for (i = 1; i < FTBLSIZ; ++i) {
    h2s(id,n->id);
    t = next_finger_id(id,i);
    f = malloc(sizeof(finger));
    n->finger[i] = *f;
    s2h(n->finger[i].id,t) ;
    n->finger[i].node = NULL;
    free(f);
    free(t);
  }
}

int
chord_fix_fingers(node *n)
{
   int i;
   for (i = 1; i < FTBLSIZ; ++i)
     n->finger[i].node  = find_successor(n,n->finger[i].id);
   return 0;
}

/* int ractify(node *n) { } */ /* successor rectifies new node ( as it's predecessor) */
/* int fail(node *n) { } */
/* int update(node *n) { } */ /* replace dead by live successor */
/* int reconcile(node *n) { } */
/* int flush(node *n) { } */ /* remove dead predecessor */

/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
