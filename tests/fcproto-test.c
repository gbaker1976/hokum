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

	return 0;
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
	struct sockaddr *restrict address, socklen_t *restrict address_len) {

	return 0;
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
 *
 *
 */
static char *
test_send_cmd() {
	int n;
	int sockfd;
	uuid_t uuid;
	char *payload = "test";
	struct sockaddr *addr;

	uuid_generate( uuid );

	n = send_cmd( CMD_REG, sockfd, addr, uuid, payload, sendto );

	pu_assert( "FAIL: send_cmd number of packets not correct number. Expected 1.", n == 1 );

	return 0;
}


/*
 *
 *	Runs all tests.
 *
 */
static char *
all_tests() {
	pu_run_test(test_build_packets);
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

	if (result != 0) {
		printf( "%s\n", result );
	}
	else {
		printf( "ALL TESTS PASSED\n" );
	}

	return result != 0;
}
