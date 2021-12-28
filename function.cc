#include <functional>
#include <iostream>
#include <emacs-module.h>

using namespace std;
using namespace std::placeholders;

emacs_value testFunc(emacs_env* env, ptrdiff_t, emacs_value*, void*) {
  cout << "emacs testfunc" << endl;
  return env->intern(env, "nil");
}

template<typename... Args>
std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
createFunctionWrapperForEmacs(emacs_env * env,
                              std::function<emacs_value(Args...)> func) {
  return createFunctionWrapperForEmacs(env, func, 0);
}

template<typename... Args>
std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
createFunctionWrapperForEmacs(emacs_env * env,
                              std::function<emacs_value(emacs_env*, const std::string&, Args...)> func,
                              int argNumber) {
  cout << "hello, emacs 1st param string!" << endl;
  std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)> f = [argNumber, func] (emacs_env* env, ptrdiff_t nargs, emacs_value* args, void*) {
    ptrdiff_t string_length;
    char* argument = NULL;
    bool ret = env->copy_string_contents(env, args[argNumber], NULL, &string_length);
    if (!ret) {
      abort();
    }
    argument = (char *)malloc(string_length);

    if (!argument) {
      abort();
    }

    ret = env->copy_string_contents(env, args[argNumber], argument, &string_length);

    if (!ret) {
      free(argument);
      abort();
    }
    auto curried = std::bind(func, env, argument, _1);
    createFunctionWrapperForEmacs(env, curried, argNumber + 1);
  };

  return testFunc;
}

// template<typename R, typename... Args>
// std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
// createFunctionWrapperForEmacs(emacs_env * env,
//                               R (*func)(emacs_env* env, int a, Args...),
//                               int argNumber) {
//   cout << "hello, emacs 1st param int!" << endl;
//   return testFunc;
// }

template<typename Fn>
void register_emacs_function(emacs_env* env, Fn fn) {
  cout << "hello, register!" << endl;
}

emacs_value func1(emacs_env* env, const std::string& s, int a) {
  cout << "Func1" << endl;
  return env->intern(env, "nil");
}

emacs_value func2(emacs_env* env, int a, int b) {
  cout << "Func2" << endl;
  return env->intern(env, "nil");
}

int main(int argc, char* argv[]) {
  createFunctionWrapperForEmacs(nullptr, std::function<emacs_value(emacs_env*, const std::string&, int)>(func1));
  //  createFunctionWrapperForEmacs(nullptr, func2);
  cout << "hello, world!" << endl;
}
