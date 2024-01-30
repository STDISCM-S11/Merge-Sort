#include <iostream>
#include <utility>
#include <vector>

#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <numeric>

using namespace std;

typedef pair<int, int> ii;

/*
This function generates all the intervals for merge sort iteratively, given the
range of indices to sort. Algorithm runs in O(n).

Parameters:
start : int - start of range
end : int - end of range (inclusive)

Returns a list of integer pairs indicating the ranges for merge sort.
*/
vector<ii> generate_intervals(int start, int end);

/*
This function performs the merge operation of merge sort.

Parameters:
array : vector<int> - array to sort
s     : int         - start index of merge
e     : int         - end index (inclusive) of merge
*/
void merge(vector<int>& array, int s, int e);

void iterative_merge_sort(vector<int>& array, const vector<ii>& intervals);
bool is_sorted(const vector<int>& array);

int main() {
    // TODO: Seed your randomizer
    srand(0);

    // TODO: Get array size and thread count from user
    int n, thread_count;
    cout << "Enter number: ";
    cin >> n;
    cout << "Enter thread count: ";
    cin >> thread_count;

    // TODO: Generate a random array of given size
    vector<int> array(n);
    iota(array.begin(), array.end(), 1); // Filling the array with 1 to n
    random_shuffle(array.begin(), array.end()); // Shuffle using seeded randomizer

    // TODO: Call the generate_intervals method to generate the merge sequence
    vector<ii> intervals = generate_intervals(0, n - 1);

    // TODO: Call merge on each interval in sequence
    auto start_time = chrono::high_resolution_clock::now();
    iterative_merge_sort(array, intervals);
    auto end_time = chrono::high_resolution_clock::now();

    for (const auto& arr : array) {
        cout << arr << endl;
    }

    chrono::duration<double, milli> duration = end_time - start_time;
    cout << "Time: " << duration.count() << " milliseconds.\n" << endl;

    // Once you get the single-threaded version to work, it's time to implement 
    // the concurrent version. Good luck :)
}

void iterative_merge_sort(vector<int>& array, const vector<ii>& intervals) {
    for (const auto& interval : intervals) {
        merge(array, interval.first, interval.second);
    }
}

bool is_sorted(const vector<int>& array) {
    for (size_t i = 1; i < array.size(); i++) {
        if (array[i - 1] > array[i]) {
            return false;
        }
    }
    return true;
}

vector<ii> generate_intervals(int start, int end) {
    vector<ii> frontier;
    frontier.push_back(ii(start, end));
    int i = 0;
    while (i < (int)frontier.size()) {
        int s = frontier[i].first;
        int e = frontier[i].second;

        i++;

        // if base case
        if (s == e) {
            continue;
        }

        // compute midpoint
        int m = s + (e - s) / 2;

        // add prerequisite intervals
        frontier.push_back(ii(m + 1, e));
        frontier.push_back(ii(s, m));
    }

    vector<ii> retval;
    for (int i = (int)frontier.size() - 1; i >= 0; i--) {
        retval.push_back(frontier[i]);
    }
    return retval;
}

void merge(vector<int>& array, int s, int e) {
    int m = s + (e - s) / 2; 

    vector<int> left;
    vector<int> right;
    for (int i = s; i <= e; i++) {
        if (i <= m) {
            left.push_back(array[i]);
        }
        else {
            right.push_back(array[i]);
        }
    }
    int l_ptr = 0, r_ptr = 0;


    for (int i = s; i <= e; i++) {
        if (l_ptr >= left.size()) {
            break;
        }

        if (r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr]) {
            array[i] = left[l_ptr];
            l_ptr++;
        }
        else {
            array[i] = right[r_ptr];
            r_ptr++;
        }
    }
}

//void merge(vector<int>& array, int s, int e) {
//    int m = s + (e - s) / 2;
//    vector<int> left(array.begin() + s, array.begin() + m + 1);
//    vector<int> right(array.begin() + m + 1, array.begin() + e + 1);
//
//    int l_ptr = 0, r_ptr = 0, k = s;
//
//    while (l_ptr < left.size() && r_ptr < right.size()) {
//        if (left[l_ptr] <= right[r_ptr]) {
//            array[k++] = left[l_ptr++];
//        }
//        else {
//            array[k++] = right[r_ptr++];
//        }
//    }
//
//    while (l_ptr < left.size()) {
//        array[k++] = left[l_ptr++];
//    }
//
//    while (r_ptr < right.size()) {
//        array[k++] = right[r_ptr++];
//    }
//}
