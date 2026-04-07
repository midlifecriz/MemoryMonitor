#include <iostream>
#include <Windows.h>


void* ptr = nullptr;
size_t size = 4096;


int rewriteMemory(void *ptr) {
    if (ptr == nullptr) return 1;

    std::cout << "___REWRITING MEMORY___" << std::endl;
    std::cout << "Address: " << ptr << std::endl;

    *(int*)ptr = 1234;
    std::cout << "First value: " << *(int*)ptr << std::endl;

    *(int*)ptr = 4321;
    std::cout << "Second value: " << *(int*)ptr << std::endl;

    return 0;
}


LONG CALLBACK Handler(PEXCEPTION_POINTERS pExceptionPointers) {
    std::cout << "Handler called" << std::endl;

    if (pExceptionPointers->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        std::cout << "Exception!!!" << std::endl;
        //element [1] contains virtual address of inaccessible data
        void* address = (void*)pExceptionPointers->ExceptionRecord->ExceptionInformation[1];

        //delete PAGE_GUARD to take context
        DWORD oldModifier;
        VirtualProtect(ptr, size, PAGE_READWRITE, &oldModifier);

        CONTEXT* context = pExceptionPointers->ContextRecord; 
        //analyse context

        VirtualProtect(ptr, size, PAGE_GUARD, &oldModifier);

        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


int main() {
    //size_t size = 4096; //1 page
    ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_GUARD);
    if (ptr == nullptr) return 1;

    //DWORD oldProtect;
    //VirtualProtect(ptr, size, PAGE_GUARD, &oldProtect);

    PVOID vehHandle = AddVectoredExceptionHandler(1, Handler);

    rewriteMemory(ptr);

    RemoveVectoredExceptionHandler(vehHandle);

    VirtualFree(ptr, 0, MEM_RELEASE);
    return 0;   
}

