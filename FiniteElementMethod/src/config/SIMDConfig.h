#pragma once

#include "logger/logger.h"

#include <Eigen/Core>

#include <string>
#include <vector>

namespace fem::config
{

class SIMDConfig
{
public:
	static constexpr bool IsVectorizationEnabled()
	{
#ifdef EIGEN_VECTORIZE
		return true;
#else
		return false;
#endif
	}

	static std::vector<std::string> GetEnabledInstructionSets()
	{
		std::vector<std::string> sets;

#ifdef EIGEN_VECTORIZE_SSE
		sets.push_back("SSE");
#endif
#ifdef EIGEN_VECTORIZE_SSE2
		sets.push_back("SSE2");
#endif
#ifdef EIGEN_VECTORIZE_SSE3
		sets.push_back("SSE3");
#endif
#ifdef EIGEN_VECTORIZE_SSSE3
		sets.push_back("SSSE3");
#endif
#ifdef EIGEN_VECTORIZE_SSE4_1
		sets.push_back("SSE4.1");
#endif
#ifdef EIGEN_VECTORIZE_SSE4_2
		sets.push_back("SSE4.2");
#endif
#ifdef EIGEN_VECTORIZE_AVX
		sets.push_back("AVX");
#endif
#ifdef EIGEN_VECTORIZE_AVX2
		sets.push_back("AVX2");
#endif
#ifdef EIGEN_VECTORIZE_AVX512
		sets.push_back("AVX-512");
#endif
#ifdef EIGEN_VECTORIZE_AVX512DQ
		sets.push_back("AVX-512DQ");
#endif
#ifdef EIGEN_VECTORIZE_FMA
		sets.push_back("FMA");
#endif
#ifdef EIGEN_VECTORIZE_NEON
		sets.push_back("NEON");
#endif

		return sets;
	}

	static std::string GetInstructionSetsString()
	{
		auto sets = GetEnabledInstructionSets();
		if (sets.empty())
			return "None";

		std::string result;
		for (size_t i = 0; i < sets.size(); ++i)
		{
			if (i > 0) result += ", ";
			result += sets[i];
		}
		return result;
	}

	static constexpr size_t GetDefaultPacketSize()
	{
		return Eigen::internal::packet_traits<double>::size;
	}

	static constexpr size_t GetDefaultAlignment()
	{
		return EIGEN_MAX_ALIGN_BYTES;
	}

	static std::string GetHighestInstructionSet()
	{
#ifdef EIGEN_VECTORIZE_AVX512
		return "AVX-512";
#elif defined(EIGEN_VECTORIZE_AVX2)
		return "AVX2";
#elif defined(EIGEN_VECTORIZE_AVX)
		return "AVX";
#elif defined(EIGEN_VECTORIZE_SSE4_2)
		return "SSE4.2";
#elif defined(EIGEN_VECTORIZE_SSE4_1)
		return "SSE4.1";
#elif defined(EIGEN_VECTORIZE_SSSE3)
		return "SSSE3";
#elif defined(EIGEN_VECTORIZE_SSE3)
		return "SSE3";
#elif defined(EIGEN_VECTORIZE_SSE2)
		return "SSE2";
#elif defined(EIGEN_VECTORIZE_SSE)
		return "SSE";
#elif defined(EIGEN_VECTORIZE_NEON)
		return "NEON";
#else
		return "Scalar";
#endif
	}

	static void PrintInfo()
	{
		LOG_INFO("SIMD/Vectorization Configuration:");

		if constexpr (IsVectorizationEnabled())
		{
			LOG_INFO("  Vectorization: Enabled");
			LOG_INFO("  Highest instruction set: {}", GetHighestInstructionSet());
			LOG_INFO("  All enabled sets: {}", GetInstructionSetsString());
			LOG_INFO("  Packet size (doubles): {}", GetDefaultPacketSize());
			LOG_INFO("  Memory alignment: {} bytes", GetDefaultAlignment());

#ifdef EIGEN_VECTORIZE_FMA
			LOG_INFO("  FMA (Fused Multiply-Add): Enabled");
#else
			LOG_INFO("  FMA (Fused Multiply-Add): Disabled");
#endif

#ifdef EIGEN_DONT_VECTORIZE
			LOG_WARN("  EIGEN_DONT_VECTORIZE is defined - vectorization may be disabled!");
#endif

#ifdef EIGEN_UNALIGNED_VECTORIZE
			LOG_INFO("  Unaligned vectorization: Enabled");
#else
			LOG_INFO("  Unaligned vectorization: Disabled");
#endif
		}
		else
		{
			LOG_INFO("  Vectorization: Disabled (scalar mode)");
			LOG_INFO("  Packet size: 1 (no SIMD)");
		}
	}

	static void PrintCompactInfo()
	{
		if constexpr (IsVectorizationEnabled())
		{
			LOG_INFO("SIMD: {} (packet size: {}, align: {}B)",
				GetHighestInstructionSet(),
				GetDefaultPacketSize(),
				GetDefaultAlignment());
		}
		else
		{
			LOG_INFO("SIMD: Disabled");
		}
	}

private:
	SIMDConfig() = delete;
};

} // namespace fem::config
