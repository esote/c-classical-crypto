/* affine cipher -- encrypt and decrypt strings with a simple formula */

#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... A B [STRING]...\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Encrypt and decrypt strings with a simple formula.");
		printf("Example: %s -e 5 7 \"Hello World!\"\n", name);
		puts("\nOptions:\n\
  -d, --decrypt    decrypt input strings\n\
  -e, --encrypt    encrypt input strings\n\
  -h, --help       display this help text and exit");
		printf("\nA must be coprime of %d, default mode is encryption.\n",
			   ALPHABET_SIZE);
	}

	exit(status);
}

__attribute__((const))
static inline intmax_t gcd(intmax_t const a, intmax_t const b)
{
	return (b != 0) ? gcd(b, a % b) : a;
}

/* Calculate modular multiplicitive inverse,
	source: Rosetta Code */
__attribute__((const))
static intmax_t mod_inverse(intmax_t a, intmax_t b)
{
	if(b == 1)
		return 1;

	intmax_t const b0 = b;
	intmax_t t, q;
	intmax_t x0 = 0, x1 = 1;

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
static char encrypt_char(char const ch, intmax_t const a, intmax_t const b)
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

static char decrypt_char(char const ch, intmax_t const b,
						 intmax_t const mod_inv)
{
	intmax_t dec;

	if(ch >= 'a' && ch <= 'z')
		dec = 'a';
	else if(ch >= 'A' && ch <= 'Z')
		dec = 'A';
	else
		return ch;

	return (char)(dec + (mod_inv * (ALPHABET_SIZE + ch - dec - b)
			% ALPHABET_SIZE));
}

static void affine_cipher(char const *const string, intmax_t const a,
						  intmax_t const b, enum cipher_mode const cipher_mode,
						  intmax_t const mod_inv)
{
	for(size_t i = 0; string[i]; ++i) {
		if(cipher_mode == encrypt)
			putchar(encrypt_char(string[i], a, b));
		else
			putchar(decrypt_char(string[i], b, mod_inv));
	}
}

int main(int const argc, char *const *const argv)
{
	enum cipher_mode cipher_mode = none;

	static char const *const default_strings_list[] = {NULL};
	char const *const *strings_list;

	int c;

	while((c = getopt_long(argc, argv, "deh", long_opts, NULL)) != -1) {
		switch(c) {
			case 'd':
				cipher_mode = decrypt;
				break;
			case 'e':
				cipher_mode = encrypt;
				break;
			case 'h':
				usage(EXIT_SUCCESS, argv[0]);
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
		}
	}

	if(cipher_mode == none)
		cipher_mode = encrypt;

	char const *const a_tmp = argv[optind++];
	char const *const b_tmp = argv[optind++];

	if(a_tmp == NULL || b_tmp == NULL)
		error(EXIT_FAILURE, 0, "missing A and B, try '--help'");

	/* Signed to avoid error when calculating
		modular multiplicitive inverse */
	intmax_t const a = strtoimax(a_tmp, NULL, 10);
	intmax_t const b = strtoimax(b_tmp, NULL, 10);

	if(a < 0 || b < 0)
		error(EXIT_FAILURE, 0, "A and B must be positive, try '--help'");

	if(gcd(a, ALPHABET_SIZE) != 1) {
		error(EXIT_FAILURE, 0, "A must be coprime to %d, try '--help'",
								ALPHABET_SIZE);
	}

	strings_list = (optind < argc
					? (char const *const *) &argv[optind]
					: default_strings_list);

	/* keep value between affine_cipher calls,
		only required for decrypt */
	intmax_t const mod_inv = mod_inverse(a, ALPHABET_SIZE);

	for(size_t i = 0; strings_list[i]; ++i) {
		affine_cipher(strings_list[i], a, b, cipher_mode, mod_inv);
		putchar('\n');
	}

	return EXIT_SUCCESS;
}

