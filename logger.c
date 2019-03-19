#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* LC_RESET =          "\x1b[0m";
const char* LC_NORMAL =         "\x1b[0m";
const char* LC_RED =            "\x1b[0;31m";
const char* LC_RED_BOLD =       "\x1b[1;31m";
const char* LC_GREEN =          "\x1b[0;32m";
const char* LC_GREEN_BOLD =     "\x1b[1;32m";
const char* LC_YELLOW =         "\x1b[0;33m";
const char* LC_YELLOW_BOLD =    "\x1b[1;33m";
const char* LC_BLUE =           "\x1b[0;34m";
const char* LC_BLUE_BOLD =      "\x1b[1;34m";
const char* LC_MAGENTA =        "\x1b[0;35m";
const char* LC_MAGENTA_BOLD =   "\x1b[1;35m";
const char* LC_CYAN =           "\x1b[0;36m";
const char* LC_CYAN_BOLD =      "\x1b[1;36m";

static int loggerOptions[5] = {
    LO_NOTHING,
    LO_NOTHING,
    LO_NOTHING,
    LO_NOTHING,
    LO_NOTHING
};

static void vlog(LoggerLevels level, const char* message, va_list args);

/**
 * Outputs the provided message, in the provided color, to the known descriptor.
 * Unlike all of the other logger functions, this does not add a newline
 * character to the end prior to outputting it.
 */
void output(const char* color, const char* message, ...) {
    va_list args;
    char buffer_char[4096] = { 0 };

    if (color != NULL) {
        strcat(buffer_char, color);
    }
    strcat(buffer_char, message);
    strcat(buffer_char, LC_RESET);

    va_start(args, message);
    vprintf(buffer_char, args);
    va_end(args);
}

/**
 * Logs the provided trace-level message.
 */
void trace(const char* message, ...) {
    va_list args;

    va_start(args, message);
    vlog(LL_TRACE, message, args);
    va_end(args);
}

/**
 * Logs the provided info-level message.
 */
void info(const char* message, ...) {
    va_list args;

    va_start(args, message);
    vlog(LL_INFO, message, args);
    va_end(args);
}

/**
 * Logs the provided warning-level message.
 */
void warn(const char* message, ...) {
    va_list args;

    va_start(args, message);
    vlog(LL_WARN, message, args);
    va_end(args);
}

/**
 * Logs the provided error-level message.
 */
void error(const char* message, ...) {
    va_list args;

    va_start(args, message);
    vlog(LL_ERROR, message, args);
    va_end(args);
}

/**
 * Sets the provided option flags (available via the LoggerOptions enum) for the
 * specified log level.
 */
void setLoggerOptions(LoggerLevels level, int options) {
    loggerOptions[level] = options;
}

/**
 * Logs the provided fatal-level message and then kills the program.
 */
void fatal(const char* message, ...) {
    va_list args;

    va_start(args, message);
    vlog(LL_FATAL, message, args);
    va_end(args);

    exit(1);
}

/**
 * Logs the provided messages as the provided log level. Supports all formatting
 * provided by printf(...) calls. Accepts a varargs list instead of actually
 * supporting varargs.
 */
static void vlog(LoggerLevels level, const char* message, va_list args) {
    char buffer_char[4096] = { 0 };

    // Add color formatters to the begginning of the buffer if necessary
    if (!(loggerOptions[level] & LO_NOCOLOR)) {
        switch (level) {
            case LL_FATAL:
                strcat(buffer_char, LC_RED_BOLD);
                break;
            case LL_ERROR:
                strcat(buffer_char, LC_RED);
                break;
            case LL_WARN:
                strcat(buffer_char, LC_YELLOW);
                break;
            case LL_INFO:
                strcat(buffer_char, LC_CYAN);
                break;
            default:
                break;
        }
    }

    // Add the name of the log level to the buffer
    if (!(loggerOptions[level] & LO_NOLABEL)) {
        switch (level) {
            case LL_TRACE:
                strcat(buffer_char, "TRACE: ");
                break;
            case LL_INFO:
                strcat(buffer_char, "INFO: ");
                break;
            case LL_WARN:
                strcat(buffer_char, "WARN: ");
                break;
            case LL_ERROR:
                strcat(buffer_char, "ERROR: ");
                break;
            case LL_FATAL:
                strcat(buffer_char, "FATAL: ");
                break;
            default:
                break;
        }
    }

    // Add the actual message formatter string
    strcat(buffer_char, message);

    // Turn off the color if it was turned on previously
    if (!(loggerOptions[level] & LO_NOCOLOR)) {
        strcat(buffer_char, LC_RESET);
    }

    // Add a newline
    strcat(buffer_char, "\n");
    
    vprintf(buffer_char, args);
}