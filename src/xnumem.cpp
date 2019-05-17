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

#include "xnumem.h"

#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/mach_error.h>

#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <mach-o/nlist.h>

#include <stdlib.h>
#include <stdio.h>
#import  <dlfcn.h>

#include <assert.h>
#include <errno.h>
#include <err.h>

#include <sys/sysctl.h>
#include <sys/mman.h>

void mach_check_error (kern_return_t ret, const char *file, unsigned int line, const char *func)
{
    if (ret == KERN_SUCCESS) {
        return;
    }
    if (func == nullptr) {
        func = "[UNKNOWN]";
    }
    
#ifndef NDEBUG
    errx(1,"fatal Mach error on line %u of \"%s\" : %s\n",
         line, file, mach_error_string (ret));
#endif

}

xnu_proc::xnu_proc() : _core(), _memory(this), _modules(*this), _code(*this)
{
    
}

xnu_proc::~xnu_proc()
{
    
}

int xnu_proc::Attach(int pid)
{
    int ret = 0;
    ret = _core.Open(pid);
    _memory.QueryRegions();
    //_modules.QueryModules();
    return ret;
}

int xnu_proc::Attach(char * procname)
{
    int ret = 0;
    int pid = PidFromName(procname);
    ret = _core.Open(pid);
    _memory.QueryRegions();
    //_modules.QueryModules();
    return ret;
}

int xnu_proc::Detach()
{
    return _core.Close();
}

int32_t xnu_proc::PidFromName(char* procname)
{
	pid_t pid = 0;
    unsigned long j;
	kinfo_proc * proclist;
	size_t procCount;
	
	GetProcessList(&proclist, &procCount);
	
	for (j = 0; j < procCount +1; j++) {
		if (strcmp(proclist[j].kp_proc.p_comm, procname) == 0 ) 
					pid = proclist[j].kp_proc.p_pid;
	}
	
	free(proclist);
	return pid;
}

int xnu_proc::GetProcessList(kinfo_proc **procList, size_t *procCount)
{
    int                 err;
    kinfo_proc *        result;
    int                 done;
    static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t              length;
	
	assert( procList != NULL);
		//assert(*procList == NULL);
    assert(procCount != NULL);
    *procCount = 0;
	
    result = nullptr;
    done = 0;
    do {
        assert(result == NULL);
		
        length = 0;
        err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
					 NULL, &length,
					 NULL, 0);
        if (err == -1) {
            err = errno;
        }
        if (err == 0) {
            result = (kinfo_proc*)malloc(length);
            if (result == NULL) {
                err = ENOMEM;
            }
        }
		
        if (err == 0) {
            err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
						 result, &length,
						 NULL, 0);
            if (err == -1) {
                err = errno;
            }
            if (err == 0) {
                done = 1;
            } else if (err == ENOMEM) {
                assert(result != NULL);
                free(result);
                result = NULL;
                err = 0;
            }
        }
    } while (err == 0 && ! done);
	
    if (err != 0 && result != NULL) {
        free(result);
        result = NULL;
    }
    *procList = result;
    if (err == 0) {
        *procCount = length / sizeof(kinfo_proc);
    }
	
    assert( (err == 0) == (*procList != NULL) );
    return err;
}
