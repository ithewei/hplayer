#ifndef H_STRING_H
#define H_STRING_H

#include <stdio.h>
#include <stdarg.h>
#include <string>

typedef std::string	HString;

inline int vscprintf(const char* fmt, va_list ap) {
	return vsnprintf(NULL, 0, fmt, ap);
}

inline std::string asprintf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int len = vscprintf(fmt, ap) + 1;
	char* buf = (char*)malloc(len);
	vsnprintf(buf, len, fmt, ap);
	va_end(ap);
	buf[len] = '\0';
	std::string ret(buf);
	free(buf);
	return ret;
}

inline std::string ntoa(long long num) {
	return asprintf("%ld", num);
}

inline std::string ftoa(double f) {
	return asprintf("%lf", f);
}

#endif // H_STRING_H
