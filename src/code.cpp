#include "code.h"
#include "xnumem.h"
#include <mach/thread_act.h>
#include <mach/thread_status.h>
#include <array>
#include <QDebug>

#define STACK_SIZE 65536

Code::Code(class xnu_proc &process) :
        _process(process), _core(process.core()), _memory(process.memory()) {
    unsigned int page = 4;
    code = new unsigned char[4096 * page];
    length = 0;
}

Code::~Code() {
    delete[] code;
}

void Code::asm_init() {
    length = 23;                            //check the flag && set flag
    asm_mov_dword_ptr_exx(0x0, Reg::EAX);   //back up registers
    asm_mov_dword_ptr_exx(0x0, Reg::EBX);
    asm_mov_dword_ptr_exx(0x0, Reg::ECX);
    asm_mov_dword_ptr_exx(0x0, Reg::EDX);
    asm_mov_dword_ptr_exx(0x0, Reg::ESI);
    asm_mov_dword_ptr_exx(0x0, Reg::EDI);
    asm_mov_dword_ptr_exx(0x0, Reg::EBP);
    asm_mov_dword_ptr_exx(0x0, Reg::ESP);
    asm_mov_exx(Reg::EBP, 0x0);             //move stack registers
    asm_mov_exx(Reg::ESP, 0x0);//length = 80
}

void Code::asm_add_byte(unsigned char value) {
    code[length] = value;
    length += 1;
}

void Code::asm_add_word(unsigned short value) {
    (unsigned short &) code[length] = value;
    length += 2;
}

void Code::asm_add_dword(unsigned int value) {
    (unsigned int &) code[length] = value;
    length += 4;
}

void Code::asm_push(int value) {
    asm_add_byte((unsigned char) (0x68));
    asm_add_dword(value);
}

void Code::asm_mov_exx(Reg reg, int value) {
    unsigned char mov_exx[] = {0xB8, 0xBB, 0xB9, 0xBA, 0xBE, 0xBF, 0xBD, 0xBC};
    asm_add_byte(mov_exx[static_cast<unsigned int>(reg)]);
    asm_add_dword(value);
}

void Code::asm_mov_dword_ptr_exx_add(Reg reg, int offset, int value) {
    unsigned short move_dword_ptr_exx_add[] = {0x40C7, 0xC347, 0x41C7, 0x42C7, 0x46C7, 0x47C7, 0x45C7, 0x44C7};
    asm_add_word(move_dword_ptr_exx_add[static_cast<unsigned int>(reg)]);
    if (reg == Reg::ESP)
        asm_add_byte(0x24);
    asm_add_byte(offset);
    asm_add_dword(value);
}

void Code::asm_add_exx(Reg reg, int value) {
    unsigned char add_exx[] = {0x05, 0xC3, 0xC1, 0xC2, 0xC6, 0xC7, 0xC5, 0xC4};
    if (reg != Reg::EAX)
        asm_add_byte((unsigned char) (0x81));
    asm_add_byte(add_exx[static_cast<unsigned int>(reg)]);
    asm_add_dword(value);
}

void Code::asm_mov_exx_dword_ptr(Reg reg, int value) {
    unsigned char mov_exx_dword_ptr[] = {0xA1, 0x1D, 0x0D, 0x15, 0x35, 0x3D, 0x2D, 0x25};
    if (reg != Reg::EAX)
        asm_add_byte((unsigned char) (0x8B));
    asm_add_byte(mov_exx_dword_ptr[static_cast<unsigned int>(reg)]);
    asm_add_dword(value);
}

void Code::asm_mov_dword_ptr_exx(int value, Reg reg) {
    unsigned char mov_dword_ptr_exx[] = {0xA3, 0x1D, 0x0D, 0x15, 0x35, 0x3D, 0x2D, 0x25};
    if (reg != Reg::EAX)
        asm_add_byte((unsigned char) (0x89));
    asm_add_byte(mov_dword_ptr_exx[static_cast<unsigned int>(reg)]);
    asm_add_dword(value);
}

void Code::asm_mov_exx_dword_ptr_exx_add(Reg reg, int value) {
    unsigned char mov_exx_dword_ptr_exx_add[] = {0x80, 0x9B, 0x89, 0x92, 0xB6, 0xBF, 0xAD, 0xA4};
    asm_add_byte((unsigned char) (0x8B));
    asm_add_byte(mov_exx_dword_ptr_exx_add[static_cast<unsigned int>(reg)]);
    if (reg == Reg::ESP)
        asm_add_byte((unsigned char) (0x24));
    asm_add_dword(value);
}

void Code::asm_mov_ptr_esp_add_exx(unsigned char offset, Reg reg) {
    unsigned char asm_mov_ptr_esp_add_exx[] = {0x44, 0x5C, 0x4C, 0x54, 0x74, 0x7C, 0x6C, 0x64};
    asm_add_byte(0x89);
    asm_add_byte(asm_mov_ptr_esp_add_exx[static_cast<unsigned int>(reg)]);
    asm_add_byte(0x24);
    asm_add_byte(offset);
}

