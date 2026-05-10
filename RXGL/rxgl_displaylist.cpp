/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_displaylist.cpp  --  Display list record and playback implementation.
 *                           Records GL calls made between glNewList/glEndList into
 *                           a dynamic command array, then replays them on glCallList.
 *                           Supports nested calls via glCallLists and glListBase.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_displaylist.h"

// ---------------------------------------------------------------------------
// RXGLDisplayList
// ---------------------------------------------------------------------------

void RXGLDisplayList::Clear()
{
    delete[] cmds;
    cmds     = NULL;
    count    = 0;
    capacity = 0;
    valid    = false;
}

void RXGLDisplayList::Add(const RXGLDisplayListCmd& c)
{
    if (count >= capacity)
    {
        int newCap = capacity ? capacity * 2 : 64;
        RXGLDisplayListCmd* n = new RXGLDisplayListCmd[newCap];
        for (int i = 0; i < count; i++) n[i] = cmds[i];
        delete[] cmds;
        cmds     = n;
        capacity = newCap;
    }
    cmds[count++] = c;
}

// ---------------------------------------------------------------------------
// RXGLDisplayListManager
// ---------------------------------------------------------------------------

RXGLDisplayListManager::RXGLDisplayListManager()
    : m_nextID(1), m_compiling(false), m_executing(false),
      m_currentList(0), m_mode(GL_COMPILE)
{
}

void RXGLDisplayListManager::Init()
{
    m_nextID      = 1;
    m_compiling   = false;
    m_executing   = false;
    m_currentList = 0;
    m_mode        = GL_COMPILE;
}

void RXGLDisplayListManager::Record(const RXGLDisplayListCmd& c)
{
    if (!m_compiling || m_executing) return;
    if (m_currentList == 0 || m_currentList >= RXGL_MAX_DISPLAY_LISTS) return;
    m_lists[m_currentList].Add(c);
}

GLuint RXGLDisplayListManager::GenLists(GLsizei range)
{
    if (range <= 0) return 0;

    // First: scan for a contiguous range of explicitly freed slots.
    // A slot is only eligible for recycling if DeleteLists cleared it AND
    // it was previously compiled (had valid content at some point).
    // Slots allocated by GenLists but never compiled are NOT recycled --
    // they stay reserved until DeleteLists is explicitly called on them.
    // We track this by marking slots valid=true at GenLists time.
    GLuint run = 0, runStart = 0;
    for (GLuint id = 1; id < m_nextID; id++)
    {
        if (!m_lists[id].valid)
        {
            if (run == 0) runStart = id;
            run++;
            if (run == (GLuint)range)
            {
                // Mark recycled slots as reserved so they cannot be double-allocated
                for (GLuint i = 0; i < (GLuint)range; i++) m_lists[runStart + i].valid = true;
                return runStart;
            }
        }
        else
        {
            run = 0;
        }
    }

    // No free range found -- allocate from the top
    if (m_nextID + (GLuint)range > RXGL_MAX_DISPLAY_LISTS) return 0;
    GLuint base = m_nextID;
    // Mark all allocated slots as reserved immediately so they cannot
    // be recycled by a subsequent GenLists call before NewList is called.
    for (GLuint i = 0; i < (GLuint)range; i++) m_lists[base + i].valid = true;
    m_nextID += (GLuint)range;
    return base;
}

void RXGLDisplayListManager::NewList(GLuint list, GLenum mode)
{
    if (list == 0 || list >= RXGL_MAX_DISPLAY_LISTS) return;
    if (mode != GL_COMPILE && mode != GL_COMPILE_AND_EXECUTE) return;

    m_lists[list].Clear();
    m_lists[list].valid = true;

    m_compiling   = true;
    m_executing   = false;
    m_currentList = list;
    m_mode        = mode;
}

void RXGLDisplayListManager::EndList()
{
    m_compiling   = false;
    m_executing   = false;
    m_currentList = 0;
    m_mode        = GL_COMPILE;
}

void RXGLDisplayListManager::DeleteLists(GLuint list, GLsizei range)
{
    if (range <= 0) return;
    for (GLsizei i = 0; i < range; i++)
    {
        GLuint id = list + (GLuint)i;
        if (id > 0 && id < RXGL_MAX_DISPLAY_LISTS)
            m_lists[id].Clear();
    }
}

void RXGLDisplayListManager::CallList(GLuint list, DispatchFn dispatch, void* ctx)
{
    if (list == 0 || list >= RXGL_MAX_DISPLAY_LISTS) return;
    RXGLDisplayList& dl = m_lists[list];
    if (!dl.valid) return;

    bool oldExecuting = m_executing;
    m_executing = true;

    for (int i = 0; i < dl.count; i++)
        dispatch(dl.cmds[i], ctx);

    m_executing = oldExecuting;
}
