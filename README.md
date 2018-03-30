![Logo](https://raw.githubusercontent.com/ymarcov/Chili/async/logo.png)

# Chili: HTTP Served Hot

***An efficient, simple to use, HTTP server library in modern C++***
This is a library for developing HTTP servers with custom request processing. \
It uses asynchronous I/O operations, relying on *epoll* (so it is Linux-only), and reaches some very good numbers in terms of performance. At the same time, it's very easy to set up, as shown below.

It has a light footprint, employs modern C++ constructs, and compiles well with no warnings under Clang 5 with `-std=c++17`.

### Feature Summary

- Asynchronous I/O
- Independent Read/Write throttling, both for the server as a whole and for individual connections
- Supports streaming
- Supports conditional message body fetching & rejection
- Supports cached responses for added efficiency
- Efficient, does not overload the CPU or memory when not required
- Uses modern C++ and is easy to use, and even to customize the code
- Public API documented with Doxygen
- Thoroughly tested with Google Test unit & integration tests, and also with Valgrind Memcheck
- Security tested with SlowHTTPTest (DoS simulator)

## Example Code (Hello World)

```c++
class HelloWorldChannel : public Channel {
    // Use constructors from Channel
    using Channel::Channel;

    // Process incoming requests
    Control Process(const Request&, Response& res) override {
        res.SetContent("<h1>Hello world!</h1>");
        res.SetField("Content-Type", "text/html");
        return SendResponse(Status::Ok);
    }
};

class HelloWorldChannelFactory : public ChannelFactory {
    std::unique_ptr<Channel> CreateChannel(std::shared_ptr<FileStream> fs) override {
        return std::make_unique<HelloWorldChannel>(std::move(fs));
    }
};

int main() {
    auto endpoint = IPEndpoint({127, 0, 0, 1}, 3000);
    auto factory = std::make_shared<HelloWorldChannelFactory>();
    auto processingThreads = 1;
    HttpServer server(endpoint, factory, processingThreads);
    Log::Default()->SetLevel(Log::Level::Info);
    server.Start().wait();
}
```

## Getting Started
### Install Build Prerequisites
Assuming you're on Ubuntu/Debian,

```bash
$ sudo apt-get install cmake libcurl4-gnutls-dev libgtest-dev google-mock libunwind-dev
```
### Compile & Run

```bash
$ git clone https://github.com/ymarcov/http
$ cd http
$ git submodule update --init
$ cmake .
$ make hello_world
$ bin/hello_world
```

Then connect to `http://localhost:3000` from your browser.

## Documentation (Doxygen)
Documentation may be generated by running ```doxygen Doxyfile```. \
After that, it'll be available under `doc/html/index.html`.

## Performance
Running on Intel i7-3630QM (Laptop) @ 2.40GHz with 8GB of memory, here are SIEGE results. In this test, Chili outperformed all other servers, with *Lighttpd* coming in second. However, keep in mind that those other servers are big, configurable software systems--not simply a library. The test itself was to serve Apache's "It works!" HTML page from each server, as many times as possible, to mainly compare the number of requests served per second.

```bash
$ bin/sandbox_echo 3000 4 0 & # Run sandbox server with 4 threads
$ siege -b -c20 -t5S http://localhost:<PORT>
```

#### Chili

```
Transactions: 155471 hits
Availability: 100.00 %
Elapsed time: 4.50 secs
Data transferred: 1586.51 MB
Response time: 0.00 secs
Transaction rate: 34549.11 trans/sec
Throughput: 352.56 MB/sec
Concurrency: 19.30
Successful transactions: 155474
Failed transactions: 0
Longest transaction: 0.01
Shortest transaction: 0.00
```

#### Lighttpd 1.4.45
```
Transactions: 89438 hits
Availability: 100.00 %
Elapsed time: 4.37 secs
Data transferred: 259.38 MB
Response time: 0.00 secs
Transaction rate: 20466.36 trans/sec
Throughput: 59.36 MB/sec
Concurrency: 19.67
Successful transactions: 89439
Failed transactions: 0
Longest transaction: 0.03
Shortest transaction: 0.00
```

#### nginx 1.10.3

```
Transactions: 64993 hits
Availability: 100.00 %
Elapsed time: 4.08 secs
Data transferred: 212.29 MB
Response time: 0.00 secs
Transaction rate: 15929.66 trans/sec
Throughput: 52.03 MB/sec
Concurrency: 19.63
Successful transactions: 64994
Failed transactions: 0
Longest transaction: 0.02
Shortest transaction: 0.00
```

#### Apache 2.4.25

```
Transactions: 27975 hits
Availability: 100.00 %
Elapsed time: 4.40 secs
Data transferred: 81.13 MB
Response time: 0.00 secs
Transaction rate: 6357.95 trans/sec
Throughput: 18.44 MB/sec
Concurrency: 19.74
Successful transactions: 27975
Failed transactions: 0
Longest transaction: 0.68
Shortest transaction: 0.00
```
