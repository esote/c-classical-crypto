/* affine cipher -- encrypt and decrypt strings with a simple formula */

#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "affine-cipher"

#define _warn(...) do {					\
		if(!quiet)						\
			error(0, 0, __VA_ARGS__);	\
	} while(0)

/* Disable printing warnings */
static bool quiet;

/* Convert string to intmax_t in base 10 */
#define STRTOIMAX_BASE 10

/* Standard 26-character alphabet */
#define ALPHABET_SIZE 26

/* Affine cipher mode */
enum cipher_mode {
	decrypt, encrypt, none
};

static struct option const long_opts[] = {
	{"decrypt", no_argument, NULL, 'd'},
	{"encrypt", no_argument, NULL, 'e'},
	{"help", no_argument, NULL, 'h'},
	{"quiet", no_argument, NULL, 'q'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [A] [B] [OPTION]... [STRING]...\n",
				PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [A] [B] [OPTION]... [STRING]...\n", PROGRAM_NAME);
		puts("Encrypt and decrypt strings with a simple formula.");
		printf("Example: %s 5 7 -e \"Hello World!\"\n", PROGRAM_NAME);
		puts("\nOptions:\n\
  -d, --decrypt    decrypt input strings\n\
  -e, --encrypt    encrypt input strings\n\
  -h, --help       display this help text and exit\n\
  -q, --quiet      disable warning when cipher mode is unspecified");
		printf("\nA must be coprime of %d.\n", ALPHABET_SIZE);
	}

	exit(status);
}

intmax_t gcd(intmax_t const a, intmax_t const b)
{
	return (b != 0) ? gcd(b, a % b) : a;
}

/* Calculate modular multiplicitive inverse,
	source: Rosetta Code */
intmax_t mod_inverse(intmax_t a, intmax_t b)
{
	intmax_t b0 = b, t, q;
	intmax_t x0 = 0, x1 = 1;
	
	if(b == 1)
		return 1;
	
	while(a > 1) {
		q = a / b;
		t = b;
		b = a % b;
		a = t;
		t = x0;
		x0 = x1 - q * x0;
		x1 = t;
	}

	if(x1 < 0)
		x1 += b0;
	
	return x1;
}

/* Assumes contiguous character encoding from a-z A-Z */
char encrypt_char(char const ch, intmax_t const a, intmax_t const b)
{
	intmax_t enc;

	if(ch >= 'a' && ch <= 'z')
		enc = 'a';
	else if(ch >= 'A' && ch <= 'Z')
		enc = 'A';
	else
		return ch;

	return (char)(enc + ((a * ((intmax_t)ch - enc) + b) % ALPHABET_SIZE));
}

char decrypt_char(char const ch, intmax_t const a, intmax_t const b)
{
	intmax_t dec;

	if(ch >= 'a' && ch <= 'z')
		dec = 'a';
	else if(ch >= 'A' && ch <= 'Z')
		dec = 'A';
	else
		return ch;

	return (char)(dec + (mod_inverse(a, ALPHABET_SIZE)
		* (ALPHABET_SIZE + ch - dec - b) % ALPHABET_SIZE));
}

void affine_cipher(char const *const string, intmax_t const a,
				   intmax_t const b, enum cipher_mode const cipher_mode)
{
	if(gcd(a, ALPHABET_SIZE) != 1)
		error(EXIT_FAILURE, 0, "A must be coprime to %d", ALPHABET_SIZE);

	for(size_t i = 0; string[i]; ++i) {
		if(cipher_mode == encrypt)
			putchar(encrypt_char(string[i], a, b));
		else
			putchar(decrypt_char(string[i], a, b));
	}
}

int main(int const argc, char *const *const argv)
{
	enum cipher_mode cipher_mode = none;

	char const *const key_a = (argc > 1) ? argv[1] : NULL;
	char const *const key_b = (argc > 2) ? argv[2] : NULL;

	if(key_a != NULL
	   && (strcmp(key_a, "-h") == 0 || strcmp(key_a, "--help") == 0)) {
		usage(EXIT_SUCCESS);
	}

	if(key_a == NULL || key_b == NULL)
		error(EXIT_FAILURE, 0, "first argument must be A, second argument must be B");

	/* First two arguments are positional */
	optind += 2;

	quiet = false;

	/* Signed to avoid error when calculating
		modular multiplicitive inverse */
	intmax_t a = 0, b = 0;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "dehq", long_opts, NULL)) != -1) {
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

	a = strtoimax(key_a, NULL, STRTOIMAX_BASE);
	b = strtoimax(key_b, NULL, STRTOIMAX_BASE);

	if(a < 0 || b < 0)
		error(EXIT_FAILURE, 0, "A and B must be positive");

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	for(size_t i = 0; strings_list[i]; ++i) {
		affine_cipher(strings_list[i], a, b, cipher_mode);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}

