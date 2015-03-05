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
class player_filter : public ITraceFilter
{
public:
	player_filter(int indexIgnore) : ignore(indexIgnore) {}

	bool ShouldHitEntity(IHandleEntity* ent, int) override
	{
		CBaseHandle asdf = ent->GetRefEHandle();

		// For some reason GetClientEntityFromHandle crashes
		// Manually get its entity index
		tfplayer* player = (tfplayer*)ifs::entities->GetClientEntity(asdf & 0x0fff)->GetBaseEntity();
		return player->entindex() != ignore;
	}
	TraceType_t GetTraceType() const override
	{
		return TRACE_EVERYTHING;
	}

private:
	int ignore;
};

tfplayer* trace::sight(const Vector& start, const QAngle& viewangle, int idignore)
{
	Vector direction = AngleVectors(viewangle);

	player_filter* filter = new player_filter(idignore);

	Ray_t ray;
	Vector end {
		start.x + direction.x * 8192,
		start.y + direction.y * 8192,
		start.z + direction.z * 8192
	};
	ray.Init(start, end);

	trace_t tr;
	ifs::tracer->TraceRay(ray, MASK_SHOT, filter, &tr);

	if (!tr.m_pEnt)
		return 0;

	// Did it hit the world?
	if (tr.m_pEnt == (IClientEntity*)ifs::entities->GetClientEntity(0)->GetBaseEntity())
		return 0;

	// Did it hit a player?
	tfplayer* player = (tfplayer*)tr.m_pEnt;
	if (player->entindex() < 0 || player->entindex() > 32)
		return 0;

	return player;
}

} /* namespace skim */
