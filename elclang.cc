#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

#include <string>

extern "C" {
#include "emacs-module-utilities.h"
}

CXCompilationDatabase compilationDatabase;

using namespace std;

void buildCompilationDatabase(const std::string& buildPath) {
  CXCompilationDatabase_Error errorCode;
  compilationDatabase = clang_CompilationDatabase_fromDirectory(buildPath.c_str(),
								&errorCode);
  if (errorCode == CXCompilationDatabase_CanNotLoadDatabase) {
    emacs_message(env, "Could not load compilation database.");
  } else {
    emacs_message(env, "Loaded compilation database.");
  }
}

void parseVisitedFile(const std::string& fullyQualifiedPath) {


}
