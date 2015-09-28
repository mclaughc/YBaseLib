#pragma once
#include "YBaseLib/Sockets/Common.h"
#include "YBaseLib/Sockets/SocketAddress.h"
#include "YBaseLib/ReferenceCounted.h"
#include "YBaseLib/MemArray.h"
#include "YBaseLib/PODArray.h"
#include "YBaseLib/Thread.h"
#include "YBaseLib/Mutex.h"
#include "YBaseLib/Error.h"

#ifdef Y_SOCKET_IMPLEMENTATION_GENERIC

class BaseSocket;
class ListenSocket;
class StreamSocket;

class SocketMultiplexer : public ReferenceCounted
{
public:
    enum EventType
    {
        EventType_Read      = (1 << 0),
        EventType_Write     = (1 << 1),
        NumEventTypes
    };

    typedef StreamSocket *(*CreateStreamSocketCallback)();
    friend BaseSocket;
    friend ListenSocket;
    friend StreamSocket;

public:
    virtual ~SocketMultiplexer();

    // Access the type of multiplexer
    SOCKET_MULTIPLEXER_TYPE GetType() const { return SOCKET_MULTIPLEXER_TYPE_GENERIC; }

    // Factory method.
    static SocketMultiplexer *Create();

    // Public interface
    template<class T> ListenSocket *CreateListenSocket(const SocketAddress *pAddress, Error *pError);
    template<class T> T *ConnectStreamSocket(const SocketAddress *pAddress, Error *pError);

    // Create worker threads, set threadCount to 0 for implementation-specific value.
    uint32 CreateWorkerThreads(uint32 threadCount = 0);

    // Stop worker threads.
    void StopWorkerThreads();

    // Close all sockets on this multiplexer.
    void CloseAll();

    // Poll for events, set to Y_UINT32_MAX for infinite pause
    void PollEvents(uint32 milliseconds);

protected:
    // Internal interface
    ListenSocket *InternalCreateListenSocket(const SocketAddress *pAddress, CreateStreamSocketCallback callback, Error *pError);
    StreamSocket *InternalConnectStreamSocket(const SocketAddress *pAddress, CreateStreamSocketCallback callback, Error *pError);

private:
    // Hide the constructor.
    SocketMultiplexer();

    // Tracking of open sockets.
    void AddOpenSocket(BaseSocket *pSocket);
    void RemoveOpenSocket(BaseSocket *pSocket);

    // Register for notifications
    void SetNotificationMask(BaseSocket *pSocket, int fileDescriptor, uint32 mask);

private:
    // We store the fd in the struct to avoid the cache miss reading the object.
    struct BoundSocket { BaseSocket *pSocket; uint32 EventMask; int FileDescriptor; };
    MemArray<BoundSocket> m_boundSockets;
    Mutex m_boundSocketLock;

    // Open socket list
    PODArray<BaseSocket *> m_openSockets;
    Mutex m_openSocketLock;

private:
    // Worker thread
    class WorkerThread : public Thread
    {
    public:
        WorkerThread(SocketMultiplexer *pThis);
        virtual int ThreadEntryPoint() override final;
        void Stop();

    private:
        SocketMultiplexer *m_pThis;
        volatile bool m_stopFlag;
    };
    WorkerThread *m_pWorkerThread;
};

template<class T>
ListenSocket *SocketMultiplexer::CreateListenSocket(const SocketAddress *pAddress, Error *pError)
{
    CreateStreamSocketCallback callback = []() -> StreamSocket * { return new T(); }
    return InternalCreateListenSocket(pAddress, callback, pError);
}

template<class T>
T *SocketMultiplexer::ConnectStreamSocket(const SocketAddress *pAddress, Error *pError)
{
    CreateStreamSocketCallback callback = []() -> StreamSocket * { return new T(); }
    return InternalConnectStreamSocket(pAddress, callback, pError);
}

#endif      // Y_SOCKET_IMPLEMENTATION_GENERIC
