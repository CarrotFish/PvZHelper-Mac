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

#include "ProcessMemory.h"
#include "xnumem.h"

#include <mach/mach_vm.h>

#include <assert.h>
#include <QDebug>

ProcessMemory::ProcessMemory(xnu_proc *pprocess) : _process(pprocess), _core(pprocess->core()) {
}

ProcessMemory::~ProcessMemory() {
}

kern_return_t ProcessMemory::Read(uintptr_t address, size_t size, void *buffer) {
    assert(size != 0 || address != 0);
    
    vm_size_t data_cnt = size;
    
    kern_return_t kret = vm_read_overwrite(_core._pmach_port, address, size, (vm_address_t) buffer, &data_cnt);
    
    if (kret != KERN_SUCCESS)
        MACH_CHECK_ERROR(kret)
    
    return kret;

//	unsigned char *rbuffer;
//	mach_msg_type_number_t data_cnt;

//    int systemPageSize = getpagesize();

//    // use the negative of the page size for the mask to find the page address
//    mach_vm_address_t page_address = address & uintptr_t(-systemPageSize);

//    mach_vm_address_t last_page_address =
//    (address + size + uintptr_t(systemPageSize - 1)) & uintptr_t(-systemPageSize);

//    mach_vm_size_t page_size = last_page_address - page_address;

//    kern_return_t kernret = vm_read(_core._pmach_port, page_address, page_size, (vm_offset_t*)&rbuffer, &data_cnt);
//    if(kernret != KERN_SUCCESS)
//    {
//        //MACH_CHECK_ERROR(kernret);
//        return 1;
//    }

//    memcpy(buffer,&rbuffer[mach_vm_address_t(address) - page_address], size);
//    mach_vm_deallocate(_core._pmach_port, mach_vm_address_t(rbuffer), size);

//	return KERN_SUCCESS;
}

kern_return_t ProcessMemory::Write(uintptr_t address, size_t size, void *buffer) {
    if (address == 0 || size == 0)
        return KERN_INVALID_ARGUMENT;
    
    kern_return_t kret;
    if ((kret = vm_write(_core._pmach_port, address, vm_offset_t(buffer), size)) != KERN_SUCCESS) {
        vm_prot_t backup = 0;
        
        for (auto it = _segments.begin(); it != _segments.end(); it++) {
            if (it->address <= address && address < (it->address + it->size)) {
                backup = it->info.protection;
                break;
            }
        }
        vm_protect(_core._pmach_port, address, size, 0, backup | VM_PROT_WRITE | VM_PROT_READ);
        kret = vm_write(_core._pmach_port, address, vm_offset_t(buffer), size);
        vm_protect(_core._pmach_port, address, size, 0, backup);
        if (kret != KERN_SUCCESS) {
            MACH_CHECK_ERROR(kret)
            return kret;
        }
    }
    return kret;
}

kern_return_t ProcessMemory::Copy(uintptr_t source_address, size_t size, uintptr_t dest_address) {
    kern_return_t kret;
    kret = vm_copy(_core._pmach_port, source_address, size, dest_address);
    return kret;
}

kern_return_t
ProcessMemory::Protect(uintptr_t address, size_t size, vm_prot_t protection, vm_prot_t *backup /* = nullptr */ ) {
    kern_return_t kret = KERN_SUCCESS;
    if (backup != nullptr) {
        for (std::vector<MemoryRegion_t>::iterator it = _segments.begin(); it != _segments.end(); ++it) {
            if (it->address <= address && address <= (it->address + it->size)) {
                *backup = it->info.protection;
                break;
            }
        }
    }
    
    kret = vm_protect(_core._pmach_port, vm_address_t(address), size, 0, protection);
    if (kret != KERN_SUCCESS)
        MACH_CHECK_ERROR(kret)
    
    return kret;
}

kern_return_t ProcessMemory::Free(uintptr_t address, size_t size) {
    kern_return_t kret = KERN_SUCCESS;
    kret = vm_deallocate(_core._pmach_port, vm_address_t(address), size);
    if (kret)
        MACH_CHECK_ERROR(kret)
    return kret;
}

