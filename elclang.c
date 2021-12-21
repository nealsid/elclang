#include <emacs-module.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

int plugin_is_GPL_compatible;

#define RETURN_NIL() return env->intern(env, "nil")

emacs_value initializeBuildTree(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);
emacs_value visited_file(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);

CXCompilationDatabase compilationDatabase;

int emacs_module_init(struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment (runtime);

  emacs_value func = env->make_function (env, 1, 1,
					 initializeBuildTree, "Initialize a build tree.  Argument is a path containing compilation_commands.json file", NULL);
  emacs_value symbol = env->intern (env, "elclang-initialize-build-tree");
  emacs_value args[] = {symbol, func};
  env->funcall (env, env->intern (env, "defalias"), 2, args);

  func = env->make_function(env, 1, 1,
                            visited_file, "Function to look up compilation information for a file.  Meant for use in after-load-functions hook.", NULL);
  symbol = env->intern(env, "elclang-file-visited");
  args[0] = symbol;
  args[1] = func;
  env->funcall (env, env->intern(env, "defalias"), 2, args);

  return 0;
}

void emacs_message(emacs_env *env, const char* logMessageFormat, ... ) {
  va_list argp;
  const int messageBufferSize = 8192;
  char messageBuffer[messageBufferSize]; // random limit.

  va_start(argp, logMessageFormat);
  int stringLength = vsnprintf(messageBuffer, messageBufferSize, logMessageFormat, argp);
  va_end(argp);

  emacs_value m_args[] = { env->make_string(env, messageBuffer, stringLength) };
  env->funcall(env, env->intern (env, "message"), 1, m_args);
}

void copy_string_from_args(emacs_env *env, emacs_value *args, ptrdiff_t arg_number, char** argument) {
  ptrdiff_t string_length;
  *argument = NULL;

  bool ret = env->copy_string_contents(env, args[arg_number], NULL, &string_length);

  if (!ret) {
    emacs_message(env, "Could not determine argument length");
    return;
  }

  *argument = (char *)malloc(string_length);

  if (!*argument) {
    emacs_message(env, "Could not allocate memory for argument buffer.");
    return;
  }

  ret = env->copy_string_contents(env, args[arg_number], *argument, &string_length);

  if (!ret) {
    emacs_message(env, "Could not retrieve string argument.");
    free(*argument);
    *argument = NULL;
  }
  return;
}

emacs_value initializeBuildTree (emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) {
  char* buildPath;

  copy_string_from_args(env, args, 0, &buildPath);
  if (!buildPath) {
    emacs_message(env, "Could not retrieve build path argument");
    RETURN_NIL();
  }

  emacs_message(env, "Successfully retrieved argument");
  emacs_message(env, buildPath);

  CXCompilationDatabase_Error errorCode;
  compilationDatabase = clang_CompilationDatabase_fromDirectory(buildPath, &errorCode);
  if (errorCode == CXCompilationDatabase_CanNotLoadDatabase) {
    emacs_message(env, "Could not load compilation database.");
  } else {
    emacs_message(env, "Loaded compilation database.");
  }
  free(buildPath);
  RETURN_NIL();
}

// This function's signature should be one that can be added to the
// after-load-functions hook.
emacs_value visited_file(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) {
  char *visitedFilePath;

  copy_string_from_args(env, args, 0, &visitedFilePath);

  if (!visitedFilePath) {
    emacs_message(env, "Could not retrieve filename from args");
    RETURN_NIL();
  }

  CXCompileCommands compileCommands =
    clang_CompilationDatabase_getCompileCommands(compilationDatabase,
                                                 visitedFilePath);

  int numberOfCommands = clang_CompileCommands_getSize(compileCommands);

  if (numberOfCommands > 1) {
    emacs_message(env, "%s had %d compilation commands, using first one.", visitedFilePath, numberOfCommands);
  } else if (numberOfCommands == 0) {
    emacs_message(env, "%s had 0 compilation commands, no information available.", visitedFilePath);
    free(visitedFilePath);
    RETURN_NIL();
  } else {
    emacs_message(env, "Found compilation command for %s\n", visitedFilePath);
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

  free(visitedFilePath);
  clang_CompileCommands_dispose(compileCommands);
  RETURN_NIL();
}
