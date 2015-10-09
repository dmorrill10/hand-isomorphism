/**
 * hand_index.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 13, 2013
 *
 * map poker hands to an index shared by all isomorphic hands, and
 * map an index to a canonical poker hand
 */

#ifndef _HAND_INDEX_H_
#define _HAND_INDEX_H_

#include <inttypes.h>
#include <stddef.h>
#include <cassert>
#include <cpp_utilities/src/lib/array.h>

#include "deck.h"

namespace HandIsomorphism {
const constexpr size_t MAX_ROUNDS = 8;
const constexpr size_t PRIhand_index = PRIu64;

namespace HandIndex {
typedef uint64_t hand_index_t;

class hand_indexer_t {
 public:
  /**
 * Initialize a hand indexer.  This generates a number of lookup tables and is
 * relatively
 * expensive compared to indexing a hand.
 *
 * @param deck
 */
  hand_indexer_t(const Deck::Deck& deck)
      : deck_(deck),
        configurations(deck_.numRounds(), 0),
        permutations(deck_.numRounds()),
        round_size(deck_.numRounds()),
        permutation_to_configuration(deck_.numRounds()),
        permutation_to_pi(deck_.numRounds()),
        configuration_to_equal(deck_.numRounds()),
        configuration(deck_.numSuits(), deck_.numRounds()),
        configuration_to_suit_size(deck_.numSuits(), deck_.numRounds()),
        configuration_to_offset(deck_.numRounds()) {
    assert(deck_.numRounds() <= MAX_ROUNDS);
    assert(deck_.numCardsRevealedByRound(deck_.numRounds() - 1) <= CARDS);

    enumerate_configurations(deck_.numRounds(),
                             deck_.numCardsRevealedOnRound().data(),
                             count_configurations, configurations.data());

    for (uint_fast32_t i = 0; i < deck_.numRounds(); ++i) {
      configuration_to_equal[i] =
          calloc(configurations[i], sizeof(uint_fast32_t));
      configuration_to_offset[i] =
          calloc(configurations[i], sizeof(hand_index_t));
      configuration[i] =
          calloc(configurations[i], SUITS * sizeof(uint_fast32_t));
      configuration_to_suit_size[i] =
          calloc(configurations[i], SUITS * sizeof(uint_fast32_t));
      assert(!(!configuration_to_equal[i] || !configuration_to_offset[i] ||
               !configuration[i] || !configuration_to_suit_size[i]));
    }

    configurations.assign(configurations.size(), 0);
    enumerate_configurations(deck_.numRounds(),
                             deck_.numCardsRevealedOnRound().data(),
                             tabulate_configurations, this);

    for (uint_fast32_t i = 0; i < deck_.numRounds(); ++i) {
      hand_index_t accum = 0;
      for (uint_fast32_t j = 0; j < configurations[i]; ++j) {
        hand_index_t next = accum + configuration_to_offset[i][j];
        configuration_to_offset[i][j] = accum;
        accum = next;
      }
      round_size[i] = accum;
    }

    memset(permutations, 0, sizeof(permutations));
    enumerate_permutations(deck_.numRounds(), cards_per_round,
                           count_permutations, this);

    for (uint_fast32_t i = 0; i < deck_.numRounds(); ++i) {
      indexer->permutation_to_configuration[i] =
          calloc(indexer->permutations[i], sizeof(uint_fast32_t));
      indexer->permutation_to_pi[i] =
          calloc(indexer->permutations[i], sizeof(uint_fast32_t));
      if (!indexer->permutation_to_configuration ||
          !indexer->permutation_to_pi) {
        hand_indexer_free(indexer);
        return false;
      }
    }

    enumerate_permutations(deck_.numRounds(), cards_per_round,
                           tabulate_permutations, indexer);

    return true;
  }
  ~hand_indexer_t() {
    for (uint_fast32_t i = 0; i < deck_.numRounds(); ++i) {
      free(permutation_to_configuration[i]);
      free(permutation_to_pi[i]);
      free(configuration_to_equal[i]);
      free(configuration_to_offset[i]);
    }
  }

  /**
   * @param indexer
   * @param round
   * @returns size of index for hands on round
   */
  hand_index_t size(uint_fast32_t round) {
    assert(round < deck_.numRounds());
    return round_size[round];
  }

  inline size_t numRounds() const { return deck_.numRounds(); }

  inline const std::vector<size_t>& cards_per_round() const {
    return deck_.numCardsRevealedOnRound();
  }

 public:
  std::vector<uint_fast32_t> configurations;
  std::vector<uint_fast32_t> permutations;
  std::vector<hand_index_t> round_size;

  std::vector<uint_fast32_t*> permutation_to_configuration;
  std::vector<uint_fast32_t*> permutation_to_pi;
  std::vector<uint_fast32_t*> configuration_to_equal;
  Utilities::Array::MultiDimensionalArray<uint_fast32_t> configuration;
  Utilities::Array::MultiDimensionalArray<uint_fast32_t>
      configuration_to_suit_size;
  std::vector<hand_index_t*> configuration_to_offset;

 private:
  const Deck::Deck& deck_;

  /**
   * Initialize a hand index state.  This is used for incrementally indexing a
   *hand as
   * new rounds are dealt and determining if a hand is canonical.
   *
   * @param indexer
   * @param state
   */
  void hand_indexer_state_init(const hand_indexer_t* indexer,
                               hand_indexer_state_t* state);

  /**
   * Index a hand on every round.  This is not more expensive than just indexing
   *the last round.
   *
   * @param indexer
   * @param cards
   * @param indices
   * @returns hand's index on the last round
   */
  hand_index_t hand_index_all(const hand_indexer_t* indexer,
                              const card_t cards[],
                              hand_index_t indices[]);

  /**
   * Index a hand on the last round.
   *
   * @param indexer
   * @param cards
   * @returns hand's index on the last round
   */
  hand_index_t hand_index_last(const hand_indexer_t* indexer,
                               const card_t cards[]);

  /**
   * Incrementally index the next round.
   *
   * @param indexer
   * @param cards the cards for the next round only!
   * @param state
   * @returns the hand's index at the latest round
   */
  hand_index_t hand_index_next_round(const hand_indexer_t* indexer,
                                     const card_t cards[],
                                     hand_indexer_state_t* state);

  /**
   * Recover the canonical hand from a particular index.
   *
   * @param indexer
   * @param round
   * @param index
   * @param cards
   * @returns true if successful
   */
  bool hand_unindex(const hand_indexer_t* indexer,
                    uint_fast32_t round,
                    hand_index_t index,
                    card_t cards[]);
};

class hand_indexer_state_t {
 public:
  hand_indexer_state_t();
  ~hand_indexer_state_t() {}
};
}
}

#endif /* _HAND_INDEX_H_ */
