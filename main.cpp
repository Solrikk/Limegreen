#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *data) {
  size_t newLength = size * nmemb;
  try {
    data->append((char *)contents, newLength);
  } catch (std::bad_alloc &e) {
    return 0;
  }
  return newLength;
}

std::string downloadHTMLContent(const std::string &url) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << "\n";
      return "";
    }
    return readBuffer;
  }
  return "";
}

std::string extractImageURL(const std::string &htmlContent) {
  std::size_t pos = htmlContent.find("<img src=\"");
  if (pos != std::string::npos) {
    std::size_t start = pos + 10;
    std::size_t end = htmlContent.find("\"", start);
    if (end != std::string::npos) {
      return htmlContent.substr(start, end - start);
    }
  }
  return "";
}

int main() {
  std::vector<std::string> urls = {
      "https://sonum.ru/catalog/mebel/pufy/banketka/"
      "?offerId=116544&utm_source=xml_agent&utm_campaign=export"};

  for (const std::string &url : urls) {
    std::string htmlContent = downloadHTMLContent(url);
    std::string imageURL = extractImageURL(htmlContent);
    std::cout << imageURL << std::endl; 
  }

  return 0;
}