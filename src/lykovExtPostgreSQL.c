
// ReSharper disable CppJoinDeclarationAndAssignment

#include "lykovExtPostgreSQL.h"

#pragma region Вызов из PostgreSQL

void _PG_init(void) {
    elog(INFO, "Lykov's extension loaded");
}


// --------------- SMALLINT, INTEGER, BIGINT ---------------
Datum integerTypeToBytea(PG_FUNCTION_ARGS) {
    const int32 input = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT32(0);
    return convertIntToBytea(input, SIZE_OF_TYPE_BYTEA);
}

Datum integerKeyfieldToBytea(PG_FUNCTION_ARGS) {
    const int32 input = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT32(0);
    return convertIntToBytea(input, SIZE_OF_KEYFIELD_BYTEA);
}

Datum integerRTRefToBytea(PG_FUNCTION_ARGS) {
    const int32 input = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT32(0);
    return convertIntToBytea(input, SIZE_OF_RTREF_BYTEA);
}

Datum bigintVersionToText(PG_FUNCTION_ARGS) {
    const int64 input = PG_ARGISNULL(0) ? 0 : PG_GETARG_INT64(0);
    return convertIntVersionToText(input < 0 ? 0 : input);
}
// --------------- SMALLINT, INTEGER, BIGINT ---------------


// --------------- NUMERIC ---------------
Datum numericTypeToBytea(PG_FUNCTION_ARGS) {
    bool error = PG_ARGISNULL(0);
    const int32 input = error ? 0 : numeric_int4_opt_error(PG_GETARG_NUMERIC(0), &error);

    return convertIntToBytea(error ? 0 : input, SIZE_OF_TYPE_BYTEA);
}

Datum numericKeyfieldToBytea(PG_FUNCTION_ARGS) {
    bool error = PG_ARGISNULL(0);
    const int32 input = error ? 0 : numeric_int4_opt_error(PG_GETARG_NUMERIC(0), &error);

    return convertIntToBytea(error ? 0 : input, SIZE_OF_KEYFIELD_BYTEA);
}

Datum numericRTRefToBytea(PG_FUNCTION_ARGS) {
    bool error = PG_ARGISNULL(0);
    const int32 input = error ? 0 : numeric_int4_opt_error(PG_GETARG_NUMERIC(0), &error);

    return convertIntToBytea(error ? 0 : input, SIZE_OF_RTREF_BYTEA);
}

Datum numericVersionToText(PG_FUNCTION_ARGS) {
    bool error        = PG_ARGISNULL(0);
    const int32 input = error ? 0 : numeric_int4_opt_error(PG_GETARG_NUMERIC(0), &error);

    return convertIntVersionToText(error || input < 0 ? 0 : input);
}
// --------------- NUMERIC ---------------


// --------------- BYTEA ---------------
Datum byteaTypeToShort(PG_FUNCTION_ARGS) {
    bytea* input;
    if (PG_ARGISNULL(0))
        PG_RETURN_INT16(0);

    input = PG_GETARG_BYTEA_PP(0);
    PG_RETURN_INT16(gl_ConvertHexToNumeric(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input)));
}

Datum byteaKeyfieldToInteger(PG_FUNCTION_ARGS) {
    bytea* input;
    if (PG_ARGISNULL(0))
        PG_RETURN_INT32(0);

    input = PG_GETARG_BYTEA_PP(0);
    PG_RETURN_INT32(gl_ConvertHexToNumeric(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input)));
}

Datum byteaRTRefToInteger(PG_FUNCTION_ARGS) {
    bytea* input;
    if (PG_ARGISNULL(0))
        PG_RETURN_INT32(0);

    input = PG_GETARG_BYTEA_PP(0);
    PG_RETURN_INT32(gl_ConvertHexToNumeric(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input)));
}

