#pragma once

#include <string>

class FileUtils {
public:
  static std::string GetPath(const std::string& iStr);
  static std::string GetFileName(const std::string& iStr);
  static std::string GetFullPath(const std::string& iRelativePath);
  static std::string GetPathWithBackslashes(const std::string& iPath);
  static std::string GetPathWithLastBackslash(const std::string& iPath);
};

