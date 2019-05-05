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

#ifndef __xnumem__ProcessMemory__
#define __xnumem__ProcessMemory__

#include <mach/mach.h>
#include <iostream>
#include <vector>
#include <initializer_list>

typedef struct MemoryRegion {
	mach_vm_address_t address;
	mach_vm_size_t size;
	vm_region_basic_info_data_64_t info;
} MemoryRegion_t;

class ProcessMemory
{
    friend class xnu_proc;
public:
    ProcessMemory( class xnu_proc* process );
    ~ProcessMemory();
    
    /**
     Read data.
     
     @param address -- Memory address.
     @param size    -- Size to read
     @param buffer  -- Output buffer
     @return Status.
     */
    kern_return_t Read ( uintptr_t address, size_t size, void * buffer );
    
    /**
     Write data.
     
     @param address -- Memory address.
     @param size    -- Data size.
     @param buffer  -- Input buffer.
     @return Status.
     */
    kern_return_t Write( uintptr_t address, size_t size, void * buffer );

    /**
     Copy a region of memory from one address to the other.
     
     @param source_address -- Memory address.
     @param size    -- Size to read
     @param buffer  -- Output buffer
     @return Status.
     */
    kern_return_t Copy ( uintptr_t source_address, size_t size, uintptr_t dest_address );
    
    /**
     Change memory protection.
     
     @param address    -- Memory address.
     @param size       -- Size.
     @param protection -- New protection.
     @param backup     -- Backup of old protection. (optional)
     @return Status.
     */
    kern_return_t Protect( uintptr_t address, size_t size, vm_prot_t protection, vm_prot_t  * backup = nullptr);
    
    /**
     Allocate new memory block.
     
     @param size     -- Block size.
     @param prot     -- Desired protection. (optional)
     @param BaseAddr -- Desired base address. If left zero it will find a suitable address and return it.
                        If the region as specified by the given base address and size would not lie within the task's
                        un-allocated memory, the kernel does not allocate the region. (optional)
     @return Actual address of the memory block.
     */
    uintptr_t Allocate( size_t size, vm_prot_t prot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE, uintptr_t BaseAddr = 0);
    
    /**
     Free memory.
     
     @param address  -- Memory address to release.
     @param size     -- Region size.
     @return Status.
     */
    kern_return_t Free(uintptr_t address, size_t size);
    
    /**
     Dump memory segment info.
     
     @param Void.
     @return Status.
     */
    kern_return_t PrintSegments();
    
    /**
     Read memory.
     
     @param dwAddress -- Memory address to read from.
     @return Output data.
     */
    template<class T>
    inline T Read( uintptr_t dwAddress )
    {
        T res;
        Read( dwAddress, sizeof(T), &res );
        return res;
    }

//    template<class T>
//    inline T Read( int dwAddress )
//    {
//        T res;
//        Read( uintptr_t(dwAddress), sizeof(T), &res );
//        return res;
//    }

    template <typename T>
    T ReadMem(std::initializer_list<uintptr_t> addr)
    {
        T result = T();
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset + *it, sizeof(uint32_t), &offset);
            else
                Read(offset + *it, sizeof(result), &result);
        return result;
    }
    
    template <typename T, size_t size>
    std::array<T, size> ReadMem(std::initializer_list<uintptr_t> addr)
    {
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

//    template<class T>
//    T Read(uintptr_t base,std::initializer_list<int> Offset)
//    {
//        unsigned long n=Offset.size();
//        auto p=Offset.begin();
//        uintptr_t ptr=base;
//        for(unsigned long i=0;i<n;i++)
//        {
//            ptr=uintptr_t(Read<int>(ptr));
//            ptr+=uintptr_t(p[i]);
//        }
//        return Read<T>(ptr);
//    }
    
    /**
     Write memory.
     
     @param dwAddress -- Memory address to write to.
     @param data      -- Data to write.
     @return Status.
     */
    template<class T>
    inline kern_return_t Write( const T& data, uintptr_t dwAddress )
    {
        return Write( dwAddress, sizeof(T), (void *)&data );
    }

//    template<class T>
//    inline kern_return_t Write( const T& data, int dwAddress )
//    {
//        return Write( uintptr_t(dwAddress), sizeof(T), (void *)&data );
//    }

//    template<class T>
//    kern_return_t Write(const T& data,uintptr_t base,std::initializer_list<int> Offset)
//    {
//        unsigned long n=Offset.size();
//        auto p=Offset.begin();
//        uintptr_t ptr=base;
//        for(unsigned long i=0;i<n;i++)
//        {
//            ptr=uintptr_t(Read<int>(ptr));
//            ptr+=uintptr_t(p[i]);
//        }
//        return Write<T>(data,ptr);
//    }

    template <typename T>
    void WriteMem(T value, std::initializer_list<uintptr_t> addr)
    {
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                Read(offset+*it,sizeof(uint32_t),&offset);
            else
                Write(offset+*it,sizeof(value),&value);
    }
    
    /**
     Reads a NULL-terminated string from another task.
     Warning!  This will not read any strings longer than kMaxStringLength-1.
     
     @param address -- Memory address of the string.
     @return the string at address.
     */
    const char * ReadString( const uint64_t address );
    
    /**
     Memory regions
     
     @param void
     @return Vector containing all region information.
     */
    inline std::vector<MemoryRegion_t> segments() { return _segments; }
    
    // Subroutines
    inline class ProcessCore& core() { return _core; }
    
private:
    ProcessMemory( const ProcessMemory& ) = delete;
    ProcessMemory& operator =(const ProcessMemory&) = delete;
    
    const char * unparse_inheritance    (vm_inherit_t i);
    const char * behavior_to_text       (vm_behavior_t b);
    char       * protection_bits_to_rwx (vm_prot_t p);
    size_t      _word_align             (size_t size);
    
    // Retrieve all region info structures
    kern_return_t QueryRegions();
    std::vector<MemoryRegion_t> _segments;
    
    // Returns the size of the memory region containing |address| and the
    // number of bytes from |address| to the end of the region.
    // We potentially, will extend the size of the original
    // region by the size of the following region if it's contiguous with the
    // first in order to handle cases when we're reading strings and they
    // straddle two vm regions.
    mach_vm_size_t GetMemoryRegionSize(const uint64_t address, mach_vm_size_t *size_to_end);
    
private:
    class xnu_proc     *_process;   // Owning process object
    class ProcessCore& _core;   // Core routines
    
};

#endif /* defined(__xnumem__ProcessMemory__) */
