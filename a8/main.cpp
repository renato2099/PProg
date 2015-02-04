#include <iostream>
#include <atomic>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>

#define THREADS 2
using namespace std;

volatile int arr[THREADS] = {1, 0};
volatile int ind[THREADS] = {0, 0};
//volatile int tail = 0;
std::atomic<int> tail = {0};
std::mutex mtx;

void initialize();
void array_queue_lock(int tid, int n);
void array_queue_unlock(int tid, int n);
void insert_routine (int n, std::stack<int> *pq, int tid);

int main()
{
	//initialize();
	std::stack<int> pq;
	//tail.store(0);
	
	std::thread* tids = new std::thread[THREADS];
	int n = 10;
	for (int i = 0; i < THREADS; i++)
	{
		tids[i] = std::thread(insert_routine, n, &pq, i);
	}
	for (int i = 0; i < THREADS; i++)
	{
		tids[i].join();
	}
	for (int i = 0; i < THREADS*n; i++)
	{
		cout<<pq.top()<<endl;
		pq.pop();
	}
	return 0;
}
void insert_routine (int n, std::stack<int> *pq, int tid) {
	array_queue_lock(tid, n);
	//std::lock_guard<std::mutex> guard(mtx);
	for (int i = 0; i < n; i++) 
	{
		pq->push(i);
	}
	//pq->push(-1);
	array_queue_unlock(tid, n);
}

void array_queue_lock(int tid, int n) {
	ind[tid] = tail.fetch_add(1) % n;
	while(!arr[ind[tid]]);
}

void array_queue_unlock(int tid, int n) {
	arr[ind[tid]] = 0;
	arr[(ind[tid]+1)%n] = 1;
}

void initialize() {
	//arr = new int[THREADS];
	//ind = new int[THREADS];
	arr[0] = 1;
	ind[0] = 0;
	for (int i = 1; i < THREADS; i ++) {
		arr[i] = 0;
		ind[i] = 0;
	}
}
