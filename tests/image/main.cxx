#include "../tests_common.h"

using namespace compv;

#define TAG_TEST_IMAGE "TestImage"

#define TEST_SCALE					1
#define TEST_CHROMA_CONV			0
#define TEST_FEATURE_FAST			0
#define TEST_DRAW_IMAGE				0
#define TEST_DRAW_POINTS			0

/* Entry point function */
compv_main()
{
	COMPV_ERROR_CODE err = COMPV_ERROR_CODE_S_OK;
	{
		COMPV_CHECK_CODE_BAIL(err = compv_tests_init());
#if TEST_SCALE
		extern COMPV_ERROR_CODE scale();
		COMPV_CHECK_CODE_BAIL(err = scale(), TAG_TEST_IMAGE "Image scaling test failed");
#endif
#if TEST_CHROMA_CONV
		extern COMPV_ERROR_CODE chroma_conv();
		COMPV_CHECK_CODE_BAIL(err = chroma_conv(), TAG_TEST_IMAGE "Chroma conversion test failed");
#endif
#if TEST_FEATURE_FAST
		extern COMPV_ERROR_CODE feature_fast();
		COMPV_CHECK_CODE_BAIL(err = feature_fast(), TAG_TEST_IMAGE "FAST feature detection test failed");
#endif
#if TEST_DRAW_IMAGE
		extern COMPV_ERROR_CODE draw_image();
		COMPV_CHECK_CODE_BAIL(err = draw_image(), TAG_TEST_IMAGE "Draw image test failed");
#endif
#if TEST_DRAW_POINTS
		extern COMPV_ERROR_CODE draw_points();
		COMPV_CHECK_CODE_BAIL(err = draw_points(), TAG_TEST_IMAGE "Draw points test failed");
#endif

	bail:
		COMPV_CHECK_CODE_ASSERT(err, TAG_TEST_IMAGE "Something went wrong!!");
		COMPV_CHECK_CODE_ASSERT(err = compv_tests_deInit());
	}

	COMPV_DEBUG_CHECK_FOR_MEMORY_LEAKS();

	COMPV_DEBUG_INFO_EX(TAG_TEST_IMAGE, "************* Program ended!!! *************");

	compv_main_return(static_cast<int>(err));
}