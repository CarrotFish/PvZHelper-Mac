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

#include "ProcessModules.h"

#include "xnumem.h"
#include <mach-o/dyld_images.h>

ProcessModules::ProcessModules( class xnu_proc& pprocess ) :
    _process( pprocess ),
    _core(pprocess.core()),
    _memory(pprocess.memory())
{
}

ProcessModules::~ProcessModules()
{
}

char *basename(const char * path)
{
    char *s = const_cast<char *>(strrchr(path, '/'));
    if(s==nullptr) {
        return strdup(path);
    } else {
        return strdup(s + 1);
    }
}

kern_return_t ProcessModules::QueryModules()
{
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    kern_return_t kret = KERN_SUCCESS;
    
    
    kret = task_info(_core._pmach_port, TASK_DYLD_INFO, task_info_t(&_dyld_info), &count);
    if(kret != KERN_SUCCESS)
        MACH_CHECK_ERROR(kret);
    
    if(_dyld_info.all_image_info_addr != 0){
         _all_module_infos = *(struct dyld_all_image_infos*)_dyld_info.all_image_info_addr;
        mach_vm_address_t address = _dyld_info.all_image_info_addr;
        
        if(address){
            // Read the structure inside of dyld that contains information about
            // loaded images.  We're reading from the desired task's address space.
            
            _all_module_infos = _memory.Read<struct dyld_all_image_infos>(int(address));
            
        }
    }
    
    for(unsigned int j = 0; j < _all_module_infos.infoArrayCount; j++)
    {
        ModuleData_t *ModInfo = reinterpret_cast<ModuleData_t *>(malloc(sizeof(ModuleData_t)));
        // Initialise _all_modules vector
        ModInfo->imageFilePath    = _all_module_infos.infoArray[j].imageFilePath;
        ModInfo->imageFileModDate = _all_module_infos.infoArray[j].imageFileModDate;
        ModInfo->imageLoadAddress = _all_module_infos.infoArray[j].imageLoadAddress;
        _all_modules.push_back(*ModInfo);
        free(ModInfo);
    }
    
    return kret;
}

const ModuleData_t* ProcessModules::GetModule( const char * name )
{
    ModuleData_t *ret = new ModuleData_t;
    
    for (std::vector<ModuleData_t>::iterator it = _all_modules.begin() ; it != _all_modules.end(); ++it)
    {
        if(strcmp(basename(it->imageFilePath), name) ==0){
            ret = _all_modules.data() + (it - _all_modules.begin());
            return ret;
        }
    }
    
    return nullptr;
}

const ModuleData_t* ProcessModules::GetModule( module_t BaseAddr )
{
    const ModuleData_t * temp = new ModuleData_t;
    for (std::vector<ModuleData_t>::iterator it = _all_modules.begin() ; it != _all_modules.end(); ++it)
    {
        if(uintptr_t(it->imageLoadAddress) == BaseAddr){
            temp = &(*it);
            return temp;
        }
    }
    
    return nullptr;
}

const ModuleData_t* ProcessModules::GetMainModule( )
{
    return &_all_modules[0];
}
