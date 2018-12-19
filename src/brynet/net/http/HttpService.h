﻿#pragma once

#include <memory>

#include <brynet/net/TCPService.h>
#include <brynet/net/http/HttpParser.h>
#include <brynet/utils/NonCopyable.h>
#include <brynet/net/http/WebSocketFormat.h>
#include <brynet/net/ListenThread.h>
#include <brynet/net/Any.h>

namespace brynet { namespace net { namespace http {

    class HttpService;

    class HttpSession : public utils::NonCopyable
    {
    public:
        typedef std::shared_ptr<HttpSession>    PTR;

        typedef std::function < void(const HttpSession::PTR&) > ENTER_CALLBACK;
        typedef std::function < void(const HTTPParser&, const HttpSession::PTR&) > HTTPPARSER_CALLBACK;
        typedef std::function < void(const HttpSession::PTR&,
            WebSocketFormat::WebSocketFrameType opcode,
            const std::string& payload) > WS_CALLBACK;

        typedef std::function < void(const HttpSession::PTR&) > CLOSE_CALLBACK;
        typedef std::function < void(const HttpSession::PTR&, const HTTPParser&) > WS_CONNECTED_CALLBACK;

    public:
        void                    setHttpCallback(HTTPPARSER_CALLBACK callback);
        void                    setCloseCallback(CLOSE_CALLBACK callback);
        void                    setWSCallback(WS_CALLBACK callback);
        void                    setWSConnected(WS_CONNECTED_CALLBACK callback);

        void                    send(const DataSocket::PACKET_PTR& packet,
            const DataSocket::PACKED_SENDED_CALLBACK& callback = nullptr);
        void                    send(const char* packet,
            size_t len,
            const DataSocket::PACKED_SENDED_CALLBACK& callback = nullptr);

        void                    postShutdown() const;
        void                    postClose() const;

        const BrynetAny&        getUD() const;
        void                    setUD(BrynetAny);

    protected:
        explicit HttpSession(DataSocket::PTR);
        virtual ~HttpSession() = default;

        static  PTR             Create(DataSocket::PTR session);

        DataSocket::PTR&        getSession();

        const HTTPPARSER_CALLBACK&      getHttpCallback();
        const CLOSE_CALLBACK&           getCloseCallback();
        const WS_CALLBACK&              getWSCallback();
        const WS_CONNECTED_CALLBACK&    getWSConnectedCallback();

    private:
        DataSocket::PTR         mSession;
        BrynetAny               mUD;
        HTTPPARSER_CALLBACK     mHttpRequestCallback;
        WS_CALLBACK             mWSCallback;
        CLOSE_CALLBACK          mCloseCallback;
        WS_CONNECTED_CALLBACK   mWSConnectedCallback;

        friend class HttpService;
    };

    class HttpService
    {
    public:
        static void setup(const DataSocket::PTR& session, const HttpSession::ENTER_CALLBACK& enterCallback);

    private:
        static void handle(const HttpSession::PTR& httpSession);

        static size_t ProcessWebSocket(const char* buffer,
            size_t len,
            const HTTPParser::PTR& httpParser,
            const HttpSession::PTR& httpSession);

        static size_t ProcessHttp(const char* buffer,
            size_t len,
            const HTTPParser::PTR& httpParser,
            const HttpSession::PTR& httpSession);
    };

} } }