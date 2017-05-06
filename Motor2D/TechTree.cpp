#include "TechTree.h"

void TechTree::Start(pugi::xml_node gameData){

	for (int i = 0; i < MAX_MULTIPLIERS; i++)
		multiplier_list.at(i) = 1;

	LoadTechTree(gameData);
	Researched(BASIC_TECH);
}


void TechTree::Update(){

	for (vector<Tech*>::iterator it = all_techs.begin(); it != all_techs.end(); it++) {
		if ((*it)->research_timer.ReadSec() > (*it)->research_time)
			Researched((*it)->id);
	}

}


void TechTree::StartResearch(int tech_id) 
{
	all_techs.at(tech_id)->research_timer.Start();
}


void TechTree::Researched(int tech_id) {

	Tech* tech = all_techs.at(tech_id);

	if (!tech->unlocks_techs.empty()) {
		for (list<pair<int, buildingType>>::iterator it = tech->unlocks_techs.begin(); it != tech->unlocks_techs.end(); it++)
			available_techs[(*it).second].push_back((*it).first);
	}

	if (!tech->unlocks_units.empty()) {
		for (list<pair<unitType, buildingType>>::iterator it2 = tech->unlocks_units.begin(); it2 != tech->unlocks_units.end(); it2++)
			available_units[(*it2).second].push_back((*it2).first);
	}

	if (!tech->unlocks_buildings.empty()) {
		for (list<buildingType>::iterator it3 = tech->unlocks_buildings.begin(); it3 != tech->unlocks_buildings.end(); it3++)
			available_buildings.push_back((*it3));
	}

	if (!tech->multipliers.empty()) {
		for (list<pair<float, TechMultiplier>>::iterator it4 = tech->multipliers.begin(); it4 != tech->multipliers.end(); it4++)
			multiplier_list[(*it4).second] += (*it4).first;
	}

	available_techs.at(tech->researched_in).remove(tech->id);
}


void TechTree::LoadTechTree(pugi::xml_node gameData) {

	pugi::xml_node TechData;
	int id_count = 0;

	if (gameData.empty() == false)
	{

		for (TechData = gameData.child("Techs").child("Tech"); TechData; TechData = TechData.next_sibling("Tech")) {

			Tech* new_Tech = new Tech();

			pugi::xml_node unlocked_tech;
			for (unlocked_tech = TechData.child("Unl_techs").child("Unl_tech"); unlocked_tech; unlocked_tech = unlocked_tech.next_sibling("Unl_tech")) {

				pair<int, buildingType> unl_tech_data;
				unl_tech_data.first = unlocked_tech.attribute("tech_id").as_int();
				unl_tech_data.second = (buildingType)unlocked_tech.attribute("building").as_int();

				new_Tech->unlocks_techs.push_back(unl_tech_data);

			}

			pugi::xml_node unlocked_unit;
			for (unlocked_unit = TechData.child("Unl_units").child("Unl_unit"); unlocked_unit; unlocked_unit = unlocked_unit.next_sibling("Unl_unit")) {

				pair<unitType, buildingType> unl_unit_data;
				unl_unit_data.first = (unitType)unlocked_tech.attribute("unit_type").as_int();
				unl_unit_data.second = (buildingType)unlocked_tech.attribute("building").as_int();

				new_Tech->unlocks_units.push_back(unl_unit_data);

			}

			pugi::xml_node mult_data;
			for (mult_data = TechData.child("Multipliers").child("Multiplier"); mult_data; mult_data = mult_data.next_sibling("Multiplier")) {

				pair<float, TechMultiplier> multiplier_data;
				multiplier_data.first = (unitType)unlocked_tech.attribute("value").as_float();
				multiplier_data.second = (TechMultiplier)unlocked_tech.attribute("multiplier").as_int();

				new_Tech->multipliers.push_back(multiplier_data);

			}

			pugi::xml_node unlocked_buidling;
			for (unlocked_buidling = TechData.child("Unl_buildings").child("Unl_building"); unlocked_buidling; unlocked_buidling = unlocked_buidling.next_sibling("Unl_building"))
				new_Tech->unlocks_buildings.push_back((buildingType)unlocked_buidling.attribute("value").as_int());


			new_Tech->researched_in = (buildingType)TechData.child("Researched_in").attribute("value").as_int();
			new_Tech->name = TechData.child("Name").attribute("value").as_string();
			new_Tech->desc = TechData.child("Vesc").attribute("value").as_string();
			new_Tech->id = id_count;

			new_Tech->cost.foodCost = TechData.child("Cost").child("foodCost").attribute("value").as_int();
			new_Tech->cost.stoneCost = TechData.child("Cost").child("stoneCost").attribute("value").as_int();
			new_Tech->cost.woodCost = TechData.child("Cost").child("woodCost").attribute("value").as_int();
			new_Tech->cost.goldCost = TechData.child("Cost").child("goldCost").attribute("value").as_int();
			
			new_Tech->research_time = TechData.child("Reseach_time").attribute("value").as_int();

			all_techs.push_back(new_Tech);
			id_count++;
		}
	}
}