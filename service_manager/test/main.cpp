#include "stdafx.h"

int main(int32 argc, cqCHAR** argv)
{
	int result;

	//chdir("test_data");

	cq_setLogLevel(MapbarLogLevel_critical);

	Stdlib_init();

	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();

	Stdlib_cleanup();

	return result;
}
