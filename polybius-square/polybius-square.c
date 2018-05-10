/* polybius square -- map alphabet characters to digits */

#include <ctype.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "polybius-square"

#define _warn(...) do {					\
		if(!quiet)						\
			error(0, 0, __VA_ARGS__);	\
	} while(0)

/* Disable printing warnings */
static bool quiet;

/* Polybius square mode */
enum cipher_mode {
	decrypt, encrypt, none
};

static struct option const long_opts[] = {
	{"decrypt", no_argument, NULL, 'd'},
	{"encrypt", no_argument, NULL, 'e'},
	{"help", no_argument, NULL, 'h'},
	{"i", no_argument, NULL, 'i'},
	{"j", no_argument, NULL, 'j'},
	{"quiet", no_argument, NULL, 'q'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [OPTION]... [STRING/COORD]...\n",
				PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [OPTION]... [STRING/COORD]...\n", PROGRAM_NAME);
		puts("Map alphabet characters to digits.");
		printf("Example: %s -e Hello World\n", PROGRAM_NAME);
		puts("\nOptions:\n\
  -d, --decrypt    decrypt input strings (use coordinates)\n\
  -e, --encrypt    encrypt input strings (use strings)\n\
  -h, --help       display this help text and exit\n\
  -i, --i          coordinate 24 represents 'I' (default)\n\
  -j, --j          coordinate 24 represents 'J'\n\
  -q, --quiet      disable warnings");
	}

	exit(status);
}

/* Traditional 5x5 polybius square */
static char square[5][5] = {
	{'A', 'B', 'C', 'D', 'E'},
	{'F', 'G', 'H', 'I', 'K'},
	{'L', 'M', 'N', 'O', 'P'},
	{'Q', 'R', 'S', 'T', 'U'},
	{'V', 'W', 'X', 'Y', 'Z'}
};

/* Map letters to coordinates,
   assumes contiguous character encoding from a-z */
static char const *const square_map['z' - 'a' + 1] = {
	['a' - 'a'] = "11",
	['b' - 'a'] = "12",
	['c' - 'a'] = "13",
	['d' - 'a'] = "14",
	['e' - 'a'] = "15",
	['f' - 'a'] = "21",
	['g' - 'a'] = "22",
	['h' - 'a'] = "23",
	['i' - 'a'] = "24",
	['j' - 'a'] = "24", /* Same value as 'I' */
	['k' - 'a'] = "25",
	['l' - 'a'] = "31",
	['m' - 'a'] = "32",
	['n' - 'a'] = "33",
	['o' - 'a'] = "34",
	['p' - 'a'] = "35",
	['q' - 'a'] = "41",
	['r' - 'a'] = "42",
	['s' - 'a'] = "43",
	['t' - 'a'] = "44",
	['u' - 'a'] = "45",
	['v' - 'a'] = "51",
	['w' - 'a'] = "52",
	['x' - 'a'] = "53",
	['y' - 'a'] = "54",
	['z' - 'a'] = "55"
};

/* Assumes contiguous character encoding from a-z A-Z */
char const *encrypt_char(char const ch)
{
	if(!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z'))
		return NULL;

	return square_map[tolower(ch) - 'a'];
}

char decrypt_char(char const *const string)
{
	int const a = string[0] - '0';
	int const b = string[1] - '0';
	return square[a - 1][b - 1];
}

void polybius_square(char const *const string,
					 enum cipher_mode const cipher_mode)
{
	if(cipher_mode == encrypt) {
		for(size_t i = 0; string[i]; ++i) {
			char const *const enc = encrypt_char(string[i]);
			if(enc == NULL) {
				_warn("\ncharacter '%c' cound not be mapped to "
					 "coordinates, skipping", string[i]);
				continue;
			}
			printf("%s ", enc);
		}
	} else {
		if(strlen(string) != 2) {
			_warn("\ncoordinates must be two digits long, skipping");
			return;
		} else if((string[0] - '0') < 1 || (string[0] - '0') > 5) {
			_warn("\nfirst coordinate digit must "
				 "be between 1 and 5, skipping");
			return;
		} else if((string[1] - '0') < 1 || (string[1] - '0') > 5) {
			_warn("\nsecond coordinate digit must "
				 "be between 1 and 5, skipping");
			return;
		}

		putchar(decrypt_char(string));
	}
}

int main(int const argc, char *const *const argv)
{
	enum cipher_mode cipher_mode = none;

	quiet = false;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "dehijq", long_opts, NULL)) != -1) {
		switch(c) {
			case 'd':
				cipher_mode = decrypt;
				break;
			case 'e':
				cipher_mode = encrypt;
				break;
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			case 'i':
				square[2 - 1][4 - 1] = 'I';
				break;
			case 'j':
				square[2 - 1][4 - 1] = 'J';
				break;
			case 'q':
				quiet = true;
				break;
			default:
				usage(EXIT_FAILURE);
		}
	}

	if(cipher_mode == none) {
		cipher_mode = encrypt;
		_warn("cipher mode unspecified ('--encrypt' or '--decrypt'), "
			 "defaulting to '--encrypt'");
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	for(size_t i = 0; strings_list[i]; ++i) {
		polybius_square(strings_list[i], cipher_mode);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}
