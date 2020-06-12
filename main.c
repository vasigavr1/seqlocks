#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "seqlock.h"

#define VALUE_SIZE 56
typedef struct {
  seqlock_t seqlock;
  uint8_t value[VALUE_SIZE];
} some_data_type_t;

int main() {

  /* Showcasing usage */

  some_data_type_t *data_item = calloc(1, sizeof(some_data_type_t));

  // 1. Write the item
  uint8_t *new_value = calloc(1, VALUE_SIZE);
  memset(new_value, 1, VALUE_SIZE);
  lock_seqlock(&data_item->seqlock);
  // Modify the item
  memcpy(data_item->value, new_value, (size_t) VALUE_SIZE);
  unlock_seqlock(&data_item->seqlock);


  // 2. Read the item
  uint8_t  *read_value = malloc(VALUE_SIZE);
  uint64_t tmp_lock = read_seqlock_lock_free(&data_item->seqlock);
  do {
    memcpy(read_value, data_item->value, (size_t) VALUE_SIZE);
  } while (!(check_seqlock_lock_free(&data_item->seqlock, &tmp_lock)));

  return 0;
}