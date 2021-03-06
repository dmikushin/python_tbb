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
#include <tbb/global_control.h>
#include <vector>

PYBIND11_MAKE_OPAQUE(std::vector<int>);

using namespace std;

vector<int> backend_tbb(int length, std::function<void (int, int, pybind11::object&)>& callback)
{
	int nthreads = tbb::task_scheduler_init::default_num_threads();
	const char* cnthreads = getenv("TBB_NUM_THREADS");
	if (cnthreads) nthreads = std::max(1, atoi(cnthreads));
	printf("Using %d threads\n", nthreads);

	tbb::task_group group;
	tbb::task_arena arena(nthreads, 1);

	tbb::global_control control(
		tbb::global_control::max_allowed_parallelism, nthreads);
        
	vector<int> result(length);

	arena.execute( [&] { 
		group.run( [&] {
			tbb::parallel_for(tbb::blocked_range<int>(0, result.size()),
				[&](const tbb::blocked_range<int>& range)
			{
				pybind11::object py_result = pybind11::cast(&result);
				for (int i = range.begin(); i != range.end(); i++)
					callback(i, i, py_result);
			});
		});
	});

	arena.execute( [&] { group.wait(); });
	
	return result;	
}

vector<int> backend_serial(int length, std::function<void (int, int, pybind11::object&)>& callback)
{
	vector<int> result(length);

	pybind11::object py_result = pybind11::cast(&result);
	for (int i = 0; i < result.size(); i++)
		callback(i, i, py_result);
	
	return result;
}

PYBIND11_MODULE(python_tbb, m)
{
	pybind11::bind_vector<std::vector<int> >(m, "stdvectorint");

	m.def("backend_tbb", &backend_tbb, "TBB backend");
	m.def("backend_serial", &backend_serial, "Serial backend");
}

