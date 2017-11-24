/* Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/intrin/x86/compv_math_convlt_intrin_sse2.h"

#if COMPV_ARCH_X86 && COMPV_INTRINSIC
#include "compv/base/intrin/x86/compv_intrin_sse.h"
#include "compv/base/compv_debug.h"

COMPV_NAMESPACE_BEGIN()

void CompVMathConvlt1VtHz_8u32f8u_Intrin_SSE2(const uint8_t* inPtr, uint8_t* outPtr, compv_uscalar_t width, compv_uscalar_t height, compv_uscalar_t step, compv_uscalar_t pad, const compv_float32_t* vthzKernPtr, compv_uscalar_t kernSize)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	compv_uscalar_t i, j, k, row;
	const compv_uscalar_t stride = (width + pad);
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width4 = width & -4;
	__m128i vecInPtr, vec0i, vec1i, vec2i, vec3i;
	__m128 vecCoeff, vecSum0, vecSum1, vecSum2, vecSum3, vec0f, vec1f, vec2f, vec3f;
	const __m128i vecZero = _mm_setzero_si128();
	uint32_t dword;

	for (j = 0; j < height; ++j) {
		/* Per #16 samples */
		for (i = 0; i < width16; i += 16) {
			vecSum0 = _mm_setzero_ps();
			vecSum1 = _mm_setzero_ps();
			vecSum2 = _mm_setzero_ps();
			vecSum3 = _mm_setzero_ps();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_ps(vthzKernPtr[row]);
				vec2i = _mm_unpacklo_epi8(vecInPtr, vecZero); // epu8 -> epu16
				vec3i = _mm_unpackhi_epi8(vecInPtr, vecZero); // epu8 -> epu16
				vec0i = _mm_unpacklo_epi16(vec2i, vecZero); // epu16 -> epu32
				vec1i = _mm_unpackhi_epi16(vec2i, vecZero); // epu16 -> epu32
				vec2i = _mm_unpacklo_epi16(vec3i, vecZero); // epu16 -> epu32
				vec3i = _mm_unpackhi_epi16(vec3i, vecZero); // epu16 -> epu32
				vec0f = _mm_cvtepi32_ps(vec0i);
				vec1f = _mm_cvtepi32_ps(vec1i);
				vec2f = _mm_cvtepi32_ps(vec2i);
				vec3f = _mm_cvtepi32_ps(vec3i);
				vec0f = _mm_mul_ps(vec0f, vecCoeff);
				vec1f = _mm_mul_ps(vec1f, vecCoeff);
				vec2f = _mm_mul_ps(vec2f, vecCoeff);
				vec3f = _mm_mul_ps(vec3f, vecCoeff);
				vecSum0 = _mm_add_ps(vecSum0, vec0f);
				vecSum1 = _mm_add_ps(vecSum1, vec1f);
				vecSum2 = _mm_add_ps(vecSum2, vec2f);
				vecSum3 = _mm_add_ps(vecSum3, vec3f);
			}
			vec0i = _mm_cvttps_epi32(vecSum0);
			vec1i = _mm_cvttps_epi32(vecSum1);
			vec2i = _mm_cvttps_epi32(vecSum2);
			vec3i = _mm_cvttps_epi32(vecSum3);
			vec0i = _mm_packs_epi32(vec0i, vec1i); // _mm_packus_epi32 is SSE4.1 -> anyway, not needed
			vec2i = _mm_packs_epi32(vec2i, vec3i);
			vec0i = _mm_packus_epi16(vec0i, vec2i);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i]), vec0i);
		}

		/* Per #4 samples */
		for (; i < width4; i += 4) {
			vecSum0 = _mm_setzero_ps();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_ps(vthzKernPtr[row]);
				vec0f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_unpacklo_epi8(vecInPtr, vecZero), vecZero));
				vec0f = _mm_mul_ps(vec0f, vecCoeff);
				vecSum0 = _mm_add_ps(vecSum0, vec0f);
			}
			vec0i = _mm_cvttps_epi32(vecSum0);
			vec0i = _mm_packs_epi32(vec0i, vec0i);
			vec0i = _mm_packus_epi16(vec0i, vec0i);
			*reinterpret_cast<uint32_t*>(&outPtr[i]) = static_cast<uint32_t>(_mm_cvtsi128_si32(vec0i));
		}

		/* Per #1 samples */
		if (i < width) {
			vecSum0 = _mm_setzero_ps();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_ps(vthzKernPtr[row]);
				vec0f = _mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_unpacklo_epi8(vecInPtr, vecZero), vecZero));
				vec0f = _mm_mul_ps(vec0f, vecCoeff);
				vecSum0 = _mm_add_ps(vecSum0, vec0f);
			}
			vec0i = _mm_cvttps_epi32(vecSum0);
			vec0i = _mm_packs_epi32(vec0i, vec0i);
			vec0i = _mm_packus_epi16(vec0i, vec0i);
			dword = _mm_cvtsi128_si32(vec0i);
			for (; i < width; ++i) {
				outPtr[i] = (dword & 0xff);
				dword >>= 8;
			}
		}

		inPtr += stride;
		outPtr += stride;
	}
}

