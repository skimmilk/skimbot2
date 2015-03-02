/*
 * trace.cpp
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#include "trace.h"

#include "tfplayer.h"
#include "ifs.h"
#include "sdk/valvetrace.h"
#include "sdk/math.h"

namespace skim
{

// Filter that hits only players
class playerfilter : public ITraceFilter
{
public:
    playerfilter(int indexIgnore) : ignore(indexIgnore) {}

    bool ShouldHitEntity(IHandleEntity* ent, int)
    {
    	auto clientent = ifs::entities->GetClientEntityFromHandle(ent->GetRefEHandle());
        tfplayer *player = (tfplayer*)clientent->GetBaseEntity();
        return player->m_iTeamNum() > 1 && clientent->entindex() != ignore;
    }
    virtual TraceType_t GetTraceType() const
    {
        return TRACE_EVERYTHING;
    }

private:
    int ignore;
};

tfplayer* trace::sight(const Vector& start, QAngle viewangle, int idignore)
{
	Vector direction = AngleVectors(viewangle);

	playerfilter* filter = new playerfilter(idignore);

	Ray_t ray;
	Vector end {
		start.x + direction.x * 8192,
		start.y + direction.y * 8192,
		start.z + direction.z * 8192
	};
	ray.Init(start, end);

	trace_t tr;
	ifs::tracer->TraceRay(ray, MASK_SHOT, filter, &tr);

	return (tfplayer*)tr.m_pEnt;
}

} /* namespace skim */
