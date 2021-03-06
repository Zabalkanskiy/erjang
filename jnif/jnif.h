
#ifndef __JNIF_H
#define __JNIF_H

#include <jni.h>
#include <list>
#include <assert.h>
#include <stdarg.h>

extern "C" {
#include "erl_nif.h"
// erl_nif.h may declare these as aliases:
#undef enif_get_int64
#undef enif_get_uint64
#undef enif_make_int64
#undef enif_make_uint64
};

#define JVM_NULL ((jobject)NULL)
#define NIF_TRUE 1
#define NIF_FALSE 0
// This may look odd - but we need to handle both "ERL_NIF_TERM is pointer" and "ERL_NIF_TERM is integral" cases.
#define THE_BADARG (reinterpret_cast<ERL_NIF_TERM>(reinterpret_cast<void*>(0xffffffffUL)))

struct jnif_module {
  void *so_handle;
  ErlNifEntry *entry;
  void *priv_data;
  ErlNifEnv *global;
};

struct jnif_dtor {
  virtual void release(ErlNifEnv* ee) {};
  virtual ~jnif_dtor() {};
};

struct enif_environment_t {
  JNIEnv *je;
  typedef enum { ALLOC, STACK } TYPE;
  TYPE type;

  // run when the environment is cleared (or freed)
  std::list< jnif_dtor* > dtors;

  // run every time a call executes, is used to "fake"
  // the behavior of enif_make_new_binary
  std::list< jnif_dtor* > commits;

  struct jnif_module *module;

  jobject self;
  ErlNifPid self_pid; // memory location to return

  enif_environment_t() { type = ALLOC; self = NULL; module = NULL; }

  const char* module_name() {
    if (module == NULL) return NULL;
    return module->entry->name;
  }
};

inline ERL_NIF_TERM J2E(jobject o) {
  assert (o != NULL);
  return reinterpret_cast<ERL_NIF_TERM>(o);
}

inline jobject E2J(ERL_NIF_TERM o) {
  assert (o != 0);
  return reinterpret_cast<jobject>(o);
}

// jnif_env.cc
void initialize_jnif_env(JavaVM* vm, JNIEnv *je);
ERL_NIF_TERM jnif_retain(ErlNifEnv* ee, jobject obj);
ERL_NIF_TERM jnif_retain(ErlNifEnv* ee, ERL_NIF_TERM term);
void jnif_init_env( ErlNifEnv * ee, JNIEnv *je, struct jnif_module *mod, enif_environment_t::TYPE type );
void jnif_release_env( ErlNifEnv * ee);
void jnif_commit_env( ErlNifEnv *ee);

// jnif_binary.cc
void initialize_jnif_binary(JavaVM* vm, JNIEnv *e);
void uninitialize_jnif_binary(JavaVM* vm, JNIEnv* je);

// jnif_resource.cc
void initialize_jnif_resource(JavaVM* vm, JNIEnv *je);

// jnif_string.cc
void initialize_jnif_string(JavaVM* vm, JNIEnv *je);

// jnif_number.cc
void initialize_jnif_number(JavaVM* vm, JNIEnv *je);

// jnif_atom.cc
void initialize_jnif_atom(JavaVM* vm, JNIEnv *je);

// jnif_tuple.cc
void initialize_jnif_tuple(JavaVM* vm, JNIEnv *je);

// jnif_list.cc
void initialize_jnif_list(JavaVM* vm, JNIEnv *je);

// jnif_sys.cc
void initialize_jnif_sys(JavaVM* vm, JNIEnv *je);

// jnif_process.cc
void initialize_jnif_process(JavaVM* vm, JNIEnv *je);

#endif
