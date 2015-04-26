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

	bool ShouldHitEntity(IHandleEntity* handle, int) override
	{
		CBaseHandle asdf = handle->GetRefEHandle();

		// For some reason GetClientEntityFromHandle crashes
		// Manually get its entity index
		tfentity* ent = (tfentity*)ifs::entities->GetClientEntity(asdf & 0x0fff)->GetBaseEntity();
		return ent->entindex() != ignore &&
				ent->GetClientClass()->m_ClassID != netvar::classid<_id>("CFuncRespawnRoomVisualizer") &&
				ent->GetClientClass()->m_ClassID != netvar::classid<_id>("CFuncAreaPortalWindow") &&
				ent->GetClientClass()->m_ClassID != netvar::classid<_id>("CTFMedigunShield");
	}
	TraceType_t GetTraceType() const override
	{
		return TRACE_EVERYTHING;
	}

	int ignore;
};

IClientEntity* trace::ray(const Vector& start, const QAngle& aim, int ignore)
{
	Vector direction = AngleVectors(aim);

	static player_filter* filter;
	if (!filter)
		filter = new player_filter(ignore);
	filter->ignore = ignore;

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
tfentity* trace::sight(const Vector& start, const QAngle& viewangle, int idignore)
{
	tfentity* aim = (tfentity*)ray(start, viewangle, idignore);
	return aim;
}

bool trace::can_see_fast(const Vector& start, tfentity* pl)
{
	Vector middle = pl->GetAbsOrigin();
	middle.y += 32;

	return can_see_fast(start, middle, pl);
}

bool trace::can_see_fast(const Vector& start, const Vector& end, tfentity* filter)
{
	Vector difference = end - start;
	QAngle aim;
	VectorAngles(difference, aim);
	auto hit = ray(start, aim, ifs::engine->GetLocalPlayer());

	return (tfplayer*)hit == filter;
}

} /* namespace skim */
