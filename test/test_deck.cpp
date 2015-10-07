#include <test_helper.hpp>
#include <deck.h>

using namespace HandIsomorphism;

SCENARIO("Deck") {
  GIVEN("Kuhn deck") {
    Deck::Deck patient(3, 1, {1, 1}, {0});
    THEN("Properties are set properly") {
      CHECK(patient.numRanks() == 3);
      CHECK(patient.numSuits() == 1);
      CHECK(patient.size() == 3);
      CHECK(patient.numPrivateCards()[0] == 1);
      CHECK(patient.numPrivateCards()[1] == 1);
      CHECK(patient.numPrivateCardsDealt() == 2);
      CHECK(patient.numCardsRevealedOnRound()[0] == 0);
      CHECK(patient.numCardsRevealedByRound(0) == 0);
      CHECK(patient.numRounds() == 1);
    }
  }
  GIVEN("Leduc deck") {
    Deck::Deck patient(3, 2, {1, 1}, {0, 1});
    THEN("Properties are set properly") {
      CHECK(patient.numRanks() == 3);
      CHECK(patient.numSuits() == 2);
      CHECK(patient.size() == 6);
      CHECK(patient.numPrivateCards()[0] == 1);
      CHECK(patient.numPrivateCards()[1] == 1);
      CHECK(patient.numCardsRevealedOnRound()[0] == 0);
      CHECK(patient.numCardsRevealedOnRound()[1] == 1);
      CHECK(patient.numPrivateCardsDealt() == 2);
      CHECK(patient.numCardsRevealedByRound(0) == 0);
      CHECK(patient.numCardsRevealedByRound(1) == 1);
      CHECK(patient.numRounds() == 2);
    }
  }
  GIVEN("Texas Hold'em deck") {
    Deck::Deck patient(13, 4, {2, 2}, {0, 3, 1, 1});
    THEN("Properties are set properly") {
      CHECK(patient.numRanks() == 13);
      CHECK(patient.numSuits() == 4);
      CHECK(patient.size() == 52);
      CHECK(patient.numPrivateCards()[0] == 2);
      CHECK(patient.numPrivateCards()[1] == 2);
      CHECK(patient.numCardsRevealedOnRound()[0] == 0);
      CHECK(patient.numCardsRevealedOnRound()[1] == 3);
      CHECK(patient.numCardsRevealedOnRound()[2] == 1);
      CHECK(patient.numCardsRevealedOnRound()[3] == 1);
      CHECK(patient.numPrivateCardsDealt() == 4);
      CHECK(patient.numCardsRevealedByRound(0) == 0);
      CHECK(patient.numCardsRevealedByRound(1) == 3);
      CHECK(patient.numCardsRevealedByRound(2) == 4);
      CHECK(patient.numCardsRevealedByRound(3) == 5);
      CHECK(patient.numRounds() == 4);
    }
  }
}