void CompVMathConvlt1VtHz_8u16s16s_Intrin_SSE2(const uint8_t* inPtr, int16_t* outPtr, compv_uscalar_t width, compv_uscalar_t height, compv_uscalar_t step, compv_uscalar_t pad, const int16_t* vthzKernPtr, compv_uscalar_t kernSize)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	compv_uscalar_t i, j, k, row;
	const compv_uscalar_t stride = width + pad;
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width4 = width & -4;
	__m128i vecInPtr, vec0, vec1, vec2, vec3, vecSum0, vecSum1, vecSum2, vecSum3, vecCoeff;
	const __m128i vecZero = _mm_setzero_si128();
	COMPV_ALIGN_SSE() int16_t mem[4];

	// Using int32_t as accumulator to avoid overflow

	for (j = 0; j < height; ++j) {
		/* Per #16 samples */
		for (i = 0; i < width16; i += 16) {
			vecSum0 = _mm_setzero_si128();
			vecSum1 = _mm_setzero_si128();
			vecSum2 = _mm_setzero_si128();
			vecSum3 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec2 = _mm_unpacklo_epi8(vecInPtr, vecZero); // epu8 -> epu16
				vec3 = _mm_unpackhi_epi8(vecInPtr, vecZero); // epu8 -> epu16
				vec0 = _mm_unpacklo_epi16(vec2, vecZero); // epu16 -> epu32
				vec1 = _mm_unpackhi_epi16(vec2, vecZero); // epu16 -> epu32
				vec2 = _mm_unpacklo_epi16(vec3, vecZero); // epu16 -> epu32
				vec3 = _mm_unpackhi_epi16(vec3, vecZero); // epu16 -> epu32
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff); // _mm_mullo_epi32 is SSE4.1 -> asm code will be SSE41
				vec1 = _mm_mullo_epi32_SSE2(vec1, vecCoeff);
				vec2 = _mm_mullo_epi32_SSE2(vec2, vecCoeff);
				vec3 = _mm_mullo_epi32_SSE2(vec3, vecCoeff);
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
				vecSum1 = _mm_add_epi32(vecSum1, vec1);
				vecSum2 = _mm_add_epi32(vecSum2, vec2);
				vecSum3 = _mm_add_epi32(vecSum3, vec3);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum1);
			vecSum2 = _mm_packs_epi32(vecSum2, vecSum3);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i]), vecSum0);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i + 8]), vecSum2);
		}
		/* Per #4 samples */
		for (; i < width4; i += 4) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vec0 = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec0 = _mm_unpacklo_epi8(vec0, vecZero); // epu8 -> epu16
				vec0 = _mm_unpacklo_epi16(vec0, vecZero); // epu16 -> epu32
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff); // _mm_mullo_epi32 is SSE4.1 -> asm code will be SSE41
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum0);
			_mm_storel_epi64(reinterpret_cast<__m128i*>(&outPtr[i]), vecSum0);
		}
		
		/* Per #1 samples */
		if (i < width) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vec0 = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec0 = _mm_unpacklo_epi8(vec0, vecZero); // epu8 -> epu16
				vec0 = _mm_unpacklo_epi16(vec0, vecZero); // epu16 -> epu32
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff); // _mm_mullo_epi32 is SSE4.1 -> asm code will be SSE41
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum0);
			_mm_storel_epi64(reinterpret_cast<__m128i*>(mem), vecSum0);
			for (k = 0; i < width; ++i, ++k) {
				outPtr[i] = mem[k];
			}
		}

		inPtr += stride;
		outPtr += stride;
	}
}

