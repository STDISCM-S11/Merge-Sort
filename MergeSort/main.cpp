#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <deque>
#include <chrono>
#include <random>
#include <cmath>
#include <condition_variable>

using namespace std;

typedef pair<int, int> ii;

// Task queue and mutex
bool shouldTerminate = false;

int N;
deque<ii> taskQ;
condition_variable waitC;
condition_variable taskC;
mutex taskM;

atomic<int> ctr(0);


vector<ii> generate_intervals(int start, int end);
void merge(vector<int>& array, int s, int e);

void print_array(const vector<int>& array) {
    for (int num : array) {
        cout << num << " ";
    }
    cout << endl;
}

// Thread function for merging
void merge_task(vector<int>& array) {
   while (!shouldTerminate) {
        unique_lock<std::mutex> lock(taskM);

        taskC.wait(lock, [&]() {
            return !taskQ.empty() || shouldTerminate;
            });

        if (shouldTerminate) {
            if (taskQ.empty()) {
                lock.unlock();
                taskC.notify_all();
                continue;
            }
        }

        ii interval = taskQ.front();
        taskQ.pop_front();
        lock.unlock();

        merge(array, interval.first, interval.second);
        ctr.fetch_add(1);
        waitC.notify_one();
    }
}

// For automated testing
//double run_merge_sort(int N, int threadCount) {

    //for (int i = 0; i < N; ++i) {
    //    array[i] = i + 1;
    //}
    //random_shuffle(array.begin(), array.end());
    //
    //vector<ii> intervals = generate_intervals(0, N - 1);
    //
    //// Fill the task queue with merge tasks
    //auto start = chrono::high_resolution_clock::now();
    //
    //// Create and start threads
    //vector<thread> threads;
    //for (int i = 0; i < threadCount; ++i) {
    //    threads.emplace_back(merge_task, ref(array));
    //}
    //
    //int pos = 0;
    //int bottomLevel = static_cast<int> (floor(log2(N))) + 1;
    //
    //int j = bottomLevel + 1;
    //
    //while (j > 0) {
    //    int nNodes = static_cast<int>(pow(2, j - 1));
    //
    //    nNodes = N != nNodes && j == bottomLevel + 1 ? 2 * N - nNodes
    //        : nNodes;
    //
    //    int s = pos;
    //
    //    for (int i = pos; i < nNodes + s; i++) {
    //        {
    //            unique_lock<mutex> lock(taskM);
    //            taskQ.push_back(intervals[pos]);
    //            ctr.fetch_sub(1);
    //            pos++;
    //        }
    //
    //        taskC.notify_one();
    //    }
    //
    //    j--;
    //    unique_lock<mutex> lock(taskM);
    //    waitC.wait(lock, [&]() {
    //        return ctr.load() == 0;
    //        });
    //}
    //
    //shouldTerminate = true;
    //
    //taskC.notify_all();
    //
    //
    //// Join threads
    //for (auto& t : threads) {
    //    t.join();
    //}
    //
    //auto end = chrono::high_resolution_clock::now();
    //chrono::duration<double> duration = end - start;
    //
    //bool isSorted = is_sorted(array.begin(), array.end());
    //cout << "Concurrent execution time: " << duration.count() << " seconds" << endl;
    //cout << "Array is " << (isSorted ? "sorted" : "not sorted") << endl;
    //    // Optional: Check if the array is sorted
    // return duration.count();
//}


int main() {
    const unsigned int SEED = 1;
    srand(SEED);

    int N, threadCount;
    cout << "Enter the size of the array: ";
    cin >> N;
    cout << "Enter the thread count: ";
    cin >> threadCount;

    vector<int> array(N);
    for (int i = 0; i < N; ++i) {
        array[i] = i + 1;
    }
    random_shuffle(array.begin(), array.end());

    vector<ii> intervals = generate_intervals(0, N - 1);

    // Fill the task queue with merge tasks
    auto start = chrono::high_resolution_clock::now();

    // Create and start threads
    vector<thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(merge_task, ref(array));
    }

    int pos = 0;
    int bottomLevel = static_cast<int> (floor(log2(N))) + 1;

    int j = bottomLevel + 1;

    while (j > 0) {
        int nNodes = static_cast<int>(pow(2, j - 1));

        nNodes = N != nNodes && j == bottomLevel + 1 ? 2 * N - nNodes
            : nNodes;

        int s = pos;

        for (int i = pos; i < nNodes + s; i++) {
            {
                unique_lock<mutex> lock(taskM);
                taskQ.push_back(intervals[pos]);
                ctr.fetch_sub(1);
                pos++;
            }

            taskC.notify_one();
        }

        j--;
        unique_lock<mutex> lock(taskM);
        waitC.wait(lock, [&]() {
            return ctr.load() == 0;
            });
    }

    shouldTerminate = true;

    taskC.notify_all();


    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    bool isSorted = is_sorted(array.begin(), array.end());
    cout << "Concurrent execution time: " << duration.count() << " seconds" << endl;
    cout << "Array is " << (isSorted ? "sorted" : "not sorted") << endl;

    //Optional: Check if the array is sorted

    // For automated testing
    //const int N = pow(2, 23);
    //const int numTests = 5;
    //
    //vector<int> threadCounts = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    //for (int threadCount : threadCounts) {
    //    double totalTime = 0.0;
    //    for (int i = 0; i < numTests; ++i) {
    //        totalTime += run_merge_sort(N, threadCount);
    //    }
    //    cout << "\n";
    //    double avgTime = totalTime / numTests;
    //    cout << "Thread Count: " << threadCount;
    //    cout << " Average Time: " << avgTime << " seconds\n" << endl;
    //}

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