void Code::asm_mov_dword_ptr_esp_add(int offset, int value) {
    asm_add_word(0x84C7);
    asm_add_byte(0x24);
    asm_add_dword(offset);
    asm_add_dword(value);
}

void Code::asm_push_exx(Reg reg) {
    unsigned char push_exx[] = {0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, 0x54};
    asm_add_byte(push_exx[static_cast<unsigned int>(reg)]);
}

void Code::asm_pop_exx(Reg reg) {
    unsigned char pop_exx[] = {0x58, 0x5B, 0x59, 0x5A, 0x5E, 0x5F, 0x5D, 0x5C};
    asm_add_byte(pop_exx[static_cast<unsigned int>(reg)]);
}

void Code::asm_call(int addr) {
    asm_add_byte((unsigned char) (0xE8));
    asm_add_dword((unsigned int) (0x00000002));
    asm_add_word((unsigned short) (0x06EB));
    asm_push(addr);
    asm_add_byte((unsigned char) (0xC3));
}

void Code::asm_ret() {
    asm_mov_exx_dword_ptr(Reg::EAX, 0x0);       //restore the registers
    asm_mov_exx_dword_ptr(Reg::EBX, 0x0);
    asm_mov_exx_dword_ptr(Reg::ECX, 0x0);
    asm_mov_exx_dword_ptr(Reg::EDX, 0x0);
    asm_mov_exx_dword_ptr(Reg::ESI, 0x0);
    asm_mov_exx_dword_ptr(Reg::EDI, 0x0);
    asm_mov_exx_dword_ptr(Reg::EBP, 0x0);
    asm_mov_exx_dword_ptr(Reg::ESP, 0x0);//length += 47
    length += 10;                               //set flag(2)
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);  //the original code
    // asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_add_word(0x888B);
    asm_add_dword(0x0000026C);//mov ecx, [eax+0x26C]
    // asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x80);
    asm_add_word(0x818B);
    asm_add_dword(0x00000088);//mov eax, [ecx+0x88]
    asm_add_byte(0xE9);                         //jump back
    asm_add_dword(0x00000000);//length += 79
    // asm_add_byte((unsigned char)(0xC3));
    // asm_add_byte((unsigned char) 0x90);
    // asm_add_byte((unsigned char) 0xEB);
    // asm_add_byte((unsigned char) 0xFD);
}

// void Code::asm_code_inject() {
//     mach_vm_address_t remoteStack = _memory.Allocate(STACK_SIZE, VM_PROT_READ | VM_PROT_WRITE);
//     mach_vm_address_t remoteCode = _memory.Allocate(length, VM_PROT_ALL);
//     x86_thread_state32_t remoteThreadState32;
//     thread_act_t remoteThread;
//     mach_msg_type_number_t remoteThreadStateCount = x86_THREAD_STATE32_COUNT;
//
//     qDebug() << hex << remoteCode;
//     qDebug() << hex << remoteStack;
//
//     vm_write(_core._pmach_port, remoteCode, vm_offset_t(code), length);
//     vm_protect(_core._pmach_port, remoteCode, length, FALSE, VM_PROT_READ | VM_PROT_EXECUTE);
//     // _memory.Write(remoteCode, length, code);
//
//     memset(&remoteThreadState32, '\0', sizeof(remoteThreadState32));
//     remoteStack += (STACK_SIZE / 2); // this is the real stack
//     remoteStack -= 4;
//
//     remoteThreadState32.__eip = (unsigned int) remoteCode;
//     // set remote Stack Pointer
//     remoteThreadState32.__esp = (unsigned int) remoteStack;
//     remoteThreadState32.__ebp = (unsigned int) remoteStack;
//
//     remoteThreadState32.__cs = 0x1B;
//     remoteThreadState32.__ds = 0x23;
//     remoteThreadState32.__es = 0x23;
//     remoteThreadState32.__fs = 0x0;
//     remoteThreadState32.__gs = 0xF;
//     remoteThreadState32.__ss = 0x23;
//
//     kern_return_t kret = thread_create_running(_core._pmach_port, x86_THREAD_STATE32,
//                                                (thread_state_t) &remoteThreadState32, x86_THREAD_STATE32_COUNT,
//                                                &remoteThread);
//     if (kret != KERN_SUCCESS)
//         MACH_CHECK_ERROR(kret)
//
//     for (;;) {
//         kret = thread_get_state(remoteThread, x86_THREAD_STATE32, (thread_state_t) &remoteThreadState32,
//                                 &remoteThreadStateCount);
//         if (kret != KERN_SUCCESS)
//             MACH_CHECK_ERROR(kret)
//         uint32_t eip = remoteThreadState32.__eip;
//         if (eip >= remoteCode + length - 3 && eip <= remoteCode + length - 1) {
//             kret = thread_terminate(remoteThread);
//             if (kret != KERN_SUCCESS)
//                 MACH_CHECK_ERROR(kret)
//             _memory.Free(remoteStack, STACK_SIZE);
//             _memory.Free(remoteCode, length);
//             return;
//         } else usleep(10000);
//     }
// }

