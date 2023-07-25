﻿// LykovExtPostgreSQL.cpp: определяет точку входа для приложения.
//

#include "lykovExtPostgreSQL.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(add_ab);
Datum add_ab(PG_FUNCTION_ARGS)
{
    int32 arg_a = PG_GETARG_INT32(0);
    int32 arg_b = PG_GETARG_INT32(1);
    PG_RETURN_INT32(arg_a + arg_b);
}