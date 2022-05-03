#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <algorithm>
#include <numeric>

template<typename Iter, typename T>
struct accumulate_block {
    void operator()(Iter first, Iter last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iter, typename T>
T parallel_accumulate(Iter first, Iter last, T init) {
    unsigned long const length = std::distance(first, last);
    if (!length)
        return init;

    unsigned long const min_per_thread = 25;

    unsigned long const max_threads =
        (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
        std::thread::hardware_concurrency();

    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    
    unsigned long const block_size = length / num_threads;
    
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iter block_start = first;
    
    for (unsigned long i = 0;i < (num_threads - 1);++i) {
        Iter block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(
            accumulate_block<Iter, T>(),
            block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iter, T>()(
        block_start, last, results[num_threads - 1]);

    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);

}


int main(int argc, char const* argv[]) {

    std::vector<int> v;
    for (int i = 0;i < 100; i++) {
        v.push_back(i);
    }

    std::cout << "Accumulated result : "
        << parallel_accumulate(v.begin(), v.end(), 0);

    return 0;
}
