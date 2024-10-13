#pragma once

#include <vector>
#include <memory>

#include "Shuffles.h"
#include "QuizCards.h"

class Quiz
{
public:
	Quiz(std::vector<QuizCard> deck, std::shared_ptr<ShuffleType> shuffleStrategy);

	std::string& GetCurrentQuestion() const;
	std::string& GetCurrentAnswer() const;
	QuizCard GetCurrentCard() const;

	void NextCard();
	void ShuffleDeck();
	void Swap();

	void SetShuffleClass(std::shared_ptr<ShuffleType> newStrategy);

private:
	std::shared_ptr<ShuffleType> m_shuffle;
	std::vector<QuizCard> m_deckArchetype;
	std::vector<QuizCard> m_deck;
	std::vector<QuizCard>::iterator m_currentCard;
};