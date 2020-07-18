#include <stdio.h>
#include <windows.h>
#include "..\include\\windows_sleep.h"

// Sleep function for windows with 100ns resolution.
int nanosleep_windows(LONGLONG ns, HANDLE timer)
{
	LARGE_INTEGER li;

	// Set timer properties.
	li.QuadPart = -ns;

	// Start timer.
	if(!SetWaitableTimer(timer, &li, 0, NULL, NULL, 0)) {
		printf("SetWaitableTimer failed (%lu)\n", GetLastError());
		CloseHandle(timer);
		return -1;
	}

	// Wait for timer.
	if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0) {
		printf("WaitForSingleObject failed (%lu)\n", GetLastError());
		CloseHandle(timer);
		return -1;
	}

	return 0;
}
