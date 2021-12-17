#include <emacs-module.h>
#include <string.h>
#include <clang-c/CXCompilationDatabase.h>
#include <stdlib.h>

int plugin_is_GPL_compatible;

emacs_value initializeBuildTree(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data);

int emacs_module_init(struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment (runtime);
  emacs_value func = env->make_function (env, 1, 1,
					 initializeBuildTree, "Initialize a build tree.  Argument is a path containing compilation_commands.json file", NULL);
  emacs_value symbol = env->intern (env, "elclang-initialize-build-tree");
  emacs_value args[] = {symbol, func};
  env->funcall (env, env->intern (env, "defalias"), 2, args);
  return 0;
}

void emacs_message(emacs_env *env, const char* logMessage) {
  emacs_value m_args[] = { env->make_string(env, logMessage, strlen(logMessage)) };
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
    return env->intern (env, "nil");
  }

  emacs_message(env, "Successfully retrieved argument");
  emacs_message(env, buildPath);

  CXCompilationDatabase_Error errorCode;
  CXCompilationDatabase db;
  db = clang_CompilationDatabase_fromDirectory(buildPath, &errorCode);
  if (errorCode == CXCompilationDatabase_CanNotLoadDatabase) {
    emacs_message(env, "Could not load compilation database.");
  } else {
    emacs_message(env, "Loaded compilation database.");
  }
  free(buildPath);
  clang_CompilationDatabase_dispose(db);
  return env->intern (env, "nil");
}

/* emacs_value visited_file(emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data) { */

/* } */
