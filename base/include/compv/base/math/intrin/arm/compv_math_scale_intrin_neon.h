/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_MATH_SCALE_INTRIN_NEON_H_)
#define _COMPV_BASE_MATH_SCALE_INTRIN_NEON_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

#if COMPV_ARCH_ARM && COMPV_INTRINSIC

COMPV_NAMESPACE_BEGIN()

void CompVMathScaleScale_32f32f_Intrin_NEON(COMPV_ALIGNED(NEON) const compv_float32_t* ptrIn, COMPV_ALIGNED(NEON) compv_float32_t* ptrOut, const compv_uscalar_t width, const compv_uscalar_t height, COMPV_ALIGNED(NEON) const compv_uscalar_t stride, const compv_float32_t* s1);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_ARM && COMPV_INTRINSIC */

#endif /* _COMPV_BASE_MATH_SCALE_INTRIN_NEON_H_ */
