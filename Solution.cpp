#include "Solution.h"

bool operator==(const Neighborhood::Iterator & a, const Neighborhood::Iterator& b) {
	return a.dist == b.dist;
}
bool operator!=(const Neighborhood::Iterator& a, const Neighborhood::Iterator& b) {
	return a.dist != b.dist;
}
bool operator<(const Neighborhood::Iterator& a, const Neighborhood::Iterator& b) {
	return a.dist < b.dist;
}
bool operator>(const Neighborhood::Iterator& a, const Neighborhood::Iterator& b) {
	return a.dist > b.dist;
}
bool operator<=(const Neighborhood::Iterator& a, const Neighborhood::Iterator& b) {
	return a.dist <= b.dist;
}
bool operator>=(const Neighborhood::Iterator& a, const Neighborhood::Iterator& b) {
	return a.dist >= b.dist;
}

bool operator==(const Neighborhood::Iterator::Move& a, const Neighborhood::Iterator::Move& b)
{
	return a.l == b.l && a.r == b.r;
}

Neighborhood::Iterator::reference Neighborhood::Iterator::operator*()
{
	if (!currentCached)
		regenerate();
	return currentSolution;
}

Neighborhood::Iterator::pointer Neighborhood::Iterator::operator->()
{
	if (!currentCached)
		regenerate();
	return &currentSolution;
}

Neighborhood::Iterator& Neighborhood::Iterator::operator++()
{
	if (left == (startingSolution.size() - 1) && right == startingSolution.size()) 
		return *this;

	right++;
	if (right >= startingSolution.size()) {
		left++;
		right = left + 1;
	}

	dist++;
	currentCached = false;

	return *this;
}

Neighborhood::Iterator& Neighborhood::Iterator::operator--()
{
	if (left == right + 1) {
		if (left == 0) {
			right = 0;
		}
		else {
			left--;
			right = startingSolution.size() - 1;
		}
	}
	else 
		right--;

	dist--;
	currentCached = false;

	return *this;
}

Neighborhood::Iterator Neighborhood::Iterator::operator++(int)
{
	Iterator temp = *this;
	++(*this);
	return temp;
}

Neighborhood::Iterator Neighborhood::Iterator::operator--(int)
{
	Iterator temp = *this;
	--(*this);
	return temp;
}

Neighborhood::Iterator::Iterator(Solution startingSolution, uint32_t l, uint32_t r, NeighborhoodFunction nf)
	: startingSolution(startingSolution), left(0), right(1), dist(1), currentCached(false), nf(nf)
{
	if (l == 0 && r == 0 || l > r) {
		left = 0;
		right = 0;
		dist = 0;
	}
	// Advanve iterator
	else
		while (l != left || r != right) 
			(*this)++;
	currentSolution.resize(startingSolution.size());
}

void Neighborhood::Iterator::regenerate()
{
//	std::copy(startingSolution.begin(), startingSolution.end(), currentSolution.begin());
	currentSolution.assign(startingSolution.begin(), startingSolution.end());
	//currentSolution.assign(startingSolution.begin(), startingSolution.end());

	auto leftIterator = currentSolution.begin();
	auto rightIterator = currentSolution.begin();

	std::advance(leftIterator, left);
	std::advance(rightIterator, right);

	switch (nf) {
	case NeighborhoodFunction::Insert: {
		auto value = *leftIterator;
		currentSolution.insert(++rightIterator, value);
		leftIterator = currentSolution.begin();
		std::advance(leftIterator, left);
		currentSolution.erase(leftIterator);
	} break;
	case NeighborhoodFunction::Reverse: {
		std::reverse(leftIterator, ++rightIterator);
	} break;
	case NeighborhoodFunction::Swap: {
		std::swap(*leftIterator, *rightIterator);
	} break;
	}
	
	currentCached = true;
}

void Neighborhood::Iterator::advance(int64_t distance)
{
	// Advance back
	if (distance < 0) {
		distance *= -1;
		// If move is possible
		if (dist >= distance) {
			while (distance > 0) {
				(*this)--;
				distance--;
			}
		}
		else {
			left = 0;
			right = 1;
		}
	}
	// Advance forward
	else {
		// If move is possible
		while (distance > 0) {
			(*this)++;
			distance--;
		}
	}
}

uint64_t Neighborhood::Iterator::getCurrentDistance()
{
	return dist;
}

Neighborhood::Neighborhood(Solution& starting, NeighborhoodFunction fun)
	: startingSolution(starting), nf(fun)
{
}

Neighborhood::~Neighborhood()
{
}

Neighborhood::Iterator Neighborhood::begin()
{
	return Iterator(startingSolution, 0, 1, nf);
}

Neighborhood::Iterator Neighborhood::end()
{
	return Iterator(startingSolution, startingSolution.size() - 1, startingSolution.size(), nf);
}