uintptr_t ProcessMemory::Allocate(size_t size, vm_prot_t prot /* =  VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE */,
                                  uintptr_t BaseAddr /* = 0 */ ) {
    kern_return_t kret = KERN_SUCCESS;
    boolean_t anywhere;
    unsigned char *address;
    address = (unsigned char *) &BaseAddr;
    
    if (size == 0)
        printf("Xnumem : Warning -- size to allocate is zero.\n");
    
    if (*address == 0)
        anywhere = TRUE;
    else
        anywhere = FALSE;
    
    kret = vm_allocate(_core._pmach_port, (vm_address_t *) &address, size, anywhere);
    if (kret)
        MACH_CHECK_ERROR(kret)
    
    kret = Protect((uintptr_t) address, size, prot);
    if (kret)
        MACH_CHECK_ERROR(kret)
    
    return (uintptr_t) address;
}

char *ProcessMemory::protection_bits_to_rwx(vm_prot_t p) {
    // previous version of this somehow lost the "p&", always returning rwx..
    static char returned[4];
    
    returned[0] = (p & VM_PROT_READ ? 'r' : '-');
    returned[1] = (p & VM_PROT_WRITE ? 'w' : '-');
    returned[2] = (p & VM_PROT_EXECUTE ? 'x' : '-');
    returned[3] = '\0';
    
    // memory leak here. No biggy
    return (strdup(returned));
}

size_t ProcessMemory::_word_align(size_t size) {
    size_t rsize = 0;
    
    rsize = ((size % sizeof(long)) > 0) ? (sizeof(long) - (size % sizeof(long))) : 0;
    rsize += size;
    
    return rsize;
}

const char *ProcessMemory::unparse_inheritance(vm_inherit_t i) {
    switch (i) {
        case VM_INHERIT_SHARE:
            return "share";
        case VM_INHERIT_COPY:
            return "copy";
        case VM_INHERIT_NONE:
            return "none";
        default:
            return "???";
    }
}

const char *ProcessMemory::behavior_to_text(vm_behavior_t b) {
    switch (b) {
        case VM_BEHAVIOR_DEFAULT:
            return ("default");
        case VM_BEHAVIOR_RANDOM:
            return ("random");
        case VM_BEHAVIOR_SEQUENTIAL:
            return ("fwd-seq");
        case VM_BEHAVIOR_RSEQNTL:
            return ("rev-seq");
        case VM_BEHAVIOR_WILLNEED:
            return ("will-need");
        case VM_BEHAVIOR_DONTNEED:
            return ("will-need");
        case VM_BEHAVIOR_FREE:
            return ("free-nowb");
        case VM_BEHAVIOR_ZERO_WIRED_PAGES:
            return ("zero-wire");
        case VM_BEHAVIOR_REUSABLE:
            return ("reusable");
        case VM_BEHAVIOR_REUSE:
            return ("reuse");
        case VM_BEHAVIOR_CAN_REUSE:
            return ("canreuse");
        default:
            return ("?");
    }
}

kern_return_t ProcessMemory::QueryRegions() {
    mach_vm_address_t address = 0x0;
    mach_vm_size_t size;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t objectName = MACH_PORT_NULL;
    
    while (mach_vm_region(_core._pmach_port, &address, &size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info,
                          &infoCount, &objectName) == 0) {
        MemoryRegion_t *region = (MemoryRegion_t *) malloc(sizeof(MemoryRegion_t));
        region->address = address;
        region->size = size;
        region->info = info;
        _segments.push_back(*region);
        address += size;
        free(region);
    }
    
    return KERN_SUCCESS;
}

