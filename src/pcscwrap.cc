#include "pcscwrap.h"
#include <string.h>


SCARDCONTEXT		hContext; // Resource manager handle
SCARD_READERSTATE   hReaderState;
SCARDHANDLE			hCard;
DWORD				dwActiveProtocol;      // Active protocol (T=0).
char				szSelectedReader[256]; // reader name

LONG pcsc_init()
{
	LONG ret = SCARD_S_SUCCESS;

	ret = SCardEstablishContext(
					SCARD_SCOPE_USER, // Scope of the resource manager context.
					NULL,             // r.f.u
					NULL,             // r.f.u
					&hContext);	 // Returns the resource manager handle.

	return ret;
}

LONG pcsc_release()
{
	LONG ret = SCARD_S_SUCCESS;

	ret = SCardReleaseContext(hContext);

	return ret;
}

DWORD pcsc_get_readers_name_length()
{

    LONG ret = SCARD_S_SUCCESS;
    DWORD readers_name_length;
    ret = SCardListReaders(hContext, NULL, NULL, &readers_name_length);
    if (ret == SCARD_S_SUCCESS) {
        return readers_name_length;
    } else {
        return 0;
    }
}

LONG pcsc_get_readers_name(char* readers_name, DWORD readers_name_length)
{
    LONG ret = SCARD_S_SUCCESS;

    ret = SCardListReaders(hContext, NULL, readers_name, &readers_name_length);
    //first reader is default reader
    //strcpy_s(szSelectedReader, readers_name);
    strcpy(szSelectedReader, readers_name);

    return ret;
}

void pcsc_set_reader_name(const char *reader_name)
{
    //printf("reader_name: %s", reader_name);
    //strcpy_s(szSelectedReader, reader_name);
    strcpy(szSelectedReader, reader_name);
}

char* pcsc_get_reader_list()
{
	LONG ret = SCARD_S_SUCCESS;
	LPTSTR readerName = NULL;
	DWORD dw = SCARD_AUTOALLOCATE;
	// The SCardListReaders function provides the list of readers
	ret = SCardListReaders(
                    hContext,             // Resource manager handle. 
                    NULL,                 // NULL: list all readers in the system
                    (LPTSTR) &readerName, // for reader len
                    &dw );
	// There is only one reader connected.
	//strcpy_s(szSelectedReader, readerName);
	strcpy(szSelectedReader, readerName);
	//printf("selectedReader %s\n", szSelectedReader);

	SCardFreeMemory(hContext, readerName);
	return szSelectedReader;
}

LONG pcsc_connect(byte* atr, LPDWORD atrLen)
{
	LONG ret = SCARD_S_SUCCESS;

    //Establishes a connection to a smart card contained by a specific reader.
	ret = SCardReconnect(hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL, SCARD_UNPOWER_CARD, &dwActiveProtocol);

	if(ret != SCARD_S_SUCCESS) {
        ret = SCardConnect(
                        hContext, // Resource manager handle.
                        szSelectedReader,     // Reader name.
                        SCARD_SHARE_SHARED,  // Share Mode.
                        SCARD_PROTOCOL,
                        &hCard,               // Returns the card handle.
                        &dwActiveProtocol);   // Active protocol.


        if(ret != SCARD_S_SUCCESS) {
            return ret;
        }
	}

	ret = SCardGetAttrib(hCard, // Card handle.
						SCARD_ATTR_ATR_STRING,// Attribute identifier.
						atr,  // Attribute buffer.
						atrLen);          // Returned attribute length.
	
	if(ret != SCARD_S_SUCCESS) {
		return ret;
	}

	return ret;
}

LONG pcsc_transmit(byte* input, DWORD inputLen, byte* output, LPDWORD outputLen)
{
	LPCSCARD_IO_REQUEST  ioRequest;
	LONG ret = SCARD_S_SUCCESS;

	switch (dwActiveProtocol)
	{
		case SCARD_PROTOCOL_T0:
			ioRequest = SCARD_PCI_T0;
			break;

		case SCARD_PROTOCOL_T1:
			ioRequest = SCARD_PCI_T1;
			break;

		default:
			ioRequest = SCARD_PCI_RAW;
			break;
	}

	*outputLen = 384;

	// APDU exchange.
	ret = SCardTransmit(hCard,			// Card handle.
						ioRequest,		// Pointer to the send protocol header.
						input,			// Send buffer.
						inputLen,		// Send buffer length.
						NULL,			// Pointer to the rec. protocol header.
						output, 
						outputLen);

	return 0;
}

LONG pcsc_disconnect()
{
    LONG ret = SCARD_S_SUCCESS;
	ret = SCardDisconnect(hCard, SCARD_EJECT_CARD);
	return ret;
}
