
#ifndef __LYKOV_EXT_POSTGRESQL_H__
#define __LYKOV_EXT_POSTGRESQL_H__

#include "globalFunctions.h"

#include <fmgr.h>
PG_MODULE_MAGIC;

#include <math.h>
#include <utils/numeric.h>
#include <mb/pg_wchar.h>

#define BYTEA_UID_SIZE 16
#define DD_1C_LENGTH_BULLSHIT 20
#define SIZE_OF_KEYFIELD_BYTEA 4
#define SIZE_OF_RTREF_BYTEA 4
#define SIZE_OF_TYPE_BYTEA 1
#define SIZE_OF_VERSION 8
#define UID_SIZE 36
#define EMPTY_UID "00000000-0000-0000-0000-000000000000"
#define FORMAT_UID "%s%s%s%s-%s%s-%s%s-%s%s-%s%s%s%s%s%s"
#define BYTEA_UID "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"

void _PG_init(void);

// Разделения по входному типу (так удобнее)

// --------------- SMALLINT, INTEGER, BIGINT ---------------
PG_FUNCTION_INFO_V1(integerTypeToBytea);
PG_FUNCTION_INFO_V1(integerKeyfieldToBytea);
PG_FUNCTION_INFO_V1(integerRTRefToBytea);
PG_FUNCTION_INFO_V1(bigintVersionToText);
// --------------- SMALLINT, INTEGER, BIGINT ---------------

// --------------- NUMERIC ---------------
PG_FUNCTION_INFO_V1(numericTypeToBytea);
PG_FUNCTION_INFO_V1(numericKeyfieldToBytea);
PG_FUNCTION_INFO_V1(numericRTRefToBytea);
PG_FUNCTION_INFO_V1(numericVersionToText);
// --------------- NUMERIC ---------------

// --------------- BYTEA ---------------
PG_FUNCTION_INFO_V1(byteaTypeToShort);
PG_FUNCTION_INFO_V1(byteaKeyfieldToInteger);
PG_FUNCTION_INFO_V1(byteaRTRefToInteger);
PG_FUNCTION_INFO_V1(byteaRrefToUID);
PG_FUNCTION_INFO_V1(byteaToDD);
PG_FUNCTION_INFO_V1(convertByteaDDToText);
// --------------- BYTEA ---------------

// --------------- TEXT ---------------
PG_FUNCTION_INFO_V1(textVersionToInteger);
PG_FUNCTION_INFO_V1(textUIDToRref);
PG_FUNCTION_INFO_V1(textDDToBytea);
// --------------- TEXT ---------------

// --------------- MVARCHAR ---------------
PG_FUNCTION_INFO_V1(mvarcharVersionToInteger);
PG_FUNCTION_INFO_V1(mvarcharUIDToRref);
PG_FUNCTION_INFO_V1(mvarcharDDToBytea);
// --------------- MVARCHAR ---------------

// --------------- MCHAR ---------------
PG_FUNCTION_INFO_V1(mcharVersionToInteger);
PG_FUNCTION_INFO_V1(mcharUIDToRref);
PG_FUNCTION_INFO_V1(mcharDDToBytea);
// --------------- MCHAR ---------------

Datum convertIntToBytea(int64, unsigned short);
Datum convertIntVersionToText(int64);
Datum convertVersionToBigint(const char*, size_t);
Datum convertCharToByteaDD(const char*, size_t);
Datum convertValueStorageToText(const char*, size_t);
Datum convertDDToTextWithEncoding(Datum);

#endif // __LYKOV_EXT_POSTGRESQL_H__
