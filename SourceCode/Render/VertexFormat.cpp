﻿#include "stdafx.h"
#include "VertexFormat.h"
#include "Renderer.h"
#include "external/Configuration.h"

CVertexFormat::CVertexFormat()
    : m_size(0)
{

}

CVertexFormat::~CVertexFormat()
{

}

void CVertexFormat::AddAttrib(const SAttrib &attrib)
{
    m_attribs.emplace(attrib.index, attrib);
    BEATS_ASSERT(CConfiguration::GetInstance()->GetMaxVertexAttributes() == 0 || 
        CConfiguration::GetInstance()->GetMaxVertexAttributes() > (int)m_attribs.size(),
        _T("vertex attributes overflow! max vertex attributes %d"), CConfiguration::GetInstance()->GetMaxVertexAttributes());
}

bool CVertexFormat::HasAttrib(GLuint index) const
{
    return m_attribs.find(index) != m_attribs.end();
}

const CVertexFormat::SAttrib &CVertexFormat::GetAttrib(GLuint index) const
{
    auto itr = m_attribs.find(index);
    BEATS_ASSERT(itr != m_attribs.end(), _T("There's no attribute for index %d"), index);
    return itr->second;
}

uint32_t CVertexFormat::AttribCount() const
{
    return (uint32_t)m_attribs.size();
}

uint32_t CVertexFormat::Size() const
{
    return m_size;
}

void CVertexFormat::SetSize(uint32_t size)
{
    BEATS_ASSERT(size <= MAX_VERTEX_SIZE, _T("Adjust max vertex size, please!"));
    m_size = size;
}

void CVertexFormat::SetupAttribPointer(GLuint vbo) const
{
    BEATS_ASSERT(vbo > 0);
    CRenderer *pRenderer = CRenderer::GetInstance();
    pRenderer->BindBuffer(GL_ARRAY_BUFFER, vbo);

    for(auto attribPair : m_attribs)
    {
        const SAttrib &attrib = attribPair.second;
        pRenderer->EnableVertexAttribArray(attrib.index);
        if(attrib.common.type == GL_INT)
        {
            pRenderer->VertexAttribIPointer(attrib.index, attrib.common.size, attrib.common.type,
                attrib.stride, (const GLvoid *)attrib.offset);
        }
        else
        {
            pRenderer->VertexAttribPointer(attrib.index, attrib.common.size, attrib.common.type,
                attrib.common.normalized, attrib.stride, (const GLvoid *)attrib.offset);
        }
    }
}

void CVertexFormat::DisableAttribPointer() const
{
    CRenderer *pRenderer = CRenderer::GetInstance();
    for(auto attribPair : m_attribs)
    {
        const SAttrib &attrib = attribPair.second;
        pRenderer->DisableVertexAttribArray(attrib.index);
    }
}

bool CVertexFormat::operator==(const CVertexFormat &another) const
{
    return this == &another;
}

bool CVertexFormat::operator!=(const CVertexFormat &another) const
{
    return !(*this == another);
}

bool CVertexFormat::Compatible(const CVertexFormat &another) const
{
    bool bCompatible = true;
    if(*this != another)
    {
        if ( m_attribs.size() == another.m_attribs.size() )
        {
            for(auto attribPair : another.m_attribs)
            {
                if(!HasAttrib(attribPair.second.index))
                {
                    bCompatible = false;
                    break;
                }
            }
        }
        else
        {
            bCompatible = false;
        }
    }
    return bCompatible;
}
