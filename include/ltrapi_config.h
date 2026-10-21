#ifndef LTRAPI_CONFIG_H_IN
#define LTRAPI_CONFIG_H_IN

/* проверка наличия в системе нужных функций и файлов */
#define LTRAPI_HAVE_STDINT_H   1
#define LTRAPI_HAVE_INTTYPES_H 1
#define LTRAPI_HAVE_INT64      1
#define LTRAPI_HAVE_UINT64     1
/* #undef LTRAPI_HAVE_USLEEP */
#define LTRAPI_HAVE_NANOSLEEP  1

/* #undef LTRAPI_USE_KD_STORESLOTS */

/* #undef __QNX4__ */


#ifdef __QNX4__
    #undef _WIN32
#endif


#endif // LTRAPI_CONFIG_H_IN