Datum byteaRrefToUID(PG_FUNCTION_ARGS) {
    bytea* input;
    text* result;
    const char* data;

    if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(LYKOV_GET_DATUM_CSTRING_WITH_LEN(EMPTY_UID, UID_SIZE + 1));

    input = PG_GETARG_BYTEA_PP(0);
    if (VARSIZE_ANY_EXHDR(input) != BYTEA_UID_SIZE)
        PG_RETURN_DATUM(LYKOV_GET_DATUM_CSTRING_WITH_LEN(EMPTY_UID, UID_SIZE + 1));

    result = palloc0(UID_SIZE + VARHDRSZ + 1);
    SET_VARSIZE(result, UID_SIZE + VARHDRSZ);

    data = VARDATA_ANY(input);
    pg_snprintf(VARDATA(result),
        UID_SIZE + 1,
        FORMAT_UID,
        gl_GetHexFromAscii(data[12]),
        gl_GetHexFromAscii(data[13]),
        gl_GetHexFromAscii(data[14]),
        gl_GetHexFromAscii(data[15]),
        gl_GetHexFromAscii(data[10]),
        gl_GetHexFromAscii(data[11]),
        gl_GetHexFromAscii(data[8]),
        gl_GetHexFromAscii(data[9]),
        gl_GetHexFromAscii(data[0]),
        gl_GetHexFromAscii(data[1]),
        gl_GetHexFromAscii(data[2]),
        gl_GetHexFromAscii(data[3]),
        gl_GetHexFromAscii(data[4]),
        gl_GetHexFromAscii(data[5]),
        gl_GetHexFromAscii(data[6]),
        gl_GetHexFromAscii(data[7]));

    PG_RETURN_TEXT_P(result);
}

Datum byteaToDD(PG_FUNCTION_ARGS) {
    char* result = NULL;
    bytea* input;
    Datum forReturn;
    size_t sizeResult = 0;

    if (!PG_ARGISNULL(0)) {
        input = PG_GETARG_BYTEA_PP(0);
        result = gl_EncodeBase64FromText(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input), &sizeResult);
    }

    forReturn = sizeResult > 0 ? LYKOV_GET_DATUM_CSTRING_WITH_LEN(result, sizeResult) : LYKOV_GET_DATUM_CSTRING_WITH_LEN("", 0);
    if (result)
        pfree(result);

    PG_RETURN_DATUM(forReturn);
}

Datum convertByteaDDToText(PG_FUNCTION_ARGS) {
    bytea* input;
    Datum result;
    char* encoded,
        * substr;
    size_t sizeResult = 0;

    if (PG_ARGISNULL(0))
        PG_RETURN_DATUM(LYKOV_GET_DATUM_CSTRING_WITH_LEN("", 0));

    input = PG_GETARG_BYTEA_PP(0);

    encoded = gl_EncodeBase64FromText(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input), &sizeResult);
    if (sizeResult <= 0)
        PG_RETURN_DATUM(LYKOV_GET_DATUM_CSTRING_WITH_LEN("", 0));

    substr = gl_SubStr(encoded, 14, 4);

    result = strcmp(substr, "+7v3") == 0 && sizeResult > 25
        ? convertValueStorageToText(encoded, sizeResult)
        : convertDDToTextWithEncoding(PG_GETARG_DATUM(0));

    pfree(encoded);
    pfree(substr);

    PG_RETURN_DATUM(result);
}
// --------------- BYTEA ---------------


// --------------- TEXT ---------------
Datum textVersionToInteger(PG_FUNCTION_ARGS) {
    text* input;
    if (PG_ARGISNULL(0))
        PG_RETURN_INT64(0);

    input = PG_GETARG_TEXT_PP(0);
    return convertVersionToBigint(VARDATA_ANY(input), VARSIZE_ANY_EXHDR(input));
}

