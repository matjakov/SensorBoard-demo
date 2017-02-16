/* 
 * File:   RegEx.cpp
 * Author: mk
 */

#include "RegEx.h"
#include <string.h>

/**
    Compiles a reqular expression pattern.
    \param pattern POSIX-compliant regular expression pattern
    \param flags REG_EXTENDED, REG_ICASE, REG_NOSUB, REG_NEWLINE
*/    
RegEx::RegEx(const char *pattern, int options) 
{
    status = regcomp(&re, pattern, options);
    match = new regmatch_t[re.re_nsub+1];
}

RegEx::~RegEx() 
{
    regfree(&re);
    delete [] match;
}

int RegEx::Test(const char *s, int flags)
{
    return regexec(&re, s, 0, NULL, flags);
}    

/**
    Matches a string against the complied pattern.
    Gets the first match and saves the source pointer.
    If 'src' is NULL the stored pointer is used to obtain 
    the next possible match, if there was one before.
    The expression must be compiled *without* REG_NOSUB.
    \param src string to match
    \param flags REG_NOTBOL, REG_NOTEOL
*/
int RegEx::Match(const char *src, int flags)
{
        if (src)
                // first match
                source = src;
        else if (!status)
                // next match
                source += match[0].rm_eo;
        else
                return status;
        status =  regexec(&re, source, re.re_nsub+1, match, flags);
        return status;
}


std::string RegEx::Sub(size_t index)
{   
    if (index > re.re_nsub)
        return "";
    int n = match[index].rm_eo - match[index].rm_so;
    std::string s(source + match[index].rm_so, n);
    return s;
}

const char* RegEx::Sub(size_t index, char *dest)
{
    if (index > re.re_nsub)
        return NULL;
    int n = match[index].rm_eo - match[index].rm_so;
    dest = strncpy(dest, source + match[index].rm_so, n);
    dest[n] = '\0';
    return dest;
}