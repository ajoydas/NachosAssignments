cmake_minimum_required(VERSION 3.9)
project(code)

set(CMAKE_CXX_STANDARD 11)
#add_custom_target(code ALL COMMAND mingw32-make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(code
        bin/coff.h
        bin/coff2flat.c
        bin/coff2noff
        bin/coff2noff.c
        bin/d.c
        bin/disasm.c
        bin/encode.h
        bin/execute.c
        bin/halt
        bin/instr.h
        bin/int.h
        bin/main.c
        bin/Makefile
        bin/noff.h
        bin/opstrings.c
        bin/out.c
        bin/system.c
        filesys/test/big
        filesys/test/medium
        filesys/test/small
        filesys/directory.cc
        filesys/directory.h
        filesys/filehdr.cc
        filesys/filehdr.h
        filesys/filesys.cc
        filesys/filesys.h
        filesys/fstest.cc
        filesys/Makefile
        filesys/openfile.cc
        filesys/openfile.h
        filesys/synchdisk.cc
        filesys/synchdisk.h
        machine/console.cc
        machine/console.h
        machine/disk.cc
        machine/disk.h
        machine/interrupt.cc
        machine/interrupt.h
        machine/machine.cc
        machine/machine.h
        machine/mipssim.cc
        machine/mipssim.h
        machine/network.cc
        machine/network.h
        machine/stats.cc
        machine/stats.h
        machine/sysdep.cc
        machine/sysdep.h
        machine/timer.cc
        machine/timer.h
        machine/translate.cc
        machine/translate.h
        network/Makefile
        network/nettest.cc
        network/post.cc
        network/post.h
        network/README
        test/halt.c
        test/Makefile
        test/matmult.c
        test/script
        test/shell.c
        test/sort.c
        threads/copyright.h
        threads/list.h
        threads/main.cc
        threads/Makefile
        threads/preemptive.cc
        threads/preemptive.h
        threads/scheduler.cc
        threads/scheduler.h
        threads/switch.h
        threads/synch.cc
        threads/synch.h
        threads/synchlist.h
        threads/system.cc
        threads/system.h
        threads/thread.cc
        threads/thread.h
        threads/threadtest.cc
        threads/utility.cc
        threads/utility.h
        userprog/addrspace.cc
        userprog/addrspace.h
        userprog/bitmap.cc
        userprog/bitmap.h
        userprog/exception.cc
        userprog/Makefile
        userprog/progtest.cc
        userprog/syscall.h
        vm/Makefile
        Makefile
        Makefile.common
        Makefile.dep userprog/memorymanager.cc userprog/memorymanager.h userprog/processtable.cc userprog/processtable.h userprog/syncconsole.h userprog/syncconsole.cc)
