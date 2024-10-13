#include "Quiz.h"

Quiz::Quiz(std::vector<QuizCard> deck, std::shared_ptr<ShuffleType> shuffleStrategy)
	: m_shuffle(std::move(shuffleStrategy)),
	m_deckArchetype(std::move(deck)),
	m_deck(m_deckArchetype)
{
	ShuffleDeck();
}

std::string& Quiz::GetCurrentQuestion() const
{
	return m_currentCard->question;
}

std::string& Quiz::GetCurrentAnswer() const
{
	return m_currentCard->answer;
}

QuizCard Quiz::GetCurrentCard() const
{
	return *m_currentCard;
}

void Quiz::NextCard()
{
	++m_currentCard;

	if (m_currentCard == m_deck.end())
	{
		ShuffleDeck();
		m_currentCard = m_deck.begin();
	}
}

void Quiz::ShuffleDeck()
{
	m_deck = m_deckArchetype;
	m_shuffle->Shuffle(m_deck);
	m_currentCard = m_deck.begin();
}

void Quiz::Swap()
{
	std::vector<QuizCard> oldDeck = m_deck;
	std::vector<QuizCard> newDeck;
	for (auto& card : oldDeck)
	{
		newDeck.emplace_back(card.answer, card.question);
	}

	m_deck = newDeck;
	m_currentCard = m_deck.begin();
}

void Quiz::SetShuffleClass(std::shared_ptr<ShuffleType> newClass)
{
	m_shuffle = newClass;
	ShuffleDeck();
}

