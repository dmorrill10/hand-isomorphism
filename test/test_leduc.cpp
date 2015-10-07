#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <string>

#include <cpp_utilities/src/lib/print_debugger.h>

#include <test_helper.hpp>

extern "C" {
#include <hand_index.h>
#include <deck.h>
}

SCENARIO("Indexing single cards") {
  const card_t num_suits = 4;
  const card_t num_ranks = 13;
  hand_indexer_t patient;
  GIVEN("A one round game with one card") {
    const card_t cards_per_round[] = {1};
    const auto success = hand_indexer_init(1, cards_per_round, &patient);
    REQUIRE(success);
    THEN("The size is computed properly") {
      CHECK(hand_indexer_size(&patient, 0) == 13);
    }
    for (card_t rank = 0; rank < num_ranks; ++rank) {
      for (card_t suit = 0; suit < num_suits; ++suit) {
        GIVEN("A card") {
          const auto card = deck_make_card(suit, rank);

          THEN("It is properly converted to its canonical version") {
            CHECK(hand_index_last(&patient, &card) == rank);
          }
        }
      }
    }
  }
  hand_indexer_free(&patient);
}
SCENARIO("Indexing a pair of cards (hole card and board card)") {
  const card_t num_suits = 2;
  const card_t num_ranks = 6;
  hand_indexer_t patient;
  GIVEN("A two round game with one card on each round") {
    const card_t cards_per_round[] = {1, 1};
    const auto success = hand_indexer_init(2, cards_per_round, &patient);
    REQUIRE(success);
    // THEN("The size on the first round is computed properly") {
    //   CHECK(hand_indexer_size(&patient, 0) == 6);
    // }
    for (card_t rank = 0; rank < num_ranks; ++rank) {
      for (card_t suit = 0; suit < num_suits; ++suit) {
        for (card_t rank2 = 0; rank2 < num_ranks; ++rank2) {
          for (card_t suit2 = 0; suit2 < num_suits; ++suit2) {
            hand_indexer_state_t state;
            hand_indexer_state_init(&patient, &state);
            GIVEN("The first card") {
              const auto card = deck_make_card(suit, rank);

              THEN("It is properly converted to its canonical version") {
                CHECK(hand_index_next_round(&patient, &card, &state) == rank);
              }
              GIVEN("The second card") {
                const auto card2 = deck_make_card(suit2, rank2);
                THEN("It is properly converted to its canonical version") {
                  CHECK(hand_index_next_round(&patient, &card2, &state) ==
                        rank);
                }
              }
            }
          }
        }
      }
    }
  }
  hand_indexer_free(&patient);
}
