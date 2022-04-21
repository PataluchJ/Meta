#ifndef META_SOLUTION_H_
#define META_SOLUTION_H_

#include <vector>
#include <iterator>
#include <cstddef>
#include <exception>
#include <algorithm>
#include <memory>

using Solution = std::vector<uint32_t>;
using SolutionPointer = std::shared_ptr<Solution>;
/*
Iteratory:
	begin:	left = 0,		right = 1
	end:	left = size-1;	right = size
	front_end:	left = 0		right = 0
	back_end:	left = size-2;	right = size-1

	Const_Iterator: cbegin(), cend()
*/
class Neighborhood {
public:
	Neighborhood(Solution& starting);
	~Neighborhood();

	struct Iterator {
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Solution;
		using pointer = Solution*;
		using reference = Solution&;

		reference operator*();
		pointer operator->();
		Iterator& operator++();
		Iterator& operator--();
		Iterator operator++(int);
		Iterator operator--(int);
		
		friend bool operator==(const Iterator& a, const Iterator& b);
		friend bool operator!=(const Iterator& a, const Iterator& b);
		friend bool operator<(const Iterator& a, const Iterator& b);
		friend bool operator>(const Iterator& a, const Iterator& b);
		friend bool operator<=(const Iterator& a, const Iterator& b);
		friend bool operator>=(const Iterator& a, const Iterator& b);

		Iterator(Solution startingSolution, uint64_t left, uint64_t right);

	private:
		uint64_t left, right;
		uint64_t dist;
		Solution startingSolution;
		Solution currentSolution;

		bool currentCached;

		void regenerate();
		void advance(int64_t distance);
	};

	Iterator begin();
	Iterator end();
private:
	Solution startingSolution;
};

#endif

