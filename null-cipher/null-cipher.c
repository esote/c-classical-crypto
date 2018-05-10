/* null-cipher -- create a ciphertext from positions of letters in a string */

#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "null-cipher"

#define _warn(...) do {					\
		if(!quiet)						\
			error(0, 0, __VA_ARGS__);	\
	} while(0)

#define STRTOUMAX_BASE 10

/* Disable warnings */
static bool quiet;

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"index", required_argument, NULL, 'i'},
	{"quiet", no_argument, NULL, 'q'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [KEY] [OPTION]... [POSITION]...\n",
				PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [KEY] [OPTION]... [POSITION]...\n", PROGRAM_NAME);
		puts("Create a ciphertext from positions of letters in a string.");
		printf("Example: %s \"Hello World\" 1 2\n", PROGRAM_NAME);
		puts("\nOptions:\n\
  -h, --help         display this help text and exit\n\
  -i, --index=NUM    begin indexing at NUM (default: 0)\n\
  -q, --quiet        disable warnings");
	}

	exit(status);
}

void null_cipher(char const *const string, char const *const key,
				 uintmax_t const index)
{
	uintmax_t const place = strtoumax(key, NULL, STRTOUMAX_BASE);

	if(strlen(string) <= index + place) {
		_warn("index in string not found");
		return;
	}

	printf("%c ", string[index + place]);
}

int main(int const argc, char *const *const argv)
{
	quiet = false;

	uintmax_t index = 0;

	char const *const key = (argc > 1) ? argv[1] : NULL;

	if(key != NULL && (strcmp(key, "-h") == 0 || strcmp(key, "--help") == 0)) {
		usage(EXIT_SUCCESS);
	}

	if(key == NULL)
		error(EXIT_FAILURE, 0, "first argument must be the key");
	
	/* First argument is positional */
	optind++;

	/* Used for tokenizing key */
	char const *token;
	char const *const delim = " ,.\t\n";

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "hiq", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			case 'i':
				index = strtoumax(optarg, NULL, STRTOUMAX_BASE);
				break;
			case 'q':
				quiet = true;
				break;
			default:
				usage(EXIT_FAILURE);
		}
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	token = strtok((char *const)key, delim);

	for(size_t i = 0; strings_list[i] && token != NULL; ++i) {
		null_cipher(token, strings_list[i], index);
		token = strtok(NULL, delim);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}
