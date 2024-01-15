#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <type_traits>
#include <string>
#include <iostream>
#include <deque>
#include <mutex>

template<typename _t>
class tsque
{
public:
	void clear() {
		std::scoped_lock lock(m_mutex);
		m_queue.clear();
	}

	void push_back(const _t& t) {
		std::scoped_lock lock(m_mutex);
		m_queue.push_back(t);
		m_condition.notify_one();
	}

	void push_front(const _t& t) {
		std::scoped_lock lock(m_mutex);
		m_queue.push_front(t);
		m_condition.notify_one();
	}

	_t pop_back() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [&] { return !m_queue.empty(); });

		_t out = std::move(m_queue.back());
		m_queue.pop_back();
		return out;
	}

	_t pop_front() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [&] { return !m_queue.empty(); });

		_t out = std::move(m_queue.front());
		m_queue.pop_front();
		return out;
	}

	_t peek_back() {
		std::scoped_lock lock(m_mutex);

		if (m_queue.size() == 0) {
			return _t{};
		}

		return m_queue.back();
	}

	_t peek_front() {
		std::scoped_lock lock(m_mutex);

		if (m_queue.size() == 0) {
			return _t{};
		}

		return m_queue.front();
	}
	
	void erase(size_t index) {
		std::scoped_lock lock(m_mutex);
		m_queue.erase(m_queue.begin() + index);
	}

	size_t size() {
		std::scoped_lock lock(m_mutex);
		return m_queue.size();
	}

	bool empty() {
		std::scoped_lock lock(m_mutex);
		return m_queue.empty();
	}

private:
	std::deque<_t> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condition;
};

// JobNode
//	A single piece of work

// JobTree
//	Holds a dependency tree of JobNodes

// Job
//	A wrapper around JobNode/Tree for manipulating the JobTree

// JobExecutor
//	Executes a JobTree

class Job;
class JobTree;

constexpr int JOB_MAX_CONTINUATIONS = 16;

struct JobNode
{
	// Store a pointer to the owning tree so a Job can be passed to 'work' when called
	JobTree* tree = nullptr;

	// When this node is finished, push these to work queue.
	JobNode* continuations[JOB_MAX_CONTINUATIONS] = {}; // could use std::array
	std::atomic<int> continuationCount = 0;

	// Once this is 0, the job is ready to run
	std::atomic<int> dependencies = 0;

	// The work for the job. 
	// todo: use a template to remove need to pass 'Job' to each one
	std::function<void(Job)> work;
	
    std::string name = "";
	int id = 0;

	void AddContinuation(JobNode* node);
	void MoveContinuationsInto(JobNode* into);
};

class Job
{
public:
	Job(JobNode* node, JobTree* tree);

	Job& SetName(const std::string& name);

	// Add a job between this node and its continuations
	// Return the new job
	// 
	// Example:
	//       A -> B -> D
	//         -> C -^
	// 
	//       A.Then(E)
	// 
	//       A -> E -> B -> D
	//              -> C -^
	//
	Job& ThenJob(Job& job);

	// See ThenJob.
	// Create a new job and return it
	template<typename _f>
	Job Then(_f&& func);

	// Add multiple jobs between this node and its continuations
	// Return an empty job that joins the fork
	//
	// Example: 
	//       A -> B
	//
	//       A.Fork({C, D})
	// 
	//       A -> C -> [join] -> B
	//         -> D -^
	//
	template<typename _iterable>
	Job Fork(const _iterable& funcs);

	// See Fork.
	// Add a number of jobs which apply a lambda over each item of a collection
	// Return the join
	template<typename _iterable, typename _f>
	Job For(int batchSize, _iterable& iterable, _f&& perItem);

	// See For. 
	// Split the work over the max number of continuations, or size of iterable
	// Return the join
	template<typename _iterable, typename _f>
	Job For(_iterable& iterable, _f&& perItem);

private:
	JobNode* node;
	JobTree* tree;
};

class JobTree
{
public:
	~JobTree();

	// Return a list of all JobNodes with zero dependencies
	std::vector<JobNode*> GetRoots();

	// Create a Job with no work
	Job CreateEmpty();

	// Create a Job with a callable with signature 'void(Job)'
	template<typename _f>
	Job Create(_f&& work);

	// call this after a tree has been run to free its nodes
	void Cleanup();

	void PrintGraphviz(std::ostream& o);

private:
	JobNode* _alloc_node();
	void _free_node(JobNode* node);

private:
	// this kinda sucks, no need to lock everyone if a concurrent 
	// data structure is used
	std::mutex nodesMutex;
	std::vector<JobNode*> nodes;

	std::atomic<int> jobIdNext;

	// testing for trees that are owned by the executor
public:
	std::atomic<int> nodeCount;
	bool ownedByTree;
};

class JobExecutor
{
public:
	JobExecutor(int numberOfThreads = 4);
	~JobExecutor();

public:
	void Run(JobTree& tree);
	void WaitForAll();

	// Create a tree which the executor will free when it is done
	JobTree& CreateTree();

private:
	struct JobThreadContext
	{
		int index;
	};

	struct JobThread
	{
		std::thread thread;
		JobThreadContext* ctx;
	};

	void ThreadWork(JobThreadContext* ctx);
	void IncWaitCount(int c);

	void CreateThreads(int numberOfThreads);
	void DestroyThreads();

private:
	std::vector<JobTree*> ownedTrees;

	tsque<JobNode*> wavefront;
	std::vector<JobThread> threads;

	std::condition_variable waitVar;
	std::mutex waitMutex;
	int workCount = 0;
};

//
//	Template impl
//

template<typename _f>
Job Job::Then(_f&& func) {
	Job job = tree->Create(func);
	ThenJob(job);

	return job;
}

template<typename _iterable>
Job Job::Fork(const _iterable& funcs) {
	// Create a fake node to join these
	Job join = tree->CreateEmpty();
	node->MoveContinuationsInto(join.node);

	for (auto&& func : funcs) {
		Job job = tree->Create(func);
		job.node->AddContinuation(join.node);

		node->AddContinuation(job.node);
	}

	return join;
}

template<typename _iterable, typename _f>
Job Job::For(int batchSize, _iterable& iterable, _f&& perItem) {
	int size = iterable.size();
	auto begin = iterable.begin();

	if (size == 0 || batchSize <= 0) {// edge case, return this job
		return *this;
	}

	// Create a fake node to join the fork & transfer this jobs continuations
	Job join = tree->CreateEmpty();
	node->MoveContinuationsInto(join.node);

	for (int i = 0; i < size; i += batchSize) {
		int thisBlockSize = std::min(size - i, batchSize);

		auto batch = [=](Job _) {
			auto itr = begin + i;
			auto end = itr + thisBlockSize;

			for (; itr != end; ++itr) {
				perItem(*itr);
			}
		};

		Job job = tree->Create(batch);
		job.node->AddContinuation(join.node);
			
		node->AddContinuation(job.node);
	}

	return join;
}

template<typename _iterable, typename _f>
Job Job::For(_iterable& iterable, _f&& perItem) {
	int batchSize = (iterable.size() + JOB_MAX_CONTINUATIONS) / JOB_MAX_CONTINUATIONS;
	return For(batchSize, iterable, perItem);
}

template<typename _f>
Job JobTree::Create(_f&& work) {
	JobNode* node = _alloc_node();
	node->work = work;
	return Job(node, this);
}
