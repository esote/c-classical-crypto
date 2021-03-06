/* caesar-cipher -- rotate strings through the alphabet */

#include <error.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Standard 26-character alphabet */
#define ALPHABET_SIZE 26

/* Standard base-10 numeric system */
#define NUMERIC_SIZE 10

/* Least common multiple of alphabet and numeric size */
#define LCM_ALPHA_NUM 130

/* When to rotate numbers */
static bool rotate_numbers;

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"no-shortcut", no_argument, NULL, 's'},
	{"numbers", no_argument, NULL, 'n'},
	{"rotations", required_argument, NULL, 'r'},

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... [STRING]...\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Rotate strings through the alphabet.");
		printf("Example: %s -n -r 25 \"Hello 123 World!\"\n", name);
		puts("\nOptions:\n\
  -h, --help             display this help text and exit\n\
  -s, --no-shortcut      do not use a shortcut to reduce\n\
                         redundant rotations\n\
  -n, --numbers          rotate numbers alongside letters\n\
  -r, --rotations=NUM    rotate the input string NUM times;\n\
                         defaults to one rotation");
	}

	exit(status);
}

/* Assumes contiguous character encoding from a-z A-Z */
static char rotate_char(char const ch, uintmax_t const rotations)
{
	uintmax_t rot_ch;

	if(ch >= 'a' && ch <= 'z')
		rot_ch = 'a';
	else if(ch >= 'A' && ch <= 'Z')
		rot_ch = 'A';
	else if(rotate_numbers && (ch >= '0' && ch <= '9'))
		rot_ch = '0';
	else
		return ch;

	uintmax_t const mod = rotate_numbers ? NUMERIC_SIZE : ALPHABET_SIZE;

	return (char)(rot_ch + (((uintmax_t)ch - rot_ch + rotations) % mod));
}

static void caesar_cipher(char const *const string, uintmax_t const rotations)
{
	for(size_t i = 0; string[i]; ++i)
		putchar(rotate_char(string[i], rotations));
}

int main(int const argc, char *const *const argv)
{
	rotate_numbers = false;
	bool rotation_shortcut = true;

	/* Rotate once by default */
	uintmax_t rotations = 1;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "hnr:s", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS, argv[0]);
				break;
			case 'n':
				rotate_numbers = true;
				break;
			case 'r':
				rotations = strtoumax(optarg, NULL, 10);
				break;
			case 's':
				rotation_shortcut = false;
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
		}
	}

	if(rotations == UINTMAX_MAX && errno == ERANGE)
		error(EXIT_FAILURE, 0, "could not convert to integer (overflow)");

	if(rotation_shortcut) {
		uintmax_t const mod = rotate_numbers ? LCM_ALPHA_NUM : ALPHABET_SIZE;
		rotations %= mod;
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	for(size_t i = 0; strings_list[i]; ++i) {
		caesar_cipher(strings_list[i], rotations);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}

