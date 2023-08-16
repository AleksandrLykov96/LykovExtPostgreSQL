
// ReSharper disable CppJoinDeclarationAndAssignment

#include "globalFunctions.h"

#pragma region Работа со строками

unsigned short gl_NumberOfDigitsOfNumber(unsigned long src) {
    unsigned short digits = 0;
    do {
	    digits++;
    } while (src /= 10);

    return digits;
}

char* gl_SubStr(const char* input, const size_t start, const size_t count) {
    char* result = palloc0(count + 1);
	strncpy(result, input + start, count);

    return result;
}

char* gl_Concatenate(const char* str1, const char* str2, size_t len1, size_t len2) {
    if (len1 == 0)
        len1 = strlen(str1);
    if (len2 == 0)
        len2 = strlen(str2);

    char* newChar = palloc0(len1 + len2 + 1);

    strncpy(newChar, str1, len1);
    strncpy(newChar + len1, str2, len2);

    return newChar;
}

size_t gl_RemoveCharactersFromChar(char* src, const char* symbols) {
    size_t i = 0, j = 0;

    while (src[i]) {
        if (!strchr(symbols, src[i]))
            src[j++] = src[i];

        i++;
    }

    src[j] = '\0';
    return i - j;
}

char* gl_MvarcharToChar(const mvarchar* src, const size_t srcSize) {
    // Предполагается, что преобразуется mvarchar для bytea, поэтому преобразовывем только по одному символу для char
	char* tmp = palloc0(srcSize);
    for (size_t i = 0; i < srcSize; i++)
        c16rtomb(tmp + i, src[i], &gl_state);

    return tmp;
}

char* gl_McharToChar(const mvarchar* src, const size_t srcSize) {
    // Первые 2 символа означают размер строки
    char* tmp = palloc0(srcSize);
    for (size_t i = 0; i < srcSize; i++)
        c16rtomb(tmp + i, src[i + LYKOV_SIZE_OF_MCHAR_INFO], &gl_state);

    return tmp;
}

#pragma endregion

#pragma region Вспомогательные функции

const char* gl_GetHexFromAscii(unsigned char symbol) {
    const unsigned short code = symbol;
    return code >= LYKOV_COUNT_ASCII_SYMBOLS ? "00" : gl_Hex_Ascii_Symbols[code];
}

unsigned char gl_GetAsciiFromHex(const char* src, const unsigned short start) {
    char result[LYKOV_SIZE_SYMBOL_IN_BYTEA_HEX + 1];

    result[0] = src[start];
    result[1] = src[start + 1];
    result[2] = '\0';

    return (unsigned short)strtoul(result, NULL, 16);
}

unsigned char gl_GetAsciiFromMvarcharHex(const mvarchar* src, const unsigned short start) {
    char result[LYKOV_SIZE_SYMBOL_IN_BYTEA_HEX + 1];

    c16rtomb(result, src[start], &gl_state);
    c16rtomb(result + 1, src[start + 1], &gl_state);
    result[2] = '\0';

    return (unsigned short)strtoul(result, NULL, 16);
}

Datum gl_ConvertNumericToByteaHex(const unsigned long src, const unsigned short degree) {
    const unsigned int sizeHex  = degree << 1;
    const unsigned short digits = gl_NumberOfDigitsOfNumber(sizeHex);
    bytea* resultBytea          = palloc0(degree + VARHDRSZ + 1);
    char hex[sizeHex + 1],
         format[LYKOV_FORMAT_BASE_HEX_SIZE + digits + 1];

    pg_snprintf(format, LYKOV_FORMAT_BASE_HEX_SIZE + digits + 1, "%%%d%dx", 0, sizeHex);
    pg_snprintf(hex, sizeHex + 1, format, src);

	hex_decode(hex, sizeHex, VARDATA(resultBytea));
    SET_VARSIZE_4B(resultBytea, degree + VARHDRSZ);

    PG_RETURN_BYTEA_P(resultBytea);
}

unsigned long gl_ConvertHexToNumeric(const char* src, const size_t srcSize) {
	const size_t sizeResult = srcSize << 1;
	char hex[sizeResult + 1];

    hex_encode(src, srcSize, hex);
    hex[sizeResult] = '\0';

    return strtoul(hex, NULL, 16);
}

char* gl_EncodeBase64FromText(const char* input, const size_t srcSize, size_t* sizeResult) {
	const size_t dstSize = pg_b64_enc_len(srcSize);
    char* result = palloc0(dstSize + 1);

#if PG_VERSION_NUM > 130000
    *sizeResult = pg_b64_encode(input, srcSize, result, dstSize);
#else
    *sizeResult = pg_b64_encode(input, srcSize, result);
#endif

	return result;
}

char* gl_DecodeBase64FromText(const char* input, const size_t srcSize, size_t* sizeResult) {
    const size_t dstSize = pg_b64_dec_len(srcSize);
    char* result = palloc0(dstSize + 1);

#if PG_VERSION_NUM > 130000
    *sizeResult = pg_b64_decode(input, srcSize, result, dstSize);
#else
    *sizeResult = pg_b64_decode(input, srcSize, result);
#endif

    return result;
}

#pragma endregion
