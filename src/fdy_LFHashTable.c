#include <stdio.h>
#include <stdlib.h>
#include "fdy_LFHashTable.h"

/***
* Author: cloudleware2015@gmail.com - taymindis
* Lock Free hashtable, The faster travese and read, free and writeable
* For single/multithreading purposes.
* LockFree Hashtable does not care you modify share memory when other people using.
* Versuib 0.0.1, 02-Nov-2016
****/
void* fdy_LF_alloc_(fdy_LFHashTable *lf_hashtable);
int fdy_LF_free_(fdy_LFHashTable *lf_hashtable, struct fdy_LFNode_s *n);

void* fdy_LF_alloc_(fdy_LFHashTable *lf_hashtable) {

	struct fdy_LFNode_s * return_ = lf_hashtable->node_buf;

RETRY:
	while (atomic_flag_test_and_set_explicit(&return_->used, memory_order_acquire)) {
		return_++;
	}

	if (return_ == NULL && atomic_load(&lf_hashtable->size) < lf_hashtable->total_size) goto RETRY;

	return return_;
}

int fdy_LF_free_(fdy_LFHashTable *lf_hashtable, struct fdy_LFNode_s *n) {
	n->key = 0;
	n->val = NULL;

	atomic_flag_clear_explicit(&n->used, memory_order_release);

	return 1;
}


/*API Endpoint*/

int fdy_LF_init(fdy_LFHashTable *lf_hashtable, size_t max_size) {

	lf_hashtable->total_size = max_size;
	max_size += 3; // give some padding

	/** Pre-allocate all nodes **/
	lf_hashtable->node_buf = malloc(max_size * sizeof(struct fdy_LFNode_s));

	if (lf_hashtable->node_buf == NULL)
		return 0;

	for (size_t i = 0; i < max_size - 1; i++) {
		lf_hashtable->node_buf[i].key = 0;
		lf_hashtable->node_buf[i].val = NULL;
		lf_hashtable->node_buf[i].next = lf_hashtable->node_buf + i + 1;
		lf_hashtable->node_buf[i].used = (atomic_flag)ATOMIC_FLAG_INIT;
		// atomic_flag_clear(&lf_hashtable->node_buf[i].used);

	}
	//For Last Node
	lf_hashtable->node_buf[max_size - 1].next = NULL;

	// Atomic Init
	lf_hashtable->size = ATOMIC_VAR_INIT(0);

	return 0;
}

int fdy_LF_put(fdy_LFHashTable *lf_hashtable, LFKey key, void *value) {
	// We add first to prevent any error;
	if (atomic_fetch_add(&lf_hashtable->size, 1) < lf_hashtable->total_size) {
		struct fdy_LFNode_s *node = fdy_LF_alloc_(lf_hashtable);
		if (node) {
			// atomic_fetch_add(&lf_hashtable->size, 1);
			node->val = value;
			node->key = key;
			return 1;
		} else {
			// If can't add more, rollback
			atomic_fetch_sub(&lf_hashtable->size, 1);
		}
	}
	// If can't add more, rollback
	atomic_fetch_sub(&lf_hashtable->size, 1);
	return 0;
}

void *fdy_LF_pop(fdy_LFHashTable *lf_hashtable, LFKey key) {
	void * return_ = NULL;

	struct fdy_LFNode_s *buffer = lf_hashtable->node_buf;

	do {
		if (((LFKey)buffer->key) == ((LFKey)key)) goto SUCCESS;
	} while ((buffer = buffer->next) != NULL);
	// printf("%s by key %lu\n", "return null", key);
SUCCESS:
	return_ = buffer->val;
	if (fdy_LF_free_(lf_hashtable, buffer)) {
		atomic_fetch_sub(&lf_hashtable->size, 1);
	}
	return return_;
}

void *fdy_LF_get(fdy_LFHashTable *lf_hashtable, LFKey key) {
	void * return_ = NULL;

	struct fdy_LFNode_s *buffer = lf_hashtable->node_buf;

	do {
		if (buffer->key == key) goto SUCCESS;
	} while ((buffer = buffer->next) != NULL);
	// printf("%s by key %lu\n", "return null", key);
	return return_;
SUCCESS:
	return_ = buffer->val;
	return return_;
}

// need to implement fdy_LF_read(fdy_LFHashTable *lf_hashtable, LFKey key)

void fdy_LF_destroy(fdy_LFHashTable *lf_hashtable) {
	free(lf_hashtable->node_buf);
}