#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <openssl/ssl.h>
#include "asio.hpp"

#include "ssl_read.hh"
#include "htmlcxx/ParserDom.h"

int parse_file(const std::string& file, std::string& html);
int parse_uri(const std::string& uri, std::string& html);
void traverse_tree(tree<htmlcxx::HTML::Node> const& dom);
namespace ssl = asio::ssl;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
// --uri https://example.com/index.html
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  std::string uri;
  std::string file;
  std::string html;

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg == "--uri")
    {
      uri = argv[++i];
    }
    else if (arg == "--file")
    {
      file = argv[++i];
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get HTML buffer
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (uri.size())
  {
    parse_uri(uri, html);
  }
  else if (file.size())
  {
    parse_file(file, html);
  }
  else
  {
    parse_file("image-url.html", html);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // parse HTML
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  try
  {
    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    parser.parse(html);
    tr = parser.getTree();
    traverse_tree(tr);
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception " << e.what() << " caught" << std::endl;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

int parse_file(const std::string& file, std::string& html)
{
  std::ifstream ifs;
  ifs.open(file);
  if (!ifs.is_open())
  {
    std::cerr << "Unable to open file " << file << std::endl;
    return -1;
  }

  while (1)
  {
    char buf[BUFSIZ];
    ifs.read(buf, BUFSIZ);
    if (ifs.gcount() == 0) {
      break;
    }
    html.append(buf, ifs.gcount());
  }
  ifs.close();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// traverse_tree
/////////////////////////////////////////////////////////////////////////////////////////////////////

void traverse_tree(tree<htmlcxx::HTML::Node> const& dom)
{
  tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
  std::cout << it->text();
  for (unsigned i = 0; i < dom.number_of_children(it); i++)
  {
    traverse_tree(dom.child(it, i));
  }
  std::cout << it->closingText();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_uri
/////////////////////////////////////////////////////////////////////////////////////////////////////

int parse_uri(const std::string& uri, std::string& html)
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //parse URI
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::size_t start = uri.find("://", 0);
  if (start == std::string::npos)
  {
    return -1;
  }
  start += 3;
  std::size_t end = uri.find("/", start + 1);
  std::string host = uri.substr(start, end - start);
  std::string path = uri.substr(end);

  std::cout << host << std::endl;
  std::cout << path << std::endl;

  const std::string port_num = "443";

  std::stringstream http;
  http << "GET " << path << " HTTP/1.1\r\n";
  http << "Host: " << host << "\r\n";
  http << "Connection: close\r\n";
  http << "\r\n";

  std::cout << http.str() << std::endl;

  std::string buf;
  if (ssl_read(host, port_num, http.str(), buf) < 0)
  {
  }
  if (!buf.size())
  {
    return -1;
  }

  html = buf;

  std::ofstream ofs("output");
  ofs << buf;
  ofs.close();

  return 0;
}

