/* tokenize-with-padding -- tokenize strings */

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "tokenize-with-padding"

#define _warn(...) do {					\
		if(!quiet)						\
			error(0, 0, __VA_ARGS__);	\
	} while(0)

/* Disable warnings */
static bool quiet;

static struct option const long_opts[] = {
	{"delim", required_argument, NULL, 'd'},
	{"help", no_argument, NULL, 'h'},
	{"padding", required_argument, NULL, 'p'},
	{"quiet", no_argument, NULL, 'q'},

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... SIZE [STRING]...\n\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Tokenize strings.");
		printf("Example: %s 2 Hello World\n", name);
		puts("\nOptions:\n\
  -d, --delim=STR       delimiting character\n\
  -h, --help            display this help text and exit\n\
  -p, --padding=CHAR    specify padding character\n\
  -q, --quiet           disable warnings");
	}

	exit(status);
}

/* Branchless saturating addition (overflow becomes UINTMAX_MAX) */
static uintmax_t sat_add_uintmax_t(uintmax_t const x, uintmax_t const y)
{
	uintmax_t res = x + y;
	res |= -(res < x);
	return res;
}

static void tokenize_string(char const *const string,
							uintmax_t const token_size,
							char const *const delim)
{
	uintmax_t count = token_size;
	for(size_t i = 0; string[i]; ++i) {
		if((uintmax_t)i == count) {
			printf("%s", delim);
			count += token_size;
		}

		putchar(string[i]);
	}
}

int main(int const argc, char *const *const argv)
{
	quiet = false;

	char const *delim = " ";
	char const *padding = " ";

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "d:hp:q", long_opts, NULL)) != -1) {
		switch(c) {
			case 'd':
				delim = optarg;
				break;
			case 'h':
				usage(EXIT_SUCCESS, argv[0]);
				break;
			case 'p':
				padding = optarg;
				break;
			case 'q':
				quiet = true;
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
		}
	}

	char const *const token_tmp = argv[optind++];

	if(token_tmp == NULL)
		error(EXIT_FAILURE, 0, "token size is missing, try '--help'");

	uintmax_t const token_size = strtoumax(token_tmp, NULL, 10);

	if(token_size == UINTMAX_MAX && errno == ERANGE)
		error(EXIT_FAILURE, 0, "cannot convert string to integer (overflow)");
	else if(token_size == 0)
		error(EXIT_FAILURE, 0, "token size must be greater than zero");

	if(strlen(padding) > 1)
		_warn("padding only uses the first character specified");

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	uintmax_t fstring_len = 0;

	for(size_t i = 0; strings_list[i]; ++i)
		fstring_len = sat_add_uintmax_t(fstring_len, strlen(strings_list[i]));

	while(fstring_len % token_size != 0)
		fstring_len++;

	char *fstring = malloc(fstring_len + 1);

	if(fstring == NULL)
		error(EXIT_FAILURE, 0, "error allocating memory for string.\n");

	if(strings_list[0] != NULL)
		strcpy(fstring, strings_list[0]);

	for(size_t i = 1; strings_list[i]; ++i)
		strcat(fstring, strings_list[i]);

	/* Padding */
	while(strlen(fstring) % token_size != 0)
		strncat(fstring, padding, 1);

	tokenize_string(fstring, token_size, delim);

	free(fstring);

	putchar('\n');

	return EXIT_SUCCESS;
}
