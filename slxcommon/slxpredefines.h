#ifndef _SLXPREDEFINES_H
#define _SLXPREDEFINES_H

#define TIMEFMT_YMDA                    "%04u-%02u-%02u"
#define TIMEFMT_HMSA                    "%02u:%02u:%02u"
#define TIMEFMT_HMSMA                   "%02u:%02u:%02u.%03u"
#define TIMEFMT_YMDHMSA                 TIMEFMT_YMDA " " TIMEFMT_HMSA
#define TIMEFMT_YMDHMSMA                TIMEFMT_YMDA " " TIMEFMT_HMSMA
#define TIMEFMT2_YMDA                   "%04u%02u%02u"
#define TIMEFMT2_HMSA                   "%02u%02u%02u"
#define TIMEFMT2_HMSMA                  "%02u%02u%02u%03u"
#define TIMEFMT2_YMDHMSA                TIMEFMT2_YMDA TIMEFMT2_HMSA
#define TIMEFMT2_YMDHMSMA               TIMEFMT2_YMDA TIMEFMT2_HMSMA

#define TIMEFMT_YMDW                    L"%04u-%02u-%02u"
#define TIMEFMT_HMSW                    L"%02u:%02u:%02u"
#define TIMEFMT_HMSMW                   L"%02u:%02u:%02u.%03u"
#define TIMEFMT_YMDHMSW                 TIMEFMT_YMDW L" " TIMEFMT_HMSW
#define TIMEFMT_YMDHMSMW                TIMEFMT_YMDW L" " TIMEFMT_HMSMW
#define TIMEFMT2_YMDW                   L"%04u%02u%02u"
#define TIMEFMT2_HMSW                   L"%02u%02u%02u"
#define TIMEFMT2_HMSMW                  L"%02u%02u%02u%03u"
#define TIMEFMT2_YMDHMSW                TIMEFMT2_YMDW TIMEFMT2_HMSW
#define TIMEFMT2_YMDHMSMW               TIMEFMT2_YMDW TIMEFMT2_HMSMW

#define TIMEEXP_YMD(st)                 (st).wYear, (st).wMonth, (st).wDay
#define TIMEEXP_HMS(st)                 (st).wHour, (st).wMinute, (st).wSecond
#define TIMEEXP_HMSM(st)                TIMEEXP_HMS(st), (st).wMilliseconds
#define TIMEEXP_YMDHMS(st)              TIMEEXP_YMD(st), TIMEEXP_HMS(st)
#define TIMEEXP_YMDHMSM(st)             TIMEEXP_YMD(st), TIMEEXP_HMSM(st)

#define TIMEEXP_YMDP(st)                (st)->wYear, (st)->wMonth, (st)->wDay
#define TIMEEXP_HMSP(st)                (st)->wHour, (st)->wMinute, (st)->wSecond
#define TIMEEXP_HMSMP(st)               TIMEEXP_HMS(st), (st)->wMilliseconds
#define TIMEEXP_YMDHMSP(st)             TIMEEXP_YMD(st), TIMEEXP_HMS(st)
#define TIMEEXP_YMDHMSMP(st)            TIMEEXP_YMD(st), TIMEEXP_HMSM(st)

#if defined(UNICODE) || defined(_UNICODE)
    #define TIMEFMT_YMD                 TIMEFMT_YMDW
    #define TIMEFMT_HMS                 TIMEFMT_HMSW
    #define TIMEFMT_HMSM                TIMEFMT_HMSMW
    #define TIMEFMT_YMDHMS              TIMEFMT_YMDHMSW
    #define TIMEFMT_YMDHMSM             TIMEFMT_YMDHMSMW
    #define TIMEFMT2_YMD                TIMEFMT2_YMDW
    #define TIMEFMT2_HMS                TIMEFMT2_HMSW
    #define TIMEFMT2_HMSM               TIMEFMT2_HMSMW
    #define TIMEFMT2_YMDHMS             TIMEFMT2_YMDHMSW
    #define TIMEFMT2_YMDHMSM            TIMEFMT2_YMDHMSMW
#else
    #define TIMEFMT_YMD                 TIMEFMT_YMDA
    #define TIMEFMT_HMS                 TIMEFMT_HMSA
    #define TIMEFMT_HMSM                TIMEFMT_HMSMA
    #define TIMEFMT_YMDHMS              TIMEFMT_YMDHMSA
    #define TIMEFMT_YMDHMSM             TIMEFMT_YMDHMSMA
    #define TIMEFMT2_YMD                TIMEFMT2_YMDA
    #define TIMEFMT2_HMS                TIMEFMT2_HMSA
    #define TIMEFMT2_HMSM               TIMEFMT2_HMSMA
    #define TIMEFMT2_YMDHMS             TIMEFMT2_YMDHMSA
    #define TIMEFMT2_YMDHMSM            TIMEFMT2_YMDHMSMA
#endif

#endif /* _SLXPREDEFINES_H */
