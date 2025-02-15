/*
 * skeltal.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "skeltal.h"

#include "ifs.h"
#include "sdk/model.h"

namespace skim
{

static tfhitbox correct_hbox(tfplayer* player, tfhitbox hbox)
{
	tfclass cls = (tfclass)player->m_iClass();
	tfhitbox hb = hbox;

    if (cls != tfclass::scout && hb == tfhitbox::HITBOX_SCOUT_PACKMIDDLE)
    	return tfhitbox::HITBOX_SPINE_2;
    if (cls != tfclass::engineer) return hbox;

    // Engineer is missing a hitbox, correct appropriately
    if (hb == tfhitbox::HITBOX_HEAD) return hbox;
    return (tfhitbox)((int)hbox - 1);
}

void skeltal::bbox(tfplayer* player, tfhitbox hitbox, Vector& min, Vector& max)
{
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
	mstudiobbox_t* box = studio->pHitbox((int)hitbox, 0);

	VectorTransform(box->bbmin, bones[box->bone], min);
	VectorTransform(box->bbmax, bones[box->bone], max);

}
Vector skeltal::pos(tfplayer* player, tfhitbox hitbox, extents ext)
{
	if ((int)hitbox < 0)
		return player->GetAbsOrigin();

	Vector min, max;
	skeltal::bbox(player, hitbox, min, max);

	if (ext == extents::middle)
		return (min + max) * .5;
	if (ext == extents::min)
		return min;
	return max;
}

} /* namespace skim */
