#ifndef WINDOWS_SLEEP_H_
#define WINDOWS_SLEEP_H_

#include <windows.h>

int nanosleep_windows(LONGLONG ns, HANDLE timer);

#endif // WINDOWS_SLEEP_H_
