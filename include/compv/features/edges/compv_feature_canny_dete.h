/* Copyright (C) 2016-2017 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_FEATURES_CANNY_DETE_H_)
#define _COMPV_FEATURES_CANNY_DETE_H_

#include "compv/compv_config.h"
#include "compv/compv_common.h"
#include "compv/compv_convlt.h"
#include "compv/features/compv_feature.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

class CompVEdgeDeteCanny : public CompVEdgeDete
{
protected:
	CompVEdgeDeteCanny();
public:
	virtual ~CompVEdgeDeteCanny();
	virtual COMPV_INLINE const char* getObjectId() {
		return "CompVEdgeDeteCanny";
	};

	// override CompVSettable::set
	virtual COMPV_ERROR_CODE set(int id, const void* valuePtr, size_t valueSize);
	// override CompVEdgeDete::process
	virtual COMPV_ERROR_CODE process(const CompVPtr<CompVImage*>& image, CompVPtrArray(uint8_t)& edges);

	static COMPV_ERROR_CODE newObj(CompVPtr<CompVEdgeDete* >* dete);

private:
	COMPV_ERROR_CODE direction();
	COMPV_ERROR_CODE nms(CompVPtrArray(uint8_t)& edges);
	COMPV_ERROR_CODE hysteresis(CompVPtrArray(uint8_t)& edges);
	bool connectEdge(CompVPtrArray(uint8_t)& edges, size_t rowIdx, size_t colIdx, size_t& numEdges);

private:
	size_t m_nImageWidth;
	size_t m_nImageHeight;
	size_t m_nImageStride;
	int16_t* m_pGx;
	int16_t* m_pGy;
	uint16_t* m_pG;
	uint8_t* m_pDirs;
	uint8_t* m_pNms;
	CompVPtr<CompVConvlt<float>* > m_gblur;
	CompVPtr<CompVArray<float>* > m_gblurKernFloat;
	CompVPtr<CompVArray<uint16_t>* > m_gblurKernFxp;
};

COMPV_NAMESPACE_END()

#endif /* _COMPV_FEATURES_CANNY_DETE_H_ */
