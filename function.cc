#include <functional>
#include <iostream>
#include <emacs-module.h>

using namespace std;
using namespace std::placeholders;

template<typename... Args>
using env_first_parameter_function_type = std::function<emacs_value(emacs_env*, Args...)>;

template<typename... Args>
using string_first_parameter_function_type = std::function<emacs_value(const std::string&, Args...)>;

using emacs_function_type = emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*);

emacs_value testFunc(emacs_env* env, ptrdiff_t, emacs_value*, void*) {
  cout << "emacs testfunc" << endl;
  return env->intern(env, "nil");
}

emacs_value func1(emacs_env* env, const std::string& s) {
  cout << "Func1" << endl;
  return env->intern(env, "nil");
}

template<typename... Args>
std::function<emacs_function_type>
createFunctionWrapperForEmacs(env_first_parameter_function_type<Args...> func) {
  return [func] (emacs_env *env, ptrdiff_t nargs, emacs_value* args, void* data) -> emacs_value {
    if (env == nullptr) {
      cout << "Did not receive emacs_env from emacs" << endl;
    }

    std::function<emacs_value(Args...)> nextParamFunc = std::bind(func, env, _1);
    return createFunctionWrapperForEmacs(nextParamFunc)(env, nargs, args, data);
  };
}

template<typename... Args>
std::function<emacs_function_type>
createFunctionWrapperForEmacs(string_first_parameter_function_type<Args...> func) {
  return [func] (emacs_env *env, ptrdiff_t nargs, emacs_value* args, void* data) -> emacs_value {
    cout << "hello, emacs 1st param string!" << endl;
    auto nextParamFunc = std::bind(func, "hello");
    return env->intern(env, "nil");
  };
}

template<typename... Args>
std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
createFunctionWrapperForEmacs(std::function<emacs_value(Args...)> func) {
  return createFunctionWrapperForEmacs(func, 0);
}

template<typename... Args>
std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
createFunctionWrapperForEmacs(std::function<emacs_value(emacs_env*, Args...)> func,
                              int argNumber) {
  cout << "Currying emacs_env" << endl;
  auto l = [argNumber, func] (emacs_env* env, ptrdiff_t nargs, emacs_value* args, void*) {
	     if (env == nullptr) {
	       cout << "Emacs_env was invalid" << endl;
	       return;
	     }
	     std::function<emacs_value(Args...)> f = std::bind(func, env, _1, _2);
	     auto l = createFunctionWrapperForEmacs(f, 1);
	   };
}

template<typename... Args>
std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
createFunctionWrapperForEmacs(std::function<emacs_value(const std::string&)> func,
                              int argNumber) {
  cout << "Currying string" << endl;
  auto l = [argNumber, func] (emacs_env* env, ptrdiff_t nargs, emacs_value* args, void*) {

	   };
  return l;
}

// template<typename... Args>
// std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)>
// createFunctionWrapperForEmacs(std::function<emacs_value(const std::string&, Args...)> func,
//                               int argNumber) {
//   cout << "hello, emacs 1st param string!" << endl;
//   std::function<emacs_value(emacs_env*, ptrdiff_t, emacs_value*, void*)> f = [argNumber, func] (emacs_env* env, ptrdiff_t nargs, emacs_value* args, void*) {
//     ptrdiff_t string_length;
//     char* argument = NULL;
//     bool ret = env->copy_string_contents(env, args[argNumber], NULL, &string_length);
//     if (!ret) {
//       abort();
//     }
//     argument = (char *)malloc(string_length);

//     if (!argument) {
//       abort();
//     }

//     ret = env->copy_string_contents(env, args[argNumber], argument, &string_length);

//     if (!ret) {
//       free(argument);
//       abort();
//     }
//     std::function<emacs_value(Args...)> curried = std::bind(func, argument, _1);
//     createFunctionWrapperForEmacs(env, curried, argNumber + 1);
//   };

//   return testFunc;
// }

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

int main(int argc, char* argv[]) {
  createFunctionWrapperForEmacs(std::function<emacs_value(emacs_env*, const std::string&)>(func1))(nullptr, 2, nullptr, nullptr);
  cout << "hello, world!" << endl;
}
