#include "FileUtils.h"
#include <algorithm>

std::string FileUtils::GetFullPath(const std::string& iRelativePath)
{
  char full[260];
  return (_fullpath(full, iRelativePath.c_str(), 260) != 0) ? full : iRelativePath;
}
std::string FileUtils::GetFileName(const std::string& iStr)
{
  int pos1 = iStr.find_last_of("/");
  int pos2 = iStr.find_last_of("\\");
  if (pos1 == std::string::npos && pos2 == std::string::npos) {
    return iStr;
  }
  auto pos = (pos1 != std::string::npos && pos2 != std::string::npos) ? std::max(pos1, pos2) : (pos1 != std::string::npos ? pos1 : pos2);
  auto fileName = iStr.substr(pos + 1);
  return fileName;
}
std::string FileUtils::GetPath(const std::string& iStr)
{
  int pos1 = iStr.find_last_of("/");
  int pos2 = iStr.find_last_of("\\");
  if (pos1 == std::string::npos && pos2 == std::string::npos) {
    return "";
  }
  auto pos = (pos1 != std::string::npos && pos2 != std::string::npos) ? std::max(pos1, pos2) : (pos1 != std::string::npos ? pos1 : pos2);
  auto path = iStr.substr(0, pos + 1);
  return path;
}
std::string FileUtils::GetPathWithLastBackslash(const std::string& iPath)
{
  if (iPath == "") {
    return iPath;
  }
  std::string res(GetPathWithBackslashes(iPath));
  if (res.back() != '\\') {
    res += "\\";
  }
  return res;
}
std::string FileUtils::GetPathWithBackslashes(const std::string& iPath)
{
  std::string res(iPath);
  auto pos = res.find_first_of('/');
  if (pos == std::string::npos) {
    return res;
  }
  res[pos] = '\\';
  return GetPathWithBackslashes(res);
}
