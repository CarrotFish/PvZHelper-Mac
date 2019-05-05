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

#ifndef __xnumem__ProcessCore__
#define __xnumem__ProcessCore__

#include <iostream>
#include <sys/sysctl.h>

class ProcessCore
{
    friend class xnu_proc;
    friend class ProcessMemory;
    friend class ProcessModules;
    friend class Code;
    
public:

    inline int pid() const { return _pid; }
    inline struct kinfo_proc * pinfo_proc() const { return _pinfo_proc; };
    
private:
    ProcessCore();
    ProcessCore( const ProcessCore& ) = delete;
    ~ProcessCore();
    
    int Open(int pid);
    int Close();
    
    int _pid = 0;
    mach_port_t _pmach_port = 0;
    struct kinfo_proc *_pinfo_proc = NULL;
};
#endif /* defined(__xnumem__ProcessCore__) */
