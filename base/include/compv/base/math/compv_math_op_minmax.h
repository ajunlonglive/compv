/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_MATH_OP_MINMAX_H_)
#define _COMPV_BASE_MATH_OP_MINMAX_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"
#include "compv/base/compv_mat.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

class CompVMathOpMinMax
{
public:
	static COMPV_ERROR_CODE minMax(const CompVMatPtr &A, double& minn, double& maxx);
	static COMPV_ERROR_CODE minn(const CompVMatPtr &A, double& minn);
};

COMPV_NAMESPACE_END()

#endif /* _COMPV_BASE_MATH_OP_MINMAX_H_ */
