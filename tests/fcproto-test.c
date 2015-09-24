#include <string.h>
#include <uuid/uuid.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include "picounit.h"
#include "../fcproto.h"

///// mocks
ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
	const struct sockaddr *dest_addr, socklen_t dest_len) {

	return (ssize_t)length;
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
	struct sockaddr *restrict address, socklen_t *restrict address_len) {

	return (ssize_t)length;
}
// end mocks


/*
 *
 *	Tests build_packets function.
 *
 */
static char *
test_build_packets() {
	char *payload = "test";
	struct fcproto_pkt pkt_arr[1];
	int n;
	uuid_t uuid;

	uuid_generate( uuid );

	n = build_packets( CMD_REG, payload, pkt_arr, uuid );

	pu_assert( "FAIL: build_packets not correct number. Expected 1.", n == 1 );

	return 0;
}


/**
 * Tests semd_cmd function.
 *
 */
static char *
test_send_cmd() {
	int n;
	uuid_t uuid;
	char *payload = "test";
	struct sockaddr *addr;

	uuid_generate( uuid );

	n = send_cmd( CMD_REG, addr, uuid, payload, sendto );

	pu_assert( "FAIL: send_cmd number of packets not correct number. Expected 1.", n == 1 );

	return 0;
}

/*
 *
 *
 */
// static char *
// test_wait_recv() {
// 	 cmd_kind type;
// 	 uuid_t uuid;
// 	 f_cmd_cb cb
// }


/*
 *
 *	Runs all tests.
 *
 */
static char *
all_tests() {
	pu_run_test( test_build_packets );
	pu_run_test( test_send_cmd );

	return 0;
}


/*
 *
 *	Program entrypoint.
 *
 */
int
main(int argc, char **argv) {
	char *result = all_tests();

	printf( ANSI_COLOR_BLUE "\n*** RUNNING TESTS***\n" ANSI_COLOR_RESET );

	if (result != 0) {
		printf( ANSI_COLOR_RED "%s\n" ANSI_COLOR_RESET, result );
	} else {
		printf( ANSI_COLOR_GREEN "\nALL TESTS PASSED\n" ANSI_COLOR_RESET );
	}

	return result != 0;
}
