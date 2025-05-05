/*
 * Copyright ©2024 Hannah C. Tang.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2024 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:

  size_t pos = buffer_.find(kHeaderEnd);
  if (pos == std::string::npos) {
    int byte_read;
    unsigned char buf[1024];

    while (true) {
      byte_read = WrappedRead(fd_, buf, sizeof(buf));
      if (byte_read == 0) {
        break;
      } else if (byte_read == -1) {
        return false;
      } else {
      buffer_.append(reinterpret_cast<char*>(buf), byte_read);
      pos = buffer_.find(kHeaderEnd);
      if (pos != std::string::npos)
        break;
    }
  }
}

if (pos == std::string::npos) {
  return false;
}

*request = ParseRequest(buffer_.substr(0, pos + 4));

if (request->uri() == "BAD_") {
  return false;
}

buffer_ = buffer_.substr(pos + 4);

std::cerr << "Remaining buffer after request: " << buffer_ << std::endl;

return true;  // you may need to change this return value
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

 HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:

  std::vector<std::string> lines;
  boost::split(lines, request, boost::is_any_of("\r\n"), 
    boost::algorithm::token_compress_on);

  if (lines.size() < 2) {
    req.set_uri("BAD_");
    return req;
  }

  for (auto& line : lines) {
    boost::trim(line);
  }

  std::vector<std::string> fst_line;
  boost::split(fst_line, lines[0], boost::is_any_of(" "), 
    boost::algorithm::token_compress_on);

  if (fst_line.size() < 2 || fst_line[0] != "GET" || fst_line[1][0] != '/') {
    req.set_uri("BAD_");
    return req;
  }

  req.set_uri(fst_line[1]);

  for (size_t i = 1; i < lines.size(); ++i) {
    size_t colon_pos = lines[i].find(':');
    if (colon_pos == std::string::npos) {
      continue;  // malformed header, skip it
    }
    std::string header_name = lines[i].substr(0, colon_pos);
    boost::trim(header_name);
    std::string header_value = lines[i].substr(colon_pos + 1);
    boost::trim(header_value);

    std::transform(header_name.begin(), header_name.end(), 
      header_name.begin(), ::tolower);

    req.AddHeader(header_name, header_value);
  }
  
  return req;
  }

}  // namespace hw4: