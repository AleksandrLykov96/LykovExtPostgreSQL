
#ifndef __GLOBAL_FUNCTION_H__
#define __GLOBAL_FUNCTION_H__

#include "stdafx.h"

#include <common/base64.h>
#include <uchar.h>
#include <utils/builtins.h>

typedef char16_t mvarchar;

#define LYKOV_GET_DATUM_CSTRING_WITH_LEN(s, l) PointerGetDatum(cstring_to_text_with_len(s, l))
#define LYKOV_COUNT_ASCII_SYMBOLS 256
#define LYKOV_SIZE_OF_MCHAR_INFO 2
#define LYKOV_SIZE_SYMBOL_IN_BYTEA_HEX 2
#define LYKOV_FORMAT_BASE_HEX_SIZE 3

static mbstate_t gl_state = { 0 };
static const char* gl_Hex_Ascii_Symbols[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
	"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
	"c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
	"e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
};

// Работа со строками
unsigned short gl_NumberOfDigitsOfNumber(unsigned long);
char* gl_SubStr(const char*, size_t, size_t);
char* gl_Concatenate(const char*, const char*, size_t, size_t);
size_t gl_RemoveCharactersFromChar(char*, const char*);
char* gl_MvarcharToChar(const mvarchar*, size_t);
char* gl_McharToChar(const mvarchar*, size_t);

// Вспомогательные функции
const char* gl_GetHexFromAscii(unsigned char);
unsigned char gl_GetAsciiFromHex(const char*, unsigned short);
unsigned char gl_GetAsciiFromMvarcharHex(const mvarchar*, unsigned short);
Datum gl_ConvertNumericToByteaHex(unsigned long, unsigned short);
unsigned long gl_ConvertHexToNumeric(const char*, size_t);
char* gl_EncodeBase64FromText(const char*, size_t, size_t*);
char* gl_DecodeBase64FromText(const char*, size_t, size_t*);

#endif // __GLOBAL_FUNCTION_H__
