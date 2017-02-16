/* 
 * File:   RegEx.h
 * Author: mk
 */

#ifndef REGEX_H
#define	REGEX_H

#include <regex.h>
#include <string>

class RegEx {
public:
    RegEx(const char *pattern, int flags = REG_EXTENDED);
    ~RegEx();
    inline size_t SubCount() { return re.re_nsub; }
    int Test(const char *s, int flags = 0);
    int Match(const char *src, int flags = 0);
    std::string Sub(size_t index);
    const char* Sub(size_t index, char *dest);   
private:
    regex_t re;
    regmatch_t *match;
    const char *source;
    int status;
};

#endif	/* REGEX_H */

