//
// Created by vasilis on 12/06/20.
//

#ifndef SEQLOCKS_SEQLOCK_H
#define SEQLOCKS_SEQLOCK_H


#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <assert.h>


#define DEBUG_SEQLOCKS 1
#define THREAD_NUMBER 10 // we want to be able to avoid locking when there is no multithreading
#define ENABLE_LOCK_FREE_READING 1
#define COMPILER_BARRIER() asm volatile ("" ::: "memory")

typedef atomic_uint_fast64_t seqlock_t;

static inline bool is_odd(uint64_t var) {
  return ((var % 2) == 1);
}

static inline bool is_even(uint64_t var) {
  return ((var % 2) == 0);
}

static inline void lock_seqlock(seqlock_t *seqlock)
{
  if (THREAD_NUMBER == 1) return;
  uint64_t tmp_lock, new_lock;
  tmp_lock = (uint64_t) atomic_load_explicit(seqlock, memory_order_acquire);
  do {
    // First spin in your L1, reading until the lock is even
    while (is_odd(tmp_lock)) {
      tmp_lock = (uint64_t) atomic_load_explicit(seqlock, memory_order_acquire);
    }

    new_lock = tmp_lock + 1;
    if (DEBUG_SEQLOCKS) assert(is_odd(new_lock));
  } while(!(atomic_compare_exchange_strong_explicit(seqlock, &tmp_lock,
                                                    new_lock,
                                                    memory_order_acquire,
                                                    memory_order_acquire)));
  if (DEBUG_SEQLOCKS) assert(is_odd ((uint64_t) atomic_load_explicit (seqlock, memory_order_acquire)));


}

static inline void unlock_seqlock(seqlock_t *seqlock)
{
  if (THREAD_NUMBER == 1) return;
  uint64_t tmp = *seqlock;
  if (DEBUG_SEQLOCKS) {
    assert(is_odd(tmp));
  }
  atomic_store_explicit(seqlock, tmp + 1, memory_order_release);
}

// LOCK-free read
static inline uint64_t read_seqlock_lock_free(seqlock_t *seqlock)
{
  if (!ENABLE_LOCK_FREE_READING) {
    lock_seqlock(seqlock);
    return 0;
  }
  uint64_t tmp_lock;
  do {
    tmp_lock = (uint64_t) atomic_load_explicit (seqlock, memory_order_acquire);
  } while (is_odd(tmp_lock));

  return tmp_lock;
}

// return true if the check was successful (loop while it returns false!)
static inline bool check_seqlock_lock_free(seqlock_t *seqlock,
                                           uint64_t *read_lock)
{
  if (!ENABLE_LOCK_FREE_READING) {
    unlock_seqlock(seqlock);
    return true;
  }
  COMPILER_BARRIER();
  uint64_t tmp_lock = (uint64_t) atomic_load_explicit (seqlock, memory_order_acquire);
  if (*read_lock == tmp_lock) return true;
  else {
    while (is_odd(tmp_lock)) {
      tmp_lock = (uint64_t) atomic_load_explicit (seqlock, memory_order_acquire);
    }
    *read_lock = tmp_lock;
    return false;
  }
}



#endif //SEQLOCKS_SEQLOCK_H
