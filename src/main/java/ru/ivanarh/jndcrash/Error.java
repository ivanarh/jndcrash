package ru.ivanarh.jndcrash;

/**
 * Error status. Matches ndcrash_error enum values in ndcrash.h.
 */
public enum Error {
    ok,
    error_already_initialized,
    error_not_supported,
    error_signal,
    error_pipe,
    error_thread,
}