#ifndef _CHORD_H
#define _CHORD_H

#include <stralloc.h>
#include <unistd.h>
#include <uint32.h>
#include <nacl/crypto_hash.h>


#define FTBLSIZ 160
#define HASHLEN 20 /* sha1 20 bytes char (160bit hex number)*/

typedef struct _finger {
  unsigned char id[HASHLEN];
  struct _node *node;
} finger;

typedef struct _node {
	unsigned char id[HASHLEN];
	struct _node *successor;
	struct _node *predecessor;
	char *ip;
	int port;
	finger finger[FTBLSIZ];
} node;

/* chord.c */
void print_node(node *);
void node_free(node *);
void finger_init(node *);
void bootstrap(node *);
void chord_join(node *, node *);
int chord_stabilize(node *);
int chord_notify(node *, node *);
int chord_send_notify(node *);
int chord_send_join(node *);
void update_fingers(node *n);
void update_finger_table(node *n, node *s, int i);
void fingers(node *);
int chord_fix_fingers(node *);
node *node_init(char *, int);
node *closest_preceding_node(node *, unsigned char *);
node *find_successor (node *, unsigned char *);
node *find_predecessor(node *, unsigned char *);

/* tools.c */
int strout(char *);
void hashout(unsigned char *);
void hashout2(unsigned char *);
unsigned long int bpow(unsigned long int, int);
void digest (unsigned char *, unsigned char*, size_t);
size_t scanx(const unsigned char *, unsigned long *);
int hdiff(const void *, const void *, size_t);
size_t h2s(char *, const void * const);
size_t s2h (unsigned char *, const void *);
void panic(const char *);
void say2_error(const char *);

/* finger.c */
char *next_finger_id(const char *, unsigned int);
int range(const unsigned char *, const unsigned char *, const unsigned char *);
int half_range(const unsigned char *, const unsigned char *, const unsigned char *);
int hash_compare(const unsigned char *, const unsigned char *);

#endif
/* vim: set ft=c et sw=2 ts=2 sts=2 cinoptions=2 tw=79 :*/
