/* backwards-cipher -- print strings backwards */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... [STRING]...\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Print strings backwards.");
		printf("Example: %s Hello World\n", name);
		puts("\nOptions:\n\
  -h, --help    display this help text and exit");
	}

	exit(status);
}

void backwards_cipher(char const *const string)
{
	for(size_t i = strlen(string); i != 0; --i)
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
				usage(EXIT_SUCCESS, argv[0]);
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
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

