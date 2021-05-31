// codeshaunted - veer
// veer.hh
// single header veer library file
// Copyright 2021 codeshaunted
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VEER_VEER_HH
#define VEER_VEER_HH

#include <cstdint>

#include <Windows.h>

namespace veer {

#define WINDOWS_IGNORE_PACKING_MISMATCH
#pragma pack(1)
struct s_JMPInstruction {
  uint8_t op_code = 0xe9;
  uintptr_t address;
};

void detour(uintptr_t __original, void* __redirect);

void detour(uintptr_t __original, void* __redirect) {
  s_JMPInstruction jmp;

  DWORD protect;
  VirtualProtect((void*)__original, sizeof(s_JMPInstruction), PAGE_EXECUTE_READWRITE, &protect);

  jmp.address = (uintptr_t)__redirect - __original - sizeof(jmp); // gets relative address

  memcpy((void*)__original, &jmp, sizeof(s_JMPInstruction)); // write jmp

  DWORD temp_protect;
  VirtualProtect((void*)__original, sizeof(s_JMPInstruction), protect, &temp_protect);
}

void* trampoline(uintptr_t __original, void* __redirect);

void* trampoline(uintptr_t __original, void* __redirect) {
  void* bridge = VirtualAlloc(nullptr, sizeof(s_JMPInstruction) * 2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

  memcpy(bridge, (void*)__original, sizeof(s_JMPInstruction)); // write bridge bytes

  s_JMPInstruction bridge_jmp;
  bridge_jmp.address = __original - (uintptr_t)bridge - sizeof(s_JMPInstruction); // setup bridge jmp
  
  memcpy((void*)((uintptr_t)bridge + sizeof(s_JMPInstruction) + 1), &bridge_jmp, sizeof(s_JMPInstruction)); // write bridge jmp
  
  detour(__original, __redirect); // detour to redirect

  return bridge;
}

} // namespace veer

#endif // VEER_VEER_HH
