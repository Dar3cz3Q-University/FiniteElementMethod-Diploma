#pragma once

namespace fem::config
{

extern "C" {
	void openblas_set_num_threads(int num_threads);
}

class OpenBLASConfig
{
public:
	static void SetNumThreads(size_t nThreads)
	{
		openblas_set_num_threads(nThreads);
	}
};

}
