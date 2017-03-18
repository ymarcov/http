# Yam::HTTP
***A simple to use, yet efficient, modern C++ HTTP server library***

Here's an example Hello World server:

```c++
class HelloWorldChannel : public ChannelBase {
    // Use constructors from ChannelBase
    using ChannelBase::ChannelBase;

    // Process incoming requests
    Control Process() override {
        GetResponder().SetContent(CreateHtml());
        GetResponder().SetField("Content-Type", "text/html");
        return SendResponse(Status::Ok);
    }

    std::shared_ptr<std::vector<char>> CreateHtml() const {
        auto text = std::string("<u><b>Hello world!</b></u>\n");
        return std::make_shared<std::vector<char>>(begin(text), end(text));
    }
};

class HelloWorldChannelFactory : public ChannelFactory {
    std::unique_ptr<ChannelBase> CreateChannel(std::shared_ptr<FileStream> fs,
                                               Channel::Throttlers throttlers) {
        return std::make_unique<HelloWorldChannel>(std::move(fs), std::move(throttlers));
    }
};

int main() {
    auto endpoint = IPEndpoint({127, 0, 0, 1}, 3000);
    auto factory = std::make_unique<HelloWorldChannelFactory>();
    auto processingThreads = 1;

    HttpServer server(endpoint, std::move(factory), processingThreads);
    Log::Default()->SetLevel(Log::Level::Info);
    server.Start().wait();
}
```
