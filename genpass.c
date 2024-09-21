#include <err.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CH_ALPHA	0x1
#define CH_DIGIT	0x2
#define CH_SYMBOL	0x4
#define CH_WHITESPACE	0x8

static void usage(void);

int
main(int argc, char *argv[])
{
	char map[89];
	int ch, i, len, map_len, mask;

	if (pledge("stdio", NULL) == -1)
		err(1, "pledge");

	mask = 0;
	while ((ch = getopt(argc, argv, "adsw")) != -1) {
		switch (ch) {
		case 'a':
			mask |= CH_ALPHA;
			break;
		case 'd':
			mask |= CH_DIGIT;
			break;
		case 's':
			mask |= CH_SYMBOL;
			break;
		case 'w':
			mask |= CH_WHITESPACE;
			break;
		default:
			usage();
		}
	}

	if (mask == 0)
		mask = CH_ALPHA | CH_DIGIT | CH_SYMBOL;

	argc -= optind;
	argv += optind;

	if (argc == 1) {
		const char *errstr;

		len = strtonum(argv[0], 0, INT_MAX, &errstr);
		if (errstr != NULL)
			errx(1, "length was %s", errstr);
	}
	else if (argc == 0)
		len = 32;
	else
		usage();

	map_len = 0;

	if (mask & CH_ALPHA) {
		for (i = 'a'; i <= 'z'; i++)
			map[map_len++] = i;
		for (i = 'A'; i <= 'Z'; i++)
			map[map_len++] = i;
	}

	if (mask & CH_DIGIT)
		for (i = '0'; i <= '9'; i++)
			map[map_len++] = i;

	if (mask & CH_SYMBOL) {
		for (i = '!'; i <= '/'; i++)
			map[map_len++] = i;
		for (i = '['; i <= '`'; i++)
			map[map_len++] = i;
		for (i = '{'; i <= '~'; i++)
			map[map_len++] = i;
	}

	if (mask & CH_WHITESPACE) {
		memcpy(&map[map_len], " \t", 2);
		map_len += 2;
	}

	for (; len != 0; len--)
		if (putchar(map[arc4random_uniform(map_len)]) == EOF)
			err(1, "putchar");

	if (putchar('\n') == EOF)
		err(1, "putchar");
}

static void
usage(void)
{
	fprintf(stderr, "usage: genpass [-adsw] [length]\n");
	exit(2);
}
