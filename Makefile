
cpp_srcs := $(shell find src -name "*.cpp")
cpp_objs := $(patsubst %.cpp,%.o,$(cpp_srcs))
cpp_objs := $(subst src/,objs/,$(cpp_objs))

c_srcs := $(shell find src -name "*.c")
c_objs := $(patsubst %.c,%.o,$(c_srcs))
c_objs := $(subst src/,objs/,$(c_objs))

lean := /data/datav/expstation/lean
include_paths := src 
library_paths := 

link_librarys := stdc++ dl

run_paths     := $(foreach item,$(library_paths),-Wl,-rpath=$(item))
include_paths := $(foreach item,$(include_paths),-I$(item))
library_paths := $(foreach item,$(library_paths),-L$(item))
link_librarys := $(foreach item,$(link_librarys),-l$(item))

cpp_compile_flags := -std=c++11 -fPIC -m64 -g -fopenmp -w -O0 -DHAS_CUDA_HALF
link_flags        := -pthread -fopenmp

cpp_compile_flags += $(include_paths)
link_flags 		  += $(library_paths) $(link_librarys) $(run_paths)

pro : workspace/pro

workspace/pro : $(cpp_objs) $(cu_objs) $(c_objs)
	@echo Link $@
	@mkdir -p $(dir $@)
	@g++ $^ -o $@ $(link_flags)

objs/%.o : src/%.cpp
	@echo Compile CXX $<
	@mkdir -p $(dir $@)
	@g++ -c $< -o $@ $(cpp_compile_flags)

objs/%.o : src/%.c
	@echo Compile CXX $<
	@mkdir -p $(dir $@)
	@g++ -c $< -o $@ $(cpp_compile_flags)

run : workspace/pro
	@cd workspace && ./pro

debug :
	@echo $(includes)

clean :
	@rm -rf objs workspace/pro