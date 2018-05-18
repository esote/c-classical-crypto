/* atbash-cipher -- monoalphabetic substitution cipher */

#include <ctype.h>
#include <error.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Standard 26-character alphabet */
#define ALPHABET_SIZE 26

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"print", no_argument, NULL, 'p'},
	{"unique", no_argument, NULL, 'u'},

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... KEY [STRING]...\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Monoalphabetic substitution cipher.");
		printf("Example: %s -u -p bcdefghijklmnopqrstuvwxyza Hello\n", name);
		puts("\nOptions:\n\
  -h, --help      display this help text and exit\n\
  -p, --print     print the key and normal alphabet for comparison\n\
  -u, --unique    check key for alphabetic uniqueness");
	}

	exit(status);
}

__attribute__((const))
static bool strunique(char const *c)
{
	bool arr[CHAR_MAX - CHAR_MIN] = {false};

	while(*c) {
		if(arr[(size_t)*c])
			return false;
		else
			arr[(size_t)*c++] = true;
	}

	return true;
}

__attribute__((const))
static bool stralpha(char const *c)
{
	while(*c) {
		if(!isalpha(*c++))
			return false;
	}

	return true;
}

/* Assumes contiguous character encoding from a-z A-Z */
static char exchange_char(char const ch, char const *const key)
{
	if(ch >= 'a' && ch <= 'z')
		return key[ch - 'a'];
	else if(ch >= 'A' && ch <= 'Z')
		return (char)toupper(key[ch - 'A']);

	return ch;
}

static void atbash_cipher(char const *const string, char const *const key)
{
	for(size_t i = 0; string[i]; ++i)
		putchar(exchange_char(string[i], key));
}

int main(int const argc, char *const *const argv)
{
	bool check_unique = false;
	bool print_comparison = false;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "hpu", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS, argv[0]);
				break;
			case 'p':
				print_comparison = true;
				break;
			case 'u':
				check_unique = true;
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
		}
	}

	char const *const key = argv[optind++];

	if(key == NULL)
		error(EXIT_FAILURE, 0, "first argument must be the key, try '--help'");

	if(strlen(key) != ALPHABET_SIZE)
		error(EXIT_FAILURE, 0, "key must be %d characters long",
								ALPHABET_SIZE);

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