// todo : show binaries
kern_return_t ProcessMemory::PrintSegments() {
    printf("\n ==== Regions for process %i (%s) \n", _core.pid(), _core._pinfo_proc->kp_proc.p_comm);
    for (std::vector<MemoryRegion_t>::iterator it = _segments.begin(); it != _segments.end(); ++it) {
        int print_size;
        const char *print_size_unit = NULL;
        print_size = (int) it->size;
        if (print_size > 1024) {
            print_size /= 1024;
            print_size_unit = "KB";
        }
        if (print_size > 1024) {
            print_size /= 1024;
            print_size_unit = "MB";
        }
        if (print_size > 1024) {
            print_size /= 1024;
            print_size_unit = "GB";
        }
        
        printf(" %p - %p [%d%s](%c%c%c/%c%c%c; %s, %s, %s, %s)\n",
               (void *) (it->address),
               (void *) (it->address + it->size),
               print_size,
               print_size_unit,
               (it->info.protection & VM_PROT_READ) ? 'r' : '-',
               (it->info.protection & VM_PROT_WRITE) ? 'w' : '-',
               (it->info.protection & VM_PROT_EXECUTE) ? 'x' : '-',
               (it->info.max_protection & VM_PROT_READ) ? 'r' : '-',
               (it->info.max_protection & VM_PROT_WRITE) ? 'w' : '-',
               (it->info.max_protection & VM_PROT_EXECUTE) ? 'x' : '-',
               unparse_inheritance(it->info.inheritance),
               it->info.shared ? "shared" : "private",
               it->info.reserved ? "reserved" : "not-reserved",
               behavior_to_text(it->info.behavior));
    }
    
    return KERN_SUCCESS;
}

mach_vm_size_t ProcessMemory::GetMemoryRegionSize(const uint64_t address, mach_vm_size_t *size_to_end) {
    mach_vm_address_t region_base = (mach_vm_address_t) address;
    mach_vm_size_t region_size;
    natural_t nesting_level = 0;
    vm_region_submap_info submap_info;
    mach_msg_type_number_t info_count = VM_REGION_SUBMAP_INFO_COUNT;
    
    // Get information about the vm region containing |address|
    vm_region_recurse_info_t region_info;
    region_info = reinterpret_cast<vm_region_recurse_info_t>(&submap_info);
    
    kern_return_t result =
            mach_vm_region_recurse(_core._pmach_port,
                                   &region_base,
                                   &region_size,
                                   &nesting_level,
                                   region_info,
                                   &info_count);
    
    if (result == KERN_SUCCESS) {
        // Get distance from |address| to the end of this region
        *size_to_end = region_base + region_size - (mach_vm_address_t) address;
        
        // If we want to handle strings as long as 4096 characters we may need
        // to check if there's a vm region immediately following the first one.
        // If so, we need to extend |*size_to_end| to go all the way to the end
        // of the second region.
        if (*size_to_end < 4096) {
            // Second region starts where the first one ends
            mach_vm_address_t region_base2 =
                    (mach_vm_address_t) (region_base + region_size);
            mach_vm_size_t region_size2;
            
            // Get information about the following vm region
            result =
                    mach_vm_region_recurse(_core._pmach_port,
                                           &region_base2,
                                           &region_size2,
                                           &nesting_level,
                                           region_info,
                                           &info_count);
            
            // Extend region_size to go all the way to the end of the 2nd region
            if (result == KERN_SUCCESS
                && region_base2 == region_base + region_size) {
                region_size += region_size2;
            }
        }
        
        *size_to_end = region_base + region_size - (mach_vm_address_t) address;
    } else {
        region_size = 0;
        *size_to_end = 0;
    }
    
    return region_size;
}

#define kMaxStringLength 8192

const char *ProcessMemory::ReadString(const uint64_t address) {
    // The problem is we don't know how much to read until we know how long
    // the string is. And we don't know how long the string is, until we've read
    // the memory!  So, we'll try to read kMaxStringLength bytes
    // (or as many bytes as we can until we reach the end of the vm region).
    mach_vm_size_t size_to_end;
    GetMemoryRegionSize(address, &size_to_end);
    
    if (size_to_end > 0) {
        mach_vm_size_t size_to_read =
                size_to_end > kMaxStringLength ? kMaxStringLength : size_to_end;
        
        char *string = (char *) malloc(size_to_read);
        kern_return_t kret = Read(address, size_to_read, string);
        if (kret)
            MACH_CHECK_ERROR(kret)
        
        return string;
    }
    
    return nullptr;
}
