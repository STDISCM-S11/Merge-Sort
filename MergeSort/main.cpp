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

// Task queue and mutex
queue<ii> task_queue;
mutex queue_mutex;
mutex print_mutex;

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
    while (true) {
        ii task;
        {
            lock_guard<mutex> lock(queue_mutex);
            if (task_queue.empty()) {
                return;
            }
            task = task_queue.front();
            task_queue.pop();
        }

        merge(array, task.first, task.second);

        // FOR PRINTING
        // Synchronize and print the array state
        /*{
            lock_guard<mutex> print_lock(print_mutex);
            cout << "After merging [" << task.first << ", " << task.second << "]: ";
            print_array(array);
        }*/
    }
}

// For automated testing
//double run_merge_sort(int N, int threadCount) {
//    vector<int> array(N);
//    for (int i = 0; i < N; ++i) {
//        array[i] = i + 1;
//    }
//    random_shuffle(array.begin(), array.end());
//
//    queue<ii> empty;
//    swap(task_queue, empty);
//
//    vector<ii> intervals = generate_intervals(0, N - 1);
//
//    for (auto& interval : intervals) {
//        task_queue.push(interval);
//    }
//    auto start = chrono::high_resolution_clock::now();
//
//    vector<thread> threads;
//    for (int i = 0; i < threadCount; ++i) {
//        threads.emplace_back(merge_task, ref(array));
//    }
//
//    for (auto& t : threads) {
//        t.join();
//    }
//    auto end = chrono::high_resolution_clock::now();
//    chrono::duration<double> diff = end - start;
//    cout << "Thread Count: " << threadCount;
//    cout << " Time taken: " << diff.count() << " seconds" << endl;
//    return diff.count();
//    // Optional: Check if the array is sorted
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
    cout << "Merge sort took " << diff.count() << " seconds." << endl;

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