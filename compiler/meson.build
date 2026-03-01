# Mostly AI Generated because I could not bother thinking about building

project('Beryl', ['cpp', 'c'], 
  version : '1.0.0', 
  default_options : ['cpp_std=c++20']
)

# 1. Dependencies
dep_zstd = dependency('libzstd')
dep_zlib = dependency('zlib')
dep_xml2 = dependency('libxml-2.0')

# 2. LLVM
llvm_dep = dependency('llvm', 
  version : '>=14.0.0', 
  modules : ['core', 'native', 'bitwriter', 'executionengine'],
  method : 'config-tool',
)

# 3. Flag Processing
cpp = meson.get_compiler('cpp')
raw_llvm_flags = llvm_dep.get_variable(configtool: 'cxxflags').split()
relevant_cxxflags = []
foreach f : raw_llvm_flags
  if f != '-fno-exceptions' and f != '-fno-rtti'
    relevant_cxxflags += f
  endif
endforeach

extra_link_args = []
build_rpath_dirs = []

if host_machine.system() == 'darwin'
  llvm_prefix = llvm_dep.get_variable(configtool: 'prefix')
  llvm_libdir = llvm_dep.get_variable(configtool: 'libdir')
  libcxx_dir = llvm_libdir / 'c++'
  homebrew_libcxx = libcxx_dir / 'libc++.dylib'
  if not import('fs').exists(homebrew_libcxx)
    libcxx_dir = llvm_libdir
    homebrew_libcxx = libcxx_dir / 'libc++.dylib'
  endif
  relevant_cxxflags += [
    '-isystem' + llvm_prefix + '/include/c++/v1',
    '-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST'
  ]
  extra_link_args += [
    homebrew_libcxx,
    '-Wl,-search_paths_first'
  ]
  build_rpath_dirs += [libcxx_dir]
endif

source_files = [
  'beryl.cpp',
  'struct-opt/struct_opt.cpp',
  'clparser/build.cpp',
  'clparser/print.cpp',
  'clparser/run.cpp',
  'clparser/venv.cpp',
  'be1/biir-codegen/gen.cpp',
  'be1/lexer/lex.cpp',
  'be1/parser/parse.cpp',
  'utils/error.cpp',
  'utils/parse_json_params.cpp'
]

executable('beryl', 
  sources : source_files, 
  dependencies : [dep_zstd, dep_zlib, dep_xml2, llvm_dep],
  cpp_args : relevant_cxxflags, 
  link_args : extra_link_args,
  build_rpath : build_rpath_dirs.length() > 0 ? ':'.join(build_rpath_dirs) : '',
  install : true
)