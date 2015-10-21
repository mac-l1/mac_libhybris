/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authored by: mac-l1 <mac.level1@gmail.com>
 */

#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hybris/internal/binding.h>
#include <android/libon2/vpu_api.h>
#include <android/libon2/vpu.h>
//#include <android/libon2/vpu_mem_pool.h> // NOT INCLUDED IN RK3188


#define RK_ON2_LIBRARY_PATH "/system/lib/librk_on2.so"
#define VPU_LIBRARY_PATH "/system/lib/libvpu.so"

HYBRIS_LIBRARY_INITIALIZE(vpu, VPU_LIBRARY_PATH);
HYBRIS_LIBRARY_INITIALIZE(rk_on2, RK_ON2_LIBRARY_PATH);

int vpu()
{
	/* Both are defined via HYBRIS_LIBRARY_INITIALIZE */
	hybris_vpu_initialize();
	return vpu_handle ? 1 : 0;
}

// vpu_api.h
//
// RK_S32 vpu_open_context(struct VpuCodecContext **ctx);
HYBRIS_IMPLEMENT_FUNCTION1(rk_on2, RK_S32, vpu_open_context, struct VpuCodecContext **);
// RK_S32 vpu_close_context(struct VpuCodecContext **ctx);
HYBRIS_IMPLEMENT_FUNCTION1(rk_on2, RK_S32, vpu_close_context, struct VpuCodecContext **);

// vpu_mem.h
//
/* SW/HW shared memory */
// RK_S32 VPUMallocLinear(VPUMemLinear_t *p, RK_U32 size);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMallocLinear, VPUMemLinear_t *,RK_U32);
// RK_S32 VPUFreeLinear(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUFreeLinear, VPUMemLinear_t *);
// RK_S32 VPUMemDuplicate(VPUMemLinear_t *dst, VPUMemLinear_t *src);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMemDuplicate, VPUMemLinear_t *,VPUMemLinear_t *); // NOT TESTED
// RK_S32 VPUMemLink(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemLink, VPUMemLinear_t *);
// RK_S32 VPUMemFlush(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemFlush, VPUMemLinear_t *); // NOT TESTED
// RK_S32 VPUMemClean(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemClean, VPUMemLinear_t *); // NOT TESTED
// RK_S32 VPUMemInvalidate(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemInvalidate, VPUMemLinear_t *); // NOT TESTED
// RK_U32 *VPUMemVirtual(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_U32*, VPUMemVirtual, VPUMemLinear_t *); // NOT TESTED
/* FOLLOWING ARE NOT INCLUDED FOR RK3188
// RK_S32 VPUMemPoolSet(RK_U32 size, RK_U32 count);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMemPoolSet, RK_U32,RK_U32); // NOT TESTED
// RK_S32 VPUMemPoolUnset(RK_U32 size, RK_U32 count);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMemPoolUnset, RK_U32,RK_U32); // NOT TESTED
// RK_S32 VPUMemGetFreeSize();
HYBRIS_IMPLEMENT_FUNCTION0(vpu, RK_S32, VPUMemGetFreeSize ); // NOT TESTED
// RK_S32 VPUMallocLinearFromRender(VPUMemLinear_t *p, RK_U32 size, void *ctx);
HYBRIS_IMPLEMENT_FUNCTION3(vpu, RK_S32, VPUMallocLinearFromRender, VPUMemLinear_t *,RK_U32,void*); // NOT TESTED
// RK_S32 VPUMemGetFD(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemGetFD, VPUMemLinear_t *); // NOT TESTED
// RK_S32 VPUMemGetREF(VPUMemLinear_t *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemGetREF, VPUMemLinear_t *); // NOT TESTED
// RK_S32 VPUMemJudgeIommu();
HYBRIS_IMPLEMENT_FUNCTION0(vpu, RK_S32, VPUMemJudgeIommu ); // NOT TESTED
// new malloc interface by with allocator for memory management
// maxSize equal to zero means no limit
// RK_HANDLE VPUMemAllocatorCreate(RK_U32 maxSize);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_HANDLE, VPUMemAllocatorCreate, RK_U32 ); // NOT TESTED
// RK_S32 VPUMemMalloc(RK_HANDLE allocator, RK_U32 size, RK_S32 timeout, VPUMemHnd **p);
HYBRIS_IMPLEMENT_FUNCTION4(vpu, RK_S32, VPUMemMalloc, RK_HANDLE,RK_U32,RK_S32,VPUMemHnd** ); // NOT TESTED
// RK_S32 VPUMemImport(RK_HANDLE allocator, RK_S32 mem_fd, VPUMemHnd **p);
HYBRIS_IMPLEMENT_FUNCTION3(vpu, RK_S32, VPUMemImport, RK_HANDLE,RK_S32,VPUMemHnd** ); // NOT TESTED
// RK_S32 VPUMemFree(VPUMemHnd *p);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUMemFree,VPUMemHnd* ); // NOT TESTED
// RK_S32 VPUMemShare(VPUMemHnd *p, RK_S32 *mem_fd);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMemShare,VPUMemHnd*, RK_S32* ); // NOT TESTED
// RK_S32 VPUMemCache(VPUMemHnd *p, VPUCacheOp_E cmd);
HYBRIS_IMPLEMENT_FUNCTION2(vpu, RK_S32, VPUMemCache,VPUMemHnd*,VPUCacheOp_E ); // NOT TESTED
// void VPUMemAllocatorDestory(RK_HANDLE allocator);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(vpu, VPUMemAllocatorDestory,RK_HANDLE ); // NOT TESTED

// vpu_mem_pool.h
//
// vpu_display_mem_pool* open_vpu_memory_pool();
HYBRIS_IMPLEMENT_FUNCTION0(vpu, vpu_display_mem_pool*, open_vpu_memory_pool); // NOT TESTED
// void close_vpu_memory_pool(vpu_display_mem_pool *p);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(vpu, close_vpu_memory_pool,vpu_display_mem_pool* ); // NOT TESTED

#if ENABLE_VPU_MEMORY_POOL_ALLOCATOR
// int create_vpu_memory_pool_allocator(vpu_display_mem_pool **ipool, int num, int size);
HYBRIS_IMPLEMENT_FUNCTION3(vpu, int, create_vpu_memory_pool_allocator, vpu_display_mem_pool**,int,int ); // NOT TESTED
// void release_vpu_memory_pool_allocator(vpu_display_mem_pool *ipool);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(vpu, release_vpu_memory_pool_allocator,vpu_display_mem_pool* ); // NOT TESTED
#endif
*/

