/*
 * skeltal.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "skeltal.h"

#include "sdk/model.h"

namespace skim
{

static int correct_hbox(tfplayer* player, int hbox)
{
	tfclass cls = (tfclass)player->m_iClass();
	tfhitbox hb = (tfhitbox)hbox;

    if (cls != tfclass::scout && hb == tfhitbox::HITBOX_SCOUT_PACKMIDDLE)
    	return (int)tfhitbox::HITBOX_SPINE_2;
    if (cls != tfclass::engineer) return hbox;

    // Engineer is missing a hitbox, correct appropriately
    if (hb == tfhitbox::HITBOX_HEAD) return hbox;
    return hbox - 1;
}

Vector skeltal::pos(tfplayer* player, int hitbox, float extent)
{
	if (hitbox < 0)
		return player->GetAbsOrigin();

	hitbox = correct_hbox(player, hitbox);
	matrix3x4_t bones[MAXSTUDIOBONES];
	player->SetupBones(bones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0);

	// If the player is disguised, get the spy's hitboxes instead of his disguise
	const model_t* model;
	if (player->m_nDisguiseClass())
	{
		int ispy = ifs::model_info->GetModelIndex("models/player/spy.mdl");
		model = ifs::model_info->GetModel(ispy);
	}
	else
		model = player->GetModel();

	studiohdr_t* studio = ifs::model_info->GetStudiomodel(model);
	mstudiobbox_t* box = studio->pHitbox(hitbox, 0);

	Vector min, max;
	VectorTransform(box->bbmin, bones[box->bone], min);
	VectorTransform(box->bbmax, bones[box->bone], max);

	return (min * extent) + (max * (1.f - extent));
}

} /* namespace skim */
