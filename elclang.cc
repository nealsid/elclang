#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

#include <string>

extern "C" {
#include "emacs-module-utilities.h"
}

CXCompilationDatabase compilationDatabase;

using namespace std;

extern "C" {
  void buildCompilationDatabase(const char* buildPath) {
    CXCompilationDatabase_Error errorCode;
    compilationDatabase = clang_CompilationDatabase_fromDirectory(buildPath,
								  &errorCode);
    if (errorCode == CXCompilationDatabase_CanNotLoadDatabase) {
      emacs_message(env, "Could not load compilation database.");
    } else {
      emacs_message(env, "Loaded compilation database.");
    }
  }

  void parseVisitedFile(const char* fullyQualifiedPath) {
    CXCompileCommands compileCommands =
      clang_CompilationDatabase_getCompileCommands(compilationDatabase,
						   fullyQualifiedPath);

    int numberOfCommands = clang_CompileCommands_getSize(compileCommands);

    if (numberOfCommands > 1) {
      emacs_message(env, "%s had %d compilation commands, using first one.", fullyQualifiedPath, numberOfCommands);
    } else if (numberOfCommands == 0) {
      emacs_message(env, "%s had 0 compilation commands, no information available.", fullyQualifiedPath);
      return;
    } else {
      emacs_message(env, "Found compilation command for %s\n", fullyQualifiedPath);
    }

    CXCompileCommand compileCommand = clang_CompileCommands_getCommand(compileCommands, 0);
    CXIndex index = clang_createIndex(1, 1);

    int numberOfCompilerArgs = clang_CompileCommand_getNumArgs(compileCommand);

    CXString compilerArguments[numberOfCompilerArgs];
    char const *compilerArgs[numberOfCompilerArgs];

    for (int i = 0; i < numberOfCompilerArgs; ++i) {
      compilerArguments[i] = clang_CompileCommand_getArg(compileCommand, i);
      compilerArgs[i] = clang_getCString(compilerArguments[i]);
      emacs_message(env, "Arg: %s", compilerArgs[i]);
    }

    CXTranslationUnit tu;

    enum CXErrorCode err =
      clang_parseTranslationUnit2(index, NULL, compilerArgs, numberOfCompilerArgs,
				  NULL, 0, 0, &tu);

    if (err != CXError_Success) {
      emacs_message(env, "Error creating translation unit: %d", err);
    } else {
      emacs_message(env, "Successfully created translation unit.");
    }

    for (int i = 0; i < numberOfCompilerArgs; ++i) {
      clang_disposeString(compilerArguments[i]);
    }
    clang_CompileCommands_dispose(compileCommands);
  }

}
