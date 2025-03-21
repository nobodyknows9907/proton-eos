/*
 * Copyright 2016 Józef Kucia for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __VKD3D_TEST_H
#define __VKD3D_TEST_H

#include "vkd3d_common.h"
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char *vkd3d_test_name;
extern const char *vkd3d_test_platform;

static void vkd3d_test_start_todo(bool is_todo);
static int vkd3d_test_loop_todo(void);
static void vkd3d_test_end_todo(void);

#define START_TEST(name) \
        const char *vkd3d_test_name = #name; \
        static void vkd3d_test_main(int argc, char **argv)

/*
 * Use assert_that() for conditions that should always be true.
 * todo_if() and bug_if() do not influence assert_that().
 */
#define assert_that assert_that_(__FILE__, __LINE__)

#define ok ok_(__FILE__, __LINE__)

#define skip skip_(__FILE__, __LINE__)

#define trace trace_(__FILE__, __LINE__)

#define assert_that_(file, line) \
        do { \
        const char *vkd3d_file = file; \
        unsigned int vkd3d_line = line; \
        VKD3D_TEST_ASSERT_THAT

#define VKD3D_TEST_ASSERT_THAT(...) \
        vkd3d_test_assert_that(vkd3d_file, vkd3d_line, __VA_ARGS__); } while (0)

#define ok_(file, line) \
        do { \
        const char *vkd3d_file = file; \
        unsigned int vkd3d_line = line; \
        VKD3D_TEST_OK

#define VKD3D_TEST_OK(...) \
        vkd3d_test_ok(vkd3d_file, vkd3d_line, __VA_ARGS__); } while (0)

#define todo_(file, line) \
        do { \
        const char *vkd3d_file = file; \
        unsigned int vkd3d_line = line; \
        VKD3D_TEST_TODO

#define VKD3D_TEST_TODO(...) \
        vkd3d_test_todo(vkd3d_file, vkd3d_line, __VA_ARGS__); } while (0)

#define skip_(file, line) \
        do { \
        const char *vkd3d_file = file; \
        unsigned int vkd3d_line = line; \
        VKD3D_TEST_SKIP

#define VKD3D_TEST_SKIP(...) \
        vkd3d_test_skip(vkd3d_file, vkd3d_line, __VA_ARGS__); } while (0)

#define trace_(file, line) \
        do { \
        const char *vkd3d_file = file; \
        unsigned int vkd3d_line = line; \
        VKD3D_TEST_TRACE

#define VKD3D_TEST_TRACE(...) \
        vkd3d_test_trace(vkd3d_file, vkd3d_line, __VA_ARGS__); } while (0)

#define todo_if(is_todo) \
    for (vkd3d_test_start_todo(is_todo); vkd3d_test_loop_todo(); vkd3d_test_end_todo())

#define bug_if(is_bug) \
    for (vkd3d_test_start_bug(is_bug); vkd3d_test_loop_bug(); vkd3d_test_end_bug())

#define todo todo_if(true)

struct vkd3d_test_state
{
    unsigned int success_count;
    unsigned int failure_count;
    unsigned int skip_count;
    unsigned int todo_count;
    unsigned int todo_success_count;
    unsigned int bug_count;

    unsigned int debug_level;

    unsigned int todo_level;
    bool todo_do_loop;

    unsigned int bug_level;
    bool bug_do_loop;
    bool bug_enabled;

    const char *test_name_filter;
    char context[8][128];
    unsigned int context_count;
};
extern struct vkd3d_test_state vkd3d_test_state;

static bool
vkd3d_test_platform_is_windows(void)
{
    return !strcmp(vkd3d_test_platform, "windows");
}

static inline bool
broken(bool condition)
{
    return condition && vkd3d_test_platform_is_windows();
}

/* basename() is not reentrant, basename_r() is not standard and this simple
 * implementation should be enough for us. */
static const char *vkd3d_basename(const char *filename)
{
    const char *p;

    if ((p = strrchr(filename, '/')))
        filename = ++p;
    if ((p = strrchr(filename, '\\')))
        filename = ++p;

    return filename;
}

static void vkd3d_test_printf(const char *file, unsigned int line, const char *msg)
{
    unsigned int i;

    printf("%s:%u: ", vkd3d_basename(file), line);
    for (i = 0; i < vkd3d_test_state.context_count; ++i)
        printf("%s: ", vkd3d_test_state.context[i]);
    printf("%s", msg);
}

