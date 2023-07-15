# url_parse_open_graph
Download OpenGraph data from a URL to generate link previews 

## Usage

url_parse_open_graph is a command line client. Usage is:

```
./url_parse_open_graph [OPTIONS]
[OPTIONS]:
  --uri <URL> URL to parse (e.g https://example.com/index.html)
  --file <FILE> Local FILE to parse

```

## Example

```
./url_parse_open_graph --uri https://example.com/index.html

```

### HTTP request

```
GET /index.html HTTP/1.1
Host: example.com
Connection: close

```


## Dependencies

- CMake [https://cmake.org/]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)
- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- JSON Modern C++ [https://json.nlohmann.me/] 

## Building

```
cmake -S . -B build
cd build
cmake --build . 

```

To install dependencies on Mac and Linux

Mac

``` cmd 
brew install cmake
brew install openssl
```

Linux

``` cmd 
sudo apt-get install cmake
sudo apt-get install libssl-dev 
```
