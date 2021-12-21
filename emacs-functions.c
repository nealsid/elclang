/* Implementation file containing functions we make available to
   Emacs, as well as code to register them. */
#include "emacs-module-utilities.h"

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>
#include <stdlib.h>

typedef emacs_value (*emacs_callable)(emacs_env *, ptrdiff_t, emacs_value *, void *);
CXCompilationDatabase compilationDatabase;

emacs_value initializeBuildTree(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);
emacs_value visited_file(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);

/* A struct to define functions we expose to elisp. */
struct EmacsLispCallableFunction {
  const char *elisp_function_name; /* The elisp function name */
  emacs_callable c_function_ptr;  /* The function pointer that is called. */
  int min_args; /* The minimum number of arguments the function can take. */
  int max_args; /* The maximum number of arguments the function can take. */
  const char *help_text; /* Help text for within Emacs. */
  void *function_data; /* User data passed to the function. */
};

/* Functions we expose to elisp. */
struct EmacsLispCallableFunction emacsLispFunctions[] = {
  {
    "elclang-initialize-build-tree", initializeBuildTree, 1, 1,
    "Initialize a build tree.  Argument is a path containing compilation_commands.json file", NULL
  },
  {
    "elclang-file-visited", visited_file, 1, 1,
    "Function to look up compilation information for a file.  Meant for use in after-load-functions hook.", NULL
  }
};

int register_elisp_functions() {
  int number_of_elisp_callables = sizeof(emacsLispFunctions) / sizeof(emacsLispFunctions[0]);
  emacs_value defalias = env->intern(env, "defalias");

  for (int i = 0; i < number_of_elisp_callables; ++i) {
    emacs_value func = env->make_function(env,
                                          emacsLispFunctions[i].min_args,
                                          emacsLispFunctions[i].max_args,
                                          emacsLispFunctions[i].c_function_ptr,
                                          emacsLispFunctions[i].help_text,
                                          emacsLispFunctions[i].function_data);

    emacs_value symbol = env->intern(env, emacsLispFunctions[i].elisp_function_name);
    emacs_value args[] = { symbol, func };
    env->funcall(env, defalias, 2, args);
  }
  // TODO fix error handling.
  return 0;
}

emacs_value initializeBuildTree(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) {
  char* buildPath;

  copy_string_from_args(env, args, 0, &buildPath);
  if (!buildPath) {
    emacs_message(env, "Could not retrieve build path argument");
    RETURN_NIL();
  }

  emacs_message(env, "Successfully retrieved argument");
  emacs_message(env, buildPath);

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
