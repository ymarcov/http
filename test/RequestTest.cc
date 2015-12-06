#include <gmock/gmock.h>

#include "Request.h"
#include "MemoryPool.h"

#include <cstring>
#include <memory>
#include <string>

using namespace ::testing;

namespace Yam {
namespace Http {

const char requestData[] =
"GET /path/to/res HTTP/1.1\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
"Accept-encoding: gzip, deflate\r\n"
"Accept-language: en-US,en;q=0.5\r\n"
"Connection: close\r\n"
"Host: request.urih.com\r\n"
"Referer: http://www.google.com/?url=http%3A%2F%2Frequest.urih.com\r\n"
"User-agent: Mozilla/5.0 (X11; Linux x86_64; rv:31.0) Gecko/20100101 Firefox/31.0 Iceweasel/31.8.0\r\n"
"Cookie: Session=abcd1234; User=Yam\r\n"
"X-http-proto: HTTP/1.1\r\n"
"X-log-7527: 95.35.33.46\r\n"
"X-real-ip: 95.35.33.46\r\n"
"Content-Length: 13\r\n"
"\r\n"
"Request body!";

class StringInputStream : public InputStream {
public:
    StringInputStream(const std::string& str) :
        _str(str) {}

    std::size_t Read(void* buffer, std::size_t bufferSize) override {
        auto remaining = _str.size() - _position;
        auto readBytes = std::min(remaining, bufferSize);
        std::memcpy(buffer, _str.data() + _position, readBytes);
        _position += readBytes;
    }

private:
    std::string _str;
    std::size_t _position = 0;
};

class RequestTest : public Test {
public:
    RequestTest() :
        _memoryPool{MemoryPool<Request::Buffer>::Create()} {}

protected:
    std::unique_ptr<Request> MakeRequest() {
        auto str = std::string{requestData, sizeof(requestData)};
        auto s = std::make_shared<StringInputStream>(str);
        return std::make_unique<Request>(_memoryPool->New(), std::move(s));
    }

    std::shared_ptr<MemoryPool<Request::Buffer>> _memoryPool;
};

TEST_F(RequestTest, header_getters) {
    auto r = MakeRequest();

    EXPECT_EQ(Protocol::Method::Get, r->GetMethod());
    EXPECT_EQ(Protocol::Version::Http11, r->GetProtocol());
    EXPECT_EQ("/path/to/res", r->GetUri());
    EXPECT_EQ("request.urih.com", r->GetField("Host"));
    EXPECT_EQ("abcd1234", r->GetCookie("Session"));
    EXPECT_EQ(2, r->GetCookieNames().size());
    EXPECT_EQ(13, r->GetContentLength());
}

TEST_F(RequestTest, body) {
    auto r = MakeRequest();

    char buffer[0x1000];
    auto bytesRead = r->ReadNextBodyChunk(buffer, sizeof(buffer));
    EXPECT_EQ("Request body!", std::string(buffer, bytesRead));
}

} // namespace Http
} // namespace Yam

