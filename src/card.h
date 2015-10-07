/**
 * deck.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 13, 2013
 *
 * utilities for manipulating a standard deck of cards
 */

#ifndef _CARD_H_
#define _CARD_H_

#include <inttypes.h>

namespace HandIsomorphism {
namespace Card {
typedef uint_fast8_t card_t;
typedef uint_fast8_t suit_t;
typedef uint_fast8_t rank_t;

static inline card_t suit(card_t card) {
  return card & 3;
}

static inline card_t rank(card_t card) {
  return card >> 2;
}

static inline card_t card(suit_t suit, rank_t rank) {
  return rank << 2 | suit;
}
}
}

#endif /* _CARD_H_ */
