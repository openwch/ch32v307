/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LOS_HOOK_TYPES_PARSE_H
#define _LOS_HOOK_TYPES_PARSE_H

#define ADDR(a) (&(a))
#define ARGS(a) (a)
#define ADDRn(...) _CONCAT(ADDR, _NARGS(__VA_ARGS__))(__VA_ARGS__)
#define ARGSn(...) _CONCAT(ARGS, _NARGS(__VA_ARGS__))(__VA_ARGS__)
#define ARGS0()
#define ADDR0()
#define ARGS1(a) ARGS(a)
#define ADDR1(a) ADDR(a)

#define ARG_const _ARG_const(
#define _ARG_const(a) ARG_CP_##a)
#define ARG_CP_LosSemCB ADDR(
#define ARG_CP_LosTaskCB ADDR(
#define ARG_CP_UINT32 ADDR(
#define ARG_CP_LosMuxCB ADDR(
#define ARG_CP_LosQueueCB ADDR(
#define ARG_UINT32 ARGS(
#define ARG_PEVENT_CB_S ARGS(
#define ARG_void ADDRn(
#define ARG(a) ARG_##a)

#define PARAM_TO_ARGS1(a) ARG(a)
#define PARAM_TO_ARGS2(a, b) ARG(a), PARAM_TO_ARGS1(b)
#define PARAM_TO_ARGS3(a, b, c) ARG(a), PARAM_TO_ARGS2(b, c)
#define PARAM_TO_ARGS4(a, b, c, d) ARG(a), PARAM_TO_ARGS3(b, c, d)
#define PARAM_TO_ARGS5(a, b, c, d, e) ARG(a), PARAM_TO_ARGS4(b, c, d, e)
#define PARAM_TO_ARGS6(a, b, c, d, e, f) ARG(a), PARAM_TO_ARGS5(b, c, d, e, f)
#define PARAM_TO_ARGS7(a, b, c, d, e, f, g) ARG(a), PARAM_TO_ARGS6(b, c, d, e, f, g)

#define _ZERO_ARGS  7, 6, 5, 4, 3, 2, 1, 0
#define ___NARGS(a, b, c, d, e, f, g, h, n, ...)    n
#define __NARGS(...) ___NARGS(__VA_ARGS__)
#define _NARGS(...) __NARGS(x, __VA_ARGS__##_ZERO_ARGS, 7, 6, 5, 4, 3, 2, 1, 0)
#define _CONCAT(a, b)  a##b
#define CONCAT(a, b)  _CONCAT(a, b)

#define PARAM_TO_ARGS(...) _CONCAT(PARAM_TO_ARGS, _NARGS(__VA_ARGS__))(__VA_ARGS__)
#define OS_HOOK_PARAM_TO_ARGS(paramList) (PARAM_TO_ARGS paramList)

#endif /* _LOS_HOOK_TYPES_PARSE_H */
