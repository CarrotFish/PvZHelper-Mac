/*
 * Copyright (C) 2014  Jonathan Daniel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact : jonathandaniel@email.com
 */

#ifndef		_xnu_mem_
#define		_xnu_mem_
    
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
    
#include <mach/mach.h>
#include <sys/mman.h>

#include "ProcessCore.h"
#include "ProcessMemory.h"
#include "ProcessModules.h"
#include "code.h"

typedef struct kinfo_proc kinfo_proc;
typedef struct vm_region_basic_info vm_region_basic_info;

#if (!defined __GNUC__ || __GNUC__ < 2 || __GNUC_MINOR__ < (defined __cplusplus ? 6 : 4))
#define __MACH_CHECK_FUNCTION (reinterpret_cast<__const char *>(0))
#else
#define __MACH_CHECK_FUNCTION __PRETTY_FUNCTION__
#endif

#define MACH_CHECK_ERROR(ret) \
mach_check_error (ret, __FILE__, __LINE__, __MACH_CHECK_FUNCTION);
void mach_check_error (kern_return_t ret, const char *file, unsigned int line, const char *func);

#ifdef __LP64__
#define nlist_native nlist_64
#define LC_SEGMENT_NATIVE LC_SEGMENT_64
#define segment_command_native segment_command_64
#define mach_header_native mach_header_64
#define section_native section_64
#define PAGEZERO_SIZE 0x100000000;
#else
#define nlist_native nlist
#define LC_SEGMENT_NATIVE LC_SEGMENT
#define segment_command_native segment_command
#define mach_header_native mach_header
#define section_native section
#define PAGEZERO_SIZE 0x1000
#endif

class xnu_proc
{
    friend class ProcessCore;
    
public:
    xnu_proc();
    ~xnu_proc(void);
    
    /**
     Attach to running process.
     
     @param pid -- target process id.
     @return int representing success or error ( 1 is success 0 is error ).
     */
    int Attach(int pid);
    
    /**
     Attach to running process.
     
     @param procname -- target process name.
     @return int representing success or error ( 1 is success 0 is error ).
     */
    int Attach(char * procname);
    
    /**
     Detach from the attached process.
     
     @param void
     @return int representing success or error ( 1 is success 0 is error ).
     */
    int Detach();
    
    /**
     Retrieve process id from process name.
     
     @param procname -- target process name
     @return pid corresponding to the target process.
     */
    static int32_t PidFromName(char* procname);
    
    /**
     Target process id.
     
     @param
     @return pid corresponding to the attached process.
     */
    inline int pid() const { return _core.pid(); }
    
    /**
     Check if process allows debugging.
     
     @param
     @return positive or negative (null) value.
     */
    inline int AllowDebug() const { return _core._pinfo_proc->kp_proc.p_debugger; }
    
    inline ProcessCore&     core()        { return _core; }
    inline ProcessMemory&   memory()      { return _memory; }
    inline ProcessModules&  modules()     { return _modules; }     // Memory manipulations
    inline Code&            code()        { return _code; }
    
private:
    xnu_proc(const xnu_proc&) = delete;
    xnu_proc& operator =(const xnu_proc&) = delete;
    
    static int GetProcessList(kinfo_proc **procList, size_t *procCount);
    
private:
    ProcessCore     _core;
    ProcessMemory   _memory;     // Memory manipulations
    ProcessModules  _modules;    // Modules info & manipulation
    Code            _code;
};

#endif /* _xnu_mem_ */
