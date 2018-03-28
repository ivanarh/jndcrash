package ru.ivanarh.jndcrash;

/**
 * Unwinder type. Matches ndcrash_unwinder values in ndcrash.h.
 */
public enum Unwinder {
    libcorkscrew,
    libunwind,
    libunwindstack,
    cxxabi,
    stackscan,
}