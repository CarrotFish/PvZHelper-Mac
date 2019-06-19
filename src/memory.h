#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <sys/sysctl.h>
#include <mach/mach.h>

typedef struct MemoryRegion {
    mach_vm_address_t address;
    mach_vm_size_t size;
    vm_region_basic_info_data_64_t info;
} MemoryRegion_t;

#if (!defined __GNUC__ || __GNUC__ < 2 || __GNUC_MINOR__ < (defined __cplusplus ? 6 : 4))
#define __MACH_CHECK_FUNCTION (reinterpret_cast<__const char *>(0))
#else
#define __MACH_CHECK_FUNCTION __PRETTY_FUNCTION__
#endif

#define MACH_CHECK_ERROR(ret) \
mach_check_error (ret, __FILE__, __LINE__, __MACH_CHECK_FUNCTION);

void mach_check_error(kern_return_t ret, const char *file, unsigned int line, const char *func);

class Memory {
public:
    Memory();
    
    pid_t Attach(const std::string &procName);
    
    void Detach();
    
    kern_return_t Read(uintptr_t address, size_t size, void *buffer);
    
    kern_return_t Write(uintptr_t address, size_t size, void *buffer);
    
    kern_return_t Protect(uintptr_t address, size_t size, vm_prot_t protection, vm_prot_t *backup = nullptr);
    
    uintptr_t Allocate(size_t size, vm_prot_t prot = VM_PROT_ALL, uintptr_t BaseAddr = 0);
    
    kern_return_t Free(uintptr_t address, size_t size);
    
    std::string ReadString(uintptr_t address);
    
    template<typename T>
    T Read(uintptr_t dwAddress) {
        T result;
        Read(dwAddress, sizeof(T), &result);
        return result;
    }
    
    template<typename T>
    T ReadMemory(std::initializer_list<uintptr_t> addr) {
        T result = T();
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset + *it, sizeof(uint32_t), &offset);
            else
                Read(offset + *it, sizeof(result), &result);
        return result;
    }
    
    template<typename T, size_t size>
    std::array<T, size> ReadMemory(std::initializer_list<uintptr_t> addr) {
        std::array<T, size> result = {T()};
        
        T buff[size] = {0};
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset + *it, sizeof(uint32_t), &offset);
            else
                Read(offset + *it, sizeof(buff), &buff);
        for (size_t i = 0; i < size; i++)
            result[i] = buff[i];
        return result;
    }
    
    template<class T>
    kern_return_t Write(const T &data, uintptr_t dwAddress) {
        return Write(dwAddress, sizeof(T), (void *) &data);
    }
    
    template<typename T>
    void WriteMemory(T value, std::initializer_list<uintptr_t> addr) {
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset + *it, sizeof(uint32_t), &offset);
            else
                Write(offset + *it, sizeof(value), &value);
    }
    
    template<typename T, size_t size>
    void WriteMemory(std::array<T, size> value, std::initializer_list<uintptr_t> addr) {
        T buff[size];
        for (size_t i = 0; i < size; i++)
            buff[i] = value[i];
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset + *it, sizeof(uint32_t), &offset);
            else
                Write(offset + *it, sizeof(buff), &buff);
    }

private:
    int Open();
    
    void Close();
    
    void QueryRegions();
    
    static int GetProcessList(kinfo_proc **procList, size_t *procCount);
    
    void PidFromName(const char *procName);
    
    pid_t pid;
    mach_port_t pmach_port;
    std::vector<MemoryRegion_t> segments;
};


#endif //MEMORY_H
