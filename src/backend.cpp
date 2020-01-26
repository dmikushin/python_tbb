#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <vector>

PYBIND11_MAKE_OPAQUE(std::vector<int>);

using namespace std;

void backend_tbb(vector<int>& result, std::function<int (int)>& callback)
{
	int nthreads = tbb::task_scheduler_init::default_num_threads();
	const char* cnthreads = getenv("TBB_NUM_THREADS");
	if (cnthreads) nthreads = std::max(1, atoi(cnthreads));

	tbb::task_group group;
	tbb::task_arena arena(nthreads, 1);
	tbb::task_scheduler_init init(nthreads);

	arena.execute( [&] { 
		group.run( [&] {
			tbb::parallel_for(tbb::blocked_range<int>(0, result.size()),
				[&](const tbb::blocked_range<int>& range)
			{
				for (int i = range.begin(); i != range.end(); i++)
					result[i] = callback(i);
			});
		});
	});

	arena.execute( [&] { group.wait(); });		
}

void backend_serial(vector<int>& result, std::function<int (int)>& callback)
{
	for (int i = 0; i < result.size(); i++)
		result[i] = callback(i);
}

PYBIND11_MODULE(python_tbb, m)
{
	pybind11::bind_vector<std::vector<int> >(m, "stdvectorint");

	m.def("backend_tbb", &backend_tbb, "TBB backend");
	m.def("backend_serial", &backend_serial, "Serial backend");
}

