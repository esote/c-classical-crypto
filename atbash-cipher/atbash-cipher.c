/* atbash-cipher -- monoalphabetic substitution cipher */

#include <ctype.h>
#include <error.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "atbash-cipher"

/* Standard 26-character alphabet */
#define ALPHABET_SIZE 26

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"print", no_argument, NULL, 'p'},
	{"unique", no_argument, NULL, 'u'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [KEY] [OPTION]... [STRING]...\n",
				PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [KEY] [OPTION]... [STRING]...\n", PROGRAM_NAME);
		puts("Monoalphabetic substitution cipher.");
		printf("Example: %s bcdefghijklMnopqrstuvwxyza -u -p Hello\n",
			   PROGRAM_NAME);
		puts("\nOptions:\n\
  -h, --help      display this help text and exit\n\
  -p, --print     print the key and normal alphabet for comparison\n\
  -u, --unique    check key for alphabetic uniqueness");
	}

	exit(status);
}

bool strunique(char const *c)
{
	bool arr[CHAR_MAX - CHAR_MIN] = {false};

	while(*c) {
		if(arr[(unsigned char)*c])
			return false;
		else
			arr[(unsigned char)*c++] = true;
	}

	return true;
}

bool stralpha(char const *c)
{
	while(*c) {
		if(!isalpha(*c++))
			return false;
	}

	return true;
}

char const *strtolower(char const *const c)
{
	if(c == NULL)
		return NULL;

	unsigned char *p = (unsigned char *)c;

	while(*p) {
		*p = (unsigned char)tolower((unsigned char)*p);
		p++;
	}

	return c;
}

/* Assumes contiguous character encoding from a-z A-Z */
char exchange_char(char const ch, char const *const key)
{
	if(ch >= 'a' && ch <= 'z')
		return key[ch - 'a'];
	else if(ch >= 'A' && ch <= 'Z')
		return (char)toupper(key[ch - 'A']);

	return ch;
}

void atbash_cipher(char const *const string, char const *const key)
{
	for(size_t i = 0; string[i]; ++i)
		putchar(exchange_char(string[i], key));
}

int main(int const argc, char *const *const argv)
{
	bool check_unique = false;
	bool print_comparison = false;

	char const *const key = (argc > 1) ? strtolower(argv[1]) : NULL;

	if(key == NULL)
		error(EXIT_FAILURE, 0, "first argument must be the key");

	if(strcmp(key, "-h") == 0 || strcmp(key, "--help") == 0)
		usage(EXIT_SUCCESS);

	/* First argument is positional */
	optind++;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "hpu", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			case 'p':
				print_comparison = true;
				break;
			case 'u':
				check_unique = true;
				break;
			default:
				usage(EXIT_FAILURE);
		}
	}

	if(strlen(key) != ALPHABET_SIZE)
		error(EXIT_FAILURE, 0, "key must be %d characters long", ALPHABET_SIZE);

	if(!stralpha(key))
		error(EXIT_FAILURE, 0, "key must be alphabetic");

	if(check_unique && !strunique(key))
		error(EXIT_FAILURE, 0, "key must be unique (called with '--unique')");

	if(print_comparison) {
		puts("abcdefghijklmnopqrstuvwxyz");
		puts(key);
		putchar('\n');
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	for(size_t i = 0; strings_list[i]; ++i) {
		atbash_cipher(strings_list[i], key);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}

