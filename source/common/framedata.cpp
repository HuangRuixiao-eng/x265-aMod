/*****************************************************************************
* Copyright (C) 2013 x265 project
*
* Author: Steve Borho <steve@borho.org>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
*
* This program is also available under a commercial proprietary license.
* For more information, contact us at license @ x265.com.
*****************************************************************************/

#include "framedata.h"
#include "picyuv.h"

using namespace x265;

FrameData::FrameData()
{
    memset(this, 0, sizeof(*this));
}

bool FrameData::create(x265_param *param, const SPS& sps)
{
    uint32_t i;

    m_numPartitions   = 1 << (g_maxFullDepth * 2);
    m_numPartInCUSize = 1 << g_maxFullDepth;
    m_param = param;

    m_slice  = new Slice;
    m_picCTU = new CUData[sps.numCUsInFrame];

    uint32_t sizeL = 1 << (g_maxLog2CUSize * 2);
    uint32_t sizeC = sizeL >> (CHROMA_H_SHIFT(param->internalCsp) + CHROMA_V_SHIFT(param->internalCsp));

    m_cuMemPool.create(m_numPartitions, sizeL, sizeC, sps.numCUsInFrame);
    for (i = 0; i < sps.numCUsInFrame; i++)
        m_picCTU[i].initialize(m_cuMemPool, m_numPartitions, g_maxCUSize, param->internalCsp, i);

    CHECKED_MALLOC(m_cuStat, RCStatCU, sps.numCUsInFrame);
    CHECKED_MALLOC(m_rowStat, RCStatRow, sps.numCuInHeight);

    reinit(param, sps);
    return true;

fail:
    return false;
}

void FrameData::reinit(x265_param *param, const SPS& sps)
{
    memset(m_cuStat, 0, sps.numCUsInFrame * sizeof(*m_cuStat));
    memset(m_rowStat, 0, sps.numCuInHeight * sizeof(*m_rowStat));
}

void FrameData::destroy()
{
    delete [] m_picCTU;
    delete m_slice;
    delete m_saoParam;

    m_cuMemPool.destroy();

    X265_FREE(m_cuStat);
    X265_FREE(m_rowStat);
}
