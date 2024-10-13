#pragma once

#include <random>
#include <vector>
#include <algorithm>

#include "QuizCards.h"

inline std::random_device rd;
inline std::mt19937 g{ rd() };

class ShuffleType
{
public:
	virtual ~ShuffleType() = default;
	virtual void Shuffle(std::vector<QuizCard>& deck) = 0;
};

class RandomShuffle : public ShuffleType
{
public:
	void Shuffle(std::vector<QuizCard>& deck) override
	{
		std::shuffle(deck.begin(), deck.end(), g);
	}
};


class MultiplyShuffle : public ShuffleType
{
public:
	MultiplyShuffle(int amount) : m_amount(amount) {}

	void Shuffle(std::vector<QuizCard>& deck) override
	{
		std::shuffle(deck.begin(), deck.end(), g);
		std::vector<QuizCard> new_deck;
		for (const auto& card : deck)
		{
			new_deck.insert(new_deck.end(), m_amount, card);
		}
		deck = std::move(new_deck);
	}

private:
	int m_amount;
};

template<typename T>
inline void shuffleMultiply(std::vector<T>& deck, int amount)
{
	std::shuffle(deck.begin(), deck.end(), g);
}
