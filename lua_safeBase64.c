#include <string.h>
#include "lua.h"
#include "lauxlib.h"

#define LIB_NAME "safeBase64"

static const char encodeChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const char decodeIndex[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 62, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 63,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static int lencode(lua_State *L) {
	unsigned i;
	unsigned unit;
	size_t l;
	const char* s;
	char* temp;
	luaL_Buffer buff;

	s = luaL_checklstring(L, 1, &l);
	luaL_buffinit(L, &buff);
	temp = luaL_prepbuffsize(&buff, 4 * (l + 3) / 3);

	for (i = 0; i < l; i += 3) {
		unit = ((unsigned)s[i] << 16) | ((unsigned)s[i + 1] << 8) | (unsigned)s[i + 2];
		*temp++ = encodeChar[(unit >> 18) & 0x3F];
		*temp++ = encodeChar[(unit >> 12) & 0x3F];
		*temp++ = encodeChar[(unit >> 6)  & 0x3F];
		*temp++ = encodeChar[unit & 0x3F];
	}

	luaL_addsize(&buff, 4 * i / 3);
	switch (i - l) {
		case 0: 
			break;
		case 1: 
			unit = ((unsigned)s[i - 2] << 16) | (unsigned)s[i - 1] << 8);
			*temp++ = encodeChar[(unit >> 18) & 0x3F];
			*temp++ = encodeChar[(unit >> 12) & 0x3F];
			*temp++ = encodeChar[(unit >> 6)  & 0x3F];
			break;
		case 2: 
			unit = ((unsigned)s[i - 2] << 16);
			*temp++ = encodeChar[(unit >> 18) & 0x3F];
			*temp++ = encodeChar[(unit >> 12) & 0x3F];
	}

	luaL_pushresult(&buff);

	return 1;
}

static int ldecode(lua_State* L) {
	unsigned i;
	size_t l;
	const char* s;
	char* temp;
	luaL_Buffer buff;

	s = luaL_checklstring(L, 1, &l);
	luaL_buffinit(L, &buff);
	temp = luaL_prepbuffsize(&buff, 3 * (l + 4) / 4);

	for (i = 0; i < l; i += 4) {
		*temp++ = decodeIndex[s[i]] << 2 | decodeIndex[s[i + 1]] >> 4;
        *temp++ = decodeIndex[s[i + 1]] << 4 | decodeIndex[s[i + 2]] >> 2;
        *temp++ = decodeIndex[s[i + 2]] << 6 | decodeIndex[s[i + 3]];
	}

	luaL_addsize(&buff, 3 * i / 4);
	switch (i - l) {
		case 0: 
			break;
		case 1: 
			*temp++ = decodeIndex[s[i - 3]] << 2 | decodeIndex[s[i - 2]] >> 4;
        	*temp++ = decodeIndex[s[i - 2]] << 4 | decodeIndex[s[i - 1]] >> 2;
        	*temp++ = decodeIndex[s[i - 1]] << 6;
			break;
		case 2: 
			*temp++ = decodeIndex[s[i - 3]] << 2 | decodeIndex[s[i - 2]] >> 4;
        	*temp++ = decodeIndex[s[i - 2]] << 4;
			break;
		case 3:
			*temp++ = decodeIndex[s[i - 3]] << 2;
	}

	luaL_pushresult(&buff);
	
	return 1;
}

static const luaL_Reg lib[] = {
	{"encode", lencode},
	{"decode", ldecode},
	{NULL, NULL}
};

int luaopen_safeBase64(lua_State* L) {
	luaL_register(L, LIB_NAME, lib);

	return 1;
}
