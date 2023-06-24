#include "internal.h"

static const char digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static const char udigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

#define BUFSZ 64

char *uconv(char *buf, unsigned long v, int base, int ucase)
{
	char *p = buf + BUFSZ;
	const char *d = ucase ? udigits : digits;

	*--p = '\0';
	if (v == 0) {
		*--p = d[0];
	} else {
		while (v) {
			*--p = d[v % base];
			v /= base;
		}
	}
	return p;
}

char *iconv(char *buf, long v, int base, int ucase)
{
	char *p;

	if (v < 0) {	// XXX: doesn't work if v is the minimum long long
		p = uconv(buf, -v, base, ucase);
		*--p = '-';
	} else {
		p = uconv(buf, v, base, ucase);
	}
	return p;
}

static void append(char **pdst, size_t *psize, const char *src, size_t n)
{
	while (n > 0 && *psize > 0) {
		*(*pdst)++ = *src++;
		(*psize)--;
		n--;
	}
}

static int getint(const char **pfmt, va_list ap)
{
	int w;

	if (**pfmt == '*') {
		(*pfmt)++;
		return va_arg(ap, int);
	}
	for (w = 0; **pfmt >= '0' && **pfmt <= '9'; (*pfmt)++)
		w = w * 10 + (**pfmt - '0');
	return w;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
	size_t count, len, maxlen;
	char *p, *src;
	int just;	// 0: right, 1: left with '0' filler, 2: right
	int sign;	// 0: none, 1: space, 2: plus
	int alternate;	// length of alternate prefix
	char alt_prefix[2];
	int width;
	int precision;
	int modifier, doubled;
	long iscalar;
	unsigned long uscalar;
	void *pointer;
	char buf[BUFSZ];
	int terminate;

	terminate = 0;
	if (size > 0) {
		terminate = 1;
		size--;
	}

	count = 0;
	while (*fmt != '\0') {
		// copy up to next '%'
		for ( ; *fmt != '%' && *fmt != '\0'; fmt++) {
			append(&str, &size, fmt, 1);
			count++;
		}
		if (*fmt == '\0')
			break;
		// process format
		fmt++;	// skip the '%'
		just = 0;
		sign = 0;
		alternate = 0;
		// flags
		for ( ; (p = index("#-+ 0", *fmt)) != nullptr; fmt++) {
			switch (*p) {
			case '#':
				alternate = 1;
				break;
			case '-':
				just = 2;
				break;
			case '+':
				sign = 2;
				break;
			case ' ':
				if (sign == 0)
					sign = 1;
				break;
			case '0':
				if (just == 0)
					just = 1;
				break;
			}
		}
		if (*fmt == '\0')
			break;
		// minimum width
		width = getint(&fmt, ap);
		if (*fmt == '\0')
			break;
		if (width < 0) {
			just = 1;
			width = -width;
		}
		// precision
		precision = -1;
		if (*fmt == '.') {
			fmt++;
			precision = getint(&fmt, ap);
			if (*fmt == '\0')
				break;
		}
		// lenght modifier
		modifier = 0;
		doubled = 0;
		if ( (p = index("hljtz", *fmt)) != nullptr ) {
			modifier = *p;
			fmt++;
			if ((*p == 'h') && *fmt == *p) {
				fmt++;
				doubled = 1;
			}
			if (*fmt == '\0')
				break;
		}
		// conversion
		// 1) get the argument (integer/unsigned scalar or pointer)
		switch (*fmt) {
		case 'd':
		case 'i':
			switch (modifier) {
			case 0:
				iscalar = va_arg(ap, int);
				break;
			case 'h':
				iscalar = doubled ? (signed char)va_arg(ap, int) :
						    (short)va_arg(ap, int);
				break;
			case 'l':
				iscalar = va_arg(ap, long);
				break;
			case 'j':
				iscalar = va_arg(ap, intmax_t);
				break;
			case 't':
				iscalar = va_arg(ap, ptrdiff_t);
				break;
			case 'z':
				iscalar = va_arg(ap, ssize_t);
				break;
			}
			break;
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			switch (modifier) {
			case 0:
				uscalar = va_arg(ap, unsigned int);
				break;
			case 'h':
				uscalar = doubled ? (unsigned char)va_arg(ap, unsigned int) :
						    (unsigned short)va_arg(ap, unsigned int);
				break;
			case 'l':
				uscalar = va_arg(ap, unsigned long);
				break;
			case 'j':
				uscalar = va_arg(ap, uintmax_t);
				break;
			case 't':
				uscalar = va_arg(ap, uptrdiff_t);
				break;
			case 'z':
				uscalar = va_arg(ap, size_t);
				break;
			}
			break;
		case 'n':
			switch (modifier) {
			case 0:
				*va_arg(ap, int *) = count;
				break;
			case 'h':
				if (doubled) {
					*va_arg(ap, signed char *) = count;
				} else {
					*va_arg(ap, short *) = count;
				}
				break;
			case 'l':
				*va_arg(ap, long *) = count;
				break;
			case 'j':
				*va_arg(ap, uintmax_t *) = count;
				break;
			case 't':
				*va_arg(ap, ptrdiff_t *) = count;
				break;
			case 'z':
				*va_arg(ap, ssize_t *) = count;
				break;
			}
			fmt++;
			continue;
		case 'c':
			if (modifier)
				goto error;
			uscalar = (unsigned char)va_arg(ap, int);
			break;
		case 's':
		case 'p':
			if (modifier)
				goto error;
			pointer = va_arg(ap, void *);
			break;
		case '%':
			if (modifier)
				goto error;
			break;
		default:
			goto error;
		}
		// 2) apply the conversion
		switch (*fmt) {
		case 'd':
		case 'i':
			src = iconv(buf, iscalar, 10, 0);
			if (sign && iscalar > 0)
				*--src = sign == 1 ? ' ': '+';
			alternate = 0;
			break;
		case 'u':
			src = uconv(buf, uscalar, 10, 0);
			alternate = 0;
			break;
		case 'o':
			src = uconv(buf, uscalar, 8, 0);
			if (alternate) {
			       if (*src != '0')
					alt_prefix[0] = '0';
			       else
				       alternate = 0;
			}
			break;
		case 'x':
		case 'X':
			src = uconv(buf, uscalar, 16, *fmt == 'X');
			if (alternate) {
				if (uscalar) {
					alt_prefix[0] = '0';
					alt_prefix[1] = *fmt;
					alternate = 2;
				} else {
					alternate = 0;
				}
			}
			break;
		case 'p':
			src = uconv(buf, (uintptr_t)pointer, 16, 0);
			*--src = 'x';
			*--src = '0';
			alternate = 0;
			break;
		case 's':
			src = static_cast<char *>(pointer);
			alternate = 0;
			break;
		case 'c':
		case '%':
			buf[0] = *fmt == '%' ? '%' : uscalar;
			buf[1] = '\0';
			src = buf;
			alternate = 0;
			break;
		}
		// 3) append taking care of width and precision
		maxlen = UINT_MAX;
		if (*fmt == 's' && precision > 0)
			maxlen = precision;
		len = strnlen(src, maxlen) + alternate;
		if (index("diouxX", *fmt) && precision > 0) {
			if (precision >= BUFSZ)
				goto error;
			for ( ; len < (size_t)precision; len++)
				*--src = '0';
			if (just == 1)
				just = 0;
		}
		if (just == 1)
			append(&str, &size, alt_prefix, alternate);
		if (just < 2) {
			char filler = just == 1 ? '0' : ' ';
			for ( ; (size_t)width > len; width--) {
				append(&str, &size, &filler, 1);
				count++;
			}
		}
		if (just != 1)
			append(&str, &size, alt_prefix, alternate);
		append(&str, &size, src, len - alternate);
		count += len;
		if (just == 2) {
			char filler = ' ';
			for ( ; (size_t)width > len; width--) {
				append(&str, &size, &filler, 1);
				count++;
			}
		}
		fmt++;
	}
	if (terminate)
		*str = '\0';
	return count;
error:
	return -1;
}
