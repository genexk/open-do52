#pragma once

#include "quantum.h"

/*
 * LAYOUT macro for the Open DO52.
 *
 * Physical layout (both halves read left-to-right):
 *
 * Left half:                              Right half:
 * L00 L01 L02 L03 L04 L05      R05 R04 R03 R02 R01 R00
 * L10 L11 L12 L13 L14 L15      R15 R14 R13 R12 R11 R10
 * L20 L21 L22 L23 L24 L25      R25 R24 R23 R22 R21 R20
 * L30 L31 L32 L33 L34 L35      R35 R34 R33 R32 R31 R30
 *     L40 L41 L42 L43 L44 L45  R45 R44 R43 R42 R41 R40  (DPAD)
 * L50 L51 L52 L53 L54 L55      R55 R54 R53 R52 R51 R50
 *
 * Matrix stores right-half columns reversed: R00 is rightmost physical key
 * mapped to matrix column 0. Rxy parameters are in PHYSICAL order
 * (left-to-right), macro reverses them into matrix order.
 */
#define LAYOUT( \
    L00, L01, L02, L03, L04, L05,      R05, R04, R03, R02, R01, R00, \
    L10, L11, L12, L13, L14, L15,      R15, R14, R13, R12, R11, R10, \
    L20, L21, L22, L23, L24, L25,      R25, R24, R23, R22, R21, R20, \
    L30, L31, L32, L33, L34, L35,      R35, R34, R33, R32, R31, R30, \
    L40, L41, L42, L43, L44, L45,      R45, R44, R43, R42, R41, R40, \
    L50, L51, L52, L53, L54, L55,      R55, R54, R53, R52, R51, R50  \
) { \
    { L00, L01, L02, L03, L04, L05 }, \
    { L10, L11, L12, L13, L14, L15 }, \
    { L20, L21, L22, L23, L24, L25 }, \
    { L30, L31, L32, L33, L34, L35 }, \
    { L40, L41, L42, L43, L44, L45 }, \
    { L50, L51, L52, L53, L54, L55 }, \
    { R00, R01, R02, R03, R04, R05 }, \
    { R10, R11, R12, R13, R14, R15 }, \
    { R20, R21, R22, R23, R24, R25 }, \
    { R30, R31, R32, R33, R34, R35 }, \
    { R40, R41, R42, R43, R44, R45 }, \
    { R50, R51, R52, R53, R54, R55 }  \
}