static void
vkd3d_test_check_assert_that(const char *file, unsigned int line, bool result, const char *fmt, va_list args)
{
    if (result)
    {
        vkd3d_atomic_increment_u32(&vkd3d_test_state.success_count);
        if (vkd3d_test_state.debug_level > 1)
            vkd3d_test_printf(file, line, "Test succeeded.\n");
    }
    else
    {
        vkd3d_atomic_increment_u32(&vkd3d_test_state.failure_count);
        vkd3d_test_printf(file, line, "Test failed: ");
        vprintf(fmt, args);
    }
}

static void VKD3D_PRINTF_FUNC(4, 5) VKD3D_UNUSED
vkd3d_test_assert_that(const char *file, unsigned int line, bool result, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vkd3d_test_check_assert_that(file, line, result, fmt, args);
    va_end(args);
}

static void
vkd3d_test_check_ok(const char *file, unsigned int line, bool result, const char *fmt, va_list args)
{
    bool is_todo = vkd3d_test_state.todo_level && !vkd3d_test_platform_is_windows();
    bool is_bug = vkd3d_test_state.bug_level && !vkd3d_test_platform_is_windows();

    if (is_bug && vkd3d_test_state.bug_enabled)
    {
        vkd3d_atomic_increment_u32(&vkd3d_test_state.bug_count);
        if (is_todo)
            result = !result;
        if (result)
            vkd3d_test_printf(file, line, "Fixed bug: ");
        else
            vkd3d_test_printf(file, line, "Bug: ");
        vprintf(fmt, args);
    }
    else if (is_todo)
    {
        if (result)
        {
            vkd3d_atomic_increment_u32(&vkd3d_test_state.todo_success_count);
            vkd3d_test_printf(file, line, "Todo succeeded: ");
        }
        else
        {
            vkd3d_atomic_increment_u32(&vkd3d_test_state.todo_count);
            vkd3d_test_printf(file, line, "Todo: ");
        }
        vprintf(fmt, args);
    }
    else
    {
        vkd3d_test_check_assert_that(file, line, result, fmt, args);
    }
}

static void VKD3D_PRINTF_FUNC(4, 5) VKD3D_UNUSED
vkd3d_test_ok(const char *file, unsigned int line, bool result, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vkd3d_test_check_ok(file, line, result, fmt, args);
    va_end(args);
}

static void VKD3D_PRINTF_FUNC(3, 4) VKD3D_UNUSED
vkd3d_test_skip(const char *file, unsigned int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vkd3d_test_printf(file, line, "Test skipped: ");
    vprintf(fmt, args);
    va_end(args);
    vkd3d_atomic_increment_u32(&vkd3d_test_state.skip_count);
}

static void VKD3D_PRINTF_FUNC(3, 4) VKD3D_UNUSED
vkd3d_test_trace(const char *file, unsigned int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vkd3d_test_printf(file, line, "");
    vprintf(fmt, args);
    va_end(args);
}

static void VKD3D_PRINTF_FUNC(1, 2) VKD3D_UNUSED
vkd3d_test_debug(const char *fmt, ...)
{
    char buffer[512];
    va_list args;
    int size;

    size = snprintf(buffer, sizeof(buffer), "%s: ", vkd3d_test_name);
    if (0 < size && size < sizeof(buffer))
    {
        va_start(args, fmt);
        vsnprintf(buffer + size, sizeof(buffer) - size, fmt, args);
        va_end(args);
    }
    buffer[sizeof(buffer) - 1] = '\0';

#ifdef _WIN32
    OutputDebugStringA(buffer);
#endif

    if (vkd3d_test_state.debug_level > 0)
        printf("%s\n", buffer);
}

#ifndef VKD3D_TEST_NO_DEFS
const char *vkd3d_test_platform = "other";
struct vkd3d_test_state vkd3d_test_state;

static void vkd3d_test_main(int argc, char **argv);