Datum textUIDToRref(PG_FUNCTION_ARGS) {
    bytea* result = palloc0(BYTEA_UID_SIZE + 1 + VARHDRSZ);
    text* data;
    const char* input;

    SET_VARSIZE(result, BYTEA_UID_SIZE + VARHDRSZ);
    if (PG_ARGISNULL(0))
        PG_RETURN_BYTEA_P(result);

    data = PG_GETARG_TEXT_PP(0);
    if (VARSIZE_ANY_EXHDR(data) != UID_SIZE)
        PG_RETURN_BYTEA_P(result);

    input = VARDATA_ANY(data);
    pg_snprintf(VARDATA(result),
        BYTEA_UID_SIZE + 1,
        BYTEA_UID,
        gl_GetAsciiFromHex(input, 19),
        gl_GetAsciiFromHex(input, 21),
        gl_GetAsciiFromHex(input, 24),
        gl_GetAsciiFromHex(input, 26),
        gl_GetAsciiFromHex(input, 28),
        gl_GetAsciiFromHex(input, 30),
        gl_GetAsciiFromHex(input, 32),
        gl_GetAsciiFromHex(input, 34),
        gl_GetAsciiFromHex(input, 14),
        gl_GetAsciiFromHex(input, 16),
        gl_GetAsciiFromHex(input, 9),
        gl_GetAsciiFromHex(input, 11),
        gl_GetAsciiFromHex(input, 0),
        gl_GetAsciiFromHex(input, 2),
        gl_GetAsciiFromHex(input, 4),
        gl_GetAsciiFromHex(input, 6));

    PG_RETURN_BYTEA_P(result);
}

Datum textDDToBytea(PG_FUNCTION_ARGS) {
    text* input;
    char* forDecode = "";
    size_t srcSize = 0;

	if (!PG_ARGISNULL(0)) {
        input     = PG_GETARG_TEXT_PP(0);
        srcSize   = VARSIZE_ANY_EXHDR(input);
        forDecode = VARDATA_ANY(input);
    }

    return convertCharToByteaDD(forDecode, srcSize);
}
// --------------- TEXT ---------------


// --------------- MVARCHAR ---------------
Datum mvarcharVersionToInteger(PG_FUNCTION_ARGS) {
    char* convertedChar;
    size_t size;
	text* input;
    Datum result;

    if (PG_ARGISNULL(0))
        PG_RETURN_INT64(0);
    
    input         = PG_GETARG_TEXT_PP(0);
    size          = VARSIZE_ANY_EXHDR(input) / sizeof(mvarchar);
    convertedChar = gl_MvarcharToChar((mvarchar*) VARDATA_ANY(input), size);
    result        = convertVersionToBigint(convertedChar, size);

    pfree(convertedChar);
    return result;
}

Datum mvarcharUIDToRref(PG_FUNCTION_ARGS) {
    bytea* result = palloc0(BYTEA_UID_SIZE + 1 + VARHDRSZ);
    text* data;
    mvarchar* input;

    SET_VARSIZE(result, BYTEA_UID_SIZE + VARHDRSZ);
    if (PG_ARGISNULL(0))
        PG_RETURN_BYTEA_P(result);

    data = PG_GETARG_TEXT_PP(0);
    if (VARSIZE_ANY_EXHDR(data) != UID_SIZE * sizeof(mvarchar))
        PG_RETURN_BYTEA_P(result);

    input = (mvarchar*)VARDATA_ANY(data);
    pg_snprintf(VARDATA(result),
        BYTEA_UID_SIZE + 1,
        BYTEA_UID,
        gl_GetAsciiFromMvarcharHex(input, 19),
        gl_GetAsciiFromMvarcharHex(input, 21),
        gl_GetAsciiFromMvarcharHex(input, 24),
        gl_GetAsciiFromMvarcharHex(input, 26),
        gl_GetAsciiFromMvarcharHex(input, 28),
        gl_GetAsciiFromMvarcharHex(input, 30),
        gl_GetAsciiFromMvarcharHex(input, 32),
        gl_GetAsciiFromMvarcharHex(input, 34),
        gl_GetAsciiFromMvarcharHex(input, 14),
        gl_GetAsciiFromMvarcharHex(input, 16),
        gl_GetAsciiFromMvarcharHex(input, 9),
        gl_GetAsciiFromMvarcharHex(input, 11),
        gl_GetAsciiFromMvarcharHex(input, 0),
        gl_GetAsciiFromMvarcharHex(input, 2),
        gl_GetAsciiFromMvarcharHex(input, 4),
        gl_GetAsciiFromMvarcharHex(input, 6));

    PG_RETURN_BYTEA_P(result);
}

