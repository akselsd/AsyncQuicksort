#ifndef QUICKSORT_HPP_INCLUDED
#define QUICKSORT_HPP_INCLUDED
#include <algorithm>
#include <deque>
#include <future>
#include <iterator>
#include <thread>
#include <tuple>
#include <utility>

namespace 
{

constexpr int N_CORES = 4;

template <class T>
inline void selection_sort(T begin, T end)
{
	for (T it = std::next(begin); it != end; std::advance(it, 1))
		std::rotate(std::upper_bound(begin, it, *it), it, std::next(it));
}

template<class T>
inline T partition(T start, T end)
{
	T last = std::prev(end);
	auto it = std::partition(start, last,
		[&](const int & n){
			return (n <= *last);
		});
	std::iter_swap(it, last);
	return it;
}
	
} /* Namespace */

template<class T>
void quicksort(T begin, T end)
{
	std::deque<std::tuple<T, T>> indexes;
	indexes.emplace_back(std::make_tuple(begin, end));

	while (!indexes.empty()){
		std::tuple<T, T> i_tuple = indexes.back();
		indexes.pop_back();
		T begin_ = std::get<0>(i_tuple);
		T end_ = std::get<1>(i_tuple);

		if (std::distance(begin_, end_) < 16){
			selection_sort(begin_, end_);
			continue;
		}
		
		T p = partition(begin_, end_);

		if (std::distance(begin_, p) > 1)
			indexes.emplace_back(std::make_tuple(begin_, p));

		if (std::distance(p, end_) > 1)
			indexes.emplace_back(std::make_tuple(p, end_));
	}
}

template<class T>
void quicksort_async(T begin, T end, int unused_cores)
{
	if (std::distance(begin, end) < 16){
		selection_sort(begin, end);
		return;
	}

	if (unused_cores == 0){
		quicksort(begin, end);
		return;
	}

	T p = partition(begin, end);

	int thread_core_tokens = (unused_cores - 1) / 2;

	auto fut = std::async(
		std::launch::async,
		quicksort_async<T>,
		begin,
		p,
		thread_core_tokens);

	quicksort_async(p, end, (unused_cores - 1) - thread_core_tokens);
	fut.wait();
}


template<class T>
void quicksort_async_wrapper(T begin, T end)
{
	quicksort_async(begin, end, N_CORES - 1);
}

template<class T>
void quicksort_async_unbound(T begin, T end)
{
	if (std::distance(begin, end) < 16){
		selection_sort(begin, end);
		return;
	}

	T p = partition(begin, end);

	/* Launch one async thread if needed
	 * else do the work on this thread
	 */

	if (std::distance(begin, p) > 1){
		if (std::distance(p, end) > 1){
			auto fut = std::async(
				std::launch::async,
				quicksort_async_unbound<T>,
				begin,
				p);

			quicksort_async_unbound(p, end);
			fut.wait();
		}
		else
			quicksort_async_unbound(begin, p);
	}
	else if (std::distance(p, end) > 1)
		quicksort_async_unbound(p, end);
}
#endif
