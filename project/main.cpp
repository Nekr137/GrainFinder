#pragma once

#include "tests.h"
#include "Treatments.h"
#include "FileUtils.h"

int main(int argc, char** argv) {

  std::string fileName, outputPath;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] != '-') {
      fileName.assign(argv[i]);
      continue;
    }
    switch (argv[i][1]) {
      case 'o':
        outputPath.assign(argv[++i]);
        break;
      case 'u':
        unit_tests();
    }
  }


  auto fileFullPath = FileUtils::GetFullPath(fileName);
  printf("Input File: %s\n", fileFullPath.c_str());

  fileName = FileUtils::GetFileName(fileFullPath);
  auto fileDir = FileUtils::GetPath(fileFullPath);

  if (!outputPath.empty() && outputPath.find(":") == std::string::npos) {

    outputPath = FileUtils::GetPathWithLastBackslash(fileDir) + outputPath;

    if (CreateDirectoryA(outputPath.c_str(), NULL)) {
      printf("Created a directory");
    }
    else if (ERROR_ALREADY_EXISTS == GetLastError()) {
      printf("The directory already exist");
    }
    else
      printf("Error while output file creation!");
  }
  
  outputPath = FileUtils::GetPathWithLastBackslash(outputPath);

  treatment1(fileFullPath, outputPath);
}