#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>

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

    vector<int> array(N);
    for (int i = 0; i < N; ++i) {
        array[i] = i + 1;
    }
    random_shuffle(array.begin(), array.end());

    // TODO: Call the generate_intervals method to generate the merge sequence
    vector<ii> intervals = generate_intervals(0, n - 1);

    // Fill the task queue with merge tasks
    for (auto& interval : intervals) {
        task_queue.push(interval);
    }

    auto start = chrono::high_resolution_clock::now();

    // Create and start threads
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(merge_task, ref(array));
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout << "Multithreaded merge sort took " << diff.count() << " seconds." << endl;

    // Optional: Check if the array is sorted

    return 0;
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
        // no more elements on left half
        if (l_ptr == (int)left.size()) {
            array[i] = right[r_ptr];
            r_ptr++;

            // no more elements on right half or left element comes first
        }
        else if (r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr]) {
            array[i] = left[l_ptr];
            l_ptr++;
        }
        else {
            array[i] = right[r_ptr];
            r_ptr++;
        }
    }
}