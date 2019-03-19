#ifndef _LOGGER_H_
#define _LOGGER_H_

/**
 * Various logger levels.
 */
typedef enum {
    LL_TRACE,
    LL_INFO,
    LL_WARN,
    LL_ERROR,
    LL_FATAL
} LoggerLevels;

/**
 * Various logger options that can be ORed together when setting a particular
 * logger level's options.
 */
typedef enum {
    LO_NOTHING = 0x0000,
    LO_NOCOLOR = 0x0001,
    LO_NOLABEL = 0x0002
} LoggerOptions;

extern const char* LC_NORMAL;
extern const char* LC_RESET;
extern const char* LC_RED;
extern const char* LC_RED_BOLD;
extern const char* LC_GREEN;
extern const char* LC_GREEN_BOLD;
extern const char* LC_YELLOW;
extern const char* LC_YELLOW_BOLD;
extern const char* LC_BLUE;
extern const char* LC_BLUE_BOLD;
extern const char* LC_MAGENTA;
extern const char* LC_MAGENTA_BOLD;
extern const char* LC_CYAN;
extern const char* LC_CYAN_BOLD;

void output(const char* color, const char* message, ...);
void trace(const char* message, ...);
void info(const char* message, ...);
void warn(const char* message, ...);
void error(const char* message, ...);
void fatal(const char* message, ...);
void setLoggerOptions(LoggerLevels level, int options);

#endif