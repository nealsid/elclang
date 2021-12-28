/* Implementation file containing functions we make available to
   Emacs, as well as code to register them. */
#include "emacs-module-utilities.h"

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>
#include <emacs-module.h>
#include <stdlib.h>

typedef emacs_value (*emacs_callable)(emacs_env *, ptrdiff_t, emacs_value *, void *);
CXCompilationDatabase compilationDatabase;

emacs_value initializeBuildTree(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);
emacs_value visited_file(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);
emacs_value dump_ast(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);
emacs_value cursor_extent_for_point(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);

void buildCompilationDatabase(emacs_env *env, const char* buildPath);
void parseVisitedFile(emacs_env *env, const char* fullyQualifiedPath);
void dumpASTForFile(emacs_env *env, const char* fullyQualifiedPath);
void getClangCursorExtentForEmacsCursorPosition(emacs_env *env, unsigned int line, unsigned int column, const char* filename,
                                                unsigned int output[4]);

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
  },
  {
    "elclang-dump-ast", dump_ast, 1, 1,
    "Function to dump AST for file.", NULL
  },
  {
    "elclang-cursor-extent-for-point", cursor_extent_for_point, 4, 4,
    "Returns the cursor range for the cursor under the point.", NULL
  }
};

int register_elisp_functions(emacs_env *env) {
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

  emacs_message(env, "Successfully retrieved build path argument");
  emacs_message(env, "%s", buildPath);
  buildCompilationDatabase(env, buildPath);
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

  parseVisitedFile(env, visitedFilePath);

  free(visitedFilePath);
  RETURN_NIL();
}

emacs_value dump_ast(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) {
  char *fullyQualifiedFilename;

  copy_string_from_args(env, args, 0, &fullyQualifiedFilename);

  if (!fullyQualifiedFilename) {
    emacs_message(env, "Could not retrieve filename from args");
    RETURN_NIL();
  }

  dumpASTForFile(env, fullyQualifiedFilename);

  free(fullyQualifiedFilename);
  RETURN_NIL();
}

emacs_value cursor_extent_for_point(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) {
  unsigned int line, col;

  if (nargs < 3) {
    RETURN_NIL();
  }

  line = env->extract_integer(env, args[0]);
  // Emacs column numbers are 0-based, and clang is 1-based.
  col = env->extract_integer(env, args[1]) + 1;

  char *fullyQualifiedFilename;
  copy_string_from_args(env, args, 2, &fullyQualifiedFilename);

  if (!fullyQualifiedFilename) {
    emacs_message(env, "Could not retrieve filename from args");
    RETURN_NIL();
  }

  unsigned int lineColValues[4];
  getClangCursorExtentForEmacsCursorPosition(env, line, col, fullyQualifiedFilename, lineColValues);
  lineColValues[1]--;
  lineColValues[3]--;
  emacs_message(env, "Start: L%d/C%d", lineColValues[0], lineColValues[1]);
  emacs_message(env, "End: L%d/C%d", lineColValues[2], lineColValues[3]);

  emacs_value output_vector = args[3];
  for (int i = 0; i < 4; ++i) {
    env->vec_set(env, output_vector, i, env->make_integer(env, lineColValues[i]));
  }

  free(fullyQualifiedFilename);
  RETURN_NIL();
}