Datum mvarcharDDToBytea(PG_FUNCTION_ARGS) {
    text* input;
    Datum result;
    char* forDecode = "";
    size_t srcSize = 0;

    if (!PG_ARGISNULL(0)) {
        input     = PG_GETARG_TEXT_PP(0);
        srcSize   = VARSIZE_ANY_EXHDR(input) / sizeof(mvarchar);
        forDecode = gl_MvarcharToChar((mvarchar*)VARDATA_ANY(input), srcSize);
    }

    result = convertCharToByteaDD(forDecode, srcSize);
    if (srcSize > 0)
        pfree(forDecode);

    return result;
}
// --------------- MVARCHAR ---------------


// --------------- MCHAR ---------------
Datum mcharVersionToInteger(PG_FUNCTION_ARGS) {
    char* convertedChar;
    size_t size;
    text* input;
    Datum result;

    if (PG_ARGISNULL(0))
        PG_RETURN_INT64(0);

    input = PG_GETARG_TEXT_PP(0);
    size = VARSIZE_ANY_EXHDR(input) / sizeof(mvarchar) - LYKOV_SIZE_OF_MCHAR_INFO;
    convertedChar = gl_McharToChar((mvarchar*)VARDATA_ANY(input), size);
    result = convertVersionToBigint(convertedChar, size);

    pfree(convertedChar);
    return result;
}

Datum mcharUIDToRref(PG_FUNCTION_ARGS) {
    bytea* result = palloc0(BYTEA_UID_SIZE + 1 + VARHDRSZ);
    text* data;
    mvarchar* input;

    SET_VARSIZE(result, BYTEA_UID_SIZE + VARHDRSZ);
    if (PG_ARGISNULL(0))
        PG_RETURN_BYTEA_P(result);

    data = PG_GETARG_TEXT_PP(0);
    if (VARSIZE_ANY_EXHDR(data) / sizeof(mvarchar) - LYKOV_SIZE_OF_MCHAR_INFO != UID_SIZE)
        PG_RETURN_BYTEA_P(result);

    input = (mvarchar*)VARDATA_ANY(data);
    pg_snprintf(VARDATA(result),
        BYTEA_UID_SIZE + 1,
        BYTEA_UID,
        gl_GetAsciiFromMvarcharHex(input, 21),
        gl_GetAsciiFromMvarcharHex(input, 23),
        gl_GetAsciiFromMvarcharHex(input, 26),
        gl_GetAsciiFromMvarcharHex(input, 28),
        gl_GetAsciiFromMvarcharHex(input, 30),
        gl_GetAsciiFromMvarcharHex(input, 32),
        gl_GetAsciiFromMvarcharHex(input, 34),
        gl_GetAsciiFromMvarcharHex(input, 36),
        gl_GetAsciiFromMvarcharHex(input, 16),
        gl_GetAsciiFromMvarcharHex(input, 18),
        gl_GetAsciiFromMvarcharHex(input, 11),
        gl_GetAsciiFromMvarcharHex(input, 13),
        gl_GetAsciiFromMvarcharHex(input, 2),
        gl_GetAsciiFromMvarcharHex(input, 4),
        gl_GetAsciiFromMvarcharHex(input, 6),
        gl_GetAsciiFromMvarcharHex(input, 8));

    PG_RETURN_BYTEA_P(result);
}

Datum mcharDDToBytea(PG_FUNCTION_ARGS) {
    text* input;
    Datum result;
    char* forDecode = "";
    size_t srcSize = 0;

    if (!PG_ARGISNULL(0)) {
        input = PG_GETARG_TEXT_PP(0);
        srcSize = VARSIZE_ANY_EXHDR(input) / sizeof(mvarchar) - LYKOV_SIZE_OF_MCHAR_INFO;
        forDecode = gl_McharToChar((mvarchar*)VARDATA_ANY(input), srcSize);
    }

    result = convertCharToByteaDD(forDecode, srcSize);
    if (srcSize > 0)
        pfree(forDecode);

    return result;
}
// --------------- MCHAR ---------------

#pragma endregion 

#pragma region Вспомогательные функции

