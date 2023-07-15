#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <assert.h>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <openssl/ssl.h>
#include "ssl_read.hh"

namespace ssl = asio::ssl;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ssl_read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int ssl_read(const std::string& host, const std::string& port_num, const std::string& http, std::string& response)
{
  std::vector<std::string> header;

  std::stringstream ss;
  try
  {
    std::error_code ec;
    asio::io_service io_service;
    asio::ip::tcp::resolver resolver(io_service); //name resolution (DNS lookup)
    asio::ip::tcp::resolver::query query(host, port_num);
    asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    ssl::context context(ssl::context::tlsv12_client);
    context.set_default_verify_paths();

    ssl::stream<asio::ip::tcp::socket> sock(io_service, context);
    asio::connect(sock.lowest_layer(), endpoint_iterator);
    sock.lowest_layer().set_option(asio::ip::tcp::no_delay(true));

    //Server Name Indication (SNI)
    ::SSL_set_tlsext_host_name(sock.native_handle(), host.c_str());

    sock.set_verify_mode(ssl::verify_none);
    sock.set_verify_callback(ssl::rfc2818_verification(host));
    sock.handshake(ssl::stream<asio::ip::tcp::socket>::client);

    size_t ret = asio::write(sock, asio::buffer(http, http.size()));
    std::cout << ret << std::endl;

    //read until end of HTTP header
    //Note:: after a successful read_until operation, the streambuf may contain additional data
    //beyond the delimiter. An application will typically leave that data in the streambuf for a subsequent
    //read_until operation to examine.

    asio::streambuf sbuf;
    std::string line;

    asio::read_until(sock, sbuf, "\r\n\r\n");
    std::istream response_stream(&sbuf);
    while (std::getline(response_stream, line) && line != "\r")
    {
      header.push_back(line);
      std::cout << line << std::endl;
    }

    //dump whatever content we already have
    if (sbuf.size() > 0)
    {
      ss << &sbuf;
    }

    //read until EOF, dumping to string stream as we go.
    while (asio::read(sock, sbuf, asio::transfer_at_least(1), ec))
    {
      ss << &sbuf;
    }
    if (ec != asio::error::eof)
    {
    }
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    std::ofstream ofs1("exception.txt");
    ofs1 << e.what();
    ofs1.close();
  }

  response = ss.str();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_response
//http://dummy.restapiexample.com/api/v1/employees
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_response(const std::string& host, const std::string& port_num, const std::string& api)
{
  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);

  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  asio::streambuf request;
  std::ostream request_stream(&request);
  request_stream << "GET " << api << " HTTP/1.0\r\n";
  request_stream << "Host: " << host << "\r\n";
  request_stream << "Accept: */*\r\n";
  request_stream << "Connection: close\r\n\r\n";

  std::string buf;
  std::stringstream ss;
  asio::streambuf::const_buffers_type bufs = request.data();
  std::string str(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + request.size());

  std::ofstream ofs("request.txt");
  ofs << str;
  ofs.close();

  std::cout << str.c_str();

  try
  {
    asio::ip::tcp::socket sock(io_context);
    asio::connect(sock, resolver.resolve(host, port_num));
    size_t ret = asio::write(sock, request);
    std::stringstream s;
    s << "sent " << ret << " bytes";

    //receive response
    asio::streambuf response;
    std::istream response_stream(&response);

    // Read the response headers, which are terminated by a blank line.
    asio::read_until(sock, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
    {
    }

    // Write whatever content we already have to output.
    if (response.size() > 0)
    {
      ss << &response;
    }

    //read until EOF
    asio::error_code error;
    while (asio::read(sock, response, asio::transfer_at_least(1), error))
    {
      ss << &response;
    }
    if (error != asio::error::eof)
      throw asio::system_error(error);
  }
  catch (std::exception& e)
  {
    std::cout << std::string(e.what()) << std::endl;
    return buf;
  }

  buf = ss.str();
  std::ofstream out("output.json");
  out << buf;
  out.close();
  return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// trim_url
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string trim_url(const std::string& str)
{
  size_t start = str.find_first_not_of(" \n\r\t");
  size_t until = str.find_last_not_of(" \n\r\t");
  std::string::const_iterator i = start == std::string::npos ? str.begin() : str.begin() + start;
  std::string::const_iterator x = until == std::string::npos ? str.end() : str.begin() + until + 1;
  return std::string(i, x);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_url
/////////////////////////////////////////////////////////////////////////////////////////////////////

void parse_url(const std::string& raw, url_t& url)
{
  std::string path, domain, x, protocol, port, query;
  int offset = 0;
  size_t pos1, pos2, pos3, pos4;
  x = trim_url(raw);
  offset = offset == 0 && x.compare(0, 8, "https://") == 0 ? 8 : offset;
  offset = offset == 0 && x.compare(0, 7, "http://") == 0 ? 7 : offset;
  pos1 = x.find_first_of('/', offset + 1);
  path = pos1 == std::string::npos ? "" : x.substr(pos1);
  domain = std::string(x.begin() + offset, pos1 != std::string::npos ? x.begin() + pos1 : x.end());
  path = (pos2 = path.find("#")) != std::string::npos ? path.substr(0, pos2) : path;
  port = (pos3 = domain.find(":")) != std::string::npos ? domain.substr(pos3 + 1) : "";
  domain = domain.substr(0, pos3 != std::string::npos ? pos3 : domain.length());
  protocol = offset > 0 ? x.substr(0, offset - 3) : "";
  query = (pos4 = path.find("?")) != std::string::npos ? path.substr(pos4 + 1) : "";
  path = pos4 != std::string::npos ? path.substr(0, pos4) : path;

  url.domain = domain;
  url.path = path;
  url.protocol = protocol;
  url.port = port;
  url.query = query;

  std::cout << "[" << raw << "]" << std::endl;
  std::cout << "protocol: " << protocol << std::endl;
  std::cout << "domain: " << domain << std::endl;
  std::cout << "port: " << port << std::endl;
  std::cout << "path: " << path << std::endl;
  std::cout << "query: " << query << std::endl;
}

