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

#include "ProcessCore.h"

#include <sys/sysctl.h>

#include <mach/mach.h>
#include "xnumem.h"

ProcessCore::ProcessCore()
{
}

ProcessCore::~ProcessCore()
{
    Close();
}

int ProcessCore::Open(int pid)
{
    // Prevent leak
    Close();
    
    if(pid)
        _pid = pid;
    
    // retrieve kinfo_proc
    unsigned long j;
	kinfo_proc * proclist;
	size_t procCount;
	
    xnu_proc::GetProcessList(&proclist, &procCount);
	
	for (j = 0; j < procCount +1; j++) {
		if (proclist[j].kp_proc.p_pid == _pid ){
            _pinfo_proc = (struct kinfo_proc*)malloc(sizeof(struct kinfo_proc));
            memcpy(_pinfo_proc, &proclist[j], sizeof(struct kinfo_proc));
        }
	}
    kern_return_t kret = task_for_pid(mach_task_self(), _pid, &_pmach_port);
	if(kret != KERN_SUCCESS)
    {
	   
#ifndef NDEBUG
       printf("task_for_pid() error, try running as sudo!\n");
#endif
       
       MACH_CHECK_ERROR(kret);
       return 0;
    }
    
    free(proclist);
    return 1;
}

int ProcessCore::Close()
{
    if (_pmach_port || _pid || _pinfo_proc)
    {
        kern_return_t kret = mach_port_deallocate(mach_task_self(), _pmach_port);
        if(kret != KERN_SUCCESS)
        {
            MACH_CHECK_ERROR(kret);
            return 0;
        }
        
        _pid = 0;
        _pmach_port = 0;
        free(_pinfo_proc);
    }
    
    return 1;
}