int main(int argc, char **argv)
{
    const char *test_filter = getenv("VKD3D_TEST_FILTER");
    const char *debug_level = getenv("VKD3D_TEST_DEBUG");
    char *test_platform = getenv("VKD3D_TEST_PLATFORM");
    const char *bug = getenv("VKD3D_TEST_BUG");

    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&vkd3d_test_state, 0, sizeof(vkd3d_test_state));
    vkd3d_test_state.debug_level = debug_level ? atoi(debug_level) : 0;
    vkd3d_test_state.bug_enabled = bug ? atoi(bug) : true;
    vkd3d_test_state.test_name_filter = test_filter;

    if (test_platform)
    {
        test_platform = strdup(test_platform);
        vkd3d_test_platform = test_platform;
    }

    if (vkd3d_test_state.debug_level > 1)
        printf("Test platform: '%s'.\n", vkd3d_test_platform);

    vkd3d_test_main(argc, argv);

    printf("%s: %u tests executed (%u failures, %u skipped, %u todo, %u bugs).\n",
            vkd3d_test_name,
            vkd3d_test_state.success_count + vkd3d_test_state.failure_count
                    + vkd3d_test_state.todo_count + vkd3d_test_state.todo_success_count,
            vkd3d_test_state.failure_count + vkd3d_test_state.todo_success_count,
            vkd3d_test_state.skip_count,
            vkd3d_test_state.todo_count,
            vkd3d_test_state.bug_count);

    if (test_platform)
        free(test_platform);

    return vkd3d_test_state.failure_count || vkd3d_test_state.todo_success_count;
}

#ifdef _WIN32
static char *vkd3d_test_strdupWtoA(WCHAR *str)
{
    char *out;
    int len;

    if (!(len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL)))
        return NULL;
    if (!(out = malloc(len)))
        return NULL;
    WideCharToMultiByte(CP_ACP, 0, str, -1, out, len, NULL, NULL);

    return out;
}

static bool running_under_wine(void)
{
    HMODULE module = GetModuleHandleA("ntdll.dll");
    return module && GetProcAddress(module, "wine_server_call");
}

int wmain(int argc, WCHAR **wargv)
{
    char **argv;
    int i, ret;

    argv = malloc(argc * sizeof(*argv));
    assert(argv);
    for (i = 0; i < argc; ++i)
    {
        if (!(argv[i] = vkd3d_test_strdupWtoA(wargv[i])))
            break;
    }
    assert(i == argc);

    vkd3d_test_platform = running_under_wine() ? "wine" : "windows";

    ret = main(argc, argv);

    for (i = 0; i < argc; ++i)
        free(argv[i]);
    free(argv);

    return ret;
}
#endif  /* _WIN32 */
#endif /* VKD3D_TEST_NO_DEFS */

typedef void (*vkd3d_test_pfn)(void);

static inline void vkd3d_run_test(const char *name, vkd3d_test_pfn test_pfn)
{
    if (vkd3d_test_state.test_name_filter && !strstr(name, vkd3d_test_state.test_name_filter))
        return;

    vkd3d_test_debug("%s", name);
    test_pfn();
}

static inline void vkd3d_test_start_todo(bool is_todo)
{
    vkd3d_test_state.todo_level = (vkd3d_test_state.todo_level << 1) | is_todo;
    vkd3d_test_state.todo_do_loop = true;
}

static inline int vkd3d_test_loop_todo(void)
{
    bool do_loop = vkd3d_test_state.todo_do_loop;
    vkd3d_test_state.todo_do_loop = false;
    return do_loop;
}

static inline void vkd3d_test_end_todo(void)
{
    vkd3d_test_state.todo_level >>= 1;
}

static inline void vkd3d_test_start_bug(bool is_bug)
{
    vkd3d_test_state.bug_level = (vkd3d_test_state.bug_level << 1) | is_bug;
    vkd3d_test_state.bug_do_loop = true;
}

static inline int vkd3d_test_loop_bug(void)
{
    bool do_loop = vkd3d_test_state.bug_do_loop;
    vkd3d_test_state.bug_do_loop = false;
    return do_loop;
}

static inline void vkd3d_test_end_bug(void)
{
    vkd3d_test_state.bug_level >>= 1;
}

static inline void vkd3d_test_push_context(const char *fmt, ...)
{
    va_list args;

    if (vkd3d_test_state.context_count < ARRAY_SIZE(vkd3d_test_state.context))
    {
        va_start(args, fmt);
        vsnprintf(vkd3d_test_state.context[vkd3d_test_state.context_count],
                sizeof(*vkd3d_test_state.context), fmt, args);
        va_end(args);
        vkd3d_test_state.context[vkd3d_test_state.context_count][sizeof(vkd3d_test_state.context[0]) - 1] = '\0';
    }
    ++vkd3d_test_state.context_count;
}

static inline void vkd3d_test_pop_context(void)
{
    if (vkd3d_test_state.context_count)
        --vkd3d_test_state.context_count;
}

#define run_test(test_pfn) \
        vkd3d_run_test(#test_pfn, test_pfn)

#endif  /* __VKD3D_TEST_H */
