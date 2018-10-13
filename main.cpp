#include <assert.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <chrono>
#include <climits>
#include <random>
#include "quicksort.hpp"


template <class T>
void measure_time(std::function<void(T, T)> f, T begin, T end)
{
	using namespace std::chrono;

	high_resolution_clock clk;

	auto start = time_point_cast<microseconds>(clk.now());
	f(begin, end);
	auto finish = time_point_cast<microseconds>(clk.now());

	if (!std::is_sorted(begin, end)){
		std::cout << "Sorting error" << std::endl;
		assert(0);
	}

	duration<double, std::micro> us = finish - start;	
	std::cout << us.count() << " microseconds" << std::endl;

}

int main()
{
	srand(time(nullptr));
	long n_numbers = 10000000;
	int number_range = 10000;

	std::vector<int> vec;
	vec.reserve(n_numbers);

	for (int i = 0; i < n_numbers; ++i){
		vec.emplace_back(rand() % number_range);
	}
	std::cout << "N = " << n_numbers << std::endl;

	std::vector<int> original(vec);


	using it_type = std::vector<int>::iterator;
	std::function<void(
		it_type,
		it_type)> f = quicksort<it_type>;
	
	std::cout << "Quicksort: ";
	measure_time(f, vec.begin(), vec.end());

	vec = original;

	f = quicksort_async_unbound<it_type>;
	std::cout << "Quicksort Async Unbound: ";
	measure_time(f, vec.begin(), vec.end());

	vec = original;

	f = quicksort_async_wrapper<it_type>;
	std::cout << "Quicksort Async Bounded: ";
	measure_time(f, vec.begin(), vec.end());

	vec = original;

	f = std::sort<it_type>;
	std::cout << "Std::sort: ";
	measure_time(f, vec.begin(), vec.end());
}