void CompVMathConvlt1VtHz_16s16s16s_Intrin_SSE2(const int16_t* inPtr, int16_t* outPtr, compv_uscalar_t width, compv_uscalar_t height, compv_uscalar_t step, compv_uscalar_t pad, const int16_t* vthzKernPtr, compv_uscalar_t kernSize)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	compv_uscalar_t i, j, k, row;
	const compv_uscalar_t stride = width + pad;
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width4 = width & -4;
	__m128i vec0, vec1, vec2, vec3, vecSum0, vecSum1, vecSum2, vecSum3, vecCoeff;
	COMPV_ALIGN_SSE() int16_t mem[4];

	// Using int32_t as accumulator to avoid overflow

	for (j = 0; j < height; ++j) {
		/* Per #16 samples */
		for (i = 0; i < width16; i += 16) {
			vecSum0 = _mm_setzero_si128();
			vecSum1 = _mm_setzero_si128();
			vecSum2 = _mm_setzero_si128();
			vecSum3 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vec2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vec3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&inPtr[i + k + 8]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec0 = _mm_cvtepi16_epi32_low_SSE2(vec2); // epi16 -> epi32: _mm_cvtepi16_epi32 is SSE41 -> asm code will be SSE41
				vec1 = _mm_cvtepi16_epi32_hi_SSE2(vec2); // epi16 -> epi32
				vec2 = _mm_cvtepi16_epi32_low_SSE2(vec3); // epi16 -> epi32
				vec3 = _mm_cvtepi16_epi32_hi_SSE2(vec3); // epi16 -> epi32
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff); // _mm_mullo_epi32 is SSE4.1 -> asm code will be SSE41
				vec1 = _mm_mullo_epi32_SSE2(vec1, vecCoeff);
				vec2 = _mm_mullo_epi32_SSE2(vec2, vecCoeff);
				vec3 = _mm_mullo_epi32_SSE2(vec3, vecCoeff);
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
				vecSum1 = _mm_add_epi32(vecSum1, vec1);
				vecSum2 = _mm_add_epi32(vecSum2, vec2);
				vecSum3 = _mm_add_epi32(vecSum3, vec3);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum1);
			vecSum2 = _mm_packs_epi32(vecSum2, vecSum3);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i]), vecSum0);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i + 8]), vecSum2);
		}
		/* Per #4 samples */
		for (; i < width4; i += 4) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vec0 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec0 = _mm_cvtepi16_epi32_low_SSE2(vec0);
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff);
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum0);
			_mm_storel_epi64(reinterpret_cast<__m128i*>(&outPtr[i]), vecSum0);
		}

		/* Per #1 samples */
		if (i < width) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vec0 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi32(static_cast<int>(vthzKernPtr[row]));
				vec0 = _mm_cvtepi16_epi32_low_SSE2(vec0);
				vec0 = _mm_mullo_epi32_SSE2(vec0, vecCoeff);
				vecSum0 = _mm_add_epi32(vecSum0, vec0);
			}
			vecSum0 = _mm_packs_epi32(vecSum0, vecSum0);
			_mm_storel_epi64(reinterpret_cast<__m128i*>(mem), vecSum0);
			for (k = 0; i < width; ++i, ++k) {
				outPtr[i] = mem[k];
			}
		}

		inPtr += stride;
		outPtr += stride;
	}
}

void CompVMathConvlt1VtHzFixedPoint_8u16u8u_Intrin_SSE2(const uint8_t* inPtr, uint8_t* outPtr, compv_uscalar_t width, compv_uscalar_t height, compv_uscalar_t step, compv_uscalar_t pad, const uint16_t* vthzKernPtr, compv_uscalar_t kernSize)
{
	compv_uscalar_t i, j, k, row;
	const compv_uscalar_t stride = (width + pad);
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width4 = width & -4;
	__m128i vecInPtr, vec0, vec1, vecSum0, vecSum1, vecCoeff;
	const __m128i vecZero = _mm_setzero_si128();
	uint32_t dword;

	for (j = 0; j < height; ++j) {
		/* Per #16 samples */
		for (i = 0; i < width16; i += 16) {
			vecSum0 = _mm_setzero_si128();
			vecSum1 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi16(static_cast<short>(vthzKernPtr[row]));
				vec0 = _mm_unpacklo_epi8(vecInPtr, vecZero); // epu8 -> epi16
				vec1 = _mm_unpackhi_epi8(vecInPtr, vecZero); // epu8 -> epi16
				vecSum0 = _mm_adds_epu16(vecSum0, _mm_mulhi_epu16(vec0, vecCoeff));
				vecSum1 = _mm_adds_epu16(vecSum1, _mm_mulhi_epu16(vec1, vecCoeff));
			}
			vec0 = _mm_packus_epi16(vecSum0, vecSum1);
			_mm_storeu_si128(reinterpret_cast<__m128i*>(&outPtr[i]), vec0);
		}
		/* Per #4 samples */
		for (; i < width4; i += 4) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi16(static_cast<short>(vthzKernPtr[row]));
				vec0 = _mm_unpacklo_epi8(vecInPtr, vecZero); // epu8 -> epi16
				vecSum0 = _mm_adds_epu16(vecSum0, _mm_mulhi_epu16(vec0, vecCoeff));
			}
			vec0 = _mm_packus_epi16(vecSum0, vecSum0);
			*reinterpret_cast<uint32_t*>(&outPtr[i]) = _mm_cvtsi128_si32(vec0);
		}

		/* Per #1 samples */
		if (i < width) {
			vecSum0 = _mm_setzero_si128();
			for (row = 0, k = 0; row < kernSize; ++row, k += step) {
				vecInPtr = _mm_cvtsi32_si128(*reinterpret_cast<const uint32_t*>(&inPtr[i + k]));
				vecCoeff = _mm_set1_epi16(static_cast<short>(vthzKernPtr[row]));
				vec0 = _mm_unpacklo_epi8(vecInPtr, vecZero); // epu8 -> epi16
				vecSum0 = _mm_adds_epu16(vecSum0, _mm_mulhi_epu16(vec0, vecCoeff));
			}
			vec0 = _mm_packus_epi16(vecSum0, vecSum0);
			dword = _mm_cvtsi128_si32(vec0);
			for (; i < width; ++i) {
				outPtr[i] = (dword & 0xff);
				dword >>= 8;
			}
		}

		inPtr += stride;
		outPtr += stride;
	}
}

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */