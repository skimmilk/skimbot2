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
#include "sdk/engine.h"

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

IClientEntity* trace::ray(const Vector& start, const QAngle& aim, int ignore)
{
	Vector direction = AngleVectors(aim);

	player_filter* filter = new player_filter(ignore);

	Ray_t ray;
	Vector end {
		start.x + direction.x * 8192,
		start.y + direction.y * 8192,
		start.z + direction.z * 8192
	};
	ray.Init(start, end);

	trace_t tr;
	ifs::tracer->TraceRay(ray, MASK_SHOT, filter, &tr);

	return tr.m_pEnt;
}

bool trace::can_see_fast(const Vector& start, tfplayer* pl)
{
	Vector middle = pl->GetAbsOrigin();
	middle.y += 32;

	Vector difference = middle - start;
	QAngle aim;
	VectorAngles(difference, aim);
	auto hit = ray(start, aim, ifs::engine->GetLocalPlayer());

	return (tfplayer*)hit == pl;
}

} /* namespace skim */
