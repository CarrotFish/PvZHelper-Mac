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

#ifndef __xnumem__ProcessModules__
#define __xnumem__ProcessModules__

#include <iostream>
#include <mach/mach.h>
#include <mach-o/dyld_images.h>
#include <vector>

typedef struct ModuleData{
 	const struct mach_header*	imageLoadAddress;	/* base address image is mapped into */
	const char*					imageFilePath;		/* path dyld used to load the image */
	uintptr_t					imageFileModDate;	/* time_t of image file */
} ModuleData_t;

typedef uintptr_t         module_t;     // Module base pointer

class ProcessModules
{
    friend class xnu_proc;
public:
    ProcessModules( class xnu_proc& process );
    ~ProcessModules();
    
    /**
     Get module data by module name.
     
     @param name -- Module name.
     @return ModuleData structure of the specified module. nullptr if not found.
     */
    const ModuleData_t* GetModule( const char * name );
    
    /**
     Get module data by module base address.
     
     @param BaseAddr -- Memory base address of module.
     @return ModuleData structure of the specified module. nullptr if not found.
     */
    const ModuleData_t* GetModule( module_t BaseAddr );
    
    /**
     Get process main module.
     
     @param void
     @return ModuleData structure of the main module. nullptr if not found.
     */
    const ModuleData_t* GetMainModule( );
    
    // Contains all modules and their data
    inline std::vector<ModuleData_t> modules() { return _all_modules; };
    
private:
    ProcessModules( const ProcessModules& ) = delete;
    ProcessModules& operator =(const ProcessModules&) = delete;
    
    // Retrieve all module info structures
    kern_return_t QueryModules();
    
    struct task_dyld_info        _dyld_info;
    struct dyld_all_image_infos _all_module_infos;
    std::vector<ModuleData_t>    _all_modules;
    
private:
    class xnu_proc&        _process;
    class ProcessCore&     _core;
    class ProcessMemory&   _memory;
};

#endif /* defined(__xnumem__ProcessModules__) */
