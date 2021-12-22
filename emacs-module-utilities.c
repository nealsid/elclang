#include "emacs-module-utilities.h"

#include <emacs-module.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

emacs_value emacs_nil;

/* Sends a message to Emacs' *Messages* buffer */
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

/* Copies a string from arguments passed to a C function called from Elisp. */
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
