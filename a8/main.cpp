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
std::atomic<int> tail = {0};
std::mutex mtx;

struct qnode{
	struct qnode *next;
	int state = 0;
};

std::atomic<qnode*> mcs_tail = {NULL};

void initialize();
void array_queue_lock(int tid, int n);
void array_queue_unlock(int tid, int n);
void mcs_lock(int tid, qnode * n);
void mcs_unlock(int tid, qnode * n);
void insert_routine (int n, std::stack<int> *pq, int tid, int typ);
qnode* fetch_and_set(std::atomic<qnode*>* old, qnode* neww);

int main()
{
	//initialize();
	std::stack<int> pq;
	//tail.store(0);
	
	std::thread* tids = new std::thread[THREADS];
	int n = 10;
	// array_queue_lock
	/*
	for (int i = 0; i < THREADS; i++)
	{
		tids[i] = std::thread(insert_routine, n, &pq, i, 0);
	}
	*/
	// mcs_lock
	for (int i = 0; i < THREADS; i++)
	{
		tids[i] = std::thread(insert_routine, n, &pq, i, 1);
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

void insert_routine (int n, std::stack<int> *pq, int tid, int typ) {
	//std::lock_guard<std::mutex> guard(mtx);
	qnode *inner_node = new qnode;
	typ? array_queue_lock(tid, n): mcs_lock(tid, inner_node);
	for (int i = 0; i < n; i++) 
	{
		pq->push(i);
	}
	typ? array_queue_unlock(tid, n): mcs_unlock(tid, inner_node);
}

void mcs_lock(int tid, qnode * n) {
	n->next = NULL;

	qnode* prev = fetch_and_set(&mcs_tail, n);

	if (prev != NULL) {
		n->state = 1;
		prev->next = n;
		while(n->state);
	}
}

void mcs_unlock(int tid, qnode * n) {
	if (n->next == NULL) {
		if (mcs_tail.compare_exchange_weak(n, NULL))
			return;
		while(n->next == NULL);
	}
	n->next->state = 0;
	n->next = NULL;
}

qnode* fetch_and_set(std::atomic<qnode*>* old, qnode* neww) {
	while(true) {
		qnode* new_old = *old;
		if ((*old).compare_exchange_weak(new_old, neww))
			return new_old;
	}
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
