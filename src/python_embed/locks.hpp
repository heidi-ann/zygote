#ifndef LOCKS_H
#define LOCKS_H

#include "python_embed_headers.hpp"

#include <boost/python.hpp>
#include <mutex>
#include "interpreter_context.hpp"

namespace lock {
    

    ///
    /// A simple way of handling python errors
    /// Run it after boost::python::error_already_set is thrown to get the python error message.
    /// TODO: move this somewhere more sensible!!!!
    ///
    std::string get_python_error_message();


    ///
    /// A RAII lock for the GIL. Usage:
    ///
    ///     {
    ///         GIL lock_gil(interpreter_context);
    ///         stuff();
    ///     }
    ///
    class GIL {
        public:
            ///
            /// Lock a GIL. Unlock on destruction.
            ///
            /// @param interpreter_context
            ///     An interpreter context to lock on.
            ///     The GIL is locked on the main thread.
            ///
            /// @param name
            ///     String for debugging.
            ///     Example: "Interpreter initialization"
            ///
            GIL(InterpreterContext interpreter_context, std::string name);

            ///
            /// Lock a GIL. Unlock on destruction.
            ///
            /// @param interpreter_context
            ///     An interpreter context to lock on.
            ///     The GIL is locked on the main thread.
            ///
            GIL(InterpreterContext interpreter_context);

            ///
            /// Unlock GIL.
            ///
            ~GIL();

            ///
            /// Tempoarily public debug ID.
            ///
            /// @deprecated
            ///     Will become private in future.
            ///
            /// @deprecated
            ///     May be removed entirely.
            ///
            static int i;
            int inst;

        private:
            // Can't copy locks
            GIL(const GIL&) = delete;

            ///
            /// String for debugging.
            /// Example: "Interpreter initialization"
            ///
            std::string name;
    };

    ///
    /// A RAII lock for PyThreadState creation. Usage:
    ///
    ///     {
    ///         ThreadState threadstate(interpreter_context);
    ///         stuff(thredstate.get_threadstate());
    ///     }
    ///
    class ThreadState {
        public:
            ///
            /// Create a PyThreadState, allowing for easy
            /// constructive locking.
            ///
            /// @param interpreter_context
            ///     An interpreter context to get an InterpreterState from.
            ///
            ThreadState(InterpreterContext interpreter_context);

            ///
            /// Clear and delete the associated PyThreadState.
            ///
            ~ThreadState();

            ///
            /// Return the PyThreadState.
            /// As always, this should be opaque. Usage after the
            /// ThreadState object has finished is undefined behaviour.
            ///
            /// @return
            ///     The associated PyThreadState
            ///
            PyThreadState *get_threadstate();

        private:
            // Can't copy locks
            ThreadState(const ThreadState&) = delete;

            ///
            /// Associated PyThreadState.
            ///
            PyThreadState *threadstate;
    };

    ///
    /// A RAII lock for PyThreadState GIL grabbing. Usage:
    ///
    ///     {
    ///         ThreadGIL lock_thread(threadstate);
    ///         stuff();
    ///     }
    ///
    class ThreadGIL {
        public:
            ///
            /// Lock the thread. Unlock on destruction.
            ///
            ThreadGIL(ThreadState &);

            ///
            /// Unlock thread.
            ///
            ~ThreadGIL();

        private:
            // Can't copy locks
            ThreadGIL(const ThreadGIL&) = delete;
    };

    ///
    /// A RAII lock for PyThreadState GIL releasing. Usage:
    ///
    ///     {
    ///         ThreadGILRelease unlock_thread;
    ///         stuff();
    ///     }
    ///
    class ThreadGILRelease {
        public:
            ///
            /// Unlock the thread. Lock on destruction.
            ///
            ThreadGILRelease();

            ///
            /// Lock thread.
            ///
            ~ThreadGILRelease();

        private:
            // Can't copy locks
            ThreadGILRelease(const ThreadGILRelease&) = delete;

            ///
            /// Associated PyThreadState.
            ///
            PyThreadState *threadstate;
    };


    ///
    /// A wrapper class largely equivalent to pair<T, std::mutex>.
    ///
    /// Allows for convenient association of locks to values,
    /// but does not enforce the lock.
    ///
    template <class T>
    class Lockable {
        public:
            ///
            /// Create lockable object using the default
            /// initializer for the templated type.
            ///
            Lockable(): value(), lock(std::make_shared<std::mutex>()) {};

            ///
            /// Create lockable object from an instance
            /// of the templated type.
            ///
            /// @param value
            ///     Instance of the templated type
            ///     to associate wrapper with.
            ///
            Lockable(T value): value(value), lock(std::make_shared<std::mutex>()) {};

            ///
            /// Create lockable object from an instance
            /// of the templated type and a premade mutex.
            ///
            /// This can be used to give the same lock to
            /// several objects.
            ///
            /// @param value
            ///     Instance of the templated type
            ///     to associate wrapper with.
            ///
            /// @param lock
            ///     Premade lock object.
            ///
            Lockable(T value, std::shared_ptr<std::mutex> lock):
                value(value), lock(lock) {};

            ///
            /// The templated type instance.
            ///
            /// Before usage, the lock should be taken as
            /// required by the specific circumstance.
            ///
            T value;

            ///
            /// The associated lock.
            ///
            std::shared_ptr<std::mutex> lock;
    };

    ///
    /// Convenience function in the style of make_shared.
    ///
    /// Usage: make_lockable<Type>(arguments...)
    ///
    /// @param args...
    ///     The arguments to be forwarded to the
    ///     templated type's constructor.
    ///
    template<class T, class... Args>
    Lockable<T> make_lockable(Args &&... args) {
        return Lockable<T>(new T(std::forward<Args>(args)...));
    }
}

#endif