Datum convertIntToBytea(const int64 src, const unsigned short degree) {
    const int64 maxValue = (int64) pow(256, degree) - 1;
    return gl_ConvertNumericToByteaHex(src < 0 || src > maxValue ? 0 : src, degree);
}

Datum convertIntVersionToText(const int64 input) {
    bytea* converted  = (bytea*)gl_ConvertNumericToByteaHex(input, SIZE_OF_VERSION);
    size_t sizeResult = 0;
    char* encoded     = gl_EncodeBase64FromText(VARDATA_ANY(converted), VARSIZE_ANY_EXHDR(converted), &sizeResult);
    text* result      = palloc0(sizeResult + VARHDRSZ + 1);

    SET_VARSIZE(result, sizeResult + VARHDRSZ);
    strncpy(VARDATA(result), encoded, sizeResult + 1);

    pfree(converted);
    pfree(encoded);

    PG_RETURN_TEXT_P(result);
}

Datum convertVersionToBigint(const char* input, const size_t size) {
    size_t sizeResult = 0;
    int64 result = 0;
    char* decoded;

    decoded = gl_DecodeBase64FromText(input, size, &sizeResult);
    result = (int64)gl_ConvertHexToNumeric(decoded, sizeResult);

    pfree(decoded);
    PG_RETURN_INT64(result);
}

Datum convertCharToByteaDD(char* input, size_t srcSize) {
    char *resultDecode;
    bytea* result;
    size_t dstSize = 0;
    int resultSize = 0;

	srcSize -= gl_RemoveCharactersFromChar(input, " \t\n\r");
    dstSize = pg_b64_dec_len(srcSize);

    resultDecode = palloc0(dstSize);
    resultSize = pg_b64_decode(input, srcSize, resultDecode, dstSize);

    result = palloc0(resultSize + 1 + VARHDRSZ);
    strncpy(VARDATA(result), resultDecode, resultSize + 1);

    if (resultSize >= 0)
        SET_VARSIZE(result, resultSize + VARHDRSZ);
    else {
        elog(WARNING, "Lykov's extension: incorrect value for serialization");
        SET_VARSIZE(result, VARHDRSZ);
    }

    pfree(resultDecode);
    PG_RETURN_BYTEA_P(result);
}

Datum convertValueStorageToText(const char* encoded, const size_t size) {
    Datum forReturn;
    text* result;
    size_t srcLen = size - DD_1C_LENGTH_BULLSHIT,
           resLen = 0,
           sizeResult = 0;

    int decoded = 0;
    char *forDecode = gl_SubStr(encoded, DD_1C_LENGTH_BULLSHIT, srcLen),
	     *resultDecode,
         *forCopy = NULL;

	srcLen      -= gl_RemoveCharactersFromChar(forDecode, " \t\n\r");
    resLen       = pg_b64_dec_len(srcLen);
	resultDecode = palloc0(resLen);
    decoded      = pg_b64_decode(forDecode, srcLen, resultDecode, resLen);

    if (decoded != -1) {
	    forCopy = gl_Concatenate("{\"", resultDecode, 2, decoded);
        sizeResult = decoded + 2 + VARHDRSZ;

        result = palloc0(sizeResult + 1);
        SET_VARSIZE(result, sizeResult);
        strncpy(VARDATA(result), forCopy, decoded + 2);

        forReturn = PointerGetDatum(result);
    } else {
        elog(WARNING, "Lykov's extension: incorrect value storage");
        forReturn = LYKOV_GET_DATUM_CSTRING_WITH_LEN("", 0);
    }
	
    pfree(forDecode);
    pfree(resultDecode);

    if (forCopy)
        pfree(forCopy);
    
    PG_RETURN_DATUM(forReturn);
}

Datum convertDDToTextWithEncoding(const Datum input) {
	Datum encodingName = DirectFunctionCall1(namein, CStringGetDatum(GetDatabaseEncodingName()));
	const Datum result = DirectFunctionCall2(pg_convert_from, input, encodingName);
	pfree((Name)encodingName);

    PG_RETURN_DATUM(result);
}

#pragma endregion
