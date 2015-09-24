/* file: minunit.h */
 #define pu_assert( message, test ) do { if (!(test)) return message; } while (0)
 #define pu_run_test( test ) do { char *message = test(); \
	 if (message) return message; } while (0)

 extern int tests_run;
