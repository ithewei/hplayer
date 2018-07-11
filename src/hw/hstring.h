#ifndef H_STRING_H
#define H_STRING_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

typedef std::string	HString;

inline int vscprintf(const char* fmt, va_list ap) {
	return vsnprintf(NULL, 0, fmt, ap);
}

inline std::string asprintf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int len = vscprintf(fmt, ap) + 1;
	va_end(ap);
	// must recall va_start in linux
	va_start(ap, fmt);
	char* buf = (char*)malloc(len);
	vsnprintf(buf, len, fmt, ap);
	va_end(ap);
	buf[len] = '\0';
	std::string ret(buf);
	free(buf);
	return ret;
}

inline std::vector<std::string> split(std::string& str, char delim){
	std::stringstream ss;
	ss << str;
	std::string item;
	std::vector<std::string> res;
	while (std::getline(ss, item, delim)){
		res.push_back(item);
	}
	return res;
}

inline std::string trim(std::string& str){
	std::string::size_type pos1 = str.find_first_not_of(" \t\r\n");
	if (pos1 == std::string::npos)	return "";

	std::string::size_type pos2 = str.find_last_not_of(" \t\r\n");
	return str.substr(pos1, pos2-pos1+1);
}

#endif // H_STRING_H
