/* backwards-cipher -- print strings backwards */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "backwards-cipher"

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [OPTION]... [STRING]...\n",
				PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [OPTION]... [STRING]...\n", PROGRAM_NAME);
		puts("Print strings backwards.");
		printf("Example: %s Hello World\n", PROGRAM_NAME);
		puts("\nOptions:\n\
  -h, --help    display this help text and exit");
	}

	exit(status);
}

void backwards_cipher(char const *const string)
{
	for(size_t i = strlen(string);i != 0; --i)
		putchar(string[i - 1]);
}

int main(int const argc, char *const *const argv)
{
	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "h", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			default:
				usage(EXIT_FAILURE);
		}
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	for(size_t i = 0; strings_list[i]; ++i) {
		backwards_cipher(strings_list[i]);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}

