#pragma once

#include "tests.h"
#include "Treatments.h"
#include "FileUtils.h"

int main(int argc, char** argv) {

  std::string fileName, outputPath;
  size_t grainsCnt = 10;
  double compactness = 0.1;

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
		break;
	  case 'n':
	  {
		  std::string str;
		  str.assign(argv[++i]);
		  grainsCnt = std::stoi(str);
		  break;
	  }
	  case 'c':
	  {
		  std::string str;
		  str.assign(argv[++i]);
		  compactness = std::stod(str);
		  break;
	  }
    }
  }

  if (fileName.empty()) {
    printf("[ error ] Filename is empty");
    return 0;
  }


  auto fileFullPath = FileUtils::GetFullPath(fileName);
  printf("Input File: %s\n", fileFullPath.c_str());
  printf("Grains count: %d\n", grainsCnt);
  printf("Compactness: %f\n", compactness);

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

  treatment2(fileFullPath, outputPath, grainsCnt, compactness);
}