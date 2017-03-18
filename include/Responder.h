#pragma once

#include "OutputStream.h"
#include "Protocol.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Yam {
namespace Http {

class CachedResponse {
    Status _status;
    bool _keepAlive = true;
    std::string _header;
    std::shared_ptr<std::vector<char>> _body;
    friend class Responder;
};

class Responder {
public:
    Responder() = default;
    Responder(std::shared_ptr<OutputStream>);

    void Send(Status);
    void SendCached(std::shared_ptr<CachedResponse>);
    std::shared_ptr<CachedResponse> CacheAs(Status);
    bool GetKeepAlive() const;
    void SetExplicitKeepAlive(bool);
    void SetField(std::string name, std::string value);
    void SetCookie(std::string name, std::string value);
    void SetCookie(std::string name, std::string value, const CookieOptions&);
    void SetBody(std::shared_ptr<std::vector<char>>);

    /**
     * Writes response data to the output stream.
     * Returns whether the operation completed, and how many bytes were written.
     */
    std::pair<bool, std::size_t> Flush(std::size_t maxBytes);

    /**
     * Gets the status that was requested to be sent.
     */
    Status GetStatus() const;

private:
    void Prepare(Status);
    CachedResponse& GetResponse() const;

    std::shared_ptr<OutputStream> _stream;
    bool _prepared = false;
    std::vector<std::pair<std::string, std::string>> _fields;
    mutable std::shared_ptr<CachedResponse> _response;
    std::size_t _writePosition = 0;
};

} // namespace Http
} // namespace Yam