void Code::asm_code_inject() {
    int addr = 0x26C710; //0x2E0FE;
    for (int i = 0; i < 100; i++) {
        if (_memory.Read<unsigned char>(addr) == 0xE9)
            usleep(10000);                          //avoid conflict
    }
    uint32_t remoteStack = _memory.Allocate(STACK_SIZE, VM_PROT_READ | VM_PROT_WRITE);
    uint32_t remoteCode = _memory.Allocate(length, VM_PROT_ALL);
    uint32_t ThreadState = _memory.Allocate(40, VM_PROT_READ | VM_PROT_WRITE);
    _memory.Write<int>(0x0, ThreadState);
    _memory.Write<int>(0x0, ThreadState + 4);
    qDebug() << hex << remoteCode;
    qDebug() << hex << remoteStack;
    qDebug() << hex << ThreadState;
    uint32_t offset = length - 13 - 22;
    std::array<unsigned char, 23> ar = {0xA1, 0x00, 0x00, 0x00, 0x00, 0x84, 0xC0, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00,
                                        0xC7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    memcpy(&ar[1], &ThreadState, 4);
    memcpy(&ar[9], &offset, 4);
    memcpy(&ar[15], &ThreadState, 4);
    memcpy(code, &ar[0], 23);
    int address;
    for (int i = 0; i < 8; i++) {
        address = ThreadState + 8 + 4 * i;
        memcpy(code + 24 + 6 * i, &address, 4);
        memcpy(code + length - 78 + 6 * i, &address, 4);
    }
    memcpy(code + 71, &remoteStack, 4);
    memcpy(code + 76, &remoteStack, 4);
    address = ThreadState + 4;
    std::array<unsigned char, 10> ar2 = {0xC7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    memcpy(&ar2[2], &address, 4);
    memcpy(code + length - 32, &ar2[0], 10);
    offset = addr + 6 - remoteCode - length;
    memcpy(code + length - 4, &offset, 4);
    std::array<unsigned char, 6> ar3 = {0xE9, 0x00, 0x00, 0x00, 0x00, 0x90};//, 0x90, 0x90, 0x90};
    offset = remoteCode - addr - 5;
    memcpy(&ar3[1], &offset, 4);
    
    _memory.Write(remoteCode, length, code);    //write code
    vm_protect(_core._pmach_port, remoteCode, length, FALSE, VM_PROT_READ | VM_PROT_EXECUTE);
    _memory.Write(ar3, addr);                //replace the original code
    
    for (;;) {
        if (_memory.Read<int>(ThreadState + 4) == 1) {  //check flag(2)
            std::array<unsigned char, 6> ar4 = {0x8B, 0x81, 0x88, 0x00, 0x00, 0x00};//, 0x00, 0x00, 0x00};
            _memory.Write(ar4, addr);        //restore the original code
            usleep(10000);
            _memory.Free(ThreadState, 40);      //free the allocated regions
            _memory.Free(remoteCode, length);
            _memory.Free(remoteStack, STACK_SIZE);
            break;
        } else
            usleep(10000);
    }
}

void Code::asm_set_plant(int row, int column, int type, bool imitater, bool iz_style) {
    if (imitater && !iz_style) {
        asm_mov_dword_ptr_esp_add(0x10, type);
        type = 48;
    } else {
        asm_mov_dword_ptr_esp_add(0x10, -1);
    }
    asm_mov_dword_ptr_esp_add(0xC, type);
    asm_mov_dword_ptr_esp_add(0x8, row);
    asm_mov_dword_ptr_esp_add(0x4, column);
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x2A0C2);
    
    if(iz_style){
        asm_mov_ptr_esp_add_exx(0x4, Reg::EAX);
        asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
        asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
        asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x154);
        asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
        asm_call(0xACB0C);
    }
}

void Code::asm_set_zombie(int row, int column, int type) {
    asm_mov_dword_ptr_esp_add(0xC, row);
    asm_mov_dword_ptr_esp_add(0x8, column);
    asm_mov_dword_ptr_esp_add(0x4, type);
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x154);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0xAC672);
}

void Code::asm_spawn_zombie(int type) {
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_dword_ptr_esp_add(0x8, 0);//Spawn wave
    asm_mov_dword_ptr_esp_add(0x4, type);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x2CEE2);
}

void Code::asm_put_ladder(int row, int column) {
    asm_mov_dword_ptr_esp_add(0x8, row);
    asm_mov_dword_ptr_esp_add(0x4, column);
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x28DB2);
}

void Code::asm_put_grave(int row, int column) {
    asm_mov_dword_ptr_esp_add(0x8, row);
    asm_mov_dword_ptr_esp_add(0x4, column);
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x27062);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x208890);
}

void Code::asm_put_rake(int row, int column) {
    asm_mov_dword_ptr_esp_add(0x8, row);
    asm_mov_dword_ptr_esp_add(0x4, column);
    asm_mov_exx_dword_ptr(Reg::EAX, 0x35EE64);
    asm_mov_exx_dword_ptr_exx_add(Reg::EAX, 0x780);
    asm_mov_ptr_esp_add_exx(0x0, Reg::EAX);
    asm_call(0x26DA6);
}
