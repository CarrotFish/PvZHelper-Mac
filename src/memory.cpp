#include "memory.h"
#include <mach/mach_vm.h>
#include <cassert>
#include <cerrno>
#include <string>
#include <err.h>

void mach_check_error(kern_return_t ret, const char *file, unsigned int line, const char *func) {
    if (ret == KERN_SUCCESS) {
        return;
    }
    if (func == nullptr) {
        func = "[UNKNOWN]";
    }

#ifndef NDEBUG
    errx(1, "fatal Mach error on line %u of \"%s\" : %s\n", line, file, mach_error_string(ret));
#endif

}

Memory::Memory() {
    pid = 0;
    pmach_port = 0;
}

pid_t Memory::Attach(const std::string &procName) {
    Close();
    PidFromName(procName.c_str());
    if (pid) {
        Open();
        QueryRegions();
    }
    return pid;
}

void Memory::Detach() {
    Close();
}

kern_return_t Memory::Read(uintptr_t address, size_t size, void *buffer) {
    assert(size != 0 || address != 0);
    
    vm_size_t data_cnt = size;
    
    kern_return_t kret = vm_read_overwrite(pmach_port, address, size, (vm_address_t) buffer, &data_cnt);
    
    if (kret != KERN_SUCCESS)
        MACH_CHECK_ERROR(kret)
    
    return kret;
}

kern_return_t Memory::Write(uintptr_t address, size_t size, void *buffer) {
    if (address == 0 || size == 0)
        return KERN_INVALID_ARGUMENT;
    
    kern_return_t kret;
    if ((kret = vm_write(pmach_port, address, vm_offset_t(buffer), size)) != KERN_SUCCESS) {
        vm_prot_t backup = 0;
        
        for (auto it = segments.begin(); it != segments.end(); it++) {
            if (it->address <= address && address < (it->address + it->size)) {
                backup = it->info.protection;
                break;
            }
        }
        vm_protect(pmach_port, address, size, 0, backup | VM_PROT_WRITE | VM_PROT_READ);
        kret = vm_write(pmach_port, address, vm_offset_t(buffer), size);
        vm_protect(pmach_port, address, size, 0, backup);
        if (kret != KERN_SUCCESS) {
            MACH_CHECK_ERROR(kret)
            return kret;
        }
    }
    return kret;
}

kern_return_t Memory::Protect(uintptr_t address, size_t size, vm_prot_t protection, vm_prot_t *backup) {
    if (backup != nullptr) {
        for (auto it = segments.begin(); it != segments.end(); ++it) {
            if (it->address <= address && address <= (it->address + it->size)) {
                *backup = it->info.protection;
                break;
            }
        }
    }
    
    kern_return_t kret = vm_protect(pmach_port, vm_address_t(address), size, 0, protection);
    if (kret != KERN_SUCCESS)
        MACH_CHECK_ERROR(kret)
    
    return kret;
}

uintptr_t Memory::Allocate(size_t size, vm_prot_t prot, uintptr_t BaseAddr) {
    kern_return_t kret = KERN_SUCCESS;
    boolean_t anywhere;
    unsigned char *address;
    address = (unsigned char *) &BaseAddr;
    
    if (size == 0)
        printf("Warning -- size to allocate is zero.\n");
    
    if (*address == 0)
        anywhere = TRUE;
    else
        anywhere = FALSE;
    
    kret = vm_allocate(pmach_port, (vm_address_t *) &address, size, anywhere);
    if (kret)
        MACH_CHECK_ERROR(kret)
    
    kret = Protect((uintptr_t) address, size, prot);
    if (kret)
        MACH_CHECK_ERROR(kret)
    
    return (uintptr_t) address;
}

kern_return_t Memory::Free(uintptr_t address, size_t size) {
    kern_return_t kret = vm_deallocate(pmach_port, vm_address_t(address), size);
    if (kret)
        MACH_CHECK_ERROR(kret)
    return kret;
}

#define kMaxStringLength 8192

std::string Memory::ReadString(uintptr_t address) {
    std::string result;
    for (size_t i = 0; i < kMaxStringLength; i++) {
        char c = Read<unsigned char>(address + i);
        if (c)
            result.append(&c, sizeof(unsigned char));
        else
            break;
    }
    return result;
}

int Memory::Open() {
    kern_return_t kret = task_for_pid(mach_task_self(), pid, &pmach_port);
    if (kret != KERN_SUCCESS) {
#ifndef NDEBUG
        printf("task_for_pid() error, try running as sudo!\n");
#endif
        MACH_CHECK_ERROR(kret)
        return 0;
    }
    return 1;
}

void Memory::Close() {
    pid = 0;
    pmach_port = 0;
}

void Memory::QueryRegions() {
    mach_vm_address_t address = 0x0;
    mach_vm_size_t size;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t objectName = MACH_PORT_NULL;
    
    while (mach_vm_region(pmach_port, &address, &size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info,
                          &infoCount, &objectName) == 0) {
        auto region = new MemoryRegion_t;
        region->address = address;
        region->size = size;
        region->info = info;
        segments.push_back(*region);
        address += size;
        delete region;
    }
}

void Memory::PidFromName(const char *procName) {
    pid = 0;
    kinfo_proc *procList;
    size_t procCount;
    
    GetProcessList(&procList, &procCount);
    
    for (size_t j = 0; j < procCount + 1; j++) {
        if (strcmp(procList[j].kp_proc.p_comm, procName) == 0)
            pid = procList[j].kp_proc.p_pid;
    }
    
    free(procList);
}

int Memory::GetProcessList(kinfo_proc **procList, size_t *procCount) {
    int err;
    kinfo_proc *result;
    int done;
    static const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t length;
    
    assert(procList != nullptr);
    assert(procCount != nullptr);
    *procCount = 0;
    
    result = nullptr;
    done = 0;
    do {
        assert(result == nullptr);
        
        length = 0;
        err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, nullptr, &length, nullptr, 0);
        if (err == -1) {
            err = errno;
        }
        if (err == 0) {
            result = (kinfo_proc *) malloc(length);
            if (result == nullptr) {
                err = ENOMEM;
            }
        }
        
        if (err == 0) {
            err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, result, &length, nullptr, 0);
            if (err == -1) {
                err = errno;
            }
            if (err == 0) {
                done = 1;
            } else if (err == ENOMEM) {
                assert(result != nullptr);
                free(result);
                result = nullptr;
                err = 0;
            }
        }
    } while (err == 0 && !done);
    
    if (err != 0 && result != nullptr) {
        free(result);
        result = nullptr;
    }
    *procList = result;
    if (err == 0) {
        *procCount = length / sizeof(kinfo_proc);
    }
    
    assert((err == 0) == (*procList != nullptr));
    return err;
}
