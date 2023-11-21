#include <iostream>
#include <windows.h>
#include <semaphore>
#include <mutex>
#include <vector>

std::mutex m;

class semaphore
{
    unsigned int count_;
    std::mutex mutex_;
    std::condition_variable condition_;

public:
    explicit semaphore(unsigned int initial_count) : count_(initial_count), mutex_(), condition_() {}

    void signal() //aka release
    {
        std::unique_lock<std::mutex> lock(mutex_);

        ++count_;
        condition_.notify_one();
    }

    void wait() //aka acquire
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ == 0) condition_.wait(lock);
        --count_;
    }

    semaphore() {
        count_ = 0;
    }
};
HANDLE hstop_sem = CreateSemaphore(NULL, 0, 1, NULL);//
HANDLE* hmarker_sem;
semaphore pause_sem(0);
semaphore* marker_sems;

void first(int i, std::vector<int>& v, std::vector<bool>& marked) {
    marker_sems[i].wait();
    WaitForSingleObject(hmarker_sem[i], INFINITE);//
    srand(i);
    int c = 0;
    while (true) {
        int temp = rand() % v.size();
        m.lock();
        if (v[temp] == 0) {
            Sleep(5);
            v[temp] = i;
            Sleep(5);
            c++;
            m.unlock();
        }
        else {
            std::cout << "\n serial number: " << i << ", number of marked elements: " << c << ", can't be marked: " << temp;
            m.unlock();
            //pause_sem.signal();
            ReleaseSemaphore(hstop_sem, 1, 0);//
            WaitForSingleObject(hmarker_sem[i], INFINITE);//
            marker_sems[i].wait();
            if (marked[i]) {
                m.lock();
                for (int j = 0; j < v.size(); j++) 
                    if (v[j] == i) 
                        v[j] = 0;
                m.unlock();
                marker_sems[i].signal();
                ReleaseSemaphore(hmarker_sem, 1, 0); //
                break;
            }
        }
    }
}

int main() {
    int n, k, p = 0;
    std::cout << "array size: ";
    std::cin >> n;
    std::vector<int> v(n);
    std::cout << "number of 'marker' threads: ";
    std::cin >> k;
    int kn = k;
    HANDLE** hmarkers = new HANDLE*[k + 1];//
    std::thread** marker = new std::thread*[k + 1];

    std::vector<bool> marked(k + 1);
    marked[0] = true;
    hmarker_sem = new HANDLE[k + 1]; //
    marker_sems = new semaphore[k + 1];
    for (int i = 1; i < k + 1; i++) {
        marker[i] = new std::thread(first, i, std::ref(v), std::ref(marked));
        marker[i]->detach();
        marker_sems[i].signal();
        ReleaseSemaphore(hmarkers, 1, 0);//
    }
    while (true) {
        for (int i = 1; i < kn + 1; i++) {
            WaitForSingleObject(hstop_sem, INFINITE);//
            //pause_sem.wait();
        }
        std::cout << std::endl;
        m.lock();
        for (int i = 0; i < n; i++) 
            std::cout << v[i] << " ";
        m.unlock();
        std::cout << std::endl;
        int del;
        std::cout << " marker to delete: "; 
        std::cin >> del;
        marked[del] = true;
        p++;
        ReleaseSemaphore(hmarker_sem[del], 1, 0);//
        marker_sems[del].signal();
        Sleep(1);
        WaitForSingleObject(hmarker_sem, INFINITE);//
        marker_sems[del].wait();
        kn--;
        for (int i = 0; i < n; i++) 
            std::cout << v[i] << " ";
        for (int i = 1; i < k + 1; i++) {
            marker_sems[i].signal();
            ReleaseSemaphore(hmarker_sem, 1, 0);//
        }
        if (p == k) break;
    }
    return 0;
}