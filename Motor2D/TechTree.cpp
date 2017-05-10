#include "TechTree.h"

void TechTree::Start(pugi::xml_node gameData){

	for (int i = 0; i < MAX_MULTIPLIERS; i++)
		multiplier_list.push_back(1);

	LoadTechTree(gameData);

	available_buildings.push_back(HOUSE);
	available_buildings.push_back(ORC_BARRACKS);
	available_buildings.push_back(ARCHERY_RANGE);
	available_buildings.push_back(STABLES);
	available_buildings.push_back(MILL);

	available_techs.push_back(RANGED_WEAPONS);
	available_techs.push_back(HORSE_TRAINING);
	available_techs.push_back(TOWN_MILITIA);

	pair<unitType, buildingType> p;

	p = { GONDOR_SPEARMAN, ORC_BARRACKS };
	available_units.push_back(p);

	p = { DUNEDAIN_RANGE, ARCHERY_RANGE };
	available_units.push_back(p);

	p = { GONDOR_KNIGHT, STABLES };
	available_units.push_back(p);

	p = { LIGHT_CATAPULT, SIEGE_WORKSHOP };
	available_units.push_back(p);


}


void TechTree::Update(){

	for (vector<Tech*>::iterator it = all_techs.begin(); it != all_techs.end(); it++) {
		if ((*it)->research_timer.ReadSec() > (*it)->research_time)
			Researched((*it)->id);
	}

}


void TechTree::StartResearch(TechType tech_id)
{
	all_techs.at(tech_id)->research_timer.Start();
}


void TechTree::Researched(TechType tech_id) {

	Tech* tech = all_techs.at(tech_id);

	if (tech->unlocks_tech.empty()) {
		for(list<TechType>::iterator it = tech->unlocks_tech.begin(); it != tech->unlocks_tech.end(); it++)
			available_techs.push_back(*it);
	}
	
	if (tech->unlocks_unit.first != -1)
		available_units.push_back(tech->unlocks_unit);

	if (tech->unlocks_building != -1)
		available_buildings.push_back(tech->unlocks_building);

	if (tech->multipliers.first != -1)
		multiplier_list[tech->unlocks_unit.second] += tech->multipliers.first;
	
	available_techs.remove(tech_id);
}


void TechTree::LoadTechTree(pugi::xml_node Techs) {

	pugi::xml_node TechData;
	int id_count = 0;

	if (Techs.empty() == false)
	{

		for (TechData = Techs.child("Tech"); TechData; TechData = TechData.next_sibling("Tech")) {

			Tech* new_Tech = new Tech();

			for (pugi::xml_node unlocked_techs = TechData.child("Unlockedtech"); unlocked_techs; unlocked_techs = TechData.next_sibling("Unlockedtech"))
				new_Tech->unlocks_tech.push_back((TechType)unlocked_techs.attribute("value").as_int());

			new_Tech->unlocks_building = (buildingType)TechData.child("Unlockedbuilding").attribute("value").as_int();

			new_Tech->unlocks_unit.first = (unitType)TechData.child("Unlockedunit").attribute("unit_type").as_int();
			new_Tech->unlocks_unit.second = (buildingType)TechData.child("Unlockedunit").attribute("building").as_int();

			new_Tech->multipliers.first = TechData.child("Multiplier").attribute("value").as_float();
			new_Tech->multipliers.second = (TechMultiplier)TechData.child("Multiplier").attribute("multiplier").as_int();

			new_Tech->researched_in = (buildingType)TechData.child("Researched_in").attribute("value").as_int();
			new_Tech->name = TechData.child("Name").attribute("value").as_string();
			new_Tech->desc = TechData.child("Desc").attribute("value").as_string();
			new_Tech->id = (TechType)id_count;

			new_Tech->cost.food = TechData.child("Cost").attribute("foodCost").as_int();
			new_Tech->cost.stone = TechData.child("Cost").attribute("stoneCost").as_int();
			new_Tech->cost.wood = TechData.child("Cost").attribute("woodCost").as_int();
			new_Tech->cost.gold = TechData.child("Cost").attribute("goldCost").as_int();
			
			new_Tech->research_time = TechData.child("Research_time").attribute("value").as_int();

			all_techs.push_back(new_Tech);
			id_count++;
		}
	}
}