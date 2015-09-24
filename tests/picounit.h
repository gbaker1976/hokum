#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define pu_assert( message, test ) do { if (!(test)) return message; } while (0)
#define pu_run_test( test ) do { char *message = test(); \
	if (message) return message; } while (0)

extern int tests_run;
