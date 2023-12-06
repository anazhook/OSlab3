#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

CRITICAL_SECTION cs;
HANDLE* hStartMarker;
HANDLE* hMarkerCantResume;
HANDLE* hMarkerRestart;
HANDLE* hMarkerStop;
HANDLE hMarkerStopped;

int* Array;
int arraysize;

void ThreadMarker(LPVOID param)
{
	int number = (int)param;

	WaitForMultipleObjects(arraysize,hStartMarker, TRUE, INFINITE);
	ResetEvent(hStartMarker[number - 1]);

	srand(number);
	int count = 0;
	std::vector<int> changedIndexes;

	while (true)
	{
		int randomNuber = std::rand();
		int modul = randomNuber % arraysize;
		EnterCriticalSection(&cs);
		if (Array[modul] == 0)
		{
			Sleep(5);
			Array[modul] = number;
			changedIndexes.push_back(modul);
			LeaveCriticalSection(&cs);
			Sleep(5);
			count++;
		}
		else
		{
			LeaveCriticalSection(&cs);
			std::cout << "\nNumber of the thread: " << number << "\n";
			std::cout << "Number of marked elements: " << count << "\n";
			std::cout << "Index of impossible to mark element: " << modul << "\n";

			HANDLE events[2] = { hMarkerRestart[number - 1], hMarkerStop[number - 1] };
			SetEvent(hMarkerCantResume[number - 1]);
			DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			{
				if (result == 0)
				{
					ResetEvent(hMarkerRestart[number - 1]);
				}
				else if (result == 1)
				{
					for (int i = 0; i < changedIndexes.size(); i++)
					{
						EnterCriticalSection(&cs);
						Array[changedIndexes[i]] = 0;
						LeaveCriticalSection(&cs);
					}
					ResetEvent(hMarkerStop[number - 1]);
					SetEvent(hMarkerStopped);
					changedIndexes.clear();
					return;
				}
				else
				{
					LeaveCriticalSection(&cs);
					std::cerr << "Wrong Response\n";
					return;
				}
			}
		}
	}


}

int main()
{
	//int arraysize
	std::cout << "Size of array: ";
	std::cin >> arraysize;

	//arraysize = arraysize
	Array = new int[arraysize];
	for (int i = 0; i < arraysize; i++)
		Array[i] = 0;

	InitializeCriticalSection(&cs);
	int threadsAmount;
	std::cout << "number of marker threads: ";
	std::cin >> threadsAmount;

	HANDLE* hThread = new HANDLE[threadsAmount];
	DWORD* dwThread = new DWORD[threadsAmount];
	hMarkerRestart = new HANDLE[threadsAmount];
	hMarkerStop = new HANDLE[threadsAmount];
	hMarkerCantResume = new HANDLE[threadsAmount];
	hStartMarker = new HANDLE[threadsAmount];
	int* indexes = new int[threadsAmount];


	for (int i = 0; i < threadsAmount; i++)
	{
		indexes[i] = i + 1;
	}

	for (int i = 0; i < threadsAmount; i++)
	{
		hMarkerRestart[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hMarkerStop[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hMarkerCantResume[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		hStartMarker[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	hMarkerStopped = CreateEvent(NULL, FALSE, FALSE, NULL);

	std::vector<int> Markers;
	for (int i = 0; i < threadsAmount; i++)
		Markers.push_back(i);

	for (int i = 0; i < threadsAmount; i++)
	{
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadMarker, (void*)indexes[i], 0, &dwThread[i]);
	}



	for (int i = 0; i < threadsAmount; i++)
	{
		SetEvent(hStartMarker[i]);
	}


	while (true)
	{
		for (int i = 0; i < Markers.size(); i++)
			if (Markers[i] == -1)
				SetEvent(hMarkerCantResume[i]);
		if (WaitForMultipleObjects(threadsAmount, hMarkerCantResume, TRUE, INFINITE) == WAIT_FAILED)
		{
			std::cout << "Wait for multiple objects failed." << std::endl;
			std::cout << "Press any key to exit." << std::endl;
		}
		for (int i = 0; i < Markers.size(); i++)
			ResetEvent(hMarkerCantResume[i]);
		std::cout << "Array after Markers' work:\n";
		for (int i = 0; i < arraysize; i++)
		{
			std::cout << Array[i] << " ";
		}
		std::cout << "\n";
		
		std::cout << "working markers: \n";
		for (int i = 0; i < Markers.size(); i++)
			if (Markers[i] != -1)
				std::cout << Markers[i] << " ";
		std::cout << "\n";
		std::cout << "Enter the index of the thread to be stopped:\n";
		int stoppedThread;
		std::cin >> stoppedThread;

		for (int i = 0; i < Markers.size(); i++)
		{
			if (Markers[i] == stoppedThread)
				Markers[i] = -1;
		}

		SetEvent(hMarkerStop[stoppedThread]);
		WaitForSingleObject(hMarkerStopped, INFINITE);
		ResetEvent(hMarkerStopped);


		std::cout << "Array after stopping " << stoppedThread <<"th thread: \n";
		for (int i = 0; i < arraysize; i++)
		{
			std::cout << Array[i] << " ";
		}
		std::cout << "\n";

		bool flag = false;
		for (int i = 0; i < Markers.size(); i++)
		{
			if (Markers[i] != -1)
				flag = true;
		}

		if (flag == false)
		{
			std::cout << "All Markers are closed\n";
			for (int i = 0; i < threadsAmount; i++)
			{
				std::cout << "Closing handle " << i << "\n";
				CloseHandle(hThread[i]);
			}
			DeleteCriticalSection(&cs);
			return 0;
		}

		for (int i = 0; i < Markers.size(); i++)
		{
			if (Markers[i] != -1)
			{
				SetEvent(hMarkerRestart[i]);
			}
		}
	}
}
