#include "lith/job.h"

int inc_if_not_greator_than(std::atomic<int>& x, int max) {
	int idx = x.load();
	
	while (idx < 16 && !x.compare_exchange_weak(idx, idx + 1)) 
	{}

	return idx;
}

void JobNode::AddContinuation(JobNode* node) {
	if (this == node) {
        return;
	}

	int idx = inc_if_not_greator_than(continuationCount, JOB_MAX_CONTINUATIONS);

	if (idx >= JOB_MAX_CONTINUATIONS) {
		throw nullptr;
		return;
	}

    continuations[idx] = node;
	node->dependencies += 1;
}

void JobNode::MoveContinuationsInto(JobNode* into) {
	int size = continuationCount;

	into->continuationCount = size;
	continuationCount = 0;

	for (int i = 0; i < size; i++) {
		into->continuations[i] = continuations[i];
		continuations[i] = nullptr;
	}

	for (int i = size; i < 16 - size; i++) {
		into->continuations[i] = nullptr;
	}
}

Job::Job(JobNode* node, JobTree* tree)
	: node (node)
	, tree (tree)
{}

Job& Job::SetName(const std::string & name) {
	node->name = name;
	return *this;
}

Job& Job::ThenJob(Job& job) {
	node->MoveContinuationsInto(job.node);
	node->AddContinuation(job.node);

	return job;
}

JobTree::~JobTree() {
	Cleanup();
}

std::vector<JobNode*> JobTree::GetRoots() {
	std::vector<JobNode*> roots;

	for (JobNode* node : nodes) {
        if (node->dependencies == 0) {
            roots.push_back(node);
		}
	}
        
	return roots;
}

void JobTree::Cleanup() {
	for (JobNode* node : nodes) {
		_free_node(node);
	}

	nodes.clear();
}

void JobTree::PrintGraphviz(std::ostream& o) {
    o << "digraph G {\n";
        
	for (JobNode* node : nodes) {
		int count = node->continuationCount;
		for (int i = 0; i < count; i++) {
			JobNode* cont = node->continuations[i];
            o << "\t\"" << node->name << "\"" << "->" << "\"" << cont->name << "\"" << "\n";
		}
	}
        
    o << "}\n";
}

Job JobTree::CreateEmpty() {
	return Job(_alloc_node(), this);
}

JobNode* JobTree::_alloc_node() {
	JobNode* node = new JobNode();
	node->tree = this;
	node->id = jobIdNext.fetch_add(1);

	{
		std::scoped_lock lock(nodesMutex);
		nodes.push_back(node);
	}

	nodeCount += 1;

	return node;
}

void JobTree::_free_node(JobNode* node) {
	delete node;
}

JobExecutor::JobExecutor(int numberOfThreads) {
	CreateThreads(numberOfThreads);
}

JobExecutor::~JobExecutor() {
	DestroyThreads();
}

void JobExecutor::Run(JobTree& tree) {
	for (JobNode* node : tree.GetRoots()) {
		IncWaitCount(1);
		wavefront.push_front(node);
	}
}

void JobExecutor::WaitForAll() {
	std::unique_lock lock(waitMutex);
	waitVar.wait(lock, [this]() { return workCount == 0; });
}

JobTree& JobExecutor::CreateTree() {
	JobTree* tree = new JobTree();
	ownedTrees.push_back(tree);
	return *tree;
}

void JobExecutor::ThreadWork(JobThreadContext* ctx) {
	while (true) {
		JobNode* node = wavefront.pop_back();

		if (!node) { // stop the thread if nullptr is pushed
			break;
		}

		//if (node->dependencies > 0) // this never gets tripped 
		//{							  // because only nodes with 0 dependencies get queued. Could remove
		//	wavefront.push_front(node);
		//	continue;
		//}
		
		if (node->work) {
			node->work(Job(node, node->tree));
		}

		int count = node->continuationCount.load(); // jobs can only add to themselves, so no need to lock
        for (int i = 0; i < count; i++) {
			JobNode* child = node->continuations[i];

            child->dependencies -= 1;
			if (child->dependencies == 0) {
				IncWaitCount(1);
				wavefront.push_front(child);
			}
        }

		IncWaitCount(-1);
		waitVar.notify_one();

		node->tree->nodeCount -= 1;

		if (node->tree->nodeCount == 0) {
			ownedTrees.erase(std::find(ownedTrees.begin(), ownedTrees.end(), node->tree));
			delete node->tree;
		}
	}
}

void JobExecutor::IncWaitCount(int c) {
	std::unique_lock lock(waitMutex);
	workCount += c;
}

void JobExecutor::CreateThreads(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		JobThreadContext* ctx = new JobThreadContext();
		ctx->index = i;

		JobThread thread = {
			std::thread([this, ctx]() { ThreadWork(ctx); }),
			ctx
		};

		threads.emplace_back(std::move(thread));
	}
}

void JobExecutor::DestroyThreads() {
	for (int i = 0; i < (int)threads.size(); i++) {
		wavefront.push_front(nullptr);
	}

	for (JobThread& th : threads) {
		if (th.thread.joinable()) {
			th.thread.join();
		}
	}

	threads.clear();
}