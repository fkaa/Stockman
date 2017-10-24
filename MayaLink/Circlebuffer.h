#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <mutex>
#include "Mutex.h"
#include <iostream>

class CircleBuffer
{
private:
	struct FileHeader 
	{
		size_t id;
		size_t length;
		size_t padding;
		size_t clients;
	};

	struct ControlHeader
	{
		size_t head;
		size_t tail;
		size_t clientCount;
	};

	enum
	{
		HEAD,
		TAIL,
		CLIENTCOUNT
	};

#pragma region Variables
	Mutex mutex;

	HANDLE fileMap, fileControl;
	HANDLE memMtx, clientMtx, readMtx;

	size_t buff_size, chnk_size, msg_Count, client_Pos;
	size_t *_head, *_tail, *_clients, *_freemem;

	LPCWSTR buffName;

	bool isProducer;

	char* messageData;
	size_t * controlPointer;

	size_t localTail;
	size_t msg_Id;
#pragma endregion


	size_t round_Up(size_t val, size_t multiple = 256);
	void createMaps();
	void updateControlBuffer();
public:
	CircleBuffer(LPCWSTR buffName,		// unique buffname
		const size_t & buffSize,		// the size of the whole filemap
		const bool &isProducer,			// if its going to be used as a producer or not
		const size_t&chunkSize);		// round up message to multiple of this <-
	~CircleBuffer();

	size_t calcFreeMemory();

	bool push(const void*msg, size_t length); // Sends a message through buffer, if return succeed if false not enough space.
	bool pop(char*msg, size_t&length);        //read a message from buffer, puts content in memory. 
	
	size_t freeMem();

	size_t fileHeaderSize = sizeof(FileHeader);

};
