#pragma once

#include <cassert>
#include <vector>

#include <cpp_utilities/src/lib/print_debugger.h>

namespace HandIsomorphism {
namespace Deck {

template <class Nonnegative>
std::vector<Nonnegative> numCardsRevealedByRound(
    const std::vector<Nonnegative>& numCardsRevealedOnRound) {
  std::vector<Nonnegative> v(numCardsRevealedOnRound.size());
  for (size_t r = 0; r < v.size(); ++r) {
    v[r] = numCardsRevealedOnRound[r] + ((r > 0) ? v[r - 1] : 0);
  }
  return v;
}

const constexpr size_t MAX_NUM_RANKS = 13;
const constexpr size_t MAX_NUM_SUITS = 4;
const char RANK_TO_CHAR[MAX_NUM_RANKS];
const char SUIT_TO_CHAR[MAX_NUM_SUITS];

class Deck {
 public:
  Deck(size_t numRanks,
       size_t numSuits,
       std::vector<size_t>&& numPrivateCards,
       std::vector<size_t>&& numCardsRevealedOnRound)
      : numRanks_(numRanks),
        numSuits_(numSuits),
        numPrivateCards_(std::move(numPrivateCards)),
        numCardsRevealedOnRound_(std::move(numCardsRevealedOnRound)),
        numCardsRevealedByRound_(HandIsomorphism::Deck::numCardsRevealedByRound(
            numCardsRevealedOnRound_)) {
    assert(numRanks_ > 0);
    assert(numSuits_ > 0);
    assert(numRounds() > 0);
  }
  Deck(size_t numRanks,
       size_t numSuits,
       size_t numPlayerCards,
       size_t numPlayers,
       const int* numCardsOnBoardOnRound,
       size_t numRounds)
      : numRanks_(numRanks),
        numSuits_(numSuits),
        numPrivateCards_(numPlayers, numPlayerCards),
        numCardsRevealedOnRound_(numRounds),
        numCardsRevealedByRound_(numRounds) {
    for (size_t r = 0; r < numRounds; ++r) {
      numCardsRevealedOnRound_[r] = numCardsOnBoardOnRound[r];
      numCardsRevealedByRound_[r] =
          numCardsOnBoardOnRound[r] +
          ((r > 0) ? numCardsRevealedByRound_[r - 1] : 0);
    }
    boardFactors_ = HandIsomorphism::Deck::allBoardFactors(
        size() - numPrivateCardsDealt(), numCardsRevealedOnRound_);
  }
  virtual ~Deck() {}

  inline std::vector<CardSet::PossibleCardSets> boardCardSets() const {
    return HandIsomorphism::Deck::allPossibleBoardCardSets(
        numRanks_, numSuits_, numCardsRevealedByRound_);
  }
  inline size_t numRanks() const { return numRanks_; }
  inline size_t numSuits() const { return numSuits_; }
  inline const std::vector<size_t>& numPrivateCards() const {
    return numPrivateCards_;
  }
  inline size_t numPrivateCardsDealt() const {
    size_t s = 0;
    for (const auto n : numPrivateCards_) {
      s += n;
    }
    return s;
  }

  inline const std::vector<size_t>& numCardsRevealedByRound() const {
    return numCardsRevealedByRound_;
  }
  inline size_t numCardsRevealedByRound(size_t r) const {
    return numCardsRevealedByRound_[r];
  }
  inline const std::vector<size_t>& numCardsRevealedOnRound() const {
    return numCardsRevealedOnRound_;
  }
  inline size_t numCardsRevealedOnRound(size_t r) const {
    return numCardsRevealedOnRound_[r];
  }
  inline size_t size() const { return numRanks_ * numSuits_; }
  inline size_t numRounds() const { return numCardsRevealedOnRound_.size(); }

 private:
  size_t numRanks_;
  size_t numSuits_;
  std::vector<size_t> numPrivateCards_;
  std::vector<size_t> numCardsRevealedOnRound_;
  std::vector<size_t> numCardsRevealedByRound_;
};
}
}
