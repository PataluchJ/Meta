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

Neighborhood::Iterator::Iterator(Solution startingSolution, uint64_t left, uint64_t right)
	: startingSolution(startingSolution), left(left), right(right), dist(0), currentCached(false)
{
	// Calculate distance
	uint64_t l = 0;
	uint64_t r = 0;
	while (l != left || r != right) {
		dist++;
		r++;
		if (r >= startingSolution.size()) {
			l++;
			r = l + 1;
		}
	}

	currentSolution.resize(startingSolution.size());
}

void Neighborhood::Iterator::regenerate()
{
	std::copy(startingSolution.begin(), startingSolution.end(), currentSolution.begin());
	//currentSolution.assign(startingSolution.begin(), startingSolution.end());

	auto leftIterator = currentSolution.begin();
	auto rightIterator = currentSolution.begin();

	std::advance(leftIterator, left);
	std::advance(rightIterator, right+1);

	std::reverse(leftIterator, rightIterator);
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
		//else
		//	throw std::exception();
	}
	// Advance forward
	else {
		// If move is possible
		if (dist + distance < startingSolution.size()) {
			while (distance > 0) {
				(*this)++;
				distance--;
			}
		}
		//else
		//	throw std::exception();
	}
}

Neighborhood::Neighborhood(Solution& starting)
	: startingSolution(starting)
{
}

Neighborhood::~Neighborhood()
{
}

Neighborhood::Iterator Neighborhood::begin()
{
	return Iterator(startingSolution, 0, 1);
}

Neighborhood::Iterator Neighborhood::end()
{
	return Iterator(startingSolution, startingSolution.size() - 1, startingSolution.size());
}