// vpu.h
//
// int VPUClientInit(VPU_CLIENT_TYPE type);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, int, VPUClientInit,VPU_CLIENT_TYPE ); // NOT TESTED
// RK_S32 VPUClientRelease(int socket);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, RK_S32, VPUClientRelease,int ); // NOT TESTED
// RK_S32 VPUClientSendReg(int socket, RK_U32 *regs, RK_U32 nregs);
HYBRIS_IMPLEMENT_FUNCTION3(vpu, RK_S32, VPUClientSendReg,int,RK_U32*,RK_U32 ); // NOT TESTED
// RK_S32 VPUClientWaitResult(int socket, RK_U32 *regs, RK_U32 nregs, VPU_CMD_TYPE *cmd, RK_S32 *len);
HYBRIS_IMPLEMENT_FUNCTION5(vpu, RK_S32, VPUClientWaitResult,int,RK_U32*,RK_U32,VPU_CMD_TYPE*,RK_S32* ); // NOT TESTED
// RK_S32 VPUClientGetHwCfg(int socket, RK_U32 *cfg, RK_U32 cfg_size);
HYBRIS_IMPLEMENT_FUNCTION3(vpu, RK_S32, VPUClientGetHwCfg,int,RK_U32*,RK_U32 ); // NOT TESTED
// RK_S32 VPUClientGetIOMMUStatus();
HYBRIS_IMPLEMENT_FUNCTION0(vpu, RK_S32, VPUClientGetIOMMUStatus); // NOT TESTED
// 
// void *vpu_service(void *);
HYBRIS_IMPLEMENT_FUNCTION1(vpu, void*, vpu_service,void* ); // NOT TESTED

