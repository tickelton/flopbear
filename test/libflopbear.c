/*
 * Copyright (c) 2018 tick <tickelton@gmail.com>
 *
 * SPDX-License-Identifier:	ISC
 */

#include "unity.h"
#include "flopbear.h"

void test_mac_ntop_valid_address(void)
{
	char dst[18];
	uint8_t src[6];

	memset(dst, 0, sizeof(dst));
	src[0] = 0x00;
	src[1] = 0xDE;
	src[2] = 0xAD;
	src[3] = 0xBE;
	src[4] = 0xEF;
	src[5] = 0x01;

	mac_ntop((char *)src, dst, sizeof(dst));
	TEST_ASSERT_EQUAL_STRING("00:DE:AD:BE:EF:01", dst);
}


int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_mac_ntop_valid_address);

	return UNITY_END();
}

